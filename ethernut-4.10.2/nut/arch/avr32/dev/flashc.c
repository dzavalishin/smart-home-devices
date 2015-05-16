/*!
 * Copyright (C) 2001-2010 by egnite Software GmbH
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
* \file arch/avr32/dev/flashc.c
* \brief AVR32 embedded flash controller support.
*
*/

#include <sys/atom.h>
#include <dev/nvmem.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <arch/avr32.h>
#include <arch/avr32/flashc.h>

#include <avr32/io.h>


/*!
 * \addtogroup xgAvr32Efc
 */
/*@{*/

/*! \brief Address offset of the configuration sector.
 */
#ifndef FLASH_CONF_SECTOR
#define FLASH_CONF_SECTOR  0x0003FF00
#endif


/*! \brief Size of the configuration area.
 *
 * During write operations a buffer with this size is allocated
 * from heap and may cause memory problems with large sectors.
 * Thus, this value may be less than the size of the configuration 
 * sector, in which case the rest of the sector is unused.
 *
 * Currently only 1 sector can be used for system configurations.
 */
#ifndef FLASH_CONF_SIZE
#define FLASH_CONF_SIZE         AVR32_FLASHC_PAGE_SIZE
#endif

#ifndef FLASH_WRITE_WAIT
#define FLASH_WRITE_WAIT        60000
#endif

#ifndef FLASH_ERASE_WAIT
#define FLASH_ERASE_WAIT        60000
#endif

#ifndef FLASH_CHIP_ERASE_WAIT
#define FLASH_CHIP_ERASE_WAIT   600000
#endif


typedef uint32_t flashdat_t;
typedef unsigned long flashadr_t;
typedef volatile flashdat_t *flashptr_t;

static int flashc_is_ready(void)
{
    return ((AVR32_FLASHC.fsr & AVR32_FLASHC_FSR_FRDY_MASK) != 0);
}

/*!
 * \brief Execute flash controller command.
 *
 */
int Avr32FlashcCmd(unsigned int cmd, uint32_t tmo)
{
    int rc = 0;

    /* Make sure that the previous command has finished. */
    while (!flashc_is_ready()) {
        if (tmo && --tmo < 1) {
            return -1;
        }
    }

    /* IRQ handlers are located in flash. Disable them. */
    NutEnterCritical();

    /* Write command. */
    outr(AVR32_FLASHC.fcmd, AVR32_FLASHC_FCMD_KEY_KEY | cmd);

    /* Wait for ready flag set. */
    while (!flashc_is_ready()) {
        if (tmo && --tmo < 1) {
            rc = -1;
            break;
        }
    }

    /* Flash command finished. Re-enable IRQ handlers. */
    NutExitCritical();

    /* Check result. */
    if (AVR32_FLASHC.fsr & (AVR32_FLASHC_FSR_LOCKE_MASK | AVR32_FLASHC_FSR_PROGE_MASK)) {
        rc = -1;
    }
    return rc;
}

/*!
 * \brief Read data from flash memory.
 *
 * \param off  Start location within the chip, starting at 0.
 * \param data Points to a buffer that receives the data.
 * \param len  Number of bytes to read.
 *
 * \return Always 0.
 */
int Avr32FlashcSectorRead(unsigned int off, void *data, unsigned int len)
{
    memcpy(data, (void *) (uptr_t) (AVR32_FLASH_ADDRESS + off), len);

    return 0;
}

/*!
 * \brief Write data into flash memory.
 *
 * The related sector will be automatically erased before writing.
 *
 * \param off  Start location within the chip, starting at 0.
 * \param data Points to a buffer that contains the bytes to be written.
 *             If this is a NULL pointer, then the sector will be erased.
 * \param len  Number of bytes to write, 1 full sector max.
 *
 * \return 0 on success or -1 in case of an error.
 */
int Avr32FlashcSectorWrite(unsigned int off, CONST void *data, unsigned int len)
{
    flashptr_t dp = (flashptr_t) (uptr_t) (AVR32_FLASH_ADDRESS + off);
    int rc;
    unsigned int i;

    if (data) {
        flashptr_t sp = (flashptr_t) data;

        /* Copy data to the flash write buffer. */
        for (i = 0; i < len; i += sizeof(flashdat_t)) {
            *dp++ = *sp++;
        }
    } else {
        /* All bits set to emulate sector erasing. */
        for (i = 0; i < len; i += sizeof(flashdat_t)) {
            *dp++ = (flashdat_t) (-1);
        }
    }

    /* Erase target flash page. */
    Avr32FlashcSectorErase(off);
    /* Execute page write command. */
    rc = Avr32FlashcCmd((off & AVR32_FLASHC_FCMD_PAGEN_MASK) | AVR32_FLASHC_FCMD_CMD_WP, FLASH_WRITE_WAIT);

    return rc;
}

/*!
 * \brief Erase sector at the specified offset.
 */
int Avr32FlashcSectorErase(unsigned int off)
{
    return Avr32FlashcCmd((off & AVR32_FLASHC_FCMD_PAGEN_MASK) | AVR32_FLASHC_FCMD_CMD_EP, FLASH_ERASE_WAIT);
}

/*!
 * \brief Lock specified region.
 *
 * \param off Location within the region to be locked.
 *
 * \return 0 on success or -1 in case of an error.
 */
int Avr32FlashcRegionLock(unsigned int off)
{
    return Avr32FlashcCmd((off & AVR32_FLASHC_FCMD_PAGEN_MASK) | AVR32_FLASHC_FCMD_CMD_LP, FLASH_WRITE_WAIT);
}

/*!
 * \brief Unlock specified region.
 *
 * \param off Location within the region to be unlocked.
 *
 * \return 0 on success or -1 in case of an error.
 */
int Avr32FlashcRegionUnlock(unsigned int off)
{
    return Avr32FlashcCmd((off & AVR32_FLASHC_FCMD_PAGEN_MASK) | AVR32_FLASHC_FCMD_CMD_UP, FLASH_WRITE_WAIT);
}

/*!
 * \brief Load configuration parameters from embedded flash memory.
 *
 * Applications should call NutNvMemLoad().
 *
 * \param pos   Start location within configuration sector.
 * \param data  Points to a buffer that receives the contents.
 * \param len   Number of bytes to read.
 *
 * \return Always 0.
 */
int Avr32FlashcParamRead(unsigned int pos, void *data, unsigned int len)
{
    return Avr32FlashcSectorRead(FLASH_CONF_SECTOR + pos, data, len);
}

/*!
 * \brief Store configuration parameters in embedded flash memory.
 *
 * Applications should call NutNvMemSave().
 *
 * The region that contains the configuration sector will be automatically 
 * locked.
 *
 * \param pos   Start location within configuration sector.
 * \param data  Points to a buffer that contains the bytes to store.
 * \param len   Number of bytes to store.
 *
 * \return 0 on success or -1 in case of an error.
 */
int Avr32FlashcParamWrite(unsigned int pos, CONST void *data, unsigned int len)
{
    int rc = -1;
    uint8_t *buff;

    /* Load the complete configuration area. */
    if ((buff = malloc(FLASH_CONF_SIZE)) != NULL) {

        rc = Avr32FlashcSectorRead(FLASH_CONF_SECTOR, buff, FLASH_CONF_SIZE);
        /* Compare old with new contents. */
        if (memcmp(buff + pos, data, len)) {
            /* New contents differs. Copy it into the sector buffer. */
            memcpy(buff + pos, data, len);
            /* Write back new data. Maintain region lock. */
            if (Avr32FlashcRegionUnlock(FLASH_CONF_SECTOR) == 0) {
                rc = Avr32FlashcSectorWrite(FLASH_CONF_SECTOR, buff, FLASH_CONF_SIZE);
                Avr32FlashcRegionLock(FLASH_CONF_SECTOR);
            }
        }
        free(buff);
    }
    return rc;
}

/*@}*/
