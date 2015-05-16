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


/*!
 * \file dev/led.c
 * \brief LED driver and event handler.
 *
 * This device driver provieds flexible handling of multiple LEDs
 *
 * \verbatim
 * $Log$
 *
 * Revision 0.3  2009/09/17 ulrichprinz
 * Changed parameters and added more flexible functionalities by reducing
 * command tokens.
 * (currently SAM7X256 is tested only)
 *
 * Revision 0.2  2009/04/13 ulrichprinz
 * First checkin, led driver with extra functionality and variable io-access
 * (currently SAM7X256 is tested only)
 *
 * \endverbatim
 */

#include <cfg/os.h>

#include <compiler.h>
#include <dev/board.h>
#include <dev/gpio.h>

#include <stdlib.h>
#include <string.h>
#include <sys/heap.h>
#include <sys/event.h>
#include <sys/timer.h>
#include <sys/atom.h>

#include <sys/nutdebug.h>

#include <cfg/pca9555.h>
#ifdef LED_SUPPORT_IOEXP
#include <dev/pca9555.h>
#endif
#include "cfg/led.h"
#include "dev/led.h"

/*!
 * \addtogroup xgDevLED
 */
/*@{*/

/* define inverted LED states as LEDs are driven by low side switching */

#ifdef LED_ON_HIGH
#define LED_IS_ON  1
#define LED_IS_OFF 0
#else
#define LED_IS_ON  0
#define LED_IS_OFF 1
#endif

typedef struct
{
    void         *next;     /**< Pointer to next LED descriptor. */
	uint32_t     timOn;     /**< Time the LED is on. */
    uint32_t     timOff;    /**< Time the LED is off. */
	uint32_t     tim;	    /**< Internal current time of a LED  */
    int          bank;      /**< Port where led is connected */
    int          pin;       /**< Pin on port, where led is connected */
	int          state;		/**< Current state */
    uint_fast8_t fx;        /**< Selected function of led */
} LEDEventT;

/* Pointer to first led registered */
LEDEventT *first_led = NULL;

/* Timer- and Timer-Event-Handler */
HANDLE led_tmr = NULL;
HANDLE led_evt = NULL;

/*!
 * \brief Configures LED connection port.
 *
 * This is an internal function called by NutRegisterLED().
 */
int InitLED(LEDEventT *led)
{
#ifdef LED_SUPPORT_IOEXP
    if( led->bank >= IOXP_PORT0) {
        IOExpPinConfigSet(led->bank, led->pin, GPIO_CFG_OUTPUT);
        IOExpSetBitHigh( led->bank, led->pin);
        return 0;
    }
    else
#else
    {
        GpioPinConfigSet( led->bank, led->pin, GPIO_CFG_OUTPUT);
        GpioPinSetHigh( led->bank, led->pin);
        return 0;
    }
#endif
    return -1;
}

/*!
 * \brief Callback function for LED blink and flash timer.
 *
 * This is an internal function of the led driver.
 * It posts the event to the LED thread.
 *
 * \param timer Handle of the elapsed timeout timer.
 * \param arg   Handle of an event queue.
 *
 */
static void LedTimerCb(HANDLE timer, void *arg)
{
    NutEventPostAsync( arg);
}

/*!
 * \brief Thread to control blinking and flashing of all registered LEDs.
 *
 * This is the internal LED handler thread. It si blocked by default, waiting
 * for a timer event to run.
 * The thread handles LED timing through reverse calculation of the actual
 * timeout. This uses mor system time, but makes blinking effects more smooth
 * even in situation where high priority tasks are delaying the timer interrupt
 * or this threadf to be executed.
 *
 * \param arg Handle of an event queue.
 *
 */
/****************************************************************************/
THREAD( sys_led, arg)
/****************************************************************************/
{
    LEDEventT *led;
    uint32_t now, last, dur;

    NUTASSERT( arg != NULL);

    last = NutGetMillis();
    NutThreadSetPriority(16);
    for(;;) {
        if (NutEventWait(arg, NUT_WAIT_INFINITE)==0) {
            now = NutGetMillis();
            dur = now-last;
            last = now;
            led = first_led;

            while( led)
            {
                switch( led->fx) {
                    case LED_ON:
                        if( led->timOn > 0) {
                            if( led->tim >= dur) led->tim -= dur;
                            else
                                NutSetLed( led, LED_OFF, 0, 0);
                        }
                        break;
                    case LED_OFF:
                        if( led->timOff > 0) {
                            if( led->tim >= dur) led->tim -= dur;
                            else
                                NutSetLed( led, LED_ON, 0, 0);
                        }
                        break;
                    case LED_BLINK:
                        if( led->tim >= dur) led->tim -= dur;
                        else {
                            NutSetLed( led, LED_FLIP, 0, 0);
                            led->fx = LED_BLINK;
                            if( led->state)
                                led->tim = led->timOff;
                            else
                                led->tim = led->timOn;
                        }
                        break;
                }

                led = led->next;
            }
        }
    }
}

/*!
 * \brief sets state of a LED
 *
 * \param ledh The handle to the LED that should be controlled.
 * \param fxin Effect to set
 * \param timOn Duration of the LED's on-time of the effect given.
 * \param timOff Duration of the LED's off time of the effect given.
 *
 * \note Timeout is limited to the granularity of the system timer.
 */
void NutSetLed( HANDLE ledh, uint_fast8_t fxin, uint32_t timOn, uint32_t timOff)
{
    LEDEventT *led = (LEDEventT *)ledh;

    NUTASSERT( ledh != NULL);

    led->fx = fxin;

    switch( fxin) {
        case LED_BLINK:
            led->state ^= 1;
            led->timOn = timOn;
            led->timOff = timOff;
            if( led->state==LED_IS_ON)
                led->tim = timOff;
            else
                led->tim = timOn;
            break;
        case LED_FLIP:
            led->state ^= 1;
            break;
        case LED_ON:
            led->state = LED_IS_ON;
            led->timOn = led->tim = timOn;
            break;
        case LED_OFF:
        default:
            led->state = LED_IS_OFF;
            led->timOff = led->tim = timOff;
            break;
    }

#ifdef LED_SUPPORT_IOEXP
    if( led->bank < IOXP_PORT0)
        GpioPinSet( led->bank, led->pin, led->state);
    else
        IOExpSetBit( led->bank, led->pin, led->state);
#else
        GpioPinSet( led->bank, led->pin, led->state);
#endif

}

/*!
 * \brief Register an LED for handling.
 *
 * Register a LED for beeing controlled by this driver. LED can then be
 * set by NutSetLed() by passing over the LEDs handler to that function.
 * Also LEDs connectd through external (I2C / SPI) port chips can be
 * driven, if the control for that chips is enabled in Nutconf.
 * The desired GPIO pin (internal or external) should be configured
 * correct before you can register a LED on it.
 *
 * \param ledh Pointer to a HANDLE for accessing the LED after registering.
 * \param bank Port of CPU or IO-Expander the LED is connected to.
 * \param pin  Pin at the given port.
 *
 * \return -1 if registering failed, else 0.
 *
 */
int NutRegisterLed( HANDLE * ledh, int bank, int pin)
{
    LEDEventT *led;

    /* Check memory constraints and assign memory to new led struct */
    led = malloc(sizeof( LEDEventT));
    *ledh = (void*)led;

    if( led == NULL) {
        return -1;
    }

    /* Preset new led struct */
    memset( led, 0, sizeof( LEDEventT));
    led->bank = bank;
    led->pin = pin;
    led->state = LED_IS_OFF;

    /* Assign the led to the chain */
    NutEnterCritical();
    if( first_led == NULL) {
        /* it is the first led */
        first_led = led;
    }
    else {
        /* if not first, put it into the chain at first position */
        led->next = first_led;
        first_led = led;
    }
    NutExitCritical();

    /* Start timer for LED effects, but only one timer for all */
    if( led_tmr == NULL) {
        NutThreadCreate("sys_led", sys_led, &led_evt, 192);
        led_tmr = NutTimerStart(10, LedTimerCb, &led_evt, 0);
    }

    return InitLED( led);
}

/*@}*/
