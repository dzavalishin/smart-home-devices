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
 * Revision 1.7  2008/08/11 07:00:32  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.6  2008/04/18 13:32:00  haraldkipp
 * Changed size parameter from u_short to int, which is easier to handle
 * for 32-bit targets. You need to recompile your ARM code. No impact on
 * AVR expected
 * I changed u_int to int at some places to avoid some warnings during
 * compilation of Nut/Net.
 * libs.
 *
 * Revision 1.5  2006/10/08 16:48:22  haraldkipp
 * Documentation fixed
 *
 * Revision 1.4  2006/03/29 01:23:52  olereinhardt
 *  Signednes of strings
 *
 * Revision 1.3  2005/04/08 15:20:51  olereinhardt
 * added <sys/types.h> (__APPLE__) and <netinet/in.h> (__linux__)
 * for htons and simmilar.
 *
 * Revision 1.2  2003/08/14 15:17:50  haraldkipp
 * Caller controls ID increment
 *
 * Revision 1.1.1.1  2003/05/09 14:41:35  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.1  2003/03/31 14:53:28  harald
 * Prepare release 3.1
 *
 * Revision 1.10  2003/02/04 18:14:56  harald
 * Version 3 released
 *
 * Revision 1.9  2002/06/26 17:29:35  harald
 * First pre-release with 2.4 stack
 *
 */

#include <string.h>

#include <dev/ppp.h>

#include <sys/heap.h>
#include <sys/thread.h>
#include <sys/types.h>
#include <net/if_var.h>
#include <net/ppp.h>
#include <netinet/if_ppp.h>
#include <netinet/ppp_fsm.h>
#include <netinet/in.h>
/*!
 * \addtogroup xgPAP
 */
/*@{*/

/*!
 * \brief Send a PAP packet.
 *
 * \note Applications typically do not call this function.
 *
 * \param dev   Identifies the device to use.
 * \param code  Type subcode.
 * \param id    Exchange identifier.
 * \param nb    Network buffer structure containing the packet to send
 *              or null if the packet contains no information.
 *              The structure must have been allocated by a previous
 *              call NutNetBufAlloc() and will be freed when this function
 *              returns.
 *
 * \return 0 on success, -1 in case of any errors.
 */
int NutPapOutput(NUTDEVICE * dev, uint8_t code, uint8_t id, NETBUF * nb)
{
    XCPHDR *xch;

    if ((nb = NutNetBufAlloc(nb, NBAF_NETWORK, sizeof(XCPHDR))) == 0)
        return -1;

    xch = nb->nb_nw.vp;
    xch->xch_code = code;
    xch->xch_id = id;
    xch->xch_len = htons(nb->nb_nw.sz + nb->nb_tp.sz + nb->nb_ap.sz);

    if (NutPppOutput(dev, PPP_PAP, 0, nb) == 0)
        NutNetBufFree(nb);

    return 0;
}

/*
 * Send a Configure-Request.
 */
void PapTxAuthReq(NUTDEVICE *dev, uint8_t id)
{
    PPPDCB *dcb = dev->dev_dcb;
    NETBUF *nb;
    char *cp;
    int len;

    /*
     * Create the request.
     */
    len = 2;
    if(dcb->dcb_user)
        len += strlen((char*)dcb->dcb_user);
    if(dcb->dcb_pass)
        len += strlen((char*)dcb->dcb_pass);
    if ((nb = NutNetBufAlloc(0, NBAF_APPLICATION, len)) != 0) {
        cp = nb->nb_ap.vp;
        *cp = dcb->dcb_user ? (char)strlen((char*)dcb->dcb_user) : 0;
        if(*cp)
            memcpy(cp + 1, dcb->dcb_user, *cp);

        cp += *cp + 1;
        *cp = dcb->dcb_pass ? (char)strlen((char*)dcb->dcb_pass) : 0;
        if(*cp)
            memcpy(cp + 1, dcb->dcb_pass, *cp);

        dcb->dcb_auth_state = PAPCS_AUTHREQ;
        NutPapOutput(dev, XCP_CONFREQ, ++dcb->dcb_reqid, nb);
    }
}

/*@}*/
