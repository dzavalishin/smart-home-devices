#ifndef _NET_NETBUF_H_
#define _NET_NETBUF_H_

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
 * Revision 1.5  2008/08/20 06:58:35  haraldkipp
 * Added a new flag to mark unicast packets.
 *
 * Revision 1.4  2008/08/11 06:59:59  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.3  2008/08/09 17:35:32  haraldkipp
 * Simplified NETBUF allocation and release.
 *
 * Revision 1.2  2008/04/18 13:31:59  haraldkipp
 * Changed size parameter from u_short to int, which is easier to handle
 * for 32-bit targets. You need to recompile your ARM code. No impact on
 * AVR expected
 * I changed u_int to int at some places to avoid some warnings during
 * compilation of Nut/Net.
 * libs.
 *
 * Revision 1.1.1.1  2003/05/09 14:41:07  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.7  2003/05/06 18:43:03  harald
 * Cleanup
 *
 * Revision 1.6  2003/02/04 18:00:37  harald
 * Version 3 released
 *
 * Revision 1.5  2002/06/26 17:29:15  harald
 * First pre-release with 2.4 stack
 *
 */

#include <sys/types.h>
#include <stdint.h>

/*!
 * \file dev/netbuf.h
 * \brief Network buffer definitions.
 */

/*!
 * \addtogroup xgnetbuf
 */
/*@{*/


/* lower bits may be used as ref-count, if ever needed */
#define NBAF_REFCNT      0x07   /*!< \brief Reference count mask. */

#define NBAF_UNICAST     0x08   /*!< \brief Unicast address. */

#define NBAF_DATALINK    0x10   /*!< \brief Datalink buffer allocated flag. */
#define NBAF_NETWORK     0x20   /*!< \brief Network buffer allocated flag. */
#define NBAF_TRANSPORT   0x40   /*!< \brief Transport buffer allocated flag. */
#define NBAF_APPLICATION 0x80   /*!< \brief Application buffer allocated flag. */
#define NBAF_ALL         0xf0   /*!< \brief Masks allocated buffer flags flag. */

/*!
 * \brief Network buffer type.
 */
typedef struct _NETBUF NETBUF;

/*!
 * \brief Network buffer data type.
 */
typedef struct _NBDATA NBDATA;

/*!
 * \struct _NBDATA netbuf.h dev/netbuf.h
 * \brief Data part of a network buffer structure.
 */
struct _NBDATA {
    void *vp;   /*!< \brief Pointer to data buffer. */
    int  sz;    /*!< \brief Size of data buffer. */
};

/*!
 * \struct _NETBUF netbuf.h dev/netbuf.h
 * \brief Network buffer structure.
 */
struct _NETBUF {
    NETBUF *nb_next;    /*!< \brief Link to next structure. */
    uint8_t nb_flags;   /*!< \brief NBAF_ flags. */
    NBDATA nb_dl;       /*!< \brief Datalink buffer. */
    NBDATA nb_nw;       /*!< \brief Network buffer. */
    NBDATA nb_tp;       /*!< \brief Transport buffer. */
    NBDATA nb_ap;       /*!< \brief Application buffer. */
    NETBUF *nb_ref;     /*!< \brief Link to clone origin. */
};

/*@}*/

__BEGIN_DECLS

extern NETBUF *NutNetBufAlloc(NETBUF *nb, uint8_t type, int size);
extern NETBUF *NutNetBufClone(NETBUF *nb);
extern NETBUF *NutNetBufClonePart(NETBUF *nb, uint8_t types);
extern void NutNetBufFree(NETBUF *nb);
extern int NutNetBufCollect(NETBUF * nbq, int total);

__END_DECLS

#endif
