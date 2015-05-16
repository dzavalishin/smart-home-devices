/*
 * Copyright (c) 1983, 1993 by The Regents of the University of California
 * Copyright (C) 2001-2007 by egnite Software GmbH
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
 * $Id: arp.c 3422 2011-05-18 09:46:57Z haraldkipp $
 */

#include <string.h>

#include "dialog.h"
#include "bootmon.h"
#include "ip.h"
#include "arp.h"

/*!
 * \addtogroup xgStack
 */
/*@{*/

static unsigned long arptab_ip;        /*!< \brief ARP entry IP address. */
static unsigned char arptab_ha[6];     /*!< \brief APR entry hardware address. */

ARPENTRY ae;
ETHERHDR arpheader;
ETHERARP arpframe;

/*!
 * \brief Request the MAC address of a specified IP address.
 *
 * \param dip  IP address in network byte order.
 * \param dmac Points to the buffer, that will receive the MAC address.
 *
 * \return 0 on success, -1 otherwise.
 */
int ArpRequest(unsigned long dip, unsigned char *dmac)
{
    ETHERARP *ea;
    unsigned char retry;
    int rlen;

    memset_(dmac, 0xFF, 6);
    if (dip == INADDR_BROADCAST) {
        return 0;
    }

    if (dip == arptab_ip) {
        memcpy_(dmac, arptab_ha, 6);
        return 0;
    }

    /*
     * Set ARP header.
     */
    arpframe.arp_hrd = ARPHRD_ETHER;
    arpframe.arp_pro = ETHERTYPE_IP;
    arpframe.arp_hln = 6;
    arpframe.arp_pln = 4;
    arpframe.arp_op = ARPOP_REQUEST;
    memcpy_(arpframe.arp_sha, confnet.cdn_mac, 6);
    memset_(arpframe.arp_tha, 0xFF, 6);
    arpframe.arp_spa = confnet.cdn_ip_addr;
    arpframe.arp_tpa = dip;


    ea = &rframe.eth.arp;
    for (rlen = retry = 0; retry < 3;) {
        /* 
         * Send a message, if nothing has been received yet.
         */
        if (rlen == 0) {
            /* Transmit failure, must be a NIC problem. Give up. */
            if (EtherOutput(dmac, ETHERTYPE_ARP, sizeof(ETHERARP)) < 0)
                break;
        }
        if ((rlen = EtherInput(ETHERTYPE_ARP, 1000)) <= 0) {
            retry++;
            continue;
        }

        /*
         * Check if the response contains the expected values.
         */
        if (ea->arp_tpa == confnet.cdn_ip_addr && ea->arp_op == ARPOP_REPLY && ea->arp_spa == dip) {
            arptab_ip = dip;
            memcpy_(arptab_ha, ea->arp_sha, 6);
            memcpy_(dmac, ea->arp_sha, 6);
            return 0;
        }
    }
    return -1;
}

/*!
 * \brief Process incoming ARP packets.
 *
 * The incoming IP and hardware address pair is stored. This routine 
 * does not respond to ARP requests.
 */

void ArpRespond(void)
{
    ETHERARP *ea = &rframe.eth.arp;

    if (ea->arp_op == ARPOP_REPLY) {
        if (ea->arp_tpa == confnet.cdn_ip_addr) {        
            ae.ae_ip = ea->arp_spa;
            memcpy_(ae.ae_ha, ea->arp_sha, 6);
        }
    }
    else if (ea->arp_op == ARPOP_REQUEST) {
        if (ea->arp_tpa == confnet.cdn_ip_addr) {
            /* Use the request as a basis for our reply. */
            memcpy_((unsigned char *)&arpframe, (unsigned char *)ea, sizeof(ETHERARP));
            /* The request's source becomes the reply's destination. */ 
            memcpy_(arpframe.arp_tha, ea->arp_sha, 6);
            arpframe.arp_tpa = ea->arp_spa;
            /* Use our configuration for the reply's source. */
            memcpy_(arpframe.arp_sha, confnet.cdn_mac, 6);
            arpframe.arp_spa = confnet.cdn_ip_addr;
            /* Make this a reply and send it . */
            arpframe.arp_op = ARPOP_REPLY;
            EtherOutput(arpframe.arp_tha, ETHERTYPE_ARP, sizeof(ETHERARP));
        }
    }
}

/*@}*/
