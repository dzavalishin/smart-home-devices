#ifndef _ARCH_AVR_TIMER_H_
#define _ARCH_AVR_TIMER_H_

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
 * Revision 1.3  2008/08/11 11:51:20  thiagocorrea
 * Preliminary Atmega2560 compile options, but not yet supported.
 * It builds, but doesn't seam to run properly at this time.
 *
 * Revision 1.2  2006/02/08 15:20:56  haraldkipp
 * ATmega2561 Support
 *
 * Revision 1.1  2005/07/26 18:35:09  haraldkipp
 * First check in
 *
 */

#if defined(MCU_AT90CAN128) || defined(MCU_ATMEGA2560) || defined(MCU_ATMEGA2561)
#define NutEnableTimerIrq()     sbi(TIMSK2, OCIE2A)
#define NutDisableTimerIrq()    cbi(TIMSK2, OCIE2A)
#else
#define NutEnableTimerIrq()     sbi(TIMSK, OCIE0)
#define NutDisableTimerIrq()    cbi(TIMSK, OCIE0)
#endif

#endif

