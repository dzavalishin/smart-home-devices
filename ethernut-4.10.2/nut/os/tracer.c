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
 * For additional information see http://www.btnode.ethz.ch
 *
 */
 /*
 * os/tracer.c
 *
 * 22.12.2004 Philipp Blum <blum@tik.ee.ethz.ch>
 * 
 * \brief Routines to capture traces of nutOS programs
 * \note  Only supported on AVR-GCC platform
 * 
 */
#if defined(__GNUC__) && defined(__AVR__)
 
/************************************************/
/* includes */
/************************************************/
#include <sys/tracer.h>            // t_traceitem, t_trace
#include <sys/heap.h>              // NutHeapAlloc
#include <sys/timer.h>             // NutGetMillis
#include <sys/thread.h>            // NUTTHREADINFO
#include <sys/atom.h>			   // NutEnterCritical_notrace
#include <dev/irqreg.h>			   // sig_OVERFLOW1
#include <stdio.h>                 // printf, sscanf
#include <string.h>                // strcmp

/************************************************/
/* global variables */
/************************************************/
unsigned int micros_high           = 0;
t_traceitem *trace_items    = 0;
t_traceitem *trace_current  = 0;
int trace_head              = 0;
int trace_size              = 0;
char trace_isfull           = 0;
char trace_isinit           = 0;
char trace_mode             = TRACE_MODE_OFF;

char trace_mask[TRACE_TAG_LAST+1] = {
    0,
    0,
    1,
    1,
    1,
    1,
    0,
    0,
    1,
    1,
    1
};

char* tag_string[TRACE_TAG_LAST+1] = {
    "Critical  Enter",
    "Critical  Exit",
    "Thread    Yield",
    "Thread    SetPrio",
    "Thread    Wait",
    "Thread    Sleep",
    "Interrupt Enter",
    "Interrupt Exit",
    "Trace Start",
    "Trace Stop",
    "User *"
};

char* int_string[TRACE_INT_LAST+1] = {
    "UART0_CTS",
    "UART0_RXCOMPL",
    "UART0_TXEMPTY",
    "UART1_CTS",
    "UART1_RXCOMPL",
    "UART1_TXEMPTY",
    "TIMER0_OVERFL",
    "TIMER1_OVERFL",
    "SUART_TIMER",
    "SUART_RX"	
};

char* mode_string[TRACE_MODE_LAST+1] = {
    "OFF",
    "CIRCULAR",
    "ONESHOT"
};

char* user_string[TRACE_MAX_USER];
	
/************************************************/
/* function definitions */
/************************************************/
static void NutTraceTimer1IRQ(void *arg)
{
//  TRACE_ADD_ITEM(TRACE_TAG_INTERRUPT_ENTER,TRACE_INT_TIMER1_OVERFL);
    micros_high++;
//  TRACE_ADD_ITEM(TRACE_TAG_INTERRUPT_EXIT,TRACE_INT_TIMER1_OVERFL);
}



int NutTraceInit(int size, char mode) 
{
    if (!trace_isinit) {
        // start timer1 at CPU frequency/8 and register interrupt service routine
        outb(TCCR1B, 2);
        NutRegisterIrqHandler(&sig_OVERFLOW1, NutTraceTimer1IRQ, 0);
        sbi(TIMSK, TOIE1);
        trace_isinit = 1;
    }		
    if (size==0) {
        size = TRACE_SIZE_DEFAULT;
    }	
    if (size != trace_size) {
        // current buffer is not of size that is wanted
        if (trace_items != 0) {
            // but memory is already allocated -> free old buffer
            NutHeapFree(trace_items);
        }
        // allocate buffer
        trace_items = (t_traceitem *)NutHeapAlloc(size * sizeof(t_traceitem));
        if (trace_items == 0) {
            // failed
            return trace_size = 0;
        }
        else {
            trace_size = size;
        }
    }

//  if (mode == TRACE_MODE_OFF) {
//      // if terminal-cmd "trace size <val>" is called trace is started in
//      // default mode
//      mode = TRACE_MODE_DEFAULT;
//  }
    trace_mode   = mode;
    NutTraceClear();	
    return trace_size;		
}



void NutTraceClear()
{
    trace_head = trace_isfull = 0;
    TRACE_ADD_ITEM(TRACE_TAG_START,0);
}

void NutTraceStop()
{
    TRACE_ADD_ITEM(TRACE_TAG_STOP,0);
    trace_mode = TRACE_MODE_OFF;
}

void NutTraceTerminal(char* arg)
{
    int val;

    if (sscanf(arg,"print%d",&val)==1) {
        NutTracePrint(val);
        return;
    }
    if (!strncmp(arg,"print",5)) {
        NutTracePrint(0);
        return;
    }
    if (!strncmp(arg,"oneshot",7)) {
        NutTraceInit(trace_size,TRACE_MODE_ONESHOT);
        printf("TRACE mode %s, restarted\n",mode_string[(int)trace_mode]);
        return;
    }
    if (!strncmp(arg,"circular",8)) {
        NutTraceInit(trace_size,TRACE_MODE_CIRCULAR);
        printf("TRACE mode %s, restarted\n",mode_string[(int)trace_mode]);
        return;
    }
    if (sscanf(arg,"size%d",&val)==1) {
        printf("TRACE new size: %d\n",NutTraceInit(val,trace_mode));
        return;
    }
    if (!strncmp(arg,"stop",4)) {
        NutTraceStop();
        printf("TRACE stopped\n");
        return;
    }
    if (sscanf(arg,"mask%d",&val)==1) {
        if (val<=TRACE_TAG_LAST) {
            trace_mask[val]=trace_mask[val] ? 0 : 1;
            NutTraceClear();
        }
        NutTraceMaskPrint();
        return;
    }
    if (!strncmp(arg,"mask",4)) {
        NutTraceMaskPrint();
        return;
    }

	
    NutTraceStatusPrint();
    printf("SYNTAX: trace [print [<size>]|oneshot|circular|size <size>|stop|mask [<tag>]]\n");
}

void NutTraceStatusPrint(void)
{
    printf("TRACE STATUS\n");
    printf(" Mode is %s\n",mode_string[(int)trace_mode]);
    printf(" Size is %d\n",trace_size);
    if (trace_isfull)
        printf(" is full\n");
    else
        printf(" contains %d elements\n",trace_head);		
}

void NutTracePrint(int size)
{
    int i,index;
    uint32_t time;
    char mode;
    unsigned int micros, millis, secs;

    mode = trace_mode;
    trace_mode = TRACE_MODE_OFF;
    printf("\nTRACE");
    if (trace_size == 0) {
        printf(" not initialized!\n\n");
        return;
    }
    printf(" contains %d items, ",(trace_isfull ? trace_size : trace_head));
    if (size == 0) {
        size = trace_size;
    }
    if (trace_isfull) {
        if (size > trace_size) {
            size = trace_size;
        }
    }	
    else {
        if (size > trace_head) {
            size = trace_head;
        }
    }
    printf(" printing %d items.\n",size);
    printf("%-20s%-12s%-12s\n","TAG","PC/Info","Time [s:ms:us]");
    printf("-----------------------------------------------\n");
    for (i=size-1;i>=0;i--) {
        index = trace_head - i - 1;
        if (index<0) {
            index += trace_size;
        }
        time = ((uint32_t)trace_items[index].time_h)<<16 | trace_items[index].time_l;
        micros = (int)(time%1000);
        millis = (int)((time/1000)%1000);
        secs   = (int)(time/1000000);
        switch (trace_items[index].tag) {
            case TRACE_TAG_THREAD_YIELD:
            case TRACE_TAG_THREAD_SETPRIO:
            case TRACE_TAG_THREAD_WAIT:
            case TRACE_TAG_THREAD_SLEEP:
                printf("%-20s%-15s%7u:%03u:%03u\n",
                        tag_string[(int)trace_items[index].tag],
                        ((NUTTHREADINFO*)(trace_items[index].pc))->td_name,
                        secs,millis,micros);
                break;
            case TRACE_TAG_INTERRUPT_ENTER:
            case TRACE_TAG_INTERRUPT_EXIT:
                printf("%-20s%-15s%7u:%03u:%03u\n",
                        tag_string[(int)trace_items[index].tag],
                        int_string[(int)trace_items[index].pc],
                        secs,millis,micros);
                break;
            case TRACE_TAG_USER:
                printf("%-20s%-15s%7u:%03u:%03u\n",
                        tag_string[(int)trace_items[index].tag],
                        user_string[(int)trace_items[index].pc],
                        secs,millis,micros);
                break;
            default:
                printf("%-20s%-#15x%7u:%03u:%03u\n",
                        tag_string[(int)trace_items[index].tag],
                        trace_items[index].pc,
                        secs,millis,micros);
        }
    }
    trace_mode = mode;
}

int NutTraceGetPC(void)
{
    int pc = ((int)(*((char*)SP+1)));
    pc = (pc<<8)|(0x00ff&(int)(*((char*)SP+2)));
    return pc<<1;
} 


void NutTraceMaskSet(int tag)
{
    if (tag<=TRACE_TAG_LAST) {
        trace_mask[tag] = 1;
    }
}

void NutTraceMaskClear(int tag)
{
    if (tag<=TRACE_TAG_LAST) {
        trace_mask[tag] = 0;
    }
}

void NutTraceMaskPrint(void)
{
    int tag;
    printf("TRACEMASK\n");
    for (tag=0;tag<=TRACE_TAG_LAST;tag++) {
        printf(" %d %s ",tag,tag_string[tag]);
        if (trace_mask[tag])
            printf("ON\n");
        else
            printf("OFF\n");
    }					
}

int NutTraceRegisterUserTag(int tag, char* tag_string)
{
    if (tag >= TRACE_MAX_USER)
        return -1;

    user_string[tag] = tag_string;
    return tag;		
}
#endif
