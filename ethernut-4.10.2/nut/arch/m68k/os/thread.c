/*
 * Copyright (C) 2001-2004 by egnite Software GmbH. All rights reserved.
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
 * Revision 1.1  2005/05/27 17:21:10  drsung
 * Moved the file.
 *
 * Revision 1.2  2005/04/30 16:42:42  chaac
 * Fixed bug in handling of NUTDEBUG. Added include for cfg/os.h. If NUTDEBUG
 * is defined in NutConf, it will make effect where it is used.
 *
 * Revision 1.1  2004/03/16 16:48:46  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 * Revision 1.2  2004/02/18 16:32:50  drsung
 * Bugfix in NutThreadCreate. Thanks to Mike Cornelius.
 *
 * Revision 1.1  2004/02/01 18:49:48  haraldkipp
 * Added CPU family support
 *
 */

#include <cfg/os.h>

/*!
 * \brief Coldfire GCC context switch frame layout.
 *
 * This is the layout of the stack after a thread's context has been 
 * switched-out.
 */
typedef struct {
    u_char csf_pchi;
    u_char csf_pclo;
} SWITCHFRAME;

/*!
 * \brief Thread entry frame layout.
 *
 * This is the stack layout being build to enter a new thread.
 */
typedef struct {
    u_char cef_arghi;
    u_char cef_arglo;
    u_char cef_pchi;
    u_char cef_pclo;
} ENTERFRAME;


/*
 * This code is executed when entering a thread.
 */
static void NutThreadEntry(void) __attribute__ ((naked));
static void NutThreadEntry(void)
{
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
 */
void NutThreadSwitch(void) __attribute__ ((noinline));
void NutThreadSwitch(void)
{
    /*
     * Save all CPU registers.
     */

    /*
     * This defines a global label, which may be called
     * as an entry point into this function.
     */
    asm volatile (".global thread_start\n" "thread_start:\n\t"::);

    /*
     * Reload CPU registers from the thread on top of the run queue.
     */
    runningThread = runQueue;
    runningThread->td_state = TDS_RUNNING;
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
HANDLE NutThreadCreate(u_char * name, void (*fn) (void *), void *arg, size_t stackSize)
{
    u_char *threadMem;
    SWITCHFRAME *sf;
    ENTERFRAME *ef;
    NUTTHREADINFO *td;

    const uptr_t *paddr;
    NutEnterCritical();
    paddr = (const uptr_t *) fn;

    /*
     * Allocate stack and thread info structure in one block.
     */
    if ((threadMem = NutHeapAlloc(stackSize + sizeof(NUTTHREADINFO))) == 0) {
        NutExitCritical();
        return 0;
    }

    td = (NUTTHREADINFO *) (threadMem + stackSize);
    ef = (ENTERFRAME *) ((uptr_t) td - sizeof(ENTERFRAME));
    sf = (SWITCHFRAME *) ((uptr_t) ef - sizeof(SWITCHFRAME));


    memcpy(td->td_name, name, sizeof(td->td_name) - 1);
    td->td_name[sizeof(td->td_name) - 1] = 0;
    td->td_sp = (uptr_t) sf - 1;
    td->td_memory = threadMem;
    *((u_long *) threadMem) = DEADBEEF;
    *((u_long *) (threadMem + 4)) = DEADBEEF;
    *((u_long *) (threadMem + 8)) = DEADBEEF;
    *((u_long *) (threadMem + 12)) = DEADBEEF;
    td->td_priority = 64;

    /*
     * Setup entry frame to simulate C function entry.
     */
    ef->cef_pchi = (u_char) (((uptr_t) fn) >> 8);
    ef->cef_pclo = (u_char) (((uptr_t) fn) & 0xff);

    ef->cef_arglo = (u_char) (((uptr_t) arg) & 0xff);
    ef->cef_arghi = (u_char) (((uptr_t) arg) >> 8);

    sf->csf_pchi = (u_char) (((uptr_t) NutThreadEntry) >> 8);
    sf->csf_pclo = (u_char) (((uptr_t) NutThreadEntry) & 0xff);

    /*
     * Insert into the thread list and the run queue.
     */
    td->td_next = nutThreadList;
    nutThreadList = td;
    td->td_state = TDS_READY;
    td->td_timer = 0;
    td->td_queue = 0;
#ifdef NUTDEBUG
    if (__os_trf)
        fprintf(__os_trs, "Cre<%08lx>", (uptr_t) td);
#endif
    NutThreadAddPriQueue(td, (NUTTHREADINFO **) & runQueue);
#ifdef NUTDEBUG
    if (__os_trf)
        NutDumpThreadList(__os_trs);
#endif

    /*
     * If no thread is active, switch to new thread.
     */
    if (runningThread == 0)
        __asm__ __volatile__(";rjmp thread_start");

    /*
     * If current context is not in front of
     * the run queue (highest priority), then
     * switch to the thread in front.
     */
    if (runningThread != runQueue) {
        runningThread->td_state = TDS_READY;
#ifdef NUTDEBUG
        if (__os_trf)
            fprintf(__os_trs, "New<%08lx %08lx>", (uptr_t) runningThread, (uptr_t) runQueue);
#endif
        NutThreadSwitch();
    }
    NutExitCritical();

    return td;
}
