/*
 * Copyright (C) 2001-2004 by egnite Software GmbH. All rights reserved.
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
 * Revision 1.1  2005/07/26 18:02:40  haraldkipp
 * Moved from dev.
 *
 * Revision 1.4  2005/04/30 16:42:41  chaac
 * Fixed bug in handling of NUTDEBUG. Added include for cfg/os.h. If NUTDEBUG
 * is defined in NutConf, it will make effect where it is used.
 *
 * Revision 1.3  2005/01/24 21:11:52  freckle
 * renamed NutEventPostFromIRQ into NutEventPostFromIrq
 *
 * Revision 1.2  2005/01/21 16:49:46  freckle
 * Seperated calls to NutEventPostAsync between Threads and IRQs
 *
 * Revision 1.1  2004/03/16 16:48:27  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 */

#include <cfg/os.h>
#include <string.h>

#include <sys/nutconfig.h>

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
#include <dev/nicrtl.h>
#include "rtlregs.h"

#ifdef NUTDEBUG
#include <sys/osdebug.h>
#include <net/netdebug.h>
#endif

/*!
 * \brief Read word from controller DMA port.
 */
#define nic_read_dma() *((volatile u_short*) (base + NIC_IOPORT))

/*!
 * \brief Write word to controller DMA port.
 */
#define nic_write_dma(data) *((u_short*) (base + NIC_IOPORT)) = data

/*!
 * \brief Size of a single ring buffer page.
 */
#define NIC_PAGE_SIZE   0x100

/*!
 * \brief First ring buffer page address.
 */
#define NIC_START_PAGE  0x40

/*!
 * \brief Last ring buffer page address plus 1.
 */
#define NIC_STOP_PAGE   0x60

/*!
 * \brief Number of pages in a single transmit buffer.
 *
 * This should be at least the MTU size.
 */
#define NIC_TX_PAGES    6

/*!
 * \brief Number of transmit buffers.
 */
#define NIC_TX_BUFFERS  2

/*!
 * \brief Controller memory layout:
 *
 * 0x4000 - 0x4bff  3k bytes transmit buffer
 * 0x4c00 - 0x5fff  5k bytes receive buffer
 */
#define NIC_FIRST_TX_PAGE   NIC_START_PAGE
#define NIC_FIRST_RX_PAGE   (NIC_FIRST_TX_PAGE + NIC_TX_PAGES * NIC_TX_BUFFERS)

/*!
 * \brief Standard sizing information
 */
#define TX_PAGES 12             /* Allow for 2 back-to-back frames */

/*!
 * \addtogroup xgNicRtl
 */
/*@{*/

/*!
 * Realtek packet header.
 */
struct nic_pkt_header {
    u_char ph_status;           /*!< \brief Status, contents of RSR register */
    u_char ph_nextpg;           /*!< \brief Page for next packet */
    u_short ph_size;            /*!< \brief Size of header and packet in octets */
};

/*!
 * \brief Reset the Ethernet controller.
 *
 */
static int NicReset(volatile u_char * base)
{
    u_char i;
    u_char j;

    /*
     * Do the software reset by reading from the reset register followed 
     * by writing to the reset register. Wait until the controller enters 
     * the reset state.
     */
    for (j = 0; j < 20; j++) {
        i = nic_read(NIC_RESET);
        NutDelay(WAIT5);
        nic_write(NIC_RESET, i);
        for (i = 0; i < 20; i++) {
            NutDelay(WAIT50);
            /*
             * We got the reset bit. However, Ethernut 1.1 may
             * still fail because the NIC hasn't got it's hardware
             * reset and the data lines remain in tristate. So we
             * read noise instead of the register. To solve this
             * problem, we will verify the NIC's id.
             */
            if ((nic_read(NIC_PG0_ISR) & NIC_ISR_RST) != 0 && nic_read(NIC_PG0_RBCR0) == 0x50 && nic_read(NIC_PG0_RBCR1) == 0x70)
                return 0;
        }
    }
    return -1;
}

/*
 * Fires up the network interface. NIC interrupts
 * should have been disabled when calling this
 * function.
 */
static int NicStart(volatile u_char * base, CONST u_char * mac)
{
    u_char i;

    if (NicReset(base))
        return -1;

    /*
     * Mask all interrupts and clear any interrupt status flag to set the 
     * INT pin back to low.
     */
    nic_write(NIC_PG0_IMR, 0);
    nic_write(NIC_PG0_ISR, 0xff);

    /*
     * During reset the nic loaded its initial configuration from an 
     * external eeprom. On the ethernut board we do not have any 
     * configuration eeprom, but simply tied the eeprom data line to 
     * high level. So we have to clear some bits in the configuration 
     * register. Switch to register page 3.
     */
    nic_write(NIC_CR, NIC_CR_STP | NIC_CR_RD2 | NIC_CR_PS0 | NIC_CR_PS1);

    /*
     * The nic configuration registers are write protected unless both 
     * EEM bits are set to 1.
     */
    nic_write(NIC_PG3_EECR, NIC_EECR_EEM0 | NIC_EECR_EEM1);

    /*
     * Disable sleep and power down.
     */
    nic_write(NIC_PG3_CONFIG3, 0);

    /*
     * Network media had been set to 10Base2 by the virtual EEPROM and
     * will be set now to auto detect. This will initiate a link test.
     * We don't force 10BaseT, because this would disable the link test.
     */
    nic_write(NIC_PG3_CONFIG2, NIC_CONFIG2_BSELB);

    /*
     * Reenable write protection of the nic configuration registers
     * and wait for link test to complete.
     */
    nic_write(NIC_PG3_EECR, 0);
    NutDelay(255);

    /*
     * Switch to register page 0 and set data configuration register
     * to byte-wide DMA transfers, normal operation (no loopback),
     * send command not executed and 8 byte fifo threshold.
     */
    nic_write(NIC_CR, NIC_CR_STP | NIC_CR_RD2);
/*     nic_write(NIC_PG0_DCR, NIC_DCR_LS | NIC_DCR_FT1); */
    nic_write(NIC_PG0_DCR, NIC_DCR_WTS | NIC_DCR_LS | NIC_DCR_FT1);

    /*
     * Clear remote dma byte count register.
     */
    nic_write(NIC_PG0_RBCR0, 0);
    nic_write(NIC_PG0_RBCR1, 0);

    /*
     * Temporarily set receiver to monitor mode and transmitter to 
     * internal loopback mode. Incoming packets will not be stored 
     * in the nic ring buffer and no data will be send to the network.
     */
    nic_write(NIC_PG0_RCR, NIC_RCR_MON);
    nic_write(NIC_PG0_TCR, NIC_TCR_LB0);

    /*
     * Configure the nic's ring buffer page layout.
     * NIC_PG0_BNRY: Last page read.
     * NIC_PG0_PSTART: First page of receiver buffer.
     * NIC_PG0_PSTOP: Last page of receiver buffer.
     */
    nic_write(NIC_PG0_TPSR, NIC_FIRST_TX_PAGE);
    nic_write(NIC_PG0_BNRY, NIC_STOP_PAGE - 1);
    nic_write(NIC_PG0_PSTART, NIC_FIRST_RX_PAGE);
    nic_write(NIC_PG0_PSTOP, NIC_STOP_PAGE);

    /*
     * Once again clear interrupt status register.
     */
    nic_write(NIC_PG0_ISR, 0xff);

    /*
     * Switch to register page 1 and copy our MAC address into the nic. 
     * We are still in stop mode.
     */
    nic_write(NIC_CR, NIC_CR_STP | NIC_CR_RD2 | NIC_CR_PS0);
    for (i = 0; i < 6; i++)
        nic_write(NIC_PG1_PAR0 + i, mac[i]);

    /*
     * Clear multicast filter bits to disable all packets.
     */
    for (i = 0; i < 8; i++)
        nic_write(NIC_PG1_MAR0 + i, 0);

    /*
     * Set current page pointer to one page after the boundary pointer.
     */
    nic_write(NIC_PG1_CURR, NIC_START_PAGE + TX_PAGES);

    /*
     * Switch back to register page 0, remaining in stop mode.
     */
    nic_write(NIC_CR, NIC_CR_STP | NIC_CR_RD2);

    /*
     * Take receiver out of monitor mode and enable it for accepting 
     * broadcasts.
     */
    nic_write(NIC_PG0_RCR, NIC_RCR_AB);

    /*
     * Clear all interrupt status flags and enable interrupts.
     */
    nic_write(NIC_PG0_ISR, 0xff);
    nic_write(NIC_PG0_IMR, NIC_IMR_PRXE | NIC_IMR_PTXE | NIC_IMR_RXEE | NIC_IMR_TXEE | NIC_IMR_OVWE);

    /*
     * Fire up the nic by clearing the stop bit and setting the start bit. 
     * To activate the local receive dma we must also take the nic out of
     * the local loopback mode.
     */
    nic_write(NIC_CR, NIC_CR_STA | NIC_CR_RD2);
    nic_write(NIC_PG0_TCR, 0);

    NutDelay(255);

    return 0;
}

/*!
 * Complete remote DMA.
 */
static void NicCompleteDma(volatile u_char * base)
{
    u_char i;

    /*
     * Complete remote dma.
     */
    nic_write(NIC_CR, NIC_CR_STA | NIC_CR_RD2);

    /*
     * Check that we have a DMA complete flag.
     */
    for (i = 0; i <= 20; i++)
        if (nic_read(NIC_PG0_ISR) & NIC_ISR_RDC)
            break;

    /*
     * Reset remote dma complete flag.
     */
    nic_write(NIC_PG0_ISR, NIC_ISR_RDC);
}

/*!
 * \brief Load a packet into the nic's transmit ring buffer.
 *
 * Interupts must have been disabled when calling this function.
 *
 * \param base NIC hardware base address.
 * \param nb Network buffer structure containing the packet to be sent.
 *           The structure must have been allocated by a previous
 *           call NutNetBufAlloc(). This routine will automatically
 *           release the buffer in case of an error.
 *
 * \return 0 on success, -1 in case of any errors. Errors
 *         will automatically release the network buffer 
 *         structure.
 */
static int NicPutPacket(volatile u_char * base, NETBUF * nb)
{
    u_short sz;
    u_short i;
    u_short *p;
    u_char padding = 0;

    /*
     * Calculate the number of bytes to be send. Do not
     * send packets larger than 1518 bytes.
     */
    sz = nb->nb_dl.sz + nb->nb_nw.sz + nb->nb_tp.sz + nb->nb_ap.sz;
    if (sz > 1518)
        return -1;

    /*
     * The controller will not append pad bytes,
     * so we have to do this.
     */
    if (sz < 60) {
        padding = (u_char) (60 - sz);
        sz = 60;
    }

    /*
     * Set remote dma byte count
     * and start address.
     */
    nic_write(NIC_PG0_RBCR0, sz);
    nic_write(NIC_PG0_RBCR1, sz >> 8);
    nic_write(NIC_PG0_RSAR0, 0);
    nic_write(NIC_PG0_RSAR1, NIC_FIRST_TX_PAGE);

    /*
     * Peform the write.
     */
    nic_write(NIC_CR, NIC_CR_STA | NIC_CR_RD1);

    /*
     * Switch MCU data bus to 16-bit mode
     */
    NicMcu16bitBus();

    /*
     * Transfer the Ethernet frame.
     */
    p = nb->nb_dl.vp;
    for (i = nb->nb_dl.sz >> 1; i; i--)
        nic_write_dma(*p++);

    p = nb->nb_nw.vp;
    for (i = nb->nb_nw.sz >> 1; i; i--)
        nic_write_dma(*p++);

    p = nb->nb_tp.vp;
    for (i = nb->nb_tp.sz >> 1; i; i--)
        nic_write_dma(*p++);

    p = nb->nb_ap.vp;
    for (i = (nb->nb_ap.sz + 1) >> 1; i; i--)
        nic_write_dma(*p++);

    /*
     * Add pad bytes.
     */
    if ((nb->nb_ap.sz & 0x0001) != 0)
        padding--;
    for (i = 0; i < (padding + 1) >> 1; i++)
        nic_write_dma(0);

    /*
     * Switch MCU data bus to 8-bit mode
     */
    NicMcu8bitBus();

    /*
     * Complete remote dma.
     */
    NicCompleteDma(base);

    /*
     * Number of bytes to be transmitted.
     */
    nic_write(NIC_PG0_TBCR0, (sz & 0xff));
    nic_write(NIC_PG0_TBCR1, ((sz >> 8) & 0xff));

    /*
     * First page of packet to be transmitted.
     */
    nic_write(NIC_PG0_TPSR, NIC_FIRST_TX_PAGE);

    /*
     * Start transmission.
     */
    nic_write(NIC_CR, NIC_CR_STA | NIC_CR_TXP | NIC_CR_RD2);

    return 0;
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
static NETBUF *NicGetPacket(volatile u_char * base, u_char dflg)
{
    NETBUF *nb = 0;
    struct nic_pkt_header hdr;
    u_short count;
    u_char nextpg;
    u_char bnry;
    u_char curr;
    u_short i;
    u_short *buf;

    /*
     * Get the current page pointer. It points to the page where the NIC 
     * will start saving the next incoming packet.
     */
    nic_write(NIC_CR, NIC_CR_STA | NIC_CR_RD2 | NIC_CR_PS0);
    curr = nic_read(NIC_PG1_CURR);
    nic_write(NIC_CR, NIC_CR_STA | NIC_CR_RD2);

    /*
     * Get the pointer to the last page we read from. The following page
     * is the one where we start reading. If it's equal to the current
     * page pointer, then there's nothing to read. In this case we return
     * a null pointer.
     */
    if ((bnry = nic_read(NIC_PG0_BNRY) + 1) >= NIC_STOP_PAGE)
        bnry = NIC_FIRST_RX_PAGE;
    if (bnry == curr)
        return 0;

    /*
     * Read the NIC specific packet header.
     */
    nic_write(NIC_PG0_RBCR0, sizeof(struct nic_pkt_header));
    nic_write(NIC_PG0_RBCR1, 0);
    nic_write(NIC_PG0_RSAR0, 0);
    nic_write(NIC_PG0_RSAR1, bnry);
    nic_write(NIC_CR, NIC_CR_STA | NIC_CR_RD0);
    buf = (u_short *) & hdr;
    NicMcu16bitBus();           /* Switch MCU data bus to 16-bit mode */
    *buf++ = nic_read_dma();    /* Read status byte and next page pointer */
    *buf = nic_read_dma();      /* Read frame length */
    NicMcu8bitBus();            /* Switch MCU data bus to 8-bit mode */
    NicCompleteDma(base);

#ifdef __BIG_ENDIAN__
    hdr.ph_size = __byte_swap2(hdr.ph_size);
#endif

    /*
     *  Check packet length.
     */
    if (hdr.ph_size < 60 + sizeof(struct nic_pkt_header) || hdr.ph_size > 1518 + sizeof(struct nic_pkt_header)) {
        return dflg ? 0 : (NETBUF *) - 1;
    }

    /*
     * Calculate the page of the next packet. If it differs from the
     * pointer in the packet header, we discard the whole buffer
     * and return a null pointer.
     */
    nextpg = bnry + (hdr.ph_size >> 8) + ((hdr.ph_size & 0xFF) != 0);
    if (nextpg >= NIC_STOP_PAGE) {
        nextpg -= NIC_STOP_PAGE;
        nextpg += NIC_FIRST_RX_PAGE;
    }
    if (nextpg != hdr.ph_nextpg) {
        u_char nextpg1 = nextpg + 1;
        if (nextpg1 >= NIC_STOP_PAGE) {
            nextpg1 -= NIC_STOP_PAGE;
            nextpg1 += NIC_FIRST_RX_PAGE;
        }
        if (nextpg1 != hdr.ph_nextpg) {
            return dflg ? 0 : (NETBUF *) - 1;
        }
        nextpg = nextpg1;
    }

    /*
     * Check packet status. It should have set bit 0, but
     * even without this bit packets seem to be OK.
     */
    if ((hdr.ph_status & 0x0E) == 0) {
        /*
         * Allocate a NETBUF.
         */
        count = hdr.ph_size - sizeof(struct nic_pkt_header);
        if (dflg == 0) {
            nb = NutNetBufAlloc(0, NBAF_DATALINK, count);
        }

        /*
         * Set remote dma byte count and
         * start address. Don't read the
         * header again.
         */
        nic_write(NIC_PG0_RBCR0, count);
        nic_write(NIC_PG0_RBCR1, count >> 8);
        nic_write(NIC_PG0_RSAR0, sizeof(struct nic_pkt_header));
        nic_write(NIC_PG0_RSAR1, bnry);

        /*
         * Perform the read.
         */
        nic_write(NIC_CR, NIC_CR_STA | NIC_CR_RD0);

        /*
         * Switch MCU data bus to 16-bit mode
         */
        NicMcu16bitBus();

        count = (count + 1) >> 1;
        if (nb) {
            buf = nb->nb_dl.vp;
            for (i = 0; i < count; i++)
                *buf++ = nic_read_dma();
        } else {
            for (i = 0; i < count; i++)
                nic_read_dma();
        }

        /*
         * Switch MCU data bus to 8-bit mode
         */
        NicMcu8bitBus();

        /*
         * Complete remote dma.
         */
        NicCompleteDma(base);
    }

    /*
     * Set boundary register to the last page we read.
     */
    if (--nextpg < NIC_FIRST_RX_PAGE)
        nextpg = NIC_STOP_PAGE - 1;
    nic_write(NIC_PG0_BNRY, nextpg);

    return dflg ? (NETBUF *) ((uptr_t) dflg) : nb;
}

/*
 * When a receiver buffer overflow occurs, the NIC will defer any
 * subsequent action until properly restarted.
 */
static int NicOverflow(volatile u_char * base)
{
    u_char cr;
    u_char resend;

    /*
     * Save the command register, so we can later determine, if NIC
     * transmitter has been interrupted. Then stop the NIC and wait
     * 5 ms for any transmission or reception in progress.
     */
    cr = nic_read(NIC_CR);
    nic_write(NIC_CR, NIC_CR_STP | NIC_CR_RD2);
    NutDelay(WAIT5);

    /*
     * Clear remote byte count register.
     */
    nic_write(NIC_PG0_RBCR0, 0);
    nic_write(NIC_PG0_RBCR1, 0);

    /*
     * Check for any incomplete transmission.
     */
    resend = 0;
    if (cr & NIC_CR_TXP) {
        if ((nic_read(NIC_PG0_ISR) & (NIC_ISR_PTX | NIC_ISR_TXE)) == 0)
            resend = 1;
    }

    /*
     * Enter loopback mode and restart the NIC.
     */
    nic_write(NIC_PG0_TCR, NIC_TCR_LB0);
    nic_write(NIC_CR, NIC_CR_STA | NIC_CR_RD2);

    /*
     * Discard all packets from the receiver buffer.
     */
    while (NicGetPacket(base, 1));

    /*
     * Switch from loopback to normal mode mode.
     */
    nic_write(NIC_PG0_TCR, 0);

    /*
     * Re-invoke any interrupted transmission.
     */
    if (resend) {
        nic_write(NIC_CR, NIC_CR_STA | NIC_CR_TXP | NIC_CR_RD2);
    }

    /* Finally clear the overflow flag. */
    nic_write(NIC_PG0_ISR, NIC_ISR_OVW);
    return resend;
}

/*
 * NIC interrupt entry.
 */
static void NicInterrupt(void *arg)
{
    u_char isr;
    volatile u_char *base = (u_char *) (((NUTDEVICE *) arg)->dev_base);
    NICINFO *ni = (NICINFO *) ((NUTDEVICE *) arg)->dev_dcb;

    ni->ni_interrupts++;

    isr = nic_read(NIC_PG0_ISR);
    nic_write(NIC_PG0_ISR, isr);

    /*
     * Recover from receive buffer overflow. This may take some
     * time, so we enable global interrupts but keep NIC
     * interrupts disabled.
     */
    if (isr & NIC_ISR_OVW) {
        NicDisableInt();
        NutEnableInt();
        ni->ni_rx_pending++;
        if (NicOverflow(base))
            ni->ni_tx_bsy++;
        else {
            NutEventPostFromIrq(&ni->ni_tx_rdy);
        }
        ni->ni_overruns++;
        NutDisableInt();
        NicEnableInt();
    } else {
        /*
         * If this is a transmit interrupt, then a packet has been sent. 
         * So we can clear the transmitter busy flag and wake up the 
         * transmitter thread.
         */
        if (isr & (NIC_ISR_PTX | NIC_ISR_TXE)) {
            ni->ni_tx_bsy = 0;
            NutEventPostFromIrq(&ni->ni_tx_rdy);
        }

        /*
         * If this is a receive interrupt, then wake up the receiver 
         * thread.
         */
        if (isr & NIC_ISR_PRX) {
            ni->ni_rx_pending++;
            NutEventPostFromIrq(&ni->ni_rx_rdy);
        }
        if (isr & NIC_ISR_RXE) {
            ni->ni_rx_frame_errors += nic_read(NIC_PG0_CNTR0);
            ni->ni_rx_crc_errors += nic_read(NIC_PG0_CNTR1);
            ni->ni_rx_missed_errors += nic_read(NIC_PG0_CNTR2);
        }
    }
}

/*! \fn NicRx(void *arg)
 * \brief NIC receiver thread.
 *
 * 
 * It runs with high priority.
 */
THREAD(NicRx, arg)
{
    NUTDEVICE *dev;
    IFNET *ifn;
    NICINFO *ni;
    NETBUF *nb;
    u_char rlcnt;

    dev = arg;
    ifn = (IFNET *) dev->dev_icb;
    ni = (NICINFO *) dev->dev_dcb;

    NutThreadSetPriority(9);
    /*
     * This is a temporary hack. Due to a change in initialization,
     * we may not have got a MAC address yet. Wait until one has been
     * set.
     */
    if ((ifn->if_mac[0] & ifn->if_mac[1] & ifn->if_mac[2]) == 0xFF) {
        while ((ifn->if_mac[0] & ifn->if_mac[1] & ifn->if_mac[2]) == 0xFF)
            NutSleep(125);
        NicDisableInt();
        NicStart((u_char *) (dev->dev_base), ifn->if_mac);
        ni->ni_curr_page = NIC_START_PAGE + TX_PAGES;
        NicEnableInt();
    }

    for (;;) {

        /*
         * Wait for the arrival of new packets or check
         * the receiver every two second.
         */
        if (ni->ni_rx_pending > 10) {
            NicDisableInt();
            if (NicStart((u_char *) (dev->dev_base), ifn->if_mac) == 0)
                ni->ni_rx_pending = 0;
            ni->ni_curr_page = NIC_START_PAGE + TX_PAGES;
            NicEnableInt();
        }
        NutEventWait(&ni->ni_rx_rdy, 2000);

        /*
         * Fetch all packets from the NIC's internal
         * buffer and pass them to the registered handler.
         */
        rlcnt = 0;
        NicDisableInt();
        while (rlcnt++ < 10) {
            if ((nb = NicGetPacket((u_char *) (dev->dev_base), 0)) == 0)
                break;
            /* The sanity check may fail because the controller is too busy.
               try another read before giving up and restarting the NIC. */
            if (nb == (void *) -1) {
                if ((nb = NicGetPacket((u_char *) (dev->dev_base), 0)) == 0)
                    break;
            }
            if (nb == (void *) -1) {
                if (NicStart((u_char *) (dev->dev_base), ifn->if_mac) == 0)
                    ni->ni_rx_pending = 0;
                ni->ni_curr_page = NIC_START_PAGE + TX_PAGES;
                ni->ni_rx_pending = 0;
            } else {
                ni->ni_rx_pending = 0;
                ni->ni_rx_packets++;
                NicEnableInt();
                (*ifn->if_recv) (dev, nb);
                NicDisableInt();
            }
        }
        NicEnableInt();
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
int NicOutput(NUTDEVICE * dev, NETBUF * nb)
{
    int rc = -1;
    NICINFO *ni;
    u_char retries = 10;
    u_char sigmod = 0;

    ni = (NICINFO *) dev->dev_dcb;

    while (ni->ni_tx_bsy && retries--) {
        if (NutEventWait(&ni->ni_tx_rdy, 200)) {
            volatile u_char *base = (u_char *) (dev->dev_base);

            /*
             * If hanging around here too long, there's something wrong 
             * with the transmit interrupt. Force sending the packet, 
             * if the transmitter has become inactive.
             */
            if (NicIntIsEnabled()) {
                NicDisableInt();
                sigmod = 1;
            }
            if ((nic_read(NIC_CR) & NIC_CR_TXP) == 0)
                ni->ni_tx_bsy = 0;
            if (sigmod) {
                NicEnableInt();
                sigmod = 0;
            }
        }
    }

    if (NicIntIsEnabled()) {
        NicDisableInt();
        sigmod = 1;
    }
    if (ni->ni_tx_bsy == 0) {
        ni->ni_tx_bsy++;
        if (NicPutPacket((u_char *) (dev->dev_base), nb) == 0) {
            ni->ni_tx_packets++;
            rc = 0;
        }
    }
    if (sigmod)
        NicEnableInt();

    return rc;
}

/*!
 * \brief Initialize Ethernet hardware.
 *
 * Resets RTL8019AS Ethernet controller, initializes all required 
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
int NicInit(NUTDEVICE * dev)
{
    volatile u_char *base;
    IFNET *ifn;
    NICINFO *ni;

    /*
     * We need to know our MAC address. If no configuration is 
     * available, load it now.
     */
    if (confnet.cd_size == 0)
        NutNetLoadConfig(dev->dev_name);

    ifn = dev->dev_icb;
    memcpy(ifn->if_mac, confnet.cdn_mac, 6);
    ni = (NICINFO *) dev->dev_dcb;
    memset(ni, 0, sizeof(NICINFO));
    base = (u_char *) (dev->dev_base);

    NicDisableInt();
    NutInitSysIrq();

    if (ifn->if_mac[0] | ifn->if_mac[1] | ifn->if_mac[2])
        if (NicStart(base, ifn->if_mac))
            return -1;
    ni->ni_curr_page = NIC_START_PAGE + TX_PAGES;

    /*
     * Start the receiver thread.
     */
    NutThreadCreate("rxi0", NicRx, dev, 640);
    NutSleep(WAIT500);

    /*
     * Register interrupt handler and enable interrupts.
     */
    if (NutRegisterIrqHandler(&RTL_SIGNAL, NicInterrupt, dev))
        return -1;
    NicEnableInt();

    return 0;
}

/*!
 * \brief Network interface device control block structure.
 *
 * Used to call.
 */
static NICINFO dcb_eth0rtl;

/*!
 * \brief Network interface information structure.
 *
 * Used to call.
 */
static IFNET ifn_eth0rtl = {
    IFT_ETHER,                  /*!< \brief Interface type. */
    {0, 0, 0, 0, 0, 0},         /*!< \brief Hardware net address. */
    0,                          /*!< \brief IP address. */
    0,                          /*!< \brief Remote IP address for point to point. */
    0,                          /*!< \brief IP network mask. */
    ETHERMTU,                   /*!< \brief Maximum size of a transmission unit. */
    0,                          /*!< \brief Packet identifier. */
    0,                          /*!< \brief Linked list of arp entries. */
    NutEtherInput,              /*!< \brief Routine to pass received data to, if_recv(). */
    NicOutput,                  /*!< \brief Driver output routine, if_send(). */
    NutEtherOutput              /*!< \brief Media output routine, if_output(). */
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
NUTDEVICE devEth0 = {
    0,                          /* Pointer to next device. */
    {'e', 't', 'h', '0', 0, 0, 0, 0, 0},        /* Unique device name. */
    IFTYP_NET,                  /* Type of device. */
    0,                          /* Base address. */
    0,                          /* First interrupt number. */
    &ifn_eth0rtl,               /* Interface control block. */
    &dcb_eth0rtl,               /* Driver control block. */
    NicInit,                    /* Driver initialization routine. */
    0,                          /* Driver specific control function. */
    0,                          /* Read from device. */
    0,                          /* Write to device. */
    /* Write from program space data to device. */
    0,                          /* Open a device or file. */
    0,                          /* Close a device or file. */
    0                           /* Request file size. */
};

/*@}*/
