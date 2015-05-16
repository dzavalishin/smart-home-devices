/*
 * Copyright (C) 2003-2006 by egnite Software GmbH.
 * Copyright (C) 2008 by egnite GmbH.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * For additional information see http://www.ethernut.de/
 *
 */

/*
 * $Id: lan91.c 3376 2011-04-26 14:11:50Z haraldkipp $
 */

#include <string.h>

#include <cfg/os.h>
#include <sys/atom.h>
#include <sys/heap.h>
#include <sys/thread.h>
#include <sys/event.h>
#include <sys/timer.h>
#include <sys/confnet.h>

#include <netinet/if_ether.h>
#include <net/ether.h>
#include <net/if.h>
#include <net/if_var.h>

#include <cfg/arch/gpio.h>
#include <dev/irqreg.h>
#include <dev/lan91.h>

#ifdef NUTDEBUG
#include <stdio.h>
#endif

/*
 * Determine interrupt settings.
 */
#if (LAN91_SIGNAL_IRQ == INT0)
#define LAN91_SIGNAL          sig_INTERRUPT0
#elif (LAN91_SIGNAL_IRQ == INT1)
#define LAN91_SIGNAL          sig_INTERRUPT1
#elif (LAN91_SIGNAL_IRQ == INT2)
#define LAN91_SIGNAL          sig_INTERRUPT2
#elif (LAN91_SIGNAL_IRQ == INT3)
#define LAN91_SIGNAL          sig_INTERRUPT3
#elif (LAN91_SIGNAL_IRQ == INT4)
#define LAN91_SIGNAL          sig_INTERRUPT4
#elif (LAN91_SIGNAL_IRQ == INT5)
#define LAN91_SIGNAL          sig_INTERRUPT5
#elif (LAN91_SIGNAL_IRQ == INT6)
#define LAN91_SIGNAL          sig_INTERRUPT6
#elif (LAN91_SIGNAL_IRQ == INT7)
#define LAN91_SIGNAL          sig_INTERRUPT7
#endif

/*
 * Determine poll timers.
 */
#if !defined(LAN91_RX_POLLTIME)
#if defined(LAN91_SIGNAL)
#define LAN91_RX_POLLTIME   2000
#else
#define LAN91_RX_POLLTIME   200
#endif
#endif

#if !defined(LAN91_TX_POLLTIME)
#if defined(LAN91_SIGNAL)
#define LAN91_TX_POLLTIME   5000
#else
#define LAN91_TX_POLLTIME   200
#endif
#endif


/*!
 * \addtogroup xgNicLan91
 */
/*@{*/

#define nic_outlb(addr, val) (*(volatile uint8_t *)(addr) = (val))
#define nic_outhb(addr, val) (*(volatile uint8_t *)((addr) + 1) = (val))
#define nic_outwx(addr, val) (*(volatile uint16_t *)(addr) = (val))
#define nic_outw(addr, val) { \
    *(volatile uint8_t *)(addr) = (uint8_t)(val); \
    *((volatile uint8_t *)(addr) + 1) = (uint8_t)((val) >> 8); \
}

#define nic_inlb(addr) (*(volatile uint8_t *)(addr))
#define nic_inhb(addr) (*(volatile uint8_t *)((addr) + 1))
#define nic_inw(addr) (*(volatile uint16_t *)(addr))

#define nic_bs(bank)    nic_outlb(LAN91_BSR, bank)

/*!
 * \struct _NICINFO
 * \brief Network interface controller information structure.
 */
struct _NICINFO {
    HANDLE volatile ni_rx_rdy;      /*!< Receiver event queue. */
    uint16_t ni_tx_cnt;             /*!< Number of bytes in transmission queue. */
#ifdef NUT_PERFMON
    uint32_t ni_rx_packets;         /*!< Number of packets received. */
    uint32_t ni_tx_packets;         /*!< Number of packets sent. */
    uint32_t ni_interrupts;         /*!< Number of interrupts. */
    uint32_t ni_overruns;           /*!< Number of packet overruns. */
    uint32_t ni_rx_frame_errors;    /*!< Number of frame errors. */
    uint32_t ni_rx_crc_errors;      /*!< Number of CRC errors. */
    uint32_t ni_rx_missed_errors;   /*!< Number of missed packets. */
#endif
};

/*!
 * \brief Network interface controller information type.
 */
typedef struct _NICINFO NICINFO;

static HANDLE mutex;
static HANDLE maq;

/*!
 * \brief Select specified PHY register for reading or writing.
 *
 * \note NIC interrupts must have been disabled before calling this routine.
 *
 * \param reg PHY register number.
 * \param we  Indicates type of access, 1 for write and 0 for read.
 *
 * \return Contents of the PHY interface rgister.
 */
static uint8_t NicPhyRegSelect(uint8_t reg, uint8_t we)
{
    uint8_t rs;
    uint8_t msk;
    uint_fast8_t i;

    nic_bs(3);
    rs = (nic_inlb(LAN91_MGMT) & ~(LAN91_MGMT_MCLK | LAN91_MGMT_MDO)) | LAN91_MGMT_MDOE;

    /* Send idle pattern. */
    for (i = 0; i < 33; i++) {
        nic_outlb(LAN91_MGMT, rs | LAN91_MGMT_MDO);
        nic_outlb(LAN91_MGMT, rs | LAN91_MGMT_MDO | LAN91_MGMT_MCLK);
    }

    /* Send start sequence. */
    nic_outlb(LAN91_MGMT, rs);
    nic_outlb(LAN91_MGMT, rs | LAN91_MGMT_MCLK);
    nic_outlb(LAN91_MGMT, rs | LAN91_MGMT_MDO);
    nic_outlb(LAN91_MGMT, rs | LAN91_MGMT_MDO | LAN91_MGMT_MCLK);

    /* Write or read mode. */
    if (we) {
        nic_outlb(LAN91_MGMT, rs);
        nic_outlb(LAN91_MGMT, rs | LAN91_MGMT_MCLK);
        nic_outlb(LAN91_MGMT, rs | LAN91_MGMT_MDO);
        nic_outlb(LAN91_MGMT, rs | LAN91_MGMT_MDO | LAN91_MGMT_MCLK);
    } else {
        nic_outlb(LAN91_MGMT, rs | LAN91_MGMT_MDO);
        nic_outlb(LAN91_MGMT, rs | LAN91_MGMT_MDO | LAN91_MGMT_MCLK);
        nic_outlb(LAN91_MGMT, rs);
        nic_outlb(LAN91_MGMT, rs | LAN91_MGMT_MCLK);
    }

    /* Send PHY address. Zero is used for the internal PHY. */
    for (i = 0; i < 5; i++) {
        nic_outlb(LAN91_MGMT, rs);
        nic_outlb(LAN91_MGMT, rs | LAN91_MGMT_MCLK);
    }

    /* Send PHY register number. */
    for (msk = 0x10; msk; msk >>= 1) {
        if (reg & msk) {
            nic_outlb(LAN91_MGMT, rs | LAN91_MGMT_MDO);
            nic_outlb(LAN91_MGMT, rs | LAN91_MGMT_MDO | LAN91_MGMT_MCLK);
        } else {
            nic_outlb(LAN91_MGMT, rs);
            nic_outlb(LAN91_MGMT, rs | LAN91_MGMT_MCLK);
        }
    }
    nic_outlb(LAN91_MGMT, rs);

    return rs;
}

/*!
 * \brief Read contents of PHY register.
 *
 * \note NIC interrupts must have been disabled before calling this routine.
 *
 * \param reg PHY register number.
 *
 * \return Contents of the specified register.
 */
static uint16_t NicPhyRead(uint8_t reg)
{
    uint16_t rc = 0;
    uint8_t rs;
    uint_fast8_t i;

    /* Select register for reading. */
    rs = NicPhyRegSelect(reg, 0);

    /* Switch data direction. */
    rs &= ~LAN91_MGMT_MDOE;
    nic_outlb(LAN91_MGMT, rs);
    nic_outlb(LAN91_MGMT, rs | LAN91_MGMT_MCLK);

    /* Clock data in. */
    for (i = 0; i < 16; i++) {
        nic_outlb(LAN91_MGMT, rs);
        nic_outlb(LAN91_MGMT, rs | LAN91_MGMT_MCLK);
        rc <<= 1;
        rc |= (nic_inlb(LAN91_MGMT) & LAN91_MGMT_MDI) != 0;
    }

    /* This will set the clock line to low. */
    nic_outlb(LAN91_MGMT, rs);

    return rc;
}

/*!
 * \brief Write value to PHY register.
 *
 * \note NIC interrupts must have been disabled before calling this routine.
 *
 * \param reg PHY register number.
 * \param val Value to write.
 */
static void NicPhyWrite(uint8_t reg, uint16_t val)
{
    uint16_t msk;
    uint8_t rs;

    /* Select register for writing. */
    rs = NicPhyRegSelect(reg, 1);

    /* Switch data direction dummy. */
    nic_outlb(LAN91_MGMT, rs | LAN91_MGMT_MDO);
    nic_outlb(LAN91_MGMT, rs | LAN91_MGMT_MDO | LAN91_MGMT_MCLK);
    nic_outlb(LAN91_MGMT, rs);
    nic_outlb(LAN91_MGMT, rs | LAN91_MGMT_MCLK);

    /* Clock data out. */
    for (msk = 0x8000; msk; msk >>= 1) {
        if (val & msk) {
            nic_outlb(LAN91_MGMT, rs | LAN91_MGMT_MDO);
            nic_outlb(LAN91_MGMT, rs | LAN91_MGMT_MDO | LAN91_MGMT_MCLK);
        } else {
            nic_outlb(LAN91_MGMT, rs);
            nic_outlb(LAN91_MGMT, rs | LAN91_MGMT_MCLK);
        }
    }

    /* Set clock line low and output line int z-state. */
    nic_outlb(LAN91_MGMT, rs & ~LAN91_MGMT_MDOE);
}

/*!
 * \brief Configure the internal PHY.
 *
 * Reset the PHY and initiate auto-negotiation.
 */
static int NicPhyConfig(void)
{
    uint16_t phy_sr;
    uint16_t phy_to;
    uint16_t mode;

    /* 
     * Reset the PHY and wait until this self clearing bit
     * becomes zero. We sleep 63 ms before each poll and
     * give up after 3 retries. 
     */
    NicPhyWrite(LAN91_PHYCR, LAN91_PHYCR_RST);
    for (phy_to = 0;; phy_to++) {
        NutSleep(63);
        if ((NicPhyRead(LAN91_PHYCR) & LAN91_PHYCR_RST) == 0)
            break;
        if (phy_to > 3)
            return -1;
    }

    /* Enable PHY interrupts. */
    NicPhyWrite(LAN91_PHYMSK, LAN91_PHYMSK_MLOSSSYN | LAN91_PHYMSK_MCWRD | LAN91_PHYMSK_MSSD |
                LAN91_PHYMSK_MESD | LAN91_PHYMSK_MRPOL | LAN91_PHYMSK_MJAB | LAN91_PHYMSK_MSPDDT | LAN91_PHYMSK_MDPLDT);

    /* Set RPC register. */
    mode = LAN91_RPCR_ANEG | LAN91_RPCR_LEDA_PAT | LAN91_RPCR_LEDB_PAT;
    nic_bs(0);
    nic_outw(LAN91_RPCR, mode);

#ifdef LAN91_FIXED
    /* Disable link. */
    phy_sr = NicPhyRead(LAN91_PHYCFR1);
    NicPhyWrite(LAN91_PHYCFR1, phy_sr | 0x8000);
    NutSleep(63);

    /* Set fixed capabilities. */
    NicPhyWrite(LAN91_PHYCR, LAN91_FIXED);
    nic_bs(0);
    nic_outw(LAN91_RPCR, mode);

    /* Enable link. */
    phy_sr = NicPhyRead(LAN91_PHYCFR1);
    NicPhyWrite(LAN91_PHYCFR1, phy_sr & ~0x8000);
    phy_sr = NicPhyRead(LAN91_PHYCFR1);

#else
    /*
     * Advertise our capabilities, initiate auto negotiation
     * and wait until this has been completed.
     */
    NicPhyWrite(LAN91_PHYANAD, LAN91_PHYANAD_TX_FDX | LAN91_PHYANAD_TX_HDX | LAN91_PHYANAD_10FDX | LAN91_PHYANAD_10_HDX | LAN91_PHYANAD_CSMA);
    NutSleep(63);
    for (phy_to = 0, phy_sr = 0;; phy_to++) {
        /* Give up after 10 seconds. */
        if (phy_to >= 1024)
            return -1;
        /* Restart auto negotiation every 4 seconds or on failures. */
        if ((phy_to & 127) == 0 /* || (phy_sr & LAN91_PHYSR_REM_FLT) != 0 */ ) {
            NicPhyWrite(LAN91_PHYCR, LAN91_PHYCR_ANEG_EN | LAN91_PHYCR_ANEG_RST);
            NutSleep(63);
        }
        /* Check if we are done. */
        phy_sr = NicPhyRead(LAN91_PHYSR);
        if (phy_sr & LAN91_PHYSR_ANEG_ACK)
            break;
        NutSleep(63);
    }
#endif

    return 0;
}

/*!
 * \brief Wait until MMU is ready.
 *
 * Poll the MMU command register until \ref LAN91_MMUCR_BUSY
 * is cleared.
 *
 * \param tmo Timeout in milliseconds.
 *
 * \return 0 on success or -1 on timeout.
 */
static INLINE int NicMmuWait(uint_fast16_t tmo)
{
    while (tmo--) {
        if ((nic_inlb(LAN91_MMUCR) & LAN91_MMUCR_BUSY) == 0)
            break;
        NutDelay(1);
    }
    return tmo ? 0 : -1;
}

/*!
 * \brief Reset the Ethernet controller.
 *
 * \return 0 on success, -1 otherwise.
 */
static int NicReset(void)
{
#ifdef LAN91_RESET_BIT
    GpioPinConfigSet(LAN91_RESET_GPIO_BANK, LAN91_RESET_GPIO_BIT, GPIO_CFG_OUTPUT);
    GpioPinSet(LAN91_RESET_GPIO_BANK, LAN91_RESET_GPIO_BIT, 1);
    NutDelay(10);
    GpioPinSet(LAN91_RESET_GPIO_BANK, LAN91_RESET_GPIO_BIT, 0);
    NutDelay(100);
#endif

    /* Disable all interrupts. */
    nic_outlb(LAN91_MSK, 0);

    /* MAC and PHY software reset. */
    nic_bs(0);
    nic_outw(LAN91_RCR, LAN91_RCR_SOFT_RST);

    /* Enable Ethernet protocol handler. */
    nic_bs(1);
    nic_outw(LAN91_CR, LAN91_CR_EPH_EN);

    NutDelay(10);

    /* Disable transmit and receive. */
    nic_bs(0);
    nic_outw(LAN91_RCR, 0);
    nic_outw(LAN91_TCR, 0);

    /* Enable auto release. */
    nic_bs(1);
    nic_outw(LAN91_CTR, LAN91_CTR_AUTO_RELEASE);

    /* Reset MMU. */
    nic_bs(2);
    nic_outlb(LAN91_MMUCR, LAN91_MMU_RST);
    if (NicMmuWait(1000))
        return -1;

    return 0;
}

/*
 * Fires up the network interface. NIC interrupts
 * should have been disabled when calling this
 * function.
 *
 * \param mac Six byte unique MAC address.
 */
static int NicStart(CONST uint8_t * mac)
{
    uint_fast8_t i;

    if (NicReset())
        return -1;

    /* Enable receiver. */
    nic_bs(3);
    nic_outlb(LAN91_ERCV, 7);
    nic_bs(0);
    nic_outw(LAN91_RCR, LAN91_RCR_RXEN);

    /* Enable transmitter and padding. */
    nic_outw(LAN91_TCR, LAN91_TCR_PAD_EN | LAN91_TCR_TXENA);

    /* Configure the PHY. */
    if (NicPhyConfig())
        return -1;

    /* Set MAC address. */
    nic_bs(1);
    for (i = 0; i < 6; i++)
        nic_outlb(LAN91_IAR + i, mac[i]);

    /* Enable interrupts. */
    nic_bs(2);
    nic_outlb(LAN91_MSK, LAN91_INT_ERCV | LAN91_INT_RCV | LAN91_INT_RX_OVRN);

    return 0;
}

#if defined(LAN91_SIGNAL)
/*
 * NIC interrupt entry.
 */
static void NicInterrupt(void *arg)
{
    uint8_t isr;
    uint8_t imr;
    NICINFO *ni = (NICINFO *) ((NUTDEVICE *) arg)->dev_dcb;

#ifdef NUT_PERFMON
    ni->ni_interrupts++;
#endif
    /* Read the interrupt mask and disable all interrupts. */
    nic_bs(2);
    imr = nic_inlb(LAN91_MSK);
    nic_outlb(LAN91_MSK, 0);

    /* Read the interrupt status and acknowledge all interrupts. */
    isr = nic_inlb(LAN91_IST);
    isr &= imr;

    /*
     * If this is a transmit interrupt, then a packet has been sent. 
     * So we can clear the transmitter busy flag and wake up the 
     * transmitter thread.
     */
    if (isr & LAN91_INT_TX_EMPTY) {
        nic_outlb(LAN91_ACK, LAN91_INT_TX_EMPTY);
        imr &= ~LAN91_INT_TX_EMPTY;
    }
    /* Transmit error. */
    else if (isr & LAN91_INT_TX) {
        /* re-enable transmit */
        nic_bs(0);
        nic_outw(LAN91_TCR, nic_inlb(LAN91_TCR) | LAN91_TCR_TXENA);
        nic_bs(2);
        nic_outlb(LAN91_ACK, LAN91_INT_TX);
        /* kill the packet */
        nic_outlb(LAN91_MMUCR, LAN91_MMU_PKT);
    }


    /*
     * If this is a receive interrupt, then wake up the receiver 
     * thread.
     */
    if (isr & LAN91_INT_RX_OVRN) {
        nic_outlb(LAN91_ACK, LAN91_INT_RX_OVRN);
        NutEventPostFromIrq(&ni->ni_rx_rdy);
    }
    if (isr & LAN91_INT_ERCV) {
        nic_outlb(LAN91_ACK, LAN91_INT_ERCV);
        NutEventPostFromIrq(&ni->ni_rx_rdy);
    }
    if (isr & LAN91_INT_RCV) {
        nic_outlb(LAN91_ACK, LAN91_INT_RCV);
        imr &= ~LAN91_INT_RCV;
        NutEventPostFromIrq(&ni->ni_rx_rdy);
    }

    if (isr & LAN91_INT_ALLOC) {
        imr &= ~LAN91_INT_ALLOC;
        NutEventPostFromIrq(&maq);
    }
    nic_outlb(LAN91_MSK, imr);
}
#endif

/*
 * Write data block to the NIC.
 */
static void NicWrite(uint8_t * buf, uint16_t len)
{
    register uint16_t l = len - 1;
    register uint8_t ih = (uint16_t) l >> 8;
    register uint8_t il = (uint8_t) l;

    if (!len)
        return;

    do {
        do {
            nic_outlb(LAN91_DATA, *buf++);
        } while (il-- != 0);
    } while (ih-- != 0);
}

/*
 * Read data block from the NIC.
 */
static void NicRead(uint8_t * buf, uint16_t len)
{
    register uint16_t l = len - 1;
    register uint8_t ih = (uint16_t) l >> 8;
    register uint8_t il = (uint8_t) l;

    if (!len)
        return;

    do {
        do {
            *buf++ = nic_inlb(LAN91_DATA);
        } while (il-- != 0);
    } while (ih-- != 0);
}

/*!
 * \brief Fetch the next packet out of the receive ring buffer.
 *
 * Nic interrupts must be disabled when calling this funtion.
 *
 * \return Pointer to an allocated ::NETBUF. If there is no
 *         no data available, then the function returns a
 *         null pointer. If the NIC's buffer seems to be
 *         corrupted, a pointer to 0xFFFF is returned.
 */
static NETBUF *NicGetPacket(void)
{
    NETBUF *nb = 0;
    uint16_t fsw;
    uint16_t fbc;

    /* Check the fifo empty bit. If it is set, then there is 
       nothing in the receiver fifo. */
    nic_bs(2);
    if (nic_inw(LAN91_FIFO) & 0x8000) {
        return 0;
    }

    /* Inialize pointer register. */
    nic_outw(LAN91_PTR, LAN91_PTR_READ | LAN91_PTR_RCV | LAN91_PTR_AUTO_INCR);
    _NOP();
    _NOP();
    _NOP();
    _NOP();

    /* Read status word and byte count. */
    fsw = nic_inw(LAN91_DATA);
    fbc = nic_inw(LAN91_DATA);

    /* Check for frame errors. */
    if (fsw & 0xAC00) {
        nb = (NETBUF *) 0xFFFF;
    }
    /* Check the byte count. */
    else if (fbc < 66 || fbc > 1524) {
        nb = (NETBUF *) 0xFFFF;
    }

    else {
        /* 
         * Allocate a NETBUF. 
         * Hack alert: Rev A chips never set the odd frame indicator.
         */
        fbc -= 3;
        nb = NutNetBufAlloc(0, NBAF_DATALINK, fbc);

        /* Perform the read. */
        if (nb)
            NicRead(nb->nb_dl.vp, fbc);
    }

    /* Release the packet. */
    nic_outlb(LAN91_MMUCR, LAN91_MMU_TOP);

    return nb;
}

/*!
 * \brief Load a packet into the nic's transmit ring buffer.
 *
 * Interupts must have been disabled when calling this function.
 *
 * \param nb Network buffer structure containing the packet to be sent.
 *           The structure must have been allocated by a previous
 *           call NutNetBufAlloc(). This routine will automatically
 *           release the buffer in case of an error.
 *
 * \return 0 on success, -1 in case of any errors. Errors
 *         will automatically release the network buffer 
 *         structure.
 */
static int NicPutPacket(NETBUF * nb)
{
    uint16_t sz;
    uint_fast8_t odd = 0;
    uint8_t imsk;

    /*
     * Calculate the number of bytes to be send. Do not send packets 
     * larger than the Ethernet maximum transfer unit. The MTU
     * consist of 1500 data bytes plus the 14 byte Ethernet header
     * plus 4 bytes CRC. We check the data bytes only.
     */
    if ((sz = nb->nb_nw.sz + nb->nb_tp.sz + nb->nb_ap.sz) > ETHERMTU)
        return -1;

    /* Disable all interrupts. */
    imsk = nic_inlb(LAN91_MSK);
    nic_outlb(LAN91_MSK, 0);

    /* Allocate packet buffer space. */
    nic_bs(2);
    nic_outlb(LAN91_MMUCR, LAN91_MMU_ALO);
    if (NicMmuWait(100))
        return -1;

    /* Enable interrupts including allocation success. */
    nic_outlb(LAN91_MSK, imsk | LAN91_INT_ALLOC);

    /* The MMU needs some time. Use it to calculate the byte count. */
    sz += nb->nb_dl.sz;
    sz += 6;
    if (sz & 1) {
        sz++;
        odd++;
    }

    /* Wait for allocation success. */
    while ((nic_inlb(LAN91_IST) & LAN91_INT_ALLOC) == 0) {
        if (NutEventWait(&maq, 125)) {
            nic_outlb(LAN91_MMUCR, LAN91_MMU_RST);
            NicMmuWait(1000);
            nic_outlb(LAN91_MMUCR, LAN91_MMU_ALO);
            if (NicMmuWait(100) || (nic_inlb(LAN91_IST) & LAN91_INT_ALLOC) == 0) {
                if (NutEventWait(&maq, 125)) {
                    return -1;
                }
            }
        }
    }

    /* Disable interrupts. */
    imsk = nic_inlb(LAN91_MSK);
    nic_outlb(LAN91_MSK, 0);


    nic_outlb(LAN91_PNR, nic_inhb(LAN91_PNR));

    nic_outw(LAN91_PTR, 0x4000);

    /* Transfer control word. */
    nic_outlb(LAN91_DATA, 0);
    nic_outlb(LAN91_DATA, 0);

    /* Transfer the byte count. */
    nic_outw(LAN91_DATA, sz);

    /* Transfer the Ethernet frame. */
    NicWrite(nb->nb_dl.vp, nb->nb_dl.sz);
    NicWrite(nb->nb_nw.vp, nb->nb_nw.sz);
    NicWrite(nb->nb_tp.vp, nb->nb_tp.sz);
    NicWrite(nb->nb_ap.vp, nb->nb_ap.sz);

    if (odd)
        nic_outlb(LAN91_DATA, 0);

    /* Transfer the control word. */
    nic_outw(LAN91_DATA, 0);

    /* Enqueue packet. */
    if (NicMmuWait(100))
        return -1;
    nic_outlb(LAN91_MMUCR, LAN91_MMU_ENQ);

    /* Enable interrupts. */
    imsk |= LAN91_INT_TX | LAN91_INT_TX_EMPTY;
    nic_outlb(LAN91_MSK, imsk);

    return 0;
}


/*! \fn NicRxLanc(void *arg)
 * \brief NIC receiver thread.
 *
 */
THREAD(NicRxLanc, arg)
{
    NUTDEVICE *dev;
    IFNET *ifn;
    NICINFO *ni;
    NETBUF *nb;
    uint8_t imsk;

    dev = arg;
    ifn = (IFNET *) dev->dev_icb;
    ni = (NICINFO *) dev->dev_dcb;

    /*
     * This is a temporary hack. Due to a change in initialization,
     * we may not have got a MAC address yet. Wait until a valid one
     * has been set.
     */
    while (!ETHER_IS_UNICAST(ifn->if_mac)) {
        NutSleep(10);
    }

    /*
     * Do not continue unless we managed to start the NIC. We are
     * trapped here if the Ethernet link cannot be established.
     * This happens, for example, if no Ethernet cable is plugged
     * in.
     */
    while(NicStart(ifn->if_mac)) {
        NutSleep(1000);
    }

#ifdef LAN91_SIGNAL
    NutIrqSetMode(&LAN91_SIGNAL, NUT_IRQMODE_RISINGEDGE);
    NutIrqEnable(&LAN91_SIGNAL);
#endif

    NutEventPost(&mutex);

    /* Run at high priority. */
    NutThreadSetPriority(9);

    for (;;) {
        /*
         * Wait for the arrival of new packets or
         * check the receiver every two second.
         */
        NutEventWait(&ni->ni_rx_rdy, LAN91_RX_POLLTIME);

        /*
         * Fetch all packets from the NIC's internal
         * buffer and pass them to the registered handler.
         */
        imsk = nic_inlb(LAN91_MSK);
        nic_outlb(LAN91_MSK, 0);
        while ((nb = NicGetPacket()) != 0) {
            if (nb != (NETBUF *) 0xFFFF) {
#ifdef NUT_PERFMON
                ni->ni_rx_packets++;
#endif
                (*ifn->if_recv) (dev, nb);
            }
        }
        nic_outlb(LAN91_MSK, imsk | LAN91_INT_RCV | LAN91_INT_ERCV);
    }
}

/*!
 * \brief Send Ethernet packet.
 *
 * \param dev   Identifies the device to use.
 * \param nb    Network buffer structure containing the packet to be sent.
 *              The structure must have been allocated by a previous
 *              call NutNetBufAlloc().
 *
 * \return 0 on success, -1 in case of any errors.
 */
static int Lan91Output(NUTDEVICE * dev, NETBUF * nb)
{
    static uint_fast16_t mx_wait = LAN91_TX_POLLTIME;
    int rc = -1;

    /*
     * After initialization we are waiting for a long time to give
     * the PHY a chance to establish an Ethernet link.
     */
    if (NutEventWait(&mutex, mx_wait) == 0) {

        if (NicPutPacket(nb) == 0) {
#ifdef NUT_PERFMON
            NICINFO *ni = (NICINFO *) dev->dev_dcb;
            ni->ni_tx_packets++;
#endif
            rc = 0;
            /* Ethernet works. Set a long waiting time in case we
               temporarly lose the link next time. */
            mx_wait = LAN91_TX_POLLTIME;
        }
        NutEventPost(&mutex);
    }
#if defined(LAN91_SIGNAL)
    /*
     * Probably no Ethernet link. Significantly reduce the waiting
     * time, so following transmission will soon return an error.
     */
    else {
        mx_wait = 500;
    }
#endif
    return rc;
}

/*!
 * \brief Initialize Ethernet hardware.
 *
 * Resets the LAN91 Ethernet controller, initializes all required 
 * hardware registers and starts a background thread for incoming 
 * Ethernet traffic.
 *
 * Applications should do not directly call this function. It is 
 * automatically executed during during device registration by 
 * NutRegisterDevice().
 *
 * If the network configuration hasn't been set by the application
 * before registering the specified device, this function will
 * call NutNetLoadConfig() to get the MAC address.
 *
 * \param dev Identifies the device to initialize.
 */
static int Lan91Init(NUTDEVICE * dev)
{
    /* Disable NIC interrupt and clear NICINFO structure. */
#ifdef LAN91_SIGNAL
    NutIrqDisable(&LAN91_SIGNAL);
#endif
    memset(dev->dev_dcb, 0, sizeof(NICINFO));

#ifdef LAN91_SIGNAL
    /* Register interrupt handler and enable interrupts. */
    if (NutRegisterIrqHandler(&LAN91_SIGNAL, NicInterrupt, dev))
        return -1;
#endif

    /*
     * Start the receiver thread.
     */
    NutThreadCreate("lan91rx", NicRxLanc, dev, 640);

    return 0;
}

static int Lan91IOCtl(NUTDEVICE * dev, int req, void *conf)
{
    int rc = 0;
    uint32_t *lvp = (uint32_t *) conf;
    IFNET *nif = (IFNET *) dev->dev_icb;

    switch (req) {
    case SIOCSIFFLAGS:
        /* Set interface flags. */
        if (*lvp & IFF_UP) {
            if ((nif->if_flags & IFF_UP) == 0) {
                /* Start interface. */
            }
        } else if (nif->if_flags & IFF_UP) {
            /* Stop interface. */
        }
        break;
    case SIOCGIFFLAGS:
        /* Get interface flags. */
        *lvp = nif->if_flags;
        break;
    case SIOCSIFADDR:
        /* Set interface hardware address. */
        memcpy(nif->if_mac, conf, sizeof(nif->if_mac));
        break;
    case SIOCGIFADDR:
        /* Get interface hardware address. */
        memcpy(conf, nif->if_mac, sizeof(nif->if_mac));
        break;
    default:
        rc = -1;
        break;
    }
    return rc;
}

static NICINFO dcb_eth0;

/*!
 * \brief Network interface information structure.
 *
 * Used to call.
 */
static IFNET ifn_eth0 = {
    IFT_ETHER,          /*!< \brief Interface type, if_type. */
    0,                  /*!< \brief Interface flags, if_flags. */
    {0, 0, 0, 0, 0, 0}, /*!< \brief Hardware net address, if_mac. */
    0,                  /*!< \brief IP address, if_local_ip. */
    0,                  /*!< \brief Remote IP address for point to point, if_remote_ip. */
    0,                  /*!< \brief IP network mask, if_mask. */
    ETHERMTU,           /*!< \brief Maximum size of a transmission unit, if_mtu. */
    0,                  /*!< \brief Packet identifier, if_pkt_id. */
    0,                  /*!< \brief Linked list of arp entries, arpTable. */
    0,                  /*!< \brief Linked list of multicast address entries, if_mcast. */
    NutEtherInput,      /*!< \brief Routine to pass received data to, if_recv(). */
    Lan91Output,        /*!< \brief Driver output routine, if_send(). */
    NutEtherOutput,     /*!< \brief Media output routine, if_output(). */
    NULL                /*!< \brief Interface specific control function, if_ioctl(). */
};

/*!
 * \brief Device information structure.
 *
 * A pointer to this structure must be passed to NutRegisterDevice() 
 * to bind this Ethernet device driver to the Nut/OS kernel.
 * An application may then call NutNetIfConfig() with the name \em eth0 
 * of this driver to initialize the network interface.
 * 
 */
NUTDEVICE devLan91 = {
    0,          /* Pointer to next device. */
    {'e', 't', 'h', '0', 0, 0, 0, 0, 0},        /* Unique device name. */
    IFTYP_NET,  /* Type of device. */
    0,          /* Base address. */
    0,          /* First interrupt number. */
    &ifn_eth0,  /* Interface control block. */
    &dcb_eth0,  /* Driver control block. */
    Lan91Init,  /* Driver initialization routine. */
    Lan91IOCtl, /* Driver specific control function. */
    0,          /* Read from device. */
    0,          /* Write to device. */
#ifdef __HARVARD_ARCH__
    0,          /* Write from program space data to device. */
#endif
    0,          /* Open a device or file. */
    0,          /* Close a device or file. */
    0           /* Request file size. */
};

/*@}*/
