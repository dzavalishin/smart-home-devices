/*
 * Copyright (C) 2001-2007 by egnite Software GmbH. All rights reserved.
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
 * Revision 1.23  2009/01/17 11:26:37  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.22  2008/08/22 09:25:33  haraldkipp
 * Clock value caching and new functions NutArchClockGet, NutClockGet and
 * NutClockSet added.
 *
 * Revision 1.21  2008/08/11 06:59:12  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.20  2008/08/06 12:51:01  haraldkipp
 * Added support for Ethernut 5 (AT91SAM9XE reference design).
 *
 * Revision 1.19  2008/07/08 08:25:04  haraldkipp
 * NutDelay is no more architecture specific.
 * Number of loops per millisecond is configurable or will be automatically
 * determined.
 * A new function NutMicroDelay provides shorter delays.
 *
 * Revision 1.18  2008/02/15 16:58:41  haraldkipp
 * Spport for AT91SAM7SE512 added.
 *
 * Revision 1.17  2007/10/04 19:59:47  olereinhardt
 * Support for SAM7S256 added
 *
 * Revision 1.16  2007/08/17 10:44:37  haraldkipp
 * Timer enable/disable macro replaces previous global interrupt
 * enable/disable or function calling.
 *
 * Revision 1.15  2007/04/12 09:03:48  haraldkipp
 * Miserable delay routine will now honor milliseconds on a 73 MHz ARM.
 *
 * Revision 1.14  2007/02/15 16:14:39  haraldkipp
 * Periodic interrupt timer can be used as a system clock.
 *
 * Revision 1.13  2006/10/08 16:48:07  haraldkipp
 * Documentation fixed
 *
 * Revision 1.12  2006/09/29 12:37:36  haraldkipp
 * Now working correctly, if the CPU is running on the second PLL.
 *
 * Revision 1.11  2006/09/05 12:27:25  haraldkipp
 * PLL clock calculation re-arranged to prevent 32-bit overflow.
 * NutTimerMillisToTicks() returned wrong result. Shane Buckham reported
 * this long time ago. Many thanks. Needs to be fixed for other platforms too.
 *
 * Revision 1.10  2006/08/31 18:59:50  haraldkipp
 * Added support for the AT91SAM9260. We now determine between processor and
 * master clock. A new API function At91GetMasterClock() had been added to
 * query the latter.
 *
 * Revision 1.9  2006/08/05 12:00:01  haraldkipp
 * NUT_CPU_FREQ did not override AT91_PLL_MAINCK or NUT_PLL_CPUCLK. Fixed.
 *
 * Revision 1.8  2006/07/26 11:17:16  haraldkipp
 * Defining AT91_PLL_MAINCK will automatically determine SAM7X clock by
 * reading PLL settings.
 *
 * Revision 1.7  2006/07/05 07:59:41  haraldkipp
 * Daidai's support for AT91SAM7X added.
 *
 * Revision 1.6  2006/06/28 17:10:35  haraldkipp
 * Include more general header file for ARM.
 *
 * Revision 1.5  2006/03/02 19:53:01  haraldkipp
 * Bugfix. The system timer configuration was based on a fixed MCU clock
 * of 66.6 MHz. Now it uses the actual frequency.
 *
 * Revision 1.4  2006/01/05 16:46:25  haraldkipp
 * Added support for CY22393 programmable clock chip.
 *
 * Revision 1.3  2005/10/24 08:34:13  haraldkipp
 * Moved AT91 family specific header files to sbudir arm.
 * Use new IRQ API.
 *
 * Revision 1.2  2005/08/02 17:46:45  haraldkipp
 * Major API documentation update.
 *
 * Revision 1.1  2005/07/26 18:02:26  haraldkipp
 * Moved from dev.
 *
 * Revision 1.2  2005/07/20 09:17:26  haraldkipp
 * Default NUT_CPU_FREQ and NUT_TICK_FREQ added.
 * NutTimerIntr() removed, because we can use the hardware independent code.
 *
 * Revision 1.1  2005/05/27 17:16:40  drsung
 * Moved the file.
 *
 * Revision 1.5  2005/04/05 17:50:46  haraldkipp
 * Use register names in gba.h.
 *
 * Revision 1.4  2004/11/08 19:16:37  haraldkipp
 * Hacked in Gameboy timer support
 *
 * Revision 1.3  2004/10/03 18:42:21  haraldkipp
 * No GBA support yet, but let the compiler run through
 *
 * Revision 1.2  2004/09/08 10:19:39  haraldkipp
 * Running on AT91 and S3C, thanks to James Tyou
 *
 */

#define NUT_DEPRECATED

#include <cfg/os.h>
#include <cfg/clock.h>
#include <arch/arm.h>
#include <dev/irqreg.h>
#include <sys/timer.h>

#ifndef NUT_CPU_FREQ
#ifdef NUT_PLL_CPUCLK
#include <dev/cy2239x.h>
#elif !defined(AT91_PLL_MAINCK)
#define NUT_CPU_FREQ    73728000UL
#endif /* !AT91_PLL_MAINCK */
#endif /* !NUT_CPU_FREQ */


/*!
 * \addtogroup xgNutArchArmOsTimerAt91
 */
/*@{*/

#ifndef NUT_TICK_FREQ
#define NUT_TICK_FREQ   1000UL
#endif

/*!
 * \brief Initialize system timer.
 *
 * This function is automatically called by Nut/OS
 * during system initialization.
 *
 * Nut/OS uses on-chip timer 0 for its timer services.
 * Applications should not modify any registers of this
 * timer, but make use of the Nut/OS timer API. Timer 1
 * and timer 2 are available to applications.
 */
void NutRegisterTimer(void (*handler) (void *))
{
#if defined(NUT_TICK_AT91PIT)
    /* Set compare value for the specified tick frequency. */
#if defined(AT91_PLL_MAINCK)
    outr(PIT_MR, (At91GetMasterClock() / (16 * NUT_TICK_FREQ) - 1) << PIT_PIV_LSB);
#else
    outr(PIT_MR, (NutGetCpuClock() / (16 * NUT_TICK_FREQ) - 1) << PIT_PIV_LSB);
#endif

    /* Register system interrupt handler. */
    NutRegisterSysIrqHandler(&syssig_PIT, handler, NULL);
    /* Enable interval timer and interval timer interrupts */
    outr(PIT_MR, inr(PIT_MR) | PIT_PITEN | PIT_PITIEN);
    NutSysIrqEnable(&syssig_PIT);
    inr(PIT_PIVR);

#else   /* NUT_TICK_AT91PIT */

#if defined(MCU_AT91SAM7X) || defined(MCU_AT91SAM7S256) || defined(MCU_AT91SAM9260) || defined(MCU_AT91SAM9G45) || defined (MCU_AT91SAM7SE512) || defined(MCU_AT91SAM9XE512)
    /* Enable TC0 clock. */
    outr(PMC_PCER, _BV(TC0_ID));
#endif

    /* Disable the Clock Counter */
    outr(TC0_CCR, TC_CLKDIS);
    /* Disable all interrupts */
    outr(TC0_IDR, 0xFFFFFFFF);
    /* Clear the status register. */
    inr(TC0_SR);
    /* Select divider and compare trigger */
    outr(TC0_CMR, TC_CLKS_MCK32 | TC_CPCTRG);
    /* Enable the Clock counter */
    outr(TC0_CCR, TC_CLKEN);
    /* Validate the RC compare interrupt */
    outr(TC0_IER, TC_CPCS);

    /* Register timer interrupt handler. */
    NutRegisterIrqHandler(&sig_TC0, handler, 0);
    /* Set to lowest priority. */
    NutIrqSetPriority(&sig_TC0, 0);

    /* Enable timer 0 interrupts */
    NutIrqEnable(&sig_TC0);
    //outr(AIC_IECR, _BV(TC0_ID));

    /* Set compare value for 1 ms. */
#if defined(AT91_PLL_MAINCK)
    outr(TC0_RC, At91GetMasterClock() / (32 * NUT_TICK_FREQ));
#else
    outr(TC0_RC, NutGetCpuClock() / (32 * NUT_TICK_FREQ));
#endif

    /* Software trigger starts the clock. */
    outr(TC0_CCR, TC_SWTRG);

#endif  /* NUT_TICK_AT91PIT */
}

#if defined(AT91_PLL_MAINCK)

#if !defined(AT91_SLOW_CLOCK)
/* This is just a guess and may be completely wrong. */
#define AT91_SLOW_CLOCK 32000
#endif

/*!
 * \brief Determine the PLL output clock frequency.
 *
 * \param plla Specifies the PLL, 0 for default, 1 for alternate.
 *
 * \return Frequency of the selected PLL in Hertz.
 */
static unsigned int At91GetPllClock(int plla)
{
    unsigned int rc;
    unsigned int pllr;
    unsigned int divider;

    /* 
     * The main oscillator clock frequency is specified by the
     * configuration. It's usually equal to the on-board crystal.
     */
    rc = AT91_PLL_MAINCK;

    /* Retrieve the clock generator register of the selected PLL. */
#if defined(CKGR_PLLAR) && defined(CKGR_PLLBR)
    pllr = plla ? inr(CKGR_PLLAR) : inr(CKGR_PLLBR);
#else
    pllr = inr(CKGR_PLLR);
#endif

    /* Extract the divider value. */
    divider = (pllr & CKGR_DIV) >> CKGR_DIV_LSB;

    if (divider) {
        rc /= divider;
        rc *= ((pllr & CKGR_MUL) >> CKGR_MUL_LSB) + 1;
    }
    return rc;
}

/*!
 * \brief Determine the processor clock frequency.
 *
 * \return CPU clock frequency in Hertz.
 */
static uint32_t At91GetProcessorClock(void)
{
    unsigned int rc = 0;
    unsigned int mckr = inr(PMC_MCKR);

    /* Determine the clock source. */
    switch(mckr & PMC_CSS) {
    case PMC_CSS_SLOW_CLK:
        /* Slow clock selected. */
        rc = AT91_SLOW_CLOCK;
        break;
    case PMC_CSS_MAIN_CLK:
        /* Main clock selected. */
        rc = AT91_PLL_MAINCK;
        break;
#if defined(PMC_CSS_PLLA_CLK)
    case PMC_CSS_PLLA_CLK:
        /* PLL A clock selected. */
        rc = At91GetPllClock(1);
        break;
#endif
#if defined(PMC_CSS_PLLB_CLK)
    case PMC_CSS_PLLB_CLK:
        /* PLL (B) clock selected. */
        rc = At91GetPllClock(0);
        break;
#elif defined(PMC_CSS_PLL_CLK)
    case PMC_CSS_PLL_CLK:
        /* PLL (B) clock selected. */
        rc = At91GetPllClock(0);
        break;
#endif
    }

    /* Handle pre-scaling. */
    mckr &= PMC_PRES;
    mckr >>= PMC_PRES_LSB;
    if (mckr < 7) {
        rc /= _BV(mckr);
    }
    else {
        rc = 0;
    }
    return rc;
}

/*!
 * \brief Determine the master clock frequency.
 *
 * \deprecated Use NutArchClockGet(NUT_HWCLK_PERIPHERAL)
 *
 * \return Master clock frequency in Hertz.
 */
uint32_t At91GetMasterClock(void)
{
    return NutArchClockGet(NUT_HWCLK_PERIPHERAL);
}

#endif /* AT91_PLL_MAINCK */

#ifndef NUT_CPU_FREQ
/*!
 * \brief Return the CPU clock in Hertz.
 *
 * On several AT91 CPUs the processor clock may differ from the clock 
 * driving the peripherals. In this case At91GetMasterClock() will 
 * provide the correct master clock.
 *
 * \return CPU clock frequency in Hertz.
 */
uint32_t NutArchClockGet(int idx)
{
    uint32_t rc = 0;

    if (idx == NUT_HWCLK_CPU) {
#if defined(AT91_PLL_MAINCK)
        rc = At91GetProcessorClock();
#elif defined(NUT_PLL_CPUCLK)
        rc = Cy2239xGetFreq(NUT_PLL_CPUCLK, 7);
#else
#warning "No CPU Clock defined"
#endif
    }
#if defined(AT91_PLL_MAINCK)
    if (idx == NUT_HWCLK_PERIPHERAL) {
        rc = At91GetProcessorClock();
#if defined(PMC_MDIV)
        switch(inr(PMC_MCKR) & PMC_MDIV) {
        case PMC_MDIV_2:
            rc /= 2;
            break;
        case PMC_MDIV_4:
            rc /= 4;
            break;
        case PMC_MDIV_3:
            rc /= 3;
            break;
        }
#endif
    }
#ifdef PMC_PLLADIV2
    if (inr(PMC_MCKR) & PMC_PLLADIV2)
    {
      rc /= 2;
    }
#endif
#endif
    return rc;
}
#endif

/*!
 * \brief Return the number of system ticks per second.
 *
 * \return System tick frequency in Hertz.
 */
uint32_t NutGetTickClock(void)
{
    unsigned int rc;

#if defined(NUT_TICK_AT91PIT)
    rc = ((inr(PIT_MR) & PIT_PIV) + 1) * 16;
#else
    rc = inr(TC0_RC) * 32;
#endif

    if (rc) {
#if defined(AT91_PLL_MAINCK)
        return At91GetMasterClock() / rc;
#else
        return NutGetCpuClock() / rc;
#endif
    }
    return NUT_TICK_FREQ;
}

/*!
 * \brief Calculate system ticks for a given number of milliseconds.
 */
uint32_t NutTimerMillisToTicks(uint32_t ms)
{
    return (ms * NutGetTickClock()) / 1000;
}

/*@}*/

