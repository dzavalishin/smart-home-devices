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

/*!
 * \file os/thread.c
 * \brief Multi-threading support.
 *
 * This kernel module implements the platform independent part of the Nut/OS 
 * cooperative multi-threading.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.37  2009/03/05 22:16:57  freckle
 * use __NUT_EMULATION instead of __APPLE__, __linux__, or __CYGWIN__
 *
 * Revision 1.36  2009/02/06 15:41:34  haraldkipp
 * Allow to query the thread handle of the currently running thread without
 * knowing its name (using parameter NULL).
 *
 * Revision 1.35  2009/01/19 18:55:12  haraldkipp
 * Added stack checking code.
 *
 * Revision 1.34  2009/01/17 15:37:52  haraldkipp
 * Added some NUTASSERT macros to check function parameters.
 *
 * Revision 1.33  2009/01/17 11:26:52  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.32  2008/08/11 07:00:34  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.31  2008/07/07 11:04:28  haraldkipp
 * Configurable ways of handling critical sections for ARM targets.
 *
 * Revision 1.30  2008/06/15 17:09:01  haraldkipp
 * Rolled back to version 1.28.
 *
 * Revision 1.28  2006/09/29 12:24:14  haraldkipp
 * Stack allocation code moved from thread module to heap module.
 * All code should use dedicated stack allocation routines. For targets
 * allocating stack from the normal heap the API calls are remapped by
 * preprocessor macros.
 *
 * Revision 1.27  2006/06/28 14:39:41  haraldkipp
 * Event and timer handling re-design, again.
 *
 * Revision 1.26  2006/03/16 15:25:36  haraldkipp
 * Changed human readable strings from u_char to char to stop GCC 4 from
 * nagging about signedness.
 *
 * Revision 1.25  2006/03/02 23:57:54  hwmaier
 * Fixed bug NutStackHeap* functions. The availabe variable was not
 * taken into account.
 *
 * Revision 1.24  2006/02/08 15:20:56  haraldkipp
 * ATmega2561 Support
 *
 * Revision 1.23  2006/01/26 15:34:49  going_nuts
 * adapted to new interrupt handling scheme for unix emulation
 * now uses Unix timer and runs without interrupts unless you emulate other hardware
 *
 * Revision 1.22  2005/10/04 05:14:49  hwmaier
 * Added support for separating stack and conventional heap as required by AT09CAN128 MCUs
 *
 * Revision 1.21  2005/07/26 16:00:16  haraldkipp
 * Cygwin added.
 * Platform dependent part is now located in arch/(target)/context.c
 *
 * Revision 1.20  2005/07/14 08:57:54  freckle
 * Rewrote CS in NutThreadSetPriority
 *
 * Revision 1.19  2005/07/12 13:57:56  freckle
 * Fixed bug in thread not waking up on NutSleep
 *
 * Revision 1.18  2005/06/12 16:56:20  haraldkipp
 * Calls new function to process elapsed timers.
 *
 * Revision 1.17  2005/05/27 17:30:26  drsung
 * Platform dependant files were moved to /arch directory.
 *
 * Revision 1.16  2005/04/19 14:15:28  phblum
 * Moved trace macro to avoid jamming the buffer with yields that do not switch to another thread (e.g. idle thread does so all the time).
 *
 * Revision 1.15  2005/02/21 12:37:57  phblum
 * Removed tabs and added semicolons after NUTTRACER macros
 *
 * Revision 1.14  2005/02/21 00:53:57  hwmaier
 * Changes to incorporate support for AT90CAN128 CPU
 *
 * Revision 1.13  2005/02/17 14:44:39  phblum
 * Removed volatile declarations of runQueue since it is not touched from interrupt context. Same for nutThreadList, runningThread and killedThreads.
 * As runQueue is not modified by interrupts, runningThread == runQueue always. Therefore removed obsolete comparison in NutThreadYield().
 * Removed unnecessary critical section in NutThreadYield.
 * Put NutThreadSwitch call of NutThreadResume into critical section.
 *
 * Revision 1.12  2005/02/16 19:53:17  haraldkipp
 * Ready-to-run queue handling removed from interrupt context.
 *
 * Revision 1.11  2005/01/24 22:34:36  freckle
 * Added new tracer by Phlipp Blum <blum@tik.ee.ethz.ch>
 *
 * Revision 1.10  2005/01/02 10:07:10  haraldkipp
 * Replaced platform dependant formats in debug outputs.
 *
 * Revision 1.9  2004/08/05 12:13:57  freckle
 * Added unix emulation hook in NutThreadYield to safely process
 * NutPostEventAsync calls occuring in non Nut/OS threads.
 * Rewrote the unix read function again using the new unix NutThreadYield hook
 * to call the NutPostEventAsync function safely (fast & correct).
 * _write(nf, 0, 0) aka fflush is ignored on unix emulation.
 *
 * Revision 1.8  2004/04/07 12:13:58  haraldkipp
 * Matthias Ringwald's *nix emulation added
 *
 * Revision 1.7  2004/03/19 09:05:12  jdubiec
 * Fixed format strings declarations for AVR.
 *
 * Revision 1.6  2004/03/16 16:48:45  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 * Revision 1.5  2004/02/01 18:49:48  haraldkipp
 * Added CPU family support
 *
 * Revision 1.4  2003/12/15 19:31:46  haraldkipp
 * Ralph Mason's thread termination and reduced stack frame.
 *
 * Revision 1.3  2003/11/19 12:06:23  drsung
 * CR/LF issue corrected
 *
 * Revision 1.2  2003/11/18 22:18:40  drsung
 * thread name with 8 characters fixed.
 *
 * Revision 1.1.1.1  2003/05/09 14:41:54  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.23  2003/05/06 18:56:24  harald
 * Avoid inlining with GCC optimizations
 *
 * Revision 1.22  2003/03/31 14:39:43  harald
 * *** empty log message ***
 *
 * Revision 1.21  2003/02/04 18:15:57  harald
 * Version 3 released
 *
 * Revision 1.20  2002/10/29 15:36:59  harald
 * New function retrieves thread by name
 *
 * Revision 1.19  2002/08/08 17:31:47  harald
 * Imagecraft support by Klaus Ummenhofer
 *
 * Revision 1.18  2002/07/03 16:45:42  harald
 * Using GCC 3.2
 *
 * Revision 1.17  2002/06/26 17:29:44  harald
 * First pre-release with 2.4 stack
 *
 * \endverbatim
 */

#include <cfg/os.h>
#include <cfg/memory.h>

#include <string.h>

#include <sys/types.h>
#include <sys/heap.h>
#include <sys/atom.h>
#include <sys/timer.h>
#include <sys/event.h>
#include <sys/thread.h>
#include <sys/nutdebug.h>

#include <sys/osdebug.h>

#ifdef NUTTRACER
#include <sys/tracer.h>
#endif

/*!
 * \addtogroup xgThread
 */
/*@{*/

#if defined(NUT_CRITICAL_NESTING) && !defined(NUT_CRITICAL_NESTING_STACK)
unsigned int critical_nesting_level;
#endif

#ifdef __NUT_EMULATION__
// prototype
extern void NutUnixThreadYieldHook(void);  // from unix_nutinit.c
#endif

/*!
 * \brief Currently running thread.
 *
 * Pointer to the NUTTHREADINFO structure of the currently
 * running thread.
 */
NUTTHREADINFO * runningThread;

/*!
 * \brief Thread to be killed.
 *
 * Pointer to the NUTTHREADINFO structure of the latest
 * killed thread.
 */
NUTTHREADINFO * killedThread;

/*!
 * \brief List of all created threads.
 *
 * Linked list of NUTTHREADINFO structures of all threads.
 * New threads are put in front. This list contains at
 * least two threads, the main application thread followed
 * by the idle thread.
 */
NUTTHREADINFO * nutThreadList;

/*!
 * \brief List of ready-to-run threads.
 *
 * Priority ordered linked list of NUTTHREADINFO structures
 * of all threads which are ready to run. The idle thread
 * will always remain at the end of this list.
 */
NUTTHREADINFO * runQueue;



/*!
 * \brief Add a thread to a prioritiy ordered queue.
 *
 * Insert the thread into a specified queue behind
 * the last thread with lower or equal priority.
 *
 * \param td   Pointer to NUTTHREADINFO of the thread to be
 *             inserted in the queue.
 * \param tqpp Pointer to the root of the queue.
 */
void NutThreadAddPriQueue(NUTTHREADINFO * td, NUTTHREADINFO * volatile *tqpp)
{
    NUTTHREADINFO *tqp;

    NUTASSERT(td != NULL);

    td->td_queue = (HANDLE) tqpp;
    td->td_qpec = 0;			// start with clean event count

    /*
     * Be most careful not to override an intermediate event from interrupt 
     * context, which may change a queue from empty to signaled state. Many 
     * thanks to Michael Jones, who detected and corrected this bug.
     */
    NutEnterCritical();
    tqp = *tqpp;

    if (tqp == SIGNALED) {
        tqp = 0;
        td->td_qpec++;			// transfer the signaled state 
    } else if (tqp) {
        NutExitCritical();		// there are other threads in queue
						// so its save to leave critical.          

        while (tqp && tqp->td_priority <= td->td_priority) {
            tqpp = &tqp->td_qnxt;
            tqp = tqp->td_qnxt;
        }

        NutEnterCritical();		// back into critical
    }

    td->td_qnxt = tqp;

    *tqpp = td;
    if (td->td_qnxt && td->td_qnxt->td_qpec) {
        td->td_qpec += td->td_qnxt->td_qpec; // don't overwrite count
        td->td_qnxt->td_qpec = 0;
    }
    NutExitCritical();
}

/*!
 * \brief Remove a thread from a specified queue.
 *
 * \note Depending on the given queue, CPU interrupts must have
 * been disabled before calling this function.
 *
 * \param td   Pointer to NUTTHREADINFO of the thread to be
 *             removed from the queue.
 * \param tqpp Pointer to the root of the queue.
 */
void NutThreadRemoveQueue(NUTTHREADINFO * td, NUTTHREADINFO * volatile *tqpp)
{
    NUTTHREADINFO *tqp;

    NutEnterCritical();
    tqp = *tqpp;
    NutExitCritical();

    if (tqp != SIGNALED) {
        while (tqp) {
            if (tqp == td) {
                NutEnterCritical();
                *tqpp = td->td_qnxt;
                if (td->td_qpec) {
                    if (td->td_qnxt) {
                        td->td_qnxt->td_qpec = td->td_qpec;
                    }
                    td->td_qpec = 0;
                }
                NutExitCritical();

                td->td_qnxt = 0;
                td->td_queue = 0;
                break;
            }
            tqpp = &tqp->td_qnxt;
            tqp = tqp->td_qnxt;
        }
    }
}

/*!
 * \brief Continue with the highest priority thread, which is ready to run.
 *
 * If the currently running thread lost its top position in the queue
 * of ready-to-run threads, then the context will be switched.
 *
 * \todo Removing a single thread from a wait queue only improves context
 *       switching, but may result in an event time-out for remaining 
 *       threads, although events had been posted already.
 */
void NutThreadResume(void)
{
    NUTTHREADINFO *td;
    NUTTHREADINFO **qhp;
    NUTTHREADINFO *tqp;
    unsigned int cnt;

    /*
     * Process events that have been posted from interrupt context.
     */
    td = nutThreadList;
    while (td) {
        NutEnterCritical();
        cnt = td->td_qpec;
        NutExitCritical();
        if (cnt) {
            /* In order to reduce context switching time, it is sufficient 
             * to remove the thread on top of the priority ordered list. */
            qhp = (NUTTHREADINFO **)(td->td_queue);
            NutEnterCritical();
            td->td_qpec--;
            tqp = *qhp;
            NutExitCritical();
            if (tqp != SIGNALED) {
                NutEventPostAsync((HANDLE *)qhp);
            }
        }
        td = td->td_next;
    }

    /*
     * Process elapsed timers. Must be done after processing the
     * events from interupt routines.
     */
    NutTimerProcessElapsed();

    /* Check for context switch. */
    if (runningThread != runQueue) {
#ifdef NUTTRACER
        TRACE_ADD_ITEM(TRACE_TAG_THREAD_YIELD,(int)runningThread);
#endif

        if (runningThread->td_state == TDS_RUNNING) {
            runningThread->td_state = TDS_READY;
        }
        NutEnterCritical();
        NutThreadSwitch();
        NutExitCritical();
    }
}

/*!
 * \brief Resume a previously suspended thread.
 *
 * This routine is called by the system when a
 * sleep timer elapses.
 *
 * \note This routine is used as a timer callback
 *       for NutSleep implementation
 *       Applications typically do not call this
 *       function.
 *
 * \param timer Handle of the elapsed timer.
 * \param th    Handle of the thread to wake up.
 *
 * \todo Used by the timer module. Should be moved there, because not all
 *       applications will use of NutSleep().
 */
void NutThreadWake(HANDLE timer, HANDLE th)
{
    NUTASSERT(th != NULL);

    /* clear pointer on timer and waiting queue */
    ((NUTTHREADINFO *) th)->td_timer = 0;
    ((NUTTHREADINFO *) th)->td_state = TDS_READY;
    NutThreadAddPriQueue(th, (NUTTHREADINFO **) & runQueue);
}

/*!
 * \brief Give up the CPU.
 *
 * If another thread within the same or higher priority
 * is ready to run, then the current thread is stopped
 * and the other one is started.
 *
 */
void NutThreadYield(void)
{

#ifdef __NUT_EMULATION__
    NutEnterCritical();
    NutUnixThreadYieldHook();
    NutExitCritical();
#endif

    /*
     * Remove current thread from runQueue and reinsert it.
     * The idle thread is the last one in the queue and will
     * never be removed.
     */
    if (runningThread->td_qnxt) {
        NutThreadRemoveQueue(runningThread, (NUTTHREADINFO **) & runQueue);
        NutThreadAddPriQueue(runningThread, (NUTTHREADINFO **) & runQueue);
    }

    /* Continue with the highest priority thread, which is ready to run. */
    NutThreadResume();
}

/*!
 * \brief Set the current thread's priority.
 *
 * The priority of newly created threads is set to 64,
 * but may be changed when the thread starts running.
 *
 * Changing the priority level to 255 will kill the
 * calling thread.
 *
 * When another thread with a higher or equal priority
 * is ready to run, the current thread will be stopped
 * and control of the CPU is passed to the other thread.
 *
 * The function returns the old priority, which makes it
 * easy to temporarily switch to another priority and
 * later set back the old one.
 *
 * \param level New priority level or 255 to kill the thread. Zero
 *              specifies the highest priority. The idle thread is
 *              running at level 254 (lowest priority). Application 
 *              threads should use levels from 32 to 253.
 *
 * \return The old priority of this thread.
 *
 * \todo Using a specific priority level for killing a thread is actually
 *       not the best idea. NutThreadKill() can be used instead.
 */
uint8_t NutThreadSetPriority(uint8_t level)
{
    uint8_t last = runningThread->td_priority;

    /*
     * Remove the thread from the run queue and re-insert it with a new
     * priority, if this new priority level is below 255. A priotity of
     * 255 will kill the thread.
     */
    NutThreadRemoveQueue(runningThread, &runQueue);
    runningThread->td_priority = level;
    if (level < 255) {
        NutThreadAddPriQueue(runningThread, (NUTTHREADINFO **) & runQueue);
    } else {
        NutThreadKill();
    }

    /*
     * Are we still on top of the queue? If yes, then change our status
     * back to running, otherwise do a context switch.
     */
    if (runningThread == runQueue) {
        runningThread->td_state = TDS_RUNNING;
    } else {
        runningThread->td_state = TDS_READY;
#ifdef NUTTRACER
        TRACE_ADD_ITEM(TRACE_TAG_THREAD_SETPRIO,(int)runningThread);
#endif

        NutEnterCritical();
        NutThreadSwitch();
        NutExitCritical();
    }

    return last;
}

/*!
 * \brief End the current thread
 *
 * Terminates the current thread, in due course the memory associated
 * with the thread will be released back to the OS this is done by the
 * idle thread.
 *
 * \todo NutThreadKill() can be used instead of setting the priority level
 *       to 255.
 */
void NutThreadExit(void)
{
    NutThreadSetPriority(255);
}

/*!
 * \brief Free a thread that was previously killed and release memory
 *        back to the OS.
 *
 * Called when another thread is killed and by the idle thread.
 *
 * Applications generally do not call this function, however you could
 * call it to try to reclaim memory.
 */
void NutThreadDestroy(void)
{
    if (killedThread) {
        NutStackFree(killedThread->td_memory);
        killedThread = 0;
    }
}

/*!
 * \brief Kill the running thread.
 *
 * The thread is moved from the schedule que and
 *
 * Applications generally do not call this function.
 */
void NutThreadKill(void)
{

    NUTTHREADINFO *pCur = nutThreadList;
    NUTTHREADINFO **pp = (NUTTHREADINFO **) & nutThreadList;

    /* Free up any unfinished already killed threads. */
    NutThreadDestroy();

    /* Remove from the thread list. */
    while (pCur) {
        if (pCur == runningThread) {
            *pp = pCur->td_next;
            break;
        }

        pp = (NUTTHREADINFO **) & pCur->td_next;
        pCur = pCur->td_next;
    }

    /* Schedule for cleanup. */
    killedThread = runningThread;
}

/*!
 * \brief Query handle of a thread with a specific name.
 *
 * \param name Case sensitive symbolic name of the thread.
 *
 * \return Handle of the thread, if it exists. Otherwise NULL is returned.
 *
 * \todo Rarely used helper function. Should be placed in a seperate module.
 */
HANDLE GetThreadByName(char * name)
{
    NUTTHREADINFO *tdp;

    if (name) {
        for (tdp = nutThreadList; tdp; tdp = tdp->td_next) {
            if (strcmp(tdp->td_name, name) == 0)
                return tdp;
        }
    } else {
        return runningThread;
    }
    return NULL;
}

#if defined(NUTDEBUG_CHECK_STACKMIN) || defined(NUTDEBUG_CHECK_STACK)
/* Calculate the size if untouched stack space. */
static size_t StackAvail(NUTTHREADINFO *td)
{
    uint32_t *sp = (uint32_t *)td->td_memory;

    while(*sp++ == DEADBEEF);

    return (size_t)((uintptr_t)sp - (uintptr_t)td->td_memory);
}

/*!
 * \brief Return the size of unused stack space.
 *
 * The stack will be treated as an array of 32-bit values, which
 * are initially set to \ref DEADBEEF. Starting at the stack's 
 * bottom, this function will simply count the number of array 
 * members, which still contain the original value.
 *
 * This implicates at least three limitations:
 *
 * - Overflows may be undetected, if some local variables are unmodified.
 * - The result may be wrong, if local variables contain the DEADBEEF value.
 * - The result is a multiple of 4 bytes.
 *
 * This function is available only if NUTDEBUG_CHECK_STACK has
 * been defined during system build.
 *
 * \param name Symbolic name of the thread.
 *
 * \return The number of bytes which never had been touched.
 */
size_t NutThreadStackAvailable(char *name)
{
    NUTTHREADINFO *tdp = (NUTTHREADINFO *)GetThreadByName(name);

    return tdp ? StackAvail(tdp) : 0;
}

/*!
 * \brief Check all Nut/OS threads for sufficient stack space.
 *
 * See NutThreadStackAvailable() for further informations.
 *
 * \param minleft Number of bytes that should have been unused.
 *
 * \return Pointer to the first thread that used too much stack
 *         space or NULL, if enough stack space has been available
 *         so far in all threads.
 */
NUTTHREADINFO *NutThreadStackCheck(size_t minsiz)
{
    NUTTHREADINFO *tdp;

    for (tdp = nutThreadList; tdp; tdp = tdp->td_next) {
        if (StackAvail(tdp) < minsiz) {
            break;
        }
    }
    return tdp;
}
#endif

/*@}*/
