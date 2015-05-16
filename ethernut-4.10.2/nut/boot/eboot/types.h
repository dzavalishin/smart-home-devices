#ifndef _SYS_TYPES_H_
#define _SYS_TYPES_H_

/*
 * Copyright (C) 1983, 1993 by The Regents of the University of California
 * Copyright (C) 2001-2004 by egnite Software GmbH
 * Copyright (C) 2010 by egnite GmbH
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

/*
 * $Id: types.h 3006 2010-05-10 12:39:44Z haraldkipp $
 */

/*!
 * \brief Nut/OS type declarations.
 */

/*!
 * \weakgroup xgEBoot
 */
/*@{*/

/*! \brief Unsigned 8-bit value. */
typedef unsigned char      u_char;
/*! \brief Unsigned 16-bit value. */
typedef unsigned short     u_short;
/*! \brief Unsigned 16-bit value. */
typedef unsigned int       u_int;
/*! \brief Unsigned 32-bit value */
typedef unsigned long      u_long;
/*! \brief Unsigned 64-bit value */
typedef unsigned long long u_longlong;
/*! \brief Void pointer */
typedef void * HANDLE;


/*
 * Conversion of 16 bit value to network order.
 */
static inline u_short __byte_swap2(u_short val)
{
    asm volatile(
        "mov __tmp_reg__, %A0\n\t"
        "mov %A0, %B0\n\t"
        "mov %B0, __tmp_reg__\n\t"
        : "=r" (val)
        : "0" (val)
    );
    return val;
}

/*
 * Conversion of 32 bit value to network order.
 */
static inline u_long __byte_swap4(u_long val)
{
    asm volatile(
        "mov __tmp_reg__, %A0\n\t"
        "mov %A0, %D0\n\t"
        "mov %D0, __tmp_reg__\n\t"
        "mov __tmp_reg__, %B0\n\t"
        "mov %B0, %C0\n\t"
        "mov %C0, __tmp_reg__\n\t"
        : "=r" (val)
        : "0" (val)
    );
    return val;
}

/*!
 * \brief Convert short value from host to network byte order.
 */
#define htons(x) __byte_swap2(x)

/*!
 * \brief Convert long value from host to network byte order.
 */
#define htonl(x) __byte_swap4(x)

/*!
 * \brief Convert short value from network to host byte order.
 */
#define ntohs(x) __byte_swap2(x)

/*!
 * \brief Convert long value from network to host byte order.
 */
#define ntohl(x) __byte_swap4(x)

/*@}*/

#endif
