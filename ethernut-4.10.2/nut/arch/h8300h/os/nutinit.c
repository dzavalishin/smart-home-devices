/*
 * Copyright (C) 2001-2003 by egnite Software GmbH. All rights reserved.
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
 * Revision 1.2  2005/08/02 17:46:46  haraldkipp
 * Major API documentation update.
 *
 * Revision 1.1  2005/05/27 17:18:41  drsung
 * Moved the file.
 *
 * Revision 1.2  2004/11/08 18:58:59  haraldkipp
 * Configurable stack sizes
 *
 * Revision 1.1  2004/03/16 16:48:46  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 */

#include <cfg/os.h>

extern void NutAppMain(void *arg) __attribute__ ((noreturn));

/*!
 * \addtogroup xgNutArchH8300hInit
 */
/*@{*/

/* \fn NutIdle(void *arg)
 * \brief Idle thread. 
 *
 * After initializing the timers, the idle thread switches to priority 254
 * and enters an endless loop.
 */
THREAD(NutIdle, arg)
{
    /* Initialize system timers. */
    NutTimerInit();

    /* Create the main application thread. */
    NutThreadCreate("main", main, 0, NUT_THREAD_MAINSTACK);

    /*
     * Run in an idle loop at the lowest priority. We can still
     * do something useful here, like killing terminated threads
     * or putting the CPU into sleep mode.
     */
    NutThreadSetPriority(254);
    for (;;) {
        NutThreadYield();
        NutThreadDestroy();
    }
}

/*!
 * \brief Nut/OS Initialization.
 *
 * Initializes the memory management and the thread system and starts 
 * an idle thread, which in turn initializes the timer management. 
 * Finally the application's main() function is called.
 */
void NutInit(void)
{
    NutHeapAdd(&__bss_end, ((uptr_t) & __stack - 256 - (uptr_t) (&__bss_end)));

    /*
     * Read eeprom configuration.
     */
    if (NutLoadConfig()) {
        strcpy(confos.hostname, "ethernut");
        NutSaveConfig();
    }

    /*
     * Create idle thread
     */
    NutThreadCreate("idle", NutIdle, 0, NUT_THREAD_IDLESTACK);
}

/*@}*/
