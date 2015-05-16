/*
 * Copyright (C) 2011 by egnite GmbH
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

/*!
 * $Id$
 */

/*!
 * \example editconf/editconf.c
 *
 * Network configuration editor.
 *
 * This application allows to view and modify the network configuration
 * that is stored on non-volatile memory. It requires a terminal emulator
 * running on the PC and an RS-232 connection between the PC and the
 * target board.
 *
 * Note, that the main UART is used here, not the DEBUG device. On some
 * boards this may require a different jumper configuration.
 */

#include <dev/board.h>
#include <sys/confnet.h>
 
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <io.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>

#include "meminfo.h"

/* Reads line from standard input. */
static int EditLine(char *prompt, char *line, int siz)
{
    int ch;
    int pos = strlen(line);
 
    printf("%s: %s", prompt, line);
    for (;;) {
        ch = getchar();
        if (ch == 8) {
            if (pos) {
                pos--;
                printf("\b \b");
            }
        }
        else if (ch < ' ') {
            break;
        }
        else if (pos + 1 < siz) {
            putchar(ch);
            line[pos++] = ch;
        }
        else {
            putchar('\a');
        }
    }
    line[pos] = 0;
    putchar('\n');
 
    return 0;
}

/* Editor main routine. */
int main(void)
{
    uint32_t baud = 115200;
    char buf[32];
    uint8_t *cp;
    uint32_t addr;
    char ch;
 
    /* Assign stdin and stdout to the default UART device. */
    NutRegisterDevice(&DEV_CONSOLE, 0, 0);
    freopen(DEV_CONSOLE_NAME, "w", stdout);
    freopen(DEV_CONSOLE_NAME, "r", stdin);    
    _ioctl(_fileno(stdout), UART_SETSPEED, &baud);
    puts("\n\nNetwork Configuration Editor - Compiled " __DATE__ " - " __TIME__);
    ShowHardwareConfiguration();
 
    for (;;) {
        /* Load configuration. */
        if (NutNetLoadConfig(DEV_ETHER_NAME)) {
            puts("\nNo configuration available");
            strcpy(confnet.cd_name, DEV_ETHER_NAME);
        } else {
            puts("\nConfiguration loaded");
        }
 
        /* Edit MAC address. */
        do {
            strcpy(buf, ether_ntoa(confnet.cdn_mac));
            EditLine("MAC Address", buf, 18);
            cp = ether_aton(buf);
        } while (cp == NULL);
        memcpy(confnet.cdn_mac, cp, 6);
 
        /* Edit IP address. */
        do {
            strcpy(buf, inet_ntoa(confnet.cdn_cip_addr));
            EditLine("IP Address", buf, 16);
            addr = inet_addr(buf);
        } while (addr == -1);
        confnet.cdn_cip_addr = addr;
 
        /* Edit IP mask. */
        do {
            strcpy(buf, inet_ntoa(confnet.cdn_ip_mask));
            EditLine("IP Mask", buf, 16);
            addr = inet_addr(buf);
        } while (addr == -1);
        confnet.cdn_ip_mask = addr;
 
        /* Edit IP gate. */
        do {
            strcpy(buf, inet_ntoa(confnet.cdn_gateway));
            EditLine("IP Gate", buf, 16);
            addr = inet_addr(buf);
        } while (addr == -1);
        confnet.cdn_gateway = addr;
 
        /* Prompt for saving. */
        printf("\nPress S to save this configuration ");
 
        /* Flush input buffer and read next character. */
        while (kbhit()) {
            ch = getchar();
        }
        ch = getchar();
 
        /* Save or discard edited configuration. */
        if (ch == 's' || ch == 'S') {
            if (NutNetSaveConfig()) {
                puts("Failed");
            } else {
                puts("Saved");
            }
        } else {
            puts("Discarded");
        }
    }
    return 0;
}
