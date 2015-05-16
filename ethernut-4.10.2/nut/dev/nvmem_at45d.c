/*
 * Copyright (C) 2008-2009 by egnite GmbH
 * Copyright (C) 2006 by egnite Software GmbH
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
 * \file dev/nvmem_at45d.c
 * \brief Non-volatile memory support using Atmel AT45 serial DataFlash memory chips.
 *
 * \verbatim
 * $Id: nvmem_at45d.c 3560 2011-08-16 10:53:31Z haraldkipp $
 * \endverbatim
 */

#include <cfg/eeprom.h>
#include <cfg/memory.h>

#include <dev/board.h>
#include <dev/spi_at45d.h>

#include <sys/nutdebug.h>

#include <stdlib.h>
#include <string.h>
#include <memdebug.h>

#include <dev/nvmem_at45d.h>

static NUTDEVICE *devSysConf;

/*!
 * \brief Mimic initialization without actually registering the device.
 *
 * This is a little bit tricky. If we want to use the existing DataFlash
 * routines for accessing the system configuration, they must be
 * properly initialized. This is normally done by calling NutRegister...
 * in the application. However, the system configuration must be know
 * before entering any application code.
 * 
 */
static int SpiAt45dConfigDevice(void)
{
#if !defined(DEV_SPIBUS)
    return -1;
#else /* DEV_SPIBUS */
    if (devSysConf == NULL) {
        NUTSPINODE *node;
        NUTDEVICE *dev;
#if NUT_CONFIG_AT45D == 0
        dev = &devSpiAt45d0;
#elif NUT_CONFIG_AT45D == 1
        dev = &devSpiAt45d1;
#elif NUT_CONFIG_AT45D == 2
        dev = &devSpiAt45d2;
#elif NUT_CONFIG_AT45D == 3
        dev = &devSpiAt45d3;
#else
        return -1;
#endif
        node = (NUTSPINODE *) dev->dev_icb;
        NUTASSERT(node != NULL);
        if (node->node_bus == NULL) {
            NUTSPIBUS *bus;
            bus = &DEV_SPIBUS;
            node->node_bus = bus;
        }
#ifdef NUT_CONFIG_AT45D_CS
        node->node_cs = NUT_CONFIG_AT45D_CS;
#endif
        NUTASSERT(node->node_bus->bus_initnode != NULL);
        if ((*node->node_bus->bus_initnode) (node)) {
            return -1;
        }
        NutEventPost(&node->node_bus->bus_mutex);
        if (SpiAt45dInit(dev)) {
            return -1;
        }
        devSysConf = dev;
    }
    return 0;
#endif /* DEV_SPIBUS */
}

static uint32_t SpiAt45dConfigPage(void)
{
#ifdef NUT_CONFIG_AT45D_PAGE
    return NUT_CONFIG_AT45D_PAGE;
#else
    return SpiAt45dPages(devSysConf) - 1;
#endif
}

/*!
 * \brief Get size of configuration area.
 *
 * A part of the DataFlash may be used to store configuration parameters,
 * like the network interface MAC address, the local hostname etc. The
 * size of this area may be configured by defining \ref NUT_CONFIG_AT45D_SIZE.
 * Otherwise one full page is used.
 *
 * \return The number of bytes available for configuration data. In case of
 *         an error, 0 is returned.
 */
size_t SpiAt45dConfigSize(void)
{
    size_t rc = 0;

    if (SpiAt45dConfigDevice() == 0) {
#ifdef NUT_CONFIG_AT45D_SIZE
        rc = NUT_CONFIG_AT45D_SIZE;
#else
        rc = (size_t) SpiAt45dPageSize(devSysConf);
#endif
    }
    return rc;
}

/*!
 * \brief Load configuration parameters from flash memory.
 *
 * \param pos  Start location within configuration sector.
 * \param data Points to a buffer that receives the contents.
 * \param len  Number of bytes to read.
 *
 * \return 0 on success or -1 in case of an error.
 */
int SpiAt45dConfigRead(size_t pos, void *data, size_t len)
{
    int rc = -1;
    uint8_t *buff;
    int csize = SpiAt45dConfigSize();

    /* Load the complete configuration area. */
    if (csize >= pos + len && (buff = malloc(csize)) != NULL) {
        uint32_t cpage = SpiAt45dConfigPage();

        if (SpiAt45dPageRead(devSysConf, cpage, buff, csize) == csize) {
            /* Copy requested contents to caller's buffer. */
            memcpy(data, buff + pos, len);
            rc = 0;
        }
        free(buff);
    }
    return rc;
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

int SpiAt45dConfigWrite(size_t pos, CONST void *data, size_t len)
{
    int rc = -1;
    uint8_t *pbuff;
    uint8_t *data_buff = (uint8_t*) data;
    int csize = SpiAt45dConfigSize();
    int psize = SpiAt45dPageSize(devSysConf);
    int page  = SpiAt45dConfigPage();
    int offset;
    int wsize;
    int remaining = len;

    if ((len > 0) && (csize >= pos + len) && (len > 0) && ((pbuff = malloc(psize)) != NULL)) {
        page    += pos / psize;
        offset   = pos % psize;

        wsize = (offset + len > psize) ? psize - offset : psize;
        if (remaining < wsize) {
            wsize = remaining;
        }

        while (remaining > 0) {
            /* Load one full page */
            if (SpiAt45dPageRead(devSysConf, page, pbuff, psize) == psize) {
                /* Compare old with new contents. */
                if (memcmp(pbuff + offset, data_buff, wsize)) {
                    /* New contents differs. Copy it into the sector buffer. */
                    memcpy(pbuff + offset, data_buff, wsize);
                    /* Erase sector and write new data. */
                    if (SpiAt45dPageWrite(devSysConf, page, pbuff, psize) == psize) {
                        rc = 0;
                    } else {
                        rc = -1;
                        break;
                    }
                } else {
                    rc = 0;
                }
            } else {
                rc = -1;
                break;
            }
        
            data_buff += wsize;
            remaining -= wsize;
            offset = 0;
            page ++;

            wsize = (remaining > psize) ? psize : remaining;
        }
        free(pbuff);
    }
        
    return rc;
}
