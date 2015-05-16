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
 * $Log: wdt.c,v $
 *
 */

#include <sys/timer.h>
#include <dev/watchdog.h>
#include <arch/avr32.h>

#include <avr32/io.h>

// AP7000 includes lack this define, even though they are the same as UC30512
#if !defined(AVR32_WDT_KEY_VALUE)
#define AVR32_WDT_KEY_VALUE                            0x00000055
#endif

/*!
 * \addtogroup xgNutArchAvr32DevWatchDog
 */
/*@{*/

static ureg_t nested;

static void wdt_set_ctrl(unsigned long ctrl)
{
    AVR32_WDT.ctrl = ctrl | (AVR32_WDT_KEY_VALUE << AVR32_WDT_CTRL_KEY_OFFSET);
    AVR32_WDT.ctrl = ctrl | ((~AVR32_WDT_KEY_VALUE << AVR32_WDT_CTRL_KEY_OFFSET) & AVR32_WDT_CTRL_KEY_MASK);
}

static long long wdt_get_us_timeout_period(void)
{
    unsigned int slowclock = NutArchClockGet(NUT_HWCLK_SLOW_CLOCK);
    // Read CTRL.PSEL and translate it into us.
    return (AVR32_WDT.ctrl & AVR32_WDT_CTRL_EN_MASK) ?
        ((1ULL << (((AVR32_WDT.ctrl & AVR32_WDT_CTRL_PSEL_MASK) >> AVR32_WDT_CTRL_PSEL_OFFSET) + 1)) *
         1000000 + slowclock / 2) / slowclock : -1;
}

/*!
 * \brief Start the AVR32 hardware watch dog timer.
 *
 * For portability, applications should use the platform independent 
 * \ref xgWatchDog "Watchdog Driver API".
 */
uint32_t Avr32WatchDogStart(uint32_t ms)
{
    unsigned long long int timeout = ms * 1000;

    unsigned int slowclock = NutArchClockGet(NUT_HWCLK_SLOW_CLOCK);

    Avr32WatchDogDisable();

#define MIN_US_TIMEOUT_PERIOD  \
	(((1ULL <<  1 ) * 1000000 + slowclock / 2) / slowclock)
#define MAX_US_TIMEOUT_PERIOD  \
	(((1ULL << (1 << AVR32_WDT_CTRL_PSEL_SIZE)) * 1000000 + slowclock / 2) / slowclock)

    // Set the CTRL.EN bit and translate the us timeout to fit in CTRL.PSEL using
    // the formula wdt = 2pow(PSEL+1) / fRCosc in useconds.
    if (timeout < MIN_US_TIMEOUT_PERIOD)
        timeout = MIN_US_TIMEOUT_PERIOD;
    else if (timeout > MAX_US_TIMEOUT_PERIOD)
        timeout = MAX_US_TIMEOUT_PERIOD;

    wdt_set_ctrl(AVR32_WDT_CTRL_EN_MASK |
                 ((32 - __builtin_clz(((((timeout * slowclock + 500000) / 1000000) << 1) - 1) >> 1) -
                   1) << AVR32_WDT_CTRL_PSEL_OFFSET));

    Avr32WatchDogRestart();
    nested = 1;

    // Return the actual wdt period in us.
    return wdt_get_us_timeout_period() / 1000;
}

/*!
 * \brief Re-start the AVR32 hardware watch dog timer.
 *
 * For portability, applications should use the platform independent 
 * \ref xgWatchDog "Watchdog Driver API".
 */
void Avr32WatchDogRestart(void)
{
    wdt_set_ctrl(AVR32_WDT.ctrl | AVR32_WDT_CTRL_EN_MASK);
    AVR32_WDT.clr = 0;
}

/*!
 * \brief Disable the AVR32 hardware watch dog timer.
 *
 * For portability, applications should use the platform independent 
 * \ref xgWatchDog "Watchdog Driver API".
 */
void Avr32WatchDogDisable(void)
{
    if (nested) {
        nested++;
    }
    wdt_set_ctrl(AVR32_WDT.ctrl & ~AVR32_WDT_CTRL_EN_MASK);
}

/*!
 * \brief Enable the AT91 hardware watch dog timer.
 *
 * For portability, applications should use the platform independent 
 * \ref xgWatchDog "Watchdog Driver API".
 */
void Avr32WatchDogEnable(void)
{
    if (nested > 1 && --nested == 1) {
        Avr32WatchDogRestart();
    }
}

/*@}*/
