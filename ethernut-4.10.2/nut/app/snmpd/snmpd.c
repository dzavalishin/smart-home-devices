/*
 * Copyright 2007 by egnite Software GmbH
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

#include <dev/board.h>

#include <sys/types.h>
#include <ctype.h>
#include <errno.h>

#include <arpa/inet.h>
#include <net/route.h>
#include <pro/dhcp.h>

#include <sys/version.h>
#include <sys/timer.h>

#include <pro/snmp_config.h>
#include <pro/snmp_mib.h>
#include <pro/snmp_api.h>
#include <pro/snmp_agent.h>

#include <stdio.h>
#include <io.h>

#include "mib2sys.h"
#include "mib2if.h"

/*!
 * \example snmpd/snmpd.c
 *
 * Basic SNMP Agent.
 */
static char *version = "0.2.0";

/*! \brief Default MAC.
 *  Used if EEPROM is empty. */
#define MYMAC   0x00, 0x06, 0x98, 0x33, 0x44, 0x00

/*! \brief Default local IP address.
 *  Used if EEPROM configuration and DHCP is unavailable. */
#define MYIP    "192.168.192.100"

/*! \brief Default IP mask.
 *  Used if EEPROM configuration and DHCP is unavailable. */
#define MYMASK  "255.255.255.0"

/*! \brief Default gateway address.
 *  Used if EEPROM configuration and DHCP is unavailable.
 *  Only required if syslog or time server are located in
 *  a different network. */
#define MYGATE  "192.168.192.1"

/*! \brief Device name used for UART output. */
#define MYUART  DEV_CONSOLE_NAME

/*! \brief Output device. */
#define MYDEV   DEV_CONSOLE

/*! \brief UART baudrate. */
#define MYBAUD  115200

/* Determine the compiler. */
#if defined(__IMAGECRAFT__)
#if defined(__AVR__)
#define COMPILERNAME   "ICCAVR"
#else
#define COMPILERNAME   "ICC"
#endif
#elif defined(__GNUC__)
#if defined(__AVR__)
#define COMPILERNAME   "AVRGCC"
#elif defined(__arm__)
#define COMPILERNAME   "ARMGCC"
#else
#define COMPILERNAME   "GCC"
#endif
#else
#define COMPILERNAME   "Compiler unknown"
#endif

/* Result codes. */
#define UART_OK     0x0001
#define STDOUT_OK   0x0002
#define STDERR_OK   0x0004
#define BAUDRATE_OK 0x0008
#define LANDEV_OK   0x0010
#define NETIF_OK    0x0020
#define NETROUTE_OK 0x0040
#define TIMED_OK    0x0080

int main(void)
{
    UDPSOCKET *sock;
    OID view_all[] = { SNMP_OID_INTERNET };
    int view_idx;
    uint32_t baud = MYBAUD;
    uint8_t mac[6] = { MYMAC };
    int rc = 0;

    /*
     * Register UART devices, assign stdout and stderr to this device
     * and set the baudrate.
     */
    if (NutRegisterDevice(&MYDEV, 0, 0) == 0) {
        rc |= UART_OK;
        if (freopen(MYUART, "w", stdout)) {
            rc |= STDOUT_OK;
            if (_ioctl(_fileno(stdout), UART_SETSPEED, &baud) == 0) {
                rc |= BAUDRATE_OK;
            }
        }
        if (freopen(MYUART, "w", stderr)) {
            rc |= STDERR_OK;
        }
    }

    /*
     * Print banner.
     */
    if (rc & STDOUT_OK) {
        printf("\n\nSNMP Agent %s\nNut/OS %s\n", version, NutVersionString());
        puts("Compiled by " COMPILERNAME);
        puts("Configure network");
    }

    /*
     * Register LAN device and configure network interface.
     */
#ifdef DEV_ETHER
    if (NutRegisterDevice(&DEV_ETHER, 0x8300, 5) == 0) {
        rc |= LANDEV_OK;
        if (NutDhcpIfConfig("eth0", 0, 60000) == 0) {
            rc |= NETIF_OK;
        } else if (NutDhcpIfConfig("eth0", mac, 60000) == 0) {
            rc |= NETIF_OK;
        } else if (NutNetIfConfig("eth0", mac, inet_addr(MYIP), inet_addr(MYMASK)) == 0) {
            rc |= NETIF_OK;
#ifdef MYGATE
            if (NutIpRouteAdd(0, 0, inet_addr(MYGATE), &DEV_ETHER) == 0) {
                rc |= NETROUTE_OK;
            }
#endif
        }
    }

    sock = NutUdpCreateSocket(SNMP_PORT);
    /* Nut/Net doesn't provide UDP datagram buffering by default. */
    {
        uint16_t max_ms = SNMP_MAX_MSG_SIZE * 2;
        NutUdpSetSockOpt(sock, SO_RCVBUF, &max_ms, sizeof(max_ms));
    }

    /* Register system variables. */
    if (MibRegisterSysVars()) {
        printf("Failed to register MibSys\n");
        for (;;)
            NutSleep(1000);
    }
    /* Register interface variables. */
    if (MibRegisterIfVars()) {
        printf("Failed to register MibIf\n");
        for (;;)
            NutSleep(1000);
    }

    /* Create views. */
    if ((view_idx = SnmpViewCreate("all", view_all, sizeof(view_all), SNMP_VIEW_INCLUDED)) <= 0) {
        printf("Failed to create view\n");
        for (;;)
            NutSleep(1000);
    }
    /* Create communities. */
    if (SnmpCommunityCreate("public", view_idx, view_idx) || SnmpCommunityCreate("private", view_idx, view_idx)) {
        printf("Failed to create communities\n");
        for (;;)
            NutSleep(1000);
    }

    /* Run agent. */
    SnmpAgent(sock);

    /* Program stopped. */
    NutUdpDestroySocket(sock);
#endif

    for (;;) {
        NutSleep(100);
        printf("Hello ");
    }
    return 0;
}
