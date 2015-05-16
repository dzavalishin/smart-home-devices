/*
 * Copyright (C) 2005-2007 by egnite Software GmbH
 * Copyright (C) 2010 by egnite GmbH
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

/*
 * $Id: bootmon.c 3422 2011-05-18 09:46:57Z haraldkipp $
 */

#include "utils.h"
#include "uart.h"
#include "dialog.h"
#include "ether.h"
#include "dhcp.h"
#include "bootmon.h"

unsigned long random_id;
ETHERHDR sheader;
ETHERFRAME sframe;
ETHERHDR rheader;
ETHERFRAME rframe;

static unsigned char my_mac[32];
static char my_ip[32];
static char my_mask[32];
static char my_gate[32];
static char my_tftpd[32];
static char my_image[64];

#ifdef USRLED
#include "npluled.h"

void NplUledCntl(int status)
{
    if (status == ULED_ON) {
        /*! \brief Activate negated chip select. */
        outb(NPL_XER, inb(NPL_XER) & ~NPL_USRLED);
    } else {
        if (status == ULED_OFF) {
            /*! \brief Deactivate negated chip select. */
            outb(NPL_XER, inb(NPL_XER) | NPL_USRLED);
        }
    }
}
#else
#define NplUledCntl(x)
#endif

static int UserEntry(void)
{
    PutString("\nBootMon 1.2.1\n");
    NplUledCntl(ULED_OFF);

    memcpy_(my_mac, confnet.cdn_mac, 6);
    strcpy_(my_ip, inet_ntoa(confnet.cdn_cip_addr));
    strcpy_(my_mask, inet_ntoa(confnet.cdn_ip_mask));
    strcpy_(my_gate, inet_ntoa(confnet.cdn_gateway));
    strcpy_(my_tftpd, inet_ntoa(confboot.cb_tftp_ip));
    strcpy_(my_image, (char*)confboot.cb_image);

    GetMac(my_mac);
    GetIP("IP address", my_ip);

    if (inet_addr(my_ip)) {
        GetIP("Net mask", my_mask);
        GetIP("Default route", my_gate);
    }
    GetIP("TFTP IP", my_tftpd);
    if (inet_addr(my_tftpd)) {
        PutString("TFTP Image: ");
        GetLine(my_image, 31);
    }
    PutString("\n");

    memcpy_(confnet.cdn_mac, my_mac, sizeof(confnet.cdn_mac));
    confnet.cdn_cip_addr = inet_addr(my_ip);
    confnet.cdn_ip_mask = inet_addr(my_mask);
    confnet.cdn_gateway = inet_addr(my_gate);

    confboot.cb_tftp_ip = inet_addr(my_tftpd);
    DEBUG("Store '");
    DEBUG(my_image);
    DEBUG("'\n");
    strcpy_((char*)confboot.cb_image, my_image);

    BootConfigWrite();

    return 0;
}

/*
 * Bootloader entry point.
 */
int main(void)
{
    int i;
    unsigned long *flash_app = (unsigned long *)0x10020000;

    NplUledCntl(ULED_ON);
    /* Initialize RS232 port. */
    UartInit();

    /* Read configuration from EEPROM. */
    if (BootConfigRead()) {
        /* First trial failed, retry once more. */
        BootConfigRead();
    }
    /* Set default netmask, if configured one is zero. */
    if (confnet.cdn_ip_mask == 0) {
        confnet.cdn_ip_mask = 0xFFFFFF;
    }

    /* Initialize random ID with MAC address. */
    memcpy_((unsigned char *)&random_id, &confnet.cdn_mac[2], sizeof(random_id));

    /*
     * Loop until a valid image is loaded.
     */
    for (;;) {
        /*
         * Give user a chance to enter a space and a new configuration.
         */
        for (i = 0; i < 100; i++) {
            if (UartRxWait(6000) == 0) {
                if (UartRx() == ' ') {
                    UserEntry();
                    break;
                }
            }
        }
        NplUledCntl(ULED_OFF);

        if (confboot.cb_tftp_ip == 0 && *flash_app == 0xE59FF018) {
            DEBUG("\nStarting Flashed Application\n");
            asm volatile ("@ Start Application" "\n\t"  /* */
                          "ldr r0, =0x10020000" "\n\t"   /* */
                          "bx  r0" "\n\t"       /* */
                          :::"r0"       /* */
            );
        }

        /*
         * Initialize the network interface controller hardware.
         */
        DEBUG("\nETHERNET ");
        if (EtherInit() == 0) {
            DEBUG("OK\n");

            /*
             * DHCP query and TFTP download.
             */
            if (DhcpQuery() == 0) {
                if (TftpRecv() == 0) {
                    break;
                }
            }
        } else {
            DEBUG("No\n");
        }
        NplUledCntl(ULED_ON);
    }
#ifdef NUTDEBUG
    /* Avoids UART garbage. */
    Delay(5);
#endif
    asm volatile ("@ Start Application" "\n\t"  /* */
                  "ldr r0, =0" "\n\t"   /* */
                  "bx  r0" "\n\t"       /* */
                  :::"r0"       /* */
        );
    return 0;
}
