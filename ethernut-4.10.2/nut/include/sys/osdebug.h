#ifndef _SYS_OSDEBUG_H_
#define _SYS_OSDEBUG_H_

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
 */

/*
 * $Log$
 * Revision 1.2  2008/08/11 07:00:27  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.1.1.1  2003/05/09 14:41:20  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.4  2003/02/04 18:00:53  harald
 * Version 3 released
 *
 * Revision 1.3  2002/10/29 15:40:50  harald
 * *** empty log message ***
 *
 * Revision 1.2  2002/06/26 17:29:29  harald
 * First pre-release with 2.4 stack
 *
 */

#include <stdio.h>
#include <compiler.h>
#include <sys/thread.h>

__BEGIN_DECLS

extern FILE *__os_trs;
extern uint_fast8_t __os_trf;

extern void NutTraceOs(FILE *stream, uint8_t flags);
extern void NutDumpThreadQueue(FILE *stream, NUTTHREADINFO *tdp);
extern void NutDumpThreadList(FILE *stream);
extern void NutDumpTimerList(FILE *stream);

extern FILE *__heap_trs;
extern uint_fast8_t __heap_trf;

extern void NutTraceHeap(FILE *stream, uint8_t flags);
extern void NutDumpHeap(FILE *stream);

__END_DECLS

#endif
