/*
 * Copyright (C) 2001-2004 by egnite Software GmbH. All rights reserved.
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
 * Revision 1.1  2005/05/27 17:21:10  drsung
 * Moved the file.
 *
 * Revision 1.1  2004/03/16 16:48:46  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 */

/*!
 * \addtogroup xgNutArchM68kInit
 */
/*@{*/

#define NUTRAMEND ((volatile u_char *)0x7FFF)

#ifdef __GNUC__
/*
 * Some special declarations for AVRGCC. The runtime library
 * executes section .init8 before finally jumping to main().
 * We never let it do that jump, but start main() as a
 * separate thread. This introduces new problems:
 * 1. The compiler reinitializes the stack pointer when
 *    entering main, at least version 3.3 does it.
 * 2. The compiler doesn't allow to redeclare main to make
 *    it fit for NutThreadCreate().
 * 3. The runtime library requires, that main is defined
 *    somewhere.
 * Solutions:
 * 1. We do not use main at all, but let the preprocessor
 *    redefine it to NutAppMain() in the application code.
 *    See compiler.h. Note, that the declaration of NutAppMain 
 *    in this module differs from the one in the application
 *    code. Fortunately the linker doesn't detect this hack.
 * 2. We use a linker option to set the symbol main to zero.
 *
 * Thanks to Joerg Wunsch, who helped to solve this.
 */
void NutInit(void) __attribute__ ((naked)) __attribute__ ((section(".init8")));
extern void NutAppMain(void *arg) __attribute__ ((noreturn));
#else
extern void main(void *);
#endif

#ifdef __GNUC__
static void NutInitXRAM(void) __attribute__ ((naked, section(".init1"), used));
void NutInitXRAM(void)
{
    /* At the very beginning enable extended memory interface.
     */
    MCUCR = _BV(SRE) | _BV(SRW);
}

#endif

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
    NutThreadCreate("main", main, 0, 768);

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

/*
 * \brief Nut/OS Initialization.
 *
 * Initializes the memory management and the thread system and starts 
 * an idle thread, which in turn initializes the timer management. 
 * Finally the application's main() function is called.
 *
 * Depending on the compiler, different methods are used to execute this
 * function before main() is called.
 *
 * For ICCAVR the default crtatmega.o startup file is replaced by
 * crtnut.o, which calls NutInit instead of main(). This is done
 * by adding the following compiler options in the project:
 * \code -ucrtnut.o nutinit.o \endcode
 * 
 * For AVRGCC this function is located in section .init8, which is
 * called immediately before jumping to main(). NutInit is defined
 * as:
 * \code
 * void NutInit(void) __attribute__ ((naked)) __attribute__ ((section (".init8")));
 * \endcode
 */
void NutInit(void)
{
    /*
     * We can't use local variables in naked functions.
     */
#ifdef NUTDEBUG
    outp(7, UBRR);
    outp(BV(RXEN) | BV(TXEN), UCR);
#endif

#ifndef __GNUC__
// FIXME: move this line to an appropriate initialization section of ICCAVR (os)
    outp(BV(SRE) | BV(SRW), MCUCR);
#endif

    /* First check, whether external RAM is available
     */
    *(NUTRAMEND - 1) = 0x55;
    *NUTRAMEND = 0xAA;
    if (*(NUTRAMEND - 1) == 0x55 && *NUTRAMEND == 0xAA) {
        /* If we have external RAM, initialize stack pointer to
         *  end of external RAM to avoid overwriting .data and .bss section
         */
        SP = (u_short) NUTRAMEND;

        /* Then add the remaining RAM to heap
         */
        if ((u_short) NUTRAMEND - (u_short) (&__heap_start) > 384)
            NutHeapAdd(&__heap_start, (u_short) NUTRAMEND - 256 - (u_short) (&__heap_start));
    } else {
        /* No external RAM, so disable external memory interface to use the port pins
           for normal operation
         */
        MCUCR = 0x00;

        /* Add the remaining internal RAM to heap
         */
        if ((u_short) RAMEND - (u_short) (&__heap_start) > 384)
            NutHeapAdd(&__heap_start, (u_short) RAMEND - 256 - (u_short) (&__heap_start));
    };
    /*
     * Read eeprom configuration.
     */
    if (NutLoadConfig()) {
        strcpy(confos.hostname, "ethernut");
        NutSaveConfig();
    }

    /* Create idle thread
     */
    NutThreadCreate("idle", NutIdle, 0, 384);
}

/*@}*/
