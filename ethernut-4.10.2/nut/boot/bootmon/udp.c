/*
 * Copyright (c) 1983, 1993 by The Regents of the University of California
 * Copyright (C) 2001-2007 by egnite Software GmbH
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
 * $Id: udp.c 2954 2010-04-03 11:22:40Z haraldkipp $
 */

#include "udp.h"
#include "bootmon.h"
#include "dialog.h"

/*!
 * \addtogroup xgStack
 */
/*@{*/

/*!
 * \brief Receive a UDP datagram on a specified port.
 *
 * This function calls IpInput(). Any incoming Ethernet frame, which is 
 * not a UDP datagram to the specified port will be discarded.
 *
 * \param port UDP port to listen to.
 * \param tms  Return with timeout after the specified number of 
 *             waiting loops. On a 14 Mhz ATmega this value represents 
 *             approximately the number of milliseconds to wait.
 *
 * \return The number of data bytes received, 0 on timeout or -1 in 
 *         case of a failure.
 */
int UdpInput(unsigned short port, unsigned int tms)
{
    int rc;
    UDPHDR *uh = &rframe.eth.udp.udp_hdr;

    for (;;) {
        if ((rc = IpInput(IPPROTO_UDP, tms)) <= 0)
            break;

        /*
         * If the datagram matches the requested port, then return
         * the number of data bytes.
         */
        if (port == uh->uh_dport) {
            rc = ntohs(uh->uh_ulen) - sizeof(UDPHDR);
            break;
        }
    }
    return rc;
}

/*!
 * \brief Send a UDP datagram.
 *
 * This function fills the UDP header of the global send frame and calls 
 * IpOutput().
 *
 * \param dip   Destination IP address in network byte order.
 * \param dport Destination port.
 * \param sport Source port.
 * \param len   Number of data bytes to transmit.
 *
 * \return 0 on success or -1 to indicate an error.
 */
int UdpOutput(unsigned long dip, unsigned short dport, unsigned short sport, unsigned short len)
{
    UDPHDR *up = &sframe.eth.udp.udp_hdr;

    /*
     * Setup the UDP header.
     */
    up->uh_sport = sport;
    up->uh_dport = dport;
    up->uh_ulen = htons(sizeof(UDPHDR) + len);

    return IpOutput(dip, IPPROTO_UDP, sizeof(UDPHDR) + len);
}

/*@}*/
