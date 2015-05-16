#ifndef _ARCH_AVR_ICC_H_
#define _ARCH_AVR_ICC_H_

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
 * Revision 1.11  2009/02/06 15:58:08  haraldkipp
 * We now have strdup() and calloc().
 *
 * Revision 1.10  2008/09/18 09:48:07  haraldkipp
 * The old Marv_XXX do no longer work with ICCAVR 7.18B.
 *
 * Revision 1.9  2008/08/11 11:51:20  thiagocorrea
 * Preliminary Atmega2560 compile options, but not yet supported.
 * It builds, but doesn't seam to run properly at this time.
 *
 * Revision 1.8  2008/08/11 06:59:58  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.7  2008/06/25 08:37:56  freckle
 * escape realloc for icc
 *
 * Revision 1.6  2006/09/29 12:22:07  haraldkipp
 * Corrected internal RAM size and check compiler environment for ATmega2561.
 *
 * Revision 1.5  2006/07/10 08:48:03  haraldkipp
 * Distinguish between enhanced and extended AVR.
 *
 * Revision 1.4  2006/05/25 09:35:27  haraldkipp
 * Dummy macros added to support the avr-libc special function register
 * definitions.
 *
 * Revision 1.3  2006/02/08 15:20:56  haraldkipp
 * ATmega2561 Support
 *
 * Revision 1.2  2005/10/24 18:04:25  haraldkipp
 * Duplicate macro definitions removed.
 * Parameter order of cstrcmp() corrected.
 * Generic port and flag names added for ATmega103 support.
 *
 * Revision 1.1  2005/10/24 10:46:05  haraldkipp
 * First check in.
 * Contents taken from avr.h in the parent directory.
 *
 */

/*
 * Include some basic header files of the ImageCraft runtime library.
 */
#include <stddef.h>
#include <macros.h>
#include <eeprom.h>

/* ICCV7 incompatibility nightmare. */
#if defined(_MCU_Enhanced) && !defined(_MCU_enhanced)
#define _MCU_enhanced
#endif

#if defined(_MCU_Extended) && !defined(_MCU_extended)
#define _MCU_extended
#endif

/*!
 * \brief Specify enhanced AVR target.
 *
 * For backward compatibility this macro is automatically defined if 
 * _MCU_enhanced is defined.
 *
 */
#if defined(_MCU_enhanced) || defined(_MCU_extended)
#undef __AVR_ENHANCED__
#define __AVR_ENHANCED__ /* Generic test for enhanced AVRs like ATMEGA128, AT09CAN128 */
#endif

/*!
 * \brief Disable const keyword.
 *
 * ICCAVR doesn't allow to mark function parameters unmodifable
 * by the 'const' modifier. Even worse, the compiler redefines
 * the meaning as a program memory attribute.
 */
#ifndef CONST
#define CONST   const
#endif

/*!
 * \brief Disable inline keyword.
 *
 * ICCAVR doesn't allow to explicitly mark a function for inline
 * code generation.
 */
#ifndef INLINE
#define INLINE
#endif

/*!
 * \brief Redefined standard library routines.
 *
 * ImageCraft has a multipass linker, which is fine for complicated 
 * dependencies in most cases. However, there is the potential risk,
 * that standard library calls with the same name are linked from
 * the wrong library. To avoid this, an additional postfix is added
 * to routines, which are implemented in Nut/OS libraries.
 */
#define printf      printf_M
#define puts        puts_M
#define sprintf     sprintf_M
#define vprintf     vprintf_M
#define scanf       scanf_M
#define gets        gets_M
#define malloc      malloc_M
#define calloc      calloc_M
#define realloc     realloc_M
#define free        free_M

/*!
 * \brief Redirected stdio routines.
 *
 * Native stdio routines with format strings in program space are
 * redirected to their Nut/OS implementation.
 */
#define cprintf     printf_P
#define csprintf    sprintf_P
#define cscanf      scanf_P
#define csscanf     sscanf_P

#define memcpy_P(dst, src_P, n) cmemcpy(dst, src_P, n)
#define strcat_P(s1, s2_P)      cstrcat(s1, s2_P)
#define strcmp_P(s1, s2_P)      (-cstrcmp(s2_P, s1))
#define strlen_P(s_P)           cstrlen(s_P)
#define strncat_P(s1, s2_P, n)  cstrncat(s1, s2_P, n)
#define strncmp_P(s1_P, s2, n)  cstrncmp(s1_P, s2, n)
#define strcpy_P(dst, src_P)    cstrcpy(dst, src_P)
#define strncpy_P(x,y,z)        cstrncpy(x,y,z)

/*!
 * \brief Case insensitive string comparisions.
 *
 * Not supported by ICCAVR and temporarly redirected to
 * the case sensitive routines.
 *
 * \bug Case insensitive string comparisions fail with ICCAVR.
 */
#define strcasecmp(s1, s2)  strcmp(s1, s2)
#define strncasecmp(s1, s2, n)  strncmp(s1, s2, n)


/*!
 * \brief Start of heap area.
 */
#define __heap_start   _bss_end

/*!
 * \brief Object attribute support.
 *
 * Not supported by ICCAVR.
 */
#define __attribute__(x)

/*!
 * \brief Declare static pointer to strings in program space.
 *
 * No chance with ICCAVR.
 */
#define PSTR(p)     (p)

/*! \def PRG_RDB(p)
 * \brief Read byte from program space.
 */
#define PRG_RDB(p)  (*((__flash char *)(p)))

/*! \def prog_char
 * \brief Character in program space.
 */
#define prog_char __flash char

/*! \def prog_int
 * \brief Integer in program space.
 */
#define prog_int __flash int

/*! \def PGM_P
 * \brief Pointer to character in program space.
 */
#define PGM_P prog_char *


/* ================================================================ */
/* To be sorted out.                                                */
/* ================================================================ */

#define wdt_enable(tmo) \
{ \
    register unsigned char s = _BV(WDCE) | _BV(WDE); \
    register unsigned char r = tmo | _BV(WDE); \
    asm("in R0, 0x3F\n"     \
        "cli\n"             \
        "wdr\n"             \
        "out 0x21, %s\n"    \
        "out 0x21, %r\n"    \
        "out 0x3F, R0\n");  \
}

#define wdt_disable() \
{ \
    register unsigned char s = _BV(WDCE) | _BV(WDE); \
    register unsigned char r = 0;  \
    asm("in R0, $3F\n"      \
        "cli\n"             \
        "out 0x21, %s\n"    \
        "out 0x21, %r\n"    \
        "out 0x3F, R0\n");  \
}

#define wdt_reset() \
{ \
    _WDR(); \
}


#define __SFR_OFFSET 0
#define SFR_IO_ADDR(sfr) ((sfr) - __SFR_OFFSET)
#define SFR_MEM_ADDR(sfr) (sfr)
#define SFR_IO_REG_P(sfr) ((sfr) < 0x40 + __SFR_OFFSET)

#define _SFR_MEM8(addr)     (addr)
#define _SFR_MEM16(addr)    (addr)

#define BV(x)       BIT(x)
#define _BV(x)      BIT(x)

#define cli()           CLI()
#define sei()           SEI()
#define cbi(reg, bit)   (reg &= ~BIT(bit))
#define sbi(reg, bit)   (reg |= BIT(bit))


#define loop_until_bit_is_set(reg, bit) while((reg & BIT(bit)) == 0)

#define bit_is_clear(reg, bit)  ((reg & BIT(bit)) == 0)
#define bit_is_set(reg, bit)    ((reg & BIT(bit)) != 0)

/* FIXME */
#define parity_even_bit(x)  (0)

/* FIXME */
#define SIGNAL(x)   void x(void)

#define outp(val, reg)  (reg = val)
#define outb(reg, val)  (reg = val)

#define inp(reg)        (reg)
#define inb(reg)        (reg)

#include <eeprom.h>

#if defined(_MCU_enhanced) || defined(_MCU_extended)

#ifdef ATMega2560
#include <iom2560v.h>
#define __AVR_ATmega2560__
#ifndef _EE_EXTIO
#error "Looks like wrong platform. Select avrext-icc, not avr-icc."
#endif
#elif defined(ATMega2561)
#include <iom2561v.h>
#define __AVR_ATmega2561__
#ifndef _EE_EXTIO
#error "Looks like wrong platform. Select avrext-icc, not avr-icc."
#endif
#else
#include <iom128v.h>
#define __AVR_ATmega128__
#endif

#ifndef RAMEND
#if defined(ATMega2560) || defined(ATMega2561)
#define RAMEND  0x21FF
#else
#define RAMEND  0x10FF
#endif
#endif

#ifndef SRW
#define SRW  6
#endif

/* ICC doesn't define generic ports and flags. */
#ifndef TXC
#define TXC     TXC0
#endif
#ifndef ADCSR
#define ADCSR   ADCSRA
#endif

/* Master */
#define TW_START		    0x08
#define TW_REP_START		0x10
/* Master Transmitter */
#define TW_MT_SLA_ACK		0x18
#define TW_MT_SLA_NACK		0x20
#define TW_MT_DATA_ACK		0x28
#define TW_MT_DATA_NACK		0x30
#define TW_MT_ARB_LOST      0x38
/* Master Receiver */
#define TW_MR_ARB_LOST      0x38
#define TW_MR_SLA_ACK		0x40
#define TW_MR_SLA_NACK		0x48
#define TW_MR_DATA_ACK		0x50
#define TW_MR_DATA_NACK		0x58
/* Slave Transmitter */
#define TW_ST_SLA_ACK		0xA8
#define TW_ST_ARB_LOST_SLA_ACK	0xB0
#define TW_ST_DATA_ACK		0xB8
#define TW_ST_DATA_NACK		0xC0
#define TW_ST_LAST_DATA		0xC8
/* Slave Receiver */
#define TW_SR_SLA_ACK		0x60
#define TW_SR_ARB_LOST_SLA_ACK	0x68
#define TW_SR_GCALL_ACK		0x70
#define TW_SR_ARB_LOST_GCALL_ACK 0x78
#define TW_SR_DATA_ACK		0x80
#define TW_SR_DATA_NACK		0x88
#define TW_SR_GCALL_DATA_ACK	0x90
#define TW_SR_GCALL_DATA_NACK	0x98
#define TW_SR_STOP		0xA0
/* Misc */
#define TW_NO_INFO		0xF8
#define TW_BUS_ERROR		0x00


#else                           /* ATmega103 */

#include <iom103v.h>
#define __AVR_ATmega103__

#ifndef DOR
#define DOR  OVR
#endif

#ifndef RAMEND
#define RAMEND  0x0FFF
#endif

#ifndef WDCE
#define WDCE    WDTOE
#endif

#endif

#define eeprom_read_block(dst, addr, size)  EEPROMReadBytes((int)addr, dst, size)
#define eeprom_write_byte(addr, src)        EEPROMwrite((int)addr, src)
#define eeprom_read_byte(addr)              EEPROMread((int)addr)

extern void *calloc(size_t num, size_t size);
extern char *strdup(CONST char *str);

#include <string.h>


#endif /* _ARCH_AVR_ICC_H_ */


