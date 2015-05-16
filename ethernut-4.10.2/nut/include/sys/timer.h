#ifndef _SYS_TIMER_H
#define _SYS_TIMER_H

/*
 * Copyright (C) 2001-2006 by egnite Software GmbH. All rights reserved.
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
 */

/*
 * $Log$
 * Revision 1.13  2008/08/22 09:25:34  haraldkipp
 * Clock value caching and new functions NutArchClockGet, NutClockGet and
 * NutClockSet added.
 *
 * Revision 1.12  2008/08/11 07:00:28  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.11  2008/07/08 08:25:05  haraldkipp
 * NutDelay is no more architecture specific.
 * Number of loops per millisecond is configurable or will be automatically
 * determined.
 * A new function NutMicroDelay provides shorter delays.
 *
 * Revision 1.10  2006/06/28 14:36:34  haraldkipp
 * Event/thread/timer re-design.
 *
 * Revision 1.9  2005/07/26 16:04:02  haraldkipp
 * Hardware dependent timer code moved to nutlibarch.
 *
 * Revision 1.8  2005/07/12 16:36:53  freckle
 * made NutTimerInsert public
 *
 * Revision 1.7  2005/07/12 16:26:08  freckle
 * extracted timer creation from NutTimerStartTicks into NutTimerCreate
 *
 * Revision 1.6  2005/06/12 16:54:14  haraldkipp
 * New function processes elapsed timers.
 *
 * Revision 1.5  2005/01/19 17:59:42  freckle
 * Improved interrupt performance by reducing some critical section
 *
 * Revision 1.4  2004/03/16 16:48:44  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 * Revision 1.3  2003/11/03 16:35:18  haraldkipp
 * New API returns system uptime in miliseconds
 *
 * Revision 1.2  2003/10/13 10:17:11  haraldkipp
 * Seconds counter added
 * 
 * Revision 1.1.1.1  2003/05/09 14:41:22  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.12  2003/02/04 18:00:54  harald
 * Version 3 released
 *
 * Revision 1.11  2003/01/14 16:38:18  harald
 * New tick counter query
 *
 * Revision 1.10  2002/06/26 17:29:29  harald
 * First pre-release with 2.4 stack
 *
 */

#include <sys/types.h>
#include <arch/timer.h>

#include <stdint.h>

/*!
 * \file sys/timer.h
 * \brief Timer management definitions.
 */

/*!
 * \brief Timer type.
 */
typedef struct _NUTTIMERINFO NUTTIMERINFO;

/*!
 * \struct _NUTTIMERINFO timer.h sys/timer.h
 * \brief Timer information structure.
 */
struct _NUTTIMERINFO {
    /*! \brief Link to next timer. 
     */
    NUTTIMERINFO *tn_next;          
    /*! \brief Link to previous timer. 
     */
    NUTTIMERINFO *tn_prev;          
    /*! \brief Number of system ticks. 
     *  Set to zero on one-shot timers.
     */
    uint32_t tn_ticks;        
    /*! \brief Decremented by one on each system tick intervall. 
     */
    uint32_t tn_ticks_left;   
    /*! \brief Callback function. 
     */
    void (*tn_callback)(HANDLE, void *);    
    /*! \brief Argument pointer passed to callback function. 
     */
    volatile void *tn_arg;          
};

extern NUTTIMERINFO* nutTimerList;

#define TM_ONESHOT  0x01

#define NUT_CACHE_LVALID        0x80000000UL

/* Set defaults. */
#ifndef NUT_HWCLK_CPU
#define NUT_HWCLK_CPU           0
#endif
#ifndef NUT_HWCLK_PERIPHERAL
#define NUT_HWCLK_PERIPHERAL    NUT_HWCLK_CPU
#endif
#ifndef NUT_HWCLK_MAX
#define NUT_HWCLK_MAX           NUT_HWCLK_PERIPHERAL
#endif

__BEGIN_DECLS
/* Prototypes */

/*
 * Functions used by the kernel.
 */
extern void NutTimerInit(void);
extern NUTTIMERINFO * NutTimerCreate(uint32_t ticks, void (*callback) (HANDLE, void *), void *arg, uint8_t flags);
extern void NutTimerInsert(NUTTIMERINFO * tn);
extern void NutTimerProcessElapsed(void);

/*
 * API declarations.
 */
extern void NutSleep(uint32_t ms);
extern void NutDelay(uint8_t ms);
extern void NutMicroDelay(uint32_t us);

extern uint32_t NutGetTickCount(void);
extern uint32_t NutGetSeconds(void);
extern uint32_t NutGetMillis(void);

extern HANDLE NutTimerStart(uint32_t ms, void (*callback)(HANDLE, void *), void *arg, uint8_t flags);
extern HANDLE NutTimerStartTicks(uint32_t ticks, void (*callback) (HANDLE, void *), void *arg, uint8_t flags);
extern void NutTimerStop(HANDLE handle);

/*
 * Clock frequencies.
 */
#if NUT_HWCLK_MAX
/* More than 1 clock available: Implement function. */
extern uint32_t NutClockGet(int idx);
#else
/* Only 1 clock available: Map to CPU clock. */
#define NutClockGet(i)          NutGetCpuClock()
#endif

extern int NutClockSet(int idx, uint32_t freq);

/* On some platforms the clock query functions may be defined by 
** a preprocessor macro to avoid function call overhead. */
#if !defined(NutGetCpuClock)
extern uint32_t NutGetCpuClock(void);
#endif

__END_DECLS
/* End of prototypes */

#endif
