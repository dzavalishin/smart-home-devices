/*
 * Copyright (C) 2001-2003 by Telogis.com. All rights reserved.
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
 * This code was kindly provided by Ralph Mason.
 *
 */

/*
 * $Log$
 * Revision 1.6  2008/08/11 07:00:34  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.5  2006/10/08 16:48:22  haraldkipp
 * Documentation fixed
 *
 * Revision 1.4  2005/01/21 16:49:44  freckle
 * Seperated calls to NutEventPostAsync between Threads and IRQs
 *
 * Revision 1.3  2005/01/19 17:59:46  freckle
 * Improved interrupt performance by reducing some critical section
 *
 * Revision 1.2  2004/03/16 16:48:45  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 * Revision 1.1  2004/02/04 18:05:07  drsung
 * First version of message queueing  implemented. Thanks to Ralph Mason, who provided this code.
 * Still contains some debug functions.
 *
 */
 
#include <sys/heap.h>
#include <sys/event.h>
#include <sys/atom.h>
#include <sys/thread.h>
#include <sys/msg.h>
#include <stddef.h> /* NULL definition */


#define ASSERT(x)

struct _NUTMSGTMR {
    NUTMSGQ *mt_que;
    int mt_param;
    void *mt_data;
    HANDLE mt_handle;
    NUTMSGTMR *mt_next;
    uint8_t mt_flags;
};

/*!
 * \addtogroup xgMessageQue
 */
/*@{*/

/*!
 * \brief global list of ques
 */
NUTMSGQ *nutMsgQue;

NUTMSGTMR *nutMsgFreeTimers;

/*!
 * \brief Allocate a new message que
 *
 * \param bits size of the que in bits 
 *
 * \return Handle of the new que
 *
 * \note que size will be 2^bits
 */
NUTMSGQ *NutMsgQCreate(uint8_t bits)
{

    NUTMSGQ *que;
    uint8_t len = 1 << bits;

    ASSERT(bits < 7);
    ASSERT(bits > 0);

    que = (NUTMSGQ *) (NutHeapAllocClear(sizeof(NUTMSGQ) + (sizeof(NUTMSG) * (len))));

    if (!que)
        return que;

    que->mq_mask = len - 1;

    /*Link into the Global list */
    que->mq_next = nutMsgQue;
    nutMsgQue = que;

    return que;
}

/*!
 * \brief Send a message to all message ques
 *
 * \param id the id of the sent message
 * \param param the param of the sent message
 * \param data the data of the sent message
 *
 * \return 0 if sent to all ques, < 0 if one or more ques were full
 *
 */
int NutMsgQBroadcast(uint8_t id, int param, void *data)
{

    NUTMSGQ *pCur = nutMsgQue;
    int ret = 0;

    while (pCur) {
        ret -= NutMsgQPost(pCur, id, param, data);
        pCur = pCur->mq_next;
    }

    return ret;
}

/*!
 * \brief Send a message to a que and return immediately
 *
 * \param que the que to send to
 * \param id the id of the sent message
 * \param param the param of the sent message
 * \param data the data of the sent message
 *
 * \return 0 if sent, < 0 if the que is full
 *
 */
int NutMsgQPost(NUTMSGQ * que, uint8_t id, int param, void *data)
{
    NUTMSG *cur;
    NutEnterCritical();

    if (NutMsgQFull(que)) {
        NutJumpOutCritical();
        return -1;
    }

    cur = que->mq_que + que->mq_write;

    cur->id = id;
    cur->param = param;
    cur->data = data;

    que->mq_write++;
    que->mq_write &= que->mq_mask;

    NutExitCritical();

    NutEventPostAsync(&que->mq_wait);
    
    return 0;
}

/*!
 * \brief Send a message to a que and yields so that 
 * a waiting thread can act on the message. 
 *
 * The sending thread should have a lower priority than the receiver thread
 *
 * \param que the que to send to
 * \param id the id of the sent message
 * \param param the param of the sent message
 * \param data the data of the sent message
 *
 * \return 0 if sent, < 0 if the que is full
 *
 */
int NutMsgQSend(NUTMSGQ * que, uint8_t id, int param, void *data)
{
    if (NutMsgQPost(que, id, param, data) == 0) {
        NutThreadYield();
        return 0;
    }
    return -1;
}

/*!
 * \brief Checks the state of a que
 *
 * \return -1 if full
 */
int NutMsgQFull(NUTMSGQ * que)
{
    if (((que->mq_write + 1) & que->mq_mask) == que->mq_read) {
        return -1;
    }
    return 0;
}


static void NutMsgQTimerCb(HANDLE hndl, void *arg)
{
    NUTMSGTMR *timer = (NUTMSGTMR *) arg;

    if (NutMsgQPost(timer->mt_que, MSG_TIMER, 0, timer)) {
        /*
         * If a oneshot is missed we need to restart it until
         * It gets into the que otherwise we can not deallocate the NUTMSGTMR
         * Also oneshots are important we expect it will go off
         */
        if (timer->mt_flags && TM_ONESHOT) {
            timer->mt_handle = NutTimerStartTicks(1, NutMsgQTimerCb, timer, TM_ONESHOT);
        }
    } else {
        /*We can't kill it b/c it kills it's self */
        if (timer->mt_flags && TM_ONESHOT)
            timer->mt_handle = NULL;
    }
}


/*!
 * \brief Starts a periodic or one-shot timer on the given que
 *
 * \param que the que to send to
 * \param ms timeout length of the timer
 * \param param the param of the sent message
 * \param data the data of the sent message
 * \param flags 0 or TM_ONESHOT
 *
 * \return HANDLE of the new timer
 *
 */
HANDLE NutMsgQStartTimer(NUTMSGQ * que, uint32_t ms, int param, void *data, uint8_t flags)
{
    NUTMSGTMR *timer = nutMsgFreeTimers;

    ASSERT(flags == TM_ONESHOT || flags == 0);

    if (timer != NULL) {
        nutMsgFreeTimers = timer->mt_next;
    } else {
        timer = (NUTMSGTMR *) NutHeapAlloc(sizeof(NUTMSGTMR));
    }

    timer->mt_que = que;
    timer->mt_data = data;
    timer->mt_param = param;
    timer->mt_flags = flags;
    timer->mt_handle = NutTimerStart(ms, NutMsgQTimerCb, timer, flags);

    timer->mt_next = que->mq_timers;
    que->mq_timers = timer;
    return (HANDLE) timer;
}

static void NutMsgQFreeTimer(NUTMSGQ * que, NUTMSGTMR * handle)
{
    NUTMSGTMR *tnp = que->mq_timers;
    NUTMSGTMR **tnpp = &que->mq_timers;

    while (tnp) {
        if (tnp == handle) {
            *tnpp = tnp->mt_next;
            tnp->mt_next = nutMsgFreeTimers;
            nutMsgFreeTimers = tnp;
            return;
        }
        tnpp = &tnp->mt_next;
        tnp = tnp->mt_next;
    }

    ASSERT(0);                  /*Timer already freed */
}

/*!
 * \brief Stops a timer
 *
 * \param timer HANDLE of the timer to stop
 *
 * \note You must not stop a one shot that has already expired
 * otherwise it *could* have been reused and you will end up stopping
 *  another timer
 * 
 */
void NutMsgQStopTimer(HANDLE timer)
{
    NUTMSGTMR *t = (NUTMSGTMR *) timer;
    NUTMSGQ *que = t->mt_que;

    /*
     * We need to remove any message in the que from this timer
     * If you stop it you don't want a message from it
     */
    NutEnterCritical();
    {
        uint8_t pos = que->mq_read;

        while (pos != que->mq_write) {
            if (que->mq_que[pos].id == MSG_TIMER && que->mq_que[pos].data == t) {
                que->mq_que[pos].id = MSG_NULL;
            }

            pos = (pos + 1) & que->mq_mask;
        }
    }

    if (t->mt_handle)
        NutTimerStop(t->mt_handle);

    NutExitCritical();

    NutMsgQFreeTimer(que, t);
}

/*!
 * \brief Gets the next message from the que
 *
 * \param que the que to wait on
 * \param msg pointer to memory to return data to
 * \param timeout how long to wait for a message
 * 
 * \return -1 on timeout, 0 if message retreived
 */

int NutMsgQGetMessage(NUTMSGQ * que, NUTMSG * msg, uint32_t timeout)
{
    NutEnterCritical();

    if (NutEventWait(&que->mq_wait, timeout)) {
        NutJumpOutCritical();
        return -1;
    }
    /* Are there messages in the queue */
    ASSERT(que->mq_read != que->mq_write);

    *msg = *(que->mq_que + que->mq_read);

    que->mq_read++;
    que->mq_read &= que->mq_mask;

    /* If more messages then we need to Post so we can get the next one */
    if (que->mq_read != que->mq_write)
        NutEventPostAsync(&que->mq_wait);

    NutExitCritical();

    if (msg->id == MSG_TIMER) {
        NUTMSGTMR *timer = (NUTMSGTMR *) msg->data;
        msg->data = timer->mt_data;
        msg->param = timer->mt_param;

        if (timer->mt_flags & TM_ONESHOT) {
            NutMsgQFreeTimer(que, timer);
        }
    }

    return 0;
}

/*!
 * \brief Removes all entries from a que
 */
void NutMsgQFlush(NUTMSGQ * que)
{
    NutEnterCritical();

    que->mq_read = que->mq_write;

    /*
       // You want to flush only when you are not waitin on it 
       ASSERT( que->event_wait == SIGNALED || que->event_wait == 0 )
     */

    que->mq_wait = 0;
    NutExitCritical();
}

/*@}*/
