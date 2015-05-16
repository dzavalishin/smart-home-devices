#ifndef _SYS_EVENT_H_
#define _SYS_EVENT_H_

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
 * Revision 1.9  2008/08/11 07:00:25  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.8  2006/10/08 16:48:22  haraldkipp
 * Documentation fixed
 *
 * Revision 1.7  2006/08/01 07:39:12  haraldkipp
 * Missing typecast in NutEventPostFromIrq() fixed. Thanks to Matthias Wilde.
 *
 * Revision 1.6  2006/06/28 14:34:02  haraldkipp
 * Event/thread/timer re-design.
 * A new macro NutEventPostFromIrq() replaces the routine with the same name.
 * In opposite to the previous routine, no result will be returned by this
 * macro, which may break your existing code.
 * The SIGNALED definition had been replaced by a less platform dependent
 * variant.
 *
 * Revision 1.5  2005/01/24 21:11:46  freckle
 * renamed NutEventPostFromIRQ into NutEventPostFromIrq
 *
 * Revision 1.4  2005/01/21 16:49:44  freckle
 * Seperated calls to NutEventPostAsync between Threads and IRQs
 *
 * Revision 1.3  2004/03/16 16:48:44  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 * Revision 1.2  2003/07/20 18:28:54  haraldkipp
 * Explain how to disable timeout.
 *
 * Revision 1.1.1.1  2003/05/09 14:41:19  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.8  2003/02/04 18:00:52  harald
 * Version 3 released
 *
 * Revision 1.7  2003/01/14 16:37:58  harald
 * Return number of woken up threads
 *
 * Revision 1.6  2002/06/26 17:29:28  harald
 * First pre-release with 2.4 stack
 *
 */

#include <sys/thread.h>

/*!
 * \file sys/event.h
 * \brief Event management definitions.
 */

/*!
 * \addtogroup xgEvent
 */
/*@{*/

/*!
 * \brief Signaled state definition.
 *
 * The root of an event queue is set to this value if an event
 * is posted to an empty queue. As this may happen during
 * interrupts, the root of an event queue must be considered
 * volatile.
 *
 * Timer handles in the THREADINFO structure are set to this value
 * if a timeout occured while waiting for an event.
 */
#define SIGNALED    ((void *)-1)

/*!
 * \brief Infinite waiting time definition.
 *
 * Applications should use this value to disable timeout monitoring
 * while waiting for an event.
 */
#define NUT_WAIT_INFINITE   0

/*!
 * \brief Post an event to a specified queue from interrupt context.
 *
 * Wake up the thread with the highest priority waiting on the
 * specified queue. This function is explicitly provided for IRQ
 * handlers to wakeup waiting user threads.
 *
 * Internally a counter is used to keep track of the posted events.
 * This counter will be examined when the currently running thread is 
 * ready to release the CPU.
 *
 * \note When calling this function, interrupt routines will change
 *       the root of an empty event queue to SIGNALED.
 *
 * \param qp Identifies the queue an event is posted to.
 *
 */
#define NutEventPostFromIrq(qp)     \
{                                   \
    if (*qp == 0) {                 \
        *qp = SIGNALED;             \
    }                               \
    else if (*qp != SIGNALED) {     \
        NUTTHREADINFO *tp = (NUTTHREADINFO *)(*qp);    \
        tp->td_qpec++;              \
    }                               \
}

/*@}*/

__BEGIN_DECLS
/* Function prototypes. */

extern void NutEventTimeout(HANDLE timer, void *arg);

extern int NutEventWait(volatile HANDLE *qhp, uint32_t ms);
extern int NutEventWaitNext(volatile HANDLE *qhp, uint32_t ms);
extern int NutEventPostAsync(volatile HANDLE *qhp);
extern int NutEventPost(volatile HANDLE *qhp);
extern int NutEventBroadcastAsync(volatile HANDLE *qhp);
extern int NutEventBroadcast(volatile HANDLE *qhp);

__END_DECLS
/* */

#endif
