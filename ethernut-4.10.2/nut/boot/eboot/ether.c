/*
 * Copyright (C) 2001-2004 by egnite Software GmbH
 * Copyright (C) 2010 by egnite GmbH
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
 * -
 * Portions Copyright (c) 1993 by Digital Equipment Corporation.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies, and that
 * the name of Digital Equipment Corporation not be used in advertising or
 * publicity pertaining to distribution of the document or software without
 * specific, written prior permission.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND DIGITAL EQUIPMENT CORP. DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS.   IN NO EVENT SHALL DIGITAL EQUIPMENT
 * CORPORATION BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

/*
 * $Id: ether.c 3006 2010-05-10 12:39:44Z haraldkipp $
 */

#include <string.h>
#include <avr/io.h>

#include "config.h"
#include "rtlregs.h"
#include "util.h"
#include "eboot.h"
#include "arp.h"
#include "ether.h"

/*!
 * \addtogroup xgStack
 */
/*@{*/

#define NIC_PAGE_SIZE       0x100
#define NIC_START_PAGE      0x40
#define NIC_STOP_PAGE       0x60
#define NIC_TX_PAGES        6
#define NIC_TX_BUFFERS      2
#define NIC_FIRST_TX_PAGE   NIC_START_PAGE
#define NIC_FIRST_RX_PAGE   (NIC_FIRST_TX_PAGE + NIC_TX_PAGES * NIC_TX_BUFFERS)
#define TX_PAGES            12

unsigned char EEPROM_read(unsigned int uiAddress)
{
    /* Wait for completion of previous write */
#if defined(__AVR_ATmega2561__)
    while(EECR & (1<<EEPE));
#else
    while(EECR & (1<<EEWE));
#endif
    /* Set up address register */
    EEAR = uiAddress;
    /* Start eeprom read by writing EERE */
    EECR |= (1<<EERE);
    /* Return data from data register */
    return EEDR;
}

/*!
 * Realtek packet header.
 */
struct nic_pkt_header {
    u_char ph_status;           /*!< \brief Status, contents of RSR register */
    u_char ph_nextpg;           /*!< \brief Page for next packet */
    u_short ph_size;            /*!< \brief Size of header and packet in octets */
};

static void EmulateNicEeprom(void)
{
    register u_int cnt;

    /*
     * Prepare the EEPROM emulation port bits. Configure the EEDO
     * and the EEMU lines as outputs and set both lines to high.
     */
    PORTC = 0xC0;
    DDRC = 0xC0;
    Delay(20);

    /*
     * Force the chip to re-read the EEPROM contents.
     */
    NIC_CR = NIC_CR_STP | NIC_CR_RD2 | NIC_CR_PS0 | NIC_CR_PS1;
    NIC_PG3_EECR = NIC_EECR_EEM0;

    /*
     * No external memory access beyond this point.
     */
#if defined(__AVR_ATmega2561__) || defined(__AVR_AT90CAN128__)
    XMCRA &= ~_BV(SRE);
#else
    MCUCR &= ~_BV(SRE);
#endif

    /*
     * Loop until the chip stops toggling our EESK input.
     */
    do {
        cnt = 0;
        if(bit_is_set(PINC, 5)) {
            while(++cnt && bit_is_set(PINC, 5));
        }
        else {
            while(++cnt && bit_is_clear(PINC, 5));
        }
    } while(cnt);

    /*
     * Enable memory interface.
     */
#if defined(__AVR_ATmega2561__) || defined(__AVR_AT90CAN128__)
    XMCRA |= _BV(SRE);
#else
    MCUCR |= _BV(SRE);
#endif

    /* Reset port outputs to default. */
    PORTC = 0x00;
    DDRC = 0x00;

    /* Wait until controller ready. */
    while(NIC_CR != (NIC_CR_STP | NIC_CR_RD2));
}


/*!
 * \brief Initialize the NIC.
 *
 * For further description of the initialization
 * please refer to the original Ethernut code.
 */
int NicInit(void)
{
    u_char c;

    /*
     * Enable external data and address
     * bus.
     */
#if defined(__AVR_ATmega2561__) || defined(__AVR_AT90CAN128__)
    XMCRA = (_BV(SRE) | _BV(SRW10));
#else
    MCUCR = (_BV(SRE) | _BV(SRW));
#endif

    c = NIC_RESET;
    Delay(5);
    NIC_RESET = c;
    Delay(1000);
    EmulateNicEeprom();

    NIC_PG0_IMR = 0;
    NIC_PG0_ISR = 0xff;
    NIC_CR = NIC_CR_STP | NIC_CR_RD2 | NIC_CR_PS0 | NIC_CR_PS1;
    NIC_PG3_EECR = NIC_EECR_EEM0 | NIC_EECR_EEM1;
    NIC_PG3_CONFIG3 = 0;
    NIC_PG3_CONFIG2 = NIC_CONFIG2_BSELB;
    NIC_PG3_EECR = 0;
    Delay(200);
    NIC_CR = NIC_CR_STP | NIC_CR_RD2;
    NIC_PG0_DCR = NIC_DCR_LS | NIC_DCR_FT1;
    NIC_PG0_RBCR0 = 0;
    NIC_PG0_RBCR1 = 0;
    NIC_PG0_RCR = NIC_RCR_MON;
    NIC_PG0_TCR = NIC_TCR_LB0;
    NIC_PG0_TPSR = NIC_FIRST_TX_PAGE;
    NIC_PG0_BNRY = NIC_STOP_PAGE - 1;
    NIC_PG0_PSTART = NIC_FIRST_RX_PAGE;
    NIC_PG0_PSTOP = NIC_STOP_PAGE;
    NIC_PG0_ISR = 0xff;
    NIC_CR = NIC_CR_STP | NIC_CR_RD2 | NIC_CR_PS0;

    NIC_PG1_PAR0 = confnet.cdn_mac[0];
    NIC_PG1_PAR1 = confnet.cdn_mac[1];
    NIC_PG1_PAR2 = confnet.cdn_mac[2];
    NIC_PG1_PAR3 = confnet.cdn_mac[3];
    NIC_PG1_PAR4 = confnet.cdn_mac[4];
    NIC_PG1_PAR5 = confnet.cdn_mac[5];

    NIC_PG1_MAR0 = 0;
    NIC_PG1_MAR1 = 0;
    NIC_PG1_MAR2 = 0;
    NIC_PG1_MAR3 = 0;
    NIC_PG1_MAR4 = 0;
    NIC_PG1_MAR5 = 0;
    NIC_PG1_MAR6 = 0;
    NIC_PG1_MAR7 = 0;

    NIC_PG1_CURR = NIC_START_PAGE + TX_PAGES;
    NIC_CR = NIC_CR_STP | NIC_CR_RD2;
    NIC_PG0_RCR = NIC_RCR_AB;
    NIC_PG0_ISR = 0xff;
    NIC_PG0_IMR = 0;
    NIC_CR = NIC_CR_STA | NIC_CR_RD2;
    NIC_PG0_TCR = 0;
    Delay(1000);

    return 0;
}

/*!
 * \brief Send an Ethernet frame.
 *
 * \param dmac Destination MAC address.
 * \param type Frame type.
 * \param len  Frame size.
 *
 * \return 0 on success, -1 otherwise.
 */
int EtherOutput(u_char * dmac, u_short type, u_short len)
{
    u_short i;
    u_short sz;
    u_char *cp;
    ETHERHDR *eh;

    if (type == ETHERTYPE_ARP) {
        eh = &arpframe.eth_hdr;
        cp = (u_char *) & arpframe;
    } else {
        eh = &sframe.eth_hdr;
        cp = (u_char *) & sframe;
    }

    for (i = 0; i < 6; i++)
        eh->ether_shost[i] = confnet.cdn_mac[i];
    if (dmac) {
        for (i = 0; i < 6; i++)
            eh->ether_dhost[i] = dmac[i];
    } else {
        for (i = 0; i < 6; i++)
            eh->ether_dhost[i] = 0xFF;
    }
    eh->ether_type = type;
    if ((len += sizeof(ETHERHDR)) < 60)
        sz = 60;
    else
        sz = len;

    NIC_CR = NIC_CR_STA | NIC_CR_RD2;
    NIC_PG0_RBCR0 = (u_char) sz;
    NIC_PG0_RBCR1 = (u_char) (sz >> 8);
    NIC_PG0_RSAR0 = 0;
    NIC_PG0_RSAR1 = NIC_FIRST_TX_PAGE;

    NIC_CR = NIC_CR_STA | NIC_CR_RD1;

    /*
     * Transfer frame.
     */
    for (i = 0; i < len; i++)
        NIC_IOPORT = *cp++;
    for (i = len; i < sz; i++)
        NIC_IOPORT = 0;

    /*
     * Complete remote dma.
     */
    NIC_CR = NIC_CR_STA | NIC_CR_RD2;
    for (i = 0; i <= 20; i++)
        if (NIC_PG0_ISR & NIC_ISR_RDC)
            break;
    NIC_PG0_ISR = NIC_ISR_RDC;

    /*
     * Number of bytes to be transmitted.
     */
    NIC_PG0_TBCR0 = (sz & 0xff);
    NIC_PG0_TBCR1 = ((sz >> 8) & 0xff);

    /*
     * First page of packet to be transmitted.
     */
    NIC_PG0_TPSR = NIC_FIRST_TX_PAGE;

    /*
     * Start transmission.
     */
    NIC_CR = NIC_CR_STA | NIC_CR_TXP | NIC_CR_RD2;

    /*
     * Wait until transmission is completed or aborted.
     */

    while (NIC_CR & NIC_CR_TXP);

    return 0;
}

/*!
 * \brief Receive an Ethernet frame.
 *
 * \param tms  Return with timeout after the specified
 *             number of waiting loops. On a 14 Mhz ATmega
 *             this value represents approximately the number
 *             of milliseconds to wait.
 *
 * \return The number of bytes received, 0 on timeout 
 *         or -1 in case of a failure.
 */
int EtherInput(u_short type, u_short tms)
{
    int rc = 0;
    u_short maxl = 0;
    u_char isr = 0;
    struct nic_pkt_header hdr;
    u_char *buf;
    u_char bnry;
    u_char curr;
    u_short i;
    u_char wtc;

    while (tms && rc == 0) {

        /*
         * Wait until frame received, timeout or receiver error.
         */
        for (wtc = 1; tms; wtc++) {
            isr = NIC_PG0_ISR;
            if (isr & NIC_ISR_RXE) {
                return -1;
            }
            if (isr & NIC_ISR_PRX) {
                break;
            }
            if (wtc == 0)
                tms--;
        }
        if ((isr & NIC_ISR_PRX) == 0) {
            NIC_PG0_ISR = isr;
            break;
        }

        /*
         * Get the current page pointer. It points to the page where the NIC 
         * will start saving the next incoming packet.
         */
        NIC_CR = NIC_CR_STA | NIC_CR_RD2 | NIC_CR_PS0;
        curr = NIC_PG1_CURR;
        NIC_CR = NIC_CR_STA | NIC_CR_RD2;

        /*
         * Get the pointer to the last page we read from. The following page
         * is the one where we start reading. If it's equal to the current
         * page pointer, then there's nothing to read.
         */
        if ((bnry = NIC_PG0_BNRY + 1) >= NIC_STOP_PAGE)
            bnry = NIC_FIRST_RX_PAGE;
        if (bnry == curr) {
            NIC_PG0_ISR = isr;
            continue;
        }

        /*
         * Read the NIC specific packet header.
         */
        NIC_PG0_RBCR0 = sizeof(struct nic_pkt_header);
        NIC_PG0_RBCR1 = 0;
        NIC_PG0_RSAR0 = 0;
        NIC_PG0_RSAR1 = bnry;
        buf = (u_char *) & hdr;
        NIC_CR = NIC_CR_STA | NIC_CR_RD0;
        for (i = 0; i < sizeof(struct nic_pkt_header); i++)
            *buf++ = NIC_IOPORT;

        /*
         * Complete remote dma.
         */
        NIC_CR = NIC_CR_STA | NIC_CR_RD2;
        for (i = 0; i <= 20; i++) {
            if (NIC_PG0_ISR & NIC_ISR_RDC)
                break;
        }
        NIC_PG0_ISR = NIC_ISR_RDC;

        /*
         * Check frame length. If it's outside limits, we assume
         * that the NIC is corrupted. Reset and retry.
         */
        if (hdr.ph_size < 60 + sizeof(struct nic_pkt_header) || hdr.ph_size > 1518 + sizeof(struct nic_pkt_header)) {
            NicInit();
            continue;
        }

        /*
         * Check packet status.
         */
        if ((hdr.ph_status & 0x0F) == 1) {
            rc = hdr.ph_size - sizeof(struct nic_pkt_header);

            /*
             * Set remote dma byte count and
             * start address. Don't read the
             * header again.
             */
            NIC_PG0_RBCR0 = (u_char) rc;
            NIC_PG0_RBCR1 = (u_char) ((u_short) rc >> 8);
            NIC_PG0_RSAR0 = sizeof(struct nic_pkt_header);
            NIC_PG0_RSAR1 = bnry;

            /*
               * Perform the read.
             */
            NIC_CR = NIC_CR_STA | NIC_CR_RD0;
            buf = (u_char *) & rframe;
            maxl = sizeof(rframe) < (u_short)rc ? sizeof(rframe) : (u_short)rc;
            for (i = 0; i < maxl; i++) {
                *buf++ = NIC_IOPORT;
            }
            for (; i < (u_short) rc; i++)
                NIC_IOPORT;

            /*
             * Complete remote dma.
             */
            NIC_CR = NIC_CR_STA | NIC_CR_RD2;
            for (i = 0; i <= 20; i++) {
                if (NIC_PG0_ISR & NIC_ISR_RDC)
                    break;
            }
            NIC_PG0_ISR = NIC_ISR_RDC;
        }

        /*
         * Set boundary register to the last page we read.
         */
        if (--hdr.ph_nextpg < NIC_FIRST_RX_PAGE)
            hdr.ph_nextpg = NIC_STOP_PAGE - 1;
        NIC_PG0_BNRY = hdr.ph_nextpg;

        /*
         * Handle incoming ARP requests.
         */
        if (rframe.eth_hdr.ether_type != type) {
            if (rframe.eth_hdr.ether_type == ETHERTYPE_ARP)
                ArpRespond();
            rc = 0;
        }
    }
    return rc;
}

/*@}*/
