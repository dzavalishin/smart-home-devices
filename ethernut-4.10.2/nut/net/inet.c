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
 * -
 * Portions Copyright (c) 1993 by Digital Equipment Corporation.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies, and that
 * the name of Digital Equipment Corporation not be used in advertising or
 * publicity pertaining to distribution of the document or software without
 * specific, written prior permission.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND DIGITAL EQUIPMENT CORP. DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS.   IN NO EVENT SHALL DIGITAL EQUIPMENT
 * CORPORATION BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

/*
 * $Log$
 * Revision 1.4  2008/08/11 07:00:29  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.3  2006/03/16 15:25:35  haraldkipp
 * Changed human readable strings from u_char to char to stop GCC 4 from
 * nagging about signedness.
 *
 * Revision 1.2  2005/08/02 17:47:03  haraldkipp
 * Major API documentation update.
 *
 * Revision 1.1.1.1  2003/05/09 14:41:30  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.11  2003/02/04 18:14:57  harald
 * Version 3 released
 *
 * Revision 1.10  2002/06/26 17:29:36  harald
 * First pre-release with 2.4 stack
 *
 */

/*!
 * \addtogroup xgIP
 */
/*@{*/

#include <arpa/inet.h>

/*!
 * \brief Convert decimal dotted ASCII representation into numeric IP address.
 *
 * \param str String containing the ASCII representation.
 *
 * \return IP address in network byte order.
 */
uint32_t inet_addr(CONST char * str)
{
    uint_fast16_t num;
    uint32_t addr = 0;
    uint_fast8_t parts = 0;
    uint8_t *ap;

    ap = (uint8_t *) & addr;
    while (parts < 4) {
        if (*str < '0' || *str > '9')
            break;
        for (num = 0; num <= 255;) {
            num = (num * 10) + (*str - '0');
            if (*++str < '0' || *str > '9')
                break;
        }
        if (num > 255)
            break;
        parts++;
        *ap++ = (uint8_t) num;
        if (*str != '.') {
            if (parts == 4)
                return addr;
            break;
        }
        str++;
    }
    return -1;
}

/*!
 * \brief Convert numeric IP address into decimal dotted 
 *        ASCII representation.
 *
 * \note This function is not thread safe. Each subsequent
 *       call will destroy the previous result. Applications
 *       should locally store the result before calling the 
 *       function again or allowing other threads to call it.
 *
 * \param addr IP address in network byte order.
 *
 * \return Pointer to a static buffer containing the 
 *         ASCII representation.
 */
char *inet_ntoa(uint32_t addr)
{
    static char str[16];
    char inv[3];
    char *rp;
    uint8_t *ap;
    uint8_t rem;
    uint_fast8_t n;
    uint_fast8_t i;

    rp = str;
    ap = (uint8_t *) & addr;
    for (n = 0; n < 4; n++) {
        i = 0;
        do {
            rem = *ap % (uint8_t) 10;
            *ap /= (uint8_t) 10;
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

/*!
 * \brief Convert numeric MAC address array into double dotted 
 *        ASCII representation.
 *
 * \note This function is not thread safe. Each subsequent
 *       call will destroy the previous result. Applications
 *       should locally store the result before calling the 
 *       function again or allowing other threads to call it.
 *
 * \param mac Pointer to MAC address array.
 *
 * \return Pointer to a static buffer containing the 
 *         ASCII representation.
 */
char *inet_mtoa(uint8_t *mac)
{
    static char str[18];
    char hex[16]="0123456789ABCDEF";
    int i, p;

    p=0;
    for(i=0;i<6;i++) 
    {
        str[p++]=hex[mac[i]>>4];
        str[p++]=hex[mac[i]&0xf];
        if(i<5) str[p++]=':'; else str[p++]='\0';
    }
    return str;
}

/*@}*/
