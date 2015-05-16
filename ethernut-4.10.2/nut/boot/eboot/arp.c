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
 * $Id: arp.c 3006 2010-05-10 12:39:44Z haraldkipp $
 */

#include <string.h>

#include "eboot.h"
#include "config.h"
#include "ip.h"
#include "arp.h"

/*!
 * \addtogroup xgStack
 */
/*@{*/

ARPENTRY ae;
ARPFRAME arpframe;

/*!
 * \brief Request the MAC address of a specified IP address.
 *
 * \param dip  IP address in network byte order.
 * \param dmac Points to the buffer, that will receive the MAC address.
 *
 * \return 0 on success, -1 otherwise.
 */
int ArpRequest(u_long dip, u_char * dmac)
{
    ETHERARP *ea = &arpframe.eth_arp;
    u_char retry;
    int rlen;
    u_char i;

    if (dip == INADDR_BROADCAST) {
        for (i = 0; i < 6; i++)
            dmac[i] = 0xFF;
        return 0;
    }
    if (ae.ae_ip != dip) {

        /*
         * Set ARP header.
         */
        ea->arp_hrd = htons(ARPHRD_ETHER);
        ea->arp_pro = ETHERTYPE_IP;
        ea->arp_hln = 6;
        ea->arp_pln = 4;
        ea->arp_op = htons(ARPOP_REQUEST);
        for (i = 0; i < 6; i++)
            ea->arp_sha[i] = confnet.cdn_mac[i];
        ea->arp_spa = confnet.cdn_ip_addr;
        for (i = 0; i < 6; i++)
            ea->arp_tha[i] = 0xFF;
        ea->arp_tpa = dip;

        for (rlen = retry = 0; rlen == 0 && retry < 10; retry++) {
            if (EtherOutput(0, ETHERTYPE_ARP, sizeof(ETHERARP)) < 0)
                break;
            if ((rlen = EtherInput(ETHERTYPE_ARP, 1000)) > 0) {
                ARPFRAME *af = (ARPFRAME *) & rframe;

                ea = &af->eth_arp;
                if (ea->arp_tpa == confnet.cdn_ip_addr) {
                    if (htons(ea->arp_op) == ARPOP_REPLY) {
                        ae.ae_ip = ea->arp_spa;
                        for (i = 0; i < 6; i++)
                            ae.ae_ha[i] = ea->arp_sha[i];
                        break;
                    }
                }
                rlen = 0;
            }
        }
    }
    if (ae.ae_ip != dip)
        return -1;

    for (i = 0; i < 6; i++)
        dmac[i] = ae.ae_ha[i];

    return 0;
}

/*!
 * \brief Process incoming ARP packets.
 *
 * The incoming IP and hardware address pair is stored. This routine 
 * does not respond to ARP requests.
 */

void ArpRespond(void)
{
    u_char i;
    ARPFRAME *af = (ARPFRAME *) & rframe;
    ETHERARP *ea = &af->eth_arp;

    ea = &arpframe.eth_arp;
    if (ea->arp_tpa == confnet.cdn_ip_addr) {
        if (htons(ea->arp_op) == ARPOP_REPLY) {
            ae.ae_ip = ea->arp_spa;
            for (i = 0; i < 6; i++)
                ae.ae_ha[i] = ea->arp_sha[i];
        } 
    } else 
    if (ea->arp_spa == confnet.cdn_ip_addr) {
        if (htons(ea->arp_op) == ARPOP_REQUEST) {          
            /*
             * Set ARP header.
             */
            ea->arp_hrd = htons(ARPHRD_ETHER);
            ea->arp_pro = ETHERTYPE_IP;
            ea->arp_hln = 6;
            ea->arp_pln = 4;
            ea->arp_op = htons(ARPOP_REPLY);
    
            /*
             * Set ARP destination data.
             */
            for (i = 0; i < 6; i++) 
                ea->arp_tha[i] = ea->arp_sha[i];
            ea->arp_tpa = ea->arp_spa;
            
            for (i = 0; i < 6; i++)
                ea->arp_sha[i] = confnet.cdn_mac[i];
            ea->arp_spa = confnet.cdn_ip_addr;

            EtherOutput(0, ETHERTYPE_ARP, sizeof(ETHERARP));
        }
    }
}

/*@}*/
