/*
 * Copyright (C) 2003-2005 by egnite Software GmbH. All rights reserved.
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
 */

/*
 * $Log$
 * Revision 1.1  2005/11/03 15:14:27  haraldkipp
 * First import.
 *
 */

#include "utils.h"

/*!
 * \brief Delay by executing a given number of nops.
 */
void MicroDelay(u_short nops)
{
    u_short i;

    for (i = 0; i < nops; i++) {
        asm volatile ("nop");
    }
}

/*!
 * \brief Delay by executing a given number of nops, multiplied by 100000.
 */
void Delay(u_char val)
{
    u_char i;

    for(i = 0; i < val; i++) {
        MicroDelay(0xFFFF);
        MicroDelay(0xFFFF);
        MicroDelay(0xFFFF);
    }
}

/*
 * Conversion of 16 bit value to network order.
 */
u_short __byte_swap2(u_short val)
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
u_long __byte_swap4(u_long val)
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

void memcpy_(u_char *dst, const u_char *src, u_char len)
{
    while(len--) {
        *dst++ = *src++;
    }
}

void memset_(u_char *dst, u_char val, u_char len)
{
    while(len--) {
        *dst++ = val;
    }
}


