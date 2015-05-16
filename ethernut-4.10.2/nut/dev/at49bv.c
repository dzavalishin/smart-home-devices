/*
 * Copyright (C) 2005-2006 by egnite Software GmbH. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY EGNITE SOFTWARE GMBH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL EGNITE
 * SOFTWARE GMBH OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
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
 * \file dev/at49bv.c
 * \brief Routines for Atmel AT49 flash memory chips.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.6  2009/01/17 11:26:46  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.5  2008/08/11 06:59:41  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.4  2008/07/17 11:51:18  olereinhardt
 * Added function AT49bvReadProtectionRegister to read the 64bit factory
 * or user id
 *
 * Revision 1.3  2006/10/08 16:48:09  haraldkipp
 * Documentation fixed
 *
 * Revision 1.2  2006/05/25 09:33:35  haraldkipp
 * Bugfix. At49bvParamWrite() returned an error when flash sector contents
 * was equal to the contents to write.
 *
 * Revision 1.1  2006/04/07 13:51:36  haraldkipp
 * AT49BV flash memory support added. A single sector may be used to
 * store system configurations in case there is no EEPROM available.
 *
 *
 * \endverbatim
 */

#include <cfg/os.h>
#include <cfg/memory.h>

#include <sys/event.h>
#include <sys/timer.h>

#include <stdlib.h>
#include <string.h>

#include <dev/at49bv.h>

/*! \brief Base address of the flash memory chip.
 */
#ifndef FLASH_CHIP_BASE
#define FLASH_CHIP_BASE  0x10000000
#endif

/*! \brief Address offset of the configuration sector.
 */
#ifndef FLASH_CONF_SECTOR
#define FLASH_CONF_SECTOR  0x6000
#endif

/*! \brief Size of the configuration area.
 *
 * During write operations a buffer with this size is allocated
 * from heap and may cause memory problems with large sectors.
 * Thus, this value may be less than the size of the configuration 
 * sector, in which case the rest of the sector is unused.
 */
#ifndef FLASH_CONF_SIZE
#define FLASH_CONF_SIZE         512
#endif

#ifndef FLASH_ERASE_WAIT
#define FLASH_ERASE_WAIT        3000
#endif

#ifndef FLASH_CHIP_ERASE_WAIT
#define FLASH_CHIP_ERASE_WAIT   50000
#endif

#ifndef FLASH_WRITE_POLLS
#define FLASH_WRITE_POLLS       1000
#endif


#ifdef FLASH_8BIT
typedef unsigned char flashdat_t;
#else
typedef unsigned short flashdat_t;
#endif

typedef unsigned long flashadr_t;
typedef volatile flashdat_t *flashptr_t;

static flashptr_t chip = (flashptr_t) FLASH_CHIP_BASE;

#define FLASH_UNLOCK(base) { \
    base[0x0555] = 0xAA; \
    base[0x0AAA] = 0x55; \
}

#define FLASH_COMMAND(base, cmd) { \
    base[0x0555] = cmd; \
}

#define FLASH_CMD_ERASE         0x80
#define FLASH_CMD_ERASE_CHIP    0x10
#define FLASH_CMD_ERASE_SECTOR  0x30

#define FLASH_CMD_ENTER_ID      0x90
#define FLASH_CMD_EXIT_ID       0xF0

#define FLASH_CMD_PROGRAM       0xA0

/*!
 * \brief Wait until flash memory cycle finished.
 *
 * \return 0 on success or -1 in case of an error.
 */
static int At49bvWaitReady(flashptr_t addr, flashdat_t data, uint32_t tmo, int poll)
{
    while (*addr != data) {
        if (!poll) {
            NutSleep(1);
        }
        if (tmo-- == 0) {
            return -1;
        }
    }
    return 0;
}

/*
 * May be later used to retrieve the chip layout.
 */
unsigned long At49bvInit(void)
{
    unsigned long id;

    FLASH_UNLOCK(chip);
    FLASH_COMMAND(chip, FLASH_CMD_ENTER_ID);
    id = chip[0];
    id <<= 16;
    id |= chip[1];
    FLASH_UNLOCK(chip);
    FLASH_COMMAND(chip, FLASH_CMD_EXIT_ID);

    return id;
}

/*!
 * \brief Read user or factory id from protection register.
 * \param factory  If true, read factory id, otherwise read user id
 *
 * \return The ID read, 64 bit long!
 */
unsigned long long AT49bvReadProtectionRegister(int factory)
{
    unsigned long long id;

    FLASH_UNLOCK(chip);
    FLASH_COMMAND(chip, FLASH_CMD_ENTER_ID);
    if (factory) {
        id  = chip[0x81];
        id <<= 16;
        id |= chip[0x82];
        id <<= 16;
        id |= chip[0x83];
        id <<= 16;
        id |= chip[0x84];
    } else {
        id  = chip[0x85];
        id <<= 16;
        id |= chip[0x86];
        id <<= 16;
        id |= chip[0x87];
        id <<= 16;
        id |= chip[0x88];
    }        
    FLASH_UNLOCK(chip);
    FLASH_COMMAND(chip, FLASH_CMD_EXIT_ID);

    return id;
}

/*!
 * \brief Erase sector at the specified offset.
 */
int At49bvSectorErase(unsigned int off)
{
    flashptr_t ptr = (flashptr_t) (uintptr_t) (FLASH_CHIP_BASE + off);

    FLASH_UNLOCK(chip);
    FLASH_COMMAND(chip, FLASH_CMD_ERASE);
    FLASH_UNLOCK(chip);
    *ptr = FLASH_CMD_ERASE_SECTOR;

    return At49bvWaitReady(ptr, (flashdat_t) - 1, FLASH_ERASE_WAIT, 0);
}

/*!
 * \brief Erase entire flash memory chip.
 */
int At49bvChipErase(void)
{
    FLASH_UNLOCK(chip);
    FLASH_COMMAND(chip, FLASH_CMD_ERASE);
    FLASH_UNLOCK(chip);
    FLASH_COMMAND(chip, FLASH_CMD_ERASE_CHIP);

    return At49bvWaitReady(chip, (flashdat_t) - 1, FLASH_CHIP_ERASE_WAIT, 0);
}

/*!
 * \brief Read data from flash memory.
 *
 * \param off  Start location within the chip, starting at 0.
 * \param data Points to a buffer that receives the data.
 * \param len  Number of bytes to read.
 *
 * \return 0 on success or -1 in case of an error.
 */
int At49bvSectorRead(unsigned int off, void *data, unsigned int len)
{
    memcpy(data, (void *) (uintptr_t) (FLASH_CHIP_BASE + off), len);

    return 0;
}

/*!
 * \brief Write data into flash memory.
 *
 * The related sector must have been erased before calling this function.
 *
 * \param off  Start location within the chip, starting at 0.
 * \param data Points to a buffer that contains the bytes to be written.
 * \param len  Number of bytes to write.
 *
 * \return 0 on success or -1 in case of an error.
 */
int At49bvSectorWrite(unsigned int off, CONST void *data, unsigned int len)
{
    int rc = 0;
    flashptr_t sp = (flashptr_t) data;
    flashptr_t dp = (flashptr_t) (uintptr_t) (FLASH_CHIP_BASE + off);
    unsigned int i;

    for (i = 0; i < len; i += sizeof(flashdat_t)) {
        /* No need to save values with all bits set. */
        if (*sp != (flashdat_t) - 1) {
            /* Write to memory location. */
            FLASH_UNLOCK(chip);
            FLASH_COMMAND(chip, FLASH_CMD_PROGRAM);
            *dp = *sp;
            if ((rc = At49bvWaitReady(dp, *sp, FLASH_WRITE_POLLS, 1)) != 0) {
                break;
            }
        }
        dp++;
        sp++;
    }
    return rc;
}

/*!
 * \brief Load configuration parameters from flash memory.
 *
 * \param pos   Start location within configuration sector.
 * \param data  Points to a buffer that receives the contents.
 * \param len   Number of bytes to read.
 *
 * \return Always 0.
 */
int At49bvParamRead(unsigned int pos, void *data, unsigned int len)
{
    return At49bvSectorRead(FLASH_CONF_SECTOR + pos, data, len);
}

/*!
 * \brief Store configuration parameters in flash memory.
 *
 * \param pos   Start location within configuration sector.
 * \param data  Points to a buffer that contains the bytes to store.
 * \param len   Number of bytes to store.
 *
 * \return 0 on success or -1 in case of an error.
 */
int At49bvParamWrite(unsigned int pos, CONST void *data, unsigned int len)
{
    int rc = -1;
    uint8_t *buff;

    /* Load the complete configuration area. */
    if ((buff = malloc(FLASH_CONF_SIZE)) != 0) {
        rc = At49bvSectorRead(FLASH_CONF_SECTOR, buff, FLASH_CONF_SIZE);
        /* Compare old with new contents. */
        if (memcmp(buff + pos, data, len)) {
            /* New contents differs. Copy it into the sector buffer. */
            memcpy(buff + pos, data, len);
            /* Erase sector and write new data. */
            if ((rc = At49bvSectorErase(FLASH_CONF_SECTOR)) == 0) {
                rc = At49bvSectorWrite(FLASH_CONF_SECTOR, buff, FLASH_CONF_SIZE);
            }
        }
        free(buff);
    }
    return rc;
}
