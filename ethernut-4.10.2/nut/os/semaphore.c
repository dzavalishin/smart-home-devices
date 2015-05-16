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

/* semaphore.c - a nut/os implementation of semaphore functions
 *
 * 2004.05.06 Matthias Ringwald <matthias.ringwald@inf.ethz.ch>
 *
 */
/*
 * $Log$
 * Revision 1.9  2009/01/17 15:37:52  haraldkipp
 * Added some NUTASSERT macros to check function parameters.
 *
 * Revision 1.8  2008/06/16 15:12:07  freckle
 * fix bug in NutSemTryWait of os/semaphore.c
 *
 * Revision 1.7  2006/10/08 16:48:22  haraldkipp
 * Documentation fixed
 *
 * Revision 1.6  2005/08/02 17:47:04  haraldkipp
 * Major API documentation update.
 *
 * Revision 1.5  2004/06/03 08:44:50  olereinhardt
 * According to a hint from oliver I changed NutEventWait to NutEventWaitNext
 *
 * Revision 1.4  2004/06/03 08:24:21  olereinhardt
 * Changed semaphore behavior in NutSemTryWait too.
 *
 * Revision 1.3  2004/06/02 16:42:53  olereinhardt
 * fixed bug (integer overflow) in semaphore implementation.
 *
 * Revision 1.2  2004/05/18 18:38:42  drsung
 * Added $Log keyword for CVS.
 *
 */

/*!
 * \addtogroup xgSemaphore
 */
/*@{*/

#ifdef __cplusplus
extern "C" {
#endif

#include <sys/nutdebug.h>
#include <sys/semaphore.h>
#include <sys/event.h>

/*!
 * \brief Initialize an unnamed semaphore to value
 */
    void NutSemInit(SEM * sem, short value) {
        NUTASSERT(sem != NULL);
        sem->qhp = 0;
        sem->value = value;
    }
/*!
 * \brief Lock a semaphore
 *
 *  If the semaphore value is currently zero, then the calling thread will not
 *  return from the call to sem_wait() the semaphore becomes available
 *
 * \note: Should not be called from interrupt context
 */ void NutSemWait(SEM * sem) {
        NUTASSERT(sem != NULL);
        sem->value--;
        if (sem->value < 0)
        {
            NutEventWaitNext(&sem->qhp, NUT_WAIT_INFINITE);
        }
    }

/*!
 * \brief Unlock a sempahore.
 *
 * \note: Should not be called from interrupt context
 */
    void NutSemPost(SEM * sem) {
        NUTASSERT(sem != NULL);
        sem->value++;
        if (sem->value <= 0)
        {
            NutEventPost(&sem->qhp);
        }
    }

/*!
 * \brief Attempt to lock a semaphore without blocking
 *
 * Return zero, if successful, otherwise the sempahore is already locked
 * \note: Should not be called from interrupt context
 */

    int NutSemTryWait(SEM * sem) {
        NUTASSERT(sem != NULL);
        if (sem->value <= 0)
            return -1;
        else
            NutSemWait(sem);
        return 0;
    }

/*!
 * \brief Free resources allocated for a semaphore
 *
 * Return zero, if successful, otherwise there are threads blocked on the
 * sempahore
 */

    int NutSemDestroy(SEM * sem) {
        NUTASSERT(sem != NULL);
        if (sem->qhp == SIGNALED)
            return 0;
        if (sem->qhp == 0)
            return 0;
        return -1;
    }

#ifdef __cplusplus
}
#endif

/*@}*/
