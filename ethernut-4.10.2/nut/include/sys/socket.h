#ifndef _SYS_SOCKET_H_

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
 */

/*
 * $Log$
 * Revision 1.12  2009/02/22 12:37:26  olereinhardt
 * Added NutUdpError and NutUdpSetSocketError to set and retrieve socket
 * errors. As udp sockets aren't connection oriented those errors will be
 * anounced asynchronously on the next NutUdpSend or NutUdpReceive
 *
 * Include "include/errno.h" instead of "include/net/errno.h"
 *
 * Revision 1.11  2008/08/20 06:56:59  haraldkipp
 * Implemented IP demultiplexer.
 *
 * Revision 1.10  2008/08/11 07:00:27  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.9  2008/04/18 13:32:00  haraldkipp
 * Changed size parameter from u_short to int, which is easier to handle
 * for 32-bit targets. You need to recompile your ARM code. No impact on
 * AVR expected
 * I changed u_int to int at some places to avoid some warnings during
 * compilation of Nut/Net.
 * libs.
 *
 * Revision 1.8  2005/07/26 15:49:59  haraldkipp
 * Cygwin support added.
 *
 * Revision 1.7  2005/04/08 10:01:21  freckle
 * removed #defines from unix emulation as provided by system headers
 *
 * Revision 1.6  2005/04/04 19:33:54  freckle
 * added creation of include/netdb_orig.h, include/sys/socket_orig.h and
 * include/netinet/in_orig.h to allow unix emulation to use tcp/ip sockets
 *
 * Revision 1.5  2004/07/30 19:54:46  drsung
 * Some code of TCP stack redesigned. Round trip time calculation is now
 * supported. Fixed several bugs in TCP state machine. Now TCP connections
 * should be more reliable under heavy traffic or poor physical connections.
 *
 * Revision 1.4  2004/03/16 16:48:44  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 * Revision 1.3  2004/02/02 19:03:09  drsung
 * Some more ICMP support added.
 *
 * Revision 1.2  2003/11/24 21:00:21  drsung
 * Packet queue added for UDP sockets.
 *
 * Revision 1.1.1.1  2003/05/09 14:41:21  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.7  2003/02/04 18:00:53  harald
 * Version 3 released
 *
 * Revision 1.6  2002/09/15 16:46:57  harald
 * Error prototype added
 *
 * Revision 1.5  2002/06/26 17:29:29  harald
 * First pre-release with 2.4 stack
 *
 */

#include <cfg/udp.h>

#if defined(__linux__) || defined(__APPLE__) || defined(__CYGWIN__)

/* use native version on unix emulation */
#include <sys/socket_orig.h>

#else /* embedded systems */
#include <compiler.h>

/*!
* \addtogroup xgSocket
 */

/*
 * Types
 */
#define SOCK_STREAM 1       /*!< \brief Stream socket */
#define SOCK_DGRAM  2       /*!< \brief Datagram socket */
#define SOCK_RAW    3       /*!< \brief Raw-protocol interface */

/*
 * Option flags per-socket.
 */
#define SO_DEBUG        0x0001      /*!< \brief Turn on debugging info recording */
#define SO_ACCEPTCONN   0x0002      /*!< \brief Socket has had listen() */
#define SO_REUSEADDR    0x0004      /*!< \brief Allow local address reuse */
#define SO_KEEPALIVE    0x0008      /*!< \brief Keep connections alive */
#define SO_DONTROUTE    0x0010      /*!< \brief Just use interface addresses */
#define SO_BROADCAST    0x0020      /*!< \brief Permit sending of broadcast msgs */
#define SO_USELOOPBACK  0x0040      /*!< \brief Bypass hardware when possible */
#define SO_LINGER       0x0080      /*!< \brief Linger on close if data present */
#define SO_OOBINLINE    0x0100      /*!< \brief Leave received OOB data in line */
#define SO_REUSEPORT    0x0200      /*!< \brief Allow local address & port reuse */

/*
 * Additional options, not kept in so_options.
 */
#define SO_SNDBUF       0x1001      /*!< \brief Send buffer size */
#define SO_RCVBUF       0x1002      /*!< \brief Receive buffer size */
#define SO_SNDLOWAT     0x1003      /*!< \brief Send low-water mark */
#define SO_RCVLOWAT     0x1004      /*!< \brief Receive low-water mark */
#define SO_SNDTIMEO     0x1005      /*!< \brief Send timeout */
#define SO_RCVTIMEO     0x1006      /*!< \brief Receive timeout */
#define SO_ERROR        0x1007      /*!< \brief Get error status and clear */
#define SO_TYPE         0x1008      /*!< \brief Get socket type */

/*
 * Address families.
 */
#define AF_INET     2       /*!< \brief internetwork: UDP, TCP, etc. */

#endif /* unix / embedded */


/* assure _SYS_SOCKET_H_ is set */
#undef  _SYS_SOCKET_H_
#define _SYS_SOCKET_H_

#include <sys/sock_var.h>

/*!
 * \file sys/socket.h
 * \brief UDP and TCP socket interface definitions.
 */

__BEGIN_DECLS

extern TCPSOCKET *NutTcpCreateSocket(void);
extern int NutTcpSetSockOpt(TCPSOCKET *sock, int optname, CONST void *optval, int optlen);
extern int NutTcpGetSockOpt(TCPSOCKET *sock, int optname, void *optval, int optlen);
extern int NutTcpConnect(TCPSOCKET *sock, uint32_t addr, uint16_t port);
extern int NutTcpAccept(TCPSOCKET *sock, uint16_t port);
extern int NutTcpInput(NUTDEVICE * dev, NETBUF *nb);
extern int NutTcpSend(TCPSOCKET *sock, CONST void *data, int len);
#ifdef __HARVARD_ARCH__
extern int NutTcpSend_P(TCPSOCKET *sock, PGM_P data, int len);
#endif
extern int NutTcpCloseSocket(TCPSOCKET *sock);
extern void NutTcpDestroySocket(TCPSOCKET *sock);
extern int NutTcpReceive(TCPSOCKET *sock, void *data, int size);
extern TCPSOCKET *NutTcpFindSocket(uint16_t lport, uint16_t rport, uint32_t raddr);
extern int NutTcpError(TCPSOCKET *sock);
extern int NutTcpAbortSocket(TCPSOCKET *sock, uint16_t last_error);
extern void NutTcpDiscardBuffers(TCPSOCKET * sock);

extern int NutTcpDeviceRead(TCPSOCKET *sock, void *buffer, int size);
extern int NutTcpDeviceWrite(TCPSOCKET *sock, CONST void *buffer, int size);
#ifdef __HARVARD_ARCH__
extern int NutTcpDeviceWrite_P(TCPSOCKET *sock, PGM_P buffer, int size);
#endif
extern int NutTcpDeviceIOCtl(TCPSOCKET *sock, int cmd, void *param);
extern int NutTcpDeviceClose(TCPSOCKET *sock);

extern UDPSOCKET *NutUdpCreateSocket(uint16_t port);
extern int NutUdpSendTo(UDPSOCKET *sock, uint32_t addr, uint16_t port, void *data, int len);
extern int NutUdpReceiveFrom(UDPSOCKET *sock, uint32_t *addr, uint16_t *port, void *data, int size, uint32_t timeout);
extern int NutUdpDestroySocket(UDPSOCKET *sock);
extern UDPSOCKET *NutUdpFindSocket(uint16_t port);
extern int NutUdpSetSockOpt(UDPSOCKET *sock, int optname, CONST void *optval, int optlen);
extern int NutUdpGetSockOpt(UDPSOCKET *sock, int optname, void *optval, int optlen);

#ifdef NUT_UDP_ICMP_SUPPORT    
extern int NutUdpSetSocketError(UDPSOCKET * sock, uint32_t remote_addr, uint16_t remote_port, uint16_t error);
extern int NutUdpError(UDPSOCKET * sock, uint32_t * addr, uint16_t * port);
#endif
    
__END_DECLS

#endif
