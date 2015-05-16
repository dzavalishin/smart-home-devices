#ifndef _SYS_MSG_H
#define _SYS_MSG_H

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
 * Revision 1.3  2008/08/11 07:00:25  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.2  2004/03/08 11:20:36  haraldkipp
 * Not all compilers like arrays of size zero.
 *
 * Revision 1.1  2004/02/04 18:05:04  drsung
 * First version of message queueing  implemented. Thanks to Ralph Mason, who provided this code.
 * Still contains some debug functions.
 *
 */

#include <sys/timer.h>
#include <stdint.h>

#ifdef _PC
#pragma warning (disable:4200)
#endif

typedef struct _NUTMSG NUTMSG;
struct _NUTMSG {
    uint8_t id;
    int param;
    void *data;
};

typedef struct _NUTMSGTMR NUTMSGTMR;
struct _NUTMSGTMR;

typedef struct _NUTMSGQ NUTMSGQ;
struct _NUTMSGQ {
    HANDLE mq_wait;             /* Wait for a message */
    NUTMSGTMR *mq_timers;
    NUTMSGQ *mq_next;
    uint8_t mq_read;
    uint8_t mq_write;
    uint8_t mq_mask;
    NUTMSG mq_que[1];
};


extern NUTMSGQ *NutMsgQCreate(uint8_t len);
extern int NutMsgQBroadcast(uint8_t id, int param, void *data);
extern int NutMsgQPost(NUTMSGQ * que, uint8_t id, int param, void *data);
extern int NutMsgQSend(NUTMSGQ * que, uint8_t id, int param, void *data);
extern int NutMsgQGetMessage(NUTMSGQ * que, NUTMSG * msg, uint32_t timeout);
extern int NutMsgQFull(NUTMSGQ * que);
extern void NutMsgQFlush(NUTMSGQ * que);
extern HANDLE NutMsgQStartTimer(NUTMSGQ * que, uint32_t ms, int param, void *data, uint8_t flags);
extern void NutMsgQStopTimer(HANDLE timer);

#define MSG_TIMER	0xfe
#define MSG_NULL	0xff

#endif
