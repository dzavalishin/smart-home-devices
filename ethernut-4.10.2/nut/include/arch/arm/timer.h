#ifndef _ARCH_ARM_TIMER_H_
#define _ARCH_ARM_TIMER_H_

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
 * Revision 1.7  2008/08/11 06:59:58  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.6  2008/08/06 12:51:11  haraldkipp
 * Added support for Ethernut 5 (AT91SAM9XE reference design).
 *
 * Revision 1.5  2007/08/17 10:44:38  haraldkipp
 * Timer enable/disable macro replaces previous global interrupt
 * enable/disable or function calling.
 *
 * Revision 1.4  2006/09/29 12:45:35  haraldkipp
 * Include clock configuration.
 *
 * Revision 1.3  2006/08/31 18:59:50  haraldkipp
 * Added support for the AT91SAM9260. We now determine between processor and
 * master clock. A new API function At91GetMasterClock() had been added to
 * query the latter.
 *
 * Revision 1.2  2006/02/23 15:33:59  haraldkipp
 * Support for Philips LPC2xxx Family and LPC-E2294 Board from Olimex added.
 * Many thanks to Michael Fischer for this port.
 *
 * Revision 1.1  2005/07/26 18:35:09  haraldkipp
 * First check in
 *
 */

#include <cfg/clock.h>
#include <compiler.h> // For deprecated warning.

#if defined(MCU_LPC2XXX) /* MCU */

#define NutEnableTimerIrq()    VICIntEnable  = (1 << VIC_TIMER0)
#define NutDisableTimerIrq()   VICIntEnClr = (1 << VIC_TIMER0)

#elif defined (MCU_AT91R40008) || defined (MCU_AT91SAM7X) || defined (MCU_AT91SAM9260) || defined(MCU_AT91SAM9XE512) || defined(MCU_AT91SAM9G45) /* MCU */

#if defined(NUT_TICK_AT91PIT)
#define NutEnableTimerIrq()    NutSysIrqEnable(&syssig_PIT)
#define NutDisableTimerIrq()   NutSysIrqDisable(&syssig_PIT)
#else /* NUT_TICK_AT91PIT */
#define NutEnableTimerIrq()    NutIrqEnable(&sig_TC0)
#define NutDisableTimerIrq()   NutIrqDisable(&sig_TC0)
#endif /* NUT_TICK_AT91PIT */

#else /* MCU */

#define NutEnableTimerIrq()
#define NutDisableTimerIrq()

#endif /* MCU */

#if defined(AT91_PLL_MAINCK)
extern uint32_t At91GetMasterClock(void) NUT_DEPRECATED;
#endif

#endif


