/*
 * Copyright (C) 2001-2004 by egnite Software GmbH. All rights reserved.
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
 * Revision 1.5  2008/08/11 07:00:29  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.4  2004/03/08 11:22:16  haraldkipp
 * Not all compilers like pointer calculation with void pointers
 *
 * Revision 1.3  2004/02/06 19:24:00  drsung
 * Bugfix. After last changes, ping didn't work any more. Thanks to Pavel Celeda, who discovered this bug.
 *
 * Revision 1.2  2004/02/02 18:59:25  drsung
 * Some more ICMP support added.
 *
 * Revision 1.1.1.1  2003/05/09 14:41:28  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.11  2003/03/31 12:02:27  harald
 * Check NEBUF allocation
 *
 * Revision 1.10  2003/02/04 18:14:57  harald
 * Version 3 released
 *
 * Revision 1.9  2002/06/26 17:29:35  harald
 * First pre-release with 2.4 stack
 *
 */

#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <netinet/ipcsum.h>
#include <netinet/icmp.h>
#include <string.h>

/*!
 * \addtogroup xgICMP
 */
/*@{*/
/*!
 * \brief Send an ICMP datagram.
 *
 * Known ICMP types are:
 *
 * - #ICMP_ECHOREPLY Echo reply
 * - #ICMP_UNREACH Destination unreachable
 * - #ICMP_SOURCEQUENCH Packet lost
 * - #ICMP_REDIRECT Shorter route
 * - #ICMP_ECHO Echo reply
 * - #ICMP_ROUTERADVERT Router advertisement
 * - #ICMP_ROUTERSOLICIT Router solicitation
 * - #ICMP_TIMXCEED Time exceeded
 * - #ICMP_PARAMPROB Bad IP header
 * - #ICMP_TSTAMP Timestamp request
 * - #ICMP_TSTAMPREPLY Timestamp reply
 * - #ICMP_IREQ Information request
 * - #ICMP_IREQREPLY Information reply
 * - #ICMP_MASKREQ Address mask request
 * - #ICMP_MASKREPLY Address mask reply
 *
 * \param type Type of the ICMP message.
 * \param dest Destination IP address.
 * \param nb   Network buffer structure containing the datagram.
 *
 * \return 0 on success, -1 otherwise.
 */
int NutIcmpOutput(uint8_t type, uint32_t dest, NETBUF * nb)
{
    ICMPHDR *icp;
    uint16_t csum;

    icp = (ICMPHDR *) nb->nb_tp.vp;
    icp->icmp_type = type;
    icp->icmp_cksum = 0;

    csum = NutIpChkSumPartial(0, nb->nb_tp.vp, nb->nb_tp.sz);
    icp->icmp_cksum = NutIpChkSum(csum, nb->nb_ap.vp, nb->nb_ap.sz);

    return NutIpOutput(IPPROTO_ICMP, dest, nb);
}

/*!
 * \brief Send an ICMP message to a given destination.
 *
 * \param type Type of the ICMP message. See NutIcmpOutput() for 
 *             a list of valid types.
 * \param code Type subcode.
 * \param spec Type specific data item.
 * \param dest IP address of the target.
 * \param nb   Network buffer structure containing the message to be sent.
 *             The structure must have been allocated by a previous
 *             call NutNetBufAlloc() and will be freed if this function
 *             returns with an error.
 *
 * \return 0 on success, -1 otherwise.
 */
int NutIcmpReply(uint8_t type, uint8_t code, uint32_t spec, uint32_t dest, NETBUF * nb)
{
    ICMPHDR *icp;

    if ((nb = NutNetBufAlloc(nb, NBAF_TRANSPORT, sizeof(ICMPHDR))) == 0)
        return -1;

    icp = (ICMPHDR *) nb->nb_tp.vp;
    icp->icmp_code = code;
    icp->icmp_spec = spec;

    return NutIcmpOutput(type, dest, nb);
}

/*!
 * \brief Send an ICMP message as a response to a given destination.
 *
 * \param type Type of the ICMP message. See NutIcmpOutput() for 
 *             a list of valid types.
 * \param code Type subcode.
 * \param spec Type specific data item.
 * \param nb   Network buffer structure containing the previously recevied
 *             network packet. According to RFC792 the complete ip header 
 *             and the first 8 bytes of the transport netbuf is used as the
 *             application data for the response. If this function returns 
 *             with an error, the buffer is freed. The destination addess is
 *             taken from the ip header.
 *
 * \return 0 on success, -1 otherwise.
 */
int NutIcmpResponse(uint8_t type, uint8_t code, uint32_t spec, NETBUF * nb)
{
    IPHDR *ip;
    uint32_t dest;

    ip = nb->nb_nw.vp;
    dest = ip->ip_src;

    if ((nb = NutNetBufAlloc(nb, NBAF_APPLICATION, sizeof(IPHDR) + 8)) == 0)
        return -1;

    memcpy(nb->nb_ap.vp, nb->nb_nw.vp, sizeof(IPHDR));
    memcpy((char *)nb->nb_ap.vp + sizeof(IPHDR), nb->nb_tp.vp, 8);

    return NutIcmpReply(type, code, spec, dest, nb);
}


/*@}*/
