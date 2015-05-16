/*
 * Copyright (C) 2000-2004 by ETH Zurich
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
 * THIS SOFTWARE IS PROVIDED BY ETH ZURICH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ETH ZURICH
 *  OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
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
 * unix_thread.c - unix emulation of cooperative threads using pthreads
 *
 * 2004.04.01 Matthias Ringwald <matthias.ringwald@inf.ethz.ch>
 *
 */

#include <cfg/os.h>
#include <pthread.h>
#include <stdio.h>

// #include <cfg/memory.h>

#include <string.h>

#include <sys/types.h>
#include <sys/heap.h>
#include <sys/atom.h>
// #include <sys/timer.h>
#include <sys/thread.h>

#ifdef NUTDEBUG
#include <sys/osdebug.h>
#endif


/*!
 * \addtogroup xgNutArchUnixOsContext
 */
/*@{*/

/* reused parameters for other calls */
pthread_attr_t attr;

/* protect thread signaling */
pthread_mutex_t thread_mutex;

/* to get a new thread start acked'd */
pthread_cond_t main_cv;

/* level of critical sections entered but not left outside a thread */
uint16_t main_cs_level;

/* has to be initialized once */
void NutThreadInit(void)
{
    /* Initialize mutex and condition variable objects */
    pthread_mutex_init(&thread_mutex, NULL);

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    // main_cv init
    pthread_cond_init(&main_cv, NULL);
}


/*
 * This code is executed when entering a thread.
 *  
 *
 */
static void *NutThreadEntry(void *arg);
static void *NutThreadEntry(void *arg)
{
    // get THREADINFO back
    NUTTHREADINFO *td = (NUTTHREADINFO *) arg;

    // confirm start
    pthread_mutex_lock(&thread_mutex);
    pthread_cond_signal(&main_cv);

    // wait for start signaled
    pthread_cond_wait(&td->td_cv, &thread_mutex);
    pthread_mutex_unlock(&thread_mutex);

    // set critical section value 
    td->td_cs_level = 0;

    // enable interrupts
    pthread_sigmask(SIG_UNBLOCK, &irq_signal, 0);

    // call real function
    td->td_fn(td->td_arg);

    // NutExitCritical();

    // tell nut/os about it
    NutThreadExit();

    // thread exit routine 
    pthread_exit(NULL);

    // make some compilers happy
    return NULL;
}

/*!
 * \brief Switch to another thread.
 *
 * Stop the current thread, saving its context. Then start the
 * one with the highest priority, which is ready to run.
 *
 * Application programs typically do not call this function.
 *
 * \note CPU interrupts must be disabled before calling this function.
 *
 * \note which means NutEnterCritical was called before
 */
void NutThreadSwitch(void);
void NutThreadSwitch(void)
{
    NUTTHREADINFO *myself;

    NutEnterCritical();

    // lock thread switching mutex
    pthread_mutex_lock(&thread_mutex);

    // next thread is the first one in the run queue
    myself = runningThread;
    if (runningThread != runQueue) {

        // switching call
        runningThread = runQueue;
        runningThread->td_state = TDS_RUNNING;

        pthread_cond_signal(&runQueue->td_cv);
        pthread_cond_wait(&myself->td_cv, &thread_mutex);

    }
    pthread_mutex_unlock(&thread_mutex);

    NutExitCritical();
}




/*!
 * \brief Create a new thread.
 *
 * If the current thread's priority is lower or equal than the default 
 * priority (64), then the current thread is stopped and the new one 
 * is started.
 *
 * \param name      String containing the symbolic name of the new thread, 
 *                  up to 8 characters long.
 * \param fn        The thread's entry point, typically created by the 
 *                  THREAD macro.
 * \param arg       Argument pointer passed to the new thread.
 * \param stackSize Number of bytes of the stack space allocated for 
 *                  the new thread.
 *
 * \return Pointer to the NUTTHREADINFO structure or 0 to indicate an
 *         error.
 */
HANDLE NutThreadCreate(char * name, void (*fn) (void *), void *arg, size_t stackSize)
{
    NUTTHREADINFO *td;
    const uintptr_t *paddr;

    paddr = (const uintptr_t *) fn;

    NutEnterCritical();

    /*
     * Allocate stack and thread info structure in one block.
     */
    if ((td = NutHeapAlloc(sizeof(NUTTHREADINFO))) == 0) {
        NutExitCritical();
        return 0;
    }

    /* store thread name */
    memcpy(td->td_name, name, sizeof(td->td_name) - 1);
    td->td_name[sizeof(td->td_name) - 1] = 0;

    // set initial critical section value 
    td->td_cs_level = 1;

    /*
     * Insert into the thread list and the run queue.
     */
    td->td_priority = 64;
    td->td_next = nutThreadList;
    nutThreadList = td;
    td->td_state = TDS_READY;
    td->td_timer = 0;
    td->td_queue = 0;
#ifdef NUTDEBUG
    if (__os_trf) {
        fprintf(__os_trs, "Cre<%08lx>\n", (uintptr_t) td);
    }
#endif
    NutThreadAddPriQueue(td, (NUTTHREADINFO **) & runQueue);
#ifdef NUTDEBUG
    if (__os_trf)
        NutDumpThreadList(__os_trs);
#endif

    // init thread structure
    pthread_cond_init(&td->td_cv, NULL);

    td->td_fn = fn;
    td->td_arg = arg;

    /*
     * If no thread is active, switch to new thread.
     * this also means, we're called from nutinit
     * (if not, there would be a runningThread..)
     *  
     */
    if (runningThread == 0) {

        // correct cs level counter
        // main_cs_level--;
        NutExitCritical();

        // switching
        runningThread = runQueue;
        runningThread->td_state = TDS_RUNNING;

        // set initial critical section value and enable interrupts
        runningThread->td_cs_level = 0;
        pthread_sigmask(SIG_UNBLOCK, &irq_signal, 0);

        // go!
        fn(arg);

        printf("Nut/OS Application terminated.\n\r");
        exit(0);
    };

    // lock mutex and start thread
    pthread_mutex_lock(&thread_mutex);
    pthread_create(&td->td_pthread, &attr, NutThreadEntry, (void *) td);

    // wait for ack
    pthread_cond_wait(&main_cv, &thread_mutex);
    pthread_mutex_unlock(&thread_mutex);

    /*
     * If current context is not in front of
     * the run queue (highest priority), then
     * switch to the thread in front.
     */
    if (runningThread != runQueue) {
        runningThread->td_state = TDS_READY;
#ifdef NUTDEBUG
        if (__os_trf)
            fprintf(__os_trs, "New<%08lx %08lx>\n", (uintptr_t) runningThread, (uintptr_t) runQueue);
#endif
        NutThreadSwitch();
    }
    NutExitCritical();

    return td;
}

/*@}*/
