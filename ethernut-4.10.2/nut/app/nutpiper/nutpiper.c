/*
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
 * $Id: nutpiper.c 3538 2011-08-10 16:34:10Z haraldkipp $
 */

/*!
 * \example nutpiper/nutpiper.c
 *
 * Shoutcast radio application..
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <io.h>

#include <dev/board.h>
#include <dev/debug.h>
#include <dev/term.h>
#include <dev/hd44780.h>
#include <dev/vs1001k.h>
#ifdef ETHERNUT2
#include <dev/lanc111.h>
#else
#include <dev/nicrtl.h>
#endif

#include <sys/heap.h>
#include <sys/thread.h>
#include <sys/timer.h>

#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <net/route.h>

#include <pro/dhcp.h>

#include <sys/bankmem.h>
#include <dev/irsony.h>

#include "config.h"
#include "display.h"
#include "scanner.h"
#include "player.h"

#if defined(__AVR__)

/*
 * TCP buffer size.
 */
static uint16_t tcpbufsiz = 4288;

/*
 * Maximum segment size, choose 536 up to 1460. Note, that segment 
 * sizes above 536 may result in fragmented packets. Remember, that 
 * Ethernut doesn't support TCP fragmentation.
 */
static uint16_t mss = 536;

/*
 * Socket receive timeout.
 */
static uint32_t rx_to = 1000;

/*
 * Connect to a radio station.
 */
static TCPSOCKET *ConnectStation(RADIOSTATION * rsp)
{
    TCPSOCKET *sock;
    int rc;

    /* Create a socket. TODO: A failure is fatal and should restart
       the system. */
    if ((sock = NutTcpCreateSocket()) == 0)
        return sock;

    /* Set socket options. Failures are ignored. */
    if (NutTcpSetSockOpt(sock, TCP_MAXSEG, &mss, sizeof(mss)))
        printf("Sockopt MSS failed\n");
    if (NutTcpSetSockOpt(sock, SO_RCVTIMEO, &rx_to, sizeof(rx_to)))
        printf("Sockopt TO failed\n");
    if (NutTcpSetSockOpt(sock, SO_RCVBUF, &tcpbufsiz, sizeof(tcpbufsiz)))
        printf("Sockopt rxbuf failed\n");

    /* Connect the stream server. */
    printf("[CNCT %s:%u]", inet_ntoa(rsp->rs_ip), rsp->rs_port);
    if ((rc = NutTcpConnect(sock, rsp->rs_ip, rsp->rs_port)) == 0) {
        printf("[CNCTD]");
        /* Process header from server. */
        if (ScanStreamHeader(sock, rsp) == 0) {
            /* TODO: Failed to start the player is fatal. */
            if (PlayerStart(sock, rsp->rs_metaint, 30000)) {
                NutTcpCloseSocket(sock);
                sock = 0;
            }
        } else {
            NutTcpCloseSocket(sock);
            sock = 0;
        }
    } else {
        printf("[CERR=%d]\n", NutTcpError(sock));
        NutTcpCloseSocket(sock);
        sock = 0;
    }
    return sock;
}

/*
 * Disconnect from a radio station.
 */
static void DisconnectStation(TCPSOCKET * sock)
{
    if (PlayerStop(3000)) {
        printf("[PPFAIL]");
    }
    printf("Disconnecting\n");
    NutTcpCloseSocket(sock);
}

/*
 * Get next command from user interface.
 *
 * We are supporting the remote control only for now.
 */
#ifdef ARTHERNET1

static uint16_t UserInput(void)
{
     /* IR will not work on Arthernet, because interrupt 5
        is in use and can't be re-configured in the IR driver. */
    return 0xFFFF;
}

#else /* ARTHERNET1 */

static uint16_t UserInput(void)
{
    uint16_t rc;
    static uint16_t old = 0xFFFF;

    if ((rc = (uint16_t) NutIrGet(500)) == 0xFFFF)
        old = rc;
    else if (rc == old || (rc >> 7) != IR_DEVICE)
        rc = 0xFFFF;
    else {
        old = rc;
        rc &= 0x7F;
        if (rc == IRCMD_VOL_UP || rc == IRCMD_VOL_DN)
            old = 0xFFFF;
    }
    return rc;
}

#endif /* ARTHERNET1 */

/*
 * If we got a background scanner 
 */
static void StationList(void)
{
    uint8_t cf = 1;
    uint8_t rs = radio.rc_cstation;
    uint16_t ircode;

    DisplayMessage(0, 1, "Stations");
    DisplayEntry(rs);
    while (cf) {
        /*
         * Process user input.
         */
        if ((ircode = UserInput()) != 0xFFFF) {
            switch (ircode) {
            case IRCMD_CHAN_UP:
                if (++rs >= MAXNUM_STATIONS)
                    rs = 0;
                DisplayEntry(rs);
                break;
            case IRCMD_CHAN_DN:
                if (rs)
                    rs--;
                else
                    rs = MAXNUM_STATIONS - 1;
                DisplayEntry(rs);
                break;
            case IRCMD_SELECT:
                radio.rc_rstation = rs;
            default:
                cf = 0;
                break;
            }
        }
    }
    DisplayStatus(DIST_FORCE);
}

/*!
 * \brief Loop for user input and update display.
 */
static void UserInterface(void)
{
    uint8_t ief;
    uint16_t ircode;
    TCPSOCKET *sock = 0;

    for (;;) {

        /*
         * Process user input.
         */
        if ((ircode = UserInput()) != 0xFFFF) {
            radio.rc_off = 0;
            switch (ircode) {
            case IRCMD_CHAN_1:
                radio.rc_rstation = 1;
                break;
            case IRCMD_CHAN_2:
                radio.rc_rstation = 2;
                break;
            case IRCMD_CHAN_3:
                radio.rc_rstation = 3;
                break;
            case IRCMD_CHAN_4:
                radio.rc_rstation = 4;
                break;
            case IRCMD_CHAN_5:
                radio.rc_rstation = 5;
                break;
            case IRCMD_CHAN_6:
                radio.rc_rstation = 6;
                break;
            case IRCMD_CHAN_7:
                radio.rc_rstation = 7;
                break;
            case IRCMD_CHAN_8:
                radio.rc_rstation = 8;
                break;
            case IRCMD_CHAN_9:
                radio.rc_rstation = 9;
                break;
            case IRCMD_CHAN_UP:
                radio.rc_rstation = NEXT_STATION;
                break;
            case IRCMD_CHAN_DN:
                radio.rc_rstation = PREV_STATION;
                break;
            case IRCMD_VOL_UP:
                player.psi_start = 1;
                if (radio.rc_rvolume < 245)
                    radio.rc_rvolume += 10;
                break;
            case IRCMD_VOL_DN:
                player.psi_start = 1;
                if (radio.rc_rvolume > 10)
                    radio.rc_rvolume -= 10;
                break;
            case IRCMD_MUTE:
                radio.rc_rmute = !radio.rc_rmute;
                break;
            case IRCMD_OFF:
                radio.rc_off = 1;
                DisplayMessage(0, 0, "Shutdown...");
                DisplayMessage(1, 0, "");
                PlayerStop(1000);
                ConfigSave();
                radio.rc_cstation = PREV_STATION;
                DisplayMessage(0, 0, "");
                break;
            case IRCMD_VTEXT:
                StationList();
                break;
            default:
                DisplayMessage(0, 1, "Code %u", ircode);
                ief = VsPlayerInterrupts(0);
                printf("%lu kBytes used, %lu kBytes free\n", NutSegBufUsed() / 1024UL, NutSegBufAvailable() / 1024UL);
                VsPlayerInterrupts(ief);
                break;
            }
        }

        if (radio.rc_off) {
            NutSleep(500);
            continue;
        }

        /*
         * Handle station changes.
         */
        if (radio.rc_rstation != radio.rc_cstation) {
            /* Disconnect current stream. */
            if (sock) {
                DisconnectStation(sock);
                sock = 0;
            }

            /* If scanning, move to the next/previous station. */
            if (radio.rc_rstation == NEXT_STATION) {
                if (++radio.rc_cstation >= MAXNUM_STATIONS)
                    radio.rc_rstation = 0;
            } else if (radio.rc_rstation == PREV_STATION) {
                if (radio.rc_cstation)
                    radio.rc_cstation--;
                else
                    radio.rc_cstation = MAXNUM_STATIONS - 1;
            } else {
                radio.rc_cstation = radio.rc_rstation;
            }

            DisplayMessage(0, 1, "Station %03u", radio.rc_cstation);

            /* Check for valid IP address and port. */
            if (station[radio.rc_cstation].rs_ip && station[radio.rc_cstation].rs_port) {
                if (station[radio.rc_cstation].rs_scandead) {
                    DisplayStatus(DIST_DEAD);
                } else {
                    DisplayStatus(DIST_CONNECTING);
                    if ((sock = ConnectStation(&station[radio.rc_cstation])) != 0) {
                        radio.rc_rstation = radio.rc_cstation;
                        DisplayStatus(DIST_CONNECTED);
                    } else {
                        DisplayStatus(DIST_DEAD);
                    }
                }
            } else {
                DisplayStatus(DIST_DEAD);
            }
        } else if (radio.rc_cmute != radio.rc_rmute) {

            radio.rc_cmute = radio.rc_rmute;
            if (radio.rc_cmute) {
                DisplayMessage(0, 1, "Mute");
                VsSetVolume(255, 255);
            } else {
                DisplayMessage(0, 1, "Volume %u", radio.rc_cvolume);
                VsSetVolume(255 - radio.rc_cvolume, 255 - radio.rc_cvolume);
            }
        } else if (radio.rc_cvolume != radio.rc_rvolume) {
            DisplayMessage(0, 1, "Volume %u", radio.rc_rvolume);
            VsSetVolume(255 - radio.rc_rvolume, 255 - radio.rc_rvolume);
            radio.rc_cvolume = radio.rc_rvolume;
            radio.rc_rmute = 0;
        } else if (player.psi_metaupdate) {
            DisplayStatus(DIST_FORCE);
            player.psi_metaupdate = 0;
        }
    }
}

#endif /* __AVR__ */

/*
 * Main entry of the Internet Radio Application.
 */
int main(void)
{
#if defined(__AVR__)
    /* Unique MAC address of the Ethernut Board. */
    uint8_t mac[6] = { 0x00, 0x06, 0x98, 0x00, 0x00, 0x00 };
    /* Unique IP address of the Ethernut Board. Ignored if DHCP is used. */
    uint32_t ip_addr = inet_addr("192.168.192.100");
    /* IP network mask of the Ethernut Board. Ignored if DHCP is used. */
    uint32_t ip_mask = inet_addr("255.255.255.0");
    /* Gateway IP address for the Ethernut Board. Ignored if DHCP is used. */
    uint32_t ip_gate = inet_addr("192.168.192.3");
    /* Baudrate for debug output. */
#endif
    uint32_t baud = 115200;

    /*
     * Assign stdout to the debug device.
     */
    NutRegisterDevice(&DEV_CONSOLE, 0, 0);
    freopen(DEV_CONSOLE_NAME, "w", stdout);
    _ioctl(_fileno(stdout), UART_SETSPEED, &baud);

    /*
     * Load configuration and display banner.
     */
    if (ConfigLoad())
        ConfigResetFactory();

#if defined(__AVR__)
    /*
     * Initialize the MP3 device early to avoid noice, caused
     * by floating, tri-stated port lines.
     */
    if (NutSegBufInit(8192) == 0) {
        printf("Can't init segbuf\n");
        for (;;);
    }
    PlayerInit();

    /*
     * Initialize the LCD.
     */
    NutRegisterDevice(&devLcd, 0, 0);
    if (DisplayInit("lcd")) {
        printf("Display failure\n");
        for (;;);
    }

#ifndef ARTHERNET1
    /*
     * Initialize IR remote control.
     * This will not work on Arthernet, because interrupt 5
     * is in use and can't be re-configured in the IR driver.
     */
    NutIrInitSony();
#endif

    /*
     * LAN configuration using EEPROM values or DHCP/ARP method.
     * If it fails, use fixed values.
     */
    if (NutRegisterDevice(&DEV_ETHER, 0x8300, 5))
        puts("Registering device failed");
    if (NutDhcpIfConfig("eth0", 0, 60000)) {
        puts("EEPROM/DHCP/ARP config failed");
        NutNetIfConfig("eth0", mac, ip_addr, ip_mask);
        NutIpRouteAdd(0, 0, ip_gate, &DEV_ETHER);
    }
    puts("ready\n");

    VsSetVolume(255 - radio.rc_rvolume, 255 - radio.rc_rvolume);
    VsBeep(2, 100);

    /*
     * Start the background scanner.
     */
    //ScannerInit();

    /*
     * Call the user interface..
     */
    for (;;) {
        UserInterface();
    }
#endif /* __AVR__ */

    for (;;)
        NutSleep(1000);

    return 0;
}
