#ifndef _ARP_H
#define _ARP_H

/*
 * Copyright (C) 1983, 1993 by The Regents of the University of California
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
 * $Id: arp.h 3006 2010-05-10 12:39:44Z haraldkipp $
 */

#include "types.h"

#include "ether.h"

/*!
 * \addtogroup xgStack
 */
/*@{*/

#define ARPHRD_ETHER    1   /*!< \brief ethernet hardware format */

#define ARPOP_REQUEST   1   /*!< \brief request to resolve address */
#define ARPOP_REPLY     2   /*!< \brief response to previous request */

/*!
 * \typedef ETHERARP
 * \brief Ethernet ARP protocol type.
 */
typedef struct ether_arp {
    u_short arp_hrd;    /*!< \brief Format of hardware address. 
                        *   Nut/Net supports ARPHRD_ETHER only. 
                        */
    u_short arp_pro;    /*!< \brief Format of protocol address.
                        *   Only ETHERTYPE_IP is supported by Nut/Net.
                        */
    u_char  arp_hln;    /*!< \brief Length of hardware address. */
    u_char  arp_pln;    /*!< \brief Length of protocol address. */
    u_short arp_op;     /*!< \brief ARP operation.
                        *   May be either ARPOP_REQUEST or ARPOP_REPLY.
                        */
    u_char arp_sha[6]; /*!< \brief Source hardware address. */
    u_long arp_spa;    /*!< \brief Source protocol address. */
    u_char arp_tha[6]; /*!< \brief Target hardware address. */
    u_long arp_tpa;    /*!< \brief Target protocol address. */
} ETHERARP;

typedef struct {
    ETHERHDR eth_hdr;
    ETHERARP eth_arp;
} ARPFRAME;

extern ARPFRAME arpframe;

typedef struct {
    u_long ae_ip;       /*!< \brief IP address. */
    u_char ae_ha[6];    /*!< \brief Hardware address. */
} ARPENTRY;

/*@}*/

extern int ArpRequest(u_long ip, u_char *mac);
extern void ArpRespond(void);

#endif

