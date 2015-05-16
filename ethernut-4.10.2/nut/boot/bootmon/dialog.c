/*
 * Copyright (C) 2005-2007 by egnite Software GmbH
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
 * $Id: dialog.c 2953 2010-04-03 10:10:20Z haraldkipp $
 */

#include "utils.h"
#include "uart.h"
#include "dialog.h"


static char buf[9];
static char hexdigit[] = "0123456789ABCDEF";
static char inbuff[60];

int PutString(char *cp)
{
    int rc = 0;

    for (; *cp; cp++) {
        if (*cp == '\n') {
            UartTx('\r');
            rc++;
        }
        UartTx(*cp);
        rc++;
    }
    return rc;
}

int GetLine(char *line, int size)
{
    int cnt;
    unsigned char ch;

    cnt = PutString(line);
    line += cnt;
    for (;;) {
        if (cnt > size - 1)
            break;
        while ((ch = UartRx()) == 0 || ch == 32);
        if (ch == 8) {
            if (cnt) {
                UartTx(ch);
                UartTx(' ');
                UartTx(ch);
                cnt--;
                line--;
            }
        }
        else if (ch < 32) {
            break;
        }
        else {
            UartTx(ch);
            *line++ = ch;
            cnt++;
        }
    }
    *line = 0;
    PutString("\n");

    return cnt;
}

/*
 * Get a line of input.
 */
void GetIP(char *prompt, char *value)
{
    for (;;) {
        PutString(prompt);
        PutString(": ");
        strcpy_(inbuff, value);
        if (GetLine(inbuff, sizeof(inbuff)) == 0)
            break;
        if (inet_addr(inbuff) != (unsigned long) (-1L)) {
            strcpy_(value, inbuff);
            break;
        }
        PutString("Bad IP address ");
        PutString(inbuff);
        PutString("\n");
    }
}

void PutHex(unsigned char val)
{
    register unsigned char i;

    for (i = 2; i-- > 0;) {
        buf[i] = hexdigit[val & 0x0F];
        val >>= 4;
    }
    buf[2] = 0;
    PutString(buf);
}

void PutShortHex(unsigned short val)
{
    register unsigned char i;

    for (i = 4; i-- > 0;) {
        buf[i] = hexdigit[val & 0x0F];
        val >>= 4;
    }
    buf[4] = 0;
    PutString(buf);
}

void PutLongHex(unsigned long val)
{
    register unsigned char i;

    for (i = 8; i-- > 0;) {
        buf[i] = hexdigit[val & 0x0F];
        val >>= 4;
    }
    buf[8] = 0;
    PutString(buf);
}

void GetMac(unsigned char *mac)
{
    int i;
    int n;

    for (;;) {
        PutString("\nMAC address: ");
        for (i = 0; i < 6; i++) {
            inbuff[i * 2] = hexdigit[mac[i] >> 4];
            inbuff[i * 2 + 1] = hexdigit[mac[i] & 0x0F];
        }
        inbuff[i * 2] = 0;

        n = GetLine(inbuff, sizeof(inbuff));
        if (n == 0) {
            break;
        }
        for (i = 0; i < n; i++) {
            if (inbuff[i] < '0' || (inbuff[i] > '9' && inbuff[i] < 'A') || (inbuff[i] > 'F' && inbuff[i] < 'a') || inbuff[i] > 'f') {
                n = 13;
            }
        }
        if (n <= 12 && (n & 1) == 0) {
            n >>= 1;
            for (i = 0; i < n; i++) {
                mac[6 - n + i] = hex2bin(inbuff[i * 2]) * 16 + hex2bin(inbuff[i * 2 + 1]);
            }
            break;
        }
        PutString("Bad MAC address");
    }
}
