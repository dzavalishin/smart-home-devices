/*
 * Copyright (C) 2001-2004 by egnite Software GmbH
 * Copyright (C) 2009 by egnite GmbH
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
 * \example httpd_upnp/httpserv.c
 *
 * Very simple multithreaded HTTP daemon with DHCP and UPnP functionality.
 *
 * Note: A DHCP server must be available on the network.
 * Tested on the Elektor Internet Radio.
 */


#include <cfg/os.h>

#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <time.h>

#include <dev/board.h>
#include <dev/urom.h>
#include <dev/nplmmc.h>
#include <dev/sbimmc.h>
#include <fs/phatfs.h>

#include <sys/version.h>
#include <sys/thread.h>
#include <sys/timer.h>
#include <sys/heap.h>
#include <sys/confnet.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <net/route.h>
#include <netinet/in.h>

#include <pro/httpd.h>
#include <pro/dhcp.h>
#include <pro/ssi.h>
#include <pro/asp.h>
#include <pro/discover.h>

#include "upnp.h"


/* Server thread stack size. */
#ifndef HTTPD_SERVICE_STACK
#define HTTPD_SERVICE_STACK ((1024 * NUT_THREAD_STACK_MULT) + NUT_THREAD_STACK_ADD)
#endif

/*! \fn Service(void *arg)
 * \brief HTTP service thread.
 *
 * The endless loop in this thread waits for a client connect,
 * processes the HTTP request and disconnects. Nut/Net doesn't
 * support a server backlog. If one client has established a
 * connection, further connect attempts will be rejected.
 * Typically browsers open more than one connection in order
 * to load images concurrently. So we run this routine by
 * several threads.
 *
 */
THREAD(Service, arg)
{
    TCPSOCKET *sock;
    FILE *stream;
    uint8_t id = (uint8_t) ((uintptr_t) arg);

    /*
     * Now loop endless for connections.
     */
    for (;;) {

        /*
         * Create a socket.
         */
        if ((sock = NutTcpCreateSocket()) == 0) {
            printf("[%u] Creating socket failed\n", id);
            NutSleep(5000);
            continue;
        }

        /*
         * Listen on port 80. This call will block until we get a connection
         * from a client.
         */
        NutTcpAccept(sock, 80);
        printf("[%u] Connected, %lu bytes free\n", id, NutHeapAvailable());

        /*
         * Wait until at least 4 kByte of free RAM is available. This will
         * keep the client connected in low memory situations.
         */
        while (NutHeapAvailable() < 4096) {
            printf("[%u] Low mem\n", id);
            NutSleep(1000);
        }

        /*
         * Associate a stream with the socket so we can use standard I/O calls.
         */
        if ((stream = _fdopen((int) ((uintptr_t) sock), "r+b")) == 0) {
            printf("[%u] Creating stream device failed\n", id);
        } else {
            /*
             * This API call saves us a lot of work. It will parse the
             * client's HTTP request, send any requested file from the
             * registered file system or handle CGI requests by calling
             * our registered CGI routine.
             */
            NutHttpProcessRequest(stream);

            /*
             * Destroy the virtual stream device.
             */
            fclose(stream);
        }

        /*
         * Close our socket.
         */
        NutTcpCloseSocket(sock);
        printf("[%u] Disconnected\n", id);
    }
}

/*!
 * \brief Main application routine.
 *
 * Nut/OS automatically calls this entry after initialization.
 */
int main(void)
{
    uint32_t baud = 115200;
    uint8_t i;

    /*
     * Initialize the uart device.
     */
    NutRegisterDevice(&DEV_CONSOLE, 0, 0);
    freopen(DEV_CONSOLE_NAME, "w", stdout);
    _ioctl(_fileno(stdout), UART_SETSPEED, &baud);
    NutSleep(200);
    printf("\n\nNut/OS %s HTTP Daemon...\n", NutVersionString());

    /*
     * Register Ethernet controller.
     */
    if (NutRegisterDevice(&DEV_ETHER, 0, 0)) {
        puts("Registering device failed");
    } else {
        /*
         * Add "All Host" multicast address.
         */
        printf("Add \"All Host\" multicast address...");
        if (NutNetIfAddMcastAddr(DEV_ETHER_NAME, INADDR_ALLHOSTS_GROUP) != 0) {
            /*
             * It could be possible that your ethernet 
             * driver does not support multicast.
             */
            puts("failed");
        } else {
            puts("OK");
        }
    }

    printf("Configure %s...", DEV_ETHER_NAME);
    if (NutNetLoadConfig(DEV_ETHER_NAME)) {
        puts("failed");
    } else {
        if (NutDhcpIfConfig(DEV_ETHER_NAME, 0, 60000)) {
            puts("failed");
        } else {
            puts("OK");
        }
    }
    printf("%s ready\n", inet_ntoa(confnet.cdn_ip_addr));

    /*
     * Register our device for the file system.
     */
    NutRegisterDevice(&devUrom, 0, 0);


    NutRegisterCgiBinPath("cgi-bin/");


    /*
     * Start four server threads.
     */
    for (i = 1; i <= 4; i++) {
        char thname[] = "httpd0";

        thname[5] = '0' + i;
        NutThreadCreate(thname, Service, (void *) (uintptr_t) i, HTTPD_SERVICE_STACK);
    }

    /*
     * Start the UPnP functionality
     */
    upnp_Init();

    /*
     * We could do something useful here, like serving a watchdog.
     */
    NutThreadSetPriority(254);
    for (;;) {
        NutSleep(60000);
    }
    return 0;
}

/*** EOF ***/
