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
 *
 * -
 * Portions Copyright (C) 2000 David J. Hudson <dave@humbug.demon.co.uk>
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can redistribute this file and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software Foundation;
 * either version 2 of the License, or (at your discretion) any later version.
 * See the accompanying file "copying-gpl.txt" for more details.
 *
 * As a special exception to the GPL, permission is granted for additional
 * uses of the text contained in this file.  See the accompanying file
 * "copying-liquorice.txt" for details.
 * -
 * Portions Copyright (c) 1983, 1993 by
 *  The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * $Log$
 * Revision 1.15  2009/03/05 22:16:57  freckle
 * use __NUT_EMULATION instead of __APPLE__, __linux__, or __CYGWIN__
 *
 * Revision 1.14  2009/02/13 14:47:12  haraldkipp
 * memory alignment macros added.
 *
 * Revision 1.13  2008/08/11 07:00:28  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.12  2006/10/08 16:48:22  haraldkipp
 * Documentation fixed
 *
 * Revision 1.11  2006/03/27 09:34:05  freckle
 * added u_longlong and longlong for emulation
 *
 * Revision 1.10  2006/03/02 20:04:11  haraldkipp
 * ICCARM doesn't know __PTRDIFF_TYPE__.
 *
 * Revision 1.9  2005/07/26 15:49:59  haraldkipp
 * Cygwin support added.
 *
 * Revision 1.8  2005/04/08 12:46:46  freckle
 * removed htons, htonl, ntohs, ntohs, ntohl,  from unix emulation as
 * provided by system headers somewhere
 *
 * Revision 1.7  2005/02/10 07:06:48  hwmaier
 * Changes to incorporate support for AT90CAN128 CPU
 *
 * Revision 1.6  2004/06/08 15:04:24  freckle
 * changed #include "types_orig.h" to <sys/types_orig.h>
 * to allow this file to be in $MODDIR. Relevant only for *nix emulation
 *
 * Revision 1.5  2004/04/20 07:58:32  drsung
 * Use __GNUC__ instead of __GCC__
 *
 * Revision 1.4  2004/04/07 12:13:57  haraldkipp
 * Matthias Ringwald's *nix emulation added
 *
 * Revision 1.3  2004/03/18 15:40:55  haraldkipp
 * ICCAVR failed to compile
 *
 * Revision 1.2  2004/03/16 16:48:44  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 * Revision 1.1.1.1  2003/05/09 14:41:23  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.8  2003/02/04 18:00:54  harald
 * Version 3 released
 *
 * Revision 1.7  2002/08/08 17:25:12  harald
 * *** empty log message ***
 *
 * Revision 1.6  2002/06/26 17:29:30  harald
 * First pre-release with 2.4 stack
 *
 */

#ifndef _SYS_TYPES_H_

#ifndef _SYS_VIRTUAL_TYPES_H_
#define _SYS_VIRTUAL_TYPES_H_


#ifdef __NUT_EMULATION__
//  on an emulation platform, we need to have both
//              a) the native types headers and libs and
#include <sys/types_orig.h>
//              b) the additional nut os header and implementation

/*! \brief 64-bit values */
typedef unsigned long long u_longlong;
typedef long long longlong;

#endif

#ifndef _SYS_TYPES_H
#define _SYS_TYPES_H_
#endif

/*!
 * \file sys/types.h
 * \brief Nut/OS type declarations.
 */

#include <compiler.h>

__BEGIN_DECLS

// Definition of size_t
#ifdef __cplusplus
#include <cstddef>
#else
#include <stddef.h>
#endif

/*!
 * \weakgroup xgNutOS
 */
/*@{*/

#ifndef NUTMEM_ALIGNMENT
#if defined(__avr__)
#define	NUTMEM_ALIGNMENT        1
#elif defined(__ARM__)
#define	NUTMEM_ALIGNMENT        4
#elif defined(__AVR32__)
#define	NUTMEM_ALIGNMENT        4
#else
#define	NUTMEM_ALIGNMENT        sizeof(int)
#endif
#endif

/*!
 * \brief Return the next lower aligned value.
 */
#define	NUTMEM_BOTTOM_ALIGN(s)  ((s) & ~(NUTMEM_ALIGNMENT - 1))

/*!
 * \brief Return the next higher aligned value.
 */
#define	NUTMEM_TOP_ALIGN(s)     NUTMEM_BOTTOM_ALIGN((s + (NUTMEM_ALIGNMENT - 1)))


#ifndef __NUT_EMULATION__

/*! \brief Unsigned 8-bit value. 
 *
 * \todo We should switch to stdint.h as suggested by Dirk Kaufmann. See
 *       feature request #1282721.
 */
    typedef unsigned char u_char;

/*! \brief Unsigned 16-bit value. */
    typedef unsigned short u_short;

/*! \brief Unsigned int value. */
/* Warning: size is highly architecture/compiler dependant! */
    typedef unsigned int u_int;

/*! \brief Unsigned 32-bit value */
    typedef unsigned long u_long;

/*! \brief Unsigned 64-bit value */
    typedef unsigned long long u_longlong;

#endif                          /* unix emulation */

/*! \brief Void pointer */
typedef void *HANDLE;

/*!
 * \brief Unsigned register type.
 *
 * The size of this type is equal to the size of a register,
 * the hardware datapath or whatever might fit to give optimum
 * performance for values from 0 to 255.
 *
 * Typically 8 bit CPUs will use unsigned characters, 16 bit
 * CPUs will use unsigned shorts etc.
 */
#if defined(__AVR__)
    typedef unsigned char ureg_t;
#elif defined(__arm__)
    typedef unsigned short ureg_t;
#elif defined(__AVR32__)
	typedef unsigned long ureg_t;
#elif defined(__H8300__) || defined(__H8300H__) || defined(__H8300S__)
    typedef unsigned short ureg_t;
#elif defined(__m68k__)
    typedef unsigned short ureg_t;
#else
    typedef unsigned short ureg_t;
#endif

/*!
 * \brief Signed register type.
 *
 * Similar to ureg_t, but for signed values from -128 to +127.
 */
#if defined(__AVR__)
    typedef unsigned char reg_t;
#elif defined(__arm__)
    typedef unsigned short reg_t;
#elif defined(__AVR32__)
	typedef unsigned short reg_t;
#elif defined(__H8300__) || defined(__H8300H__) || defined(__H8300S__)
    typedef unsigned short reg_t;
#elif defined(__m68k__)
    typedef unsigned short reg_t;
#else
    typedef unsigned short reg_t;
#endif

/*!
 * \brief Unsigned pointer value type.
 *
 * The size of this type is at least the size of a memory pointer.
 * For CPUs with 16 address bits this will be an unsigned short.
 */
#if defined(__AVR__)
    typedef unsigned short uptr_t;
#elif defined(__GNUC__)
/*
 * For remaining MCUs GCC is assumed where __PTRDIFF_TYPE__ macro is defined
 */
    typedef unsigned __PTRDIFF_TYPE__ uptr_t;
#else
    typedef unsigned int uptr_t;
#endif

#define __byte_swap2(val)           \
    ((((val) & 0xff) << 8) |        \
     (((val) & 0xff00) >> 8))

#define __byte_swap4(val)           \
    ((((val) & 0xff) << 24) |       \
     (((val) & 0xff00) << 8) |      \
     (((val) & 0xff0000) >> 8) |    \
     (((val) & 0xff000000) >> 24))

#if defined(__GNUC__) && defined(__AVR__)
/*
 * Conversion of 16 bit value to network order.
 */
#undef __byte_swap2
    static inline uint16_t __byte_swap2(uint16_t val) {
        asm volatile ("mov __tmp_reg__, %A0\n\t" "mov %A0, %B0\n\t" "mov %B0, __tmp_reg__\n\t":"=r" (val)
                      :"0"(val)
            );
         return val;
    }
/*
 * Conversion of 32 bit value to network order.
 */
#undef __byte_swap4
    static inline uint32_t __byte_swap4(uint32_t val) {
        asm volatile ("mov __tmp_reg__, %A0\n\t"
                      "mov %A0, %D0\n\t"
                      "mov %D0, __tmp_reg__\n\t" "mov __tmp_reg__, %B0\n\t" "mov %B0, %C0\n\t" "mov %C0, __tmp_reg__\n\t":"=r" (val)
                      :"0"(val)
            );
        return val;
    }
#endif                          /* #if defined(__GCC__) && defined(__AVR__) */


#ifndef __NUT_EMULATION__
	
    /*!
 * \brief Convert short value from host to network byte order.
 */
#ifndef __BIG_ENDIAN__
#define htons(x) __byte_swap2(x)
#else
#define htons(x) (x)
#endif
    
/*!
 * \brief Convert long value from host to network byte order.
 */
#ifndef __BIG_ENDIAN__
#define htonl(x) __byte_swap4(x)
#else
#define htonl(x) (x)
#endif

/*!
 * \brief Convert short value from network to host byte order.
 */
#ifndef __BIG_ENDIAN__
#define ntohs(x) __byte_swap2(x)
#else
#define ntohs(x) (x)
#endif

/*!
 * \brief Convert long value from network to host byte order.
 */
#ifndef __BIG_ENDIAN__
#define ntohl(x) __byte_swap4(x)
#else
#define ntohl(x) (x)
#endif

#endif /* network to host byte conversion */

/*@}*/

__END_DECLS
/* */
#endif                          /* #ifndef _SYS_VIRTUAL_TYPES_H_ */

#endif                          /* #ifndef _SYS_TYPES_H_ */
