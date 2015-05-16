/*
 * Copyright (C) 2008-2009 by egnite GmbH
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
 * \file dev/spi_at45d1.c
 * \brief Second AT45D DataFlash device.
 *
 * \verbatim
 * $Id: spi_at45d1.c 2379 2009-01-09 17:59:05Z haraldkipp $
 * \endverbatim
 */

#include <cfg/memory.h>

#include <dev/blockdev.h>
#include <dev/spi_at45d.h>

#ifndef SPI_RATE_AT45D1
#define SPI_RATE_AT45D1  1000000
#endif

#ifndef SPI_MODE_AT45D1
#ifndef SPI_CSHIGH_AT45D1
#define SPI_MODE_AT45D1 (SPI_MODE_3 | SPI_MODE_CSHIGH)
#else
#define SPI_MODE_AT45D1 SPI_MODE_3
#endif
#endif

#ifndef MOUNT_OFFSET_AT45D1
#define MOUNT_OFFSET_AT45D1         0
#endif

#ifndef MOUNT_TOP_RESERVE_AT45D1
#define MOUNT_TOP_RESERVE_AT45D1    0
#endif

/*!
 * \brief AT45D DataFlash SPI node implementation structure.
 */
NUTSPINODE nodeSpiAt45d1 = {
    NULL,                       /*!< \brief Pointer to the bus controller driver, node_bus. */
    NULL,                       /*!< \brief Pointer to device driver specific settings, node_stat. */
    SPI_RATE_AT45D1,            /*!< \brief Initial clock rate, node_rate. */
    SPI_MODE_AT45D1,            /*!< \brief Initial mode, node_mode. */
    8,                          /*!< \brief Initial data bits, node_bits. */
    0                           /*!< \brief Chip select, node_cs. */
};

/*!
 * \brief AT45D DataFlash block I/O implementation structure.
 */
static NUTBLOCKIO blkIoAt45d1 = {
    NULL,                       /*!< \brief Device specific parameters, blkio_info. */
    0,                          /*!< \brief Total number of pages, blkio_blk_cnt. */
    0,                          /*!< \brief Number of bytes per page, blkio_blk_siz. */
    MOUNT_OFFSET_AT45D1,        /*!< \brief Number of sectors reserved at bottom, blkio_vol_bot. */
    MOUNT_TOP_RESERVE_AT45D1,   /*!< \brief Number of sectors reserved at top, blkio_vol_top. */
    SpiAt45dPageRead,           /*!< \brief Read from node, blkio_read. */
    SpiAt45dPageWrite,          /*!< \brief Write to node, blkio_write. */
#ifdef __HARVARD_ARCH__
    SpiAt45dPageWrite_P,        /*!< \brief Write program memory to node, blkio_write_P. */
#endif
    SpiAt45dIOCtl               /*!< \brief Control functions, blkio_ioctl. */
};

/*!
 * \brief AT45D DataFlash device implementation structure.
 */
NUTDEVICE devSpiAt45d1 = {
    NULL,                       /* Pointer to next device, dev_next. */
    {'A', 'T', '4', '5', 'D', '1', 0, 0, 0},    /* Unique device name, dev_name. */
    IFTYP_BLKIO,                /* Type of device, dev_type. */
    0,                          /* Base address, dev_base (not used). */
    0,                          /* First interrupt number, dev_irq (not used). */
    &nodeSpiAt45d1,             /* Interface control block, dev_icb. */
    &blkIoAt45d1,               /* Driver control block, dev_dcb. */
    SpiAt45dInit,               /* Driver initialization routine, dev_init. */
    NutBlockDeviceIOCtl,        /* Driver specific control function, dev_ioctl. */
    NutBlockDeviceRead,         /* Read from device, dev_read. */
    NutBlockDeviceWrite,        /* Write to device, dev_write. */
#ifdef __HARVARD_ARCH__
    NutBlockDeviceWrite_P,      /* Write data from program space to device, dev_write_P. */
#endif
    NutBlockDeviceOpen,         /* Open a device or file, dev_open. */
    NutBlockDeviceClose,        /* Close a device or file, dev_close. */
    NutBlockDeviceSize          /* Request file size, dev_size. */
};
