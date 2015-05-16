/*
 * Copyright (C) 2006 by egnite Software GmbH. All rights reserved.
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

#include <string.h>
#include <netinet/if_ether.h>

static INLINE int hex2bin(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return 0;
}

static char hexdigit[] = "0123456789ABCDEF";

/*!
 * \brief Convert an ASCII string to a binary Ethernet address.
 *
 * \note This function returns a pointer to internal static storage space 
 *       that will be overwritten by subsequent calls.
 *
 * \param str String to convert.
 *
 * \return Pointer to a static string that contains the converted 
 *         binary address.
 */
uint8_t *ether_aton(CONST char *str)
{
    static uint8_t mac[6];
    CONST char *cp = str;
    int n = 0;

    memset(mac, 0, sizeof(mac));
    for (cp = str; *cp;) {
        if (*cp < '0' || (*cp > '9' && *cp < 'A') || (*cp > 'F' && *cp < 'a') || *cp > 'f') {
            return NULL;
        }
        if (mac[n] > 0x0F) {
            return NULL;
        }
        mac[n] <<= 4;
        mac[n] |= hex2bin(*cp);
        cp++;
        if (*cp == ':') {
            n++;
            if (n > 5) {
                return NULL;
            }
            cp++;
        }
    }
    return mac;
}

/*!
 * \brief Convert a binary Ethernet address to an ASCII string.
 *
 * \note This function returns a pointer to internal static storage space 
 *       that will be overwritten by subsequent calls.
 *
 * \param mac Address to convert.
 *
 * \return Pointer to a static string that contains the converted 
 *         ASCII string.
 */
char *ether_ntoa(CONST uint8_t *mac)
{
    static char str[18];
    uint_fast8_t i;
    char *cp = str;

    for (i = 0; i < 6; i++) {
        *cp++ = hexdigit[(mac[i] >> 4) & 0x0F];
        *cp++ = hexdigit[mac[i] & 0x0F];
        *cp++ = ':';
    }
    *--cp = 0;

    return str;
}
