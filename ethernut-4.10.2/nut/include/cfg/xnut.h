/*
 * Copyright (c) 2005-2007 proconX Pty Ltd <www.proconx.com>
 *
 * $Id: xnut.h 1928 2007-09-08 01:50:49Z hwmaier $
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
 */


#ifndef _CFG_XNUT_H_
#define _CFG_XNUT_H_

#ifndef _XNUT_XXX_H_INCLUDED
#  define _XNUT_XXX_H_INCLUDED "cfg/xnut.h"
#else
#  error "Attempt to include more than one <xnut-xxx.h> file"
#endif


#include <cfg/os.h>
#include <cfg/memory.h>
#include <cfg/arch.h>
#include <cfg/eeprom.h>
#include <arch/avr.h>
#include <cfg/arch/avrpio.h>

/* Validate F_CPU setting */
#if F_CPU == 1000000UL
#  error "F_CPU is set to avr-lib default 1 MHz! Please include <avr/delay.h> after <xnut.h>"
#endif
#define F_CPU NUT_CPU_FREQ


/*!
 * \defgroup xgXnutCfg XNUT-100 & XNUT-105 DIN-rail SBC
 * \ingroup xgConfigAvr
 * \brief DIN-rail mounted Single Board Computer in enclosure with
 * 2 x RS-232, 2 x RS-485, 1 x RS-422 interfaces (software configurable),
 * CAN port (XNUT-105) and 10-30 V (24 V) switch mode power supply.
 * The <A href="http://www.proconx.com/xnut100">XNUT-100</A> and
 * <A href="http://www.proconx.com/xnut105">XNUT-105</A>
 * modules have been specifically designed for industrial communication
 * tasks such as Monitoring & Controlling serial devices,
 * gathering sensor data, Gateway Applications and Protocol Conversion.
 */
//@{

/*****************************************************************************
 * LED control macros
 *****************************************************************************/

/**
 * @defgroup xnutLed LED Control Macros
 *
 * @brief Functions to drive the XNUT status LED S1 (LED3) and S2 (LED4).
 */
//@{

/** Switch LED S1 off */
#define LED_S1_OFF()         do {PORTF &= ~_BV(2); PORTF &= ~_BV(3);} while(0)
/** Switch LED S1 red */
#define LED_S1_RED()         do {PORTF &= ~_BV(2); PORTF |= _BV(3);} while(0)
/** Toggle LED S1 between red and off */
#define LED_S1_RED_TOGGLE()  do {PORTF &= ~_BV(2); PORTF ^= _BV(3);} while(0)
/** Switch LED S1 green */
#define LED_S1_GREEN()       do {PORTF |= _BV(2); PORTF &= ~_BV(3);} while(0)
/** Toggle LED S1 between green and off */
#define LED_S1_GREEN_TOGGLE() do {PORTF ^= _BV(2); PORTF &= ~_BV(3);} while(0)
#define LED_S1_RED_GREEN_TOGGLE() do {PORTF ^= _BV(2); if (bit_is_set(PORTF, 2)) \
   PORTF &= ~_BV(3); else PORTF |= _BV(3);} while(0)

/** Switch LED S2 off */
#define LED_S2_OFF()         do {PORTF &= ~_BV(0); PORTF &= ~_BV(1);} while(0)
/** Switch LED S2 red */
#define LED_S2_RED()         do {PORTF &= ~_BV(0); PORTF |= _BV(1);} while(0)
/** Toggle LED S2 between red and off */
#define LED_S2_RED_TOGGLE()  do {PORTF &= ~_BV(0); PORTF ^= _BV(1);} while(0)
/** Switch LED S2 green */
#define LED_S2_GREEN()       do {PORTF |= _BV(0); PORTF &= ~_BV(1);} while(0)
/** Toggle LED S2 between green and off */
#define LED_S2_GREEN_TOGGLE() do {PORTF ^= _BV(0); PORTF &= ~_BV(1);} while(0)
#define LED_S2_RED_GREEN_TOGGLE() do {PORTF ^= _BV(0); if (bit_is_set(PORTF, 0)) \
   PORTF &= ~_BV(1); else PORTF |= _BV(1);} while(0)

//@}


/*****************************************************************************
 * UART control macros
 *****************************************************************************/

/**
 * @defgroup xnutSer UART Control Macros
 *
 * @brief Functions to control UART modes
 */
//@{

/**
 * Configure Ser 0 to operate in RS232 mode.
 *
 * In RS232 mode receiption from SUB-D connector J9 is enabled
 * and the receiver on connector J6-2/3 is disabled.
 */
#define SER0_RS232_MODE()    (PORTB |= _BV(0))

/**
 * Configure Ser 0 to operate in RS485 mode.
 *
 * In RS485 mode receiption from connector J6-2/3 is enabled
 * and the receiver on SUB-D connector J9 is disabled.
 */
#define SER0_RS485_MODE()    (PORTB &= ~_BV(0))

/**
 * Switches the RS485 line driver of Ser 0 on.
 *
 * RS485 is a half-duplex link and the line driver must be switched off to
 * allow receiption of data.
 */
#define SER0_RS485_DRV_ON()   (PORTB |= _BV(1))

/**
 * Switches the RS485 line driver of Ser 0 off.
 */
#define SER0_RS485_DRV_OFF()  (PORTB &= ~_BV(1))

/**
 * Configure Ser 1 to operate in RS232 mode.
 *
 * In RS232 mode receiption from SUB-D connector J7 is enabled
 * and the receiver on connector J6-5/6 is disabled.
 */
#define SER1_RS232_MODE()    (PORTB |= _BV(2))

/**
 * Configure Ser 1 to operate in RS485 mode.
 *
 * In RS485 mode receiption from connector J6-5/6 is enabled
 * and the receiver on SUB-D connector J7 is disabled.
 */
#define SER1_RS485_MODE()    (PORTB &= ~_BV(2))

/**
 * Switches the RS485 line driver of Ser 1 on.
 *
 * RS485 is a half-duplex link and the line driver must be switched off to
 * allow receiption of data.
 */
#define SER1_RS485_DRV_ON()   (PORTB |= _BV(3))

/**
 * Switches the RS485 line driver of Ser 1 off.
 */
#define SER1_RS485_DRV_OFF()  (PORTB &= ~_BV(3))

/**
 * Returns the status of the RS232 CD signal input
 * of Ser 0 on SUB-D connector J9
 *
 * @return 1 if CD is asserted
 */
#define SER0_GET_CD()        bit_is_set(PINB, 6)

/**
 * Returns the status of the RS232 RI signal input
 * of Ser 0 on SUB-D connector J9
 *
 * @return 1 if RI is asserted
 */
#define SER0_GET_RI()        bit_is_clear(PINB, 7)

/**
 * Returns the status of the RS232 CTS signal input
 * of Ser 0 on SUB-D connector J9
 *
 * @return 1 if CTS is asserted
 */
#define SER0_GET_CTS()       bit_is_clear(PINE, 6)

/**
 * Returns the status of the RS232 DSR signal input of Ser 0 on
 * SUB-D connector J9
 * 
 * @note Feature only available for devices with serial number
 *       224 upwards (since PCB Revision C)
 * @return 1 if DSR is asserted
 */
#define SER0_GET_DSR()       bit_is_clear(PINE, 4)


/**
 * Asserts the RS232 RTS signal output of Ser 0 on SUB-D connector J9
 */
#define SER0_SET_RTS()       (PORTB &= ~_BV(4))

/**
 * Clears the RS232 RTS signal output of Ser 0 on SUB-D connector J9
 */
#define SER0_CLR_RTS()       (PORTB |= _BV(4))

/**
 * Asserts the RS232 DTR signal output of Ser 0 on SUB-D
 * connector J9
 * 
 * @note Feature only available for devices with serial number
 *       224 upwards (since PCB Revision C)
 */
#define SER0_SET_DTR()       (PORTB &= ~_BV(5))

/**
 * Clears the RS232 DTR signal output of Ser 0 on SUB-D
 * connector J9
 * 
 * @note Feature only available for devices with serial number
 *       224 upwards (since PCB Revision C)
 */
#define SER0_CLR_DTR()       (PORTB |= _BV(5))

//@}

//@}

#endif // ifdef ..._H_INCLUDED

