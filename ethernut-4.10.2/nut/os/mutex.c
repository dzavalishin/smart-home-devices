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
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
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

/* mutex.c - a nut/os implementation of recursive mutex functions
 *
 * 2004.05.06 Matthias Ringwald <matthias.ringwald@inf.ethz.ch>
 *
 */
/*
 * $Log$
 * Revision 1.6  2009/01/17 15:37:52  haraldkipp
 * Added some NUTASSERT macros to check function parameters.
 *
 * Revision 1.5  2006/10/08 16:48:22  haraldkipp
 * Documentation fixed
 *
 * Revision 1.4  2005/08/02 17:47:04  haraldkipp
 * Major API documentation update.
 *
 * Revision 1.3  2004/07/19 16:24:23  freckle
 * Code contained same bug as os/semaphore.c.
 * Unfortunately I didn't understand the other fix, but anyway made
 * NutMutexLock more robust.
 * Fixed wrong indention of all functions
 *
 * Revision 1.2  2004/05/18 18:38:42  drsung
 * Added $Log keyword for CVS.
 *
 */

/*!
 * \addtogroup xgMutex
 */
/*@{*/

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/nutdebug.h>
#include <sys/mutex.h>
#include <sys/event.h>

/*!
 * \brief Create a mutex
 *
 * The type for the mutex is recursive
 */
 
void NutMutexInit(MUTEX * mutex) {
    NUTASSERT(mutex != NULL);
    mutex->thread = 0;
    mutex->count = 0;
    mutex->qhp = 0;
}

/*!
 * \brief Lock a mutex
 *
 * If the mutex is already locked by another thread,
 * the thread will block until the mutex becomes available
 *
 * \note: Should not be called from interrupt context
 */
 
void NutMutexLock(MUTEX * mutex) {
    NUTASSERT(mutex != NULL);
    if (mutex->thread != runningThread) {
        while( mutex->count != 0)
            NutEventWaitNext(&mutex->qhp, NUT_WAIT_INFINITE);
    }
    mutex->thread = runningThread;
    mutex->count++;
}

/*!
 * \brief Attempt to lock a mutex without blocking
 *
 * Return zero, if successful, otherwise the mutex is already locked
 * by another thread
 * \note: Should not be called from interrupt context
 */
 
int NutMutexTrylock(MUTEX * mutex) {
    NUTASSERT(mutex != NULL);
    if ((mutex->count != 0) && (mutex->thread != runningThread))
        return -1;
    NutMutexLock(mutex);
    return 0;
}

/*!
 * \brief Unlock a mutex.
 *
 * Return zero, if successful, otherwise the current thread does not
 * hold a lock on mutex.
 * \note: Should not be called from interrupt context
 */
 
int NutMutexUnlock(MUTEX * mutex) {
    NUTASSERT(mutex != NULL);
    if (mutex->thread != runningThread)
        return -1;
    if (--mutex->count == 0) {
        NutEventPost(&mutex->qhp);
    }
    return 0;
}

/*!
 * \brief Free resources allocated for a mutex
 *
 * Return zero, if successful, otherwise the mutex is locked
 * by another thread
 */

int NutMutexDestroy(MUTEX * mutex) {
    NUTASSERT(mutex != NULL);
    if (mutex->count == 0)
        return 0;
    if (mutex->thread == runningThread)
        return 0;
    return -1;
}

#ifdef __cplusplus
}
#endif

/*@}*/
