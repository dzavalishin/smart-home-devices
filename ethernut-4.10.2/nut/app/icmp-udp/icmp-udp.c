/*!
 * Copyright (C) 2009 by Ole Reinhardt <ole.reinhardt@thermotemp.de>. 
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
 * $Id: icmp-udp.c 3538 2011-08-10 16:34:10Z haraldkipp $
 */

/*!
 * \example icmp-udp/icmp-udp.c
 *
 * This sample demonstrates the icmp error handling for udp sockets.
 * NUT_UDP_ICMP_SUPPORT has to be enabled in the configurator to use
 * icmp support on UDP sockets.
 *
 * Connect the RS232 port of the Ethernut with a free COM
 * port of your PC and run a terminal emulator at 115200 Baud.
 *
 * Configure MY_IP, MY_MASK, and MY_GATE approriate. Configure UDP_ECHO_IP
 * to the IP of a host running an udp echo server.
 *
 * The program will try to send some UDP packets to the given 
 * destination address on port 7 (UDP echo port) and will try to 
 * read back the sended data.
 *
 * You can run the udp_echo java program in this directory with admin (root) 
 * priviledges to have a simple udp echo server on your pc.
 *
 * Just start the program with 'java udp_echo.class'
 * This program will echo back every data packet received on udp port 7.
 * The program can be stoped with <ctrl>+<c>
 *
 * If the udp_echo program (or any other udp echo server) is not running
 * you'll receive ICMP destination unreachable messages. Which will
 * be noticed from this demo app on the next send or receive call.
 */

#define MY_MAC          {0x00,0x06,0x98,0x20,0x00,0x00}
#define MY_IP           "192.168.1.100"
#define MY_MASK         "255.255.255.0"
#define MY_GATE         "192.168.1.254"

#define UDP_ECHO_IP     "192.168.1.103"
#define UDP_ECHO_PORT   7

#include <string.h>
#include <stdio.h>
#include <io.h>
//#include <inttypes.h>
#include <errno.h>

#include <dev/board.h>

#include <sys/thread.h>
#include <sys/timer.h>
#include <sys/socket.h>
#include <sys/confnet.h>

#include <arpa/inet.h>
#include <net/route.h>

#define UDP_BUFF_SIZE 256

static char send_buffer[UDP_BUFF_SIZE];
static char rcv_buffer[UDP_BUFF_SIZE];
static uint8_t my_mac[] = MY_MAC;

#ifdef DEV_ETHER

/* Print error message */
void print_udp_icmp_error(uint32_t remote_ip, uint16_t remote_port, int error)
{
    printf("ICMP destination unreachable received for remote ip: %s\r\nremote port: %d, errno: %d -- ", 
           inet_ntoa(remote_ip), remote_port, error);
    
    switch (error) {
        case ENETUNREACH:
            printf("Destination network unreachable\r\n");
            break;
        case EHOSTUNREACH:
            printf("Destination host unreachable\r\n");
            break;
        case ENOPROTOOPT:
            printf("Destination protocol unreachable\r\n");
            break;
        case ECONNREFUSED:	
            printf("Destination port unreachable / connection refused\r\n");
            break;
        case EMSGSIZE:
            printf("Fragmentation required, and DF flag set\r\n");
            break;
        case EOPNOTSUPP:
            printf("Source route failed\r\n");
            break;
        case EHOSTDOWN:
            printf("Destination host unknown or down\r\n");
            break;
        default:
            printf("Other error\r\n");
    }
}

THREAD(UDPReceiver, arg)
{
    uint32_t remote_ip;
    uint16_t remote_port;
    UDPSOCKET *socket = (UDPSOCKET*) arg;
    int      rc;
    
    while (1) {
        rc = NutUdpReceiveFrom(socket, &remote_ip, &remote_port, rcv_buffer, UDP_BUFF_SIZE, 2000);
        if (rc == 0) {
            printf("<-- Timeout (2 sec.) on UDP receive\r\n");
        } else 
        if (rc < 0) {
#ifdef NUT_UDP_ICMP_SUPPORT             
            int error;
            error = NutUdpError(socket, &remote_ip, &remote_port);
            print_udp_icmp_error(remote_ip, remote_port, error);
#endif            
        } else {
            printf("<-- Received packet: \"%s\"\r\n", rcv_buffer);
        }
    }
}

#endif /* DEV_ETHER */

/*
 * Main application routine. 
 *
 */
int main(void)
{
    uint32_t baud = 115200;
    UDPSOCKET *socket;

    uint32_t ip_addr;
    uint32_t ip_udp_echo;
    int    rc;
    int    packet_nr;
    uint16_t length;

    /*
     * Initialize the uart device.
     */
    NutRegisterDevice(&DEV_CONSOLE, 0, 0);
    freopen(DEV_CONSOLE_NAME, "w", stdout);
    _ioctl(_fileno(stdout), UART_SETSPEED, &baud);
    puts("Demo for ICMP support in UDP sockets...\r\n");

#ifdef DEV_ETHER
#ifndef NUT_UDP_ICMP_SUPPORT
#warning ICMP support for UDP sockets not enabled in the configurator, please enable NUT_UDP_ICMP_SUPPORT
    puts("ICMP support for UDP sockets not enabled in the configurator\r\n");
    puts("Please enable NUT_UDP_ICMP_SUPPORT\r\n");
#endif    
    /*
     * Register the network device.
     */
    puts("Configuring Ethernet interface");
    NutRegisterDevice(&DEV_ETHER, 0, 0);

    ip_addr = inet_addr(MY_IP);
    NutNetIfConfig("eth0", my_mac, ip_addr, inet_addr(MY_MASK));
    NutIpRouteAdd(0, 0, inet_addr(MY_GATE), &DEV_ETHER);

    printf("%s ready\r\n", inet_ntoa(ip_addr));


    socket = NutUdpCreateSocket(UDP_ECHO_PORT);
    if (socket == 0) {
        printf("Could not create UDP socket in port '%d'\r\n", UDP_ECHO_PORT);
        puts("Demo halted...\r\n");
        while (1);
    } else {
        printf("Successfully created UDP socket on port '%d'\r\n", UDP_ECHO_PORT);
        length = UDP_BUFF_SIZE;
        if (NutUdpSetSockOpt(socket, SO_RCVBUF, &length, sizeof(length))) {;
            printf("Could not set UDP receive buffer size (%d)\r\n", length);
            puts("Demo halted...\r\n");
            while (1);
        }
    }
    
    if (NutThreadCreate("RCV", UDPReceiver, (void*)socket, 1024) == 0) {
            puts("Could not start receiver thread\r\n");
            puts("Demo halted...\r\n");
            while (1);
    } else {
        puts("Receiver thread started\r\n");
    }
    
    puts("Starting echo test (1 packet / second)\r\n");
    
    ip_udp_echo = inet_addr(UDP_ECHO_IP);
    packet_nr = 0;
    
    while (1) {
        packet_nr ++;
        sprintf(send_buffer, "Packet: %d", packet_nr);
        rc = NutUdpSendTo(socket, ip_udp_echo, UDP_ECHO_PORT, send_buffer, length);
        printf("--> Sended packet: \"%s\", to %s, rc: %d\r\n", send_buffer, inet_ntoa(ip_udp_echo), rc);
        if (rc < 0) {
#ifdef NUT_UDP_ICMP_SUPPORT
            int      error;
            uint32_t remote_ip;
            uint16_t remote_port;             
            error = NutUdpError(socket, &remote_ip, &remote_port);
            print_udp_icmp_error(remote_ip, remote_port, error);
#endif
        }
        
        NutSleep(1000);
    }
#endif /* DEV_ETHER */
    return 0;
}
