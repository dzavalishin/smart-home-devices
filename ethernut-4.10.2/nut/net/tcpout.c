/*
 * Copyright (C) 2001-2005 by egnite Software GmbH. All rights reserved.
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
 * Revision 1.11  2009/02/22 12:30:36  olereinhardt
 * Include "include/errno.h" instead of "include/net/errno.h"
 *
 * Revision 1.10  2008/08/11 07:00:32  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.9  2008/04/06 13:29:01  haraldkipp
 * In unreliable or high traffic networks connections may suddenly freeze.
 * The problem is, that during overflows (happening every 65s) the
 * retransmission timer may be loaded with 0, which in turn disables all
 * outstanding retransmission. Applied fix contributed by Henrik Maier.
 *
 * Revision 1.8  2006/10/08 16:48:22  haraldkipp
 * Documentation fixed
 *
 * Revision 1.7  2006/03/02 19:57:34  haraldkipp
 * ICCARM insists on a (void *) typecast for the second parameter of memcpy().
 *
 * Revision 1.6  2006/01/23 17:33:47  haraldkipp
 * Avoid memory alignment errors.
 *
 * Revision 1.5  2005/04/30 16:42:42  chaac
 * Fixed bug in handling of NUTDEBUG. Added include for cfg/os.h. If NUTDEBUG
 * is defined in NutConf, it will make effect where it is used.
 *
 * Revision 1.4  2005/04/05 17:41:48  haraldkipp
 * Header is dangerous when you forget to remove all debug outputs.
 *
 * Revision 1.3  2004/07/30 19:54:46  drsung
 * Some code of TCP stack redesigned. Round trip time calculation is now
 * supported. Fixed several bugs in TCP state machine. Now TCP connections
 * should be more reliable under heavy traffic or poor physical connections.
 *
 * Revision 1.2  2004/04/15 11:05:35  haraldkipp
 * Set retransmission timer on first transmit queue entry
 *
 * Revision 1.1.1.1  2003/05/09 14:41:40  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.15  2003/03/31 12:29:17  harald
 * Check NEBUF allocation
 *
 * Revision 1.14  2003/02/04 18:14:57  harald
 * Version 3 released
 *
 * Revision 1.13  2002/09/15 16:50:44  harald
 * *** empty log message ***
 *
 * Revision 1.12  2002/06/26 17:29:36  harald
 * First pre-release with 2.4 stack
 *
 */

#include <cfg/os.h>
#include <string.h>
#include <errno.h>

#include <sys/heap.h>
#include <sys/event.h>
#include <sys/timer.h>

#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/icmp.h>
#include <netinet/ip_icmp.h>
#include <netinet/ipcsum.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <sys/thread.h>

#ifdef NUTDEBUG
#include <net/netdebug.h>
#endif

/*!
 * \addtogroup xgTCP
 */

/*!
 * \brief Initiate TCP segment transmission.
 *
 * Check the TCP socket status and send any segment waiting
 * for transmission.
 *
 * The function will not return until the data has been stored in the 
 * network device hardware for transmission. If the device is not ready 
 * for transmitting a new packet, the calling thread will be suspended 
 * until the device becomes ready again. 
 *
 * If the target host is connected through an Ethernet network and if 
 * the hardware address of that host is currently unknown, an ARP 
 * request is sent out and the function will block until a response 
 * is received or an ARP timeout occurs.
 *
 * Segments containing data or SYN and FIN flags are added to a special 
 * queue for unacknowledged segments and will be retransmitted by the 
 * TCP timer thread, if not acknowledged by the remote within a specific 
 * time. The state machine will remove these segments from the queue
 * as soon as they are acknowledged.
 *
 * \note This function is mainly used by the TCP state machine.
 *       Applications typically do not call this function but 
 *       use NutTcpSend(), which is part of the TCP socket interface.
 *
 * \param sock  Socket descriptor. This pointer must have been retrieved 
 *              by calling NutTcpCreateSocket().
 * \param data  Pointer to TCP segment contents.
 * \param size  TCP segment length.
 *
 * \return 0 on success, -1 otherwise. Returning 0 does not imply that 
 *         the data has been successfully delivered, because flow control 
 *         and retransmission is still handled in the background.
 */
int NutTcpOutput(TCPSOCKET * sock, CONST uint8_t * data, uint16_t size)
{
    NETBUF *nb;
    NETBUF *nb_clone = 0;
    TCPHDR *th;
    uint16_t csum;
    uint8_t hlen;

    /*
     * Check if anything to send at all.
     */
    if (size == 0
        && (sock->so_tx_flags & (SO_SYN | SO_FIN | SO_FORCE)) == 0)
        return 0;

    /*
     * Build TCP header. Add room for MAXSEG option if this is a
     * SYN segment.
     */
    hlen = sizeof(TCPHDR);
    if (sock->so_tx_flags & SO_SYN)
        hlen += 4;
    if ((nb = NutNetBufAlloc(0, NBAF_TRANSPORT, hlen)) == 0) {
        sock->so_last_error = ENOBUFS;
        return -1;
    }
    th = (TCPHDR *) nb->nb_tp.vp;
    th->th_sport = sock->so_local_port;
    th->th_dport = sock->so_remote_port;
    th->th_x2 = 0;
    th->th_off = hlen >> 2;

    sock->so_tx_flags &= ~SO_FORCE;

    /*
     * Process ACK flag.
     */
    th->th_seq = htonl(sock->so_tx_nxt);
    if (sock->so_tx_flags & SO_ACK) {
        th->th_flags = TH_ACK;
        sock->so_tx_flags &= ~SO_ACK;
        th->th_ack = htonl(sock->so_rx_nxt);
    } else {
        th->th_flags = 0;
        th->th_ack = 0;
    }

    /*
     * Any SYN is sent first. Add options too. We rely on the caller 
     * not to send a SYN segment with data, because this may break
     * some old stacks.
     */
    if (sock->so_tx_flags & SO_SYN) {
        uint8_t *cp;
        uint16_t n_mss = htons(sock->so_mss);

        th->th_flags |= TH_SYN;
        sock->so_tx_flags &= ~SO_SYN;
        sock->so_tx_nxt++;

        cp = (uint8_t *) (th + 1);
        *cp++ = TCPOPT_MAXSEG;
        *cp++ = TCPOLEN_MAXSEG;
        *cp++ = *(uint8_t *)&n_mss;
        *cp = *((uint8_t *)(&n_mss) + 1);
    }

    /*
     * Next preference is sending data. Set PUSH flag.
     */
    else if (size) {
        if ((nb = NutNetBufAlloc(nb, NBAF_APPLICATION, size)) == 0) {
            sock->so_last_error = ENOBUFS;
            return -1;
        }
        memcpy(nb->nb_ap.vp, (void *)data, size);
        sock->so_tx_nxt += size;
        th->th_flags |= TH_PUSH;
    }

    /*
     * If all data sent, transmit any waiting FIN.
     */
    else if (sock->so_tx_flags & SO_FIN) {
        th->th_flags |= TH_FIN;
        sock->so_tx_flags &= ~SO_FIN;
        sock->so_tx_nxt++;
        //@@@printf ("[%04X]TcpOutput: sending FIN\n", (u_short) sock);
    }

    /*
     * We close our receiver window, if it is
     * below the maximum segment size.
     */
    if (sock->so_rx_win < sock->so_mss)
        th->th_win = 0;
    else
        th->th_win = htons(sock->so_rx_win);
    th->th_sum = 0;
    th->th_urp = 0;

    /*
     * Calculate TCP checksum.
     */
    csum =
        NutIpPseudoChkSumPartial(sock->so_local_addr, sock->so_remote_addr,
                                 IPPROTO_TCP,
                                 htons(nb->nb_tp.sz + nb->nb_ap.sz));
    csum = NutIpChkSumPartial(csum, th, nb->nb_tp.sz);
    th->th_sum = NutIpChkSum(csum, nb->nb_ap.vp, nb->nb_ap.sz);

#ifdef NUTDEBUG
    if (__tcp_trf)
        NutDumpTcpHeader(__tcp_trs, "OUT", sock, nb);
#endif

	/*
	 * To avoid a race condition in tcp state machine, the segment is first
     * appended to the transmission que, and then sent to the network.
	 */

    /*
     * Append the segment to our transmission queue.
     */
    //@@@printf ("[%04X]TcpOutput: size: %u, flags: %u\n", (u_short) sock, size, th->th_flags);
    if (size || ((th->th_flags & (TH_FIN | TH_SYN)))) {
        //@@@printf ("[%04X]TcpOutput: appending nb to queue\n", (u_short) sock);
        NETBUF *nbp;

        nb->nb_next = 0;
        if ((nbp = sock->so_tx_nbq) == 0) {
            sock->so_tx_nbq = nb;
            /*
             * First entry, so initialize our retransmission timer.
             * It is also set at various places in the state machine,
             * but here is the best central point to do it. We may
             * carefully check later, if we can remove some in the
             * state machine.
             */
            sock->so_retran_time = (uint16_t) NutGetMillis() | 1;
        }
        else {
            while (nbp->nb_next)
                nbp = nbp->nb_next;
            nbp->nb_next = nb;
        }
        if (sock->so_rtt_seq == 0)
            sock->so_rtt_seq = ntohl (th->th_seq);
        nb_clone = NutNetBufClonePart(nb, 0);
        if (nb_clone == NULL) {
            sock->so_last_error = ENOBUFS;
            return -1;
        }
    }
    else
        nb_clone = nb;

    /*
     * IP output might fail because of routing, ARP or network device 
     * problems or because the system ran out of memory.
     */
    if (NutIpOutput(IPPROTO_TCP, sock->so_remote_addr, nb_clone)) 
        return -1;

    NutNetBufFree (nb_clone);
    return 0;
}


/*!
 * \brief Reject an incoming segment.
 *
 * Send RST in response to an incoming segment, which should
 * be rejected.
 *
 * The function avoids to send out a RST segment in response to 
 * an incoming RST segment.
 *
 * \note This function is mainly used by the TCP state machine.
 *       Applications typically do not call this function.
 *
 * \param nb Network buffer structure of the incoming segment. 
 *           Will be released within this function.
 *
 * \return 0 on success, -1 otherwise.
 */
int NutTcpReject(NETBUF * nb)
{
    uint16_t csum;
    IPHDR *ih = (IPHDR *) nb->nb_nw.vp;
    TCPHDR *th = (TCPHDR *) nb->nb_tp.vp;

    /*
     * Never send RST in response to RST.
     */
    if (th->th_flags & TH_RST) {
        NutNetBufFree(nb);
        return 0;
    }

    /*
     * Remove any application data and TCP header options.
     */
    nb->nb_ap.sz = 0;
    nb->nb_tp.sz = sizeof(TCPHDR);

    /*
     * Swap ports.
     */
    csum = th->th_sport;
    th->th_sport = th->th_dport;
    th->th_dport = csum;

    /*
     * If the incoming segment has an ACK field, the reset
     * takes its sequence number from the ACK field of the
     * segment, otherwise the reset has sequence number zero
     * and the ACK field is set to the sum of the sequence
     * number and segment length of the incoming segment.
     */
    if (th->th_flags & TH_ACK) {
        th->th_flags = TH_RST;
        th->th_seq = th->th_ack;
        th->th_ack = 0;
    } else {
        if (th->th_flags & TH_SYN)
            th->th_ack = htonl(ntohl(th->th_seq) + 1);
        else
            th->th_ack = th->th_seq;
        th->th_seq = 0;
        th->th_flags = TH_RST | TH_ACK;
    }
    th->th_x2 = 0;
    th->th_off = sizeof(TCPHDR) / 4;
    th->th_win = 0;
    th->th_urp = 0;

    /*
     * Calculate TCP checksum without application data.
     */
    th->th_sum = 0;
    csum =
        NutIpPseudoChkSumPartial(ih->ip_dst, ih->ip_src, IPPROTO_TCP, 
                                 htons(nb->nb_tp.sz));
    th->th_sum = NutIpChkSum(csum, th, nb->nb_tp.sz);

    /*
     * Sent segment back to the source.
     */
#ifdef NUTDEBUG
    if (__tcp_trf)
        NutDumpTcpHeader(__tcp_trs, "REJ", 0, nb);
#endif
    if(NutIpOutput(IPPROTO_TCP, ih->ip_src, nb) == 0)
        NutNetBufFree(nb);
    return 0;
}
