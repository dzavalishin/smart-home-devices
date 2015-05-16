#ifndef _UTILS_H
#define _UTILS_H

/*
 * Copyright (C) 2003-2007 by egnite Software GmbH
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
 *
 */

/*
 * $Id: utils.h 2935 2010-04-01 12:14:17Z haraldkipp $
 */

extern void MicroDelay(unsigned int nops);
extern void Delay(unsigned char val);

extern void memset_(unsigned char *dst, unsigned char val, unsigned char len);
extern void memcpy_(unsigned char *dst, const unsigned char *src, unsigned char len);
extern void strcpy_(char *dst, const char *src);
extern int memcmp_(const unsigned char *dst, const unsigned char *src, unsigned char len);
extern unsigned long inet_addr(char *str);
extern int hex2bin(char c);

extern char *inet_ntoa(unsigned long addr);


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

#define __byte_swap2(val)           \
    ((((val) & 0xff) << 8) |        \
     (((val) & 0xff00) >> 8))

#define __byte_swap4(val)           \
    ((((val) & 0xff) << 24) |       \
     (((val) & 0xff00) << 8) |      \
     (((val) & 0xff0000) >> 8) |    \
     (((val) & 0xff000000) >> 24))

#define _NOP() __asm__ __volatile__ ("mov r0, r0")

#endif
