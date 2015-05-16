/*
 * Copyright (C) 2011 by egnite GmbH
 * Copyright (C) 2003-2006 by egnite Software GmbH
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
 * $Id: pppc.c 3538 2011-08-10 16:34:10Z haraldkipp $
 */

/*!
 * \example pppc/pppc.c
 *
 * PPP client sample.
 *
 * This example demonstrates Nut/OS Point-To-Point protocol. Most likely
 * it will not run without modification. See the '#define' entries
 * below.
 *
 * The application will dial the provider via a modem, using the specified
 * chat script. When connected, it will establish a PPP session. Then DNS
 * is used to query the IP address of an NTP server, from which it queries
 * the current time. Finally the PPP session is terminated and the whole
 * procedure is repeated every hour.
 *
 * By default the application will use 2 serial ports. The first one is
 * used for debugging output on stdout and the second UART will be 
 * connected to a modem.
 *
 * Note, that PPP is not available for all target boards. If this is the
 * case the program will use the local Ethernet network, where DHCP must
 * be available.
 */

#include <cfg/ahdlc.h>

#include <dev/board.h>
#include <dev/ahdlcavr.h>
#include <dev/ppp.h>
#include <dev/chat.h>

#include <sys/version.h>
#include <sys/timer.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <net/if_var.h>
#include <net/route.h>

#include <pro/dhcp.h>
#include <pro/sntp.h>
#include <pro/rfctime.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <time.h>

/*
 * Conversational script with a modem.
 *
 * The Nut/OS modem chat utility works similar to the the UNIX chat
 * script. In general it consists of one or more expect-send pairs of
 * strings or optional keyword-parameter pairs. Keywords are
 *
 * - TIMEOUT to set the max. time in seconds to wait for expected string
 * - ABORT to specify up to 10 abort strings
 * - REPORT to specify up to 3 reported abort strings
 *
 * Adjust this script to the requirements of your modem equipment
 * and your GPRS provider.
 *
 * The following script had been tested with a Siemens GPRS modem
 * (S55 mobile phone), using a t-mobile pre-paid account.
 */
#define PPP_CHAT    /* Wait up to 20 seconds for any response. */ \
                    "TIMEOUT 20" \
                    /* Expect nothing at the beginning. */ \
                    " ''" \
                    /* Send simple AT command to check the modem. */ \
                    " AT OK" \
                    /* Define PDP context, expecting 'OK'. */ \
                    " AT+CGDCONT=1,\"IP\",\"internet.t-mobile\" OK" \
                    /* Dial and wait for 'CONNECT'. */ \
                    " ATD*99***1# CONNECT"

/*
 * PPP user and password.
 *
 * Often GPRS providers accept any login.
 */
#define PPP_USER    "me"
#define PPP_PASS    "secret"

/*
 * PPP device settings.
 */
#if defined(NUT_THREAD_AHDLCRXSTACK)
#define PPP_DEV         devAhdlc1   /* AHDLC driver */
#define PPP_DEV_NAME    "uart1"     /* Physical device name */
#define PPP_SPEED       115200      /* Baudrate */
#define PPP_RXTO        1000        /* Receive timeout (ms) */
#else
#warning "PPP is not supported on your target"
#endif

/*
 * Open serial debug port for standard output.
 */
static void DebugPortOpen(void)
{
    uint32_t baud = 115200;

    /* Register debug UART. */
    NutRegisterDevice(&DEV_CONSOLE, 0, 0);
    /* Open debug device for standard output. */
    freopen(DEV_CONSOLE_NAME, "w", stdout);
    /* Set baud rate. */
    _ioctl(_fileno(stdout), UART_SETSPEED, &baud);
}

/*
 * Initialize the protocol port.
 */
static void ProtocolPortInit(void)
{
#ifdef PPP_DEV
    /* Register PPP and UART device. */
    NutRegisterDevice(&PPP_DEV, 0, 0);
    NutRegisterDevice(&devPpp, 0, 0);
#else
    /* Use Ethernet. */
    NutRegisterDevice(&DEV_ETHER, 0, 0);
#endif
}

/*
 * Open the physical device.
 */
static int ProtocolPortOpen(void)
{
#ifdef PPP_DEV
    int pppcom;
    uint32_t lctl;

    /* Open PPP device. Specify physical device, user and password. */
    printf("Open PPP interface at " PPP_DEV_NAME "...");
    pppcom = _open("ppp:" PPP_DEV_NAME "/" PPP_USER "/" PPP_PASS, _O_RDWR | _O_BINARY);
    if (pppcom == -1) {
        puts("failed");
        return -1;
    }

    /*
     * Set PPP line speed.
     */
    lctl = PPP_SPEED;
    _ioctl(pppcom, UART_SETSPEED, &lctl);
    printf("%lu baud", lctl);

    /*
     * The PPP driver doesn't set any receive timeout, but
     * may require it.
     */
    lctl = PPP_RXTO;
    _ioctl(pppcom, UART_SETREADTIMEOUT, &lctl);
    printf(", %lu ms timeout\n", lctl);

    return pppcom;
#else
    /* Nothing to be done for Ethernet. */
    return 0;
#endif
}

/*
 * Establish a modem connection.
 */
static int ProtocolPortConnect(int pppcom)
{
#ifdef PPP_DEV
    int rc;

    /*
     * Connect using a chat script. We may also set any
     * required hardware handshake line at this stage.
     */
    printf("Connecting...");
    for (;;) {
        rc = NutChat(pppcom, PPP_CHAT);
        switch (rc) {
        case 0:
            puts("done");
            /* A short pause is required here to let the driver
               initialize the receiver thread. */
            NutSleep(100);
            break;
        case 1:
            puts("bad script");
            break;
        case 2:
            puts("I/O error");
            break;
        case 3:
            puts("no response");
            break;
        default:
            printf("aborted (%d)\n", rc);
            break;
        }
        if (rc != 3) {
            break;
        }
        NutSleep(1000);
    }
    return rc;
#else
    /* Nothing to be done for Ethernet. */
    return 0;
#endif
}

/*
 * Hang up.
 */
static void ProtocolPortClose(int pppcom)
{
#ifdef PPP_DEV
    /* Set the UART back to normal mode. */
    _ioctl(pppcom, HDLC_SETIFNET, NULL);
    /* Close the physical port. This may or may not hang up. Please
       check the modem's documentation. */
    _close(pppcom);
#endif
}

static int ProtocolPortConfigure(void)
{
#ifdef PPP_DEV
    PPPDCB *dcb;

    /*
     * We are connected, configure our PPP network interface.
     * This will initiate the PPP configuration negotiation
     * and authentication with the server.
     */
    printf("Configure network interface...");
    if (NutNetIfConfig("ppp", 0, 0, 0)) {
        puts("failed");
        return -1;
    }
    puts("done");

    /*
     * Set name server and default route. Actually the PPP interface
     * should do this, but the current release doesn't.
     */
    dcb = devPpp.dev_dcb;
    NutDnsConfig2(0, 0, dcb->dcb_ip_dns1, dcb->dcb_ip_dns2);
    NutIpRouteAdd(0, 0, dcb->dcb_remote_ip, &devPpp);

    /*
     * Display our IP settings.
     */
    printf("     Local IP: %s\n", inet_ntoa(dcb->dcb_local_ip));
    printf("    Remote IP: %s\n", inet_ntoa(dcb->dcb_remote_ip));
    printf("  Primary DNS: %s\n", inet_ntoa(dcb->dcb_ip_dns1));
    printf("Secondary DNS: %s\n", inet_ntoa(dcb->dcb_ip_dns2));
#else
    /* We use DHCP on Ethernet. */
    printf("Configure network interface...");
    if (NutDhcpIfConfig(DEV_ETHER_NAME, 0, 60000)) {
        puts("failed");
        return -1;
    }
    puts("done");
#endif

    return 0;
}

/*
 * Connect any NTP-Pool server to query the current time.
 */
static void QueryDateAndTime(void)
{
    uint32_t ip;
    time_t now;
    int i;

    /* First connect the DNS server to get the IP address. */
    printf("Is There Anybody Out There? ");
    ip = NutDnsGetHostByName((uint8_t *) "pool.ntp.org");
    if (ip == 0) {
        puts("No?");
    } else {
        /* We do have an IP, now try to connect it. */
        printf("Yes, found %s\n", inet_ntoa(ip));
        for (i = 0; i < 3; i++) {
            printf("What's the time? ");
            if (NutSNTPGetTime(&ip, &now) == 0) {
                /* We got a valid response, display the result. */
                printf("%s GMT\n", Rfc1123TimeString(gmtime(&now)));
                return;
            } else {
                /* It failed. May be the server is too busy. Try
                   again in 5 seconds. */
                puts("Sorry?");
                NutSleep(5000);
            }
        }
    }
    /* We give up after 3 trials. */
    puts("You missed the starting gun.");
}

/*
 * PPP client application entry.
 */
int main(void)
{
    int pppcom;

    /* Initialize a debug port for standard output and display a banner. */
    DebugPortOpen();
    printf("\n\nPPP Client Sample - Nut/OS %s\n", NutVersionString());

    /* Initialize the network interface. */
    ProtocolPortInit();
    /* This loop runs once per hour. */
    for (;;) {
        /* Open the network interface. */
        pppcom = ProtocolPortOpen();
        if (pppcom == -1) {
            /* If this fails, we are busted. */
            break;
        }
        /* Establish the modem connection. */
        if (ProtocolPortConnect(pppcom) == 0) {
            /* Configure the network interface. */
            if (ProtocolPortConfigure() == 0) {
                /* Talk to the Internet. */
                QueryDateAndTime();
            }
        }
        /* Hang up. */
        ProtocolPortClose(pppcom);

        /* Sleep 1 hour. */
        puts("I'll be back in 1 hour.");
        NutSleep(3600000);
    }
    /* Ouch... */
    puts("Good bye cruel world");

    return 0;
}
