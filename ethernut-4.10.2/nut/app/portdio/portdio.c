/*!
 * Copyright (C) 2001-2008 by egnite Software GmbH
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
 * $Id: portdio.c 3307 2011-02-11 16:12:43Z olereinhardt $
 */

/*!
 * \example portdio/portdio.c
 *
 * TCP server for Port D I/O.
 *
 * Program Ethernut with portdio.hex and enter
 *
 * telnet x.x.x.x 12345
 *
 * two times on two command prompts, replacing x.x.x.x with the IP address 
 * of your ethernut board. This will start two telnet session. 
 *
 * Enter help for a list of available commands.
 *
 * Enter query on the first will show the current port status.
 *
 * Then enter wait on this session, which will hang until the port status 
 * changes. 
 *
 * On the second telnet session enter set1 to set the first output bit.
 */

#define MY_MAC          {0x00,0x06,0x98,0x20,0x00,0x00}
#define MY_IP           "192.168.192.100"
#define MY_MASK         "255.255.255.0"
#define MY_PORT         12345

#include <string.h>
#include <stdio.h>

#include <dev/board.h>
#include <dev/gpio.h>

#include <sys/heap.h>
#include <sys/thread.h>
#include <sys/timer.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <net/route.h>
#include <netdb.h>

#include <pro/dhcp.h>

#if defined(ETHERNUT1) || defined(ETHERNUT2)
#define INBANK      2    /* PORTB */
#define INPIN1      0
#define INPIN2      1
#define INPIN3      2
#define INPIN4      3
#define OUTBANK     2    /* PORTB */
#define OUTPIN1     4
#define OUTPIN2     5
#define OUTPIN3     6
#define OUTPIN4     7
#elif defined(ETHERNUT3)
/* Uses same expansion port pins as Ethernut 1/2. */
#define INBANK      0    /* PIO */
#define INPIN1      0
#define INPIN2      1
#define INPIN3      2
#define INPIN4      3
#define OUTBANK     0    /* PIO */
#define OUTPIN1     4
#define OUTPIN2     5
#define OUTPIN3     6
#define OUTPIN4     7
#elif defined(AT91SAM7X_EK)
#define INBANK      1    /* PIOA Joystick */
#define INPIN1      21
#define INPIN2      22
#define INPIN3      23
#define INPIN4      24
#define OUTBANK     2    /* PIOB User LEDs */
#define OUTPIN1     19
#define OUTPIN2     20
#define OUTPIN3     21
#define OUTPIN4     22
#elif defined(AT91SAM9260_EK)
#define INBANK      1   /* PIOA */
#define INPIN1      30  /* Push button 3 */
#define INPIN2      31  /* Push button 4 */
#define OUTBANK     1   /* PIOA */
#define OUTPIN1     6   /* User LED */
#define OUTPIN2     9   /* Power LED */
#elif defined(EVK1100)
#define INBANK		0   /* PA Joystick */
#define INPIN1      25  
#define INPIN2      26  
#define INPIN3      27
#define INPIN4      28
#define OUTBANK     1    /* PIOB User LEDs */
#define OUTPIN1     27
#define OUTPIN2     28
#define OUTPIN3     29
#define OUTPIN4     30
#endif

/*
 * Previous AVR versions read the full PIN register. Now each input
 * and output pin is freely configurable (within a single port bank).
 * This routine collects all pins as they would have been read
 * from a single 8-bit register.
 */

#ifdef INBANK
static int PortStatus(void)
{
    int stat = 0;

#ifdef INPIN1
    stat |= GpioPinGet(INBANK, INPIN1);
#endif
#ifdef INPIN2
    stat |= GpioPinGet(INBANK, INPIN2) << 1;
#endif
#ifdef INPIN3
    stat |= GpioPinGet(INBANK, INPIN3) << 2;
#endif
#ifdef INPIN4
    stat |= GpioPinGet(INBANK, INPIN4) << 3;
#endif


#ifdef OUTBANK
#ifdef OUTPIN1
    stat |= GpioPinGet(OUTBANK, OUTPIN1) << 4;
#endif
#ifdef OUTPIN2
    stat |= GpioPinGet(OUTBANK, OUTPIN2) << 5;
#endif
#ifdef OUTPIN3
    stat |= GpioPinGet(OUTBANK, OUTPIN3) << 6;
#endif
#ifdef OUTPIN4
    stat |= GpioPinGet(OUTBANK, OUTPIN4) << 7;
#endif
#endif /* OUTBANK */

    return stat;
}
#endif
/*
 * Process client requests.
 */
void ProcessRequests(FILE * stream)
{
    char buff[128];
    char *cp;
    int stat = -1;

    fputs("200 Welcome to portdio. Type help to get help.\r\n", stream);
    for (;;) {
        fflush(stream);

        /*
         * Read a line from the client. Ignore
         * blank lines.
         */
        if (fgets(buff, sizeof(buff), stream) == 0)
            break;
        if ((cp = strchr(buff, '\r')) != 0)
            *cp = 0;
        if ((cp = strchr(buff, '\n')) != 0)
            *cp = 0;
        if (buff[0] == 0)
            continue;

        /*
         * Memory info.
         */
        if (strncmp(buff, "memory", strlen(buff)) == 0) {
            fprintf(stream, "210 %u bytes RAM free\r\n", (unsigned int)NutHeapAvailable());
            continue;
        }

#ifdef OUTBANK
        /*
         * Reset output bit.
         */
        if (strlen(buff) > 1 && strncmp(buff, "reset", strlen(buff) - 1) == 0) {
            int ok = 1;
            switch (buff[strlen(buff) - 1]) {
#ifdef OUTPIN1
            case '1':
                GpioPinSetLow(OUTBANK, OUTPIN1);
                break;
#endif
#ifdef OUTPIN2
            case '2':
                GpioPinSetLow(OUTBANK, OUTPIN2);
                break;
#endif
#ifdef OUTPIN3
            case '3':
                GpioPinSetLow(OUTBANK, OUTPIN3);
                break;
#endif
#ifdef OUTPIN4
            case '4':
                GpioPinSetLow(OUTBANK, OUTPIN4);
                break;
#endif
            default:
                ok = 0;
                break;
            }
            if (ok) {
                fputs("210 OK\r\n", stream);
            } else
                fputs("410 Bad pin\r\n", stream);
            continue;
        }

        /*
         * Set output bit.
         */
        if (strlen(buff) > 1 && strncmp(buff, "set", strlen(buff) - 1) == 0) {
            int ok = 1;
            switch (buff[strlen(buff) - 1]) {
#ifdef OUTPIN1
            case '1':
                GpioPinSetHigh(OUTBANK, OUTPIN1);
                break;
#endif
#ifdef OUTPIN2
            case '2':
                GpioPinSetHigh(OUTBANK, OUTPIN2);
                break;
#endif
#ifdef OUTPIN3
            case '3':
                GpioPinSetHigh(OUTBANK, OUTPIN3);
                break;
#endif
#ifdef OUTPIN4
            case '4':
                GpioPinSetHigh(OUTBANK, OUTPIN4);
                break;
#endif
            default:
                ok = 0;
                break;
            }
            if (ok) {
                fputs("210 OK\r\n", stream);
            } else
                fputs("410 Bad pin\r\n", stream);
            continue;
        }
#endif /* OUTBANK */

#ifdef INBANK
        /*
         * Port status.
         */
        if (strncmp(buff, "query", strlen(buff)) == 0) {
            stat = PortStatus();
            fprintf(stream, "210 %02X\r\n", stat);
            continue;
        }

        /*
         * wait for status change.
         */
        if (strncmp(buff, "wait", strlen(buff)) == 0) {
            while (stat == PortStatus())
                NutThreadYield();
            stat = PortStatus();
            fprintf(stream, "210 %02X\r\n", stat);
            continue;
        }
#endif /* INBANK */

        /*
         * Help.
         */
        fputs("400 List of commands follows\r\n", stream);
        fputs("memory\tQueries number of RAM bytes free\r\n", stream);
#if OUTBANK
        fputs("reset#\tSet output bit 1..4 low\r\n", stream);
        fputs("set#\tSet output bit 1..4 high\r\n", stream);
#endif
#if INBANK
        fputs("query\tQuery digital i/o status\r\n", stream);
        fputs("wait\tWaits for digital i/o change\r\n", stream);
#endif
        fputs(".\r\n", stream);
    }
}

/*
 * Init Port D
 */
void init_dio(void)
{
    /* Configure input pins, enable pull up. */
#ifdef INBANK
#ifdef INPIN1
    GpioPinConfigSet(INBANK, INPIN1, GPIO_CFG_PULLUP);
#endif
#ifdef INPIN2
    GpioPinConfigSet(INBANK, INPIN2, GPIO_CFG_PULLUP);
#endif
#ifdef INPIN3
    GpioPinConfigSet(INBANK, INPIN3, GPIO_CFG_PULLUP);
#endif
#ifdef INPIN4
    GpioPinConfigSet(INBANK, INPIN4, GPIO_CFG_PULLUP);
#endif
#endif /* INBANK */

    /* Configure output pins, set to low. */
#ifdef OUTBANK
#ifdef OUTPIN1
    GpioPinConfigSet(OUTBANK, OUTPIN1, GPIO_CFG_OUTPUT);
    GpioPinSetLow(OUTBANK, OUTPIN1);
#endif
#ifdef OUTPIN2
    GpioPinConfigSet(OUTBANK, OUTPIN2, GPIO_CFG_OUTPUT);
    GpioPinSetLow(OUTBANK, OUTPIN2);
#endif
#ifdef OUTPIN3
    GpioPinConfigSet(OUTBANK, OUTPIN3, GPIO_CFG_OUTPUT);
    GpioPinSetLow(OUTBANK, OUTPIN3);
#endif
#ifdef OUTPIN4
    GpioPinConfigSet(OUTBANK, OUTPIN4, GPIO_CFG_OUTPUT);
    GpioPinSetLow(OUTBANK, OUTPIN4);
#endif
#endif /* OUTBANK */
}

#ifdef DEV_ETHER

void service(void)
{
    TCPSOCKET *sock;
    FILE *stream;

    /*
     * Loop endless for connections.
     */
    for (;;) {
        /*
         * Create a socket.
         */
        sock = NutTcpCreateSocket();

        /*
         * Listen at the configured port. If we return, we got a client.
         */
        NutTcpAccept(sock, MY_PORT);

        /*
         * Create a stream from the socket.
         */
        stream = _fdopen((int) sock, "r+b");

        /*
         * Process client requests.
         */
        ProcessRequests(stream);

        /*
         * Destroy our device.
         */
        fclose(stream);

        /*
         * Close our socket.
         */
        NutTcpCloseSocket(sock);
    }
}

THREAD(service_thread, arg)
{
    for (;;)
        service();
}

#endif /* DEV_ETHER */

/*
 * Main application routine. 
 *
 * Nut/OS automatically calls this entry after initialization.
 */
int main(void)
{
    uint8_t my_mac[] = MY_MAC;

    /*
     * Initialize digital I/O.
     */
    init_dio();

#ifdef DEV_ETHER
    /*
     * Register Realtek controller at address 8300 hex
     * and interrupt 5.
     */
    NutRegisterDevice(&DEV_ETHER, 0x8300, 5);

    /*
     * Configure lan interface. 
     */
    if (NutDhcpIfConfig(DEV_ETHER_NAME, 0, 60000) && NutDhcpIfConfig("eth0", my_mac, 60000)) {
        /*
         * No DHCP server available. Use hard coded values.
         */
        uint32_t ip_addr = inet_addr(MY_IP);      /* ICCAVR fix. */
        NutNetIfConfig("eth0", my_mac, ip_addr, inet_addr(MY_MASK));
    }

    /*
     * Start another service thread to allow
     * two concurrent connections.
     */
    NutThreadCreate("sback", service_thread, 0, 1384);

    for (;;)
        service();
#endif /* DEV_ETHER */

    return 0;
}
