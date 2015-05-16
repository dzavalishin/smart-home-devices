#ifndef _NET_IF_H_
#define _NET_IF_H_

/*
 * Copyright (C) 2007 by egnite Software GmbH. All rights reserved.
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
 * Copyright (c) 1982, 1986, 1989, 1993
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

/*!
 * \file net/if.h
 * \brief Network interface header file.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.2  2008/08/11 07:00:19  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.1  2007/05/02 11:15:31  haraldkipp
 * Socket ioctl functions added. Untested.
 *
 *
 * \endverbatim
 */

#include <stdint.h>

/*!
 * \brief Length of interface name, including terminating zero character.
 */
#define IF_NAMESIZE     8

/*!
 * \name Interface Flags
 */
/*@{*/
#define IFF_UP          0x00000001      /*!< \brief Interface is up. */
#define IFF_BROADCAST   0x00000002      /*!< \brief Broadcast address valid. */
#define IFF_DEBUG       0x00000004      /*!< \brief Turn on debugging. */
#define IFF_LOOPBACK    0x00000008      /*!< \brief Is a loopback net. */
#define IFF_POINTOPOINT 0x00000010      /*!< \brief Is a point-to-point link. */
#define IFF_NOARP       0x00000080      /*!< \brief No address resolution protocol. */
#define IFF_PROMISC     0x00000100      /*!< \brief Receive all packets. */
#define IFF_ALLMULTI    0x00000200      /*!< \brief Receive all multicast packets. */
#define IFF_DRV_OACTIVE 0x00000400      /*!< \brief Transmit hardware queue is full. */
#define IFF_SIMPLEX     0x00000800      /*!< \brief Can't hear own transmissions. */
#define IFF_LINK0       0x00001000      /*!< \brief Per link layer defined bit. */
#define IFF_LINK1       0x00002000      /*!< \brief Per link layer defined bit. */
#define IFF_LINK2       0x00004000      /*!< \brief Per link layer defined bit. */
#define IFF_MULTICAST   0x00008000      /*!< \brief Supports multicast. */
#define IFF_PPROMISC    0x00020000      /*!< \brief User-requested promisc mode. */
#define IFF_MONITOR     0x00040000      /*!< \brief User-requested monitor mode. */
#define IFF_STATICARP   0x00080000      /*!< \brief Static ARP. */
/*@}*/

/*!
 * \name Interface Link State
 */
/*@{*/
#define LINK_STATE_UNKNOWN  0   /* Link invalid/unknown. */
#define LINK_STATE_DOWN     1   /* Link is down. */
#define LINK_STATE_UP       2   /* Link is up. */
/*@}*/

/*!
 * \brief Interface request structure used for socket ioctl's.
 */
struct ifreq {
    char ifr_name[IF_NAMESIZE];
    union {
        uint32_t ifru_addr;
        uint32_t ifru_dstaddr;
        uint32_t ifru_broadaddr;
        uint16_t ifru_flags[2];
        uint16_t ifru_index;
        int ifru_metric;
        int ifru_mtu;
        int ifru_phys;
        int ifru_media;
        int ifru_cap[2];
    } ifr_ifru;
};


#endif
