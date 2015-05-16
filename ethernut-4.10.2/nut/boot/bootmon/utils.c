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
 * $Id: utils.c 2935 2010-04-01 12:14:17Z haraldkipp $
 */

#include "utils.h"

/*!
 * \brief Delay by executing a given number of nops.
 */
void MicroDelay(unsigned int nops)
{
    while (nops--) {
        _NOP();
        _NOP();
        _NOP();
        _NOP();
        _NOP();
        _NOP();
        _NOP();
        _NOP();
    }
}

/*!
 * \brief Delay by executing a given number of nops, multiplied by 100000.
 */
void Delay(unsigned char val)
{
    unsigned char i;

    for (i = 0; i < val; i++) {
        MicroDelay(0xFFFF);
        MicroDelay(0xFFFF);
        MicroDelay(0xFFFF);
    }
}

void strcpy_(char *dst, const char *src)
{
    while (*src) {
        *dst++ = *src++;
    }
    *dst = 0;
}

void memcpy_(unsigned char *dst, const unsigned char *src, unsigned char len)
{
    while (len--) {
        *dst++ = *src++;
    }
}

int memcmp_(const unsigned char *dst, const unsigned char *src, unsigned char len)
{
    while (len--) {
        if (*dst != *src) {
            return -1;
        }
    }
    return 0;
}

void memset_(unsigned char *dst, unsigned char val, unsigned char len)
{
    while (len--) {
        *dst++ = val;
    }
}

int hex2bin(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return 0;
}

unsigned long inet_addr(char *str)
{
    unsigned short num;
    unsigned long addr = 0;
    unsigned char parts = 0;
    unsigned char *ap;

    ap = (unsigned char *) &addr;
    while (parts < 4) {
        if (*str < '0' || *str > '9') {
            break;
        }
        for (num = 0; num <= 255;) {
            num = (num * 10) + (*str - '0');
            if (*++str < '0' || *str > '9') {
                break;
            }
        }
        if (num > 255) {
            break;
        }
        parts++;
        *ap++ = (unsigned char) num;
        if (*str != '.') {
            if (parts == 4) {
                return addr;
            }
            break;
        }
        str++;
    }
    return -1;
}

char *inet_ntoa(unsigned long addr)
{
    static char str[16];
    char inv[3];
    char *rp;
    unsigned char *ap;
    unsigned char rem;
    unsigned char n;
    unsigned char i;

    rp = str;
    ap = (unsigned char *) &addr;
    for (n = 0; n < 4; n++) {
        i = 0;
        do {
            rem = *ap % (unsigned char) 10;
            *ap /= (unsigned char) 10;
            inv[i++] = '0' + rem;
        } while (*ap);
        while (i--)
            *rp++ = inv[i];
        *rp++ = '.';
        ap++;
    }
    *--rp = 0;
    return str;
}
