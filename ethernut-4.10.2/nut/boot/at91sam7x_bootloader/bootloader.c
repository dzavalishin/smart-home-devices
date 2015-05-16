/*
 * Copyright (C) 2008 by Thermotemp GmbH. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THERMOTEMP GMBH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THERMOTEMP
 * GMBH OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * For additional information see http://www.ethernut.de/
 */

#include <cfg/crt.h>

#include <string.h>
#include <stdio.h>
#include <io.h>

#include <dev/board.h>
#include <dev/npl.h>
#include <dev/nvmem.h>

#include <sys/timer.h>
#include <sys/confnet.h>

#include <net/if_var.h>
#include <net/route.h>
#include <arpa/inet.h>

#include "default.h"
#include "debug.h"
#include "loader.h"

void init_network(void)
{
    NutRegisterDevice(&DEV_ETHER, 0, 0);
    /* Read Standard Network Parameters */
    if (NutNetLoadConfig(DEV_ETHER_NAME) != 0) {     /* EEPROM net configuration is NOT valid */
        u_char mac[] = DEFAULT_MAC;
        
        /* Try to load the last valid mac address from confnet structure */
        if (NutNvMemLoad(MAC_POSITION, mac, 6) != 0) {
            ERROR("Could not load MAC address\r\n");
        }

        /* Check if it's a valid mac. The first three bytes needs to be the same like the default MAC */
        if ((mac[0] != (u_char) DEFAULT_MAC[0]) || (mac[1] == (u_char) DEFAULT_MAC[1]) || (mac[2] == (u_char) DEFAULT_MAC[2])) {
            memcpy(mac, DEFAULT_MAC, sizeof(mac));
        }
        
        confnet.cdn_ip_addr = inet_addr(DEFAULT_IP);
        confnet.cdn_gateway = inet_addr(DEFAULT_GATWAY);
        confnet.cdn_ip_mask = inet_addr(DEFAULT_NETMASK);

        NutNetSaveConfig();
        NutNetIfConfig(DEV_ETHER_NAME, mac, confnet.cdn_ip_addr, confnet.cdn_ip_mask);
    } else {
        /* Check of the MAC address is valid. Check for the first three bytes of the default MAC address */
        if ((confnet.cdn_mac[0] == (u_char) DEFAULT_MAC[0]) && (confnet.cdn_mac[1] == (u_char) DEFAULT_MAC[1]) && (confnet.cdn_mac[2] == (u_char) DEFAULT_MAC[2])) {
            NutNetIfConfig(DEV_ETHER_NAME, confnet.cdn_mac, confnet.cdn_ip_addr, confnet.cdn_ip_mask);
        } else {
            u_char mac[] = DEFAULT_MAC;
            NutNetIfConfig(DEV_ETHER_NAME, mac, confnet.cdn_ip_addr, confnet.cdn_ip_mask);
        }
    }
    NutIpRouteAdd(0, 0, confnet.cdn_gateway, &DEV_ETHER);

    INFO("MAC address: %02X:%02X:%02X:%02X:%02X:%02X\r\n", confnet.cdn_mac[0], confnet.cdn_mac[1], confnet.cdn_mac[2],
           confnet.cdn_mac[3], confnet.cdn_mac[4], confnet.cdn_mac[5]);
    INFO("IP:%s\r\n", inet_ntoa(confnet.cdn_ip_addr));
    INFO("Netmask: %s\r\n", inet_ntoa(confnet.cdn_ip_mask));
    INFO("Gateway: %s\r\n", inet_ntoa(confnet.cdn_gateway));   
}

FILE *init_uart(void)
{
    u_long baud       = DEFAULT_RS232_BAUD;
    u_long cookedmode = FALSE;

    FILE *uart0;

    /* Init the debug UART */
    
    NutRegisterDevice(&DEV_UART0, 0, 0);
    
    uart0 = fopen(DEV_UART0_NAME, "r+");
    
    _ioctl(_fileno(uart0), UART_SETSPEED, &baud);
    _ioctl(_fileno(uart0), UART_SETCOOKEDMODE, &cookedmode);
        
    freopen(DEV_UART0_NAME, "w", stdout);
    freopen(DEV_UART0_NAME, "r", stdin);
    freopen(DEV_UART0_NAME, "w", stderr);
    
    return uart0;
}

int main(void)
{
    FILE *uart0;
    
    uart0 = init_uart();
    
    INFO("At91sam7X Bootloader started.\r\n\r\n");
    
    init_loader();

    if (check_or_save_md5(confboot.size, FALSE) != 0) {
        init_network();
        loader(uart0);
    }
    INFO("Booting...\r\n");
    NutSleep(100);
    boot();
    
    while (1);
}
