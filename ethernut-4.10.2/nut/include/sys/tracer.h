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
 * sys/tracer.h
 *
 * 22.12.2004 Philipp Blum <blum@tik.ee.ethz.ch>
 */
#ifndef _SYS_TRACER_H_
#define _SYS_TRACER_H_

/**

\file sys/tracer.h

\author Philipp Blum <blum@tik.ee.ethz.ch>

\date 22.12.2004

\brief Trace functions


*/

#include <sys/types.h>
#include <stdint.h>
#include <sys/atom.h>

/******************************************************************
 * defines 
 ******************************************************************/
#define TRACE_MODE_FIRST            0
#define TRACE_MODE_OFF              0
#define TRACE_MODE_CIRCULAR         1 
#define TRACE_MODE_ONESHOT          2
#define TRACE_MODE_LAST             2
#define TRACE_MODE_DEFAULT          TRACE_MODE_CIRCULAR

#define TRACE_SIZE_DEFAULT        500

/* Event types */
#define TRACE_TAG_FIRST             0
#define TRACE_TAG_CRITICAL_FIRST    0
#define TRACE_TAG_CRITICAL_ENTER    0
#define TRACE_TAG_CRITICAL_EXIT     1
#define TRACE_TAG_CRITICAL_LAST     1
#define TRACE_TAG_THREAD_FIRST      2
#define TRACE_TAG_THREAD_YIELD      2
#define TRACE_TAG_THREAD_SETPRIO    3
#define TRACE_TAG_THREAD_WAIT       4
#define TRACE_TAG_THREAD_SLEEP      5
#define TRACE_TAG_THREAD_LAST       5
#define TRACE_TAG_INTERRUPT_FIRST   6
#define TRACE_TAG_INTERRUPT_ENTER   6
#define TRACE_TAG_INTERRUPT_EXIT    7
#define TRACE_TAG_INTERRUPT_LAST    7
#define TRACE_TAG_START             8
#define TRACE_TAG_STOP              9
#define TRACE_TAG_USER             10
#define TRACE_TAG_LAST             10

#define TRACE_MAX_USER             10

#define TRACE_INT_FIRST             0
#define TRACE_INT_UART0_CTS         0
#define TRACE_INT_UART0_RXCOMPL     1
#define TRACE_INT_UART0_TXEMPTY     2
#define TRACE_INT_UART1_CTS         3
#define TRACE_INT_UART1_RXCOMPL     4
#define TRACE_INT_UART1_TXEMPTY     5
#define TRACE_INT_TIMER0_OVERFL     6
#define TRACE_INT_TIMER1_OVERFL     7
#define TRACE_INT_SUART_TIMER       8
#define TRACE_INT_SUART_RX          9
#define TRACE_INT_LAST              9

/******************************************************************
 * typedefs
 ******************************************************************/
/*! \brief Item in the trace buffer
*/
typedef struct _t_traceitem {
    /*! \brief Type of event
    */
    uint8_t tag;
    /*! \brief Additional information, depending on the type of the event
    */
    unsigned int  pc;
    /*! \brief Upper 16 bit of microseconds clock when event occured
    */
    unsigned int  time_h;
    /*! \brief Lower 16 bit of microseconds clock when event occured
    */
    unsigned int  time_l;
} t_traceitem;


/******************************************************************
 * global variables 
 ******************************************************************/
/*! \brief Upper 16 bits of microseconds clock, incremented on timer 1 overflow interrupts
*/
extern unsigned int micros_high;
/*! \brief Trace buffer, initialized by NutTraceInit
*/
extern t_traceitem *trace_items;
/*! \brief Pointer to the current item in the trace buffer
*/
extern t_traceitem *trace_current;
/*! \brief Current index in the trace buffer
*/
extern int trace_head;
/*! \brief Size of the trace buffer
*/
extern int trace_size;
/*! \brief Flag indicating whether all items in the trace buffer contain valid information
*/
extern char trace_isfull;
/*! \brief Current state of the tracing facility
*/
extern char trace_mode;
/*! \brief Mask to individually disable tracing of specific event types
*/
extern char trace_mask[TRACE_TAG_LAST+1];

/******************************************************************
 * function prototypes API
 ******************************************************************/
/*******************************************************************************
 * NutTraceInit
 ******************************************************************************/
/**
 * Initializes the trace buffer and activates tracing. 
 * Starts timer 1 for microsecond clock, enables interrupt on overflow
 * 
 *
 * @param size Number of items in the trace buffer
 * @param mode Mode of operation
 * - #TRACE_MODE_CIRCULAR Trace buffer wraps around when full
 * - #TRACE_MODE_ONESHOT  Traceing is stopped when buffer is full
 *
 * @return int Status
 */
extern int  NutTraceInit(int size, char mode); 
/*******************************************************************************
 * NutTraceStop
 ******************************************************************************/
/**
 * Sets trace_mode to #TRACE_MODE_OFF and thus stop tracing
 */
extern void NutTraceStop(void);
/*******************************************************************************
 * NutTracePrint
 ******************************************************************************/
/**
 * Prints the current contents of the trace buffer
 * 
 * @param size can be used to limit the number of printed items, 
 * if size==0, then all items of the buffer are printed
 */
extern void NutTracePrint(int size);
/*******************************************************************************
 * NutTraceTerminal
 ******************************************************************************/
/**
 * Commands to manipulate the tracing facility
 * 
 * @param arg String containing the commands
 * - print &lt;size&gt; calls NutPrintTrace
 * - oneshot restarts tracing in the oneshot mode
 * - circular restarts tracing in the ciruclar mode
 * - size &lt;size&gt; restarts tracing in the current mode, using a buffer of length &lt;size&gt;
 * - stop stops tracing
 */
extern void NutTraceTerminal(char* arg);
/*******************************************************************************
 * NutTraceGetPC
 ******************************************************************************/
/**
 * Returns the program counter (PC) of the instruction following NutGetPC
 * The .map file of the application can be used to find the actual C Code
 *
 * WARNING: Most likely only works on AVR. Works by inspecting the stack, thus the function
 * breaks when local variables are introduced in NutGetPC
 * 
 * @return int Program counter
 */
extern int  NutTraceGetPC(void);
/*******************************************************************************
 * NutTraceClear
 ******************************************************************************/
/**
 * Clears the trace buffer, but leaves trace_mode unaltered.
 */
extern void NutTraceClear(void);
/*******************************************************************************
 * NutTraceMaskPrint
 ******************************************************************************/
/**
 * Prints the current state of trace_mask, which for every type of event (TRACE_TAG_XXX)
 * determines, whether they are inserted in the trace buffer or not. 
 */
extern void NutTraceMaskPrint(void);
/*******************************************************************************
 * NutTraceMaskClear
 ******************************************************************************/
/**
 * Disables tracing of a particular event type
 * 
 * @param tag of event to disable
 */
extern void NutTraceMaskClear(int tag);
/*******************************************************************************
 * NutTraceMaskSet
 ******************************************************************************/
/**
 * Enables tracing of a particular event type
 * 
 * @param tag of event to enable
 */
extern void NutTraceMaskSet(int tag);
/*******************************************************************************
 * NutTraceStatusPrint
 ******************************************************************************/
/**
 * Prints current status of tracing facility
 */
extern void NutTraceStatusPrint(void);
/*******************************************************************************
 * NutTraceRegisterUserTag
 ******************************************************************************/
/**
 * Registers a user event type
 * 
 * @param tag of the new event type (e.g. #define TRACE_USER_SEND 0)
 * @param tag_string name of the event type used when printing the trace buffer
 */
extern int NutTraceRegisterUserTag(int tag, char* tag_string);
/**
 * Macro to insert an event in the trace buffer
 * 
 * @param TAG Type of event
 * @param PC  Additional information, depending on the type of event
 */
#define TRACE_ADD_ITEM(TAG,PC)                      \
    if ((trace_mode != TRACE_MODE_OFF) &&           \
        (trace_mask[TAG] == 1))                     \
    {                                               \
        asm volatile(                               \
            "in  __tmp_reg__, __SREG__" "\n\t"      \
            "push __tmp_reg__"  "\n\t"              \
            "cli"                       "\n\t"      \
        );                                          \
        trace_current = &trace_items[trace_head++]; \
        trace_current->tag = TAG;                   \
        trace_current->pc = PC;                     \
        trace_current->time_h = micros_high;        \
        trace_current->time_l = TCNT1;              \
        if (trace_head >= trace_size) {             \
            trace_isfull = 1;                       \
            trace_head = 0;                     	\
            if (trace_mode == TRACE_MODE_ONESHOT)   \
                trace_mode = TRACE_MODE_OFF;        \
        }                                           \
        asm volatile(                               \
            "pop __tmp_reg__"           "\n\t"      \
            "out __SREG__, __tmp_reg__" "\n\t"      \
        );                                          \
    }
/**
 * Macro to insert an event in the trace buffer, 
 * filling the additional information field with the program counter (PC)
 * 
 * @param TAG Type of event
 */
#define TRACE_ADD_ITEM_PC(TAG) TRACE_ADD_ITEM(TAG,NutTraceGetPC())

#endif

