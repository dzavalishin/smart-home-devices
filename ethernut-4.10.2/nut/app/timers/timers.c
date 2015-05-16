/*!
 * Copyright (C) 2001-2005 by egnite Software GmbH
 *
 * All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * For additional information see http://www.ethernut.de/
 */

/*!
 * $Id: timers.c 3538 2011-08-10 16:34:10Z haraldkipp $
 */

/*!
 * \example timers/timers.c
 *
 * This sample demonstrates the usage of Nut/OS timer functions.
 *
 * In addition it demonstrates how to enable system debug output.
 * You need the debug version of the Nut/OS libraries. To create
 * them, you must enable the RTOS Kernel - OS Debug option in
 * the Configurator. Then use the Build Menu in the Configurator
 * to generate the build tree again and build Nut/OS.
 *
 * Note, that the debug version consumes much more memory than the
 * original version. If in doubt, check the map file.
 */
#include <cfg/os.h>
#ifdef NUTDEBUG
#include <sys/osdebug.h>
#endif

#include <stdio.h>
#include <io.h>

#include <cfg/arch.h>
#include <dev/board.h>

#include <sys/thread.h>
#include <sys/timer.h>
#include <sys/event.h>
#include <sys/heap.h>

/*
 * Timer callback routine.
 *
 * This function is called by the system timer thread. It is executed 
 * at a very high priority and must return as soon as possible and must 
 * not call any potentially blocking function.
 *
 * To keep this example as simple as possible, we break the above rule 
 * and call print functions. However, this is not really a problem, 
 * because the UART output queue won't overflow on our few characters 
 * and return immediately after starting transmit interrupts, which are
 * running in the background.
 */

void TimerCallback(HANDLE timer, void *arg)
{
    NutEventPostAsync(arg);
}

THREAD(TimerEvent1, arg)
{
    printf(" I1");
    NutThreadSetPriority(4);
    for (;;) {
        if (NutEventWait(arg, 12500))
            printf(" T1");
        else
            printf(" E1");
    }
}

THREAD(TimerEvent2, arg)
{
    printf(" I2");
    NutThreadSetPriority(8);
    for (;;) {
        if (NutEventWait(arg, 12500))
            printf(" T2");
        else
            printf(" E2");
    }
}

THREAD(TimerEvent3, arg)
{
    printf(" I3");
    NutThreadSetPriority(16);
    for (;;) {
        if (NutEventWait(arg, 12500))
            printf(" T3");
        else
            printf(" E3");
    }
}

THREAD(TimerEvent4, arg)
{
    printf(" I4");
    NutThreadSetPriority(32);
    for (;;) {
        if (NutEventWait(arg, 12500))
            printf(" T4");
        else
            printf(" E4");
    }
}

THREAD(Sleeper1, arg)
{
    NutThreadSetPriority(128);
    for (;;) {
        if (NutHeapAvailable() > 500)
            printf("\n%u free ", (unsigned int)NutHeapAvailable());
        else
            puts("Memory low");
        NutSleep(500);
    }
}

THREAD(Sleeper2, arg)
{
    NutThreadSetPriority(129);
    for (;;) {
        NutSleep(500);
        printf(" S2");
    }
}

THREAD(Sleeper3, arg)
{
    NutThreadSetPriority(130);
    for (;;) {
        NutSleep(500);
        printf(" S3");
    }
}

THREAD(Sleeper4, arg)
{
    NutThreadSetPriority(131);
    for (;;) {
        NutSleep(500);
        printf(" S4");
    }
}

/*
 * Main application routine. 
 *
 */
int main(void)
{
    int seq;
    uint32_t baud = 115200;
    uint32_t sleep_ms = 2000;
    uint32_t timer_ms = 125;
    uint32_t cpu_crystal;
    int one_shot;
    HANDLE timer1, timer2, timer3, timer4;
    HANDLE event1 = 0, event2 = 0, event3 = 0, event4 = 0;

    /*
     * Register the UART device, open it, assign stdout to it and set 
     * the baudrate.
     */
    NutRegisterDevice(&DEV_CONSOLE, 0, 0);
    freopen(DEV_CONSOLE_NAME, "w", stdout);
    _ioctl(_fileno(stdout), UART_SETSPEED, &baud);

#ifdef NUTDEBUG
    NutTraceHeap(stdout, 1);
    NutTraceOs(stdout, 1);
#endif

    NutThreadSetPriority(8);

    /*
     * The timer functions automatically determine the
     * CPU speed during system initialization. Query that
     * value and print it on the console.
     */
    cpu_crystal = NutGetCpuClock();
    puts("\n*******************************************************************************");
    printf("Timer sample running on %u.%04u MHz CPU\n",
           (int) (cpu_crystal / 1000000UL), (int) ((cpu_crystal - (cpu_crystal / 1000000UL) * 1000000UL) / 100)
        );

    NutThreadCreate("tmr1", TimerEvent1, &event1, 512);
    NutThreadCreate("tmr2", TimerEvent2, &event2, 512);
    NutThreadCreate("tmr3", TimerEvent3, &event3, 512);
    NutThreadCreate("tmr4", TimerEvent4, &event4, 512);

    NutThreadCreate("slpr1", Sleeper1, 0, 512);
    NutThreadCreate("slpr2", Sleeper2, 0, 512);
    NutThreadCreate("slpr3", Sleeper3, 0, 512);
    NutThreadCreate("slpr4", Sleeper4, 0, 512);

    /*
     * Endless application loop.
     */
    for (seq = 0;; seq++) {

        /*
         * Predefine the one-shot option flag for the
         * timer started below. Each odd sequence starts
         * a one-shot timer, each even sequence a
         * priodical one.
         */
        if (seq & 1)
            one_shot = TM_ONESHOT;
        else
            one_shot = 0;

        /*
         * Start a timer with 1 second timer intervals.
         * This timer will call TimerCallback exactly one
         * time, if it's a one-shot timer or periodically,
         * if not a one-shot timer.
         *
         * We pass a pointer to the sequence counter,
         * which in turn is passed to the callback
         * function.
         */
        //if((timer_ms += 125) > 500)
        //    timer_ms = 0;
        printf("\nStart %s t1 ", one_shot ? "oneshot" : "periodic");
        timer1 = NutTimerStart(timer_ms, TimerCallback, &event1, one_shot);

        printf("\nStart %s t2 ", one_shot ? "oneshot" : "periodic");
        timer2 = NutTimerStart(timer_ms, TimerCallback, &event2, one_shot);

        printf("\nStart %s t3 ", one_shot ? "oneshot" : "periodic");
        timer3 = NutTimerStart(timer_ms, TimerCallback, &event3, one_shot);

        printf("\nStart %s t4 ", one_shot ? "oneshot" : "periodic");
        timer4 = NutTimerStart(timer_ms, TimerCallback, &event4, one_shot);

        /*
         * Sleep for a number of seconds.
         */
        if ((sleep_ms += 1000) > 30000)
            sleep_ms = 1000;
        printf("\nSleeping %u seconds ", (int) (sleep_ms / 1000UL));
        NutSleep(sleep_ms);

        /*
         * Stop periodical timer. One-shot timers
         * are automatically stopped by Nut/OS.
         */
        if (one_shot == 0) {
            printf("\nStop timers ");
            NutTimerStop(timer1);
            NutTimerStop(timer2);
            NutTimerStop(timer3);
            NutTimerStop(timer4);
        }
        //printf("\nSleeping %u seconds ", (int)(sleep_ms / 1000UL));
        //NutSleep(sleep_ms);
        printf("\n%u bytes free\n", (unsigned int)NutHeapAvailable());
    }
    return 0;
}
