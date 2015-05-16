#ifndef _ARCH_H8_H_
#define _ARCH_H8_H_

/*
 * Copyright (C) 2004 by Jan Dubiec. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY JAN DUBIEC AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL JAN DUBIEC
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * $Log$
 * Revision 1.1  2004/03/16 16:48:28  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 */

/*
 * GCC for H8 microcontrolles defines following macros:
 *      __H8300H__ when -mh switch is specified in the command line,
 *      __H8300S__ when -ms switch is specified in the command line,
 *      __H8300__ when none of above switches is specified.
 *
 */

#if !defined(__H8300H__) && !defined(__H8300S__)
#error "Nut/OS currently runs only on H8/300H or H8S microcontrollers."
#endif

/* H8/300 is big endian core */
#if !defined(__BIG_ENDIAN__)
#define __BIG_ENDIAN__
#endif

#include <dev/mweeprom.h>

#define CONST      const
#define INLINE     inline

#define PSTR(p)    (p)
#define PRG_RDB(p) (*((const char *)(p)))

#define prog_char  const char
#define PGM_P      prog_char *

#define SIGNAL(x)  __attribute__((interrupt_handler)) void x(void)

#define main       NutAppMain

#define strlen_P(x)             strlen(x)
#define strcpy_P(x,y)           strcpy(x,y)
#define strcmp_P(x, y)          strcmp(x, y)
#define memcpy_P(x, y, z)       memcpy(x, y, z)
#define fputs_P(x, y)           fputs(x, y)
#define fprintf_P               fprintf

/*!
 * \brief End of uninitialised data segment. Defined in the linker script.
 */
extern void *__bss_end;

/*!
 * \brief Begin of the stack segment. Defined in the linker script.
 */
extern void *__stack;

#define RAMSTART    ((void *)0xffbf20)

/* #define RAMEND  (0xffbf20 + 0x4000) */

#endif                          /* #ifndef _CPU_H8_H_ */
