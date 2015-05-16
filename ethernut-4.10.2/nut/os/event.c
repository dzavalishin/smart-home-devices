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
 */

/*!
 * \file os/event.c
 * \brief Event management routines.
 *
 * This kernel module provides thread synchronization.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.26  2009/01/17 15:37:52  haraldkipp
 * Added some NUTASSERT macros to check function parameters.
 *
 * Revision 1.25  2008/08/11 07:00:33  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.24  2006/06/28 14:38:34  haraldkipp
 * Event and timer handling re-design, again. This fixes a bug, which
 * possibly existed since version 3.9.8 and freezes threads under heavy
 * load. After several people reported this problem, Michael Jones and
 * Henrik Maier finally detected the cause and came up with a solution.
 * However, this fix let interrupt latency times depend on the number
 * of running threads again and a new solution was implemented, which
 * not only avoids this problem but further decreases interrupt latencies
 * by adding an event post counter to the THREADINFO structure. This
 * counter frees the interrupt routines from dealing with linked lists
 * and frees the kernel from dealing with linked lists concurrently
 * modified by interrupts. Furthermore, timeout timers are now released
 * early. Michael Jones reported, that previous versions suffer from low
 * memory situations while processing many events within short periods.
 * The timer list is now double linked to reduce removal time. Internally
 * timeout condition is now flagged by setting the timer handle to
 * SIGNALED.
 * Unfortunately new bugs were introduced with this re-design. Special
 * thanks to Michael Jones, who located the "exact spot of the crime" and
 * proofed, that his final fixes let Nut/OS behave quite well under heavy
 * traffic storms. This new version will probably help also, if you
 * experienced long term instability.
 * LGPL copyright removed after all the initial sources had been finally
 * replaced by BSDL'ed code.
 * Last not least the documentation had been updated.
 *
 * Revision 1.23  2005/08/18 15:33:54  christianwelzel
 * Fixed bug in handling of NUTDEBUG.
 *
 * Revision 1.22  2005/07/21 14:23:17  freckle
 * inlined NutEventPostFromIrq call in NutEventPostAsync to reduce CS
 *
 * Revision 1.21  2005/07/20 09:19:45  haraldkipp
 * Use native heap calls to avoid dependencies
 *
 * Revision 1.20  2005/07/13 15:25:50  freckle
 * Rewrote NutEventWait to get short critical sections
 *
 * Revision 1.19  2005/07/12 18:44:05  freckle
 * Removed unnecessary critical sections in NutEventPost, NutEventWait
 *
 * Revision 1.18  2005/07/12 18:04:12  freckle
 * Reverted NutEventWait back to 1.15 but kept critical section in
 * NutEventTimeout + changed CS in NutEventWaitNext
 *
 * Revision 1.15  2005/06/10 12:59:26  freckle
 * corrected NuEventBroadcastAsync documentation.
 * NutEventBroadcast uses NutEventPostAsync which disables IRQsCVS:
 * ----------------------------------------------------------------------
 *
 * Revision 1.14  2005/02/21 12:37:57  phblum
 * Removed tabs and added semicolons after NUTTRACER macros
 *
 * Revision 1.13  2005/02/17 14:45:10  phblum
 * In NutEventWait(), replaced NutThreadRemoveQueue by direct removal of first thread in queue.
 *
 * Revision 1.12  2005/02/16 19:53:17  haraldkipp
 * Ready-to-run queue handling removed from interrupt context.
 *
 * Revision 1.11  2005/01/24 22:34:35  freckle
 * Added new tracer by Phlipp Blum <blum@tik.ee.ethz.ch>
 *
 * Revision 1.10  2005/01/24 21:11:21  freckle
 * renamed NutEventPostFromIRQ into NutEventPostFromIrq
 *
 * Revision 1.9  2005/01/21 16:49:44  freckle
 * Seperated calls to NutEventPostAsync between Threads and IRQs
 *
 * Revision 1.8  2005/01/19 17:59:45  freckle
 * Improved interrupt performance by reducing some critical section
 *
 * Revision 1.7  2005/01/02 10:07:10  haraldkipp
 * Replaced platform dependant formats in debug outputs.
 *
 * Revision 1.6  2004/07/20 08:33:28  freckle
 * Fixed NutPostEvent to give up CPU if there is another thread ready with
 * same priority to match the documentation
 * Also removed 'SWP..' NBUTDEBUG output, as switching is done calling NutThreadYield()
 *
 * Revision 1.5  2004/04/07 12:13:58  haraldkipp
 * Matthias Ringwald's *nix emulation added
 *
 * Revision 1.4  2004/03/19 09:05:12  jdubiec
 * Fixed format strings declarations for AVR.
 *
 * Revision 1.3  2004/03/16 16:48:45  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 * Revision 1.2  2003/07/20 18:27:44  haraldkipp
 * Explain how to disable timeout.
 *
 * Revision 1.1.1.1  2003/05/09 14:41:49  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.20  2003/04/21 17:07:41  harald
 * Comments added
 *
 * Revision 1.19  2003/03/31 14:46:41  harald
 * Broadcasts clear signaled queues
 *
 * Revision 1.18  2003/02/04 18:15:56  harald
 * Version 3 released
 *
 * Revision 1.17  2003/01/14 16:56:39  harald
 * Racing condition on signaled events fixed.
 * All posting routines return the number of woken up threads.
 *
 * Revision 1.16  2002/06/26 17:29:44  harald
 * First pre-release with 2.4 stack
 *
 * \endverbatim
 */

#include <cfg/os.h>

#include <compiler.h>
#include <sys/atom.h>
#include <sys/heap.h>
#include <sys/timer.h>
#include <sys/thread.h>
#include <sys/event.h>
#include <sys/nutdebug.h>

#ifdef NUTDEBUG
#include <sys/osdebug.h>
#include <stdio.h>
#endif

#ifdef NUTTRACER
#include <sys/tracer.h>
#endif

/*!
 * \addtogroup xgEvent
 */
/*@{*/


/*!
 * \brief Timer callback in case of event timeout.
 *
 * Applications should not call this function. It is provided as a global
 * to enable debugging code inspecting the callbacks in the timer list.
 *
 * \param timer Handle of the elapsed timeout timer.
 * \param arg   Handle of an event queue.
 *
 */
void NutEventTimeout(HANDLE timer, void *arg)
{
    NUTTHREADINFO *tqp;
    NUTTHREADINFO *volatile *tqpp = arg;

    NUTASSERT(tqpp != NULL);

    /* Get the queue's root atomically. */
    NutEnterCritical();
    tqp = *tqpp;
    NutExitCritical();

    /*
     * A signaled queue is an empty queue. So our
     * thread already left this queue.
     */
    if (tqp != SIGNALED) {

        /*
         * Walk down the linked list and identify
         * the thread by the timer id it is waiting
         * for.
         */
        while (tqp) {
            if (tqp->td_timer == timer) {
                /* Found the thread. Remove it from the event queue. */
                   
                NutEnterCritical();
                *tqpp = tqp->td_qnxt;
                if (tqp->td_qpec) {
                    if (tqp->td_qnxt) {
                        tqp->td_qnxt->td_qpec = tqp->td_qpec;
                    }
                    else {
                        *tqpp = SIGNALED;
                    }
                    tqp->td_qpec = 0;
                }
                NutExitCritical();

                /* Add it to the queue of threads, which are ready to run. */
                tqp->td_state = TDS_READY;
                NutThreadAddPriQueue(tqp, (NUTTHREADINFO **) & runQueue);

                /* Signal the timer entry in the thread's info structure.
                   This will tell the waiting thread, that it has been
                   woken up by a timeout. */
                tqp->td_timer = SIGNALED;
                break;
            }
            tqpp = &tqp->td_qnxt;
            tqp = tqp->td_qnxt;
        }
    }
}

/*!
 * \brief Wait for an event in a specified queue.
 *
 * Give up the CPU until another thread or an interrupt routine posts an 
 * event to this queue or until a time-out occurs, whichever comes first.
 *
 * If previously an event had been posted to this queue without any 
 * thread waiting, then the thread will not wait for a new event,
 * but may still pass CPU control, if another thread with equal or
 * higher priority is ready to run.
 *
 * \param qhp Identifies the queue to wait on.
 * \param ms  Maximum wait time in milliseconds. To disable timeout,
 *            set this parameter to NUT_WAIT_INFINITE.
 *
 * \return 0 if event received, -1 on timeout.
 *
 * \note Timeout is limited to the granularity of the system timer.
 */
int NutEventWait(volatile HANDLE * qhp, uint32_t ms)
{
    NUTTHREADINFO *tdp;

    NUTASSERT(qhp != NULL);

    /* Get the queue's root atomically. */
    NutEnterCritical();
    tdp = *qhp;
    NutExitCritical();

    /*
     * Check for posts on a previously empty queue. 
     */
    if (tdp == SIGNALED) {
        /* Clear the singaled state. */
        NutEnterCritical();
        *qhp = 0;
        NutExitCritical();
        
        /*
         * Even if already signaled, switch to any other thread, which 
         * is ready to run and has the same or higher priority.
         */
        NutThreadYield();
        return 0;
    }

    /*
     * Remove the current thread from the list of running threads 
     * and add it to the specified queue.
     */
    NutThreadRemoveQueue(runningThread, &runQueue);
    NutThreadAddPriQueue(runningThread, (NUTTHREADINFO **) qhp);

    /* Update our thread's state (sleeping + timer) */
    runningThread->td_state = TDS_SLEEP;
    if (ms) {
        runningThread->td_timer = NutTimerStart(ms, NutEventTimeout, (void *) qhp, TM_ONESHOT);
    }
    else {
        runningThread->td_timer = 0;
    }

    /*
     * Switch to the next thread, which is ready to run.
     */
#ifdef NUTTRACER
    TRACE_ADD_ITEM(TRACE_TAG_THREAD_WAIT,(int)runningThread);
#endif
    NutThreadResume();

    /* If our timer handle is signaled, we were woken up by a timeout. */
    if (runningThread->td_timer == SIGNALED) {
        runningThread->td_timer = 0;
        return -1;
    }
    return 0;
}

/*!
 * \brief Wait for a new event in a specified queue.
 *
 * Give up the CPU until another thread or an interrupt routine posts 
 * an event to this queue or until a time-out occurs, whichever comes 
 * first.
 *
 * This call is similar to NutEventWait(), but will ignore the SIGNALED 
 * state of the queue. This way, previously posted events to an empty 
 * queue are not considered.
 *
 * \param qhp Identifies the queue to wait on.
 * \param ms  Maximum wait time in milliseconds. To disable timeout,
 *            set this parameter to NUT_WAIT_INFINITE.
 *
 * \return 0 if event received, -1 on timeout.
 *
 * \note Timeout is limited to the granularity of the system timer.
 */
int NutEventWaitNext(volatile HANDLE * qhp, uint32_t ms)
{
    NUTASSERT(qhp != NULL);

    /*
     * Check for posts on a previously empty queue. 
     */
    NutEnterCritical();
    if (*qhp == SIGNALED)
        *qhp = 0;
    NutExitCritical();

    return NutEventWait(qhp, ms);
}

/*!
 * \brief Asynchronously post an event to a specified queue.
 *
 * Wake up the thread with the highest priority waiting on the 
 * specified queue. But even if the priority of the woken thread is 
 * higher than the current thread's priority, the current one 
 * continues running.
 *
 * If no thread is waiting, then the queue will be set to the SIGNALED
 * state. 
 *
 * \note Interrupts must not call this function but use NutEventPostFromIrq()
 *       to post events to specific queues.
 *
 * \param qhp Identifies the queue an event is posted to.
 *
 * \return The number of threads woken up, either 0 or 1.
 *
 */
int NutEventPostAsync(volatile HANDLE * qhp)
{
    NUTTHREADINFO *td;

    NUTASSERT(qhp != NULL);

    NutEnterCritical();
    td = *qhp;
    NutExitCritical();

    /* Ignore signaled queues. */
    if (td != SIGNALED) {
        
        /* A thread is waiting. */
        if (td) {
            /* Remove the thread from the wait queue. */
            NutEnterCritical();
            *qhp = td->td_qnxt;
            if (td->td_qpec) {
                if (td->td_qnxt) {
                    td->td_qnxt->td_qpec = td->td_qpec;
                }
                else {
                    *qhp = SIGNALED;
                }
                td->td_qpec = 0;
            }
            NutExitCritical();

            /* Stop any running timeout timer. */
            if (td->td_timer) {
                NutTimerStop(td->td_timer);
                td->td_timer = 0;
            }
            /* The thread is ready to run. */
            td->td_state = TDS_READY;
            NutThreadAddPriQueue(td, (NUTTHREADINFO **) & runQueue);

            return 1;
        }
        
        /* No thread is waiting. Mark the queue signaled. */
        else {
            NutEnterCritical();
            *qhp = SIGNALED;
            NutExitCritical();
        }
    }
    return 0;
}

/*!
 * \brief Post an event to a specified queue.
 *
 * Wake up the thread with the highest priority waiting on this queue. 
 * If the priority of the waiting thread is higher or equal than the 
 * current thread's priority, then the current thread is stopped and 
 * CPU control is passed to the waiting thread.
 *
 * If no thread is waiting, the queue will be set to the signaled
 * state. 
 *
 * \note Interrupts must not call this function but use NutEventPostFromIrq()
 *       to post events to specific queues.
 *
 * \param qhp Identifies the queue an event is posted to.
 *
 * \return The number of threads woken up, either 0 or 1.
 *
 */
int NutEventPost(volatile HANDLE * qhp)
{
    int rc;

    rc = NutEventPostAsync(qhp);

    /*
     * If any thread with higher or equal priority is
     * ready to run, switch the context.
     */
    NutThreadYield();

    return rc;
}

/*!
 * \brief Asynchronously broadcast an event to a specified queue.
 *
 * Wake up all threads waiting on this queue. But even if the
 * priority of any woken thread is higher than the current thread's
 * priority, the current one continues running.
 *
 * In opposite to NutEventPostAsync(), the queue will be cleared in 
 * any case, even if it is in signaled state. Applications may use this
 * call to make sure, that a queue is cleared before initiating some 
 * event triggering action.
 *
 * \param qhp Identifies the queue an event is broadcasted to.
 *
 * \return The number of threads woken up.
 *
 */
int NutEventBroadcastAsync(volatile HANDLE * qhp)
{
    int rc = 0;
    NUTTHREADINFO *tdp;

    NUTASSERT(qhp != NULL);

    /* Get the queue's root atomically. */
    NutEnterCritical();
    tdp = *qhp;
    NutExitCritical();

    if (tdp == SIGNALED) {
        NutEnterCritical();
        *qhp = 0;
        NutExitCritical();
    }
    else if (tdp) {
        do {
            rc += NutEventPostAsync(qhp);
            /* Get the queue's updated root atomically. */
            NutEnterCritical();
            tdp = *qhp;
            NutExitCritical();
        } while (tdp && tdp != SIGNALED);
    }
    return rc;
}

/*!
 * \brief Broadcast an event to a specified queue.
 *
 * Wake up all threads waiting on this queue. If the priority of any 
 * waiting thread is higher or equal than the current thread's 
 * priority, then the current thread is stopped and CPU control is 
 * passed to the woken up thread with the highest priority.
 *
 * In opposite to NutEventPost(), the queue will be cleared in any
 * case, even if it is in signaled state. Applications may use this
 * call to make sure, that a queue is cleared before initiating
 * some event triggering action.
 *
 * \param qhp Identifies the queue an event is broadcasted to.
 *
 * \return The number of threads woken up.
 *
 */
int NutEventBroadcast(volatile HANDLE * qhp)
{
    int rc = NutEventBroadcastAsync(qhp);

    NutThreadYield();

    return rc;
}

/*@}*/
