#ifndef _SYS_BANKMEM_H_
#define _SYS_BANKMEM_H_

/*
 * Copyright (C) 2003-2005 by egnite Software GmbH. All rights reserved.
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
 */

/*!
 * $Log$
 * Revision 1.11  2008/08/27 07:05:37  thornen
 * Added bank support for MMnet03..04 and Mmnet102..104
 *
 * Revision 1.10  2008/08/26 17:36:46  haraldkipp
 * Revoked changes 2008/08/26 by thornen.
 *
 * Revision 1.8  2008/08/11 07:00:25  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.7  2007/04/12 09:08:57  haraldkipp
 * Segmented buffer routines ported to ARM.
 *
 * Revision 1.6  2005/09/07 16:22:11  christianwelzel
 * Added banking support for MMnet02
 *
 * Revision 1.5  2005/05/16 08:35:32  haraldkipp
 * Added banking support for Arthernet.
 *
 * Revision 1.4  2004/09/01 14:19:41  haraldkipp
 * Avoiding too many ifdefs
 *
 * Revision 1.3  2004/08/18 18:51:41  haraldkipp
 * Made banked memory configurable.
 *
 * Revision 1.2  2003/12/15 19:27:53  haraldkipp
 * Ethernut 2 support added
 *
 * Revision 1.1  2003/07/21 18:21:34  haraldkipp
 * First check in
 *
 */

#include <sys/types.h>
#include <stdint.h>
#include <cfg/bankmem.h>

/*!
 * \file sys/bankmem.h
 * \brief Banked memory management definitions.
 */

#ifndef NUTBANK_COUNT
/*
 * Arrrgh! I hate this. OK, on one hand the code is not
 * spoiled by ifdefs, but this one is sooouuu ugly.
 */
#define NUTBANK_COUNT   0
#endif

#if NUTBANK_COUNT

#ifdef ARTHERNET1
/* Arthernet uses a different banking. */
#define NutSegBufEnable(bank) *(volatile uint8_t *)(NUTBANK_SR) = (((uint8_t)bank+1)<<4)
#elif MMNET02 || MMNET03 || MMNET04 || MMNET102 || MMNET103 || MMNET104
/* MMnet02..04 and MMnet102..104 use a different banking. */
#define NutSegBufEnable(bank) *((char *)(NUTBANK_SR)) = (bank)
#else
/* This is the Ethernut way. */
#define NutSegBufEnable(bank) *((char *)(NUTBANK_SR) + (bank)) = (bank)
#endif

#else
/* No banked memory. */
#define NutSegBufEnable(bank)
#endif


__BEGIN_DECLS
/* Prototypes */
extern char *NutSegBufReset(void);
extern char *NutSegBufInit(size_t size);
extern char *NutSegBufWriteRequest(size_t * bcp);
extern char *NutSegBufReadRequest(size_t * bcp);
extern char *NutSegBufWriteCommit(size_t bc);
extern char *NutSegBufReadCommit(size_t bc);
extern void NutSegBufWriteLast(size_t bc);
extern void NutSegBufReadLast(size_t bc);
extern uint32_t NutSegBufAvailable(void);
extern uint32_t NutSegBufUsed(void);
/* */
__END_DECLS
#endif
