/*
 * Copyright (C) 2002-2003 by Call Direct Cellular Solutions Pty. Ltd. All rights reserved.
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
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgement:
 *
 *    This product includes software developed by Call Direct Cellular Solutions Pty. Ltd.
 *    and its contributors.
 *
 * THIS SOFTWARE IS PROVIDED BY CALL DIRECT CELLULAR SOLUTIONS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL CALL DIRECT
 * CELLULAR SOLUTIONS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * For additional information see http://www.calldirect.com.au/
 *
 * -
 * Copyright (C) 2001-2003 by egnite Software GmbH. All rights reserved.
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
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgement:
 *
 *    This product includes software developed by egnite Software GmbH
 *    and its contributors.
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
 * -
 * Portions Copyright (C) 2000 David J. Hudson <dave@humbug.demon.co.uk>
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can redistribute this file and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software Foundation;
 * either version 2 of the License, or (at your discretion) any later version.
 * See the accompanying file "copying-gpl.txt" for more details.
 *
 * As a special exception to the GPL, permission is granted for additional
 * uses of the text contained in this file.  See the accompanying file
 * "copying-liquorice.txt" for details.
 * -
 * Portions Copyright (c) 1983, 1993 by
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
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
 * $Log$
 * Revision 1.4  2009/01/17 11:26:37  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.3  2008/08/11 06:59:14  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.2  2006/10/08 16:48:07  haraldkipp
 * Documentation fixed
 *
 * Revision 1.1  2005/07/26 18:02:27  haraldkipp
 * Moved from dev.
 *
 * Revision 1.9  2005/04/30 16:42:41  chaac
 * Fixed bug in handling of NUTDEBUG. Added include for cfg/os.h. If NUTDEBUG
 * is defined in NutConf, it will make effect where it is used.
 *
 * Revision 1.8  2004/05/26 09:40:30  olereinhardt
 * Changed reading of packet length / receive status to be compatible with
 * newer AVRGCC versions. (Need to read high byte first!)
 *
 * Added software reset / wakeup routine to init code. (only avalilable in new code)
 *
 * Revision 1.7  2004/05/25 11:39:47  olereinhardt
 * Define NUT_CS8900_OLD to get the old functionality back again
 *
 * Revision 1.6  2004/05/24 17:09:17  olereinhardt
 * Changed base address handling in cs8900.c and moved cs8900.h to /include/dev
 * Base address can now be passed to the nic driver by NutRegisterDevice.
 * Removed some Assembler code in cs8900.c
 *
 * Added some databus waitstate settings for the upper half of the address space in os/arch/avr_nutinit.c. Now three waitstates are default for 0x8000-0xFFFF
 *
 * Added terminal device driver for hd44780 compatible LCD displays directly
 * connected to the memory bus (memory mapped). See hd44780.c for more information.
 * Therefore some minor changed in include/dev/term.h and dev/term.c are needet to
 * pass a base address to the lcd driver.
 *
 * Revision 1.5  2004/03/18 14:06:52  haraldkipp
 * Deprecated header file replaced
 *
 * Revision 1.4  2003/10/13 10:13:49  haraldkipp
 * First release
 *
 * Revision 1.3  2003/08/05 20:11:30  haraldkipp
 * Removed from ICCAVR compilation
 *
 * Revision 1.2  2003/07/20 20:07:38  haraldkipp
 * Conflicting Ethernet driver routine names solved.
 *
 * Revision 1.1  2003/07/20 16:37:21  haraldkipp
 * CrystalTek 8900A driver added.
 *
 *
 * Revision 1.0  2002/03/28 MJC CDCS
 * Created
 *
 * Revision 1.1  2003/03/25 MJC CDCS
 * Modified behaviour when transmit buffer space unavailable
 *
 */

/* Not ported. */
#ifdef __GNUC__

#include <cfg/os.h>
#include <string.h>
#include <avr/interrupt.h>

#include <sys/heap.h>
#include <sys/thread.h>
#include <sys/event.h>
#include <sys/timer.h>
#include <sys/confnet.h>

#include <dev/nicrtl.h>
#include <netinet/if_ether.h>
#include <net/ether.h>
#include <net/if_var.h>
#include <netinet/ip.h>

#include <dev/irqreg.h>
#include "cs8900.h"

#ifdef NUTDEBUG
#include <sys/osdebug.h>
#include <net/netdebug.h>
#endif

/*!
 * \addtogroup xgNicCs8900
 */
/*@{*/


// Ethernet flags byte
// Bit 0 = transmit byte flag
uint8_t cs_flags;
volatile uint16_t cs_base = 0x0000;


void CSWrite16(uint16_t addr, uint16_t data)
{
    uint16_t *p;

    p = (uint16_t *) addr;
    cli();
    *p = data;
    sei();
}

void CSWritePP16(uint16_t addr, uint16_t data)
{
    uint16_t *p;

    cli();
    p = (uint16_t *) CS_PP_PTR;
    *p = addr;

    CSWrite16(CS_PP_DATA0, data);

    return;
}

uint16_t CSRead16(uint16_t addr)
{
    uint16_t *p;
    uint16_t d;

    cli();
    p = (uint16_t *) addr;
    d = *p;
    sei();

    return d;
}

uint16_t CSReadPP16(uint16_t addr)
{
    uint16_t *p;

    cli();
    p = (uint16_t *) CS_PP_PTR;
    *p = addr;

    return CSRead16(CS_PP_DATA0);
}

uint32_t CSReadPP32(unsigned int addr)
{
    uint32_t l;
    uint32_t *p;

    cli();
    p = (uint32_t *) CS_PP_PTR;
    *p = addr;
    p = (uint32_t *) CS_PP_DATA0;
    l = *p;
    sei();

    return l;
}



void CSBeginFrame(void)
{
    cs_flags &= ~1;
}

void CSEndFrame(void)
{
    uint8_t *p;

    cli();
    p = (uint8_t *) CS_DATA_P0 + 1;
    sei();

    // If odd number of bytes in packet pad it out
    if (cs_flags & 1)
        p = 0;
}

void CSWriteFrameByte(uint8_t data)
{
    uint8_t *p;

    if (cs_flags & 1)
        p = (uint8_t *) CS_DATA_P0 + 1;
    else
        p = (uint8_t *) CS_DATA_P0;

    *p = data;
    cs_flags ^= 1;
}

static int CSEthPutPacket(NUTDEVICE * dev, NETBUF * nb)
{
    uint16_t i;
    uint16_t sz;
    uint8_t *p;
    NICINFO *ni;

    ni = (NICINFO *) dev->dev_dcb;

    //
    // Calculate the number of bytes to be send. Do not
    // send packets larger than 1536 bytes.
    //
    sz = nb->nb_dl.sz + nb->nb_nw.sz + nb->nb_tp.sz + nb->nb_ap.sz;
    if (sz >= 0x600) {
        NutNetBufFree(nb);
        return -1;
    }
#if 0
    if (tcp_trace) {
        NutPrintFormat_P(dev_debug, PSTR("[ETHTX-%u]\r\n"), sz);
        NutPrintFlush(dev_debug);
    }
#endif

    // Start transmission after entire frame is loaded into CS8900
    CSWrite16(CS_TX_CMD_I, 0xC0);
    // Set frame size
    CSWrite16(CS_TX_LEN_I, sz);

    // Wait for buffer space, but only for a while (200ms)
    // If the cable is disconnected this will never become true
    // If we don't get the go ahead within 200ms return 0 (Sucess)
    // And let the upper layers deal with re-transmission 
    // If we return failure TCP sockets will close straight away which probably
    // isn't the correct behaviour
    i = 0;
    while ((CSReadPP16(CS_BUS_STAT) & 0x0100) == 0) {
        i++;
        if (i > 20)
            return 0;
        NutSleep(10);
    }

    //
    // Transfer ethernet physical header.
    //
    CSBeginFrame();

    p = nb->nb_dl.vp;
    for (i = 0; i < nb->nb_dl.sz; i++) {
        CSWriteFrameByte(*p++);
    }


    p = nb->nb_nw.vp;
    for (i = 0; i < nb->nb_nw.sz; i++) {
        CSWriteFrameByte(*p++);
    }

    p = nb->nb_tp.vp;
    for (i = 0; i < nb->nb_tp.sz; i++) {
        CSWriteFrameByte(*p++);
    }

    p = nb->nb_ap.vp;
    for (i = 0; i < nb->nb_ap.sz; i++) {
        CSWriteFrameByte(*p++);
    }

    CSEndFrame();

    return 0;
}

/*!
 * \brief Send Ethernet packet.
 *
 * \param dev   Identifies the device to use.
 * \param nb    Network buffer structure containing the packet to be sent.
 *              The structure must have been allocated by a previous
 *              call NutNetBufAlloc(). This buffer will be automatically
 *              released in case of an error.
 * 
 * \return 0 on success, -1 in case of any errors. Errors
 *         will automatically release the network buffer 
 *         structure.
 */

int CSNicOutput(NUTDEVICE * dev, NETBUF * nb)
{
    int rc = -1;
    NICINFO *ni;

    ni = (NICINFO *) dev->dev_dcb;

#if 0
    if (tcp_trace) {
        NutPrintFormat_P(dev_debug, PSTR("Enter EthOutput\r\n"));
        NutPrintFlush(dev_debug);
    }
#endif

    if ((rc = CSEthPutPacket(dev, nb)) == 0)
        ni->ni_tx_packets++;

    return rc;
}





/*! \fn CSNICrx(void *arg)
 * \brief NIC receiver thread.
 *
 * 
 * It runs with high priority.
 */

THREAD(CSNICrx, arg)
{
    NUTDEVICE *dev;
    IFNET *ifn;
    NICINFO *ni;
    NETBUF *nb;
    uint8_t *p;
    uint8_t *q;
    uint16_t i, m;
    volatile uint16_t l;

    dev = arg;
    ifn = (IFNET *) dev->dev_icb;
    ni = (NICINFO *) dev->dev_dcb;

#if 0
    if (tcp_trace) {
        NutPrintFormat_P(dev_debug, PSTR("Enter ETHReceive\r\n"));
        NutPrintFlush(dev_debug);
    }
#endif

    l = 0;

    NutThreadSetPriority(8);
    for (;;) {
        while ((CSReadPP16(CS_RX_EVENT) & 0x0100) == 0) {
            NutSleep(10);
        }

#ifdef NUT_CS8900_OLD
        // Get the RxStatus But don't let the compiler do any optomisation
        asm volatile ("lds __tmp_reg__, %3" "\n\t"
                      "mov %B0, __tmp_reg__" "\n\t" "lds __tmp_reg__, %2" "\n\t" "mov %A0, __tmp_reg__" "\n\t":"=r" (l)
                      :"0"(l), "n"((unsigned short) (CS_DATA_P0)), "n"((unsigned short) (CS_DATA_P0 + 1))
            );

        // Get the Packet Length But don't let the compiler do any optomisation
        asm volatile ("lds __tmp_reg__, %3" "\n\t"
                      "mov %B0, __tmp_reg__" "\n\t" "lds __tmp_reg__, %2" "\n\t" "mov %A0, __tmp_reg__" "\n\t":"=r" (l)
                      :"0"(l), "n"((unsigned short) (CS_DATA_P0)), "n"((unsigned short) (CS_DATA_P0 + 1))
            );
#else

        l = *(uint8_t *) (CS_DATA_P0 + 1) << 8 | *(uint8_t *) (CS_DATA_P0);
        l = *(uint8_t *) (CS_DATA_P0 + 1) << 8 | *(uint8_t *) (CS_DATA_P0);
#endif
        //NutPrintFormat_P(dev_debug,PSTR("RxLength = %x \r\n"), l);
        //NutPrintFlush(dev_debug);

        // Account for odd length packets
        if (l & 1)
            m = l - 1;
        else
            m = l;


        nb = NutNetBufAlloc(0, NBAF_DATALINK, l);
        if (nb) {
            q = nb->nb_dl.vp;
            for (i = 0; i < m; i += 2) {
                p = (uint8_t *) CS_DATA_P0;
                *q++ = *p;
                p = (uint8_t *) CS_DATA_P0 + 1;
                *q++ = *p;
            }

            // Odd length packets
            if (m != l) {
                p = (uint8_t *) CS_DATA_P0;
                *q++ = *p;

                p = (uint8_t *) CS_DATA_P0 + 1;
                m = *p;
            }
            ni->ni_rx_packets++;
            (*ifn->if_recv) (dev, nb);
        }
    }
}

void CSSoftwareWakeup(void)
{
    volatile uint16_t *p;

    p = (uint16_t *) CS_PP_PTR;
    *p = CS_SELF_CTRL;

    NutDelay(10);
}


void CSSoftwareReset(void)
{
    volatile uint16_t *p;

    p = (uint16_t *) CS_PP_PTR;
    *p = CS_SELF_CTRL;
    p = (uint16_t *) CS_DATA_P0;
    *p = 0x0040;
}


/*!
 * \brief Initialize Ethernet Interface.
 *
 *
 * Applications typically do not use this function, but
 * call NutNetIfConfig().
 *
 * \param dev Identifies the device to initialize. The
 *            structure must be properly set.
 */
int CSNicInit(NUTDEVICE * dev)
{
    uint16_t i;
    uint16_t j;
    IFNET *ifn;
    NICINFO *ni;

#if 0
    if (tcp_trace) {
        NutPrintFormat_P(dev_debug, PSTR("Enter NicInit  \r\n"));
        NutPrintFlush(dev_debug);
    }
#endif
    cs_base = dev->dev_base;

    if (confnet.cd_size == 0)
        NutNetLoadConfig(dev->dev_name);

    ifn = dev->dev_icb;
    memcpy(ifn->if_mac, confnet.cdn_mac, 6);
    memset(dev->dev_dcb, 0, sizeof(NICINFO));
    ni = (NICINFO *) dev->dev_dcb;

    // Take CS8900 out of reset and wait for internal reset to complete
#ifdef NUT_CS8900_OLD
    outb(PORTD, inb(PORTD) & ~RESETE);
#else
    CSSoftwareWakeup();
    CSSoftwareReset();
#endif

    NutDelay(100);

    // Check for presence
    if (CSReadPP16(CS_PROD_ID) != 0x630E)
        return -1;

    //
    //  Copy our MAC address to the NIC
    // 
    for (i = 0; i < 6; i += 2) {
        j = ifn->if_mac[i] << 8;
        j |= ifn->if_mac[i + 1];
        CSWritePP16(CS_IEEE_ADDR + i, j);
        j = CSReadPP16(CS_IEEE_ADDR + i);
#if 0
        if (tcp_trace) {
            NutPrintFormat_P(dev_debug, PSTR("ADDR = %x\r\n"), j);
            NutPrintFlush(dev_debug);
        }
#endif
    }

    //
    // Enable the Transmitter and Receiver
    //
    CSWritePP16(CS_LINE_CTRL, 0x00C0);
    //i = CSReadPP16(CS_LINE_CTRL);
    //NutPrintFormat_P(dev_debug,PSTR("CS_LINE_CTRL = %x\r\n"), i);

    CSWritePP16(CS_RX_CTL, 0x0F40);
    //i = CSReadPP16(CS_RX_CTL);
    //NutPrintFormat_P(dev_debug,PSTR("CS_RX_CTL = %x\r\n"), i);

    // 
    // Start receiver thread
    //
    NutThreadCreate("csnicrx", CSNICrx, dev, 500);

    return 0;
}

/*@}*/
#else
void keep_icc_happy(void)
{
}

#endif
