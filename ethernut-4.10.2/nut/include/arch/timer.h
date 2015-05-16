#ifndef _ARCH_TIMER_H_
#define _ARCH_TIMER_H_

/*
 * Copyright (C) 2001-2005 by egnite Software GmbH. All rights reserved.
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
 */

/*
 * $Log$
 * Revision 1.5  2009/03/05 22:16:57  freckle
 * use __NUT_EMULATION instead of __APPLE__, __linux__, or __CYGWIN__
 *
 * Revision 1.4  2008/08/22 09:25:34  haraldkipp
 * Clock value caching and new functions NutArchClockGet, NutClockGet and
 * NutClockSet added.
 *
 * Revision 1.3  2008/08/11 06:59:58  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.2  2008/07/08 08:25:05  haraldkipp
 * NutDelay is no more architecture specific.
 * Number of loops per millisecond is configurable or will be automatically
 * determined.
 * A new function NutMicroDelay provides shorter delays.
 *
 * Revision 1.1  2005/07/26 18:42:19  haraldkipp
 * First check in
 *
 */

#include <stdint.h>

#ifdef __NUT_EMULATION__
#include <arch/unix/timer.h>
#elif defined(__AVR__)
#include <arch/avr/timer.h>
#elif defined(__arm__)
#include <arch/arm/timer.h>
#elif defined(__AVR32__)
#include <arch/avr32/timer.h>
#elif defined(__H8300H__) || defined(__H8300S__)
#include <arch/h8300h/timer.h>
#elif defined(__m68k__)
#include <arch/m68k/timer.h>
#endif

__BEGIN_DECLS
/* Prototypes */

/*
 * Architecture dependent functions.
 */
extern void NutRegisterTimer(void (*handler) (void *));
extern uint32_t NutArchClockGet(int idx);
extern uint32_t NutGetTickClock(void);
extern uint32_t NutTimerMillisToTicks(uint32_t ms);

__END_DECLS
/* End of prototypes */

#endif
