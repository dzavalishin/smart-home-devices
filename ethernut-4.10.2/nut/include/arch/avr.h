#ifndef _ARCH_AVR_H_
#define _ARCH_AVR_H_

/*
 * Copyright (C) 2001-2005 by egnite Software GmbH. All rights reserved.
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

/*
 * $Log$
 * Revision 1.15  2008/08/11 11:51:20  thiagocorrea
 * Preliminary Atmega2560 compile options, but not yet supported.
 * It builds, but doesn't seam to run properly at this time.
 *
 * Revision 1.14  2006/02/08 15:20:22  haraldkipp
 * ATmega2561 Support
 *
 * Revision 1.13  2005/10/24 10:42:48  haraldkipp
 * Definitions distributed to avr/icc.h and avr/gcc.h.
 *
 * Revision 1.12  2005/10/04 05:21:52  hwmaier
 * Added TIFR definition for AT09CAN128
 *
 * Revision 1.11  2005/08/02 17:46:48  haraldkipp
 * Major API documentation update.
 *
 * Revision 1.10  2005/02/22 17:03:02  freckle
 * changed avr-libc-1.2 test to use eeprom_rb, as other test was wrong on
 * 1.0.5
 *
 * Revision 1.9  2005/02/22 16:22:21  freckle
 * Added cpp test to guess avr-libc-version required  to specify twi.h path
 *
 * Revision 1.8  2005/02/10 07:06:48  hwmaier
 * Changes to incorporate support for AT90CAN128 CPU
 *
 * Revision 1.7  2005/01/10 12:40:15  olereinhardt
 * Included check if atof is just defined (needed by new avr-libc versions on debian unstable)
 *
 * Revision 1.6  2004/07/09 19:51:34  freckle
 * Added new function NutThreadSetSleepMode to tell nut/os to set the MCU
 * into sleep mode when idle (avr-gcc && avr128 only)
 *
 * Revision 1.5  2004/05/23 14:30:32  drsung
 * Added some macros, because they are no longer available since version 1.1.0 of avr-libc.
 *
 * Revision 1.4  2004/03/18 15:53:42  haraldkipp
 * ICCAVR failed to compile
 *
 * Revision 1.3  2004/03/18 09:57:01  haraldkipp
 * Architecture required in UserConf.mk
 *
 * Revision 1.2  2004/03/17 14:54:00  haraldkipp
 * Compiling for AVR works again
 *
 * Revision 1.1  2004/03/16 16:48:28  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 * Revision 1.1  2004/02/01 18:49:47  haraldkipp
 * Added CPU family support
 *
 */

#if defined(__IMAGECRAFT__)
#include <arch/avr/icc.h>
#elif defined(__GNUC__)
#include <arch/avr/gcc.h>
#else
#error "Unable to determine AVR compiler."
#endif

/*!
 * \brief Specify AVR target.
 *
 * Only GCC provides this as a predefined macro. Nut/OS explicitly
 * re-defines this, so that it will be available for all compilers.
 */
#undef __AVR__
#define __AVR__


/*
 * Since version 1.1.0 of avr-libc, some former deprecated macros are deleted.
 * But we need them futher on, so they are defined here.
 */
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif
#ifndef inb
#define inb(sfr) _SFR_BYTE(sfr)
#endif
#ifndef outb
#define outb(sfr, val) (_SFR_BYTE(sfr) = (val))
#endif
#ifndef outp
#define outp(val, sfr) outb(sfr, val)
#endif
#ifndef inp
#define inp(sfr) inb(sfr)
#endif
#ifndef BV
#define BV(bit) _BV(bit)
#endif
#ifndef inw
#define inw(sfr) _SFR_WORD(sfr)
#endif
#ifndef outw
#define outw(sfr, val) (_SFR_WORD(sfr) = (val))
#endif
#ifndef PRG_RDB
#define PRG_RDB(addr)       pgm_read_byte(addr)
#endif

#define __bss_end	__heap_start
extern void *__heap_start;

#ifdef __AVR_ENHANCED__

/* Nut/OS is still using the original ATmega103 register names for
   backward compatibility. */
#ifndef UDR
#define UDR     UDR0
#endif
#ifndef UBRR
#define UBRR    UBRR0L
#endif
#ifndef USR
#define USR     UCSR0A
#endif
#ifndef UCR
#define UCR     UCSR0B
#endif
#ifndef EICR
#define EICR    EICRB
#endif
#ifndef RXC
#define RXC     RXC0
#endif
#ifndef UDRE
#define UDRE    UDRE0
#endif
#ifndef FE
#define FE      FE0
#endif
#ifndef DOR
#define DOR     DOR0
#endif
#ifndef RXCIE
#define RXCIE   RXCIE0
#endif
#ifndef TXCIE
#define TXCIE   TXCIE0
#endif
#ifndef UDRIE
#define UDRIE   UDRIE0
#endif
#ifndef RXEN
#define RXEN    RXEN0
#endif
#ifndef TXEN
#define TXEN    TXEN0
#endif

/* Some ATC90CAN128 SFR names are different to ATMEGA128. Define some
   compatibilty macros. */
#if defined(__AVR_AT90CAN128__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__)
#ifndef ADCW
#define ADCW    ADC
#endif
#ifndef ADCSR
#define ADCSR   ADCSRA
#endif
#ifndef ADFR
#define ADFR    ADATE
#endif
#ifndef OCIE0
#define OCIE0   OCIE0A
#endif
#ifndef TCCR0
#define TCCR0   TCCR0A
#endif
#ifndef TCCR2
#define TCCR2   TCCR2A
#endif
#ifndef OCR0
#define OCR0    OCR0A
#endif
#ifndef TIMSK
#define TIMSK   TIMSK0
#endif
#ifndef TIFR
#define TIFR   TIFR0
#endif
#endif /* __AVR_AT90CAN128__ || __AVR_ATmega2560__ || __AVR_ATmega2561__*/


#endif /* __AVR_ENHANCED__ */


#endif /* _ARCH_AVR_H_ */
