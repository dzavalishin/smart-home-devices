/*!
 * Copyright (C) 2001-2005 by egnite Software GmbH
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
 * $Id: logtime.c 3538 2011-08-10 16:34:10Z haraldkipp $
 */

#include <stdio.h>
#include <io.h>

#include <arpa/inet.h>
#include <net/route.h>
#include <pro/dhcp.h>
#include <pro/sntp.h>

#include <sys/version.h>
#include <sys/confnet.h>
#include <sys/timer.h>
#include <sys/heap.h>
#include <sys/syslog.h>

#include <dev/board.h>

/*!
 * \example logtime/logtime.c
 *
 * Shows how to use syslog and SNTP.
 */
static char *version = "1.0.1";

/*
 * User configuration.
 */

/*! \brief Default MAC. 
 *  Used if EEPROM is empty. */
#define MYMAC   0x00, 0x06, 0x98, 0x00, 0x00, 0x00

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

/*! \brief IP address of the host running a syslog daemon. */
#define MYLOGD  "192.168.192.222"

/*! \brief IP address of the host running a time daemon. */
#define MYTIMED "130.149.17.21"

/*! \brief Device name used for UART output. */
#define MYUART  DEV_CONSOLE_NAME

/*! \brief UART baudrate. */
#define MYBAUD  115200

/*! \brief Output device. */
#define MYDEV   DEV_CONSOLE

/*! \brief Local timezone, -1 for Central Europe. */
#define MYTZ    -1


#ifdef __IMAGECRAFT__
#define COMPILERNAME "ICCAVR"
#else
#define COMPILERNAME "GCC"
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

/*
 * Application entry.
 */
int main(void)
{
    uint32_t baud = MYBAUD;
    uint8_t mac[6] = { MYMAC };
    uint32_t timeserver = inet_addr(MYTIMED);
    int rc = 0;
    time_t now;

    /*
     * Register UART devices, assign stdout and stderr to this device
     * and set the baudrate.
     */
    if(NutRegisterDevice(&MYDEV, 0, 0) == 0) {
        rc |= UART_OK;
        if(freopen(MYUART, "w", stdout)) {
            rc |= STDOUT_OK;
            if(_ioctl(_fileno(stdout), UART_SETSPEED, &baud) == 0) {
                rc |= BAUDRATE_OK;
            }
        }
        if(freopen(MYUART, "w", stderr)) {
            rc |= STDERR_OK;
        }
    }

    /*
     * Print banner.
     */
    if(rc & STDOUT_OK) {
        printf("\n\nTimeLog %s\nNut/OS %s\n", version, NutVersionString());
        puts("Compiled by " COMPILERNAME);
        puts("Configure network");
    }

#ifdef DEV_ETHER
    /*
     * Register LAN device and configure network interface.
     */
    if(NutRegisterDevice(&DEV_ETHER, 0x8300, 5) == 0) {
        rc |= LANDEV_OK;
        if (NutDhcpIfConfig("eth0", 0, 60000) == 0) {
            rc |= NETIF_OK;
        }
        else if (NutDhcpIfConfig("eth0", mac, 60000) == 0) {
            rc |= NETIF_OK;
        }
        else if(NutNetIfConfig("eth0", mac, inet_addr(MYIP), inet_addr(MYMASK)) == 0) {
            rc |= NETIF_OK;
#ifdef MYGATE
            if(NutIpRouteAdd(0, 0, inet_addr(MYGATE), &DEV_ETHER) == 0) {
                rc |= NETROUTE_OK;
            }
#endif
        }
    }

    if(rc & NETIF_OK) {
        /*
         * Set timezone, query SNTP server and set system time.
         */
        if(rc & STDOUT_OK) {
            puts("Query time from " MYTIMED);
        }
        _timezone = MYTZ * 60L * 60L;
        if(NutSNTPGetTime(&timeserver, &now) == 0) {
            rc |= TIMED_OK;
            stime(&now);
        }
    }

    /*
     * Open syslog output and route messages to stderr and to
     * a remote server.
     */
    if(rc & STDOUT_OK) {
        puts("Initialize syslog");
    }
    openlog("logtime", (rc & STDERR_OK) ? LOG_PERROR : 0, LOG_USER);
    if(rc & NETIF_OK) {
        setlogserver(inet_addr(MYLOGD), 0);
    }
    syslog(LOG_INFO, "TimeLog %s started on Nut/OS %s", version, NutVersionString());

    /*
     * Print the result of our initialization.
     */
    if((rc & UART_OK) == 0) {
        syslog(LOG_ERR, "Registering UART device failed");
    }
    else if((rc & STDOUT_OK) == 0) {
        syslog(LOG_ERR, "Assigning stdout failed");
    }
    else if((rc & STDERR_OK) == 0) {
        syslog(LOG_ERR, "Assigning stderr failed");
    }
    else if((rc & BAUDRATE_OK) == 0) {
        syslog(LOG_ERR, "Setting baudrate failed");
    }
    if((rc & LANDEV_OK) == 0) {
        syslog(LOG_ERR, "Registering Ethernet device failed");
    }
    else if((rc & NETIF_OK) == 0) {
        syslog(LOG_ERR, "Configuring network interface failed");
    }
    else {
        syslog(LOG_INFO, "IP %s", inet_ntoa(confnet.cdn_ip_addr));
        syslog(LOG_INFO, "Gate %s", inet_ntoa(confnet.cdn_gateway));
        syslog(LOG_INFO, "Timed " MYTIMED);
        syslog(LOG_INFO, "Syslogd " MYLOGD);
    }

    /*
     * Endless loop.
     */
    for(;;) {
        syslog(LOG_DEBUG, "%d bytes free", NutHeapAvailable());
        NutSleep(60000);
    }
#endif
    return 0;
}
