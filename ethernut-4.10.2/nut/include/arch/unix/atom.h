/*
 * Copyright (C) 2001-2005 by egnite Software GmbH. All rights reserved.
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
 * Revision 1.3  2008/08/11 06:59:58  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.2  2006/01/26 15:34:49  going_nuts
 * adapted to new interrupt handling scheme for unix emulation
 * now uses Unix timer and runs without interrupts unless you emulate other hardware
 *
 * Revision 1.1  2005/06/06 10:49:35  haraldkipp
 * Building outside the source tree failed. All header files moved from
 * arch/cpu/include to include/arch/cpu.
 *
 * Revision 1.1  2005/05/27 17:41:52  drsung
 * Moved the file.
 *
 * Revision 1.1  2005/05/26 10:08:42  drsung
 * Moved the platform dependend code from include/sys/atom.h to this file.
 *
 *
 */

#ifndef _SYS_ATOM_H_
#error "Do not include this file directly. Use sys/atom.h instead!"
#endif

__BEGIN_DECLS
#include <pthread.h>
#include <signal.h>
#include <sys/thread.h>
#include <stdio.h>
#include <stdlib.h>

extern uint16_t main_cs_level;
extern sigset_t irq_signal;
extern pthread_cond_t irq_cv;
extern uint16_t int_disabled;

extern FILE *__os_trs;
extern uint8_t __os_trf;

#define AtomicInc(p)     (++(*p))
#define AtomicDec(p)     (--(*p))

extern void NutExitCritical(void);
extern void NutEnterCritical(void);


// uncommenting the following causes a segmentation fault because stdout isn't defined at startup.
// #define CRITSECT_TRACE

#ifndef CRITSECT_TRACE
#define NutEnterCritical()                                                  \
    pthread_sigmask(SIG_BLOCK, &irq_signal, 0);         \
    int_disabled = 1;                                   \
    if (runningThread) {                                \
        runningThread->td_cs_level++;                   \
    } else {                                            \
        main_cs_level++;                                \
    }                                                   \
    pthread_sigmask(SIG_UNBLOCK, &irq_signal, 0);


#define NutExitCritical()                                                           \
    pthread_sigmask(SIG_BLOCK, &irq_signal, 0);         \
    if (runningThread) {                                \
        if (--runningThread->td_cs_level == 0) {        \
            int_disabled = 0;                           \
            pthread_cond_signal(&irq_cv);               \
        }                                               \
    } else {                                            \
        if (--main_cs_level == 0) {                     \
            int_disabled = 0;                           \
            pthread_cond_signal(&irq_cv);               \
        }                                               \
    }                                                   \
    pthread_sigmask(SIG_UNBLOCK, &irq_signal, 0);

#else

#define NutEnterCritical()                                           \
    pthread_sigmask(SIG_BLOCK, &irq_signal, 0);         \
    int_disabled = 1;                                   \
    if (runningThread) {                                \
        if (runningThread->td_cs_level==0)                                              \
            printf("Entered a: %s.%d - %s\n", __FILE__, __LINE__, runningThread->td_name);  \
        runningThread->td_cs_level++;                   \
    } else {                                            \
        if (main_cs_level==0)                                               \
            printf("Entered b: %s.%d - %s\n", __FILE__, __LINE__, "ROOT");  \
        main_cs_level++;                                \
    }                                                   \
    pthread_sigmask(SIG_UNBLOCK, &irq_signal, 0);

#define NutExitCritical()                                                                       \
    pthread_sigmask(SIG_BLOCK, &irq_signal, 0);         \
    if (runningThread) {                                \
        if (--runningThread->td_cs_level == 0) {        \
            int_disabled = 0;                           \
            printf("Left a: %s.%d - %s\n", __FILE__, __LINE__, runningThread->td_name); \
            pthread_cond_signal(&irq_cv);               \
        }                                               \
    } else {                                            \
        if (--main_cs_level == 0) {                     \
            int_disabled = 0;                           \
            printf("Left a: %s.%d - %s\n", __FILE__, __LINE__, "ROOT"); \
            pthread_cond_signal(&irq_cv);               \
        }                                               \
    }                                                   \
    pthread_sigmask(SIG_UNBLOCK, &irq_signal, 0);
#endif

#define NutJumpOutCritical() NutExitCritical()

__END_DECLS
