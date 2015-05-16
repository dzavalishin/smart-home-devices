/*
 * Copyright (C) 2009 by egnite GmbH
 * Copyright (C) 2005 by egnite Software GmbH
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
 * $Id: ftpserv.c 3538 2011-08-10 16:34:10Z haraldkipp $
 */

#include <stdio.h>
#include <fcntl.h>
#include <io.h>

#include <dev/board.h>
#include <dev/pnut.h>
#include <dev/sbimmc.h>
#include <fs/phatfs.h>

#include <sys/confnet.h>
#include <sys/version.h>
#include <sys/heap.h>
#include <sys/thread.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <net/route.h>
#include <pro/dhcp.h>
#include <pro/ftpd.h>
#include <pro/wins.h>
#include <pro/sntp.h>
#include <pro/discover.h>

/* Determine the compiler. */
#if defined(__IMAGECRAFT__)
#if defined(__AVR__)
#define CC_STRING   "ICCAVR"
#else
#define CC_STRING   "ICC"
#endif
#elif defined(__GNUC__)
#if defined(__AVR__)
#define CC_STRING   "AVRGCC"
#elif defined(__arm__)
#define CC_STRING   "ARMGCC"
#else
#define CC_STRING   "GCC"
#endif
#else
#define CC_STRING   "Compiler unknown"
#endif

/*!
 * \example ftpd/ftpserv.c
 *
 * FTP server sample.
 *
 * This application requires the PNUT file system, which is
 * not available on Ethernut 1.x or Charon II.
 */

/* 
 * Baudrate for debug output. 
 */
#ifndef DBG_BAUDRATE
#define DBG_BAUDRATE 115200
#endif

/*
 * Wether we should use DHCP.
 */
#define USE_DHCP

/*
 * Wether we should run a discovery responder.
 */
#if defined(__arm__)
#define USE_DISCOVERY
#endif

/* 
 * Unique MAC address of the Ethernut Board. 
 *
 * Ignored if EEPROM contains a valid configuration.
 */
#define MY_MAC { 0x00, 0x06, 0x98, 0x30, 0x00, 0x35 }

/* 
 * Unique IP address of the Ethernut Board. 
 *
 * Ignored if DHCP is used. 
 */
#define MY_IPADDR "192.168.192.35"

/* 
 * IP network mask of the Ethernut Board.
 *
 * Ignored if DHCP is used. 
 */
#define MY_IPMASK "255.255.255.0"

/* 
 * Gateway IP address for the Ethernut Board.
 *
 * Ignored if DHCP is used. 
 */
#define MY_IPGATE "192.168.192.1"

/* 
 * NetBIOS name.
 *
 * Use a symbolic name with Win32 Explorer.
 */
//#define MY_WINSNAME "ETHERNUT"

/*
 * FTP port number.
 */
#define FTP_PORTNUM 21

/*
 * FTP timeout.
 *
 * The server will terminate the session, if no new command is received
 * within the specified number of milliseconds.
 */
#define FTPD_TIMEOUT 600000

/*
 * TCP buffer size.
 */
#define TCPIP_BUFSIZ 5840

/*
 * Maximum segment size. 
 *
 * Choose 536 up to 1460. Note, that segment sizes above 536 may result 
 * in fragmented packets. Remember, that Ethernut doesn't support TCP 
 * fragmentation.
 */
#define TCPIP_MSS 1460

#if defined(ETHERNUT3)

/* Ethernut 3 file system. */
#define FSDEV       devPhat0
#define FSDEV_NAME  "PHAT0" 

/* Ethernut 3 block device interface. */
#define BLKDEV      devNplMmc0
#define BLKDEV_NAME "MMC0"

#elif defined(AT91SAM7X_EK)

/* SAM7X-EK file system. */
#define FSDEV       devPhat0
#define FSDEV_NAME  "PHAT0" 

/* SAM7X-EK block device interface. */
#define BLKDEV      devAt91SpiMmc0
#define BLKDEV_NAME "MMC0"

#elif defined(AT91SAM9260_EK)

/* SAM9260-EK file system. */
#define FSDEV       devPhat0
#define FSDEV_NAME  "PHAT0" 

/* SAM9260-EK block device interface. */
#define BLKDEV      devAt91Mci0
#define BLKDEV_NAME "MCI0"

#elif defined(ETHERNUT2)

/*
 * Ethernut 2 File system
 */
#define FSDEV       devPnut
#define FSDEV_NAME  "PNUT" 

#else

#define FSDEV_NAME  "NONE" 

#endif

/*! \brief Local timezone, -1 for Central Europe. */
#define MYTZ    -1

/*! \brief IP address of the host running a time daemon. */
#if defined(DEV_ETHER)
#define MYTIMED "130.149.17.21"
#endif

#ifdef ETHERNUT3
/*! \brief Defined if X1226 RTC is available. */
#define X12RTC_DEV
#endif

/*
 * FTP service.
 *
 * This function waits for client connect, processes the FTP request 
 * and disconnects. Nut/Net doesn't support a server backlog. If one 
 * client has established a connection, further connect attempts will 
 * be rejected. 
 *
 * Some FTP clients, like the Win32 Explorer, open more than one 
 * connection for background processing. So we run this routine by
 * several threads.
 */
static void FtpService(void)
{
#if defined(DEV_ETHER)
    TCPSOCKET *sock;

    /*
     * Create a socket.
     */
    if ((sock = NutTcpCreateSocket()) != 0) {

        /* 
         * Set specified socket options. 
         */
#ifdef TCPIP_MSS
        {
            uint16_t mss = TCPIP_MSS;
            NutTcpSetSockOpt(sock, TCP_MAXSEG, &mss, sizeof(mss));
        }
#endif
#ifdef FTPD_TIMEOUT
        {
            uint32_t tmo = FTPD_TIMEOUT;
            NutTcpSetSockOpt(sock, SO_RCVTIMEO, &tmo, sizeof(tmo));
        }
#endif
#ifdef TCPIP_BUFSIZ
        {
            uint16_t siz = TCPIP_BUFSIZ;
            NutTcpSetSockOpt(sock, SO_RCVBUF, &siz, sizeof(siz));
        }
#endif

        /*
         * Listen on our port. If we return, we got a client.
         */
        printf("\nWaiting for an FTP client...");
        if (NutTcpAccept(sock, FTP_PORTNUM) == 0) {
            printf("%s connected, %u bytes free\n", inet_ntoa(sock->so_remote_addr), (unsigned int)NutHeapAvailable());
            NutFtpServerSession(sock);
            printf("%s disconnected, %u bytes free\n", inet_ntoa(sock->so_remote_addr), (unsigned int)NutHeapAvailable());
        } else {
            puts("Accept failed");
        }

        /*
         * Close our socket.
         */
        NutTcpCloseSocket(sock);
    }
#endif
}

/*
 * FTP service thread.
 */
THREAD(FtpThread, arg)
{
    /* Loop endless for connections. */
    for (;;) {
        FtpService();
    }
}

/*
 * Assign stdout to the UART device.
 */
void InitDebugDevice(void)
{
    uint32_t baud = DBG_BAUDRATE;

    NutRegisterDevice(&DEV_CONSOLE, 0, 0);
    freopen(DEV_CONSOLE_NAME, "w", stdout);
    _ioctl(_fileno(stdout), UART_SETSPEED, &baud);
}

/*
 * Setup the ethernet device. Try DHCP first. If this is
 * the first time boot with empty EEPROM and no DHCP server
 * was found, use hardcoded values.
 */
static int InitEthernetDevice(void)
{
#if defined(DEV_ETHER)
    uint32_t ip_addr = inet_addr(MY_IPADDR);
    uint32_t ip_mask = inet_addr(MY_IPMASK);
    uint32_t ip_gate = inet_addr(MY_IPGATE);
    uint8_t mac[6] = MY_MAC;

    if (NutRegisterDevice(&DEV_ETHER, 0x8300, 5) == 0) {
        printf("Configure %s...", DEV_ETHER_NAME);
#ifdef USE_DHCP
        if (NutDhcpIfConfig(DEV_ETHER_NAME, 0, 60000) == 0) {
            puts("OK");
            return 0;
        }
        printf("initial boot...");
        if (NutDhcpIfConfig(DEV_ETHER_NAME, mac, 60000) == 0) {
            puts("OK");
            return 0;
        }
#endif
        printf("No DHCP...");
        NutNetIfConfig(DEV_ETHER_NAME, mac, ip_addr, ip_mask);
        /* Without DHCP we had to set the default gateway manually.*/
        if(ip_gate) {
            printf("hard coded gate...");
            NutIpRouteAdd(0, 0, ip_gate, &DEV_ETHER);
        }
        puts("OK");
        return 0;
    }
#endif /* DEV_ETHER */
    puts("No Ethernet Device");

    return -1;
}

/*
 * Query a time server and optionally update the hardware clock.
 */
static int QueryTimeServer(void)
{
    int rc = -1;

#ifdef MYTIMED
    {
        time_t now;
        uint32_t timeserver = inet_addr(MYTIMED);

        /* Query network time service and set the system time. */
        printf("Query time from %s...", MYTIMED);
        if(NutSNTPGetTime(&timeserver, &now) == 0) {
            puts("OK");
            rc = 0;
            stime(&now);
#ifdef X12RTC_DEV
            /* If RTC hardware is available, update it. */
            {
                struct _tm *gmt = gmtime(&now);

                if (X12RtcSetClock(gmt)) {
                    puts("RTC update failed");
                }
            }
#endif
        }
        else {
            puts("failed");
        }
    }
#endif

    return rc;
}

/*
 * Try to get initial date and time from the hardware clock or a time server.
 */
static int InitTimeAndDate(void)
{
    int rc = -1;

    /* Set the local time zone. */
    _timezone = MYTZ * 60L * 60L;

#ifdef X12RTC_DEV
    /* Query RTC hardware if available. */
    {
        uint32_t rs;

        /* Query the status. If it fails, we do not have an RTC. */
        if (X12RtcGetStatus(&rs)) {
            puts("No hardware RTC");
            rc = QueryTimeServer();
        }
        else {
            /* RTC hardware seems to be available. Check for power failure. */
            //rs = RTC_STATUS_PF;
            if ((rs & RTC_STATUS_PF) == RTC_STATUS_PF) {
                puts("RTC power fail detected");
                rc = QueryTimeServer();
            }

            /* RTC hardware status is fine, update our system clock. */
            else {
                struct _tm gmt;

                /* Assume that RTC is running at GMT. */
                if (X12RtcGetClock(&gmt) == 0) {
                    time_t now = _mkgmtime(&gmt);

                    if (now != -1) {
                        stime(&now);
                        rc = 0;
                    }
                }
            }
        }
    }
#else
    /* No hardware RTC, query the time server if available. */
    rc = QueryTimeServer();
#endif
    return rc;
}

/*
 * Main application routine. 
 *
 * Nut/OS automatically calls this entry after initialization.
 */
int main(void)
{
    int volid;
    uint32_t ipgate;

    /* Initialize a debug output device and print a banner. */
    InitDebugDevice();
    printf("\n\nFTP Server Sample - Nut/OS %s - " CC_STRING "\n", NutVersionString());

    /* Initialize the Ethernet device and print our IP address. */
    if (InitEthernetDevice()) {
        for(;;);
    }
#if defined(DEV_ETHER)
    printf("IP Addr: %s\n", inet_ntoa(confnet.cdn_ip_addr));
    printf("IP Mask: %s\n", inet_ntoa(confnet.cdn_ip_mask));
    NutIpRouteQuery(0, &ipgate);
    printf("IP Gate: %s\n", inet_ntoa(ipgate));
#endif

#ifdef USE_DISCOVERY
    /* Register a discovery responder. */
    printf("Start Responder...");
    if (NutRegisterDiscovery((uint32_t)-1, 0, DISF_INITAL_ANN)) {
        puts("failed");
    }
    else {
        puts("OK");
    }
#endif

    /* Initialize system clock and calendar. */
    if (InitTimeAndDate() == 0) {
        time_t now = time(0);
        struct _tm *lot = localtime(&now);
        printf("Date: %02u.%02u.%u\n", lot->tm_mday, lot->tm_mon + 1, 1900 + lot->tm_year);
        printf("Time: %02u:%02u:%02u\n", lot->tm_hour, lot->tm_min, lot->tm_sec);
    }

#ifdef FSDEV
    /* Initialize the file system. */
    printf("Register file system...");
    if (NutRegisterDevice(&FSDEV, 0, 0)) {
        puts("failed");
        for (;;);
    }
    puts("OK");
#endif

#ifdef BLKDEV
    /* Register block device. */
    printf("Register block device...");
    if (NutRegisterDevice(&BLKDEV, 0, 0)) {
        puts("failed");
        for (;;);
    }
    puts("OK");

    /* Mount partition. */
    printf("Mounting partition...");
    if ((volid = _open(BLKDEV_NAME ":1/" FSDEV_NAME, _O_RDWR | _O_BINARY)) == -1) {
        puts("failed");
        for (;;);
    }
    puts("OK");
#else
    volid = 0;
#endif

#if defined(DEV_ETHER)
    /* Register root path. */
    printf("Register FTP root...");
    if (NutRegisterFtpRoot(FSDEV_NAME ":")) {
        puts("failed");
        for (;;);
    }
    puts("OK");
#endif

    /* Start two additional server threads. */
    NutThreadCreate("ftpd0", FtpThread, 0, 1640);
    NutThreadCreate("ftpd1", FtpThread, 0, 1640);

    /* Main server thread. */
    for (;;) {
#ifdef MY_WINSNAME
        NutWinsNameQuery(MY_WINSNAME, confnet.cdn_ip_addr);
#endif
        FtpService();
    }
    return 0;
}
