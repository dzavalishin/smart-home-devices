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

/*
 * This demo demonstrates the usage of the bootloader
 *
 */

#include <stdio.h>
#include <string.h>
#include <io.h>

#include <dev/board.h>
#include <dev/nvmem.h>
#include <dev/watchdog.h>

#include <sys/timer.h>

#define TRUE     1
#define FALSE    0

#ifndef EE_CONFBOOT
#define EE_CONFBOOT  0x80     /* Offset in eeprom we store the boot info structure   */
#endif

/*
 * Boot configuration structure.
 */
typedef struct __attribute__ ((packed)) _CONFBOOT {
    unsigned char cb_size;      /* Size of this structure. */
    unsigned char cb_flags;     /* Currently unused */ 
    unsigned long cb_tftp_ip;   /* IP address of the TFTP server */
    char          cb_image[58]; /* Name of the image file to load */
    u_char        digest[16];   /* MD5 digest */
    int           size;         /* Size of the image */
} CONFBOOT;

CONFBOOT confboot;

/* Reset the processor using the reset controller */
void boot(void)
{
    printf("Booting...\r\n");
    NutSleep(100);
    outr(RSTC_CR, RSTC_KEY | RSTC_EXTRST | RSTC_PERRST | RSTC_PROCRST);
}

/* Read boot configuration from non-volatile memory. */
static int read_boot_config(void)
{
    int rc;

    rc = NutNvMemLoad(EE_CONFBOOT, &confboot, sizeof(CONFBOOT));
    if (confboot.cb_size != sizeof(CONFBOOT)) {
        rc = -1;
    }

    return rc;
}

/* Store boot configuration in non-volatile memory. */
static void write_boot_config(void)
{
    confboot.cb_size = sizeof(CONFBOOT);
    NutNvMemSave(EE_CONFBOOT, &confboot, sizeof(CONFBOOT));
}


/* Init the uart */

FILE *init_uart(void)
{
    u_long baud       = 115200;
    u_long cookedmode = FALSE;

    FILE *uart0;

    /* Init the debug UART */
    
    NutRegisterDevice(&DEV_UART, 0, 0);
    
    uart0 = fopen(DEV_UART_NAME, "r+");
    
    _ioctl(_fileno(uart0), UART_SETSPEED, &baud);
    _ioctl(_fileno(uart0), UART_SETCOOKEDMODE, &cookedmode);
        
    freopen(DEV_UART_NAME, "w", stdout);
    freopen(DEV_UART_NAME, "r", stdin);
    freopen(DEV_UART_NAME, "w", stderr);
    
    return uart0;
}

static char inbuf[128];

int main(void)
{	
    char *cp;
    int i;

    FILE *uart = init_uart();
    printf("Demo application for at91sam7x_bootloader started...\r\n");
    for (i = 0;; i++) {
        printf("\nEnter \"reset\" to reset or \"boot\" to reboot from tftp server:\r\n");
        fflush(uart);
        fgets(inbuf, 10, uart);

        /*
         * Chop off trailing linefeed.
         */
        cp = strchr(inbuf, '\n');
        if (cp) {
            *cp = 0;
	}

	/* Check if the string is "boot" or "load" */
	if (strcmp(inbuf, "reset") == 0) {
            printf("Reset...\r\n");
            boot();
	} else
	if (strcmp(inbuf, "boot") == 0) {
	    printf("Reboot from tftp server\r\n");
            read_boot_config();
            memset(confboot.digest, 0, sizeof(confboot.digest));
            write_boot_config();
            boot();
	} else { 
	    printf("You entered: %s\r\n", inbuf);
	}
    }
    return 0;
}
