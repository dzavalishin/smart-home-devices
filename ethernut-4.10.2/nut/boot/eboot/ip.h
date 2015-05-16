#ifndef _IP_H
#define _IP_H

/*
 * Copyright (C) 2001-2004 by egnite Software GmbH
 * Copyright (C) 2010 by egnite GmbH
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
 * $Id: ip.h 3006 2010-05-10 12:39:44Z haraldkipp $
 */

#include "types.h"

/*!
 * \addtogroup xgStack
 */
/*@{*/

#define INADDR_BROADCAST    (u_long)0xffffffff

/*
 * Protocols
 */
#define IPPROTO_IP      0   /*!< \brief Dummy for IP. */
#define IPPROTO_ICMP    1   /*!< \brief Control message protocol. */
#define IPPROTO_TCP     6   /*!< \brief Transmission control protocol. */
#define IPPROTO_UDP     17  /*!< \brief User datagram protocol. */

#define IPVERSION   4   /*!< \brief IP protocol version. */

#define IP_DF       0x4000  /*!< \brief Don't fragment flag. */
#define IP_MF       0x2000  /*!< \brief More fragments flag. */
#define IP_OFFMASK  0x1fff  /*!< \brief Mask for fragmenting bits. */

typedef struct ip {
    u_char  ip_hl:4,        /*!< \brief Header length. */
            ip_v:4;         /*!< \brief Version. */
    u_char  ip_tos;         /*!< \brief Type of service. */
    short   ip_len;         /*!< \brief Total length. */
    u_short ip_id;          /*!< \brief Identification. */
    short   ip_off;         /*!< \brief Fragment offset field. */
    u_char  ip_ttl;         /*!< \brief Time to live. */
    u_char  ip_p;           /*!< \brief Protocol. */
    u_short ip_sum;         /*!< \brief Checksum. */
    u_long  ip_src;         /*!< \brief Source IP address. */
    u_long  ip_dst;         /*!< \brief Destination IP address. */
} IPHDR;

/*@}*/

extern int IpInput(u_char proto, u_short tms);
extern int IpOutput(u_long dst, u_char proto, u_short len);

#endif
