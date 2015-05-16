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
 * $Log$
 * Revision 1.4  2007/04/12 09:18:01  haraldkipp
 * Compiles with avr-gcc 4.1.1.
 *
 * Revision 1.3  2005/11/03 15:10:29  haraldkipp
 * Some globals replaced by CONF structures.
 *
 * Revision 1.2  2004/02/20 12:31:36  haraldkipp
 * Ignore target IP if local IP is not configured
 *
 * Revision 1.1  2003/11/03 16:19:38  haraldkipp
 * First release
 *
 */

#include <avr/io.h>

#include "debug.h"
#include "arp.h"
#include "ip.h"
#include "appload.h"

/*!
 * \addtogroup xgStack
 */
/*@{*/

/*!
 * \brief Calculate the IP checksum over a block of data.
 *
 * \param data Pointer to the data block.
 * \param size Size of the data block.
 *
 * \return The checksum in network byte order.
 */
u_short IpChkSum(const u_char *data, u_short size)
{
    register u_long sum = 0;

    for (;;) {
        if (size < 2)
            break;
        sum += *((u_short *) data);
        data += 2;
        size -= 2;
    }
    if (size)
        sum += *(u_char *) data;

    while ((size = (u_short) (sum >> 16)) != 0)
        sum = (u_short) sum + size;

    return (u_short) sum ^ 0xFFFF;
}

/*!
 * \brief Receive an IP packet with the specified protocol type.
 *
 * This function calls EtherInput(). Any incoming Ethernet 
 * frame, which is not of the specified type will be discarded.
 *
 * \param proto Protocol type.
 * \param tms   Return with timeout after the specified
 *              number of waiting loops. On a 14 Mhz ATmega
 *              this value represents approximately the number
 *              of milliseconds to wait.
 *
 * \return The number of bytes received, 0 on timeout or -1 in case of 
 *         a failure.
 */
int IpInput(u_char proto, u_short tms)
{
    int rc;
    u_char unexp = 10;
    IPHDR *ip = &rframe.ip_hdr;

    for (;;) {
        /*
         * Limit the number of unexpected packets. Otherwise we may get
         * trapped in an endless loop here, if someone is continuously
         * sending us frames we don't want.
         */
        if (unexp-- == 0) {
            rc = 0;
            break;
        }

        /*
         * Get the next IP packet.
         */
        if ((rc = EtherInput(ETHERTYPE_IP, tms)) <= 0) {
            break;
        }

        /*
         * Discard packets of different IP version and
         * packets with different protocols.
         */
        if (ip->ip_v == IPVERSION && ip->ip_p == proto) {

            /*
             * Accept this packet, if it is addressed to us.
             */
            rc = htons(ip->ip_len) - (ip->ip_hl * 4);
            if (ip->ip_dst == INADDR_BROADCAST || confnet.cdn_ip_addr == 0)
                break;
            if (confnet.cdn_ip_addr) {
                if (ip->ip_dst == confnet.cdn_ip_addr) {
                    break;
                }
                if ((ip->ip_dst | confnet.cdn_ip_mask) == INADDR_BROADCAST) {
                    break;
                }
            }
        }
    }
    return rc;
}

/*!
 * \brief Send an IP packet.
 *
 * This function fills the IP header of the global send frame and calls 
 * EtherOutput(). Routing is not supported.
 *
 * \param dip   Destination IP address in network byte order.
 * \param proto IP protocol type.
 * \param len   Number of data bytes to transmit.
 *
 * \return 0 on success or -1 to indicate an error.
 */
int IpOutput(u_long dip, u_char proto, u_short len)
{
    u_char dmac[6];
    IPHDR *ip;

    /*
     * Get the Ethernet hardware address.
     */
    if (ArpRequest(dip, dmac)) {
        return -1;
    }

    /*
     * Fill the IP header.
     */
    ip = &sframe.ip_hdr;
    ip->ip_v = IPVERSION;
    ip->ip_hl = sizeof(IPHDR) >> 2;
    ip->ip_len = htons(sizeof(IPHDR) + len);
    ip->ip_ttl = 0x40;
    ip->ip_p = proto;
    ip->ip_dst = dip;
    ip->ip_src = confnet.cdn_ip_addr;
    ip->ip_id++;
    ip->ip_sum = 0;
    ip->ip_sum = IpChkSum(ip, sizeof(IPHDR));

    return EtherOutput(dmac, ETHERTYPE_IP, sizeof(IPHDR) + len);
}

/*@}*/
