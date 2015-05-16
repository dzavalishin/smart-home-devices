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
 * Revision 1.4  2008/10/05 16:48:52  haraldkipp
 * Security fix. Check various lengths of incoming packets.
 *
 * Revision 1.3  2005/04/08 15:20:50  olereinhardt
 * added <sys/types.h> (__APPLE__) and <netinet/in.h> (__linux__)
 * for htons and simmilar.
 *
 * Revision 1.2  2004/07/27 19:41:00  drsung
 * If NutArpOutput fails, the NetBuf must not be deallocated by
 * calling NutNetBufFree.
 *
 * Revision 1.1.1.1  2003/05/09 14:41:26  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.9  2003/02/04 18:14:56  harald
 * Version 3 released
 *
 * Revision 1.8  2002/06/26 17:29:35  harald
 * First pre-release with 2.4 stack
 *
 */
#include <sys/types.h>
#include <net/if_var.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
/*!
 * \addtogroup xgARP
 *
 * \todo Response may reuse received ARP packet.
 */
/*@{*/


/*!
 * \brief Handle incoming ARP packets.
 *
 * Packets not destined to us or packets with unsupported 
 * address type or item length are silently discarded.
 *
 * \note This routine is called by the Ethernet layer on
 *       incoming ARP packets. Applications typically do
 *       not call this function.
 *
 * \param dev Identifies the device that received the packet.
 * \param nb  Pointer to a network buffer structure containing 
 *            the ARP packet.
 */
void NutArpInput(NUTDEVICE * dev, NETBUF * nb)
{
    ETHERARP *ea;
    ARPHDR *ah;
    IFNET *nif;

    if (nb->nb_nw.sz < sizeof(ETHERARP)) {
        NutNetBufFree(nb);
        return;
    }

    ea = (ETHERARP *) nb->nb_nw.vp;
    ah = (ARPHDR *) & ea->ea_hdr;

    /*
     * Silently discard packets with unsupported
     * address types and lengths.
     */
    if (ntohs(ah->ar_hrd) != ARPHRD_ETHER ||
        ntohs(ah->ar_pro) != ETHERTYPE_IP ||
        ah->ar_hln != 6 || ah->ar_pln != 4) {
        NutNetBufFree(nb);
        return;
    }

    /*
     * Silently discard packets for other destinations.
     */
    nif = dev->dev_icb;
    if (ea->arp_tpa != nif->if_local_ip) {
        NutNetBufFree(nb);
        return;
    }

    /*
     * TODO: Silently discard packets with our own 
     * source address.
     */

    /*
     * TODO: Discard packets with broadcast source
     * address.
     */

    /*
     * TODO: Discard packets if source IP address
     * equals our address. We should send a reply.
     */

    /*
     * Add the sender to our arp cache. Note, that we don't do
     * this with replies only, but also with requests on our
     * address. The assumption is that if someone is requesting
     * our address, they are probably intending to talk to us,
     * so it saves time if we cache their address.
     */
    NutArpCacheUpdate(dev, ea->arp_spa, ea->arp_sha);

    /*
     * Reply to ARP requests.
     */
    if (htons(ea->ea_hdr.ar_op) == ARPOP_REQUEST) {
        NETBUF *nbr =
            NutArpAllocNetBuf(ARPOP_REPLY, ea->arp_spa, ea->arp_sha);

        if (nbr) {
            if (!NutArpOutput(dev, nbr))
                NutNetBufFree(nbr);
        }
    }
    NutNetBufFree(nb);
}

/*@}*/
