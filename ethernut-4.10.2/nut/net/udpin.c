/*
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
 * -
 * Portions Copyright (c) 1983, 1993 by
 *  The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
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
 * Revision 1.10  2008/10/05 16:48:52  haraldkipp
 * Security fix. Check various lengths of incoming packets.
 *
 * Revision 1.9  2008/09/18 09:45:56  haraldkipp
 * All broadcasts were answered with ICMP unreachable. Due to this the ARP
 * table grew fast in large networks. Fixed.
 *
 * Revision 1.8  2008/08/20 06:57:00  haraldkipp
 * Implemented IP demultiplexer.
 *
 * Revision 1.7  2008/04/18 13:13:11  haraldkipp
 * Using fast ints.
 *
 * Revision 1.6  2006/10/08 16:48:22  haraldkipp
 * Documentation fixed
 *
 * Revision 1.5  2005/06/05 16:48:32  haraldkipp
 * Additional parameter enables NutUdpInput() to avoid responding to UDP
 * broadcasts with ICMP unreachable messages. Fixes bug #1215192.
 *
 * Revision 1.4  2005/05/26 11:47:24  drsung
 * ICMP unreachable will be sent on incoming udp packets with no local peer port.
 *
 * Revision 1.3  2005/02/02 16:22:35  haraldkipp
 * Do not wake up waiting threads if the incoming datagram
 * doesn't fit in the buffer.
 *
 * Revision 1.2  2003/11/24 21:01:04  drsung
 * Packet queue added for UDP sockets.
 *
 * Revision 1.1.1.1  2003/05/09 14:41:45  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.10  2003/02/04 18:14:57  harald
 * Version 3 released
 *
 * Revision 1.9  2002/06/26 17:29:36  harald
 * First pre-release with 2.4 stack
 *
 */

#include <sys/event.h>

#include <netinet/udp.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <netinet/icmp.h>

/*!
 * \addtogroup xgUDP
 */
/*@{*/

/*!
 * \brief Handle incoming UDP packets.
 *
 * \note This routine is called by the IP layer on
 *       incoming UDP packets. Applications typically do
 *       not call this function.
 *
 * \param nb    Network buffer structure containing the UDP packet.
 */
int NutUdpInput(NUTDEVICE * dev, NETBUF * nb)
{
    UDPHDR *uh;
    UDPSOCKET *sock;

    uh = (UDPHDR *) nb->nb_tp.vp;
    /* Make sure that the datagram contains a full header. */
    if (uh == NULL || nb->nb_tp.sz < sizeof(UDPHDR)) {
        NutNetBufFree(nb);
        return 0;
    }

    nb->nb_ap.sz = nb->nb_tp.sz - sizeof(UDPHDR);
    nb->nb_tp.sz = sizeof(UDPHDR);
    if (nb->nb_ap.sz) {
        nb->nb_ap.vp = uh + 1;
    }

    /*
     * Find a port. If none exists and if this datagram hasn't been
     * broadcasted, return an ICMP unreachable.
     */
    if ((sock = NutUdpFindSocket(uh->uh_dport)) == 0) {
        if ((nb->nb_flags & NBAF_UNICAST) == 0 || 
            NutIcmpResponse(ICMP_UNREACH, ICMP_UNREACH_PORT, 0, nb) == 0) {
        	NutNetBufFree(nb);
        }
        return 0;
    }

    /* if buffer size is defined, use packet queue */
    if (sock->so_rx_bsz) {
        /* New packet fits into the buffer? */
        if (sock->so_rx_cnt + nb->nb_ap.sz > sock->so_rx_bsz) {
            /* No, so discard it */
            NutNetBufFree(nb);
            return 0;
        } else {
            /* if a first packet is already in the queue, find the end
             * and add the new packet */
            if (sock->so_rx_nb) {
                NETBUF *snb;
                for (snb = sock->so_rx_nb; snb->nb_next != 0; snb = snb->nb_next);
                snb->nb_next = nb;
            } else
                sock->so_rx_nb = nb;

            /* increment input buffer count */
            sock->so_rx_cnt += nb->nb_ap.sz;
        };
    } else {                    /* no packet queue */
        /* if a packet is still buffered, discard it */
        if (sock->so_rx_nb) {
            NutNetBufFree(sock->so_rx_nb);
        }
        sock->so_rx_nb = nb;
        sock->so_rx_cnt = nb->nb_ap.sz; /* set input buffer count to size of new packet */
    };

    /* post the event only, if one thread is waiting */
    if (sock->so_rx_rdy)
        NutEventPost(&sock->so_rx_rdy);
    return 0;
}

/*@}*/
