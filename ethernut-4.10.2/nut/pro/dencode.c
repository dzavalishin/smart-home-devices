/*
 * Copyright (C) 2001-2003 by egnite Software GmbH. All rights reserved.
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
 * Revision 1.7  2009/03/07 00:19:29  olereinhardt
 * Moved base64 decoder to gorp/base64/base64_decode.c
 *
 * Revision 1.6  2006/03/16 15:25:38  haraldkipp
 * Changed human readable strings from u_char to char to stop GCC 4 from
 * nagging about signedness.
 *
 * Revision 1.5  2004/03/16 16:48:46  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 * Revision 1.4  2004/01/16 10:27:55  drsung
 * Another code size improvement.
 *
 * Revision 1.3  2003/11/27 08:46:41  drsung
 * Code size improvement
 *
 * Revision 1.2  2003/11/27 08:30:54  drsung
 * Bug fix
 *
 */
 
#include "dencode.h"

int NutDecodeHex(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return -1;
}

char *NutDecodePath(char *path)
{
    int x1;
    int x2;
    char *src = path;
    char *dst = path;
    char last = *path;

    if (last != '/')
        return 0;
    while (*++src) {
        if (*src == '%' &&
            (x1 = NutDecodeHex(*(src + 1))) >= 0 &&
            (x2 = NutDecodeHex(*(src + 2))) >= 0) {
            src += 2;
            if ((*src = x1 * 16 + x2) == 0)
                break;
        }
        if (*src == '\\')
            *src = '/';
        if ((last != '.' && last != '/') || (*src != '.' && *src != '/'))
            *dst++ = last = *src;

    }
    *dst = 0;

    return path;
}
