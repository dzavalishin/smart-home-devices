/*
 * Copyright (C) 2008-2011 by egnite GmbH
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
 * \file dev/spi_blkio_at45d.c
 * \brief Low level block I/O routines for Atmel AT45D SPI Flash.
 *
 * \verbatim
 * $Id$
 * \endverbatim
 */

#include <cfg/memory.h>
#include <sys/nutdebug.h>

#include <dev/blockdev.h>
#include <dev/spi_node_at45d.h>

/*!
 * \addtogroup xgSpiBlockIoAt45d
 */
/*@{*/

/*!
 * \brief Initialize the block I/O interface.
 *
 * \param dev Specifies the blcok I/O device.
 *
 * \return 0 on success or -1 in case of an error.
 */
static int At45dBlkIoInit(NUTDEVICE * dev)
{
    NUTBLOCKIO *blkio;
    NUTSPINODE *node;
    AT45D_INFO *df;

    NUTASSERT(dev != NULL);
    NUTASSERT(dev->dev_dcb != NULL);
    NUTASSERT(dev->dev_icb != NULL);
    blkio = dev->dev_dcb;
    node = dev->dev_icb;

    df = At45dNodeProbe(node);
    if (df) {
        /* Known DataFlash type detected. */
        blkio->blkio_info = (void *) df;
        blkio->blkio_blk_cnt = df->at45d_pages;
        blkio->blkio_blk_siz = df->at45d_psize;
        return 0;
    }
    /* No known DataFlash type detected. */
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
static int At45dBlkIoRead(NUTDEVICE * dev, uint32_t pgn, void *data, int len)
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

    if (At45dNodeLock((NUTSPINODE *) dev->dev_icb) ||
        At45dNodeTransfer((NUTSPINODE *) dev->dev_icb, DFCMD_CONT_READ, pgn, 8, NULL, data, len)) {
        At45dNodeUnlock((NUTSPINODE *) dev->dev_icb);
        return -1;
    }
    At45dNodeUnlock((NUTSPINODE *) dev->dev_icb);
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
static int At45dBlkIoWrite(NUTDEVICE * dev, uint32_t pgn, CONST void *data, int len)
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
        if (At45dNodeLock(node)) {
            break;
        }
        /* Copy data to the internal DataFlash RAM buffer. */
        if (At45dNodeTransfer(node, DFCMD_BUF1_WRITE, 0, 4, dp, NULL, step)) {
            break;
        }
        /* Erase the page and flash the RAM buffer contents. */
        if (At45dNodeCommand(node, DFCMD_BUF1_FLASH, pgn << pshft, 4)) {
            break;
        }
        if (At45dNodeWaitReady(node, AT45_WRITE_POLLS, 1)) {
            break;
        }
        At45dNodeUnlock(node);
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
static int At45dBlkIoWrite_P(NUTDEVICE * dev, uint32_t pgn, PGM_P data, int len)
{
    return -1;
}
#endif

/*!
 * \brief Perform block I/O device control functions.
 *
 * This function is called by the ioctl() function of the C runtime
 * library.
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
static int At45dBlkIoCtl(NUTDEVICE * dev, int req, void *conf)
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

#ifndef BLKIO_MOUNT_OFFSET_AT45D0
#ifdef MOUNT_OFFSET_AT45D0
#define BLKIO_MOUNT_OFFSET_AT45D0       MOUNT_OFFSET_AT45D0
#else
#define BLKIO_MOUNT_OFFSET_AT45D0       0
#endif
#endif

#ifndef BLKIO_MOUNT_TOP_RESERVE_AT45D0
#ifdef MOUNT_TOP_RESERVE_AT45D0
#define BLKIO_MOUNT_TOP_RESERVE_AT45D0  MOUNT_TOP_RESERVE_AT45D0
#else
#define BLKIO_MOUNT_TOP_RESERVE_AT45D0  1
#endif
#endif

/*!
 * \brief First AT45D block I/O control implementation structure.
 */
static NUTBLOCKIO blkIoAt45d0 = {
    NULL,                       /*!< \brief Device specific parameters, blkio_info. */
    0,                          /*!< \brief Total number of pages, blkio_blk_cnt. */
    0,                          /*!< \brief Number of bytes per page, blkio_blk_siz. */
    BLKIO_MOUNT_OFFSET_AT45D0,  /*!< \brief Number of sectors reserved at bottom, blkio_vol_bot. */
    BLKIO_MOUNT_TOP_RESERVE_AT45D0,     /*!< \brief Number of sectors reserved at top, blkio_vol_top. */
    At45dBlkIoRead,             /*!< \brief Read from node, blkio_read. */
    At45dBlkIoWrite,            /*!< \brief Write to node, blkio_write. */
#ifdef __HARVARD_ARCH__
    At45dBlkIoWrite_P,          /*!< \brief Write program memory to node, blkio_write_P. */
#endif
    At45dBlkIoCtl               /*!< \brief Control functions, blkio_ioctl. */
};

/*!
 * \brief First AT45D block I/O device implementation structure.
 */
NUTDEVICE devSpiBlkAt45d0 = {
    NULL,                       /*!< \brief Pointer to next device, dev_next. */
    {'A', 'T', '4', '5', 'D', '0', 0, 0, 0},    /*!< \brief Unique device name, dev_name. */
    IFTYP_BLKIO,                /*!< \brief Type of device, dev_type. */
    0,                          /*!< \brief Base address, dev_base (not used). */
    0,                          /*!< \brief First interrupt number, dev_irq (not used). */
    &nodeAt45d0,                /*!< \brief Interface control block, dev_icb. */
    &blkIoAt45d0,               /*!< \brief Driver control block, dev_dcb. */
    At45dBlkIoInit,             /*!< \brief Driver initialization routine, dev_init. */
    NutBlockDeviceIOCtl,        /*!< \brief Driver specific control function, dev_ioctl. */
    NutBlockDeviceRead,         /*!< \brief Read from device, dev_read. */
    NutBlockDeviceWrite,        /*!< \brief Write to device, dev_write. */
#ifdef __HARVARD_ARCH__
    NutBlockDeviceWrite_P,      /*!< \brief Write data from program space to device, dev_write_P. */
#endif
    NutBlockDeviceOpen,         /*!< \brief Mount volume, dev_open. */
    NutBlockDeviceClose,        /*!< \brief Unmount volume, dev_close. */
    NutBlockDeviceSize          /*!< \brief Request file size, dev_size. */
};

#ifndef BLKIO_MOUNT_OFFSET_AT45D1
#ifdef MOUNT_OFFSET_AT45D1
#define BLKIO_MOUNT_OFFSET_AT45D1       MOUNT_OFFSET_AT45D1
#else
#define BLKIO_MOUNT_OFFSET_AT45D1       0
#endif
#endif

#ifndef BLKIO_MOUNT_TOP_RESERVE_AT45D1
#ifdef MOUNT_TOP_RESERVE_AT45D1
#define BLKIO_MOUNT_TOP_RESERVE_AT45D1  MOUNT_TOP_RESERVE_AT45D1
#else
#define BLKIO_MOUNT_TOP_RESERVE_AT45D1  1
#endif
#endif

/*!
 * \brief Second AT45D block I/O control implementation structure.
 */
static NUTBLOCKIO blkIoAt45d1 = {
    NULL,                       /*!< \brief Device specific parameters, blkio_info. */
    0,                          /*!< \brief Total number of pages, blkio_blk_cnt. */
    0,                          /*!< \brief Number of bytes per page, blkio_blk_siz. */
    BLKIO_MOUNT_OFFSET_AT45D1,  /*!< \brief Number of sectors reserved at bottom, blkio_vol_bot. */
    BLKIO_MOUNT_TOP_RESERVE_AT45D1,     /*!< \brief Number of sectors reserved at top, blkio_vol_top. */
    At45dBlkIoRead,             /*!< \brief Read from node, blkio_read. */
    At45dBlkIoWrite,            /*!< \brief Write to node, blkio_write. */
#ifdef __HARVARD_ARCH__
    At45dBlkIoWrite_P,          /*!< \brief Write program memory to node, blkio_write_P. */
#endif
    At45dBlkIoCtl               /*!< \brief Control functions, blkio_ioctl. */
};

/*!
 * \brief Second AT45D block I/O device implementation structure.
 */
NUTDEVICE devSpiBlkAt45d1 = {
    NULL,                       /*!< \brief Pointer to next device, dev_next. */
    {'A', 'T', '4', '5', 'D', '1', 0, 0, 0},    /*!< \brief Unique device name, dev_name. */
    IFTYP_BLKIO,                /*!< \brief Type of device, dev_type. */
    0,                          /*!< \brief Base address, dev_base (not used). */
    0,                          /*!< \brief First interrupt number, dev_irq (not used). */
    &nodeAt45d1,                /*!< \brief Interface control block, dev_icb. */
    &blkIoAt45d1,               /*!< \brief Driver control block, dev_dcb. */
    At45dBlkIoInit,             /*!< \brief Driver initialization routine, dev_init. */
    NutBlockDeviceIOCtl,        /*!< \brief Driver specific control function, dev_ioctl. */
    NutBlockDeviceRead,         /*!< \brief Read from device, dev_read. */
    NutBlockDeviceWrite,        /*!< \brief Write to device, dev_write. */
#ifdef __HARVARD_ARCH__
    NutBlockDeviceWrite_P,      /*!< \brief Write data from program space to device, dev_write_P. */
#endif
    NutBlockDeviceOpen,         /*!< \brief Mount volume, dev_open. */
    NutBlockDeviceClose,        /*!< \brief Unmount volume, dev_close. */
    NutBlockDeviceSize          /*!< \brief Request file size, dev_size. */
};

#ifndef BLKIO_MOUNT_OFFSET_AT45D2
#ifdef MOUNT_OFFSET_AT45D2
#define BLKIO_MOUNT_OFFSET_AT45D2       MOUNT_OFFSET_AT45D2
#else
#define BLKIO_MOUNT_OFFSET_AT45D2       0
#endif
#endif

#ifndef BLKIO_MOUNT_TOP_RESERVE_AT45D2
#ifdef MOUNT_TOP_RESERVE_AT45D2
#define BLKIO_MOUNT_TOP_RESERVE_AT45D2  MOUNT_TOP_RESERVE_AT45D2
#else
#define BLKIO_MOUNT_TOP_RESERVE_AT45D2  1
#endif
#endif

/*!
 * \brief Third AT45D block I/O control implementation structure.
 */
static NUTBLOCKIO blkIoAt45d2 = {
    NULL,                       /*!< \brief Device specific parameters, blkio_info. */
    0,                          /*!< \brief Total number of pages, blkio_blk_cnt. */
    0,                          /*!< \brief Number of bytes per page, blkio_blk_siz. */
    BLKIO_MOUNT_OFFSET_AT45D2,  /*!< \brief Number of sectors reserved at bottom, blkio_vol_bot. */
    BLKIO_MOUNT_TOP_RESERVE_AT45D2,     /*!< \brief Number of sectors reserved at top, blkio_vol_top. */
    At45dBlkIoRead,             /*!< \brief Read from node, blkio_read. */
    At45dBlkIoWrite,            /*!< \brief Write to node, blkio_write. */
#ifdef __HARVARD_ARCH__
    At45dBlkIoWrite_P,          /*!< \brief Write program memory to node, blkio_write_P. */
#endif
    At45dBlkIoCtl               /*!< \brief Control functions, blkio_ioctl. */
};

/*!
 * \brief Third AT45D block I/O device implementation structure.
 */
NUTDEVICE devSpiBlkAt45d2 = {
    NULL,                       /*!< \brief Pointer to next device, dev_next. */
    {'A', 'T', '4', '5', 'D', '2', 0, 0, 0},    /*!< \brief Unique device name, dev_name. */
    IFTYP_BLKIO,                /*!< \brief Type of device, dev_type. */
    0,                          /*!< \brief Base address, dev_base (not used). */
    0,                          /*!< \brief First interrupt number, dev_irq (not used). */
    &nodeAt45d2,                /*!< \brief Interface control block, dev_icb. */
    &blkIoAt45d2,               /*!< \brief Driver control block, dev_dcb. */
    At45dBlkIoInit,             /*!< \brief Driver initialization routine, dev_init. */
    NutBlockDeviceIOCtl,        /*!< \brief Driver specific control function, dev_ioctl. */
    NutBlockDeviceRead,         /*!< \brief Read from device, dev_read. */
    NutBlockDeviceWrite,        /*!< \brief Write to device, dev_write. */
#ifdef __HARVARD_ARCH__
    NutBlockDeviceWrite_P,      /*!< \brief Write data from program space to device, dev_write_P. */
#endif
    NutBlockDeviceOpen,         /*!< \brief Mount volume, dev_open. */
    NutBlockDeviceClose,        /*!< \brief Unmount volume, dev_close. */
    NutBlockDeviceSize          /*!< \brief Request file size, dev_size. */
};

#ifndef BLKIO_MOUNT_OFFSET_AT45D3
#ifdef MOUNT_OFFSET_AT45D3
#define BLKIO_MOUNT_OFFSET_AT45D3       MOUNT_OFFSET_AT45D3
#else
#define BLKIO_MOUNT_OFFSET_AT45D3       0
#endif
#endif

#ifndef BLKIO_MOUNT_TOP_RESERVE_AT45D3
#ifdef MOUNT_TOP_RESERVE_AT45D3
#define BLKIO_MOUNT_TOP_RESERVE_AT45D3  MOUNT_TOP_RESERVE_AT45D3
#else
#define BLKIO_MOUNT_TOP_RESERVE_AT45D3  1
#endif
#endif

/*!
 * \brief Forth AT45D block I/O control implementation structure.
 */
static NUTBLOCKIO blkIoAt45d3 = {
    NULL,                       /*!< \brief Device specific parameters, blkio_info. */
    0,                          /*!< \brief Total number of pages, blkio_blk_cnt. */
    0,                          /*!< \brief Number of bytes per page, blkio_blk_siz. */
    BLKIO_MOUNT_OFFSET_AT45D3,  /*!< \brief Number of sectors reserved at bottom, blkio_vol_bot. */
    BLKIO_MOUNT_TOP_RESERVE_AT45D3,     /*!< \brief Number of sectors reserved at top, blkio_vol_top. */
    At45dBlkIoRead,             /*!< \brief Read from node, blkio_read. */
    At45dBlkIoWrite,            /*!< \brief Write to node, blkio_write. */
#ifdef __HARVARD_ARCH__
    At45dBlkIoWrite_P,          /*!< \brief Write program memory to node, blkio_write_P. */
#endif
    At45dBlkIoCtl               /*!< \brief Control functions, blkio_ioctl. */
};

/*!
 * \brief Forth AT45D block I/O device implementation structure.
 */
NUTDEVICE devSpiBlkAt45d3 = {
    NULL,                       /*!< \brief Pointer to next device, dev_next. */
    {'A', 'T', '4', '5', 'D', '3', 0, 0, 0},    /*!< \brief Unique device name, dev_name. */
    IFTYP_BLKIO,                /*!< \brief Type of device, dev_type. */
    0,                          /*!< \brief Base address, dev_base (not used). */
    0,                          /*!< \brief First interrupt number, dev_irq (not used). */
    &nodeAt45d3,                /*!< \brief Interface control block, dev_icb. */
    &blkIoAt45d3,               /*!< \brief Driver control block, dev_dcb. */
    At45dBlkIoInit,             /*!< \brief Driver initialization routine, dev_init. */
    NutBlockDeviceIOCtl,        /*!< \brief Driver specific control function, dev_ioctl. */
    NutBlockDeviceRead,         /*!< \brief Read from device, dev_read. */
    NutBlockDeviceWrite,        /*!< \brief Write to device, dev_write. */
#ifdef __HARVARD_ARCH__
    NutBlockDeviceWrite_P,      /*!< \brief Write data from program space to device, dev_write_P. */
#endif
    NutBlockDeviceOpen,         /*!< \brief Mount volume, dev_open. */
    NutBlockDeviceClose,        /*!< \brief Unmount volume, dev_close. */
    NutBlockDeviceSize          /*!< \brief Request file size, dev_size. */
};

/*@}*/
