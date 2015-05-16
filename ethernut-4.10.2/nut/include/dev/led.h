#ifndef _LED_H_
#define _LED_H_

/*
 * Copyright (C) 2009 by Rittal GmbH & Co. KG,
 * Ulrich Prinz <prinz.u@rittal.de> All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY EMBEDDED IT AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL EMBEDDED IT
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * For additional information see http://www.ethernut.de/
 *
 */

/*
 * $Log$
 *
 * Revision 0.2  2009/04/13 ulrichprinz
 * First checkin, led driver with extra functionality and variable io-access
 * (currently SAM7X256 is tested only)
 *
 */

/*!
 * \file dev/led.h
 * \brief LED handler definitions.
 */

/*!
 * \addtogroup xgDevLED
 */
/*@{*/

#include <stdint.h>

/*!
 * \brief LED action definitions.
 *
 * The LED modifies the LED according to these tokens when calling
 * NutSetLed().
 *
 */

/*!
 * \brief Switch LED off.
 *
 * The LED will be switched off immediately.
 * If a timOff parameter of not 0 is given, the LED will be switched off
 * for the given time in ms an then return to on state.
 */
#define	LED_OFF		0

/*!
 * \brief Switch LED on.
 *
 * The LED will be switched on immediately.
 * If a timOff parameter of not 0 is given, the LED will be switched on
 * for the given time in ms an then return off state.
 */
#define	LED_ON		1

/*!
 * \brief Flip the LED state (toggle).
 *
 * Toggle the LED from its actual state the the other.
 * Timing parameters are not supported.
 */
#define LED_FLIP    2

/*!
 * \brief Let the LED blink continuously.
 *
 * The parameter timOn specifies the on-time and parameter timOff the off-time.
 * Together with higher timer settings a software dimming can as well be
 * established as a simple blink effect or short / long flash effects.
 * Be adviced that high timer rates can decrease overall system speed.
 * Therefore you have to modify the timer rate in the led.c directly. Standard
 * setting is 10ms cycle time.
 */
#define LED_BLINK   4

/*@}*/

__BEGIN_DECLS
/* Function prototypes. */

extern void NutSetLed( HANDLE ledh, uint_fast8_t fxin, uint32_t timOn, uint32_t timOff);
extern void SetLedEvent( uint8_t led, uint32_t interval, uint32_t duration );

extern int NutRegisterLed( HANDLE * ledh, int port, int pin);

__END_DECLS
/* */

#endif
