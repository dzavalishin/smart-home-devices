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
 * Revision 1.6  2008/08/11 06:59:42  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.5  2008/08/09 17:35:32  haraldkipp
 * Simplified NETBUF allocation and release.
 *
 * Revision 1.4  2008/04/18 13:31:59  haraldkipp
 * Changed size parameter from uint16_t to int, which is easier to handle
 * for 32-bit targets. You need to recompile your ARM code. No impact on
 * AVR expected
 * I changed u_int to int at some places to avoid some warnings during
 * compilation of Nut/Net.
 * libs.
 *
 * Revision 1.3  2005/04/30 16:42:41  chaac
 * Fixed bug in handling of NUTDEBUG. Added include for cfg/os.h. If NUTDEBUG
 * is defined in NutConf, it will make effect where it is used.
 *
 * Revision 1.2  2004/01/16 07:51:43  drsung
 * Bugfix for reallocating smaller network buffers. Thx to Mike Cornelius.
 *
 * Revision 1.1.1.1  2003/05/09 14:40:46  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.12  2003/03/31 14:53:07  harald
 * Prepare release 3.1
 *
 * Revision 1.11  2003/02/04 17:50:54  harald
 * Version 3 released
 *
 * Revision 1.10  2002/06/26 17:29:08  harald
 * First pre-release with 2.4 stack
 *
 */

#include <cfg/os.h>
#include <string.h>

#include <sys/heap.h>
#include <dev/netbuf.h>

#include <sys/nutdebug.h>

/*!
 * \addtogroup xgnetbuf
 */
/*@{*/

static int NutNetBufAllocData(NBDATA * nbd, int size)
{
    nbd->vp = NutHeapAlloc(size);
    if (nbd->vp) {
        nbd->sz = size;
        return 0;
    }
    return -1;
}

/*!
 * \brief Allocate or re-allocate a network buffer part.
 *
 * \param nb   Points to an existing network buffer structure or NULL, 
 *             if a new structure should be created. An existing buffer
 *             must not be used any further if this function returns
 *             a null pointer.
 * \param type Part of the buffer to be allocated. This can be any of
 *             the following:
 *             - NBAF_DATALINK
 *             - NBAF_NETWORK
 *             - NBAF_TRANSPORT
 *             - NBAF_APPLICATION
 * \param size Size of the part to be allocated.
 *
 * \return Pointer to the allocated network buffer structure. A null
 *         pointer is returned if not enough memory is available and
 *         the whole structure is released.
 */
NETBUF *NutNetBufAlloc(NETBUF * nb, uint8_t type, int size)
{
    NBDATA * nbd;

    NUTASSERT(size > 0);
    NUTASSERT(type == NBAF_DATALINK || 
        type == NBAF_NETWORK || 
        type == NBAF_TRANSPORT || 
        type == NBAF_APPLICATION);

    /* Allocate a new buffer, if the caller don't provide one. */
    if (nb == NULL) {
        nb = NutHeapAllocClear(sizeof(NETBUF));
    }
    /* Make sure, that the allocation above was successful. */
    if (nb) {
        /* Determine the data type. */
        switch(type) {
        case NBAF_DATALINK:
            nbd = &nb->nb_dl;
            break;
        case NBAF_NETWORK:
            nbd = &nb->nb_nw;
            break;
        case NBAF_TRANSPORT:
            nbd = &nb->nb_tp;
            break;
        default:
            nbd = &nb->nb_ap;
            break;
        }
        /* Check previously allocated buffer. */
        if (nb->nb_flags & type) {
            if (nbd->sz < size) {
                /* Existing buffer is too small. */
                NutHeapFree(nbd->vp);
                nbd->sz = 0;
            } else {
                /* 
                 * Reduce the size. This is actually a bad idea, because
                 * we may waste too much memory. One option would be to
                 * use the new NutHeapRealloc, but not sure if it will
                 * work for 32-bit CPUs after some crashes after its
                 * introduction. Further, I'm not fully sure, if our
                 * network routines actually re-use network buffers.
                 */
                nbd->sz = size;
            }
        } else {
            /* Buffer was not allocated from heap. */
            nbd->sz = 0;
        }
        /* If the size is zero at this point, 
           we need to allocate a new buffer. */
        if (nbd->sz == 0) {
            if (NutNetBufAllocData(nbd, size)) {
                /* Out of memory, release the complete net buffer. */
                NutNetBufFree(nb);
                nb = NULL;
            } else {
                /* Success, mark the buffer allocated. */
                nb->nb_flags |= type;
            }
        }
    }
    return nb;
}

/*!
 * \brief Create a referencing copy of an existing network buffer structure.
 *
 * \param nb   Points to an existing network buffer structure, previously 
 *             allocated by NutNetBufAlloc().
 * \param type Part of the buffer to be additionally allocated. This can 
 *             be any combination of the following flags:
 *             - NBAF_DATALINK
 *             - NBAF_NETWORK
 *             - NBAF_TRANSPORT
 *             - NBAF_APPLICATION
 *
 * \return Pointer to a newly allocated copy.
 */
NETBUF *NutNetBufClonePart(NETBUF * nb, uint8_t inserts)
{
    NETBUF *cb;
    
    NUTASSERT(nb != NULL);

    if ((nb->nb_flags & NBAF_REFCNT) == NBAF_REFCNT) {
        inserts = NBAF_ALL;
    }

    cb = NutHeapAllocClear(sizeof(NETBUF));
    if (cb) {
        uint_fast8_t referenced = 0;
        register int e = 0;
        register int s;

        s = nb->nb_dl.sz;
        if (s) {
            if (inserts & NBAF_DATALINK) {
                e = NutNetBufAllocData(&cb->nb_dl, s);
                if (e == 0) {
                    memcpy(cb->nb_dl.vp, nb->nb_dl.vp, s);
                    cb->nb_flags |= NBAF_DATALINK;
                }
            } else {
                cb->nb_dl.vp = nb->nb_dl.vp;
                cb->nb_dl.sz = s;
                referenced = 1;
            }
        }
        s = nb->nb_nw.sz;
        if (s && e == 0) {
            if (inserts & NBAF_NETWORK) {
                e = NutNetBufAllocData(&cb->nb_nw, s);
                if (e == 0) {
                    memcpy(cb->nb_nw.vp, nb->nb_nw.vp, s);
                    cb->nb_flags |= NBAF_NETWORK;
                }
            } else {
                cb->nb_nw.vp = nb->nb_nw.vp;
                cb->nb_nw.sz = s;
                referenced = 1;
            }
        }
        s = nb->nb_tp.sz;
        if (s && e == 0) {
            if (inserts & NBAF_TRANSPORT) {
                e = NutNetBufAllocData(&cb->nb_tp, s);
                if (e == 0) {
                    memcpy(cb->nb_tp.vp, nb->nb_tp.vp, s);
                    cb->nb_flags |= NBAF_TRANSPORT;
                }
            } else {
                cb->nb_tp.vp = nb->nb_tp.vp;
                cb->nb_tp.sz = s;
                referenced = 1;
            }
        }
        s = nb->nb_ap.sz;
        if (s && e == 0) {
            if (inserts & NBAF_APPLICATION) {
                e = NutNetBufAllocData(&cb->nb_ap, s);
                if (e == 0) {
                    memcpy(cb->nb_ap.vp, nb->nb_ap.vp, s);
                    cb->nb_flags |= NBAF_APPLICATION;
                }
            } else {
                cb->nb_ap.vp = nb->nb_ap.vp;
                cb->nb_ap.sz = s;
                referenced = 1;
            }
        }
        if (e) {
            NutNetBufFree(cb);
            cb = NULL;
        } 
        else if (referenced) {
            cb->nb_ref = nb;
            nb->nb_flags++;
        }
    }
    return cb;
}

/*!
 * \brief Create a copy of an existing network buffer
 *        structure.
 *
 * \param nb Points to an existing network buffer
 *           structure, previously allocated by
 *           NutNetBufAlloc().
 *
 * \return Pointer to a newly allocated copy.
 */
NETBUF *NutNetBufClone(NETBUF * nb)
{
    return NutNetBufClonePart(nb, NBAF_ALL);
}

/*!
 * \brief Release a network buffer structure.
 *
 * Returns all memory previously allocated by a
 * network buffer to the available heap space.
 *
 * \param nb Points to an existing network buffer
 *           structure, previously allocated by
 *           NutNetBufAlloc().
 */
void NutNetBufFree(NETBUF * nb)
{
    NUTASSERT(nb);

    if (nb->nb_flags & NBAF_REFCNT) {
        nb->nb_flags--;
    } else {
        if (nb->nb_flags & NBAF_DATALINK) {
            NutHeapFree(nb->nb_dl.vp);
        }
        if (nb->nb_flags & NBAF_NETWORK) {
            NutHeapFree(nb->nb_nw.vp);
        }
        if (nb->nb_flags & NBAF_TRANSPORT) {
            NutHeapFree(nb->nb_tp.vp);
        }
        if (nb->nb_flags & NBAF_APPLICATION) {
            NutHeapFree(nb->nb_ap.vp);
        }
        if (nb->nb_ref) {
            NutNetBufFree(nb->nb_ref);
        }
        NutHeapFree(nb);
    }
}

/*!
 * \brief Collect linked list of network buffers.
 *
 * \param nbq Points to an existing network buffer queue.
 *
 * \return Number of released network buffers.
 */
int NutNetBufCollect(NETBUF * nbq, int total)
{
    int rc = 0;
    NETBUF *nb;
    NETBUF *nbx;
    NBDATA nbd;
    uint8_t *ap;

    if (NutNetBufAllocData(&nbd, total) == 0) {
        ap = (uint8_t *) nbd.vp;
        memcpy(nbd.vp, nbq->nb_ap.vp, nbq->nb_ap.sz);
        nbd.sz = nbq->nb_ap.sz;
        nb = nbq->nb_next;
        while (nb && total >= nbd.sz + nb->nb_ap.sz) {
            nbx = nb->nb_next;
            memcpy(ap + nbd.sz, nb->nb_ap.vp, nb->nb_ap.sz);
            nbd.sz += nb->nb_ap.sz;
            NutNetBufFree(nb);
            nb = nbx;
            rc++;
        }
        nbq->nb_next = nb;
        if (nbq->nb_flags & NBAF_APPLICATION) {
            NutHeapFree(nbq->nb_ap.vp);
        } else {
            nbq->nb_flags |= NBAF_APPLICATION;
        }
        nbq->nb_ap.vp = nbd.vp;
        nbq->nb_ap.sz = nbd.sz;
    }
    return rc;
}

/*@}*/
