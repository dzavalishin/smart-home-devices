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

#ifndef _DEFAULT_H_
#define _DEFAULT_H_

/* Enable / disable debugging */

#define DEBUG_LEVEL LEVEL_ALWAYS
//#define DEBUG_LEVEL LEVEL_NEVER

#define DEFAULT_RS232_BAUD    115200

/* EEPROM */

#ifndef EE_OFFSET
#define EE_OFFSET	 0x200    /* Offset in eeprom we can use to store our own data   */
#endif

#ifndef EE_CONFBOOT
#define EE_CONFBOOT  0x80     /* Offset in eeprom we store the boot info structure   */
#endif

#define MAC_POSITION 0x4A     /* This is the offset of the MAC Position in out NVMEM */

#define FLASH_SECTOR_SIZE     256

/* Network */

#define DEFAULT_MAC           "\x02\x01\x02\x03\x04\x05"
#define DEFAULT_IP            "192.168.1.100"
#define DEFAULT_NETMASK       "255.255.255.0"
#define DEFAULT_GATWAY        "192.168.1.254"

/* TFTP Server */

#define DEFAULT_TFTP_SERVER   "192.168.1.1"
#define DEFAULT_TFTP_IMAGE    "demo-app.bin"

/* Some helper macros */

#define TRUE     1
#define FALSE    0

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))

#endif
