#ifndef _ARCH_AVR_GCC_H_
#define _ARCH_AVR_GCC_H_

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
 * Revision 1.6  2009/02/06 15:45:04  haraldkipp
 * Routines using heap memory moved from c to crt module.
 * We now have strdup() and calloc().
 *
 * Revision 1.5  2008/08/11 11:51:20  thiagocorrea
 * Preliminary Atmega2560 compile options, but not yet supported.
 * It builds, but doesn't seam to run properly at this time.
 *
 * Revision 1.4  2008/06/16 13:03:37  haraldkipp
 * Added Thiago's patch to fix _SLEEP_MODE_MASK problem, which is no longer
 * public in the latest avrlibc.
 *
 * Revision 1.3  2007/04/12 09:20:34  haraldkipp
 * Added new register bit names for the ATmega2561.
 *
 * Revision 1.2  2006/01/11 08:32:21  hwmaier
 * Support for avr-libc >= 1.4.x
 *
 * Revision 1.1  2005/10/24 10:46:05  haraldkipp
 * First check in.
 * Contents taken from avr.h in the parent directory.
 *
 */


/* ================================================================ */
/* To be sorted out.                                                */
/* ================================================================ */


#define CONST   const
#define INLINE  inline


#include <avr/io.h>
#include <avr/interrupt.h>
#if __AVR_LIBC_VERSION__ < 10400UL
#include <avr/signal.h>
#endif
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <stdlib.h>


/*
 * test for a macro added in avr-libc 1.2.0, if yes use different path for twi.h
 * note: has to be after #include <eeprom.h>
 */
#ifdef eeprom_rb
#include <avr/twi.h>
#else
#include <compat/twi.h>
#endif

#if defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__)
#if !defined(TXC)
#define TXC     TXC0
#endif
#if !defined(TXB8)
#define TXB8     TXB80
#endif
#if !defined(UMSEL)
#define UMSEL     UMSEL00
#endif
#if !defined(U2X)
#define U2X     U2X0
#endif
#if !defined(UCSZ0)
#define UCSZ0     UCSZ00
#endif
#if !defined(UCSZ1)
#define UCSZ1     UCSZ01
#endif
#if !defined(UCSZ2)
#define UCSZ2     UCSZ02
#endif
#if !defined(UPM0)
#define UPM0     UPM00
#endif
#if !defined(UPM1)
#define UPM1     UPM01
#endif

#if !defined(USBS)
#define USBS     USBS0
#endif
#if !defined(UPE)
#define UPE     UPE0
#endif
#if !defined(MPCM)
#define MPCM     MPCM0
#endif
#if !defined(UCPOL)
#define UCPOL     UCPOL0
#endif
#endif

#ifndef __SFR_OFFSET
#define __SFR_OFFSET    0
#endif

#define COMPRESS_DISABLE
#define COMPRESS_REENABLE

#ifndef _NOP
#define _NOP() __asm__ __volatile__ ("nop")
#endif

#ifndef atof
#define atof(s)	    strtod(s, 0)
#endif

#define EEPROMReadBytes(addr, ptr, size)    eeprom_read_block((char *)(addr), ptr, size)
/*!
 * \brief Read multibyte types from the EEPROM.
 */
#define EEPROM_READ(addr, dst)		    eeprom_read_block((char *)(addr), &dst, sizeof(dst))
#define EEPROMread(addr)	 	    eeprom_read_byte((char *)(addr))

/*!
 * \brief Write multibyte types to the EEPROM.
 */
#define EEPROM_WRITE(addr, src)							\
{										\
    unsigned short __i;								\
    for(__i = 0; __i < sizeof(src); __i++)					\
	eeprom_write_byte(((char *)(addr)) + __i, *(((char *)(&(src))) + __i)); \
}

#define EEPROMWriteBytes(addr, ptr, size)					\
{										\
    unsigned short __i;								\
    for(__i = 0; __i < size; __i++)						\
	eeprom_write_byte(((char *)(addr)) + __i, *(((char *)(ptr)) + __i));	\
}

#define main    NutAppMain

/* Define internal _SLEEP_MODE_MASK that is no longer public in avrlibc. */
#ifndef _SLEEP_MODE_MASK
#if defined(SM) && !defined(SM0) && !defined(SM1) && !defined(SM2)
#define _SLEEP_MODE_MASK _BV(SM)
#elif !defined(SM) && defined(SM0) && defined(SM1) && !defined(SM2)
#define _SLEEP_MODE_MASK (_BV(SM0) | _BV(SM1))
#elif !defined(SM) && defined(SM0) && defined(SM1) && defined(SM2)
#define _SLEEP_MODE_MASK (_BV(SM0) | _BV(SM1) | _BV(SM2))
#endif
#endif

#if defined(__AVR_LIBC_VERSION__)
extern void *calloc(size_t num, size_t size);
extern char *strdup(CONST char *str);
#endif

#endif /* _ARCH_AVR_GCC_H_ */

