#ifndef _SYS_THREAD_H_
#define _SYS_THREAD_H_

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
 * \file sys/thread.h
 * \brief Thread management definitions.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.23  2009/03/05 22:16:57  freckle
 * use __NUT_EMULATION instead of __APPLE__, __linux__, or __CYGWIN__
 *
 * Revision 1.22  2009/02/06 15:37:40  haraldkipp
 * Added stack space multiplier and addend. Adjusted stack space.
 *
 * Revision 1.21  2009/01/19 18:55:12  haraldkipp
 * Added stack checking code.
 *
 * Revision 1.20  2009/01/17 11:26:51  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.19  2008/08/11 07:00:28  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.18  2008/06/15 17:05:52  haraldkipp
 * Rolled back to version 1.16.
 *
 * Revision 1.16  2007/06/14 07:24:38  freckle
 * Disable ADC and buskeeper during idle thread sleep, if IDLE_THREAD_ADC_OFF and IDLE_THREAD_BUSKEEPER_OFF are defined
 *
 * Revision 1.15  2006/09/29 12:24:14  haraldkipp
 * Stack allocation code moved from thread module to heap module.
 * All code should use dedicated stack allocation routines. For targets
 * allocating stack from the normal heap the API calls are remapped by
 * preprocessor macros.
 *
 * Revision 1.14  2006/06/29 03:52:19  hwmaier
 * Added include statement for cfg/memory.h so the macro
 * NUTMEM_STACKHEAP is found when compiling for the
 * AT90CAN128.
 *
 * Revision 1.13  2006/06/28 14:35:31  haraldkipp
 * Event/thread/timer re-design.
 * Code indention corrected.
 * Volatile attribute removed from THREADINFO link pointer.
 *
 * Revision 1.12  2006/03/16 15:25:34  haraldkipp
 * Changed human readable strings from u_char to char to stop GCC 4 from
 * nagging about signedness.
 *
 * Revision 1.11  2005/10/07 21:48:25  hwmaier
 * Using __AVR_ENHANCED__ macro instead of __AVR_ATmega128__ to support also AT90CAN128 MCU
 *
 * Revision 1.10  2005/10/04 05:25:17  hwmaier
 * Added support for separating stack and conventional heap as required by AT09CAN128 MCUs
 *
 * Revision 1.9  2005/08/02 17:46:49  haraldkipp
 * Major API documentation update.
 *
 * Revision 1.8  2005/07/26 15:49:59  haraldkipp
 * Cygwin support added.
 *
 * Revision 1.7  2005/02/17 14:42:37  phblum
 * Removed volatile declarations of runQueue since it is not touched from interrupt context. Same for nutThreadList, runningThread and killedThreads.
 *
 * Revision 1.6  2005/02/16 19:53:15  haraldkipp
 * Ready-to-run queue handling removed from interrupt context.
 *
 * Revision 1.5  2004/07/09 19:51:34  freckle
 * Added new function NutThreadSetSleepMode to tell nut/os to set the MCU
 * into sleep mode when idle (avr-gcc && avr128 only)
 *
 * Revision 1.4  2004/04/07 12:13:57  haraldkipp
 * Matthias Ringwald's *nix emulation added
 *
 * Revision 1.3  2004/03/16 16:48:44  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 * Revision 1.2  2003/12/15 19:28:26  haraldkipp
 * Kill function prototypes
 *
 * Revision 1.1.1.1  2003/05/09 14:41:22  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.14  2003/02/04 18:00:54  harald
 * Version 3 released
 *
 * Revision 1.13  2002/10/29 15:40:50  harald
 * *** empty log message ***
 *
 * Revision 1.12  2002/06/26 17:29:29  harald
 * First pre-release with 2.4 stack
 *
 * \endverbatim
 */

#include <compiler.h>
#include <sys/types.h>
#include <cfg/memory.h>

#include <stdint.h>

#define DEADBEEF    0xDEADBEEF


/*!
 * \addtogroup xgThread
 */
/*@{*/

/*!
 * Thread information structure type.
 */
typedef struct _NUTTHREADINFO NUTTHREADINFO;

/*!
 * \struct _NUTTHREADINFO thread.h sys/thread.h
 * \brief Thread information structure.
 *
 * \todo Sort items while considering alignment.
 */

struct _NUTTHREADINFO {
    NUTTHREADINFO *td_next;         /*!< \brief Linked list of all threads. */
    NUTTHREADINFO *td_qnxt;         /*!< \brief Linked list of all queued thread. */
    volatile unsigned int td_qpec;  /*!< \brief Pending event counter. */
    char td_name[9];                /*!< \brief Name of this thread. */
    uint8_t td_state;               /*!< \brief Operating state. One of TDS_ */
    uintptr_t td_sp;                /*!< \brief Stack pointer. */
    uint8_t td_priority;            /*!< \brief Priority level. 0 is highest priority. */
    uint8_t *td_memory;             /*!< \brief Pointer to heap memory used for stack. */
    HANDLE td_timer;                /*!< \brief Event timer. */
    volatile HANDLE td_queue;       /*!< \brief Root entry of the waiting queue. */
#ifdef __NUT_EMULATION__	
    pthread_t td_pthread;           /*!< \brief pthread for unix emulations. */
    void (*td_fn) (void *);         /*!< \brief thread function */
    void *td_arg;                   /*!< \brief args given to NutCreateThread */
    pthread_cond_t td_cv;           /*!< \brief conditional variable for unix emulations. */
    uint16_t td_cs_level;           /*! \brief number critical sections has been entered without leaving */
#endif
};
/*@}*/

/*!
 * \name Thread States
 */
/*@{*/
#define TDS_TERM        0       /*!< Thread has exited. */
#define TDS_RUNNING     1       /*!< Thread is running. */
#define TDS_READY       2       /*!< Thread is ready to run. */
#define TDS_SLEEP       3       /*!< Thread is sleeping. */
/*@}*/

#define SLEEP_MODE_NONE 0xff

/*!
 * \brief Stack size factor.
 *
 * Configured stack sizes are multiplied with this value.
 *
 * All stack size settings of internal Nut/OS threads had been calculated 
 * for size optimized code. Probably more stack space is required with 
 * other compiler settings.
 *
 * For example, when GCC generates non-optimized code for source code
 * debugging, a factor of 3 should be applied to all stack sizes.
 *
 * Application code may also make use of this macro.
 *
 * \code
 * #include <sys/thread.h>
 *
 * #define MY_THREAD_STACK  ((384 * NUT_THREAD_STACK_MULT) + NUT_THREAD_STACK_ADD)
 *
 * NutThreadCreate("myth", ThreadFunc, 0, MY_THREAD_STACK);
 * \endcode
 *
 * See also \ref NUT_THREAD_STACK_ADD.
 */
#ifndef NUT_THREAD_STACK_MULT
#define NUT_THREAD_STACK_MULT   1
#endif

/*!
 * \brief Stack size summand.
 *
 * The specified value will be added to all configured stack sizes.
 *
 * See \ref NUT_THREAD_STACK_MULT.
 */
#ifndef NUT_THREAD_STACK_ADD
#define NUT_THREAD_STACK_ADD    0
#endif

extern NUTTHREADINFO *runningThread;
extern NUTTHREADINFO *nutThreadList;
extern NUTTHREADINFO *runQueue;

__BEGIN_DECLS
/* Function prototypes. */
#ifndef __NUT_EMULATION__	
extern void NutThreadInit(void);
#endif

#if defined(__GNUC__) && defined (__AVR_ENHANCED__)
extern uint8_t NutThreadSetSleepMode(uint8_t mode);
#endif

  
extern HANDLE NutThreadCreate(char *name, void (*fn) (void *), void *arg, size_t stackSize);
extern uint8_t NutThreadSetPriority(uint8_t level);

extern void NutThreadKill(void);
extern void NutThreadDestroy(void);
extern void NutThreadExit(void);

extern void NutThreadResume(void);
extern void NutThreadWake(HANDLE timer, HANDLE th);
extern void NutThreadYield(void);

extern void NutThreadAddPriQueue(NUTTHREADINFO * td, NUTTHREADINFO * volatile *tqpp);
extern void NutThreadRemoveQueue(NUTTHREADINFO * td, NUTTHREADINFO * volatile *tqpp);

extern void NutThreadSwitch(void);
extern HANDLE GetThreadByName(char *name);

extern NUTTHREADINFO *NutThreadStackCheck(size_t minsiz);
extern size_t NutThreadStackAvailable(char *name);

/*!
 * \brief Macro for thread entry definitions.
 */
#define THREAD(threadfn, arg) \
void threadfn(void *arg) __attribute__ ((noreturn)); \
void threadfn(void *arg)

__END_DECLS
/* */
#endif
