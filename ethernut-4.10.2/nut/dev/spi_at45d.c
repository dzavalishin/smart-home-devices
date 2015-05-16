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
 * \file dev/spi_at45d.c
 * \brief Routines for Atmel AT45 serial DataFlash memory chips.
 *
 * \verbatim
 * $Id: spi_at45d.c 3653 2011-11-14 15:20:31Z haraldkipp $
 * \endverbatim
 */

#include <cfg/os.h>
#include <cfg/memory.h>

#include <dev/blockdev.h>

#include <sys/nutdebug.h>
#include <sys/timer.h>

#include <string.h>

#include <dev/at45d.h>
#include <dev/spi_at45d.h>

#ifndef AT45_WRITE_POLLS
#define AT45_WRITE_POLLS        1000
#endif

#if 0
/*! \brief Parameter table of known DataFlash types. */
AT45D_INFO at45d_info[] = {
    {8, 512, 256, 0x0D},    /* AT45DB011B - 128kB */
    {9, 512, 264, 0x0C},    /* AT45DB011B - 128kB */
    {8, 1025, 256, 0x15},   /* AT45DB021B - 256kB */
    {9, 1025, 264, 0x14},   /* AT45DB021B - 256kB */
    {8, 2048, 256, 0x1D},   /* AT45DB041B - 512kB */
    {9, 2048, 264, 0x1C},   /* AT45DB041B - 512kB */
    {8, 4096, 256, 0x25},   /* AT45DB081B - 1MB */
    {9, 4096, 264, 0x24},   /* AT45DB081B - 1MB */
    {9, 4096, 512, 0x2D},   /* AT45DB0161B - 2MB */
    {10, 4096, 528, 0x2C},  /* AT45DB0161B - 2MB */
    {9, 8192, 512, 0x35},   /* AT45DB0321B - 4MB */
    {10, 8192, 528, 0x34},  /* AT45DB0321B - 4MB */
    {10, 8192, 1024, 0x39}, /* AT45DB0642 - 8MB */
    {11, 8192, 1056, 0x38}, /* AT45DB0642 - 8MB */
    {10, 8192, 1024, 0x3D}, /* AT45DB0642D - 8MB */
    {11, 8192, 1056, 0x3C}  /* AT45DB0642D - 8MB */
};

/*! \brief Number of known Dataflash types. */
uint_fast8_t at45d_known_types = sizeof(at45d_info) / sizeof(AT45D_INFO);
#endif

/*!
 * \brief Send DataFlash command.
 *
 * \param node  Specifies the SPI node.
 * \param op    Command operation code.
 * \param parm  Optional command parameter.
 * \param oplen Command length.
 * \param txbuf Pointer to the transmit data buffer, may be set to NULL.
 * \param rxbuf Pointer to the receive data buffer, may be set to NULL.
 * \param xlen  Number of byte to receive and/or transmit.
 */
static int At45dCommand(NUTSPINODE * node, uint8_t op, uint32_t parm, int oplen, CONST void *txbuf, void *rxbuf, int xlen)
{
    int rc = -1;
    NUTSPIBUS *bus;
    uint8_t cmd[8];

    NUTASSERT(node != NULL);
    bus = (NUTSPIBUS *) node->node_bus;
    NUTASSERT(bus != NULL);
    NUTASSERT(bus->bus_alloc != NULL);
    NUTASSERT(bus->bus_transfer != NULL);
    NUTASSERT(bus->bus_release != NULL);

    NUTASSERT(oplen <= 8);
    memset(cmd, 0, oplen);
    cmd[0] = op;
    if (parm) {
        cmd[1] = (uint8_t) (parm >> 16);
        cmd[2] = (uint8_t) (parm >> 8);
        cmd[3] = (uint8_t) parm;
    }
    rc = (*bus->bus_alloc) (node, 1000);
    if (rc == 0) {
        rc = (*bus->bus_transfer) (node, cmd, NULL, oplen);
        if (rc == 0 && xlen) {
            rc = (*bus->bus_transfer) (node, txbuf, rxbuf, xlen);
        }
        (*bus->bus_release) (node);
    }
    return rc;
}

/*!
 * \brief Query the status of the DataFlash.
 *
 * \param node  Specifies the SPI node.
 *
 * \return 0 on success or -1 in case of an error.
 */
static uint8_t At45dStatus(NUTSPINODE * node)
{
    int rc;
    uint8_t cmd[2] = { DFCMD_READ_STATUS, 0xFF };
    NUTSPIBUS *bus;

    NUTASSERT(node != NULL);
    NUTASSERT(node->node_bus != NULL);
    bus = (NUTSPIBUS *) node->node_bus;

    NUTASSERT(bus->bus_alloc != NULL);
    NUTASSERT(bus->bus_transfer != NULL);
    NUTASSERT(bus->bus_wait != NULL);
    NUTASSERT(bus->bus_release != NULL);

    rc = (*bus->bus_alloc) (node, 1000);
    if (rc == 0) {
        rc = (*bus->bus_transfer) (node, cmd, cmd, 2);
        if (rc == 0) {
            (*bus->bus_wait) (node, NUT_WAIT_INFINITE);
            rc = cmd[1];
        }
        (*bus->bus_release) (node);
    }
    return (uint8_t) rc;
}

/*!
 * \brief Wait until DataFlash memory cycle finished.
 *
 * \param node  Specifies the SPI node.
 *
 * \return 0 on success or -1 in case of an error.
 */
static int At45dWaitReady(NUTSPINODE * node, uint32_t tmo, int poll)
{
    uint8_t sr;

    while (((sr = At45dStatus(node)) & 0x80) == 0) {
        if (!poll) {
            NutSleep(1);
        }
        if (tmo-- == 0) {
            return -1;
        }
    }
    return 0;
}

/*!
 * \brief Send DataFlash command.
 *
 * \param dev   Specifies the DataFlash device.
 * \param op    Command operation code.
 * \param parm  Optional command parameter.
 * \param oplen Command length.
 * \param txbuf Pointer to the transmit data buffer, may be set to NULL.
 * \param rxbuf Pointer to the receive data buffer, may be set to NULL.
 * \param xlen  Number of byte to receive and/or transmit.
 */
int SpiAt45dCommand(NUTDEVICE * dev, uint8_t op, uint32_t parm, int oplen, CONST void *txbuf, void *rxbuf, int xlen)
{
    NUTASSERT(dev != NULL);
    return At45dCommand(dev->dev_icb, op, parm, oplen, txbuf, rxbuf, xlen);
}

/*!
 * \brief Query the status of the DataFlash.
 *
 * \param dev Specifies the Flash device.
 *
 * \return 0 on success or -1 in case of an error.
 */
uint8_t SpiAt45dStatus(NUTDEVICE * dev)
{
    NUTASSERT(dev != NULL);
    return At45dStatus(dev->dev_icb);
}

/*!
 * \brief Wait until DataFlash memory cycle finished.
 *
 * \param dev Specifies the DataFlash device.
 *
 * \return 0 on success or -1 in case of an error.
 */
int SpiAt45dWaitReady(NUTDEVICE * dev, uint32_t tmo, int poll)
{
    NUTASSERT(dev != NULL);
    return At45dWaitReady(dev->dev_icb, tmo, poll);
}

/*!
 * \brief Erase specified DataFlash page.
 *
 * \param dev Specifies the registered DataFlash device.
 * \param pgn  Page number to erase, starting at 0.
 *
 * \return 0 on success or -1 in case of an error.
 */
int SpiAt45dPageErase(NUTDEVICE * dev, uint32_t pgn)
{
    NUTBLOCKIO *blkio;
    AT45D_INFO *info;

    NUTASSERT(dev != NULL);
    NUTASSERT(dev->dev_dcb != NULL);
    blkio = dev->dev_dcb;
    
    info = (AT45D_INFO *) blkio->blkio_info;
    NUTASSERT(blkio->blkio_info != NULL);

    if (pgn >= info->at45d_pages) {
        return -1;
    }
    pgn <<= info->at45d_pshft;
    return At45dCommand((NUTSPINODE *) dev->dev_icb, DFCMD_PAGE_ERASE, pgn, 4, NULL, NULL, 0);
}

/*!
 * \brief Erase all DataFlash pages.
 */
int SpiAt45dChipErase(NUTDEVICE * dev)
{
    return -1;
}

/*!
 * \brief Initialize the DataFlash device.
 *
 * This routine determines the DataFlash type. It is internally called 
 * by Nut/OS during device registration.
 *
 * The driver framework may call this function more than once.
 *
 * \param dev Specifies the DataFlash device.
 *
 * \return 0 on success or -1 if no valid DataFlash was found.
 */
int SpiAt45dInit(NUTDEVICE * dev)
{
    NUTBLOCKIO *blkio;
    NUTSPINODE *node;
    uint8_t sr;
    int_fast8_t i;

    NUTASSERT(dev != NULL);
    NUTASSERT(dev->dev_dcb != NULL);
    NUTASSERT(dev->dev_icb != NULL);
    blkio = dev->dev_dcb;
    node = dev->dev_icb;

    if (At45dWaitReady(node, 10, 1) == 0) {
        /* Read the status byte and locate the related table entry. */
        sr = At45dStatus(node);
        if (sr != 0xff) {
            sr &= AT45D_STATUS_DENSITY | AT45D_STATUS_PAGE_SIZE;
            for (i = at45d_known_types; --i >= 0;) {
                if (sr == at45d_info[i].at45d_srval) {
                    /* Known DataFlash type. */
                    blkio->blkio_info = (void *) &at45d_info[i];
                    blkio->blkio_blk_cnt = at45d_info[i].at45d_pages;
                    blkio->blkio_blk_siz = at45d_info[i].at45d_psize;
                    return 0;
                }
            }
        }
    }
    /* Unknown DataFlash type. */
    return -1;
}

/*!
 * \brief Read data from DataFlash memory.
 *
 * \param dev  Specifies the registered DataFlash device.
 * \param pgn  Page number to read, starting at 0.
 * \param data Points to a buffer that receives the data.
 * \param len  Number of bytes to read.
 *
 * \return The number of bytes actually read. A return value of -1 indicates 
 *         an error.
 */
int SpiAt45dPageRead(NUTDEVICE * dev, uint32_t pgn, void *data, int len)
{
    NUTBLOCKIO *blkio;
    AT45D_INFO *info;

    NUTASSERT(dev != NULL);
    NUTASSERT(dev->dev_dcb != NULL);
    blkio = dev->dev_dcb;
    
    info = (AT45D_INFO *) blkio->blkio_info;
    NUTASSERT(blkio->blkio_info != NULL);

    if (pgn >= info->at45d_pages) {
        return -1;
    }
    pgn <<= info->at45d_pshft;

    if (At45dCommand((NUTSPINODE *) dev->dev_icb, DFCMD_CONT_READ, pgn, 8, NULL, data, len)) {
        return -1;
    }
    return len;
}

/*!
 * \brief Write data to DataFlash memory.
 *
 * Each page will be automatically erased before writing the data. If the
 * last page is not completely filled with new data, the contents of 
 * remaining bytes at the end of the page is undetermined.
 *
 * \param dev  Specifies the registered DataFlash device.
 * \param pgn  The page number.
 * \param data Points to the buffer that contains the bytes to be written.
 * \param len  Number of bytes available in the buffer. This may be less
 *             than the page size, in which case the remaining bytes of
 *             the page will be set to 0xff.
 *
 * \return The number of bytes actually written. A return value of -1 indicates 
 *         an error.
 */
int SpiAt45dPageWrite(NUTDEVICE * dev, uint32_t pgn, CONST void *data, int len)
{
    int rc = -1;
    uint8_t *dp = (uint8_t *) data;
    int step;
    uint_fast8_t pshft;
    uint32_t limit;
    NUTBLOCKIO *blkio;
    NUTSPINODE *node;

    /* Sanity check. */
    if (len == 0) {
        return 0;
    }
    NUTASSERT(data != NULL);
    NUTASSERT(dev != NULL);
    NUTASSERT(dev->dev_dcb != NULL);
    NUTASSERT(dev->dev_icb != NULL);
    blkio = (NUTBLOCKIO *) dev->dev_dcb;
    node = (NUTSPINODE *) dev->dev_icb;
    NUTASSERT(blkio->blkio_info != NULL);
    pshft = ((AT45D_INFO *) blkio->blkio_info)->at45d_pshft;
    step = ((AT45D_INFO *) blkio->blkio_info)->at45d_psize;
    limit = ((AT45D_INFO *) blkio->blkio_info)->at45d_pages;

    while (len) {
        if (step > len) {
            step = len;
        }
        /* Copy data to the internal DataFlash RAM buffer. */
        if (At45dCommand(node, DFCMD_BUF1_WRITE, 0, 4, dp, NULL, step)) {
            break;
        }
        /* Erase the page and flash the RAM buffer contents. */
        if (At45dCommand(node, DFCMD_BUF1_FLASH, pgn << pshft, 4, NULL, NULL, 0)) {
            break;
        }
        if (At45dWaitReady(node, AT45_WRITE_POLLS, 1)) {
            break;
        }
        if (rc < 0) {
            rc = 0;
        }
        rc += step;
        dp += step;
        len -= step;
        if (++pgn >= limit) {
            break;
        }
    }
    return rc;
}

#ifdef __HARVARD_ARCH__
int SpiAt45dPageWrite_P(NUTDEVICE * dev, uint32_t pgn, PGM_P data, int len)
{
    return -1;
}
#endif

/*!
 * \brief Return the number of pages.
 *
 * \param dev Specifies the registered DataFlash device.
 *
 * \return The total number of pages available in this device.
 */
uint32_t SpiAt45dPages(NUTDEVICE * dev)
{
    NUTBLOCKIO *blkio;

    NUTASSERT(dev != NULL);
    NUTASSERT(dev->dev_dcb != NULL);
    blkio = dev->dev_dcb;

    NUTASSERT(blkio->blkio_info != NULL);
    return ((AT45D_INFO *) blkio->blkio_info)->at45d_pages;
}

/*!
 * \brief Return the page size.
 *
 * \param dev Specifies the registered DataFlash device.
 *
 * \return The number of bytes per page.
 */
int SpiAt45dPageSize(NUTDEVICE * dev)
{
    NUTBLOCKIO *blkio;

    NUTASSERT(dev != NULL);
    NUTASSERT(dev->dev_dcb != NULL);
    blkio = dev->dev_dcb;

    NUTASSERT(blkio->blkio_info != NULL);
    return ((AT45D_INFO *) blkio->blkio_info)->at45d_psize;
}

/*!
 * \brief Perform block I/O device control functions.
 *
 * This function is called by the ioctl() function of the C runtime
 * library. Applications should not directly call this function.
 *
 * \param dev  Identifies the device that receives the control command.
 * \param req  Requested control command. May be set to one of the
 *             following constants:
 *             - \ref NUTBLKDEV_MEDIAAVAIL
 *             - \ref NUTBLKDEV_MEDIACHANGE
 *
 * \param conf Points to a buffer that contains any data required for
 *             the given control function or receives data from that
 *             function.
 * \return 0 on success, -1 otherwise.
 */
int SpiAt45dIOCtl(NUTDEVICE * dev, int req, void *conf)
{
    int rc = 0;

    switch (req) {
    case NUTBLKDEV_MEDIAAVAIL:
        /* Modification required for DataFlash cards. */
        {
            int *flg;
            NUTASSERT(conf != NULL);
            flg = (int *) conf;
            *flg = 1;
        }
        break;
    case NUTBLKDEV_MEDIACHANGE:
        /* Modification required for DataFlash cards. */
        {
            int *flg;
            NUTASSERT(conf != NULL);
            flg = (int *) conf;
            *flg = 0;
        }
        break;
    default:
        rc = -1;
        break;
    }
    return rc;
}
