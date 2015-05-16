#ifndef _ARCH_AVR32_TIMER_H_
#define _ARCH_AVR32_TIMER_H_

/*!
 * Copyright (C) 2001-2010 by egnite Software GmbH
 *
 * All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
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
 * $Log: timer.h,v $
 *
 */

#include <cfg/clock.h>
#include <dev/irqreg.h>

#define NutEnableTimerIrq() 
#define NutDisableTimerIrq() NutIrqDisable(&sig_sysCompare)

/*!
	PBA Clock
*/
#define NUT_HWCLK_PERIPHERAL_A 1

/*!
	PBB Clock
*/
#define NUT_HWCLK_PERIPHERAL_B 2

/*!
	HSB (High Speed Bus) Clock
*/
#define NUT_HWCLK_PERIPHERAL_HSB 3

/*!
	RC Oscillator. Also known as Slow Clock.
	This is used for the Watchdog timer and can be used as 
	the main clock.
*/
#define NUT_HWCLK_SLOW_CLOCK 4

#define NUT_HWCLK_MAX 4
#define NUT_HWCLK_PERIPHERAL NUT_HWCLK_PERIPHERAL_A

#endif // _ARCH_AVR32_TIMER_H_
