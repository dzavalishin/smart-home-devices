/*!
 * Copyright (C) 2001-2004 by egnite Software GmbH. All rights reserved.
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
 * $Id: threads.c 3538 2011-08-10 16:34:10Z haraldkipp $
 */

/*!
 * \example threads/threads.c
 *
 * This sample demonstrates Nut/OS multithreading.
 *
 * Each thread is started with 192 bytes of stack. This is very
 * low and doesn't provide much space for local variables.
 */

#include <stdio.h>
#include <io.h>

#include <cfg/arch.h>
#include <dev/board.h>

#include <sys/thread.h>
#include <sys/timer.h>

/*
 * High priority thread.
 */
THREAD(Thread1, arg)
{
    /*
     * Endless loop in high priority thread.
     */
    NutThreadSetPriority(16);
    for (;;) {
        putchar('H');
        NutSleep(125);
    }
}

/*
 * Low priority thread.
 */
THREAD(Thread2, arg)
{
    /*
     * Endless loop in low priority thread.
     */
    NutThreadSetPriority(128);
    for (;;) {
        putchar('L');
        NutSleep(125);
    }
}

/*
 * Main application thread. 
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

    puts("\nThread Test");

    /*
     * Start two additional threads. All threads are started with 
     * priority 64.
     */
    NutThreadCreate("t1", Thread1, 0, 512);
    NutThreadCreate("t2", Thread2, 0, 512);

    /*
     * Endless loop in main thread.
     */
    for (;;) {
        putchar('M');
        NutSleep(125);
    }
    return 0;
}
