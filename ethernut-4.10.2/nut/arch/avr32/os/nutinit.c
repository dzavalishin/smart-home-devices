/*!
 * Copyright (C) 2001-2010 by egnite Software GmbH
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


/*
 * $Log: nutinit.c,v $
 *
 *
 */

#include <cfg/clock.h>
#include <cfg/arch.h>
#include <cfg/memory.h>
#include <cfg/os.h>
#include <sys/heap.h>
#include <sys/confos.h>
#include <sys/thread.h>
#include <sys/timer.h>

#include <arch/avr32.h>

#include <arch/avr32/pm.h>
#include <arch/avr32/ihndlr.h>


/*!
 * \addtogroup xgNutArchAvr32Init
 */
/*@{*/

#ifndef NUT_THREAD_MAINSTACK
#define NUT_THREAD_MAINSTACK    1024
#endif

/* Unknown optimized value */
#ifndef NUT_THREAD_IDLESTACK
#define NUT_THREAD_IDLESTACK    512
#endif


extern void *__heap_start;

extern void *__ram_start__;
extern void *__ram_size__;

/*!
* \brief Last memory address.
*/
#undef NUTMEM_START
#define NUTMEM_START ((uint32_t)&__ram_start__)

#undef NUTMEM_SIZE
#define NUTMEM_SIZE ((uint32_t)&__ram_size__)

#define NUTMEM_END (uptr_t)(NUTMEM_START + NUTMEM_SIZE - 1U)
extern void *__heap_start;

#define HEAP_START  &__heap_start
#define HEAP_SIZE  ((uptr_t) (NUTMEM_END - 256 - (uptr_t) (&__heap_start)))


#if !defined(__cplusplus)
extern void NutAppMain(void *arg) __attribute__ ((noreturn));
#else
extern void main(void *);
#endif


typedef union {
    unsigned long fcr;
    avr32_flashc_fcr_t FCR;
} u_avr32_flashc_fcr_t;

static void flashc_set_wait_state(unsigned int wait_state)
{
    u_avr32_flashc_fcr_t u_avr32_flashc_fcr = { AVR32_FLASHC.fcr };
    u_avr32_flashc_fcr.FCR.fws = wait_state;
    AVR32_FLASHC.fcr = u_avr32_flashc_fcr.fcr;
}

/*!
 * \brief Idle thread.
 *
 * \param arg Ignored by the idle thread.
 *
 * This function runs in an endless loop as a lowest priority thread.
 */
THREAD(NutIdle, arg)
{
    /* Init INTC */
    init_interrupts();

    /* Initialize system timers. */
    NutTimerInit();

    /* Read OS configuration from non-volatile memory. We can't do this
     ** earlier, because the low level driver may be interrupt driven. */
    NutLoadConfig();

    /* Create the main application thread. Watch this carefully when
     ** changing compilers and compiler versions. Some handle main()
     ** in a special way, like setting the stack pointer and other
     ** weird stuff that may break this code. */
    NutThreadCreate("main", main, 0, (NUT_THREAD_MAINSTACK * NUT_THREAD_STACK_MULT) + NUT_THREAD_STACK_ADD);

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
    uint32_t CPUFrequency;

    /* Switch main clock to Oscillator 0 */
    pm_switch_to_osc0(&AVR32_PM, OSC0_VAL, AVR32_PM_OSCCTRL0_STARTUP_2048_RCOSC);

    pm_pll_setup(&AVR32_PM, 0,  /* use PLL0     */
                 PLL_MUL_VAL,   /* MUL          */
                 PLL_DIV_VAL,   /* DIV          */
                 0,             /* Oscillator 0 */
                 16);           /* lockcount in main clock for the PLL wait lock */

    /*
     * This function will set a PLL option.
     *
     * pm             Base address of the Power Manager (i.e. &AVR32_PM)
     * pll            PLL number 0
     * pll_freq       Set to 1 for VCO frequency range 80-180MHz,
     *                set to 0 for VCO frequency range 160-240Mhz.
     * pll_div2       Divide the PLL output frequency by 2 (this settings does
     *                not change the FVCO value)
     * pll_wbwdisable 1 Disable the Wide-Bandwidth Mode (Wide-Bandwidth mode
     *                allow a faster startup time and out-of-lock time). 0 to
     *                enable the Wide-Bandwidth Mode.
     */
    pm_pll_set_option(&AVR32_PM, 0,     /* use PLL0 */
                      PLL_FREQ_VAL,     /* pll_freq */
                      PLL_DIV2_VAL,     /* pll_div2 */
                      PLL_WBWD_VAL);    /* pll_wbwd */

    /* Enable PLL0 */
    pm_pll_enable(&AVR32_PM, 0);

    /* Wait for PLL0 locked */
    pm_wait_for_pll0_locked(&AVR32_PM);

    /* Create PBA, PBB and HSB clock */
    pm_cksel(&AVR32_PM, PLL_PBADIV_VAL, /* pbadiv */
             PLL_PBASEL_VAL,    /* pbasel */
             PLL_PBBDIV_VAL,    /* pbbdiv */
             PLL_PBBSEL_VAL,    /* pbbsel */
             PLL_HSBDIV_VAL,    /* hsbdiv */
             PLL_HSBSEL_VAL);   /* hsbsel */

    /* Calculate CPU frequency */
    CPUFrequency = (OSC0_VAL * (PLL_MUL_VAL + 1)) / PLL_DIV_VAL;
    CPUFrequency = (PLL_DIV2_VAL == 0) ? CPUFrequency : CPUFrequency >> 1;

    if (PLL_HSBDIV_VAL > 0) {
        CPUFrequency = CPUFrequency >> (PLL_HSBSEL_VAL + 1);
    }

    if (CPUFrequency > AVR32_FLASHC_FWS_0_MAX_FREQ) {
        /*
         * Set one wait-state (WS) for the flash controller if the 
         * HSB/CPU is more than AVR32_FLASHC_FWS_0_MAX_FREQ.
         */
        flashc_set_wait_state(1);
    }

    /* Switch PLL to main clock */
    pm_switch_to_clock(&AVR32_PM, AVR32_PM_MCSEL_PLL0);

#ifdef EARLY_STDIO_DEV
    /* We may optionally initialize stdout as early as possible.
     ** Be aware, that no heap is available and no threads are 
     ** running. We need a very basic driver here, which won't
     ** use interrupts or call malloc, NutEventXxx, NutSleep etc. */
    {
        extern NUTDEVICE EARLY_STDIO_DEV;
        static struct __iobuf early_stdout;
        /* Initialize the output device. */
        EARLY_STDIO_DEV.dev_init(&EARLY_STDIO_DEV);
        /* Assign a static iobuf. */
        stdout = &early_stdout;
        /* Open the device. */
        stdout->iob_fd = (int) EARLY_STDIO_DEV.dev_open(&EARLY_STDIO_DEV, "", 0, 0);
        /* Set the mode. No idea if this is required. */
        stdout->iob_mode = _O_WRONLY | _O_CREAT | _O_TRUNC;
        /* A first trial. */
        puts("\nStarting Nut/OS");
    }
#endif

    /* Initialize our heap memory. */
    NutHeapAdd(HEAP_START, HEAP_SIZE);

    /* Create idle thread. Note, that the first call to NutThreadCreate 
     ** will never return. */
    NutThreadCreate("idle", NutIdle, 0, (NUT_THREAD_IDLESTACK * NUT_THREAD_STACK_MULT) + NUT_THREAD_STACK_ADD);
}

/*@}*/
