/*!
 * Copyright (C) 2001-2006 by egnite Software GmbH
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
 * $Log$
 * Revision 1.6  2009/02/18 12:18:58  olereinhardt
 * 2009-02-18  Ole Reinhardt <ole.reinhardt@thermotemp.de>
 *
 *           Fixed compilier warnings. Especialy signedness of char buffers
 *           as well as unused code on arm platform and main functions without
 *           return value
 *
 * Revision 1.5  2006/07/21 09:07:48  haraldkipp
 * Fixed warnings about wrong signedness.
 *
 * Revision 1.4  2005/11/22 09:14:13  haraldkipp
 * Replaced specific device names by generalized macros.
 *
 * Revision 1.3  2005/04/19 08:53:56  haraldkipp
 * Added more detailed description
 *
 * Revision 1.2  2003/11/04 17:46:52  haraldkipp
 * Adapted to Ethernut 2
 *
 * Revision 1.1  2003/08/05 18:59:05  haraldkipp
 * Release 3.3 update
 *
 * Revision 1.7  2003/05/08 12:00:53  harald
 * Changed target to famous spammer
 *
 * Revision 1.6  2003/02/04 18:19:37  harald
 * Version 3 released
 *
 * Revision 1.5  2003/02/04 16:24:32  harald
 * Adapted to version 3
 *
 * Revision 1.4  2002/06/12 10:55:54  harald
 * *** empty log message ***
 *
 * Revision 1.3  2002/06/04 19:11:29  harald
 * *** empty log message ***
 *
 * Revision 1.2  2002/05/08 16:02:30  harald
 * First Imagecraft compilation
 *
 */

/*!
 * \example inetq/inetq.c
 *
 * Requests an URL from the Internet and transfers the HTML
 * source code to the serial device.
 *
 * Your local Ethernet network must provide Internet access.
 * Connect the RS232 port of the Ethernut with a free COM
 * port of your PC and run a terminal emulator at 115200 Baud.
 *
 * If your local network does not support DHCP, it may be
 * required to modify the MY_IP, MY_MASK and MY_GATE below.
 *
 * This sample demonstrates DNS query and default route usage.
 */
#define DNSSERVERIP     "192.168.192.2"
#define INETSERVER	"www.kornet.net"
#define INETSERVERPORT  80
#define INETURL         "/"
#define MY_MAC          {0x00,0x06,0x98,0x20,0x00,0x00}
#define MY_IP           "192.168.192.100"
#define MY_MASK         "255.255.255.0"
#define MY_GATE         "192.168.192.3"

#include <string.h>
#include <stdio.h>
#include <io.h>

#include <dev/board.h>

#include <sys/heap.h>
#include <sys/thread.h>
#include <sys/timer.h>
#include <sys/socket.h>
#include <sys/confnet.h>

#include <arpa/inet.h>
#include <net/route.h>
#include <netdb.h>

#include <pro/dhcp.h>

static char buff[1024];
static uint8_t my_mac[] = MY_MAC;

/*
 * Main application routine. 
 *
 */
int main(void)
{
    uint32_t baud = 115200;
    TCPSOCKET *sock;
    FILE *stream;
    uint32_t rip;
    uint32_t ip_addr;
    int bite;
    size_t rc;
    size_t len;
    uint32_t start_time;
    uint32_t total_bytes;

    /*
     * Initialize the uart device.
     */
    NutRegisterDevice(&DEV_CONSOLE, 0, 0);
    freopen(DEV_CONSOLE_NAME, "w", stdout);
    _ioctl(_fileno(stdout), UART_SETSPEED, &baud);
    puts("\nInetQuery 1.0");

#ifdef DEV_ETHER
    /*
     * Register Realtek controller at address 8300 hex and interrupt 5.
     */
    puts("Configuring Ethernet interface");
    NutRegisterDevice(&DEV_ETHER, 0, 0);

    /*
     * Try DHCP. First use MAC from EEPROM.
     */
    if (NutDhcpIfConfig("eth0", 0, 60000) && NutDhcpIfConfig("eth0", my_mac, 60000)) {
        /*
         * No DHCP server available. Use hard coded values.
         */
        ip_addr = inet_addr(MY_IP);
        NutNetIfConfig("eth0", my_mac, ip_addr, inet_addr(MY_MASK));
        NutIpRouteAdd(0, 0, inet_addr(MY_GATE), &DEV_ETHER);
        NutDnsConfig2(0, 0, inet_addr(DNSSERVERIP), 0);
    } else
        ip_addr = confnet.cdn_ip_addr;
    printf("%s ready\n", inet_ntoa(ip_addr));


    /*
     * Resolve hostname using DNS.
     */
    if ((rip = NutDnsGetHostByName((uint8_t*)INETSERVER)) != 0) {

        /*
         * Let's try a stdio stream first.
         */
        if ((sock = NutTcpCreateSocket()) != 0) {

            /*
             * Connect a HTTP server in the Internet.
             */
            printf("Connecting %s:%u\r\n", inet_ntoa(rip), INETSERVERPORT);
            if (NutTcpConnect(sock, rip, INETSERVERPORT) == 0) {

                /*
                 * Assign a stream to our connected socket.
                 */
                if ((stream = _fdopen((int) sock, "r+b")) != 0) {
                    /*
                     * Send HTTP request to the server.
                     */
                    fprintf(stream, "GET %s HTTP/1.0\r\n", INETURL);
                    fputs("User-Agent: Ethernut [en] (NutOS)\r\n", stream);
                    fputs("\r\n", stream);
                    fflush(stream);

                    /*
                     * Init measure values.
                     */
                    start_time = NutGetTickCount();
                    total_bytes = 0;

                    /*
                     * Read server response and send it to the UART.
                     */
                    while (fgets(buff, sizeof(buff), stream)) {
                        puts(buff);
                        total_bytes += strlen(buff);
                    }
                    printf("Transfered %lu bytes in %lu seconds\n", total_bytes, (NutGetTickCount() - start_time) / 16UL);
                    fclose(stream);
                } else
                    puts("Creating stream device failed");

            } else {
                printf("Bad news, %s refuses the connection.\n", INETSERVER);
            }
            printf("Disconnecting %s:%u\n", inet_ntoa(rip), INETSERVERPORT);
            NutTcpCloseSocket(sock);
        }

        NutSleep(5000);

        /*
         * Now let's use native calls.
         */
        if ((sock = NutTcpCreateSocket()) != 0) {

            /*
             * Connect a HTTP server in the Internet.
             */
            printf("Connecting %s:%u\r\n", inet_ntoa(rip), INETSERVERPORT);
            if (NutTcpConnect(sock, rip, INETSERVERPORT) == 0) {

                /*
                 * Send HTTP request to the server. NutTcpSend() doesn't
                 * guarantee to send out all bytes, thus the loop.
                 */
                strcpy(buff, "GET " INETURL " HTTP/1.0\r\nUser-Agent: Ethernut [en] (NutOS)\r\n\r\n");
                len = (int) strlen(buff);
                for (rc = 0; rc < len; rc += bite)
                    if ((bite = NutTcpSend(sock, buff + rc, len - rc)) <= 0)
                        break;

                /*
                 * Init measure values.
                 */
                start_time = NutGetTickCount();
                total_bytes = 0;

                /*
                 * Read server response and send it to the UART.
                 */
                while ((bite = NutTcpReceive(sock, buff, sizeof(buff) - 1)) > 0) {
                    total_bytes += bite;
                    buff[bite] = 0;
                    puts(buff);
                }
                printf("Transfered %lu bytes in %lu seconds\n", total_bytes, (NutGetTickCount() - start_time) / 16UL);
            } else {
                printf("Bad news, %s refuses the connection.\n", INETSERVER);
            }
            printf("Disconnecting %s:%u\n", inet_ntoa(rip), INETSERVERPORT);
            NutTcpCloseSocket(sock);
        }
    } else
        printf("Great news, %s has been removed!\n", INETSERVER);
#endif

    for (;;)
        NutSleep(1000);
    return 0;
}
