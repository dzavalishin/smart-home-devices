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


/*!
 * \file arch/avr32/dev/reset.c
 * \brief AVR32 system reset.
 *
 * \verbatim
 * $Id: reset.c,v 1.2 2008/10/03 11:30:22 haraldkipp Exp $
 * \endverbatim
 */

#include <arch/avr32.h>
#include <dev/reset.h>
#include <dev/watchdog.h>

#include <avr32/io.h>

/*!
 * \addtogroup xgNutArchAvr32Rstc
 */
/*@{*/

/*!
 * \brief System reset.
 */
void Avr32Reset(void)
{
    /* use the watchdog. */
    if (NutWatchDogStart(1, 0)) {
        for (;;);
    }
}

int Avr32ResetCause(void)
{
    int rc = NUT_RSTTYP_UNKNOWN;

    switch (AVR32_PM.rcause) {
    case AVR32_PM_RCAUSE_POR:
        rc = NUT_RSTTYP_POWERUP;
        break;
    case AVR32_PM_RCAUSE_WDT:
        rc = NUT_RSTTYP_WATCHDOG;
        break;
    case AVR32_PM_RCAUSE_EXT:
        rc = NUT_RSTTYP_EXTERNAL;
        break;
    case AVR32_PM_RCAUSE_BOD:
        rc = NUT_RSTTYP_BROWNOUT;
        break;
    }
    return rc;
}

/*@}*/
