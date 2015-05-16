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
 *  OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
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
 * unix_nutinit.c - init for unix emulation
 *
 * 2004.04.01 Matthias Ringwald <matthias.ringwald@inf.ethz.ch>
 *
 */

#ifdef __CYGWIN__
#include <sys/features.h>
#endif

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <getopt.h>
#include <termios.h>
#include <cfg/os.h>

#include <sys/types.h>
#include <sys/event.h>
#include <sys/device.h>
#include <sys/osdebug.h>
#include <sys/atom.h>
#include <dev/irqreg.h>
#include <dev/unix_devs.h>

extern void NutAppMain(void *arg) __attribute__ ((noreturn));

#ifndef NUT_THREAD_MAINSTACK
#define NUT_THREAD_MAINSTACK    1024
#endif

#ifndef NUT_THREAD_IDLESTACK
#define NUT_THREAD_IDLESTACK    1024
#endif

/* number of interrupts that can be outstanding before one is lost */
#define MAX_IRQ_SLOTS 3

/* type of raised interrupt (timer, usart, ...) */
int interrupts_pending[MAX_IRQ_SLOTS];

/* our IRQ signal */
sigset_t irq_signal;

/* our emulated interrupt enabled/disabled flag */
uint16_t int_disabled;

/* index to first, next free, and first unsignalled interrupt type */
int irq_current = 0;
int irq_slot = 0;
int irq_sent = 0;

/* interrupt thread, signalling Nut threads */
static pthread_t interrupt_thread;

/* mutex and condition variable used to signal that a new interrupt is pending */
pthread_mutex_t pending_mutex;
pthread_cond_t pending_cv;

/* mutex and condition variable used to signal that interrupts have been re-enabled */
pthread_mutex_t irq_mutex;
pthread_cond_t irq_cv;

/* interrupt handler routines */
IRQ_HANDLER irq_handlers[IRQ_MAX];

/* event queues to signal from non-Nut thread */
HANDLE *irq_eventqueues[IRQ_MAX];


/*!
 * \brief Register an interrupt handler.
 *
 * This function is typically called by device drivers.
 *
 * \param irq     Interrupt number to be associated with this handler.
 * \param handler This routine will be called by Nut/OS, when the
 *                specified interrupt occurs.
 * \param arg     Argument to be passed to the interrupt handler.
 *
 * \return 0 on success, -1 otherwise.
 */
int NutRegisterIrqHandler(uint8_t irq, void (*handler) (void *), void *arg)
{
    if (irq >= IRQ_MAX)
        return -1;

    NutEnterCritical();

    irq_handlers[irq].ir_arg = arg;
    irq_handlers[irq].ir_handler = handler;

    NutExitCritical();

    return 0;
}


/*!
 * \brief Register NutEventPostAsync for next NutThreadYield
 * 
 * Store responsible IRQ and queue to signal in list
 *
 * \param irq responsible IRQ
 * \param queue to signal
 *
 * this is added to allow an non-nut thread to post events without
 * introducing a race-condition
 *
 */
void NutUnixIrqEventPostAsync(uint8_t irq, HANDLE * queue)
{
    if (irq < IRQ_MAX)
        irq_eventqueues[irq] = queue;
}

/*!
 * \brief Handle interrupt triggered NutEventPostAsync
 * Check list of events to post and call NutEventPostAsync on them
 *
 * this is added to allow an non-nut thread to post events without
 * introducing a race-condition
 *
 */
void NutUnixThreadYieldHook(void);
void NutUnixThreadYieldHook()
{
    uint8_t irq;
    for (irq = 0; irq < IRQ_MAX; irq++) {
        if (irq_eventqueues[irq] != 0) {
            // printf("NutUnixThreadYield posting event nr %d\n\r", irq);
            NutEventPostFromIrq(irq_eventqueues[irq]);
            irq_eventqueues[irq] = 0;
        }
    }
}


/*
 * Handles SIGINT
 *
 */
static void NutUnixControlC(int);
static void NutUnixControlC(int signal)
{
    printf("CTRL-C! Abort application.\n\r");
    tcsetattr(fileno(stdout), TCSANOW, &emulation_options.saved_termios);
    exit(0);
}

/*
 * Signal handler for SIGUSR1
 * emulates interrupt hardware
 * serializes all interrupts and calls their corresponding handlers
 * 
 * all IRQs are multiplexed through the same signal handler (using only SIGUSR1)
 * a global array is used to keep track of the interrupts that have been raised
 * further signals are block until interrupt handling has finished
 * thus, it may happen that an interrupt signal is silently ignored and never arrives here
 * the corresponding irq would never happen.
 * to avoid this, all interrupts marked in the interrupts_pending table are handled upon before
 * control is relinquished.
 * thus, an interrupt may be handled upon before it corresponding signal is received
 * and its "real" signal is processed - too early, so to say.
 * it doesn't matter, even if the signal is still received as the interrupts_pending table
 * will be empty.
 * 
 */

static void NutUnixInterruptScheduler(int);
static void NutUnixInterruptScheduler(int signal)
{
    int irq;

    // disable interrupts for interrupt processing
    pthread_sigmask(SIG_BLOCK, &irq_signal, 0);

    // call interrupt handler
    if (irq_current != irq_slot) {
        irq = interrupts_pending[irq_current];
        if (++irq_current >= MAX_IRQ_SLOTS)
            irq_current = 0;
        if (irq < IRQ_MAX) {
            if (irq_handlers[irq].ir_handler) {
                irq_handlers[irq].ir_handler(irq_handlers[irq].ir_arg);
            }
        }
    }
    // re-enable interrupts
    pthread_sigmask(SIG_UNBLOCK, &irq_signal, 0);
}


/*!
 * \brief Emulate a Nut hardware interrupt on Unix
 * 
 * Add new interrupt to list of pending interrupts.
 * 
 * \param irq IRQ raised
 *
 * This is a support function which just maintains the table
 * listing raised interrupts. It is called by the non-Nut thread
 * emulating the hardware (e.g. the timer).
 * 
 * Sending the interrupt to the Nut threads is done by
 * NutInterruptEmulation().
 */
extern uint32_t nut_ticks;
void NutUnixRaiseInterrupt(int);
void NutUnixRaiseInterrupt(int irq)
{
    int r;


    // is there a slot available in our list of pending interrupts?
    // if so, let signal handler know the type of interrupt
    if ((irq_current == 0 && irq_slot != MAX_IRQ_SLOTS - 1) || (irq_current != 0 && irq_slot != irq_current - 1)) {
        // make sure we're the only one manipulating the IRQ table
        pthread_mutex_lock(&pending_mutex);

        interrupts_pending[irq_slot] = irq;
        if (++irq_slot >= MAX_IRQ_SLOTS) {
            irq_slot = 0;
        }
#if 0
        if( (nut_ticks % 1000) == 0 )
            printf( "%u\n", nut_ticks );
#endif
        pthread_mutex_unlock(&pending_mutex);

        // signal interrupt thread to interrupt Nut threads
        r = pthread_cond_signal(&pending_cv);
    }
}


/*!
 * \brief Send emulated interrupt signal to Nut threads
 * 
 * Hardware devices, such as timer, usart, etc., raise interrupts
 * to get serviced by NutOS. This function does the same for
 * devices on Unix emulation.
 *
 * \param irq IRQ raised
 *
 * The Nut thread is interrupted using the SIGUSR1 signal.
 * The corresponding signal handler dispatches to the respective
 * interrupt handler. All IRQs are multiplexed through the same signal handler.
 * 
 * Signalling the interrupt is done in a separate thread so that
 * the "calling/interrupting" thread can go back to emulate the hardware.
 * Otherwise, in case of disabled interrupts, it would be hanging and waiting
 * for interrupts to be re-enabled.
 * 
 * This thread here does nothing but signal interrupts and can safely hang
 * it they are disabled (by NutEnterCritical, for example).
 *
 */
void *NutInterruptEmulation(void *unused_arg)
{
    // non-nut thread => not interested in SIGUSR1 (IRQ signals)
    pthread_sigmask(SIG_BLOCK, &irq_signal, 0);

    for (;;) {
        pthread_mutex_lock(&pending_mutex);
        while (irq_slot == irq_sent) {
            // instead of busy waiting, let interrupting thread wake us
            pthread_cond_wait(&pending_cv, &pending_mutex);
        }
        pthread_mutex_unlock(&pending_mutex);

        // interrupt pending ?
        if (irq_slot != irq_sent) {
            pthread_mutex_lock(&irq_mutex);
            // instead of busy waiting, let Nut thread wake us once interrupts have been re-enabled
            while (int_disabled == 1) {
                pthread_cond_wait(&irq_cv, &irq_mutex);
            }
            // signal NUT thread, same effect as hardware interrupt
            kill(-getpgrp(), SIGUSR1);
            irq_sent = irq_slot;
            pthread_mutex_unlock(&irq_mutex);
        }
    }
	return NULL;
}

/*
 * Init IRQ handling
 *
 */

static void NutIRQInit(void);
static void NutIRQInit()
{
    int irq;

    /* enable interrupts */
    int_disabled = 0;

    // initialize interrupt type table
    irq_current = 0;
    irq_slot = 0;
    irq_sent = 0;

    // clear async event postings
    for (irq = 0; irq < IRQ_MAX; irq++)
        irq_eventqueues[irq] = 0;

    // define our IRQ signal
    sigemptyset(&irq_signal);
    sigaddset(&irq_signal, SIGUSR1);

    // the signal/IRQ handler
    signal(SIGUSR1, NutUnixInterruptScheduler);
    signal(SIGINT, NutUnixControlC);    // catch SIGINT (abort) to restore terminal

    // synchronization tools
    pthread_mutex_init(&irq_mutex, NULL);       // enable/disable interrupts
    irq = pthread_cond_init(&irq_cv, NULL);

    irq = pthread_mutex_init(&pending_mutex, NULL);     // maintenance of internal pending interrupts table
    irq = pthread_cond_init(&pending_cv, NULL);

    // start interrupt emulation thread
    pthread_create(&interrupt_thread, NULL, NutInterruptEmulation, (void *) (void *) NULL);
}

/*!
 * \addtogroup xgNutArchUnixInit
 */
/*@{*/

/*!
 * \brief Emulated idle thread. 
 *
 * After initializing the timers, the idle thread switches to priority 254
 * and enters an endless loop.
 */
THREAD(NutIdle, arg)
{
    /* Initialize system timers. */
    NutTimerInit();

    /* Create the main application thread. */
    NutThreadCreate("main", NutAppMain, 0, NUT_THREAD_MAINSTACK);

    // printf("main task created, idling now..\n");
    /*
     * Run in an idle loop at the lowest priority. We can still
     * do something useful here, like killing terminated threads
     * or putting the CPU into sleep mode.
     */
    NutThreadSetPriority(254);
    for (;;) {
        NutThreadYield();
        NutThreadDestroy();

        // sleep(); ... sleeping would be fine.
    }
}

/*!
 * \brief Nut/OS Initialization.
 *
 * Initializes the memory management and the thread system and starts 
 * an idle thread, which in turn initializes the timer management. 
 * Finally the application's main() function is called.
 */
#undef main

#define PSEUDO_RAM_SIZE 999999
uint8_t PSEUDO_RAM[PSEUDO_RAM_SIZE];

extern void NutThreadInit(void);

extern NUTFILE *NUT_freopen(CONST char *name, CONST char *mode, NUTFILE * stream);
extern NUTFILE *__iob[];

int main(int argc, char *argv[])
{
    tcgetattr(fileno(stdout), &emulation_options.saved_termios);

    /* get command line options */
    emulation_options_parse(argc, argv);

    /*
     * Register our Pseudo RAM
     */
    NutHeapAdd(PSEUDO_RAM, PSEUDO_RAM_SIZE);

    /* Read OS configuration from non-volatile memory. */
    NutLoadConfig();

    /*
     * set stdio
     */

    /*
       NutRegisterDevice(&devUart0, 0, 0);
       NUT_freopen("uart0", "w", __iob[1]);
       printf("OS Debug Mode, stdout opened in unix_nutinit.c\n");
       // NutTraceOs( stdout, 1);
     */

    /*
     * Init interrupt handling
     */
    NutIRQInit();

    /*
     * Init threading
     */
    NutThreadInit();

    /*
     * Create idle thread
     */
    NutThreadCreate("idle", NutIdle, 0, NUT_THREAD_IDLESTACK);

    return 0;
}

/*@}*/
