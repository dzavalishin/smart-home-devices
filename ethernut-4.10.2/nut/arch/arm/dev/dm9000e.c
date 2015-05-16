/*
 * Copyright (C) 2008 by egnite GmbH. All rights reserved.
 * Copyright (C) 2003-2005 by egnite Software GmbH. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY EGNITE SOFTWARE GMBH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL EGNITE
 * SOFTWARE GMBH OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
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
 * $Log$
 * Revision 1.12  2009/02/06 15:37:39  haraldkipp
 * Added stack space multiplier and addend. Adjusted stack space.
 *
 * Revision 1.11  2008/08/28 11:12:15  haraldkipp
 * Added interface flags, which will be required to implement Ethernet ioctl
 * functions.
 *
 * Revision 1.10  2008/08/11 06:59:07  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.9  2008/02/15 17:09:44  haraldkipp
 * Added support for the Elektor Internet Radio.
 *
 * Revision 1.8  2007/08/17 11:43:46  haraldkipp
 * Enable multicast.
 *
 * Revision 1.7  2007/05/24 07:26:44  haraldkipp
 * Added some delay befor reading the status of the received packet. Fixes
 * bug #1672527, thanks to Andreas Helmcke.
 *
 * Revision 1.6  2007/05/02 11:22:51  haraldkipp
 * Added multicast table entry.
 *
 * Revision 1.5  2006/06/28 17:10:15  haraldkipp
 * Include more general header file for ARM.
 *
 * Revision 1.4  2006/03/16 19:04:48  haraldkipp
 * Adding a short delay before reading the status word makes it work with
 * compiler optimization. On receiver overflow interrupts the chip is
 * declared insane. The output routine will no more enter NutEventWait()
 * on insane chips.
 *
 * Revision 1.3  2006/03/02 19:51:16  haraldkipp
 * Replaced GCC specific inline specifications with their portable
 * counterparts.
 *
 * Revision 1.2  2006/01/23 17:33:14  haraldkipp
 * Possible memory alignment problem may start network interface too early.
 *
 * Revision 1.1  2005/10/24 08:49:05  haraldkipp
 * Initial check in.
 *
 */

#include <cfg/os.h>
#include <arch/arm.h>

#include <string.h>
#include <stdlib.h>

#include <sys/atom.h>
#include <sys/heap.h>
#include <sys/thread.h>
#include <sys/event.h>
#include <sys/timer.h>
#include <sys/confnet.h>

#include <netinet/if_ether.h>
#include <net/ether.h>
#include <net/if_var.h>

#include <dev/irqreg.h>
#include <dev/dm9000e.h>

/* WARNING: Variadic macros are C99 and may fail with C89 compilers. */
#ifdef NUTDEBUG
#include <stdio.h>
#define DMPRINTF(args,...) printf(args,##__VA_ARGS__)
#else
#define DMPRINTF(args,...)
#endif

#ifndef NUT_THREAD_NICRXSTACK
/* arm-elf-gcc size optimized code used 160 bytes. */
#define NUT_THREAD_NICRXSTACK   384
#endif

/*
 * Determine ports, which had not been explicitely configured.
 */
#if defined(ETHERNUT3)

#ifndef NIC_BASE_ADDR
#define NIC_BASE_ADDR   0x20000000
#endif

#ifndef NIC_SIGNAL_IRQ
#define NIC_SIGNAL_IRQ  INT1
#endif

#ifndef NIC_SIGNAL_PDR
#define NIC_SIGNAL_PDR  PIO_PDR
#endif

#ifndef NIC_SIGNAL_BIT
#define NIC_SIGNAL_BIT  10
#endif

#elif defined(ELEKTOR_IR1)

#ifndef NIC_BASE_ADDR
#define NIC_BASE_ADDR   0x30000000
#endif

#ifndef NIC_SIGNAL_IRQ
#define NIC_SIGNAL_IRQ  INT0
#endif

#ifndef NIC_SIGNAL_PDR
#define NIC_SIGNAL_PDR  PIOB_PDR
#endif

#ifndef NIC_SIGNAL_XSR
#define NIC_SIGNAL_XSR  PIOB_ASR
#endif

#ifndef NIC_SIGNAL_BIT
#define NIC_SIGNAL_BIT  PB20_IRQ0_A
#endif

#endif

#ifdef NIC_BASE_ADDR

#ifndef NIC_DATA_ADDR
#define NIC_DATA_ADDR   (NIC_BASE_ADDR + 4)
#endif

#define INT0    0
#define INT1    1
#define INT2    2
#define INT3    3
#define INT4    4
#define INT5    5
#define INT6    6
#define INT7    7

#ifdef NIC_RESET_BIT

#if (NIC_RESET_AVRPORT == AVRPORTB)
#define NIC_RESET_PORT   PORTB
#define NIC_RESET_DDR    DDRB

#elif (NIC_RESET_AVRPORT == AVRPORTD)
#define NIC_RESET_PORT   PORTD
#define NIC_RESET_DDR    DDRD

#elif (NIC_RESET_AVRPORT == AVRPORTE)
#define NIC_RESET_PORT   PORTE
#define NIC_RESET_DDR    DDRE

#elif (NIC_RESET_AVRPORT == AVRPORTF)
#define NIC_RESET_PORT   PORTF
#define NIC_RESET_DDR    DDRF

#endif                          /* NIC_RESET_AVRPORT */

#endif                          /* NIC_RESET_BIT */

/*
 * Determine interrupt settings.
 * DOES NOT WORK
 */
#if (NIC_SIGNAL_IRQ == INT0)
#define NIC_SIGNAL          sig_INTERRUPT0

#elif (NIC_SIGNAL_IRQ == INT2)
#define NIC_SIGNAL          sig_INTERRUPT2

#elif (NIC_SIGNAL_IRQ == INT3)
#define NIC_SIGNAL          sig_INTERRUPT3

#elif (NIC_SIGNAL_IRQ == INT4)
#define NIC_SIGNAL          sig_INTERRUPT4

#elif (NIC_SIGNAL_IRQ == INT5)
#define NIC_SIGNAL          sig_INTERRUPT5

#elif (NIC_SIGNAL_IRQ == INT6)
#define NIC_SIGNAL          sig_INTERRUPT6

#elif (NIC_SIGNAL_IRQ == INT7)
#define NIC_SIGNAL          sig_INTERRUPT7

#else
#define NIC_SIGNAL          sig_INTERRUPT1

#endif

/*!
 * \addtogroup xgDm9000eRegs
 */
/*@{*/

#define NIC_NCR     0x00        /* Network control register (0x00). */
#define NIC_NCR_LBM     0x06    /* Loopback mode. */
#define NIC_NCR_LBNORM  0x00    /* Normal mode. */
#define NIC_NCR_LBMAC   0x02    /* MAC loopback. */
#define NIC_NCR_LBPHY   0x04    /* PHY loopback. */
#define NIC_NCR_RST     0x01    /* Software reset, auto clear. */

#define NIC_NSR     0x01        /* Network status register (0x00). */
#define NIC_NSR_SPEED   0x80
#define NIC_NSR_LINKST  0x40
#define NIC_NSR_WAKEST  0x20
#define NIC_NSR_TX2END  0x08
#define NIC_NSR_TX1END  0x04
#define NIC_NSR_RXOV    0x02

#define NIC_TCR     0x02        /* TX control register (0x00). */
#define NIC_TCR_TXREQ    0x01   /* TX request */

#define NIC_TSR1    0x03        /* TX status register I (0x00). */

#define NIC_TSR2    0x04        /* TX status register II (0x00). */

#define NIC_RCR     0x05        /* RX control register (0x00). */
#define NIC_RCR_DIS_LONG 0x20   /* Discard long packets. */
#define NIC_RCR_DIS_CRC 0x10    /* Discard CRC error packets. */
#define NIC_RCR_ALL		0x08    /* Pass all multicast */
#define NIC_RCR_PRMSC   0x02    /* Enable promiscuous mode. */
#define NIC_RCR_RXEN    0x01    /* Enable receiver. */

#define NIC_RSR     0x06        /* RX status register (0x00). */
#define NIC_RSR_ERRORS  0xBF    /* Error bit mask. */
#define NIC_RSR_RF      0x80    /* Runt frame. */
#define NIC_RSR_MF      0x40    /* Multicast frame. */
#define NIC_RSR_LCS     0x20    /* Late collision. */
#define NIC_RSR_RWTO    0x10    /* Receiver watchdog time out. */
#define NIC_RSR_PLE     0x08    /* Physical layer error. */
#define NIC_RSR_AE      0x04    /* Alignment error. */
#define NIC_RSR_CE      0x02    /* CRC error. */
#define NIC_RSR_FOE     0x01    /* FIFO overflow error. */

#define NIC_ROCR    0x07        /* Receive overflow counter register (0x00). */

#define NIC_BPTR    0x08        /* Back pressure threshold register (0x37). */

#define NIC_FCTR    0x09        /* Flow control threshold register (0x38). */

#define NIC_FCR     0x0A        /* RX flow control register (0x00). */

#define NIC_EPCR    0x0B        /* EEPROM and PHY control register. */

#define NIC_EPAR    0x0C        /* EEPROM and PHY address register. */

#define NIC_EPDRL   0x0D        /* EEPROM and PHY low byte data register. */

#define NIC_EPDRH   0x0E        /* EEPROM and PHY high byte data register. */

#define NIC_WCR     0x0F        /* Wake up control register (0x00). */

#define NIC_PAR     0x10        /* 6 byte physical address register. */

#define NIC_MAR     0x16        /* 8 byte multicast address register. */

#define NIC_GPCR    0x1E        /* General purpose control register (?). */

#define NIC_GPR     0x1F        /* General purpose register (?). */

#define NIC_TRPA    0x22        /* 2 byte TX SRAM read pointer address, low/high (0x0000). */

#define NIC_RWPA    0x24        /* 2 byte RX SRAM write pointer address, low/high (0x0000). */

#define NIC_VID     0x28        /* 2 byte vendor ID (0x0A46). */

#define NIC_PID     0x2A        /* 2 byte product ID (0x0900). */

#define NIC_CHIPR   0x2C        /* Chip revision (0x00). */

#define NIC_SMCR    0x2F        /* Special mode register (0x00). */

#define NIC_MRCMDX  0xF0        /* Memory data read command w/o increment (?). */

#define NIC_MRCMD   0xF2        /* Memory data read command with increment (?). */

#define NIC_MRR     0xF4        /* 2 byte memory data read register, low/high (?). */

#define NIC_MWCMDX  0xF6        /* Memory data write command register w/o increment (?). */

#define NIC_MWCMD   0xF8        /* Memory data write command register with increment (?). */

#define NIC_MWR     0xFA        /* Memory data write command register with increment (?). */

#define NIC_TXPL    0xFC        /* 2 byte TX packet length register. (?). */

#define NIC_ISR     0xFE        /* Interrupt status register (0x00). */
#define NIC_ISR_IOM     0xC0    /* I/O mode mask */
#define NIC_ISR_M16     0x00    /* 16-bit I/O mode */
#define NIC_ISR_M32     0x40    /* 32-bit I/O mode */
#define NIC_ISR_M8      0x80    /* 8-bit I/O mode */
#define NIC_ISR_ROOS    0x08    /* Receiver overflow counter interrupt. */
#define NIC_ISR_ROS     0x04    /* Receiver overflow interrupt. */
#define NIC_ISR_PTS     0x02    /* Transmitter interrupt. */
#define NIC_ISR_PRS     0x01    /* Receiver interrupt. */

#define NIC_IMR     0xFF        /* Interrupt mask register (0x00). */
#define NIC_IMR_PAR     0x80    /* Enable read/write pointer wrap around. */
#define NIC_IMR_ROOM    0x08    /* Enable receiver overflow counter interrupts. */
#define NIC_IMR_ROM     0x04    /* Enable receiver overflow interrupts. */
#define NIC_IMR_PTM     0x02    /* Enable transmitter interrupts. */
#define NIC_IMR_PRM     0x01    /* Enable receiver interrupts. */

#define NIC_PHY_BMCR    0x00    /* Basic mode control register. */

#define NIC_PHY_BMSR    0x01    /* Basic mode status register. */
#define NIC_PHY_BMSR_ANCOMPL    0x0020  /* Auto negotiation complete. */
#define NIC_PHY_BMSR_LINKSTAT   0x0004  /* Link status. */

#define NIC_PHY_ID1     0x02    /* PHY identifier register 1. */

#define NIC_PHY_ID2     0x03    /* PHY identifier register 2. */

#define NIC_PHY_ANAR    0x04    /* Auto negotiation advertisement register. */

#define NIC_PHY_ANLPAR  0x05    /* Auto negotiation link partner availability register. */

#define NIC_PHY_ANER    0x06    /* Auto negotiation expansion register. */

#define NIC_PHY_DSCR    0x10    /* Davicom specified configuration register. */

#define NIC_PHY_DSCSR   0x11    /* Davicom specified configuration and status register. */

#define NIC_PHY_10BTCSR 0x12    /* 10BASE-T configuration and status register. */

/*!
 * \brief Network interface controller information structure.
 */
struct _NICINFO {
#ifdef NUT_PERFMON
    uint32_t ni_rx_packets;       /*!< Number of packets received. */
    uint32_t ni_tx_packets;       /*!< Number of packets sent. */
    uint32_t ni_overruns;         /*!< Number of packet overruns. */
    uint32_t ni_rx_frame_errors;  /*!< Number of frame errors. */
    uint32_t ni_rx_crc_errors;    /*!< Number of CRC errors. */
    uint32_t ni_rx_missed_errors; /*!< Number of missed packets. */
#endif
    HANDLE volatile ni_rx_rdy;  /*!< Receiver event queue. */
    HANDLE volatile ni_tx_rdy;  /*!< Transmitter event queue. */
    HANDLE ni_mutex;            /*!< Access mutex semaphore. */
    volatile int ni_tx_queued;  /*!< Number of packets in transmission queue. */
    volatile int ni_tx_quelen;  /*!< Number of bytes in transmission queue not sent. */
    volatile int ni_insane;     /*!< Set by error detection. */
    int ni_iomode;              /*!< 8 or 16 bit access. 32 bit is not supported. */
    uint8_t ni_mar[8];          /*!< Multicast Address Register. */
};

/*!
 * \brief Network interface controller information type.
 */
typedef struct _NICINFO NICINFO;

/*@}*/

/*!
 * \addtogroup xgNicDm9000e
 */
/*@{*/


/*
 * ether_crc32_le based on FreeBSD code from if_ethersubr.c
 */
static const uint32_t crctab[] = {
    0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
    0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
    0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
    0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
};

static uint32_t ether_crc32_le(const uint8_t *buf, uint8_t len)
{
   uint32_t crc;
   uint8_t i;

   crc = 0xffffffff; /* initial value */

   for (i = 0; i < len; i++) {
      crc ^= buf[i];
      crc = (crc >> 4) ^ crctab[crc & 0xf];
      crc = (crc >> 4) ^ crctab[crc & 0xf];
   }

   return (crc);
}

static INLINE void nic_outb(uint8_t reg, uint8_t val)
{
    outb(NIC_BASE_ADDR, reg);
    outb(NIC_DATA_ADDR, val);
}

static INLINE uint8_t nic_inb(uint16_t reg)
{
    outb(NIC_BASE_ADDR, reg);
    return inb(NIC_DATA_ADDR);
}

/*!
 * \brief Read contents of PHY register.
 *
 * \param reg PHY register number.
 *
 * \return Contents of the specified register.
 */
static uint16_t phy_inw(uint8_t reg)
{
    /* Select PHY register */
    nic_outb(NIC_EPAR, 0x40 | reg);

    /* PHY read command. */
    nic_outb(NIC_EPCR, 0x0C);
    NutDelay(1);
    nic_outb(NIC_EPCR, 0x00);

    /* Get data from PHY data register. */
    return ((uint16_t) nic_inb(NIC_EPDRH) << 8) | (uint16_t) nic_inb(NIC_EPDRL);
}

/*!
 * \brief Write value to PHY register.
 *
 * \note NIC interrupts must have been disabled before calling this routine.
 *
 * \param reg PHY register number.
 * \param val Value to write.
 */
static void phy_outw(uint8_t reg, uint16_t val)
{
    /* Select PHY register */
    nic_outb(NIC_EPAR, 0x40 | reg);

    /* Store value in PHY data register. */
    nic_outb(NIC_EPDRL, (uint8_t) val);
    nic_outb(NIC_EPDRH, (uint8_t) (val >> 8));

    /* PHY write command. */
    nic_outb(NIC_EPCR, 0x0A);
    NutDelay(1);
    nic_outb(NIC_EPCR, 0x00);
}

static int NicPhyInit(void)
{
    /* Restart auto negotiation. */
    phy_outw(NIC_PHY_ANAR, 0x01E1);
    phy_outw(NIC_PHY_BMCR, 0x1200);

    nic_outb(NIC_GPCR, 1);
    nic_outb(NIC_GPR, 0);

    return 0;
}

/*!
 * \brief Reset the Ethernet controller.
 *
 * \return 0 on success, -1 otherwise.
 */
static int NicReset(void)
{
    /* Hardware reset. */
#ifdef undef_NIC_RESET_BIT
    sbi(NIC_RESET_DDR, NIC_RESET_BIT);
    sbi(NIC_RESET_PORT, NIC_RESET_BIT);
    NutDelay(WAIT100);
    cbi(NIC_RESET_PORT, NIC_RESET_BIT);
    NutDelay(WAIT250);
    NutDelay(WAIT250);
#else
    /* Software reset. */
    nic_outb(NIC_NCR, NIC_NCR_RST | NIC_NCR_LBMAC);
    NutDelay(1);
    /* FIXME: Delay required. */
#endif
    DMPRINTF("\n*DMRES*\n");
    return NicPhyInit();
}

/*
 * NIC interrupt entry.
 */
static void NicInterrupt(void *arg)
{
    uint_fast8_t isr;
    NICINFO *ni = (NICINFO *) ((NUTDEVICE *) arg)->dev_dcb;

    /* Read interrupt status and disable interrupts. */
    isr = nic_inb(NIC_ISR);
    DMPRINTF("*DMIRQ %02x:", isr);

    /* Receiver interrupt. */
    if (isr & NIC_ISR_PRS) {
        DMPRINTF("RX*");
        nic_outb(NIC_ISR, NIC_ISR_PRS);
        NutEventPostFromIrq(&ni->ni_rx_rdy);
    }

    /* Transmitter interrupt. */
    if (isr & NIC_ISR_PTS) {
        DMPRINTF("TX*");
        if (ni->ni_tx_queued) {
            if (ni->ni_tx_quelen) {
                /* Initiate transfer of a queued packet. */
                nic_outb(NIC_TXPL, (uint8_t) ni->ni_tx_quelen);
                nic_outb(NIC_TXPL + 1, (uint8_t) (ni->ni_tx_quelen >> 8));
                ni->ni_tx_quelen = 0;
                nic_outb(NIC_TCR, NIC_TCR_TXREQ);
            }
            ni->ni_tx_queued--;
        }
        nic_outb(NIC_ISR, NIC_ISR_PTS);
        NutEventPostFromIrq(&ni->ni_tx_rdy);
    }

    /* Receiver overflow interrupt. */
    if (isr & NIC_ISR_ROS) {
        DMPRINTF("OVI*");
        nic_outb(NIC_ISR, NIC_ISR_ROS);
        ni->ni_insane = 1;
        NutEventPostFromIrq(&ni->ni_rx_rdy);
    }

    /* Receiver overflow counter interrupt. */
    if (isr & NIC_ISR_ROOS) {
        DMPRINTF("OVC*");
        nic_outb(NIC_ISR, NIC_ISR_ROOS);
        NutEventPostFromIrq(&ni->ni_rx_rdy);
    }

    if( ni->ni_insane)
        DMPRINTF("INS\n");
    else
        DMPRINTF("\n");
}

/*!
 * \brief Write data block to the NIC.
 *
 * NIC interrupts must be disabled when calling this function.
 */
static void NicWrite8(uint8_t * buf, uint16_t len)
{
    while (len--) {
        outb(NIC_DATA_ADDR, *buf);
        buf++;
    }
}

/*!
 * \brief Write data block to the NIC.
 *
 * NIC interrupts must be disabled when calling this function.
 */
static void NicWrite16(uint8_t * buf, uint16_t len)
{
    uint16_t *wp = (uint16_t *) buf;

    len = (len + 1) / 2;
    while (len--) {
        outw(NIC_DATA_ADDR, *wp);
        wp++;
    }
}

/*!
 * \brief Read data block from the NIC.
 *
 * NIC interrupts must be disabled when calling this function.
 */
static void NicRead8(uint8_t * buf, uint16_t len)
{
    while (len--) {
        *buf++ = inb(NIC_DATA_ADDR);
    }
}

/*!
 * \brief Read data block from the NIC.
 *
 * NIC interrupts must be disabled when calling this function.
 */
static void NicRead16(uint8_t * buf, uint16_t len)
{
    uint16_t *wp = (uint16_t *) buf;

    len = (len + 1) / 2;
    while (len--) {
        *wp++ = inw(NIC_DATA_ADDR);
    }
}

/*!
 * \brief Fetch the next packet out of the receive ring buffer.
 *
 * \return Pointer to an allocated ::NETBUF. If there is no
 *         no data available, then the function returns a
 *         null pointer. If the NIC's buffer seems to be
 *         corrupted, a pointer to 0xFFFF is returned.
 */
static int NicGetPacket(NICINFO * ni, NETBUF ** nbp)
{
    int rc = -1;
    uint16_t fsw;
    uint16_t fbc;

    *nbp = NULL;

    /* Disable NIC interrupts. */
    NutIrqDisable(&NIC_SIGNAL);

    /*
     * Read the status word w/o auto increment. If zero, no packet is
     * available. Otherwise it should be set to one. Any other value
     * indicates a weird chip crying for reset.
     */
    nic_inb(NIC_MRCMDX);
    /* Add some delay befor reading the status of the received packet. */
    _NOP(); _NOP(); _NOP(); _NOP();
    fsw = inb(NIC_DATA_ADDR);
    if (fsw > 1) {
        ni->ni_insane = 1;
    } else if (fsw) {
        /* Now read status word and byte count with auto increment. */
        outb(NIC_BASE_ADDR, NIC_MRCMD);
        if (ni->ni_iomode == NIC_ISR_M16) {
            fsw = inw(NIC_DATA_ADDR);
            _NOP(); _NOP(); _NOP(); _NOP();
            fbc = inw(NIC_DATA_ADDR);
        } else {
            fsw = inb(NIC_DATA_ADDR) + ((uint16_t) inb(NIC_DATA_ADDR) << 8);
            _NOP(); _NOP(); _NOP(); _NOP();
            fbc = inb(NIC_DATA_ADDR) + ((uint16_t) inb(NIC_DATA_ADDR) << 8);
        }

        /*
         * Receiving long packets is unexpected, because we disabled
         * this during initialization. Let's declare the chip insane.
         * Short packets will be handled by the caller.
         */
        if (fbc > 1536) {
            ni->ni_insane = 1;
        } else {
            /*
             * The high byte of the status word contains a copy of the
             * receiver status register.
             */
            fsw >>= 8;
            fsw &= NIC_RSR_ERRORS;
#ifdef NUT_PERMON
            /* Update statistics. */
            if (fsw) {
                if (RxStatus & NIC_RSR_CE) {
                    ni->ni_crc_errors++;
                } else if (RxStatus & NIC_RSR_FOE) {
                    ni->ni_overruns++;
                } else {
                    ni->ni_rx_missed_errors++;
                }
            } else {
                ni->ni_rx_packets++;
            }
#endif
            /*
             * If we got an error packet or failed to allocated the
             * buffer, then silently discard the packet.
             */
            if (fsw || (*nbp = NutNetBufAlloc(0, NBAF_DATALINK, fbc - 4)) == NULL) {
                if (ni->ni_iomode == NIC_ISR_M16) {
                    fbc = (fbc + 1) / 2;
                    while (fbc--) {
                        fsw = inw(NIC_DATA_ADDR);
                    }
                } else {
                    while (fbc--) {
                        fsw = inb(NIC_DATA_ADDR);
                    }
                }
            } else {
                if (ni->ni_iomode == NIC_ISR_M16) {
                    /* Read packet data from 16 bit bus. */
                    NicRead16((*nbp)->nb_dl.vp, (*nbp)->nb_dl.sz);
                    /* Read packet CRC. */
                    fsw = inw(NIC_DATA_ADDR);
                    fsw = inw(NIC_DATA_ADDR);
                } else {
                    /* Read packet data from 8 bit bus. */
                    NicRead8((*nbp)->nb_dl.vp, (*nbp)->nb_dl.sz);
                    /* Read packet CRC. */
                    fsw = inb(NIC_DATA_ADDR);
                    fsw = inb(NIC_DATA_ADDR);
                    fsw = inb(NIC_DATA_ADDR);
                    fsw = inb(NIC_DATA_ADDR);
                }
                /* Return success. */
                rc = 0;
            }
        }
    }

    /* Enable NIC interrupts if the chip is sane. */
    if (ni->ni_insane == 0) {
        NutIrqEnable(&NIC_SIGNAL);
    }
    return rc;
}

/*!
 * \brief Load a packet into the nic's transmit ring buffer.
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
static int NicPutPacket(NICINFO * ni, NETBUF * nb)
{
    int rc = -1;
    uint16_t sz;

    /*
     * Calculate the number of bytes to be send. Do not send packets
     * larger than the Ethernet maximum transfer unit. The MTU
     * consist of 1500 data bytes plus the 14 byte Ethernet header
     * plus 4 bytes CRC. We check the data bytes only.
     */
    if ((sz = nb->nb_nw.sz + nb->nb_tp.sz + nb->nb_ap.sz) > ETHERMTU) {
        return -1;
    }
    sz += nb->nb_dl.sz;
    if (sz & 1) {
        sz++;
    }

    /* Disable interrupts. */
    NutIrqDisable(&NIC_SIGNAL);

    /* TODO: Check for link. */
    if (ni->ni_insane == 0) {
        /* Enable data write. */
        outb(NIC_BASE_ADDR, NIC_MWCMD);

        /* Transfer the Ethernet frame. */
        if (ni->ni_iomode == NIC_ISR_M16) {
            NicWrite16(nb->nb_dl.vp, nb->nb_dl.sz);
            NicWrite16(nb->nb_nw.vp, nb->nb_nw.sz);
            NicWrite16(nb->nb_tp.vp, nb->nb_tp.sz);
            NicWrite16(nb->nb_ap.vp, nb->nb_ap.sz);
        } else {
            NicWrite8(nb->nb_dl.vp, nb->nb_dl.sz);
            NicWrite8(nb->nb_nw.vp, nb->nb_nw.sz);
            NicWrite8(nb->nb_tp.vp, nb->nb_tp.sz);
            NicWrite8(nb->nb_ap.vp, nb->nb_ap.sz);
        }

        /* If no packet is queued, start the transmission. */
        if (ni->ni_tx_queued == 0) {
            nic_outb(NIC_TXPL, (uint8_t) sz);
            nic_outb(NIC_TXPL + 1, (uint8_t) (sz >> 8));
            nic_outb(NIC_TCR, NIC_TCR_TXREQ);
        }
        /* ...otherwise mark this packet queued. */
        else {
            ni->ni_tx_quelen = sz;
        }
        ni->ni_tx_queued++;
        rc = 0;
#ifdef NUT_PERFMON
        ni->ni_tx_packets++;
#endif
    }

    /* Enable interrupts. */
    NutIrqEnable(&NIC_SIGNAL);

    /* If the controller buffer is filled with two packets, then
       wait for the first being sent out. */
    if (rc == 0 && ni->ni_tx_queued > 1) {
        NutEventWait(&ni->ni_tx_rdy, 500);
    }
    return rc;
}

/*!
 * \brief Update the multicast register.
 *
 * \param Network interface controller information.
 */
static void NicUpdateMCHardware(NICINFO *ni)
{
    int i;

    /* Enable broadcast receive. */
    ni->ni_mar[7] |= 0x80;
    
    /* Set multicast address register */
    for (i = 0; i < 7; i++) {
        nic_outb(NIC_MAR + i, ni->ni_mar[i]);
    }
}

/*!
 * \brief Fire up the network interface.
 *
 * NIC interrupts must be disabled when calling this function.
 *
 * \param mac Six byte unique MAC address.
 */
static int NicStart(CONST uint8_t * mac, NICINFO * ni)
{
    int i;
    int link_wait = 20;

    /* Power up the PHY. */
    nic_outb(NIC_GPR, 0);
    NutDelay(5);

    /* Software reset with MAC loopback. */
    nic_outb(NIC_NCR, NIC_NCR_RST | NIC_NCR_LBMAC);
    NutDelay(5);
    nic_outb(NIC_NCR, NIC_NCR_RST | NIC_NCR_LBMAC);
    NutDelay(5);

    /*
     * PHY power down followed by PHY power up. This should activate
     * the auto sense link.
     */
    nic_outb(NIC_GPR, 1);
    nic_outb(NIC_GPR, 0);

    /* Set MAC address. */
    for (i = 0; i < 6; i++) {
        nic_outb(NIC_PAR + i, mac[i]);
    }

    /* Set multicast address register */
    NicUpdateMCHardware(ni);    

    /* Clear interrupts. */
    nic_outb(NIC_ISR, NIC_ISR_ROOS | NIC_ISR_ROS | NIC_ISR_PTS | NIC_ISR_PRS);

    /* Enable late collision retries on the DM9000A. */
    if (nic_inb(NIC_CHIPR) == 0x19) {
        nic_outb(0x2D, 0x40);
    }

    /* Enable receiver. */
    nic_outb(NIC_RCR, NIC_RCR_DIS_LONG | NIC_RCR_DIS_CRC | NIC_RCR_RXEN | NIC_RCR_ALL);

    /* Wait for link. */
    for (link_wait = 20;; link_wait--) {
        if (phy_inw(NIC_PHY_BMSR) & NIC_PHY_BMSR_ANCOMPL) {
            break;
        }
        if (link_wait == 0) {
            return -1;
        }
        NutSleep(200);
    }

    /* Enable interrupts. */
    nic_outb(NIC_IMR, NIC_IMR_PAR | NIC_IMR_PTM | NIC_IMR_PRM);

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
    while (NicStart(ifn->if_mac, ni)) {
        NutSleep(1000);
    }

    /* Initialize the access mutex. */
    NutEventPost(&ni->ni_mutex);

    /* Run at high priority. */
    NutThreadSetPriority(9);

    /* Enable interrupts. */
#ifdef NIC_SIGNAL_XSR
    outr(NIC_SIGNAL_XSR, _BV(NIC_SIGNAL_BIT));
#if defined(ELEKTOR_IR1)
    /* Ugly code alarm: Should be configurable. */
    outr(PMC_PCER, _BV(IRQ0_ID));
#endif
#endif
    outr(NIC_SIGNAL_PDR, _BV(NIC_SIGNAL_BIT));
    NutIrqEnable(&NIC_SIGNAL);
#if defined(ELEKTOR_IR1)
    /* Ugly code alarm: Should be configurable. */
    NutIrqSetMode(&NIC_SIGNAL, NUT_IRQMODE_HIGHLEVEL);
#endif

    for (;;) {
        /*
         * Wait for the arrival of new packets or poll the receiver
         * every two seconds.
         */
        NutEventWait(&ni->ni_rx_rdy, 2000);

        /*
         * Fetch all packets from the NIC's internal buffer and pass
         * them to the registered handler.
         */
        while (NicGetPacket(ni, &nb) == 0) {

            /* Discard short packets. */
            if (nb->nb_dl.sz < 60) {
                NutNetBufFree(nb);
            } else {
                (*ifn->if_recv) (dev, nb);
            }
        }

        /* We got a weird chip, try to restart it. */
        while (ni->ni_insane) {
            if (NicStart(ifn->if_mac, ni) == 0) {
                ni->ni_insane = 0;
                ni->ni_tx_queued = 0;
                ni->ni_tx_quelen = 0;
                NutIrqEnable(&NIC_SIGNAL);
            } else {
                NutSleep(1000);
            }
        }
    }
}

/*!
 * \brief Send Ethernet packet.
 *
 * \param dev Identifies the device to use.
 * \param nb  Network buffer structure containing the packet to be sent.
 *            The structure must have been allocated by a previous
 *            call NutNetBufAlloc().
 *
 * \return 0 on success, -1 in case of any errors.
 */
int DmOutput(NUTDEVICE * dev, NETBUF * nb)
{
    static uint32_t mx_wait = 5000;
    int rc = -1;
    NICINFO *ni = (NICINFO *) dev->dev_dcb;

    /*
     * After initialization we are waiting for a long time to give
     * the PHY a chance to establish an Ethernet link.
     */
    while (rc) {
        if (ni->ni_insane) {
            break;
        }
        if (NutEventWait(&ni->ni_mutex, mx_wait)) {
            break;
        }

        /* Check for packet queue space. */
        if (ni->ni_tx_queued > 1) {
            if (NutEventWait(&ni->ni_tx_rdy, 500)) {
                /* No queue space. Release the lock and give up. */
                NutEventPost(&ni->ni_mutex);
                break;
            }
        } else if (NicPutPacket(ni, nb) == 0) {
            /* Ethernet works. Set a long waiting time in case we
               temporarly lose the link next time. */
            rc = 0;
            mx_wait = 5000;
        }
        NutEventPost(&ni->ni_mutex);
    }
    /*
     * Probably no Ethernet link. Significantly reduce the waiting
     * time, so following transmission will soon return an error.
     */
    if (rc) {
        mx_wait = 500;
    }
    return rc;
}

/*!
 * \brief Initialize Ethernet hardware.
 *
 * Resets the LAN91C111 Ethernet controller, initializes all required
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
int DmInit(NUTDEVICE * dev)
{
    uint32_t id;
    NICINFO *ni = (NICINFO *) dev->dev_dcb;

#if defined(ELEKTOR_IR1)
    
    /* @@MF: "Power on reset" the Ethernet controller */
    outr(PIOC_SODR, _BV(PC17_NANDOE_B));
    outr(PIOC_PER,  _BV(PC17_NANDOE_B));
    outr(PIOC_OER,  _BV(PC17_NANDOE_B));
    outr(PIOC_CODR, _BV(PC17_NANDOE_B));
    NutDelay(WAIT250);
    NutDelay(WAIT250);
    NutDelay(WAIT250);
    NutDelay(WAIT250);
    outr(PIOC_SODR, _BV(PC17_NANDOE_B));
    NutDelay(WAIT250);
    NutDelay(WAIT250);
    NutDelay(WAIT250);
    NutDelay(WAIT250);
    /**************************************************/

    outr(PIOA_BSR, _BV(PA20_NCS2_B));
    outr(PIOA_PDR, _BV(PA20_NCS2_B));
    outr(PIOC_BSR, _BV(PC16_NWAIT_B) | _BV(PC21_NWR0_B) | _BV(PC22_NRD_B));
    outr(PIOC_PDR, _BV(PC16_NWAIT_B) | _BV(PC21_NWR0_B) | _BV(PC22_NRD_B));

    outr(SMC_CSR(2)
        , (1 << SMC_NWS_LSB)
        | SMC_WSEN
        | (2 << SMC_TDF_LSB)
        | SMC_BAT
        | SMC_DBW_16
        | (1 << SMC_RWSETUP_LSB)
        | (1 << SMC_RWHOLD_LSB)
        );
#endif

    /* Probe chip by verifying the identifier registers. */
    id = (uint32_t) nic_inb(NIC_VID);
    id |= (uint32_t) nic_inb(NIC_VID + 1) << 8;
    id |= (uint32_t) nic_inb(NIC_PID) << 16;
    id |= (uint32_t) nic_inb(NIC_PID + 1) << 24;
    if (id != 0x90000A46) {
        return -1;
    }

    /* Reset chip. */
    if (NicReset()) {
        return -1;
    }

    /* Clear NICINFO structure. */
    memset(ni, 0, sizeof(NICINFO));

    /* Determine bus mode. We do not support 32 bit access. */
    ni->ni_iomode = nic_inb(NIC_ISR) & NIC_ISR_IOM;
    if (ni->ni_iomode == NIC_ISR_M32) {
        return -1;
    }

    /* Register interrupt handler. */
    if (NutRegisterIrqHandler(&NIC_SIGNAL, NicInterrupt, dev)) {
        return -1;
    }

    /* Start the receiver thread. */
    if (NutThreadCreate("rxi1", NicRxLanc, dev,
        (NUT_THREAD_NICRXSTACK * NUT_THREAD_STACK_MULT) + NUT_THREAD_STACK_ADD) == NULL) {
        return -1;
    }
    return 0;
}

static int DmIOCtl(NUTDEVICE * dev, int req, void *conf)
{
    int rc = 0;
    int i;
    IFNET *nif = (IFNET *) dev->dev_icb;
    NICINFO *ni = (NICINFO *) dev->dev_dcb;
    uint32_t index;
    uint32_t ip_addr;
    MCASTENTRY *mcast;
    MCASTENTRY *mcast_prev;
    MCASTENTRY *mcast_next;
    
    uint8_t mac[6];

    switch (req) {
        /* Set interface address */ 
        case SIOCSIFADDR:
            /* Set interface hardware address. */
            memcpy(nif->if_mac, conf, sizeof(nif->if_mac));
            break;
    
        /* Add multicast address */
        case SIOCADDMULTI:
            ip_addr = *((uint32_t*)conf);
            
            /* Test if the address is still in the list */
            mcast = nif->if_mcast;
            while(mcast)
            {
                if (ip_addr == mcast->mca_ip)
                {
                    /* The address is still in the list, do nothing */
                    return -1;
                }
                mcast = mcast->mca_next;
            }
           
            /* Create MAC address */ 
            mac[0] = 0x01;
            mac[1] = 0x00;
            mac[2] = 0x5E;
            mac[3] = ((uint8_t*)conf)[1] & 0x7f;
            mac[4] = ((uint8_t*)conf)[2];
            mac[5] = ((uint8_t*)conf)[3];
            
            mcast = malloc(sizeof(MCASTENTRY));
            if (mcast != NULL) {
                /* Calculate MAR index, range 0...63 */
                index  = ether_crc32_le(&mac[0], 6);
                index &= 0x3F;
            
                /* Set multicast bit */            
                ni->ni_mar[index / 8] |= (1 << (index % 8));
                
                /* Add new mcast to the mcast list */
                memcpy(mcast->mca_ha, mac, 6);
                mcast->mca_ip = *((uint32_t*)conf);
                mcast->mca_next = nif->if_mcast;
                nif->if_mcast = mcast;
                
                /* Update the MC hardware */
                NicUpdateMCHardware(ni);
            }
            else {
                rc = -1;
            }    
            break;
        
        /* Delete multicast address */    
        case SIOCDELMULTI:
            ip_addr = *((uint32_t*)conf);

            /* Test if the address is still in the list */
            mcast = nif->if_mcast;
            mcast_prev = mcast;
            while(mcast)
            {
                if (ip_addr == mcast->mca_ip)
                {
                    /* The address is in the list, leave the loop */
                    break;
                }
                mcast_prev = mcast;
                mcast = mcast->mca_next;
            }

            if (NULL == mcast)
            {
                /* The address is not in the list */
                return -1;
            }
            
            /* 
             * Remove the address from the list 
             */
            mcast_next = mcast->mca_next;  
             
            /* Check if the first element must be removed */ 
            if (nif->if_mcast == mcast)
            {
                /* 
                 * The element is the first one 
                 */
                
                /* The first element is now the "next" element */
                nif->if_mcast = mcast_next;
                free(mcast); 
            } else {
                /*
                 * The element is in the middle of the list 
                 */
                 
                /* The next element of the previous is the "next" element */
                mcast_prev->mca_next = mcast_next;
                free(mcast); 
            }
            
            /*
             * Clear the multicast filter, and rebuild it
             */
            
            /* Clear */ 
            for (i = 0; i < 7; i++) {
                ni->ni_mar[i] = 0;
            }
             
            /* Rebuild  */  
            mcast = nif->if_mcast;
            while(mcast)
            {
                /* Calculate MAR index, range 0...63 */
                index  = ether_crc32_le(&mcast->mca_ha[0], 6);
                index &= 0x3F;
            
                /* Set multicast bit */            
                ni->ni_mar[index / 8] |= (1 << (index % 8));
            
                mcast = mcast->mca_next;
            }
            
            /* Update the MC hardware */
            NicUpdateMCHardware(ni);
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
    IFT_ETHER,                  /*!< \brief Interface type, if_type. */
    0,                          /*!< \brief Interface flags, if_flags. */
    {0, 0, 0, 0, 0, 0},         /*!< \brief Hardware net address, if_mac. */
    0,                          /*!< \brief IP address, if_local_ip. */
    0,                          /*!< \brief Remote IP address for point to point, if_remote_ip. */
    0,                          /*!< \brief IP network mask, if_mask. */
    ETHERMTU,                   /*!< \brief Maximum size of a transmission unit, if_mtu. */
    0,                          /*!< \brief Packet identifier, if_pkt_id. */
    0,                          /*!< \brief Linked list of arp entries, arpTable. */
    0,                          /*!< \brief Linked list of multicast address entries, if_mcast. */
    NutEtherInput,              /*!< \brief Routine to pass received data to, if_recv(). */
    DmOutput,                   /*!< \brief Driver output routine, if_send(). */
    NutEtherOutput,             /*!< \brief Media output routine, if_output(). */
    0                           /*!< \brief Interface specific control function. */
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
NUTDEVICE devDM9000E = {
    0,                          /*!< \brief Pointer to next device. */
    {'e', 't', 'h', '0', 0, 0, 0, 0, 0},        /*!< \brief Unique device name. */
    IFTYP_NET,                  /*!< \brief Type of device. */
    0,                          /*!< \brief Base address. */
    0,                          /*!< \brief First interrupt number. */
    &ifn_eth0,                  /*!< \brief Interface control block. */
    &dcb_eth0,                  /*!< \brief Driver control block. */
    DmInit,                     /*!< \brief Driver initialization routine. */
    DmIOCtl,                    /*!< \brief Driver specific control function. */
    0,                          /*!< \brief Read from device. */
    0,                          /*!< \brief Write to device. */
#ifdef __HARVARD_ARCH__
    0,                          /*!< \brief Write from program space data to device. */
#endif
    0,                          /*!< \brief Open a device or file. */
    0,                          /*!< \brief Close a device or file. */
    0                           /*!< \brief Request file size. */
};

#endif /* NIC_BASE_ADDR */

/*@}*/
