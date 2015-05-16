/*
 * Copyright (C) 2008 by EmbeddedIT,
 * Ole Reinhardt <ole.reinhardt@embedded-it.de> All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY EMBEDDED IT AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL EMBEDDED IT
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * For additional information see http://www.ethernut.de/
 *
 */

/*
 * $Log$
 * Revision 1.3  2008/08/11 07:00:33  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.2  2008/04/29 01:51:35  thiagocorrea
 * Compile fix
 *
 * Revision 1.1  2008/04/21 22:24:53  olereinhardt
 * Implemented condition variables to use with NutOS as an application candy
 *
 */

#include <sys/heap.h>
#include <sys/event.h>
#include <sys/atom.h>
#include <sys/timer.h>
#include <sys/thread.h>
#include <sys/condition.h>
#include <stddef.h> /* NULL definition */

/*!
 * \addtogroup xgConditionVariables
 */
/*@{*/

/*!
 * \brief Creates a new condition variable
 *
 * \return Handle of the condition variable, NULL if not enough memory available
 */

int NutConditionInit( CONDITION * cond)
{
    cond = NutHeapAlloc(sizeof(struct _CONDITION));
    if (cond == NULL) return 1;
    NutMutexInit(&cond->mutex);
    return 0;
}

/*!
 * \brief Locks the condition mutex
 *
 * To avoid the "lost wakeup" bug it is important to always lock the condition before
 * modifying the condition or signalling the condition variable
 *
 * \param cond The condition to be locked
 *
 */

void NutConditionLock(CONDITION * cond)
{
    if (cond == NULL) return;
    NutMutexLock(&cond->mutex);
}

/*!
 * \brief Unocks the condition mutex
 *
 * Always unlock the confition after modifying the condition and signalling the
 * condition variable.
 *
 * \param cond The condition to be unlocked
 *
 */

void NutConditionUnlock(CONDITION * cond)
{
    if (cond == NULL) return;
    NutMutexUnlock(&cond->mutex);
}

/*!
 * \brief Waits until this thread is woken up on cond.
 *
 * The condition is unlocked before falling asleep and locked again before resuming.
 *
 * It is important to use the NutConditionWait() and NutConditionTimedWait()
 * functions only inside a loop which checks for the condition to be true.
 * It is not guaranteed that the waiting thread will find the condition
 * fulfilled after it wakes up, even if the signaling thread left the condition
 * in that state: another thread may have altered the condition before the
 * waiting thread got the chance to be woken up, even if the condition itself
 * is protected by locking with NutConditionLock.
 *
 * Always lock the condition before entering the above mentioned check loop
 * and always unlock the condition after successfully leaving the loop and
 * processing the data you wait for.
 *
 * \param cond The condition to wait on.
 *
 * \return 0 on success, -1 on error
 *
 */

int NutConditionWait(CONDITION * cond)
{
    if (cond == NULL) return -1;
    NutMutexUnlock(&cond->mutex);
    NutEventWait(cond->event, NUT_WAIT_INFINITE);
    NutMutexLock(&cond->mutex);
    return 0;
}

/*!
 * \brief Waits until this thread is woken up on cond but not longer than until
 * the time specified by abs_ms.
 *
 * The condition is unlocked before falling asleep and locked again before resuming.
 *
 * It is important to use the NutConditionWait() and NutConditionTimedWait()
 * functions only inside a loop which checks for the condition to be true.
 * It is not guaranteed that the waiting thread will find the condition
 * fulfilled after it wakes up, even if the signaling thread left the condition
 * in that state: another thread may have altered the condition before the
 * waiting thread got the chance to be woken up, even if the condition itself
 * is protected by locking with NutConditionLock.
 *
 * Always lock the condition before entering the above mentioned check loop
 * and always unlock the condition after successfully leaving the loop and
 * processing the data you wait for.
 *
 * \param cond The condition to wait on.
 * \param abs_ms Absolute time in ms to longest wait for. Use NutGetMillis() to
 *        obtain the current time and add your desired offset. Overflows are
 *        handled correct. At longest you can wait 2147483648ms
 *
 * \return 0 on success, -1 on error or timeout
 *
 *
 */

int NutConditionTimedWait(CONDITION * cond, uint32_t abs_ms)
{
    uint32_t ms;

    if (cond == NULL) return -1;
    ms = abs_ms - NutGetMillis();
    if (ms > 0x7FFFFFFF) return -1;

    NutMutexUnlock(&cond->mutex);
    NutEventWait(&cond->event, ms);
    NutMutexLock(&cond->mutex);
    return 0;
}

/*!
 * \brief If threads are waiting for cond, exactly one of them is woken up.
 *
 * Call this function after you fullfilled the condition. The conditon should be
 * locked befor fulfilling the condition should not be unlocked before calling
 * this function.
 *
 * \param cond The condition to signal
 * \return -1 on error. Otherwise the number of woken up threads.
 *
 */

int NutConditionSignal(CONDITION * cond)
{
    if (cond == NULL) return -1;
    return NutEventPost(&cond->event);
}

/*!
 * \brief If threads are waiting for cond, all of them are woken up.
 *
 * Call this function after you fullfilled the condition. The conditon should be
 * locked befor fulfilling the condition should not be unlocked before calling
 * this function.
 *
 * \param cond The condition to signal
 * \return -1 on error. Otherwise the number of woken up threads.
 *
 */

int NutConditionBroadcast(CONDITION * cond)
{
    if (cond == NULL) return -1;
    return NutEventBroadcast(&cond->event);
}

/*!
 * \brief Free the ressources used by this condition variable
 *
 * \param cond Pointer to the condition
 */
void NutConditionFree(CONDITION * cond)
{
    NutMutexDestroy(&cond->mutex);
    NutHeapFree(cond);
    cond = NULL;
}

/*@}*/
