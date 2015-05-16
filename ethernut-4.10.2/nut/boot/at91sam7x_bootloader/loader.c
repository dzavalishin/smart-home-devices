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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/atom.h>
#include <sys/timer.h>
#include <sys/heap.h>
#include <dev/nvmem.h>
#include <arpa/inet.h>
#include <arch/arm/atmel/at91_efc.h>
#include <gorp/md5.h>


#include "default.h"
#include "debug.h"
#include "loader.h"
#include "tftp.h"

#define IMAGE_OFFSET      0x0000C000
#define IMAGE_START_ADDR  0x0010C000
#define IMAGE_START      "0x0010C000"

/* Boot info structure. */
CONFBOOT confboot;

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

/* Set default values for boot config */
static void init_boot_config(void)
{
    confboot.cb_size = sizeof (CONFBOOT);
    confboot.cb_flags = 0;
    confboot.cb_tftp_ip = inet_addr(DEFAULT_TFTP_SERVER);
    strcpy((char*)confboot.cb_image, DEFAULT_TFTP_IMAGE);
    memset(confboot.digest, 0, sizeof(confboot.digest));
    confboot.size = 0;
}

RAMFUNC void boot(void)
{       
    NutEnterCritical();    
    
    /* Set all interrupts to be treated */
    outr(AIC_EOICR, 0);
    /* Disable _all_ interrupts */
    outr(AIC_IDCR, 0xFFFFFFFF);
    /* Disable clock to all devices */
    outr(PMC_PCDR, 0xFFFFFFFF);
    
    asm volatile ("@ Start Application" "\n\t"
                  "ldr r0, =" IMAGE_START "\n\t"
                  "bx  r0" "\n\t"
                  :::"r0"
    );
}

static int flasher_callback(u_char *buffer, u_short block_size, u_long offset, void* user_data)
{
    int pos;
    int size;
    int rc;
    
    pos = 0;
    size = block_size;
    
    do {
        rc = -1;
        /* Write back new data. Maintain region lock. */
        if (At91EfcRegionUnlock(IMAGE_OFFSET + offset + pos) == 0) {
            size = MIN(FLASH_SECTOR_SIZE, block_size - pos);

            rc = At91EfcSectorWrite(IMAGE_OFFSET + offset + pos, &buffer[pos], size);
            At91EfcRegionLock(IMAGE_OFFSET + offset + pos);
        }
        if (rc == 0) {
            pos += size;
        } else {
            return -1;
        }
    } while ((int)block_size - pos > 0);
    return 0;
}

int  check_or_save_md5(int size, int save)
{
    MD5CONTEXT md5_ctx;
    u_char  digest[16];
    int     idx;
    int     ok = 0;
    
    NutMD5Init(&md5_ctx);
    NutMD5Update(&md5_ctx, (u_char *)(IMAGE_START_ADDR), size);
    NutMD5Final(&md5_ctx, digest);
    
    if (save) {
#if ((DEBUG_LEVEL) >= (LEVEL_INFO))
        INFO("MD5: ");
        for (idx = 0; idx < 16; idx++) {
            INFO("%02x", digest[idx]);
        }
        INFO("\r\n");
#endif
        memcpy(confboot.digest, digest, sizeof(confboot.digest));
        confboot.size = size;
        write_boot_config();
    } else {
        for (idx = 0; idx < sizeof(digest); idx ++) {
            if (digest[idx] != confboot.digest[idx]) {
                ok = -1;
                break;
            }
        }
#if ((DEBUG_LEVEL) >= (LEVEL_INFO))
        if (ok != 0) {
            INFO("MD5: ");
            for (idx = 0; idx < 16; idx++) {
                INFO("%02x", digest[idx]);
            }
            INFO("\r\n");
            INFO("Saved MD5: ");
            for (idx = 0; idx < 16; idx++) {
                INFO("%02x", confboot.digest[idx]);
            }
            INFO("\r\n");
        }
#endif
    }
    
    return ok;
}

void loader(FILE *uart)
{
    int size = 0;
    int loaded = FALSE;
    int retry = 3;

    while (!loaded) {
        if (confboot.cb_tftp_ip != 0) {
            size = tftp_receive(confboot.cb_image, confboot.cb_tftp_ip, flasher_callback, NULL);
            if (size < 0) {
                ERROR("TFTP error loading file %s from %s\r\n", confboot.cb_image, inet_ntoa(confboot.cb_tftp_ip));
            } else {
                check_or_save_md5(size, TRUE); 
                loaded = TRUE;
            }
        } else {
            WARN("TFTP boot skipped. No tftp server configured\r\n");
            loaded = TRUE;
        }
        NutSleep(1000);

	retry--;
	
	if(!retry) 
		break;
    }
}

void init_loader(void)
{
    if (read_boot_config() != 0) {
        init_boot_config();
        write_boot_config();
    }
}
