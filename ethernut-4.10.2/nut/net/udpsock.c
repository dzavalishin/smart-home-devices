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
 * Revision 1.13  2009/02/22 12:37:26  olereinhardt
 * Added NutUdpError and NutUdpSetSocketError to set and retrieve socket
 * errors. As udp sockets aren't connection oriented those errors will be
 * anounced asynchronously on the next NutUdpSend or NutUdpReceive
 *
 * Include "include/errno.h" instead of "include/net/errno.h"
 *
 * Revision 1.12  2009/02/13 14:52:05  haraldkipp
 * Include memdebug.h for heap management debugging support.
 *
 * Revision 1.11  2009/02/06 15:40:29  haraldkipp
 * Using newly available strdup() and calloc().
 * Replaced NutHeap routines by standard malloc/free.
 * Replaced pointer value 0 by NULL.
 *
 * Revision 1.10  2008/08/20 06:57:00  haraldkipp
 * Implemented IP demultiplexer.
 *
 * Revision 1.9  2008/08/11 07:00:33  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.8  2008/04/18 13:32:00  haraldkipp
 * Changed size parameter from u_short to int, which is easier to handle
 * for 32-bit targets. You need to recompile your ARM code. No impact on
 * AVR expected
 * I changed u_int to int at some places to avoid some warnings during
 * compilation of Nut/Net.
 * libs.
 *
 * Revision 1.7  2005/08/02 17:47:03  haraldkipp
 * Major API documentation update.
 *
 * Revision 1.6  2005/04/08 15:20:51  olereinhardt
 * added <sys/types.h> (__APPLE__) and <netinet/in.h> (__linux__)
 * for htons and simmilar.
 *
 * Revision 1.5  2003/11/26 12:55:12  drsung
 * Portability issues ... again
 *
 * Revision 1.4  2003/11/24 21:01:04  drsung
 * Packet queue added for UDP sockets.
 *
 * Revision 1.3  2003/08/14 15:07:18  haraldkipp
 * Optimization
 *
 * Revision 1.2  2003/07/20 16:01:01  haraldkipp
 * Bugfix: Send crashed on routing problems.
 *
 * Revision 1.1.1.1  2003/05/09 14:41:46  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.13  2003/05/06 18:21:52  harald
 * Memory hole fixed
 *
 * Revision 1.12  2003/04/21 17:04:57  harald
 * Bugfix: Return port number in host byte order
 *
 * Revision 1.11  2003/02/04 18:14:57  harald
 * Version 3 released
 *
 * Revision 1.10  2002/06/26 17:29:36  harald
 * First pre-release with 2.4 stack
 *
 */

#include <sys/heap.h>
#include <sys/event.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <memdebug.h>

/*!
 * \addtogroup xgUdpSocket
 */
/*@{*/


UDPSOCKET *udpSocketList;       /*!< Global linked list of all UDP sockets. */
static uint16_t last_local_port = 4096;  /* Unassigned local port. */
static uint_fast8_t registered;

/*!
 * \brief Create a UDP socket.
 *
 * \param  port Server applications provide the local port number
 *              with this parameter. Client applications should
 *              pass zero.
 *
 * \return Socket descriptor of the newly created UDP socket or
 *         0 if there is not enough memory left.
 *
 */
UDPSOCKET *NutUdpCreateSocket(uint16_t port)
{
    UDPSOCKET *sock;

    if (!registered) {
        if (NutRegisterIpHandler(IPPROTO_UDP, NutUdpInput)) {
            return NULL;
        }
        registered = 1;
    }
    if (port == 0) {
        do {
            if (++last_local_port == 0)
                last_local_port = 4096;
            port = htons(last_local_port);
            sock = udpSocketList;
            while (sock) {
                if (sock->so_local_port == port)
                    break;
                sock = sock->so_next;
            }
        } while (sock);
        port = last_local_port;
    }
    if ((sock = calloc(1, sizeof(UDPSOCKET))) != 0) {
        sock->so_local_port = htons(port);
        sock->so_next = udpSocketList;
        udpSocketList = sock;
    }
    return sock;
}

/*!
 * \brief Send a UDP datagram.
 *
 * \param sock Socket descriptor. This pointer must have been 
 *             retrieved by calling NutUdpCreateSocket().
 * \param addr IP address of the remote host in network byte order.
 * \param port Remote port number in host byte order.
 * \param data Pointer to a buffer containing the data to send.
 * \param len  Number of bytes to be sent.
 *
 * \return 0 on success, -1 otherwise. The specific error code
 *         can be retrieved by calling NutUdpError().
 */

int NutUdpSendTo(UDPSOCKET * sock, uint32_t addr, uint16_t port, void *data, int len)
{
    int rc;
    NETBUF *nb;

#ifdef NUT_UDP_ICMP_SUPPORT    
    if (sock->so_last_error)
        return -1;
#endif
    
    if ((nb = NutNetBufAlloc(0, NBAF_APPLICATION, len)) == 0) {
        sock->so_last_error = ENOMEM;
        return -1;
    }
    memcpy(nb->nb_ap.vp, data, len);

    /* Bugfix by Ralph Mason. We should not free the NETBUF in case of an error. */
    if ((rc = NutUdpOutput(sock, addr, port, nb)) == 0)
        NutNetBufFree(nb);

    return rc;
}

/*!
 * \brief Receive a UDP datagram.
 *
 * \param sock    Socket descriptor. This pointer must have been 
 *                retrieved by calling NutUdpCreateSocket().
 * \param addr    IP address of the remote host in network byte order.
 * \param port    Remote port number in host byte order.
 * \param data    Pointer to the buffer that receives the data.
 * \param size    Size of the buffer that receives the data.
 * \param timeout Maximum number of milliseconds to wait.
 *
 * \return The number of bytes received, if successful. The return
 *         value < 0 indicates an error. The specific error code
 *         can be retrieved by calling NutUdpError().
 *
 * \note Timeout is limited to the granularity of the system timer.
 */
 /* @@@ 2003-10-24: modified by OS for udp packet queue */
int NutUdpReceiveFrom(UDPSOCKET * sock, uint32_t * addr, uint16_t * port, void *data, int size, uint32_t timeout)
{
    IPHDR *ip;
    UDPHDR *uh;
    NETBUF *nb;

#ifdef NUT_UDP_ICMP_SUPPORT    
    /* The ICMP handler might have set an error condition. */
    if (sock->so_last_error)
        return -1;
#endif
    
    if (sock->so_rx_nb == 0)
        NutEventWait(&sock->so_rx_rdy, timeout);

#ifdef NUT_UDP_ICMP_SUPPORT    
    /* An ICMP message might have posted the rx event. So check again */
    if (sock->so_last_error)
        return -1;
#endif
    
    if ((nb = sock->so_rx_nb) == 0)
        return 0;

    /* forward the queue's head to the next packet */
    sock->so_rx_nb = nb->nb_next;

    ip = nb->nb_nw.vp;
    *addr = ip->ip_src;

    uh = nb->nb_tp.vp;
    *port = htons(uh->uh_sport);

    if (size > nb->nb_ap.sz)
        size = nb->nb_ap.sz;

    sock->so_rx_cnt -= nb->nb_ap.sz;    /* decrement input buffer count */

    memcpy(data, nb->nb_ap.vp, size);
    NutNetBufFree(nb);

    return size;
}

/*!
 * \brief Close UDP socket.
 *
 * The memory occupied by the socket is immediately released
 * after calling this function. The application  must not use 
 * the socket after this call.
 *
 * \param sock Socket descriptor. This pointer must have been 
 *             retrieved by calling NutUdpCreateSocket().
 *
 * \return 0 on success, -1 otherwise.
 */
 /* @@@ 2003-10-24: modified by OS for udp packet queue */
int NutUdpDestroySocket(UDPSOCKET * sock)
{
    UDPSOCKET *sp;
    UDPSOCKET **spp;
    int rc = -1;
    NETBUF *nb;

    spp = &udpSocketList;
    sp = udpSocketList;

    while (sp) {
        if (sp == sock) {
            *spp = sock->so_next;
            /* packets may have arrived that the application 
               did not read before closing the socket. */
            while ((nb = sock->so_rx_nb) != 0) {
                sock->so_rx_nb = nb->nb_next;
                NutNetBufFree(nb);
            }
            free(sock);
            rc = 0;
            break;
        }
        spp = &sp->so_next;
        sp = sp->so_next;
    }
    return rc;
}

/*!
 * \brief Return specific code of the last error and the IP address / port of 
 *        the host to which the communication failed
 *
 * Possible error codes are:
 * - ENOTSOCK: Socket operation on non-socket
 * - EMSGSIZE: Message too long
 * - ENOPROTOOPT: Protocol not available
 * - EOPNOTSUPP: Operation not supported on socket
 * - ENETUNREACH: Network is unreachable
 * - ECONNREFUSED: Connection refused
 * - EHOSTDOWN: Host is down
 * - EHOSTUNREACH: No route to host
 *
 * \param sock Socket descriptor. This pointer must have been 
 *             retrieved by calling NutUdpCreateSocket().
 * \param addr IP address of the remote host in network byte order.
 * \param port Remote port number in host byte order.
 *
 * \todo Not all error codes are properly set right now. Some socket
 *       functions return an error without setting an error code.
 */

int NutUdpError(UDPSOCKET * sock, uint32_t * addr, uint16_t * port)
{
    int rc;
    
    if (sock == 0) {
        addr = 0;
        port = 0;
        return ENOTSOCK;
    }
    
    if (sock->so_last_error) {
        rc = sock->so_last_error;
        *port = ntohs(sock->so_remote_port);
        *addr = sock->so_remote_addr;

        sock->so_last_error  = 0;
        sock->so_remote_port = 0;
        sock->so_remote_addr = 0;
        return rc;
    }     
    return 0;
}

/*!
 * \brief Find a matching socket.
 *
 * Loop through all sockets and find a matching one.
 *
 * \note Applications typically do not need to call this function.
 *
 * \param port  Local port number.
 *
 * \return Socket descriptor.
 */
UDPSOCKET *NutUdpFindSocket(uint16_t port)
{
    UDPSOCKET *sp;
    UDPSOCKET *sock = 0;

    for (sp = udpSocketList; sp; sp = sp->so_next) {
        if (sp->so_local_port == port) {
            sock = sp;
            break;
        }
    }
    return sock;
}

/*!
 * \brief Set value of a UDP socket option.
 *
 * The following values can be set:
 *
 * - #SO_RCVBUF   Socket input buffer size (#uint16_t).
 *
 * \param sock    Socket descriptor. This pointer must have been 
 *                retrieved by calling NutUdpCreateSocket().
 * \param optname Option to set.
 * \param optval  Pointer to the value.
 * \param optlen  Length of the value.
 * \return 0 on success, -1 otherwise. 
 */
int NutUdpSetSockOpt(UDPSOCKET * sock, int optname, CONST void *optval, int optlen)
{
    int rc = -1;

    if (sock == 0)
        return -1;
    switch (optname) {

    case SO_RCVBUF:
        if (optval != 0 && optlen == sizeof(uint16_t)) {
            sock->so_rx_bsz = *((uint16_t *) optval);
            rc = 0;
        }
        break;

    default:
        /* sock->so_last_error = ENOPROTOOPT; */
        break;
    }
    return rc;
}

/*!
 * \brief Get a UDP socket option value.
 *
 * The following values can be set:
 *
 * - #SO_RCVBUF   Socket input buffer size (#uint16_t).
 *
 * \param sock    Socket descriptor. This pointer must have been 
 *                retrieved by calling NutUdpCreateSocket().
 * \param optname Option to get.
 * \param optval  Points to a buffer receiving the value.
 * \param optlen  Length of the value buffer.
 *
 * \return 0 on success, -1 otherwise. 
 */
int NutUdpGetSockOpt(UDPSOCKET * sock, int optname, void *optval, int optlen)
{
    int rc = -1;

    if (sock == 0)
        return -1;
    switch (optname) {

    case SO_RCVBUF:
        if (optval != 0 && optlen == sizeof(uint16_t)) {
            *((uint16_t *) optval) = sock->so_rx_bsz;
            rc = 0;
        }
        break;

    default:
        /* sock->so_last_error = ENOPROTOOPT; */
        break;
    }
    return rc;
}

/*!
 * \brief Set a UDP socket error.
 *
 * This function should only be used together (and from) the icmp input routine 
 *    
 * The following values can be set:
 *
 * - #EHOSTUNREACH   Host is unreachable
 *
 * \param sock    Socket descriptor. This pointer must have been 
 *                retrieved by calling NutUdpCreateSocket().
 * \param remote_addr remote ip address in network byte order
 * \param remote_port remote port in network byte order
 * \param error   Error number.
 *
 * \return 0 on success, -1 otherwise. 
 */

int NutUdpSetSocketError(UDPSOCKET * sock, uint32_t remote_addr, uint16_t remote_port, uint16_t error)
{
    if (sock == 0)
        return -1;
    
    sock->so_remote_addr = remote_addr;
    sock->so_remote_port = remote_port;
    sock->so_last_error  = error;

    /* post the event only, if a thread is waiting */
    if (sock->so_rx_rdy)
        NutEventPost(&sock->so_rx_rdy);

    return 0;
}


/*@}*/
