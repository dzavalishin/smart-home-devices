#ifndef _ARCH_ARM_H_
#define _ARCH_ARM_H_

/*
 * Copyright (C) 2001-2006 by egnite Software GmbH. All rights reserved.
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
 * Revision 1.22  2009/02/17 09:34:34  haraldkipp
 * Added inline assembler macros for reading/writing coprocessor CP15
 * control register.
 *
 * Revision 1.21  2009/01/16 19:45:42  haraldkipp
 * All ARM code is now running in system mode.
 *
 * Revision 1.20  2008/08/06 12:51:09  haraldkipp
 * Added support for Ethernut 5 (AT91SAM9XE reference design).
 *
 * Revision 1.19  2008/02/15 17:00:24  haraldkipp
 * Spport for AT91SAM7SE512 added.
 *
 * Revision 1.18  2007/10/04 20:29:08  olereinhardt
 * Support for SAM7S256 added
 *
 * Revision 1.17  2007/05/02 11:32:07  haraldkipp
 * Mapping of Harvard specific stdio functions moved to stdio.h and io.h.
 *
 * Revision 1.16  2006/08/31 19:04:08  haraldkipp
 * Added support for the AT91SAM9260 and Atmel's AT91SAM9260 Evaluation Kit.
 *
 * Revision 1.15  2006/08/05 11:58:22  haraldkipp
 * Missing brackets may result in unexpected expansion of the _BV() macro.
 *
 * Revision 1.14  2006/08/01 07:35:59  haraldkipp
 * Exclude function prototypes when included by assembler.
 *
 * Revision 1.13  2006/07/21 09:08:58  haraldkipp
 * Map puts_P to puts and _write_P to _write for non-Harvard architectures.
 *
 * Revision 1.12  2006/07/10 14:27:03  haraldkipp
 * C++ will use main instead of NutAppMain. Contributed by Matthias Wilde.
 *
 * Revision 1.11  2006/07/05 07:45:25  haraldkipp
 * Split on-chip interface definitions.
 *
 * Revision 1.10  2006/06/28 17:22:34  haraldkipp
 * Make it compile for AT91SAM7X256.
 *
 * Revision 1.9  2006/05/25 09:35:27  haraldkipp
 * Dummy macros added to support the avr-libc special function register
 * definitions.
 *
 * Revision 1.8  2006/03/16 15:25:26  haraldkipp
 * Changed human readable strings from u_char to char to stop GCC 4 from
 * nagging about signedness.
 *
 * Revision 1.7  2006/03/02 20:02:05  haraldkipp
 * Added a few macros to allow compilation with ICCARM.
 *
 * Revision 1.6  2006/02/23 15:34:00  haraldkipp
 * Support for Philips LPC2xxx Family and LPC-E2294 Board from Olimex added.
 * Many thanks to Michael Fischer for this port.
 *
 * Revision 1.5  2005/11/20 14:45:15  haraldkipp
 * Define printf_P for non Harvard architectures.
 *
 * Revision 1.4  2005/10/24 18:03:02  haraldkipp
 * GameBoy header file added.
 *
 * Revision 1.3  2005/10/24 10:35:05  haraldkipp
 * Port I/O macros added.
 *
 * Revision 1.2  2004/09/08 10:24:26  haraldkipp
 * RAMSTART is too platform dependant
 *
 * Revision 1.1  2004/03/16 16:48:28  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 * Revision 1.1  2004/02/01 18:49:47  haraldkipp
 * Added CPU family support
 *
 */

#include <cfg/arch.h>
#if defined (MCU_AT91)
#include <arch/arm/at91.h>
#elif defined (MCU_GBA)
#include <arch/arm/gba.h>
#elif defined (MCU_LPC2XXX)
#include <arch/arm/lpc2xxx.h>
#endif

#ifndef __ASSEMBLER__
#include <dev/mweeprom.h>
#endif

#define ARM_MODE_USER       0x10
#define ARM_MODE_FIQ        0x11
#define ARM_MODE_IRQ        0x12
#define ARM_MODE_SVC        0x13
#define ARM_MODE_ABORT      0x17
#define ARM_MODE_UNDEF      0x1B
#define ARM_MODE_SYS        0x1F
#define ARM_MODE_MASK       0x1F

#define I_BIT               0x80
#define ARM_CPSR_I_BIT      0x80
#define F_BIT               0x40
#define ARM_CPSR_F_BIT      0x40
#define T_BIT               0x20
#define ARM_CPSR_T_BIT      0x20

#ifdef __GNUC__
#define CONST      const
#define INLINE     inline
#else
#ifndef CONST
#define CONST      const
#endif
#ifndef INLINE
#define INLINE
#endif
#endif

#define PSTR(p)    (p)
#define PRG_RDB(p) (*((const char *)(p)))

#define prog_char  const char
#define PGM_P      prog_char *

#define SIGNAL(x)  __attribute__((interrupt_handler)) void x(void)
#define RAMFUNC __attribute__ ((long_call, section (".ramfunc")))

#if !defined(__arm__) && !defined(__cplusplus)
#define main       NutAppMain
#endif

#define strlen_P(x)             strlen((char *)(x))
#define strcpy_P(x,y)           strcpy(x,(char *)(y))
#define strcat_P(x,y)           strcat(x,(char *)(y))

#define strcmp_P(x, y)          strcmp((char *)(x), (char *)(y))
#define memcpy_P(x, y, z)       memcpy(x, y, z)

#ifndef __ASSEMBLER__
/*!
 * \brief End of uninitialised data segment. Defined in the linker script.
 */
extern void *__bss_end;

/*!
 * \brief Begin of the stack segment. Defined in the linker script.
 */
extern void *__stack;
#endif

#ifndef _NOP
#ifdef __GNUC__
#define _NOP() __asm__ __volatile__ ("mov r0, r0  @ _NOP")
#else
#define _NOP() asm("mov r0, r0")
#endif
#endif

#define outb(_reg, _val)  (*((volatile unsigned char *)(_reg)) = (_val))
#define outw(_reg, _val)  (*((volatile unsigned short *)(_reg)) = (_val))
#define outr(_reg, _val)  (*((volatile unsigned int *)(_reg)) = (_val))

#define inb(_reg)   (*((volatile unsigned char *)(_reg)))
#define inw(_reg)   (*((volatile unsigned short *)(_reg)))
#define inr(_reg)   (*((volatile unsigned int *)(_reg)))

#define _BV(bit)    (1 << (bit))

#define sbi(_reg, _bit)         outr(_reg, inr(_reg) | _BV(_bit))
#define cbi(_reg, _bit)         outr(_reg, inr(_reg) & ~_BV(_bit))
#define bit_is_set(_reg, _bit)  ((inr(_reg) & _BV(_bit)) != 0)

#ifdef __IMAGECRAFT__
#define __attribute__(x)
#endif

#define _SFR_MEM8(addr)     (addr)
#define _SFR_MEM16(addr)    (addr)

#ifndef __ASSEMBLER__
#ifdef __GNUC__
#define ARM_SET_CP15_CR(val) __asm__ __volatile__("mcr p15, 0, %0, c1, c0, 0" :: "r"(val) : "cc")
#define ARM_GET_CP15_CR() ( \
    { \
        unsigned int val; \
        __asm__ __volatile__("mrc p15, 0, %0, c1, c0, 0" : "=r"(val) :: "cc"); \
        val; \
    } \
)
#endif /* __GNUC__ */
#endif /* __ASSEMBLER__ */

#if !defined (__ASSEMBLER__) && defined(__CROSSWORKS_ARM)
/*!
 * \brief Case insensitive string comparisions.
 *
 * Not supported by CrossWorks and temporarly redirected 
 * to the stricmp and strnicmp routines.
 *
 */
#define strcasecmp(s1, s2)      stricmp(s1, s2)
#define strncasecmp(s1, s2, n)  strnicmp(s1, s2, n)

/*
 * Not supported by CrossWorks, added prototypes here.
 */
int   stricmp(CONST char *s1, CONST char *s2);
int   strnicmp(CONST char *s1, CONST char *s2, size_t n);
char *strdup(CONST char *str);

/*
 * If "Enforce ANSI Checking" is enabled, which is
 * the default from the v2.x version of CrossWorks
 * the keyword asm will not be recognize. Therefore
 * the next define is needed to solve the problem.
 */
#define asm __asm__
#endif

#endif
