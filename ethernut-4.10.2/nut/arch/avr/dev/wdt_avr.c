/*
 * Copyright (C) 2006 by egnite Software GmbH. All rights reserved.
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
 * Revision 1.2  2008/08/11 06:59:18  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.1  2006/05/25 09:13:23  haraldkipp
 * Platform independent watchdog API added.
 *
 */

#if defined(__GNUC__)
#include <avr/wdt.h>
#endif
#include <sys/timer.h>
#include <dev/watchdog.h>

/*!
 * \addtogroup xgNutArchAvrDevWatchDog
 */
/*@{*/

/*!
 * \brief Watchdog oscillator frequency.
 */
#ifndef NUT_WDT_FREQ
#define NUT_WDT_FREQ    1165000
#endif

static ureg_t nested;
static uint8_t wdt_div;


/*!
 * \brief Start the AVR hardware watch dog timer.
 *
 * For portability, applications should use the platform independent 
 * \ref xgWatchDog "Watchdog Driver API".
 *
 * \param ms Desired watchdog timeout in milliseconds.
 *
 * \return The actual watchdog timeout.
 */
uint32_t AvrWatchDogStart(uint32_t ms)
{
    uint32_t ticks;

    wdt_reset();

    ticks = ((NUT_WDT_FREQ / 1000UL) * ms) >> 14;
    for (wdt_div = 0; wdt_div < 7 && ticks; wdt_div++) {
        ticks >>= 1;
    }
    wdt_enable(wdt_div);
    
    nested = 1;

    return (16384UL << wdt_div) / (NUT_WDT_FREQ / 1000UL);
}

/*!
 * \brief Re-start the AVR hardware watch dog timer.
 *
 * For portability, applications should use the platform independent 
 * \ref xgWatchDog "Watchdog Driver API".
 */
void AvrWatchDogRestart(void)
{
    wdt_reset();
}

/*!
 * \brief Disable the AVR hardware watch dog timer.
 *
 * For portability, applications should use the platform independent 
 * \ref xgWatchDog "Watchdog Driver API".
 */
void AvrWatchDogDisable(void)
{
    if (nested) {
        nested++;
    }
    wdt_disable();
}

/*!
 * \brief Enable the AVR hardware watch dog timer.
 *
 * For portability, applications should use the platform independent 
 * \ref xgWatchDog "Watchdog Driver API".
 */
void AvrWatchDogEnable(void)
{
    if (nested > 1 && --nested == 1) {
        wdt_enable(wdt_div);
    }
}

/*@}*/

