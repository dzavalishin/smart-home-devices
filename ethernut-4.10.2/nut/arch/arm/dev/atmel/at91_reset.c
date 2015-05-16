/*
 * Copyright 2008 by egnite GmbH
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
 * \file arch/arm/dev/at91_reset.c
 * \brief AT91 system reset.
 *
 * \verbatim
 * $Id: at91_reset.c 3449 2011-05-31 19:08:15Z mifi $
 * \endverbatim
 */

#include <arch/arm.h>
#include <dev/watchdog.h>
#include <dev/reset.h>

/*!
 * \addtogroup xgNutArchArmAt91Rstc
 */
/*@{*/

/*!
 * \brief System reset.
 */
void At91Reset(void)
{
#if defined(RSTC_PROCRST)
    /* Use reset controller if available. */
    outr(RSTC_CR, RSTC_KEY | RSTC_EXTRST | RSTC_PERRST | RSTC_PROCRST);
#else
    /* Otherwise use the watchdog. */
    if (At91WatchDogStart(1, 0)) {
        for (;;);
    }
#endif
}

int At91ResetCause(void)
{
    int rc = NUT_RSTTYP_UNKNOWN;

#if defined(RSTC_SR)
    switch (inr(RSTC_SR) & RSTC_RSTTYP) {
    case RSTC_RSTTYP_POWERUP:
        rc = NUT_RSTTYP_POWERUP;
        break;
    case RSTC_RSTTYP_WATCHDOG:
        rc = NUT_RSTTYP_WATCHDOG;
        break;
    case RSTC_RSTTYP_SOFTWARE:
        rc = NUT_RSTTYP_SOFTWARE;
        break;
    case RSTC_RSTTYP_USER:
        rc = NUT_RSTTYP_EXTERNAL;
        break;
    case RSTC_RSTTYP_BROWNOUT:
        rc = NUT_RSTTYP_BROWNOUT;
        break;
    }
#endif
    return rc;
}

/*@}*/
