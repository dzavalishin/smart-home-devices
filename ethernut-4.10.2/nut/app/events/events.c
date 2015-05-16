/*!
 * Copyright (C) 2001-2006 by egnite Software GmbH
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
 * $Log$
 * Revision 1.3  2008/01/31 09:38:15  haraldkipp
 * Added return statement in main to avoid warnings with latest GCC.
 *
 * Revision 1.2  2006/07/21 09:07:19  haraldkipp
 * Use dev/board.h to determine output device.
 *
 * Revision 1.1  2005/07/23 13:54:39  haraldkipp
 * First check in
 *
 */

/*!
 * \example events/events.c
 *
 * This sample demonstrates the usage of Nut/OS event queues. It further 
 * shows how an event queue can be used as a mutual exclusion semaphore.
 *
 * Two additional threads are started, one with a higher and another one 
 * with a lower priority than the main thread.
 *
 * The results are printed on the debug device. Each thread prints the 
 * current action in one of three columns. The first column is used by 
 * the highest priority, the last column by the lowest priority thread.
 */
#include <cfg/os.h>

#include <stdio.h>
#include <io.h>

#include <dev/board.h>

#include <sys/thread.h>
#include <sys/timer.h>
#include <sys/event.h>

/*
 * A global event queue, used as a mutex semaphore.
 */
static HANDLE mutex;

/*
 * High priority background thread. 
 */
THREAD(High, arg)
{
    NutThreadSetPriority(32);
    for(;;) {
        puts("Request");
        if (NutEventWait(&mutex, 2000)) {
            puts("Timeout");
        }
        else {
            puts("Acquired");
            NutSleep(2500);
            puts("Release");
            NutEventPost(&mutex);
        }
        NutSleep(1000);
    }
}

/*
 * Low priority background thread. 
 */
THREAD(Low, arg)
{
    NutThreadSetPriority(96);
    for(;;) {
        puts("                  Request");
        if (NutEventWait(&mutex, 3000)) {
            puts("                  Timeout");
        }
        else {
            puts("                  Acquired");
            NutSleep(3500);
            puts("                  Release");
            NutEventPost(&mutex);
        }
    }
}

/*
 * Main application routine. 
 */
int main(void)
{
    uint32_t baud = 115200;

    /*
     * Register the UART device, open it, assign stdout to it and set 
     * the baudrate.
     */
    NutRegisterDevice(&DEV_CONSOLE, 0, 0);
    freopen(DEV_CONSOLE_NAME, "w", stdout);
    _ioctl(_fileno(stdout), UART_SETSPEED, &baud);

    /*
     * Print title.
     */
    puts("\nNut/OS Event Queue Demo");
    puts("High     Main     Low      ");

    /*
     * Post an initial event. This will put the queue into signaled 
     * state and immediately grant the next call to NutEventWait().
     */
    NutEventPost(&mutex);

    /*
     * Start two background threads.
     */
    NutThreadCreate("high", High, 0, 256);
    NutThreadCreate("low", Low, 0, 256);

    for(;;) {
        puts("         Request");
        if (NutEventWait(&mutex, 1000)) {
            puts("         Timeout");
        }
        else {
            puts("         Acquired");
            NutSleep(1500);
            puts("         Release");
            NutEventPost(&mutex);
        }
        NutSleep(1000);
    }
    return 0;
}
