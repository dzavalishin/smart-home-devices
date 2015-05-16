/*
 * Copyright (C) 2001-2007 by egnite Software GmbH
 * Copyright (C) 2009 by egnite GmbH
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
 * $Id: tcpsm.c 3416 2011-05-15 16:58:54Z haraldkipp $
 */

#include <cfg/os.h>
#include <cfg/tcp.h>

#include <sys/thread.h>
#include <sys/heap.h>
#include <sys/event.h>
#include <sys/timer.h>

#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <net/route.h>
#include <sys/socket.h>
#include <netinet/tcputil.h>
#include <netinet/tcp.h>

#ifdef NUTDEBUG
#include <net/netdebug.h>
#endif

#ifndef NUT_THREAD_TCPSMSTACK
#if defined(__AVR__)
#if defined(__GNUC__)
/* avr-gcc size optimized code used 148 bytes. */
#define NUT_THREAD_TCPSMSTACK   256
#else
/* icc-avr v7.19 used 312 bytes. */
#define NUT_THREAD_TCPSMSTACK   512
#endif
#else
/* arm-elf-gcc used 260 bytes with size optimized, 644 bytes with debug code. */
#define NUT_THREAD_TCPSMSTACK   384
#endif
#endif

#ifndef TCP_RETRIES_MAX
#define TCP_RETRIES_MAX         7
#endif

extern TCPSOCKET *tcpSocketList;

/*!
 * \addtogroup xgTCP
 */
/*@{*/

HANDLE tcp_in_rdy;
NETBUF *volatile tcp_in_nbq;
static uint16_t tcp_in_cnt;
static HANDLE tcpThread = 0;

#ifndef TCP_COLLECT_INADV
#define TCP_COLLECT_INADV   8
#endif

#ifndef TCP_COLLECT_SLIMIT
#define TCP_COLLECT_SLIMIT  256
#endif

#ifndef TCP_BACKLOG_MAX
#define TCP_BACKLOG_MAX     8
#endif

#if TCP_BACKLOG_MAX
#ifndef TCP_BACKLOG_TIME
#define TCP_BACKLOG_TIME    5
#endif
static NETBUF *tcp_backlog[TCP_BACKLOG_MAX];
static uint_fast8_t tcp_backlog_time[TCP_BACKLOG_MAX];
#endif

static size_t tcp_adv_cnt;
static size_t tcp_adv_max = TCP_WINSIZE;

static void NutTcpStateProcess(TCPSOCKET * sock, NETBUF * nb);

/* ================================================================
 * Helper functions
 * ================================================================
 */

#if TCP_BACKLOG_MAX
static int NutTcpBacklogAdd(NETBUF *nb)
{
    uint_fast8_t i;
    uint_fast8_t n = TCP_BACKLOG_MAX;
    IPHDR *ih = (IPHDR *) nb->nb_nw.vp;
    TCPHDR *th = (TCPHDR *) nb->nb_tp.vp;

    /* Process SYN segments only. */
    if ((th->th_flags & (TH_SYN | TH_ACK | TH_RST)) == TH_SYN) {
        for (i = 0; i < TCP_BACKLOG_MAX; i++) {
            if (tcp_backlog[i] == NULL) {
                /* Remember the first free entry. */
                if (n == TCP_BACKLOG_MAX) {
                    n = i;
                }
            }
            else if (((IPHDR *) tcp_backlog[i]->nb_nw.vp)->ip_src == ih->ip_src &&
                    ((TCPHDR *) tcp_backlog[i]->nb_tp.vp)->th_sport == th->th_sport) {
                /* Already received a SYN. Either the remote is too impatient
                ** or we are too busy. Kill this entry and reject the SYN. */
                NutNetBufFree(tcp_backlog[i]);
                tcp_backlog[i] = NULL;
                return -1;
            }
        }
        /* First SYN from the remote for this port.
        ** If there is a free entry left, then use it. */
        if (n != TCP_BACKLOG_MAX) {
            tcp_backlog[n] = nb;
            tcp_backlog_time[n] = 0;
            return 0;
        }
    }
    return -1;
}

static NETBUF *NutTcpBacklogCheck(uint16_t port)
{
    NETBUF *nb;
    uint_fast8_t i;
    uint_fast8_t n = TCP_BACKLOG_MAX;

    for (i = 0; i < TCP_BACKLOG_MAX; i++) {
        if (tcp_backlog[i]) {
            if (((TCPHDR *) tcp_backlog[i]->nb_tp.vp)->th_dport == port) {
                if (n == TCP_BACKLOG_MAX || tcp_backlog_time[i] > tcp_backlog_time[n]) {
                    n = i;
                }
            }
        }
    }
    if (n == TCP_BACKLOG_MAX) {
        nb = NULL;
    } else {
        nb = tcp_backlog[n];
        tcp_backlog[n] = NULL;
    }
    return nb;
}

static NETBUF *NutTcpBacklogTimer(void)
{
    NETBUF *nb;
    uint_fast8_t i;
    uint_fast8_t n = TCP_BACKLOG_MAX;

    for (i = 0; i < TCP_BACKLOG_MAX; i++) {
        if (tcp_backlog[i]) {
            if (tcp_backlog_time[i] < TCP_BACKLOG_TIME) {
                tcp_backlog_time[i]++;
            } else {
                n = i;
            }
        }
    }
    if (n == TCP_BACKLOG_MAX) {
        nb = NULL;
    } else {
        nb = tcp_backlog[n];
        tcp_backlog[n] = NULL;
    }
    return nb;
}
#endif /* TCP_BACKLOG_MAX */

/*!
 * \brief Reads TCP option fields if any, and writes the data to
 *        the socket descriptor if important for us.
 *
 * \param sock Socket descriptor. This pointer must have been
 *             retrieved by calling NutTcpCreateSocket().
 * \param nb   Network buffer structure containing a TCP segment.
 */
static void NutTcpInputOptions(TCPSOCKET * sock, NETBUF * nb)
{
    uint8_t *cp;
    uint16_t s;

    /* any options there? */
    if (nb->nb_tp.sz <= sizeof (TCPHDR))
        return;

    /* loop through available options */
    for (cp = ((uint8_t*) nb->nb_tp.vp) + sizeof(TCPHDR); (*cp != TCPOPT_EOL)
       && (cp - (uint8_t *)nb->nb_tp.vp < (int)nb->nb_tp.sz); )
    {
        switch (*cp)
        {
            /* On NOP just proceed to next option */
            case TCPOPT_NOP:
                cp++;
                continue;

            /* Read MAXSEG option */
            case TCPOPT_MAXSEG:
                /* Network uses big endian. */
                s = cp[2];
                s <<= 8;
                s |= cp[3];
                if (s < sock->so_mss)
                    sock->so_mss = s;
                cp += TCPOLEN_MAXSEG;
                break;
            /* Ignore any other options */
            default:
                cp += *(uint8_t*) (cp + 1);
                break;
        }
    }
}

/*!
 * \brief Move application data in sync from the network buffer
 *        structure to the socket's receive buffer.
 *
 * \param sock Socket descriptor. This pointer must have been
 *             retrieved by calling NutTcpCreateSocket().
 * \param nb   Network buffer structure containing a TCP segment.
 */
static void NutTcpProcessAppData(TCPSOCKET * sock, NETBUF * nb)
{
    /*
     * Add the NETBUF to the socket's input buffer.
     */
    if (sock->so_rx_buf) {
        NETBUF *nbp = sock->so_rx_buf;

        while (nbp->nb_next)
            nbp = nbp->nb_next;
        nbp->nb_next = nb;
    } else
        sock->so_rx_buf = nb;

    /*
     * Update the number of bytes available in the socket's input buffer
     * and the sequence number we expect next.
     */
    sock->so_rx_cnt += nb->nb_ap.sz;
    sock->so_rx_nxt += nb->nb_ap.sz;

    /*
     * Reduce our TCP window size.
     */
    if (nb->nb_ap.sz >= sock->so_rx_win)
        sock->so_rx_win = 0;
    else
        sock->so_rx_win -= nb->nb_ap.sz;

    /*
     * Set the socket's ACK flag. This will enable ACK transmission in
     * the next outgoing segment. If no more NETBUFs are queued, we
     * force immediate transmission of the ACK.
     */
    sock->so_tx_flags |= SO_ACK;
    if (nb->nb_next)
        nb->nb_next = 0;
    else
        sock->so_tx_flags |= SO_FORCE;

    if (++sock->so_rx_apc > TCP_COLLECT_INADV) {
        NETBUF *nbq;
        int_fast8_t apc = sock->so_rx_apc;
        int cnt = sock->so_rx_cnt;

        for (nbq = sock->so_rx_buf; nbq; nbq = nbq->nb_next) {
            if (nbq->nb_ap.sz < TCP_COLLECT_SLIMIT) {
                sock->so_rx_apc -= NutNetBufCollect(nbq, cnt);
                break;
            }
            if (--apc < 8) {
                break;
            }
            cnt -= nbq->nb_ap.sz;
        }
    }
    NutTcpOutput(sock, 0, 0);
}

/*
 * \param sock Socket descriptor.
 */
static void NutTcpProcessSyn(TCPSOCKET * sock, IPHDR * ih, TCPHDR * th)
{
    uint16_t mss;
    NUTDEVICE *dev;
    IFNET *nif;

    sock->so_local_addr = ih->ip_dst;
    sock->so_remote_port = th->th_sport;
    sock->so_remote_addr = ih->ip_src;

    sock->so_rx_nxt = sock->so_tx_wl1 = sock->so_rx_isn = ntohl(th->th_seq);
    sock->so_rx_nxt++;
    sock->so_tx_win = ntohs(th->th_win);

    /*
     * To avoid unnecessary fragmentation, limit the
     * maximum segment size to the maximum transfer
     * unit of our interface.
     */
    if ((dev = NutIpRouteQuery(ih->ip_src, 0)) != 0) {
        nif = dev->dev_icb;
        mss = nif->if_mtu - sizeof(IPHDR) - sizeof(TCPHDR);
        if (sock->so_mss == 0 || sock->so_mss > mss)
            sock->so_mss = mss;

        /* Limit output buffer size to mms */
        if (sock->so_devobsz > sock->so_mss)
            sock->so_devobsz = sock->so_mss;
    }
}

/*!
 * \brief ACK processing.
 *
 * \param sock Socket descriptor. This pointer must have been
 *             retrieved by calling NutTcpCreateSocket().
 *
 */
static int NutTcpProcessAck(TCPSOCKET * sock, TCPHDR * th, uint16_t length)
{
    NETBUF *nb;
    uint32_t h_seq;
    uint32_t h_ack;

    /*
     * If remote acked something not yet send, reply immediately.
     */
    h_ack = ntohl(th->th_ack);
    if (SeqIsAfter(h_ack, sock->so_tx_nxt)) {
        sock->so_tx_flags |= SO_ACK | SO_FORCE;
        return 0;
    }

    /*
     * If the new sequence number or acknowledged sequence number
     * is above our last update, we adjust our transmit window.
     * Avoid dupe ACK processing on window updates.
     */
    if (h_ack == sock->so_tx_una) {
        h_seq = ntohl(th->th_seq);
        if (SeqIsAfter(h_seq, sock->so_tx_wl1) || (h_seq == sock->so_tx_wl1 && !SeqIsAfter(sock->so_tx_wl2, h_ack))) {
            sock->so_tx_win = ntohs(th->th_win);
            sock->so_tx_wl1 = h_seq;
            sock->so_tx_wl2 = h_ack;
        }
    }

    /*
     * Ignore old ACKs but wake up sleeping transmitter threads, because
     * the window size may have changed.
     */
    if (SeqIsAfter(sock->so_tx_una, h_ack)) {
        return 0;
    }

    /*
     * Process duplicate ACKs.
     */
    if (h_ack == sock->so_tx_una) {
        /*
         * Don't count, if nothing is waiting for ACK,
         * segment contains data or on SYN/FIN segments.
         */
        if (sock->so_tx_nbq && length == 0 && (th->th_flags & (TH_SYN | TH_FIN)) == 0) {
            /*
             * If dupe counter reaches it's limit, resend
             * the oldest unacknowledged netbuf.
             */
            if (++sock->so_tx_dup >= 3) {
                sock->so_tx_dup = 0;
#ifdef NUTDEBUG
                if (__tcp_trf & NET_DBG_SOCKSTATE)
                    NutDumpTcpHeader(__tcp_trs, "RET", sock, sock->so_tx_nbq);
#endif
                /*
                 * Retransmit first unacked packet from queue.
                 * Actually we got much more trouble if this fails.
                 */
                if (NutTcpStateRetranTimeout(sock))
                    return -1;
            }
        }
        return 0;
    }

    /*
     * We're here, so the ACK must have actually acked something
     */
    sock->so_tx_dup = 0;
    sock->so_tx_una = h_ack;

    /*
     * Bugfix contributed by Liu Limin: If the remote is slow and this
     * line is missing, then Ethernut will send a full data packet even
     * if the remote closed the window.
     */
    sock->so_tx_win = ntohs(th->th_win);

    /*
     * Do round trip time calculation.
     */
    if (sock->so_rtt_seq && SeqIsAfter(h_ack, sock->so_rtt_seq))
        NutTcpCalcRtt (sock);
    sock->so_rtt_seq = 0;
    /*
     * Remove all acknowledged netbufs.
     */
    while ((nb = sock->so_tx_nbq) != 0) {
        /* Calculate the sequence beyond this netbuf. */
        h_seq = ntohl(((TCPHDR *) (nb->nb_tp.vp))->th_seq) + nb->nb_ap.sz;
        if (((TCPHDR *) (nb->nb_tp.vp))->th_flags & (TH_SYN | TH_FIN)) {
            h_seq++;
        }
        //@@@printf ("[%04X]*: processack, check seq#: %lu\n", (u_short) sock, h_seq);
        if (SeqIsAfter(h_seq, h_ack)) {
            break;
        }
        sock->so_tx_nbq = nb->nb_next;
        NutNetBufFree(nb);
    }

    /*
     * Reset retransmit timer and wake up waiting transmissions.
     */
    if (sock->so_tx_nbq) {
        sock->so_retran_time = (uint16_t) NutGetMillis() | 1;
    } else {
        sock->so_retran_time = 0;
    }
    sock->so_retransmits = 0;

    return 0;
}



/* ================================================================
 * State changes.
 * ================================================================
 */
/*!
 * State change, possibly inform application.
 *
 * \param sock  Socket descriptor.
 * \param state New state to switch to.
 *
 * \return 0 on success, -1 on illegal state change attempt.
 */
static int NutTcpStateChange(TCPSOCKET * sock, uint8_t state)
{
    int rc = 0;
    ureg_t txf = 0;

    switch (sock->so_state) {
        /* Handle the most common case first. */
    case TCPS_ESTABLISHED:
        switch (state) {
        case TCPS_FIN_WAIT_1:
            /*
             * Closed by application.
             */
            sock->so_tx_flags |= SO_FIN | SO_ACK;
            txf = 1;

#ifdef RTLCONNECTHACK
            /*
             * Hack alert!
             * On the RTL8019AS we got a problem. Because of not handling
             * the CHRDY line, the controller drops outgoing packets when
             * a browser opens multiple connections concurrently, producing
             * several short incoming packets. Empirical test showed, that
             * a slight delay during connects and disconnects helped to
             * remarkably reduce this problem.
             */
            NutDelay(5);
#endif
            break;
        case TCPS_CLOSE_WAIT:
            /*
             * FIN received.
             */
            sock->so_tx_flags |= SO_ACK | SO_FORCE;
            txf = 1;
            break;
        default:
            rc = -1;
            break;
        }
        break;

    case TCPS_LISTEN:
        /*
         * SYN received.
         */
        if (state == TCPS_SYN_RECEIVED) {
            sock->so_tx_flags |= SO_SYN | SO_ACK;
            txf = 1;
#ifdef RTLCONNECTHACK
            /*
             * Hack alert!
             * On the RTL8019AS we got a problem. Because of not handling
             * the CHRDY line, the controller drops outgoing packets when
             * a browser opens multiple connections concurrently, producing
             * several short incoming packets. Empirical test showed, that
             * a slight delay during connects and disconnects helped to
             * remarkably reduce this problem.
             */
            NutDelay(5);
#endif
        } else
            rc = -1;
        break;

    case TCPS_SYN_SENT:
        switch (state) {
        case TCPS_LISTEN:
            /*
             * RST received on passive socket.
             */
            break;
        case TCPS_SYN_RECEIVED:
            /*
             * SYN received.
             */
            sock->so_tx_flags |= SO_SYN | SO_ACK;
            txf = 1;
            break;
        case TCPS_ESTABLISHED:
            /*
             * SYNACK received.
             */
            sock->so_tx_flags |= SO_ACK | SO_FORCE;
            txf = 1;
            break;
        default:
            rc = -1;
            break;
        }
        break;

    case TCPS_SYN_RECEIVED:
        switch (state) {
        case TCPS_LISTEN:
            /*
             * RST received on passive socket.
             */
            break;
        case TCPS_ESTABLISHED:
            /*
             * ACK of SYN received.
             */
            break;
        case TCPS_FIN_WAIT_1:
            /*
             * Closed by application.
             */
            sock->so_tx_flags |= SO_FIN;
            txf = 1;
            break;
        case TCPS_CLOSE_WAIT:
            /*
             * FIN received.
             */
            sock->so_tx_flags |= SO_FIN | SO_ACK;
            txf = 1;
            break;
        default:
            rc = -1;
            break;
        }
        break;

    case TCPS_FIN_WAIT_1:
        switch (state) {
        case TCPS_FIN_WAIT_1:
        case TCPS_FIN_WAIT_2:
            /*
             * ACK of FIN received.
             */
            break;
        case TCPS_CLOSING:
            /*
             * FIN received.
             */
            sock->so_tx_flags |= SO_ACK | SO_FORCE;
            txf = 1;
            break;
        case TCPS_TIME_WAIT:
            /*
             * FIN and ACK of FIN received.
             */
            break;
        default:
            rc = -1;
            break;
        }
        break;

    case TCPS_FIN_WAIT_2:
        /*
         * FIN received.
         */
        if (state != TCPS_TIME_WAIT)
            rc = -1;
        sock->so_tx_flags |= SO_ACK | SO_FORCE;
        txf = 1;
        break;

    case TCPS_CLOSE_WAIT:
        /*
         * Closed by application.
         */
        if (state == TCPS_LAST_ACK) {
            sock->so_tx_flags |= SO_FIN | SO_ACK;
            txf = 1;
        } else
            rc = -1;
        break;

    case TCPS_CLOSING:
        /*
         * ACK of FIN received.
         */
        if (state != TCPS_TIME_WAIT)
            rc = -1;
        break;

    case TCPS_LAST_ACK:
        rc = -1;
        break;

    case TCPS_TIME_WAIT:
        rc = -1;
        break;

    case TCPS_CLOSED:
        switch (state) {
        case TCPS_LISTEN:
            /*
             * Passive open by application.
             */
            break;
        case TCPS_SYN_SENT:
            /*
             * Active open by application.
             */
            sock->so_tx_flags |= SO_SYN;
            txf = 1;
            break;
        default:
            rc = -1;
            break;
        }
        break;
    }
#ifdef NUTDEBUG
    if (__tcp_trf & NET_DBG_SOCKSTATE) {
        fprintf(__tcp_trs, " %04x-", (unsigned int) sock);
        if (rc)
            NutDumpSockState(__tcp_trs, sock->so_state, "**ERR ", "**>");
        NutDumpSockState(__tcp_trs, state, "[>", "]");
    }
#endif

    if (rc == 0) {
        sock->so_state = state;
        if (txf && NutTcpOutput(sock, 0, 0)) {
            if (state == TCPS_SYN_SENT) {
                rc = -1;
                sock->so_last_error = EHOSTDOWN;
                NutEventPostAsync(&sock->so_ac_tq);
            }
        }
        if (state == TCPS_CLOSE_WAIT) {
            /*
             * Inform application.
             */
            NutEventBroadcast(&sock->so_rx_tq);
            NutEventBroadcast(&sock->so_pc_tq);
            NutEventBroadcast(&sock->so_ac_tq);
        }
    }
    return rc;
}

/* ================================================================
 * Application events.
 * ================================================================
 */
/*!
 * \brief Initiated by the application.
 *
 * \param sock Socket descriptor. This pointer must have been
 *             retrieved by calling NutTcpCreateSocket().
 *
 * \return 0 if granted, error code otherwise.
 */
int NutTcpStatePassiveOpenEvent(TCPSOCKET * sock)
{
    if (sock->so_state != TCPS_CLOSED)
        return (sock->so_last_error = EISCONN);

    NutTcpStateChange(sock, TCPS_LISTEN);

#if TCP_BACKLOG_MAX
    {
        /* If a SYN segment is already waiting in the backlog,
        ** then process it and return to the caller. */
        NETBUF *nb = NutTcpBacklogCheck(sock->so_local_port);
        if (nb) {
            NutTcpInputOptions(sock, nb);
            NutTcpStateProcess(sock, nb);

            return 0;
        }
    }
    if (NutEventWait(&sock->so_pc_tq, sock->so_read_to)) {
        sock->so_state = TCPS_CLOSED;
        return (sock->so_last_error = ETIMEDOUT);
    }
#else
    /* For backward compatibility we simply block the application.
    ** If we do not have a backlog, then timing out would not make
    ** much sense anyway, because incoming connection attempts will
    ** be immediately rejected. */
    NutEventWait(&sock->so_pc_tq, 0);
#endif /* TCP_BACKLOG_MAX */

    return 0;
}

/*!
 * \brief Initiated by the application.
 *
 * The caller must make sure, that the socket is in closed state.
 *
 * \param sock Socket descriptor. This pointer must have been
 *             retrieved by calling NutTcpCreateSocket().
 *
 * \return 0 if granted, -1 otherwise.
 */
int NutTcpStateActiveOpenEvent(TCPSOCKET * sock)
{
    /*
     * Switch state to SYN_SENT. This will
     * transmit a SYN packet.
     */
    NutTcpStateChange(sock, TCPS_SYN_SENT);

    /*
     * Block application.
     */
	 if(sock->so_state == TCPS_SYN_SENT)
		NutEventWait(&sock->so_ac_tq, 0);

    if (sock->so_state != TCPS_ESTABLISHED && sock->so_state != TCPS_CLOSE_WAIT)
        return -1;

    return 0;
}

/*!
 * \brief Socket close by application.
 *
 * If socket is in state SYN_RECEIVED or ESTABLISHED,
 * it is changed to FINWAIT1.
 *
 * No further data sending is accepted.
 * Receiving is still allowed.
 *
 * \param sock Socket descriptor. This pointer must have been
 *             retrieved by calling NutTcpCreateSocket().
 */
int NutTcpStateCloseEvent(TCPSOCKET * sock)
{
    if (sock == 0)
        return -1;

    NutThreadYield();

    switch (sock->so_state) {
    case TCPS_LISTEN:
    case TCPS_SYN_SENT:
    case TCPS_CLOSED:
        /*
         * No connection yet, immediately destroy the socket.
         */
        NutTcpDestroySocket(sock);
        break;

    case TCPS_SYN_RECEIVED:
    case TCPS_ESTABLISHED:
        /*
         * Send a FIN and wait for ACK or FIN.
         */
        //@@@printf ("[%04X]ESTABLISHED: going to FIN_WAIT_1\n", (u_short) sock);
        NutTcpStateChange(sock, TCPS_FIN_WAIT_1);
        break;

    case TCPS_CLOSE_WAIT:
        /*
         * RFC 793 is wrong.
         */
        //@@@printf("[%04X]CLOSE_WAIT: going to LAST_ACK\n", (u_short) sock);
        NutTcpStateChange(sock, TCPS_LAST_ACK);
        break;

    case TCPS_FIN_WAIT_1:
    case TCPS_FIN_WAIT_2:
    case TCPS_CLOSING:
    case TCPS_LAST_ACK:
    case TCPS_TIME_WAIT:
        sock->so_last_error = EALREADY;
        return -1;

    default:
        sock->so_last_error = ENOTCONN;
        return -1;
    }
    return 0;
}

/*!
 * \brief Initiated by the application.
 *
 * \param sock Socket descriptor. This pointer must have been
 *             retrieved by calling NutTcpCreateSocket().
 */
int NutTcpStateWindowEvent(TCPSOCKET * sock)
{
    if (sock == 0)
        return -1;
    sock->so_tx_flags |= SO_ACK | SO_FORCE;
    NutTcpOutput(sock, 0, 0);

    return 0;
}

/* ================================================================
 * Timeout events.
 * ================================================================
 */
/*!
 * \brief Retransmit a segment after ACK timeout.
 *
 * This function is called by the TCP timer.
 *
 * \param sock Socket descriptor. This pointer must have been
 *             retrieved by calling NutTcpCreateSocket().
 * \returns Nonzero if socket was aborted due to reach of retransmit
 *          limit or network error.
 *
 */
int NutTcpStateRetranTimeout(TCPSOCKET * sock)
{
    NETBUF *so_tx_next;
    if (sock->so_retransmits++ > TCP_RETRIES_MAX)
    {
        /* Abort the socket */
        NutTcpAbortSocket(sock, ETIMEDOUT);
        return -1;
    } else {
#ifdef NUTDEBUG
        if (__tcp_trf & NET_DBG_SOCKSTATE)
            NutDumpTcpHeader(__tcp_trs, "RET", sock, sock->so_tx_nbq);
#endif
        /* We must save sock->so_tx_nbq->nb_next before calling NutIpOutput,
         * because in case of error the NETBUF is release by NutIpOutput and
         * not longer available.
         */
        so_tx_next = sock->so_tx_nbq->nb_next;
        if (NutIpOutput(IPPROTO_TCP, sock->so_remote_addr, sock->so_tx_nbq)) {
            /* Adjust packet queue */
            sock->so_tx_nbq = so_tx_next;
            /* Abort the socket */
            NutTcpAbortSocket(sock, ENETDOWN);
            return -1;
        } else {
            /* Restart the retransmission timer. */
            sock->so_retran_time = (uint16_t) NutGetMillis() | 1;
            return 0;
        }
    }
}

/* ================================================================
 * Segment arrival events.
 * ================================================================
 */

/*!
 * \brief
 * Process incoming segments in listening state.
 *
 * Wait for a connection request from a remote socket.
 *
 * \param sock Socket descriptor.
 * \param nb   Network buffer structure containing a TCP segment.
 */
static void NutTcpStateListen(TCPSOCKET * sock, uint8_t flags, TCPHDR * th, NETBUF * nb)
{
    /*
     * Got a SYN segment. Store relevant data in our socket
     * structure and switch to TCPS_SYN_RECEIVED.
     */
    if ((flags & (TH_SYN | TH_ACK | TH_RST)) == TH_SYN) {
        NutTcpProcessSyn(sock, nb->nb_nw.vp, th);
        NutTcpStateChange(sock, TCPS_SYN_RECEIVED);
        NutNetBufFree(nb);
    } else
        NutTcpReject(nb);
}


/*!
 * \brief Process incoming segments in SYN-SENT state.
 *
 * Wait for a matching connection request after having sent ours.
 *
 * \param sock Socket descriptor.
 * \param nb   Network buffer structure containing a TCP segment.
 */
static void NutTcpStateSynSent(TCPSOCKET * sock, uint8_t flags, TCPHDR * th, NETBUF * nb)
{
    /*
     * Validate ACK, if set.
     */
    if (flags & TH_ACK) {
        if (!SeqIsBetween(ntohl(th->th_ack), sock->so_tx_isn + 1, sock->so_tx_nxt)) {
            NutTcpReject(nb);
            return;
        }
    }

    /*
     * Handle RST flag. If we were in the LISTEN state,
     * then we return to the LISTEN state, otherwise we
     * abort the connection and go to the CLOSED state.
     */
    if (flags & TH_RST) {
        if (flags & TH_ACK) {
            /*if (sock->so_pc_tq)
                NutTcpStateChange(sock, TCPS_LISTEN);
            else */
                NutTcpAbortSocket(sock, ECONNREFUSED);
        }
        NutNetBufFree(nb);
        return;
    }

    /*
     * Handle SYN flag. If we got a valid ACK too, then
     * our connection is established. Otherwise enter
     * SYNRCVD state.
     */
    if (flags & TH_SYN) {
        NutTcpProcessSyn(sock, nb->nb_nw.vp, th);
        if (flags & TH_ACK) {
            NutTcpProcessAck(sock, th, nb->nb_ap.sz);
            NutTcpStateChange(sock, TCPS_ESTABLISHED);
            /* Wake up the actively connecting thread. */
            NutEventPost(&sock->so_ac_tq);
        } else {
            NutTcpStateChange(sock, TCPS_SYN_RECEIVED);
        }
    }
    NutNetBufFree(nb);
}

/*
 * \brief
 * Process incoming segments in SYN-RECEIVED state.
 *
 * Waiting for a confirming connection request
 * acknowledgment after having both received
 * and sent a connection request.
 *
 * \param sock Socket descriptor.
 * \param nb   Network buffer structure containing a TCP segment.
 */
static void NutTcpStateSynReceived(TCPSOCKET * sock, uint8_t flags, TCPHDR * th, NETBUF * nb)
{
    /*
     * If our previous ack receives a reset response,
     * then we fall back to the listening state.
     */
    if (flags & TH_RST) {
        if (sock->so_pc_tq)
            NutTcpStateChange(sock, TCPS_LISTEN);
        else
            NutTcpAbortSocket(sock, ECONNREFUSED);
        NutNetBufFree(nb);
    	sock->so_retran_time = 0;
        NutTcpDiscardBuffers(sock);
        return;
    }

    /*
     * Reject SYNs.
     */
    if (flags & TH_SYN) {
        NutTcpReject(nb);
        return;
    }

    /*
     * Silently discard segments without ACK.
     */
    if ((flags & TH_ACK) == 0) {
        NutNetBufFree(nb);
        return;
    }

    /*
     * Reject out of window sequence.
     */
    if (!SeqIsBetween(ntohl(th->th_ack), sock->so_tx_una + 1, sock->so_tx_nxt)) {
        NutTcpReject(nb);
        return;
    }

    /* Acknowledge processing. */
    NutTcpProcessAck(sock, th, nb->nb_ap.sz);

    /*
     * Even SYN segments may contain application data, which will be stored
     * in the socket's input buffer. However, there is no need to post an
     * event to any thread waiting for data, because our connection is not
     * yet established.
     */
    if (nb->nb_ap.sz)
        NutTcpProcessAppData(sock, nb);
    else
        NutNetBufFree(nb);

    /*
     * Process state change.
     */
    if (flags & TH_FIN) {
        sock->so_rx_nxt++;
        NutTcpStateChange(sock, TCPS_CLOSE_WAIT);
    } else {
        NutTcpStateChange(sock, TCPS_ESTABLISHED);
        NutEventPost(&sock->so_pc_tq);
        NutEventPost(&sock->so_ac_tq);
    }
}

/*
 * \brief Process incoming segments from established connections.
 *
 * Received application data will be delivered to the application
 * until we receive a FIN segment.
 *
 * \param sock  Socket descriptor.
 * \param flags TCP flags.
 * \param th    Pointer to the TCP header within the NETBUF.
 * \param nb    Network buffer structure containing a TCP segment.
 *
 * \todo We may remove the unused counter of dropped segments, which
 *       were out of sequence.
 */
static void NutTcpStateEstablished(TCPSOCKET * sock, uint8_t flags, TCPHDR * th, NETBUF * nb)
{
    if (flags & TH_RST) {
        NutNetBufFree(nb);
        NutTcpAbortSocket(sock, ECONNRESET);
        return;
    }

    /*
     * Reject SYNs. Silently discard late SYNs
     * (Thanks to Mike Cornelius).
     */
    if (flags & TH_SYN) {
        if (ntohl(th->th_seq) != sock->so_rx_isn)
            NutTcpReject(nb);
        else
            NutNetBufFree(nb);
        return;
    }

    /*
     * Silently discard segments without ACK.
     */
    if ((flags & TH_ACK) == 0) {
        NutNetBufFree(nb);
        return;
    }

    NutTcpProcessAck(sock, th, nb->nb_ap.sz);

    /*
     * If the sequence number of the incoming segment is larger than
     * expected, we probably missed one or more previous segments. Let's
     * add this one to a linked list of segments received in advance and
     * hope that the missing data will arrive later.
     */
    if (SeqIsAfter(ntohl(th->th_seq),sock->so_rx_nxt)) {
        NETBUF *nbq;
        NETBUF **nbqp;
        TCPHDR *thq;
        uint32_t th_seq;
        uint32_t thq_seq;

        if (nb->nb_ap.sz) {
            /* Keep track of the number of bytes used by packets
            ** received in advance. Honor a global limit. */
            tcp_adv_cnt += nb->nb_dl.sz + sizeof(IPHDR) + sizeof (TCPHDR) + nb->nb_ap.sz;
            if (tcp_adv_cnt > tcp_adv_max) {
                /* Limit reached, discard the packet. */
                NutNetBufFree(nb);
                tcp_adv_cnt -= nb->nb_dl.sz + sizeof(IPHDR) + sizeof (TCPHDR) + nb->nb_ap.sz;
            } else {
                nbq = sock->so_rx_nbq;
                nbqp = &sock->so_rx_nbq;
                while (nbq) {
                    thq = (TCPHDR *) (nbq->nb_tp.vp);
                    th_seq = ntohl(th->th_seq);
                    thq_seq = ntohl(thq->th_seq);
                    if (SeqIsAfter(thq_seq, th_seq)) {
                        *nbqp = nb;
                        nb->nb_next = nbq;
                        break;
                    }
                    if (th_seq == thq_seq) {
                        NutNetBufFree(nb);
                        sock->so_tx_flags |= SO_ACK | SO_FORCE;
                        NutTcpOutput(sock, 0, 0);
                        return;
                    }
                    nbqp = &nbq->nb_next;
                    nbq = nbq->nb_next;
                }
                if (nbq == 0) {
                    *nbqp = nb;
                    nb->nb_next = 0;
                }
            }
        } else
            NutNetBufFree(nb);

        sock->so_tx_flags |= SO_ACK | SO_FORCE;
        NutTcpOutput(sock, 0, 0);
        return;
    }

    /*
     * Acknowledge any sequence numbers not expected,
     * even if they do not contain any data. Keepalive
     * packets contain a sequence number one less
     * than the next data expected and they do not
     * contain any data.
     */
    if (ntohl(th->th_seq) != sock->so_rx_nxt) {
        sock->so_tx_flags |= SO_ACK | SO_FORCE;
        /* This seems to be unused. */
        sock->so_oos_drop++;
        NutNetBufFree(nb);
        NutTcpOutput(sock, 0, 0);
    }

    /*
     * The sequence number is exactly what we expected.
     */
    else if (nb->nb_ap.sz) {
        NutTcpProcessAppData(sock, nb);
        /*
         * Process segments we may have received in advance.
         */
        while ((nb = sock->so_rx_nbq) != 0) {
            th = (TCPHDR *) (nb->nb_tp.vp);
            if (SeqIsAfter(ntohl(th->th_seq), sock->so_rx_nxt))
                break;
            sock->so_rx_nbq = nb->nb_next;
            /* Update the heap space used by packets
            ** received in advance. */
            tcp_adv_cnt -= nb->nb_dl.sz + sizeof(IPHDR) + sizeof (TCPHDR) + nb->nb_ap.sz;
            if (ntohl(th->th_seq) == sock->so_rx_nxt) {
                NutTcpProcessAppData(sock, nb);
                flags |= th->th_flags;
            } else
                NutNetBufFree(nb);
        }
        /* Wake up a thread waiting for data. */
        NutEventPost(&sock->so_rx_tq);
    } else {
        NutNetBufFree(nb);
        //sock->so_tx_flags |= SO_ACK | SO_FORCE;
        //NutTcpOutput(sock, 0, 0);
    }
    if (flags & TH_FIN) {
        //@@@printf ("[%04X]ESTABLISHED: going to CLOSE_WAIT\n", (u_short) sock);
        sock->so_rx_nxt++;
        NutTcpStateChange(sock, TCPS_CLOSE_WAIT);
    }
}

/*
 * \brief Process incoming segments in FIN-WAIT1 state.
 *
 * Waiting for a connection termination request
 * from the remote, or an acknowledgment of the
 * connection termination request previously sent.
 *
 * The application already closed the socket.
 *
 * \param sock Socket descriptor.
 * \param nb   Network buffer structure containing a TCP segment.
 *
 * \todo The out of sync case seems to be ignored. Anyway, do we
 *       really need to process application data in this state?
 */
static void NutTcpStateFinWait1(TCPSOCKET * sock, uint8_t flags, TCPHDR * th, NETBUF * nb)
{
    //@@@printf ("[%04X]FIN_WAIT_1: incomming segment, IP %04X\n", (u_short) sock, ntohs(((IPHDR*)nb->nb_nw.vp)->ip_id));
    if (flags & TH_RST) {
        NutNetBufFree(nb);
        NutTcpDestroySocket(sock);
        return;
    }

    /*
     * Reject SYNs.
     */
    if (flags & TH_SYN) {
        NutTcpReject(nb);
        return;
    }

    /*
     * Silently discard segments without ACK.
     */
    if ((flags & TH_ACK) == 0) {
        NutNetBufFree(nb);
        return;
    }

    //@@@if (flags & TH_FIN) printf ("[%04X]FIN_WAIT_1: received FIN\n", (u_short) sock);
    //@@@printf ("[%04X]FIN_WAIT_1: received ACK: %lu, unack: %lu, next: %lu\n", (u_short) sock, ntohl(th->th_ack), sock->so_tx_una, sock->so_tx_nxt);

    //@@@printf ("[%04X]FIN_WAIT_1:  pre processack, nbq: %04X\n", (u_short) sock, (u_short) sock->so_tx_nbq);
    NutTcpProcessAck(sock, th, nb->nb_ap.sz);
    //@@@printf ("[%04X]FIN_WAIT_1: post processack, nbq: %04X\n", (u_short) sock, (u_short) sock->so_tx_nbq);

    /*
     * All segments had been acknowledged, including our FIN.
     */
    if (sock->so_tx_nxt == sock->so_tx_una) {
        //@@@printf ("[%04X]FIN_WAIT_1: going to FIN_WAIT_2\n", (u_short) sock);
        NutTcpStateChange(sock, TCPS_FIN_WAIT_2);
    }

    /*
     * Process application data and release the network buffer.
     * Is this really required?
     */
    if (nb->nb_ap.sz) {
        NutTcpProcessAppData(sock, nb);
        /* Wake up a thread waiting for data. */
        NutEventPost(&sock->so_rx_tq);
    }
    else
        NutNetBufFree(nb);

    if (flags & TH_FIN) {
        sock->so_rx_nxt++;
        /*
         * Our FIN has been acked.
         */
        sock->so_time_wait = 0;
        //@@@printf ("[%04X]FIN_WAIT_1: going to CLOSING\n", (u_short) sock);
        if (sock->so_state == TCPS_FIN_WAIT_2)
            NutTcpStateChange(sock, TCPS_TIME_WAIT);
        else
            NutTcpStateChange(sock, TCPS_CLOSING);
    }
}

/*
 * \brief Process incoming segments in FIN-WAIT2 state.
 *
 * Waiting for a connection termination request
 * from the remote.
 *
 * The application already closed the socket.
 *
 * \param sock Socket descriptor.
 * \param nb   Network buffer structure containing a TCP segment.
 *
 * \todo There's probably no need to process application data.
 */
static void NutTcpStateFinWait2(TCPSOCKET * sock, uint8_t flags, TCPHDR * th, NETBUF * nb)
{
    //@@@printf ("[%04X]FIN_WAIT_2: incomming segment, IP %04X\n", (u_short) sock, ntohs(((IPHDR*)nb->nb_nw.vp)->ip_id));
    if (flags & TH_RST) {
        NutNetBufFree(nb);
        NutTcpDestroySocket(sock);
        return;
    }

    /*
     * Reject SYNs.
     */
    if (flags & TH_SYN) {
        NutTcpReject(nb);
        return;
    }

    /*
     * Silently discard segments without ACK.
     */
    if ((flags & TH_ACK) == 0) {
        NutNetBufFree(nb);
        return;
    }

    //@@@printf ("[%04X]FIN_WAIT_2: received ACK: %lu, unack: %lu, next: %lu\n", (u_short) sock, ntohl(th->th_ack), sock->so_tx_una, sock->so_tx_nxt);
    /*
     * Process acknowledge and application data and release the
     * network buffer.
     */
    NutTcpProcessAck(sock, th, nb->nb_ap.sz);

    //@@@if (sock->so_tx_nbq) printf ("[%04X]FIN_WAIT_2: xmit buffer not empty!", (u_short) sock);
    /* Do we really need this? */
    if (nb->nb_ap.sz) {
        NutTcpProcessAppData(sock, nb);
        /* Wake up a thread waiting for data. */
        NutEventPost(&sock->so_rx_tq);
    }
    else
        NutNetBufFree(nb);

    if (flags & TH_FIN) {
        sock->so_rx_nxt++;
        sock->so_time_wait = 0;
        //@@@printf ("[%04X]FIN_WAIT_2: going to TIME_WAIT\n", (u_short) sock);
        NutTcpStateChange(sock, TCPS_TIME_WAIT);
    }
}

/*
 * \brief
 * Process incoming segments in CLOSE-WAIT state.
 *
 * Waiting for a connection termination request
 * from the local application.
 *
 * \param sock Socket descriptor.
 * \param nb   Network buffer structure containing a TCP segment.
 */
static void NutTcpStateCloseWait(TCPSOCKET * sock, uint8_t flags, TCPHDR * th, NETBUF * nb)
{
    //@@@printf ("[%04X]CLOSE_WAIT: incomming segment, IP %04X\n", (u_short) sock, ((IPHDR*)nb->nb_nw.vp)->ip_id);
    if (flags & TH_RST) {
        NutNetBufFree(nb);
        NutTcpAbortSocket(sock, ECONNRESET);
        return;
    }

    /*
     * Reject SYNs.
     */
    if (flags & TH_SYN) {
        NutTcpReject(nb);
        return;
    }

    /*
     * Silently discard segments without ACK.
     */
    if ((flags & TH_ACK) == 0) {
        NutNetBufFree(nb);
        return;
    }

    NutTcpProcessAck(sock, th, nb->nb_ap.sz);

    NutNetBufFree(nb);
}

/*
 * \brief
 * Process incoming segments in CLOSING state.
 *
 * Waiting for a connection termination request
 * acknowledgment from the remote.
 *
 * The application already closed the socket.
 *
 * \param sock Socket descriptor.
 * \param nb   Network buffer structure containing a TCP segment.
 */
static void NutTcpStateClosing(TCPSOCKET * sock, uint8_t flags, TCPHDR * th, NETBUF * nb)
{
    //@@@printf ("[%04X]CLOSING: Incomming segment\n", (u_short) sock);
    if (flags & TH_RST) {
        NutNetBufFree(nb);
        NutTcpDestroySocket(sock);
        return;
    }

    /*
     * Reject SYNs.
     */
    if (flags & TH_SYN) {
        NutTcpReject(nb);
        return;
    }

    /*
     * Silently discard segments without ACK.
     */
    if ((flags & TH_ACK) == 0) {
        NutNetBufFree(nb);
        return;
    }

    NutTcpProcessAck(sock, th, nb->nb_ap.sz);

    /*
     * All segments had been acknowledged, including our FIN.
     */
    if (sock->so_tx_nxt == sock->so_tx_una) {
        sock->so_time_wait = 0;
        NutTcpStateChange(sock, TCPS_TIME_WAIT);
        //@@@printf ("[%04X]CLOSING: Going to TIME_WAIT\n", (u_short) sock);
    }
    //@@@else printf ("[%04X]CLOSING: NOT changing state\n", (u_short) sock);

    NutNetBufFree(nb);
}

/*!
 * \brief
 * Process incoming segment in LAST_ACK state.
 *
 * Waiting for an acknowledgment of the connection termination
 * request previously sent.
 *
 * The application already closed the socket.
 *
 * \param sock  Socket descriptor.
 * \param flags TCP flags of incoming segment.
 * \param th    TCP header of incoming segment.
 * \param nb    Network buffer structure containing a TCP segment.
 */
static void NutTcpStateLastAck(TCPSOCKET * sock, uint8_t flags, TCPHDR * th, NETBUF * nb)
{
    //@@@printf ("[%04X]LAST_ACK: incomming segment, IP %04X\n", (u_short) sock, ((IPHDR*)nb->nb_nw.vp)->ip_id);
    if (flags & TH_RST) {
        NutNetBufFree(nb);
        NutTcpDestroySocket(sock);
        return;
    }

    /*
     * Reject SYNs.
     */
    if (flags & TH_SYN) {
        NutTcpReject(nb);
        return;
    }

    /*
     * Silently discard segments without ACK.
     */
    if ((flags & TH_ACK) == 0) {
        NutNetBufFree(nb);
        return;
    }

    NutTcpProcessAck(sock, th, nb->nb_ap.sz);
    NutNetBufFree(nb);

    if (sock->so_tx_nxt == sock->so_tx_una)
        NutTcpDestroySocket(sock);
    //@@@else printf ("[%04X]LAST_ACK: no destroy sock\n", (u_short) sock);
}

/*!
 * \brief Process incoming TCP segments.
 *
 * Processing is based on the current state of the socket connection.
 *
 * \param sock Socket descriptor. This pointer must have been
 *             retrieved by calling NutTcpCreateSocket().
 * \param nb   Network buffer structure containing a TCP segment.
 *             Will be released within this routine.
 */
static void NutTcpStateProcess(TCPSOCKET * sock, NETBUF * nb)
{
    uint32_t tx_win;
    uint32_t tx_una;
    TCPHDR *th = (TCPHDR *) nb->nb_tp.vp;
    uint8_t flags = th->th_flags;

#ifdef NUTDEBUG
    if (__tcp_trf & NET_DBG_SOCKSTATE) {
        fprintf(__tcp_trs, " %04x-", (unsigned int) sock);
        NutDumpSockState(__tcp_trs, sock->so_state, "[", ">]");
    }
#endif
    switch (sock->so_state) {
        /* Handle the most common case first. */
    case TCPS_ESTABLISHED:
        tx_win = sock->so_tx_win;
        tx_una = sock->so_tx_una;
        NutTcpStateEstablished(sock, flags, th, nb);
        /* Wake up all threads waiting for transmit, if something interesting happened. */
        if(sock->so_state != TCPS_ESTABLISHED || /* Status changed. */
           sock->so_tx_win > tx_win ||           /* Windows changed. */
           sock->so_tx_una != tx_una) {          /* Unacknowledged data changed. */
            NutEventBroadcast(&sock->so_tx_tq);
        }
        break;
    case TCPS_LISTEN:
        NutTcpStateListen(sock, flags, th, nb);
        break;
    case TCPS_SYN_SENT:
        NutTcpStateSynSent(sock, flags, th, nb);
        break;
    case TCPS_SYN_RECEIVED:
        NutTcpStateSynReceived(sock, flags, th, nb);
        break;
    case TCPS_FIN_WAIT_1:
        NutTcpStateFinWait1(sock, flags, th, nb);
        break;
    case TCPS_FIN_WAIT_2:
        NutTcpStateFinWait2(sock, flags, th, nb);
        break;
    case TCPS_CLOSE_WAIT:
        NutTcpStateCloseWait(sock, flags, th, nb);
        break;
    case TCPS_CLOSING:
        NutTcpStateClosing(sock, flags, th, nb);
        break;
    case TCPS_LAST_ACK:
        NutTcpStateLastAck(sock, flags, th, nb);
        break;
    case TCPS_TIME_WAIT:
        /*
         * Ignore everything while in TIME_WAIT state.
         */
        NutNetBufFree(nb);
        break;
    case TCPS_CLOSED:
        /*
         * Reject everything while in CLOSED state.
         */
        NutTcpReject(nb);
        break;
    default:
        NutNetBufFree(nb);
        break;
    }
}

/*! \fn NutTcpSm(void *arg)
 * \brief TCP state machine thread.
 *
 * The TCP state machine serves two purposes: It processes incoming TCP
 * segments and handles TCP timers.
 */
THREAD(NutTcpSm, arg)
{
    NETBUF *nb;
    NETBUF *nbx;
    TCPHDR *th;
    IPHDR *ih;
    TCPSOCKET *sock;
    uint8_t tac = 0;

    /*
     * It won't help giving us a higher priority than the application
     * code. We depend on the speed of the reading application.
     */
    NutThreadSetPriority (32);

    for (;;) {
        if (++tac > 3 || NutEventWait(&tcp_in_rdy, 200)) {
            tac = 0;

#if TCP_BACKLOG_MAX
            /* Process backlog timer.
            **
            ** Note, that the tac counter will spoil any exact timing.
            ** On the other hand, if we are very busy, it may not be that
            ** bad to kill early SYN segments soon. */
            nb = NutTcpBacklogTimer();
            if (nb) {
                NutTcpReject(nb);
            }
#endif /* TCP_BACKLOG_MAX */

            for (sock = tcpSocketList; sock; sock = sock->so_next) {

                /*
                 * Send late acks.
                 */
                if (sock->so_tx_flags & SO_ACK) {
                    sock->so_tx_flags |= SO_FORCE;
                    NutTcpOutput(sock, 0, 0);
                }

                /*
                 * Process retransmit timer.
                 */
                if (sock->so_tx_nbq && sock->so_retran_time) {
                    if ((uint16_t)((uint16_t)NutGetMillis() - (sock->so_retran_time & ~1)) >= sock->so_rtto) {
                        NutTcpStateRetranTimeout(sock);
                    }
                }


                /*
                 * Destroy sockets after timeout in TIMEWAIT state.
                 */
                if (sock->so_state == TCPS_TIME_WAIT || sock->so_state == TCPS_FIN_WAIT_2) {
                    if (sock->so_time_wait++ >= 9) {
                        NutTcpDestroySocket(sock);
                        break;
                    }
                }

                /*
                 * Recover from SYN flood attacks.
                 */
                else if (sock->so_state == TCPS_SYN_RECEIVED) {
                    if (sock->so_time_wait++ >= 45) {
                        sock->so_state = TCPS_LISTEN;
                        sock->so_time_wait = 0;
                    }
                }
            }
        } else {
            nb = tcp_in_nbq;
            tcp_in_nbq = 0;
            tcp_in_cnt = 0;
            while (nb) {
                ih = (IPHDR *) nb->nb_nw.vp;
                th = (TCPHDR *) nb->nb_tp.vp;
                sock = NutTcpFindSocket(th->th_dport, th->th_sport, ih->ip_src);
#ifdef NUTDEBUG
                if (__tcp_trf & NET_DBG_SOCKSTATE)
                    NutDumpTcpHeader(__tcp_trs, " IN", sock, nb);
#endif
                nbx = nb->nb_next;
                /* If a matching socket exists, process the NETBUF. */
                if (sock) {
                    NutTcpInputOptions(sock, nb);
                    NutTcpStateProcess(sock, nb);
                }
#if TCP_BACKLOG_MAX
                /* No matching socket, try to add it to the backlog. */
                else if (NutTcpBacklogAdd(nb) == 0) {
                }
#endif
                /* No matching socket and no backlog. Reject it. */
                else {
                    NutTcpReject(nb);
                }
                nb = nbx;
            }
        }
    }
}

/*!
 * \brief Process incoming TCP segments.
 *
 * All incoming TCP packets are passed to this routine. They will
 * be added to a global queue and processed by the TCP state
 * machine, which is running on a separate thread.
 *
 * \note This routine is called by the IP layer on incoming
 *       TCP segments. Applications typically do not call
 *       this function.
 */
void NutTcpStateMachine(NETBUF * nb)
{
    NETBUF *nbp;
    uint16_t size;

    nb->nb_next = 0;

    /*
     * Incoming TCP segments are rejected and released if no TCP
     * sockets have been opened. Not doing so would add them
     * to the queue and never release the NETBUF. Thanks to
     * Ralph Mason for this fix.
     */
    if (tcpThread == 0) {
        NutTcpReject(nb);
        return;
    }

    if ((nbp = tcp_in_nbq) == 0) {
        tcp_in_nbq = nb;
        NutEventPost(&tcp_in_rdy);
    } else {
        size = nb->nb_nw.sz + nb->nb_tp.sz + nb->nb_ap.sz;
        if (tcp_in_cnt + size + 2048 < NutHeapAvailable()) {
            tcp_in_cnt += size;
            while (nbp->nb_next)
                nbp = nbp->nb_next;
            nbp->nb_next = nb;
            NutEventPost(&tcp_in_rdy);
        } else
            NutNetBufFree(nb);
    }
}

/*!
 * \brief Start TCP state machine.
 *
 * The socket interface will automatically call this routine as
 * soon as the first socket is created.
 *
 * \return 0 on success, -1 otherwise.
 */
int NutTcpInitStateMachine(void)
{
    if (tcpThread == 0 && (tcpThread = NutThreadCreate("tcpsm", NutTcpSm, NULL,
        (NUT_THREAD_TCPSMSTACK * NUT_THREAD_STACK_MULT) + NUT_THREAD_STACK_ADD)) == 0)
        return -1;
    return 0;
}

/*!
 * \brief Closes socket with error.
 *
 * Aborts any socket activity and sets last error.
 *
 * \param sock  Socket descriptor.
 * \param last_error Error number to report
 *
 * \note This routine is called internally by Nut/Net.
 *       Applications typically do not call this function.
 *
 * \return 0 on success, -1 otherwise.
 */
int NutTcpAbortSocket(TCPSOCKET * sock, uint16_t last_error)
{
    sock->so_last_error = last_error;
    sock->so_retran_time = 0;
    sock->so_time_wait = 0;
    /*
     * If NutTcpCloseSocket was already called, we have to change
     * to TCPS_TIME_WAIT state, otherwise the socket will not be destroyed.
     * For the other cases just go to TCPS_CLOSED.
     */
    if (sock->so_state >= TCPS_FIN_WAIT_1)      /* FIN_WAIT_1, FIN_WAIT_2, CLOSING, LAST_ACK, TIME_WAIT */
        sock->so_state = TCPS_TIME_WAIT;
    else
        sock->so_state = TCPS_CLOSED;
    NutTcpDiscardBuffers(sock);
    NutEventBroadcast(&sock->so_rx_tq);
    NutEventBroadcast(&sock->so_tx_tq);
    NutEventBroadcast(&sock->so_pc_tq);
    NutEventBroadcast(&sock->so_ac_tq);
    return 0;
}

/*@}*/
