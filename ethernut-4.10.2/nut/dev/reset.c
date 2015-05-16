/*
 * Copyright (C) 2008 by egnite GmbH.
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
 * \file dev/reset.c
 * \brief System reset function.
 *
 * \verbatim
 * $Id: reset.c 2560 2009-03-18 04:00:06Z thiagocorrea $
 * \endverbatim
 */

#include <dev/watchdog.h>
#include <dev/reset.h>

/*!
 * \brief System reset.
 *
 * Typically this function will not return. If it does, then the
 * reset failed or the function is not supported.
 */
void NutReset(void)
{
#if defined(__AVR__)
    /* Use watchdog on AVR. */
    if (AvrWatchDogStart(1)) {
        for (;;);
    }
#elif defined(MCU_AT91)
    At91Reset();
#elif defined(__AVR32__)
	Avr32Reset();
#endif
}

/*!
 * \brief Retrieve the cause of the last system reset.
 *
 * Not implemented, always returns NUT_RSTTYP_UNKNOWN.
 *
 * \return Reset type.
 */
int NutResetCause(void)
{
#if defined(MCU_AT91)
    return At91ResetCause();
#elif defined(__AVR32__)
	return Avr32ResetCause();
#else
    return NUT_RSTTYP_UNKNOWN;
#endif
}
