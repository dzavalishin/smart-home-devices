/*
 * Copyright (C) 2009-2011 by egnite GmbH
 * Copyright (C) 2001-2004 by egnite Software GmbH
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

/*!
 * $Id$
 */

/*!
 * \example httpd_simple/httpd_simple.c
 *
 * Simple Webserver
 *
 */

#include <dev/board.h>

#include <dev/urom.h>

#include <sys/version.h>
#include <sys/timer.h>
#include <sys/confnet.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#include <pro/dhcp.h>
#include <pro/httpd.h>

#include <io.h>
#include <errno.h>

/*!
 * \brief Main application routine.
 *
 * Nut/OS automatically calls this entry after initialization.
 */
int main(void)
{
    uint32_t baud = 115200;
    TCPSOCKET *sock;
    FILE *stream;

    /*
     * Initialize the console.
     */
    NutRegisterDevice(&DEV_CONSOLE, 0, 0);
    freopen(DEV_CONSOLE_NAME, "w", stdout);
    _ioctl(_fileno(stdout), UART_SETSPEED, &baud);

    printf("\n\nSimple HTTP Daemon running on Nut/OS %s\n",
           NutVersionString());

    /*
     * Initialize the network interface.
     */
    printf("Configure %s...", DEV_ETHER_NAME);
    if (NutRegisterDevice(&DEV_ETHER, 0, 0)) {
        puts("failed. Cannot register Ethernet device.");
        for (;;);
    }
    if (NutDhcpIfConfig(DEV_ETHER_NAME, NULL, 60000)) {
        puts("failed. Cannot configure network.\nUse editconf.");
        for (;;);
    }
    printf("%s ready\n", inet_ntoa(confnet.cdn_ip_addr));

    /*
     * Initialize the file system.
     */
    printf("Register UROM file system...");
    if (NutRegisterDevice(&devUrom, 0, 0)) {
        puts("failed.");
        for (;;);
    }
    puts("OK.");

    /*
     * Now loop endless for connections.
     */
    for (;;) {
        /* Create a socket. */
        sock = NutTcpCreateSocket();
        if (sock == NULL) {
            printf("Error %d creating socket.\n", errno);
            NutSleep(1000);
            continue;
        }

        /* Listen on port 80. NutTcpAccept() will block
           until we get a connection from a client. */
        printf("Listening...");
        NutTcpAccept(sock, 80);
        printf("Connected...");

        /* Associate a binary stdio stream with the socket. */
        stream = _fdopen((int) ((uintptr_t) sock), "r+b");
        if (stream == NULL) {
            printf("Error %d creating stream.\n", errno);
        } else {
            /* This API call saves us a lot of work. It will parse
               the client's HTTP request, send the requested file. */
            NutHttpProcessRequest(stream);

            /* Destroy the associated stream. */
            fclose(stream);
        }

        /* Close the socket. */
        NutTcpCloseSocket(sock);
        puts("Disconnected");
    }
    return 0;
}
