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
 * $Log: ostimer.c,v $
 *
 */

#include <cfg/os.h>
#include <cfg/clock.h>
#include <arch/avr32.h>
#include <dev/irqreg.h>
#include <sys/timer.h>

#include <arch/avr32/ihndlr.h>

#include <avr32/io.h>


/*!
 * \addtogroup xgNutArchAvr32OsTimer
 */
/*@{*/

#ifndef NUT_TICK_FREQ
#define NUT_TICK_FREQ   1000UL
#endif

IRQ_HANDLER sig_sysCompare = {
#ifdef NUT_PERFMON
    0,                          /* Interrupt counter, ir_count. */
#endif
    NULL,                       /* Passed argument, ir_arg. */
    NULL,                       /* Handler subroutine, ir_handler. */
    NULL                        /* Interrupt control, ir_ctl. */
};


/*!
 * \brief Determine the PLL output clock frequency.
 *
 * \param pll Specifies the PLL, 0 for PLL0, 1 for PLL1.
 *
 * \return Frequency of the selected PLL in Hertz.
 */
static u_int AVR32GetPllClock(int pll)
{
    u_int rc;
    u_int osc = 0;

    if (AVR32_PM.PLL[pll].pllosc)
        osc = 0;
    else
        osc = OSC0_VAL;

    /*
     * The main oscillator clock frequency is specified by the
     * configuration. It's usually equal to the on-board crystal.
     */
    rc = osc;

    if (AVR32_PM.PLL[pll].pllen) {
        u_int divider = AVR32_PM.PLL[pll].plldiv;
        u_int multiplier = AVR32_PM.PLL[pll].pllmul;

        if (divider)
            rc *= (multiplier + 1) / divider;
        else
            rc *= 2 * (multiplier + 1);

        if (AVR32_PM.PLL[pll].pllopt)
            rc /= 2;
    }
    return rc;
}

/*!
 * \brief Determine the processor clock frequency.
 *
 * \return CPU clock frequency in Hertz.
 */
static uint32_t Avr32GetProcessorClock(void)
{
    u_int rc = 0;
    u_int mckr = AVR32_PM.mcctrl;

    /* Determine the clock source. */
    switch (mckr & AVR32_PM_MCCTRL_MCSEL_MASK) {
    case AVR32_PM_MCCTRL_MCSEL_OSC0:
        /* OSC0 clock selected */
        rc = OSC0_VAL;
        break;
    case AVR32_PM_MCCTRL_MCSEL_SLOW:
        /* Slow clock selected. */
        rc = AVR32_PM_RCOSC_FREQUENCY;
        break;
    case AVR32_PM_MCCTRL_MCSEL_PLL0:
        /* PLL0 clock selected. */
        rc = AVR32GetPllClock(0);
        break;
    }

    /* Handle pre-scaling. */
    if (AVR32_PM.cksel & AVR32_PM_CKSEL_CPUDIV_MASK) {
        int cpusel = (AVR32_PM.cksel & AVR32_PM_CKSEL_CPUSEL_MASK) >> AVR32_PM_CKSEL_CPUSEL_OFFSET;
        /* CPUDIV = 1: CPU Clock equals main clock divided by 2^(CPUSEL+1). */
        rc /= _BV(cpusel + 1);
    }

    return rc;
}


/*!
* \brief System Compare interrupt entry.
*/
static SIGNAL(SystemCompareIrqEntry)
{
    IRQ_ENTRY();
    uint32_t compare;
    compare = Get_system_register(AVR32_COMPARE);
    Set_system_register(AVR32_COMPARE, 0);
    if (sig_sysCompare.ir_handler) {
        (sig_sysCompare.ir_handler) (sig_sysCompare.ir_arg);
    }
#if __AVR32_AP7000__ || __AT32AP7000__ || __AVR32_UC3A0512ES__ || __AVR32_UC3A1512ES__
    /* AP7000 and UC3 prior to rev H doesn't clear COUNT on compare match, so we need to
       offset COMPARE */
    compare += NutGetCpuClock() / NUT_TICK_FREQ;
    if (!compare)               // Avoid disabling compare.
        ++compare;
#endif
    Set_system_register(AVR32_COMPARE, compare);
    IRQ_EXIT();
}

/*!
 * \brief Initialize system timer.
 *
 * This function is automatically called by Nut/OS
 * during system initialization.
 *
 * Nut/OS uses on-chip tick counter for its timer services.
 * Applications should not modify any registers of this
 * timer, but make use of the Nut/OS timer API. Timer 1
 * and timer 2 are available to applications.
 */
void NutRegisterTimer(void (*handler) (void *))
{
    /* Set compare value for the specified tick frequency. */
    Set_system_register(AVR32_COMPARE, NutGetCpuClock() / NUT_TICK_FREQ + Get_system_register(AVR32_COUNT));

    sig_sysCompare.ir_handler = handler;

    register_interrupt(SystemCompareIrqEntry, AVR32_CORE_COMPARE_IRQ, AVR32_INTC_INT0);
}

/*!
 * \brief Return the CPU clock in Hertz.
 *
 * On AVR32 CPUs the processor clock may differ from the clock
 * driving the peripherals. In this case NutArchClockGet() will
 * provide the correct clock depending on it's argument.
 *
 * \return CPU clock frequency in Hertz.
 */
uint32_t NutArchClockGet(int idx)
{
    uint32_t rc = AVR32_PM_PBA_MAX_FREQ;

    if (idx == NUT_HWCLK_CPU || idx == NUT_HWCLK_PERIPHERAL_HSB) {
        rc = Avr32GetProcessorClock();
    } else if (idx == NUT_HWCLK_PERIPHERAL_A) {
        /* Get PBA Clock */
        rc = Avr32GetProcessorClock();

        if (AVR32_PM.CKSEL.pbadiv) {
            rc /= _BV(AVR32_PM.CKSEL.pbasel + 1);
        }
    } else if (idx == NUT_HWCLK_PERIPHERAL_B) {
        /* Get PBB Clock */
        rc = Avr32GetProcessorClock();

        if (AVR32_PM.CKSEL.pbbdiv) {
            rc /= _BV(AVR32_PM.CKSEL.pbbsel + 1);
        }
    } else if (idx == NUT_HWCLK_SLOW_CLOCK) {
        /* Can be changed using the RCCR register 
           but there is no information on the datasheet yet
           on how to do so. Therefore we don't know how to calculate
           non-default values yet. */
#if defined( __AVR32_AP7000__ )
        rc = 32768;             // AP7000 has no constant for slow clock, but this is the same as XIN32, which should be 32768hz
#else
        rc = AVR32_PM_RCOSC_FREQUENCY;
#endif
    }

    return rc;
}


/*!
 * \brief Return the number of system ticks per second.
 *
 * \return System tick frequency in Hertz.
 */
uint32_t NutGetTickClock(void)
{
    return NUT_TICK_FREQ;
}

/*!
 * \brief Calculate system ticks for a given number of milliseconds.
 */
uint32_t NutTimerMillisToTicks(uint32_t ms)
{
#if (NUT_TICK_FREQ % 1000)
    if (ms >= 0x3E8000UL)
        return (ms / 1000UL) * NUT_TICK_FREQ;
    return (ms * NUT_TICK_FREQ + 999UL) / 1000UL;
#else
    return ms * (NUT_TICK_FREQ / 1000UL);
#endif
}

/*@}*/
