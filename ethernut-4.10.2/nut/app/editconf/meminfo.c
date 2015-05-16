/*
 * Copyright (C) 2011 by egnite GmbH
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
 *
 */

/*!
 * $Id$
 */

#include <dev/board.h>
#include <sys/confnet.h>

#if defined(NUT_CONFIG_X12RTC)
#include <dev/x12rtc.h>
#elif defined(NUT_CONFIG_AT45D)
#include <dev/nvmem_at45d.h>
#elif defined(NUT_CONFIG_AT45DB)
#include <dev/at45db.h>
#elif defined(NUT_CONFIG_AT49BV)
#include <dev/at49bv.h>
#elif defined(NUT_CONFIG_AT91EFC)
#include <arch/arm/atmel/at91_efc.h>
#elif defined(NUT_CONFIG_AT24)
#include <dev/eeprom.h>
#endif

#include <stdio.h>
#include "meminfo.h"

/*
 * Print Nut Configuration.
 */
void ShowHardwareConfiguration(void)
{
    printf("Board   : ");
#if defined(ARTHERNET1)
    puts("Arthernet 1");
#elif defined(AT91SAM7X_EK)
    puts("AT91SAM7X(C)-EK");
#elif defined(AT91SAM9260_EK)
    puts("AT91SAM9260-EK");
#elif defined(CHARON2)
    puts("Charon II");
#elif defined(ELEKTOR_IR1)
    puts("Elektor Internet Radio");
#elif defined(ENET_SAM7X)
    puts("eNet-sam7X");
#elif defined(ETHERNUT1)
    puts("Ethernut 1");
#elif defined(ETHERNUT2)
    puts("Ethernut 2");
#elif defined(ETHERNUT3)
#if defined(NUT_CONFIG_X12RTC)
    puts("Ethernut 3.0");
#else
    puts("Ethernut 3.1");
#endif
#elif defined(ETHERNUT5)
    puts("Ethernut 5");
#elif defined(EVK1100)
    puts("EVK1100");
#elif defined(EVK1104)
    puts("EVK1104");
#elif defined(EVK1105)
    puts("EVK1105");
#elif defined(GBAXPORT2)
    puts("GBA-XPORT2");
#elif defined(MMNET01)
    puts("MMNet01");
#elif defined(MMNET02)
    puts("MMNet02");
#elif defined(MMNET03)
    puts("MMNet03");
#elif defined(MMNET04)
    puts("MMNet04");
#elif defined(MMNET101)
    puts("MMNet101");
#elif defined(MMNET102)
    puts("MMNet102");
#elif defined(MMNET103)
    puts("MMNet103");
#elif defined(MMNET104)
    puts("MMNet104");
#elif defined(MORPHOQ1)
    puts("Morphoq 1");
#elif defined(OLIMEX_LPCE2294) 
    puts("Olimex LPCE2294");
#elif defined(XNUT_100)
    puts("XNUT-100");
#elif defined(XNUT_105)
    puts("XNUT-105");
#elif defined(__arm__)
    puts("Generic ARM");
#elif defined(__AVR32__)
    puts("Generic AVR32");
#elif defined(__AVR__)
    puts("Generic AVR");
#else
    puts("Unknown");
#endif

    printf("Memory  : ");
#if defined(NUT_CONFIG_X12RTC)
    puts("X12xx RTC EEPROM");

#elif defined(NUT_CONFIG_AT45D)
    puts("AT45D DataFlash");
#ifdef NUT_CONFIG_AT45D_PAGE
    printf("Page    : %d\n", NUT_CONFIG_AT45D_PAGE);
#else
    puts("Page    : Last");
#endif
    printf("Size    : %d\n", (int) SpiAt45dConfigSize());
#ifdef DEV_SPIBUS
    if (DEV_SPIBUS.bus_base) {
        printf("SPI Base: 0x%08X\n", DEV_SPIBUS.bus_base);
    } else {
        puts("SPI     : Ext. or GPIO");
    }
#else
    puts("SPI Base: Undefined", DEV_SPIBUS.bus_base);
#endif
#ifdef NUT_CONFIG_AT45D
    printf("Chip    : %d\n", NUT_CONFIG_AT45D);
#else
    puts("Chip    : Undefined");
#endif
#ifdef NUT_CONFIG_AT45D_CS
    printf("Chip Sel: %d\n", NUT_CONFIG_AT45D_CS);
#else
    puts("Chip Sel: Undefined");
#endif

#elif defined(NUT_CONFIG_AT45DB)
    puts("AT45D DataFlash (old, deprecated)");
#ifdef AT45_CONF_SIZE
    printf("Size    : %d\n", AT45_CONF_SIZE);
#else
    printf("Size    : %d\n", (int) At45dbPageSize());
#endif
#ifdef AT45_CONF_PAGE
    printf("Page    : %d\n", AT45_CONF_PAGE);
#else
    puts("Page    : Last");
#endif
#ifdef AT45_CONF_DFSPI
    printf("SPI Base: 0x%08X\n", AT45_CONF_DFSPI);
#else
    printf("SPI Base: 0x%08X\n", SPI0_BASE);
#endif
#ifdef AT45_CONF_DFPCS
    printf("Chip Sel: %d\n", AT45_CONF_DFPCS);
#else
    puts("Chip Sel: 0");
#endif

#elif defined(NUT_CONFIG_AT49BV)
    puts("AT49BV NOR Flash");
#ifdef FLASH_CONF_SIZE
    printf("Size    : %d\n", FLASH_CONF_SIZE);
#else
    puts("Size    : 512");
#endif
#ifdef FLASH_CONF_SECTOR
    printf("Sector  : 0x%X\n", FLASH_CONF_SECTOR);
#else
    puts("Sector  : 0x6000");
#endif
#ifdef FLASH_CHIP_BASE
    printf("NOR Base: 0x%08X\n", FLASH_CHIP_BASE);
#else
    puts("NOR Base: 0x10000000");
#endif

#elif defined(__AVR__)
    puts("Internal EEPROM");

#elif defined(NUT_CONFIG_AT91EFC)
    puts("AT91 Program Flash");
#ifdef FLASH_CONF_SIZE
    printf("Size    : %d\n", FLASH_CONF_SIZE);
#else
    puts("Size    : 256");
#endif
#ifdef FLASH_CONF_SECTOR
    printf("Sector  : 0x%X\n", FLASH_CONF_SECTOR);
#elif defined(MCU_AT91SAM7S512) || defined(MCU_AT91SAM7SE512) || \
    defined(MCU_AT91SAM7X512) || defined(MCU_AT91SAM9XE512)
    puts("Sector  : 0x0007FF00");
#else
    puts("Sector  : 0x0003FF00");
#endif

#elif defined(NUT_CONFIG_AT24)
    puts("AT24C32 EEPROM");
#ifdef NUT_CONFIG_AT24_ADR
    printf("Address : 0x%02X\n", NUT_CONFIG_AT24_ADR);
#else
    puts("Address : 0x50");
#endif

#else
    puts("Unknown");
#endif
}
