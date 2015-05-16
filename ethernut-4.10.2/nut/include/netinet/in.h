#ifndef _NETINET_IN_H_

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
 */

/*
 * $Log$
 * Revision 1.6  2009/03/05 22:16:57  freckle
 * use __NUT_EMULATION instead of __APPLE__, __linux__, or __CYGWIN__
 *
 * Revision 1.5  2008/08/11 07:00:23  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.4  2007/05/02 11:18:32  haraldkipp
 * IGMP support added. Incomplete.
 *
 * Revision 1.3  2005/07/26 15:49:59  haraldkipp
 * Cygwin support added.
 *
 * Revision 1.2  2005/04/04 19:33:55  freckle
 * added creation of include/netdb_orig.h, include/sys/socket_orig.h and
 * include/netinet/in_orig.h to allow unix emulation to use tcp/ip sockets
 *
 * Revision 1.1.1.1  2003/05/09 14:41:14  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.5  2003/02/04 18:00:45  harald
 * Version 3 released
 *
 * Revision 1.4  2002/06/26 17:29:21  harald
 * First pre-release with 2.4 stack
 *
 */

#include <sys/types.h>

/* use native version on unix emulation */
#ifdef __NUT_EMULATION__
#include <netinet/in_orig.h>
/* assure _NETINET_IN_H_ is set */
#undef  _NETINET_IN_H_
#define _NETINET_IN_H_
#else  /* unix emulation */

#define _NETINET_IN_H_

/*!
 * \file netinet/in.h
 * \brief Internet definitions.
 *
 * Constants and structures defined by the internet system.
 */

/*
 * Protocols
 */
#define IPPROTO_IP      0   /*!< \brief Dummy for IP. */
#define IPPROTO_ICMP    1   /*!< \brief Control message protocol. */
#define IPPROTO_IGMP    2   /*!< \brief Group management protocol. */
#define IPPROTO_TCP     6   /*!< \brief Transmission control protocol. */
#define IPPROTO_UDP     17  /*!< \brief User datagram protocol. */

#define IPPROTO_RAW     255 /*!< \brief Raw IP packet. */
#define IPPROTO_MAX     256 /*!< \brief Maximum protocol identifier. */


#define IPPORT_RESERVED     1024    /*!< \brief Last reserved port number. */
#define IPPORT_USERRESERVED 5000    /*!< \brief User reserved port number. */

/*!
 * \brief Any IP address.
 */
#define INADDR_ANY          (uint32_t)0x00000000

/*!
 * \brief Broadcast IP address.
 */
#define INADDR_BROADCAST    (uint32_t)0xffffffff

/*
 * Definitions of bits in internet address integers.
 */
#ifdef __BIG_ENDIAN__
#define IN_CLASSA(i)        (((uint32_t)(i) & 0x80000000) == 0)
#define IN_CLASSA_NET       0xff000000
#define IN_CLASSB(i)        (((uint32_t)(i) & 0xc0000000) == 0x80000000)
#define IN_CLASSB_NET       0xffff0000
#define IN_CLASSC(i)        (((uint32_t)(i) & 0xe0000000) == 0xc0000000)
#define IN_CLASSC_NET       0xffffff00
#define IN_CLASSD(i)        (((uint32_t)(i) & 0xf0000000) == 0xe0000000)
#define IN_CLASSD_NET       0xf0000000
#else /* __BIG_ENDIAN__ */
#define IN_CLASSA(i)        (((uint32_t)(i) & 0x00000080) == 0)
#define IN_CLASSA_NET       0x000000ff
#define IN_CLASSB(i)        (((uint32_t)(i) & 0x000000c0) == 0x00000080)
#define IN_CLASSB_NET       0x0000ffff
#define IN_CLASSC(i)        (((uint32_t)(i) & 0x000000e0) == 0x000000c0)
#define IN_CLASSC_NET       0x00ffffff
#define IN_CLASSD(i)        (((uint32_t)(i) & 0x000000f0) == 0x000000e0)
#define IN_CLASSD_NET       0x0000000f
#endif /* __BIG_ENDIAN__ */

#define IN_MULTICAST(i)     IN_CLASSD(i)

#ifdef __BIG_ENDIAN__
#define INADDR_UNSPEC_GROUP     (uint32_t)0xe0000000  /* 224.0.0.0 */
#define INADDR_ALLHOSTS_GROUP   (uint32_t)0xe0000001  /* 224.0.0.1 */
#define INADDR_ALLRTRS_GROUP    (uint32_t)0xe0000002  /* 224.0.0.2 */
#define INADDR_ALLRPTS_GROUP    (uint32_t)0xe0000016  /* 224.0.0.22, IGMPv3 */
#define INADDR_CARP_GROUP       (uint32_t)0xe0000012  /* 224.0.0.18 */
#define INADDR_PFSYNC_GROUP     (uint32_t)0xe00000f0  /* 224.0.0.240 */
#define INADDR_ALLMDNS_GROUP    (uint32_t)0xe00000fb  /* 224.0.0.251 */
#define INADDR_MAX_LOCAL_GROUP  (uint32_t)0xe00000ff  /* 224.0.0.255 */
#else /* __BIG_ENDIAN__ */
#define INADDR_UNSPEC_GROUP     (uint32_t)0x000000e0  /* 224.0.0.0 */
#define INADDR_ALLHOSTS_GROUP   (uint32_t)0x010000e0  /* 224.0.0.1 */
#define INADDR_ALLRTRS_GROUP    (uint32_t)0x020000e0  /* 224.0.0.2 */
#define INADDR_ALLRPTS_GROUP    (uint32_t)0x160000e0  /* 224.0.0.22, IGMPv3 */
#define INADDR_CARP_GROUP       (uint32_t)0x120000e0  /* 224.0.0.18 */
#define INADDR_PFSYNC_GROUP     (uint32_t)0xf00000e0  /* 224.0.0.240 */
#define INADDR_ALLMDNS_GROUP    (uint32_t)0xfb0000e0  /* 224.0.0.251 */
#define INADDR_MAX_LOCAL_GROUP  (uint32_t)0xff0000e0  /* 224.0.0.255 */
#endif /* __BIG_ENDIAN__ */

#ifdef __BIG_ENDIAN__
#define INADDR_LOOPBACK     (uint32_t)0x7f000001
#else /* __BIG_ENDIAN__ */
#define INADDR_LOOPBACK     (uint32_t)0x0100007f
#endif /* __BIG_ENDIAN__ */

/*!
 * \brief Official loopback net address.
 */
#define IN_LOOPBACKNET      127
#endif /* unix emulation */

#endif
