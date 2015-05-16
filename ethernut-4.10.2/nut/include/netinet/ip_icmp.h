#ifndef _NETINET_IP_ICMP_H_
#define _NETINET_IP_ICMP_H_

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
 */

/*
 * $Log$
 * Revision 1.3  2008/10/05 16:46:15  haraldkipp
 * Added missing attributes 'packed'. This may fix a problem with
 * different ARM compiler optimization settings.
 *
 * Revision 1.2  2008/08/11 07:00:23  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.1.1.1  2003/05/09 14:41:15  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.6  2003/02/04 18:00:46  harald
 * Version 3 released
 *
 * Revision 1.5  2002/06/26 17:29:21  harald
 * First pre-release with 2.4 stack
 *
 */

#include <sys/types.h>
#include <net/if_arp.h>
#include <netinet/ip.h>

/*!
 * \file netinet/ip_icmp.h
 * \brief Interface control message protocol definitions.
 *
 * RFC 792.
 */

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Definition of type and code field values.
 */
#define ICMP_ECHOREPLY                  0   /*!< \brief Echo reply type. */
#define ICMP_UNREACH                    3   /*!< \brief Destination unreachable type. */
#define     ICMP_UNREACH_NET            0   /*!< \brief Destination unreachable: Bad net code. */
#define     ICMP_UNREACH_HOST           1   /*!< \brief Destination unreachable: Bad host. */
#define     ICMP_UNREACH_PROTOCOL       2   /*!< \brief Destination unreachable: Bad protocol. */
#define     ICMP_UNREACH_PORT           3   /*!< \brief Destination unreachable: Bad port. */
#define     ICMP_UNREACH_NEEDFRAG       4   /*!< \brief Destination unreachable: Fragmentation needed. */
#define     ICMP_UNREACH_SRCFAIL        5   /*!< \brief Destination unreachable: Source route failed. */
#define     ICMP_UNREACH_NET_UNKNOWN    6   /*!< \brief Destination unreachable: Unknown net. */
#define     ICMP_UNREACH_HOST_UNKNOWN   7   /*!< \brief Destination unreachable: Unknown host. */
#define     ICMP_UNREACH_ISOLATED       8   /*!< \brief Destination unreachable: Source host isolated. */
#define     ICMP_UNREACH_NET_PROHIB     9   /*!< \brief Destination unreachable: Net access prohibited. */
#define     ICMP_UNREACH_HOST_PROHIB    10  /*!< \brief Destination unreachable: Host access prohibited. */
#define     ICMP_UNREACH_TOSNET         11  /*!< \brief Destination unreachable: Bad tos for net. */
#define     ICMP_UNREACH_TOSHOST        12  /*!< \brief Destination unreachable: Bad tos for host. */
#define ICMP_SOURCEQUENCH               4   /*!< \brief Packet lost type. */
#define ICMP_REDIRECT                   5   /*!< \brief Shorter route type. */
#define     ICMP_REDIRECT_NET           0   /*!< \brief Shorter route for network. */
#define     ICMP_REDIRECT_HOST          1   /*!< \brief Shorter route for host. */
#define     ICMP_REDIRECT_TOSNET        2   /*!< \brief Shorter route for tos and net. */
#define     ICMP_REDIRECT_TOSHOST       3   /*!< \brief Shorter route for tos and host. */
#define ICMP_ECHO                       8   /*!< \brief Echo reply type. */
#define ICMP_ROUTERADVERT               9   /*!< \brief Router advertisement type. */
#define ICMP_ROUTERSOLICIT              10  /*!< \brief Router solicitation type. */
#define ICMP_TIMXCEED                   11  /*!< \brief Time exceeded type. */
#define     ICMP_TIMXCEED_INTRANS       0   /*!< \brief Time exceeded in transit. */
#define     ICMP_TIMXCEED_REASS         1   /*!< \brief Time exceeded in reassembly. */
#define ICMP_PARAMPROB                  12  /*!< \brief Bad IP header type. */
#define     ICMP_PARAMPROB_OPTABSENT    1   /*!< \brief Bad IP header: Required option missing. */
#define ICMP_TSTAMP                     13  /*!< \brief Timestamp request type. */
#define ICMP_TSTAMPREPLY                14  /*!< \brief Timestamp reply type. */
#define ICMP_IREQ                       15  /*!< \brief Information request type. */
#define ICMP_IREQREPLY                  16  /*!< \brief Information reply type. */
#define ICMP_MASKREQ                    17  /*!< \brief Address mask request type. */
#define ICMP_MASKREPLY                  18  /*!< \brief Address mask reply type. */

#define ICMP_MAXTYPE                    18  /*!< \brief Maximum ICMP type number. */


#define ICMP_MINLEN 8   /*!< \brief Lower bounds on packet length. */

/*!
 * \struct icmp ip_icmp.h netinet/ip_icmp.h
 * \brief ICMP protocol header structure.
 */
/*!
 * \typedef ICMPHDR
 * \brief ICMP protocol header type.
 */
typedef struct __attribute__ ((packed)) icmp {
    uint8_t  icmp_type;      /*!< \brief Type of message. */
    uint8_t  icmp_code;      /*!< \brief Type sub code. */
    uint16_t icmp_cksum;     /*!< \brief Ones complement header checksum. */
    uint32_t  icmp_spec;      /*!< \brief Type specific information. */
} ICMPHDR;

#ifdef __cplusplus
}
#endif

#endif
