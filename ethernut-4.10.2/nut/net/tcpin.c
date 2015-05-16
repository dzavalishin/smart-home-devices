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
 * Revision 1.5  2008/10/05 16:48:52  haraldkipp
 * Security fix. Check various lengths of incoming packets.
 *
 * Revision 1.4  2008/08/20 06:57:00  haraldkipp
 * Implemented IP demultiplexer.
 *
 * Revision 1.3  2008/08/11 07:00:32  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.2  2005/04/30 16:42:42  chaac
 * Fixed bug in handling of NUTDEBUG. Added include for cfg/os.h. If NUTDEBUG
 * is defined in NutConf, it will make effect where it is used.
 *
 * Revision 1.1.1.1  2003/05/09 14:41:39  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.11  2003/02/04 18:14:57  harald
 * Version 3 released
 *
 * Revision 1.10  2002/06/26 17:29:36  harald
 * First pre-release with 2.4 stack
 *
 */

#include <cfg/os.h>
#include <sys/thread.h>

#include <netinet/ip.h>
#include <sys/socket.h>
#include <netinet/tcp.h>

#ifdef NUTDEBUG
#include <net/netdebug.h>
#endif

/*!
 * \addtogroup xgTCP
 */
/*@{*/


/*!
 * \brief Process incoming TCP segments from IP layer.
 *
 * \warning The caller must take care not to pass broadcast
 *          or multicast segments.
 *
 * \note This routine is called by the IP layer on incoming 
 *       TCP  segments. Applications typically do not call 
 *       this function.
 *
 */
int NutTcpInput(NUTDEVICE * dev, NETBUF * nb)
{
    TCPHDR *th = (TCPHDR *) nb->nb_tp.vp;

    /* Process unicasts only. */
    if (th && (nb->nb_flags & NBAF_UNICAST) != 0) {
        uint_fast8_t hdrlen = th->th_off * 4;

        /* Check the header length. */
        if (hdrlen >= sizeof(TCPHDR) && hdrlen <= nb->nb_tp.sz) {
            nb->nb_ap.sz = nb->nb_tp.sz - hdrlen;
            if (nb->nb_ap.sz) {
                nb->nb_ap.vp = ((uint32_t *) th) + th->th_off;
            }
            nb->nb_tp.sz = hdrlen;

            /*
            ** According to RFC1122 we MUST check the checksum
            ** on incoming segments. Anyway, we rely on lower
            ** level checksums to save processing resources.
            **
            ** However, we may combine checksum calculation
            ** with moving data to the receiver buffer.
            **/

            NutTcpStateMachine(nb);

            return 0;
        }
    }
    /* Silently discard this segment. */
    NutNetBufFree(nb);

    return 0;
}

/*@}*/
