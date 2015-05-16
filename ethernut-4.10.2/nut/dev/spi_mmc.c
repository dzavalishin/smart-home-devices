/*
 * Copyright (C) 2010 by egnite GmbH
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
 * \brief Basic SPI bus block device driver for multimedia cards.
 *
 * The driver provides generic memory card access, but doesn't include 
 * low level hardware support like card detection. This must be provided 
 * by a hardware specific support module.
 *
 * \verbatim
 *
 * $Id$
 *
 * \endverbatim
 */

#include <cfg/mmci.h>
#include <sys/nutdebug.h>

#if 0
/* Use for local debugging. */
#define NUTDEBUG
#include <stdio.h>
#endif

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <memdebug.h>

#include <sys/heap.h>
#include <sys/timer.h>
#include <sys/event.h>
#include <fs/dospart.h>
#include <fs/fs.h>

#include <dev/blockdev.h>
#include <dev/spibus.h>
#include <dev/mmcard.h>

/*!
 * \addtogroup xgMmCard
 */
/*@{*/

#ifndef MMC_BLOCK_SIZE
/*!
 * \brief Block size.
 *
 * Block size in bytes. Do not change unless you are sure that both, 
 * the file system and the hardware support it.
 */
#define MMC_BLOCK_SIZE          512
#endif

#ifndef MMC_MAX_INIT_POLLS
/*!
 * \brief Card init timeout.
 *
 * Max. number of loops waiting for card's idle mode after initialization. 
 * An additional delay of 1 ms is added to each loop after one quarter of 
 * this value elapsed.
 */
#define MMC_MAX_INIT_POLLS      512
#endif

#ifndef MMC_MAX_RESET_RETRIES
/*!
 * \brief Card reset timeout.
 *
 * Max. number of loops waiting for card's idle mode after resetting it.
 */
#define MMC_MAX_RESET_RETRIES   2
#endif

#ifndef MMC_MAX_WRITE_RETRIES
/*!
 * \brief Card write retries.
 *
 * Max. number of retries while writing.
 */
#define MMC_MAX_WRITE_RETRIES   2
#endif

#ifndef MMC_MAX_READ_RETRIES
/*!
 * \brief Card read retries.
 *
 * Max. number of retries while reading.
 */
#define MMC_MAX_READ_RETRIES    MMC_MAX_WRITE_RETRIES
#endif

#ifndef MMC_MAX_CMDACK_POLLS
/*!
 * \brief Command acknowledge timeout.
 *
 * Max. number of loops waiting for card's acknowledge of a command. 
 * An additional delay of 1 ms is added to each loop after three quarter 
 * of this value elapsed.
 */
#define MMC_MAX_CMDACK_POLLS    1024
#endif

#ifndef MMC_MAX_READY_POLLS
/*!
 * \brief Card busy timeout.
 *
 * Max. number of loops waiting for card's ready state.
 * An additional delay of 1 ms is added to each loop after one quarter 
 * of this value elapsed.
 */
#define MMC_MAX_READY_POLLS     800
#endif


/* HACK!!!
   Some SPI hardware just shift around data read from MISO pin to the MOSI pin if the SPI data register 
   is not set manually. At least on the AT91 platform it is impossible to use DMA transfers just for reading  
   and to hold the MOSI line at high level if no tx data is send out at the same time. So we declare a buffer   
   filled with 0xFF here, to make sure the MOSI pin is held at high level (0xFF) all the time during a 
   read only transfer. This buffer is used in CardRXData and several other places too. The buffer is read only.

   This is a very very nasty hack and waists 512 Bytes of ram, but I don't see a better solution right now!
*/

static uint8_t dummy_tx_buf[MMC_BLOCK_SIZE];

/*!
 * \brief Local multimedia card status information.
 */
typedef struct _MMCFCB {
    /*! \brief Attached file system device. 
     */
    NUTDEVICE *fcb_fsdev;

    /*! \brief Partition table entry of the currently mounted partition.
     */
    DOSPART fcb_part;

    /*! \brief Next block number to read.
     *
     * The file system driver will send a NUTBLKDEV_SEEK control command
     * to set this value before calling the read or the write routine.
     *
     * The number is partition relative.
     */
    uint32_t fcb_address;

    /*! \brief Internal block buffer.
     *
     * A file system driver may use this one or optionally provide it's 
     * own buffers.
     *
     * Minimal systems may share their external bus interface with
     * device I/O lines, in which case the buffer must be located
     * in internal memory.
     */
    uint8_t fcb_blkbuf[MMC_BLOCK_SIZE];
} MMCFCB;

#ifdef NUTDEBUG
static void DumpData(uint8_t *buf, size_t len)
{
    int i;
    int j;

    for (i = 0; i < len; i += 16) {
        printf("\n%03x ", i);
        for (j = 0; j < 16; j++) {
            if (i + j < len) {
                printf("%02x ", buf[i + j]);
            } else {
                printf("   ");
            }
        }
        for (j = 0; j < 16; j++) {
            if (i + j < len) {
                if (buf[i + j] >= 32 && buf[i + j] < 127) {
                    putchar(buf[i + j]);
                } else {
                    putchar('.');
                }
            } else {
                break;
            }
        }
    }
    putchar('\n');
}
#endif

/*!
 * \brief Wait while a multimedia card is busy.
 *
 * \param node Specifies the SPI node.
 *
 * \return 0 if the card is ready, -1 if not.
 */
static int CardWaitRdy(NUTSPINODE * node)
{
    int poll = MMC_MAX_READY_POLLS;
    uint8_t b;
    NUTSPIBUS *bus = (NUTSPIBUS *) node->node_bus;

    do {
        (*bus->bus_transfer) (node, dummy_tx_buf, &b, 1);
        if (b == 0xFF) {
            return 0;
        }
#ifdef NUTDEBUG
        putchar('b');
#endif
        if (poll < MMC_MAX_READY_POLLS / 4) {
            NutSleep(1);
        }
    } while (poll--);

    return -1;
}

/*!
 * \brief Get next token from a multimedia card.
 *
 * \param node Specifies the SPI node.
 *
 * \return The token received or 0xFF if timed out.
 */
static uint8_t CardRxTkn(NUTSPINODE * node)
{
    uint8_t rc;

    int poll = MMC_MAX_CMDACK_POLLS;
    NUTSPIBUS *bus = (NUTSPIBUS *) node->node_bus;

    do {
        (*bus->bus_transfer) (node, dummy_tx_buf, &rc, 1);
        if (rc != 0xFF) {
            break;
        }
#ifdef NUTDEBUG
        putchar('w');
#endif
        if (poll < 3 * MMC_MAX_CMDACK_POLLS / 4) {
            NutSleep(1);
        }
    } while (poll--);
#ifdef NUTDEBUG
    printf("[R%02x]", rc);
#endif

    return rc;
}

/*!
 * \brief Send command to a multimedia card.
 *
 * Allocates the bus, transmits the command with the command parameter 
 * set to zero and receives a one or two byte response. Before returning
 * to the caller, the bus is typically released. However, if len is 
 * neither 1 nor 2, the bus will not be released.
 *
 * In SPI mode, the card sends a 1 byte response after every command 
 * except after SEND_STATUS and READ_OCR commands, where 2 or 5 bytes
 * are returned resp.
 *
 * \param node Specifies the SPI node.
 * \param cmd  Command code. See MMCMD_ macros.
 * \param len  Length of the expected response, either 1, 2 or 0. If 0, 
 *             then the first byte of the response will be returned and 
 *             the bus will be kept allocated.
 *
 * \return The 1 or 2 byte response. On time out 0xFFFF is returned and
 *         the bus is released, regardless of the expected response length.
 */
static uint16_t CardTxCommand(NUTSPINODE * node, uint8_t cmd, uint32_t param, int len)
{
    uint16_t rc = 0xFFFF;
    int retries = 4;
    uint8_t txb[7];
    uint8_t rxb;
    NUTSPIBUS *bus;

    bus = (NUTSPIBUS *) node->node_bus;

    /* Send command and parameter. */
    txb[0] = 0xFF;
    txb[1] = MMCMD_HOST | cmd;
    txb[2] = (uint8_t) (param >> 24);
    txb[3] = (uint8_t) (param >> 16);
    txb[4] = (uint8_t) (param >> 8);
    txb[5] = (uint8_t) param;
    /* We are running with CRC disabled. However, the reset command must
    ** be send with a valid CRC. Fortunately this command is sent with a
    ** fixed parameter value of zero, which results in a fixed CRC value. */
    txb[6] = MMCMD_RESET_CRC;

    while (rc == 0xFFFF && retries--) {
        if ((*bus->bus_alloc) (node, 1000)) {
            break;
        }
        if (CardWaitRdy(node) == 0) {
#ifdef NUTDEBUG
            printf("\n[CMD%u,%lu]", cmd, param);
#endif
            (*bus->bus_transfer) (node, txb, NULL, sizeof(txb));
            rxb = CardRxTkn(node);
            if (rxb != 0xFF) {
                rc = rxb;
                if (len == 0) {
                    break;
                }
                if (len == 2) {
                    (*bus->bus_transfer) (node, dummy_tx_buf, &rxb, 1);
                    rc <<= 8;
                    rc |= rxb;
                }
            }
        }
        (*bus->bus_release) (node);
    }
    return rc;
}

/*!
 * \brief Initialize the multimedia card.
 *
 * This routine will put a newly powered up card into SPI mode.
 * It is called by SpiMmcMount().
 *
 * \param node Specifies the SPI node.
 *
 * \return 0 on success, -1 otherwise.
 */
static int CardInit(NUTSPINODE * node)
{
    int i;
    int j;
    uint8_t rsp;

    /*
     * Switch to idle state and wait until initialization is running
     * or idle state is reached.
     */
    for (i = 0; i < MMC_MAX_RESET_RETRIES; i++) {
        rsp = CardTxCommand(node, MMCMD_GO_IDLE_STATE, 0, 1);
        for (j = 0; j < MMC_MAX_INIT_POLLS; j++) {
            rsp = CardTxCommand(node, MMCMD_SEND_OP_COND, 0, 1);
            if (rsp == MMR1_IDLE_STATE) {
                return 0;
            }
            if (j > MMC_MAX_INIT_POLLS / 4) {
                NutSleep(1);
            }
        }
    }
    return -1;
}

/*!
 * \brief Read data transaction.
 *
 * This routine is used to read data blocks as well as reading the CSD 
 * and CID registers.
 *
 * \param node  Specifies the SPI node.
 * \param cmd   Command code. See MMCMD_ macros.
 * \param param Command parameter.
 * \param buf   Pointer to the data buffer to fill.
 * \param len   Number of data bytes to read.
 *
 * \return 0 on success or -1 in case of an error.
 */
static int CardRxData(NUTSPINODE * node, uint8_t cmd, uint32_t param, uint8_t *buf, int len)
{
    int rc = -1;
    uint8_t rsp;
    NUTSPIBUS *bus;
    int retries = MMC_MAX_READ_RETRIES;


    /* Sanity checks. */
    NUTASSERT(node != NULL);
    NUTASSERT(node->node_bus != NULL);

    bus = (NUTSPIBUS *) node->node_bus;

    while (rc && retries--) {
        rsp = (uint8_t)CardTxCommand(node, cmd, param, 0);
        if (rsp != 0xFF) {
            if (rsp == 0 && CardRxTkn(node) == 0xFE) {
                /* Data transfer. */
                (*bus->bus_transfer) (node, dummy_tx_buf, buf, len);
                /* Ignore the CRC. */
                (*bus->bus_transfer) (node, dummy_tx_buf, NULL, 2);
                rc = 0;
            }
            (*bus->bus_release) (node);
        }
    }
    return rc;
}

/*!
 * \brief Read data transaction.
 *
 * This routine is used to write data blocks as well as writing to the 
 * CSD register.
 *
 * \param node  Specifies the SPI node.
 * \param cmd   Command code. See MMCMD_ macros.
 * \param param Command parameter.
 * \param buf   Pointer to the data buffer.
 * \param len   Number of data bytes to write.
 *
 * \return 0 on success or -1 in case of an error.
 */
static int CardTxData(NUTSPINODE * node, uint8_t cmd, uint32_t param, CONST uint8_t *buf, int len)
{
    int rc = -1;
    uint8_t rsp;
    NUTSPIBUS *bus;
    int retries = MMC_MAX_WRITE_RETRIES;

    /* Sanity checks. */
    NUTASSERT(node != NULL);
    NUTASSERT(node->node_bus != NULL);

    bus = (NUTSPIBUS *) node->node_bus;
    while (rc && retries--) {
        rsp = (uint8_t)CardTxCommand(node, cmd, param, 0);
        if (rsp != 0xFF) {
            if (rsp == 0) {
                uint8_t tkn[2] = { 0xFF, 0xFE };

                /* Send start token. */
                (*bus->bus_transfer) (node, &tkn, NULL, sizeof(tkn));
                /* Data transfer. */
                (*bus->bus_transfer) (node, buf, NULL, len);
                /* Send dummy CRC. */
                (*bus->bus_transfer) (node, dummy_tx_buf, NULL, 2);
                /* Get response. */
                if (CardRxTkn(node) == 0xE5) {
                    rc = 0;
                }
            }
            (*bus->bus_release) (node);
        }
    }
    return rc;
}

/*!
 * \brief Read data blocks from a mounted partition.
 *
 * Applications should not call this function directly, but use the
 * stdio interface.
 *
 * \param nfp    Pointer to a ::NUTFILE structure, obtained by a previous 
 *               call to SpiMmcMount().
 * \param buffer Pointer to the data buffer to fill.
 * \param num    Maximum number of blocks to read. However, reading 
 *               multiple blocks is not yet supported by this driver.
 *
 * \return The number of blocks actually read. A return value of -1 
 *         indicates an error.
 */
int SpiMmcBlockRead(NUTFILE * nfp, void *buffer, int num)
{
    MMCFCB *fcb;
    NUTDEVICE *dev;
    MEMCARDSUPP *msc;

    /* Sanity checks. */
    NUTASSERT(nfp != NULL);
    NUTASSERT(nfp->nf_fcb != NULL);
    NUTASSERT(nfp->nf_dev != NULL);
    fcb = (MMCFCB *) nfp->nf_fcb;
    dev = (NUTDEVICE *) nfp->nf_dev;
    NUTASSERT(dev->dev_dcb != NULL);
    msc = (MEMCARDSUPP *) dev->dev_dcb;

    /* Make sure there was no card change. */
    if (msc->mcs_cf == 0) {
        /* Activate the write indicator. */
        msc->mcs_act(NUTMC_IND_READ);
        /* Use the internal buffer if none is provided. */
        if (buffer == NULL) {
            buffer = fcb->fcb_blkbuf;
        }
        /* Transfer the data from the card. */
        if (CardRxData(dev->dev_icb, MMCMD_READ_SINGLE_BLOCK, fcb->fcb_address, buffer, MMC_BLOCK_SIZE) == 0) {
            /* Deactivate the indicator. */
            msc->mcs_act(NUTMC_IND_OFF);
#ifdef NUTDEBUG
            DumpData(buffer, MMC_BLOCK_SIZE);
#endif
            return 1;
        }
    }
    /* Activate the error indicator. */
    msc->mcs_act(NUTMC_IND_ERROR);
    return -1;
}

/*!
 * \brief Write data blocks to a mounted partition.
 *
 * Applications should not call this function directly, but use the
 * stdio interface.
 *
 * \param nfp    Pointer to a \ref NUTFILE structure, obtained by a previous 
 *               call to SpiMmcMount().
 * \param buffer Pointer to the data to be written. However, writing
 *               multiple blocks is not yet supported by this driver.
 * \param num    Number of blocks to write.
 *
 * \return The number of blocks written. A return value of -1 indicates an 
 *         error.
 */
int SpiMmcBlockWrite(NUTFILE * nfp, CONST void *buffer, int num)
{
    MMCFCB *fcb;
    NUTDEVICE *dev;
    MEMCARDSUPP *msc;

    /* Sanity checks. */
    NUTASSERT(nfp != NULL);
    NUTASSERT(nfp->nf_fcb != NULL);
    NUTASSERT(nfp->nf_dev != NULL);
    fcb = (MMCFCB *) nfp->nf_fcb;
    dev = (NUTDEVICE *) nfp->nf_dev;
    NUTASSERT(dev->dev_dcb != NULL);
    msc = (MEMCARDSUPP *) dev->dev_dcb;

    /* Make sure there was no card change. */
    if (msc->mcs_cf == 0) {
        /* Activate the write indicator. */
        msc->mcs_act(NUTMC_IND_WRITE);
        /* Use the internal buffer if none is provided. */
        if (buffer == NULL) {
            buffer = fcb->fcb_blkbuf;
        }
        /* Transfer the data to the card. */
        if (CardTxData(dev->dev_icb, MMCMD_WRITE_BLOCK, fcb->fcb_address, buffer, MMC_BLOCK_SIZE) == 0) {
            /* Deactivate the indicator. */
            msc->mcs_act(NUTMC_IND_OFF);
            return 1;
        }
    }
    /* Activate the error indicator. */
    msc->mcs_act(NUTMC_IND_ERROR);
    return -1;
}

#ifdef __HARVARD_ARCH__
/*! 
 * \brief Write data blocks from program space to a mounted partition.
 *
 * This function is not yet implemented and will always return -1.
 *
 * Similar to SpiMmcBlockWrite() except that the data is located in 
 * program memory.
 *
 * Applications should not call this function directly, but use the
 * stdio interface.
 *
 * \param nfp    File pointer to a previously opened device.
 * \param buffer Pointer to the data bytes in program space to be written.
 * \param num    Maximum number of blocks to write. However, writing
 *               multiple blocks is not yet supported by this driver.
 *
 * \return The number of blocks written. A return value of -1 indicates an 
 *         error.
 */
int SpiMmcBlockWrite_P(NUTFILE * nfp, PGM_P buffer, int num)
{
    return -1;
}
#endif

int SpiMmcUnmount(NUTFILE * nfp);

/*!
 * \brief Mount a partition.
 *
 * Nut/OS doesn't provide specific routines for mounting. Instead routines
 * for opening files are used.
 *
 * Applications should not directly call this function, but use the high
 * level stdio routines for opening a file.
 *
 * \param dev  Pointer to the MMC device.
 * \param name Partition number followed by a slash followed by a name
 *             of the file system device. Both items are optional. If no 
 *             file system driver name is given, the first file system
 *             driver found in the list of registered devices will be 
 *             used. If no partition number is specified or if partition
 *             zero is given, the first active primary partition will be 
 *             used.
 * \param mode Opening mode. Currently ignored, but 
 *             \code _O_RDWR | _O_BINARY \endcode should be used for
 *             compatibility with future enhancements.
 * \param acc  File attributes, ignored.
 *
 * \return Pointer to a newly created file pointer to the mounted
 *         partition or NUTFILE_EOF in case of any error.
 */
NUTFILE *SpiMmcMount(NUTDEVICE * dev, CONST char *name, int mode, int acc)
{
    int partno = 0;
    int i;
    NUTDEVICE *fsdev;
    NUTFILE *nfp;
    MMCFCB *fcb;
    DOSPART *part;
    MEMCARDSUPP *msc = (MEMCARDSUPP *) dev->dev_dcb;
    FSCP_VOL_MOUNT mparm;
    NUTSPINODE * node = (NUTSPINODE *) dev->dev_icb;

    /* Return an error if no card is detected. */
    if ((msc->mcs_sf & NUTMC_SF_CD) == 0) {
        errno = ENODEV;
        return NUTFILE_EOF;
    }
    /* Clear the card change flag. */
    msc->mcs_cf = 0;

    /* Set the card in SPI mode. */
    if (CardInit(node)) {
        errno = ENODEV;
        return NUTFILE_EOF;
    }

    /* Parse the name for a partition number and a file system driver. */
    if (*name) {
        partno = atoi(name);
        do {
            name++;
        } while (*name && *name != '/');
        if (*name == '/') {
            name++;
        }
    }

    /*
     * Check the list of registered devices for the given name of the
     * files system driver. If none has been specified, get the first
     * file system driver in the list. Hopefully the application
     * registered one only.
     */
    for (fsdev = nutDeviceList; fsdev; fsdev = fsdev->dev_next) {
        if (*name == 0) {
            if (fsdev->dev_type == IFTYP_FS) {
                break;
            }
        } else if (strcmp(fsdev->dev_name, name) == 0) {
            break;
        }
    }

    if (fsdev == 0) {
#ifdef NUTDEBUG
        printf("[NoFS'%s]", name);
#endif
        errno = ENODEV;
        return NUTFILE_EOF;
    }

    if ((fcb = calloc(1, sizeof(MMCFCB))) == 0) {
        errno = ENOMEM;
        return NUTFILE_EOF;
    }
    fcb->fcb_fsdev = fsdev;

    /* Read MBR. */
    if (CardRxData(node, MMCMD_READ_SINGLE_BLOCK, 0, fcb->fcb_blkbuf, MMC_BLOCK_SIZE)) {
        free(fcb);
        return NUTFILE_EOF;
    }
#ifdef NUTDEBUG
    DumpData(fcb->fcb_blkbuf, MMC_BLOCK_SIZE);
#endif
    /* Check for the cookie at the end of this sector. */
	if (fcb->fcb_blkbuf[DOSPART_MAGICPOS] != 0x55 || fcb->fcb_blkbuf[DOSPART_MAGICPOS + 1] != 0xAA) {
        free(fcb);
        return NUTFILE_EOF;
	}
    /* Check for the partition table. */
	if(fcb->fcb_blkbuf[DOSPART_TYPEPOS] == 'F' && 
       fcb->fcb_blkbuf[DOSPART_TYPEPOS + 1] == 'A' &&
       fcb->fcb_blkbuf[DOSPART_TYPEPOS + 2] == 'T') {
        /* No partition table. Assume FAT12 and 32MB size. */
        fcb->fcb_part.part_type = PTYPE_FAT12;
        fcb->fcb_part.part_sect_offs = 0;
        fcb->fcb_part.part_sects = 65536; /* How to find out? */
	}
    else {
        /* Read partition table. */
        part = (DOSPART *) & fcb->fcb_blkbuf[DOSPART_SECTORPOS];
#ifdef NUTDEBUG
        for (i = 0; i < 4; i++) {
            printf("- State 0x%02x\n", part[i].part_state);
            printf("  Type  0x%02x\n", part[i].part_type);
            printf("  Start %lu\n", part[i].part_sect_offs);
            printf("  Size  %lu\n", part[i].part_sects);
        }
#endif
        for (i = 1; i <= 4; i++) {
            if (partno) {
                if (i == partno) {
                    /* Found specified partition number. */
                    fcb->fcb_part = *part;
                    break;
                }
            } else if (part->part_state & 0x80) {
                /* Located first active partition. */
                fcb->fcb_part = *part;
                break;
            }
            part++;
        }

        if (fcb->fcb_part.part_type == PTYPE_EMPTY) {
            free(fcb);
            return NUTFILE_EOF;
        }
    }

    if ((nfp = NutHeapAlloc(sizeof(NUTFILE))) == 0) {
        free(fcb);
        errno = ENOMEM;
        return NUTFILE_EOF;
    }
    nfp->nf_next = 0;
    nfp->nf_dev = dev;
    nfp->nf_fcb = fcb;

    /*
     * Mount the file system volume.
     */
    mparm.fscp_bmnt = nfp;
    mparm.fscp_part_type = fcb->fcb_part.part_type;
    if (fsdev->dev_ioctl(fsdev, FS_VOL_MOUNT, &mparm)) {
        SpiMmcUnmount(nfp);
        return NUTFILE_EOF;
    }
    return nfp;
}

/*!
 * \brief Unmount a previously mounted partition.
 *
 * Applications should not directly call this function, but use the high
 * level stdio routines for closing a previously opened volume.
 *
 * \param nfp Pointer to a \ref NUTFILE structure, obtained by a previous 
 *            call to SpiMmcMount().
 * \return 0 on success, -1 otherwise.
 */
int SpiMmcUnmount(NUTFILE * nfp)
{
    int rc;
    MMCFCB *fcb;

    /* Sanity checks. */
    NUTASSERT(nfp != NULL);
    NUTASSERT(nfp->nf_fcb != NULL);
    fcb = (MMCFCB *) nfp->nf_fcb;

    /* Intentionally we do not check the card change flag here to allow the
    ** file system to release all claimed resources. */
    rc = fcb->fcb_fsdev->dev_ioctl(fcb->fcb_fsdev, FS_VOL_UNMOUNT, NULL);
    free(fcb);
    free(nfp);

    return rc;
}

/*!
 * \brief Perform MMC control functions.
 *
 * This function is called by the ioctl() function of the C runtime
 * library. Applications should not directly call this function.
 *
 * \todo Card change detection should verify the serial card number.
 *
 * \param dev  Identifies the device that receives the device-control
 *             function.
 * \param req  Requested control function. May be set to one of the
 *             following constants:
 *             - \ref NUTBLKDEV_MEDIACHANGE
 *             - \ref NUTBLKDEV_INFO
 *             - \ref NUTBLKDEV_SEEK
 *             - \ref MMCARD_GETCID
 *             - \ref MMCARD_GETCSD
 *
 * \param conf Points to a buffer that contains any data required for
 *             the given control function or receives data from that
 *             function.
 * \return 0 on success, -1 otherwise.
 */
int SpiMmcIOCtl(NUTDEVICE * dev, int req, void *conf)
{
    int rc = 0;
    NUTSPINODE * node = (NUTSPINODE *) dev->dev_icb;
    MEMCARDSUPP *msc = (MEMCARDSUPP *) dev->dev_dcb;

    switch (req) {
    case NUTBLKDEV_MEDIAAVAIL:
        {
            int *flg = (int *) conf;
            *flg = msc->mcs_sf & NUTMC_SF_CD;
        }
        break;
    case NUTBLKDEV_MEDIACHANGE:
        {
            int *flg = (int *) conf;
            *flg = msc->mcs_cf;
        }
        break;
    case NUTBLKDEV_INFO:
        {
            BLKPAR_INFO *par = (BLKPAR_INFO *) conf;
            MMCFCB *fcb = (MMCFCB *) par->par_nfp->nf_fcb;

            par->par_nblks = fcb->fcb_part.part_sects;
            par->par_blksz = MMC_BLOCK_SIZE;
            par->par_blkbp = fcb->fcb_blkbuf;
        }
        break;
    case NUTBLKDEV_SEEK:
        {
            BLKPAR_SEEK *par = (BLKPAR_SEEK *) conf;
            MMCFCB *fcb = (MMCFCB *) par->par_nfp->nf_fcb;

            /* Caclaulate the byte offset. */
            fcb->fcb_address = (par->par_blknum + fcb->fcb_part.part_sect_offs) << 9;
        }
        break;
    case MMCARD_GETSTATUS:
        {
            uint16_t *s = (uint16_t *) conf;

            *s = CardTxCommand(node, MMCMD_SEND_STATUS, 0, 2);
        }
        break;
    case MMCARD_GETOCR:
        {
            NUTSPIBUS *bus = (NUTSPIBUS *) node->node_bus;

            if (CardTxCommand(node, MMCMD_READ_OCR, 0, 0) == MMR1_IDLE_STATE) {
                uint32_t * ocr = (uint32_t *) conf;
                uint_fast8_t i;
                uint8_t rxb;

                for (i = 0; i < 4; i++) {
                    (*bus->bus_transfer) (node, dummy_tx_buf, &rxb, 1);
                    *ocr <<= 8;
                    *ocr |= rxb;
                }

            } else {
                rc = 1;
            }
            (*bus->bus_release) (node);
        }
        break;
    case MMCARD_GETCID:
        rc = CardRxData(node, MMCMD_SEND_CID, 0, (uint8_t *) conf, sizeof(MMC_CID));
        break;
    case MMCARD_GETCSD:
        rc = CardRxData(node, MMCMD_SEND_CSD, 0, (uint8_t *) conf, sizeof(MMC_CSD));
        break;
    case MMCARD_GETEXTCSD:
        rc = CardRxData(node, MMCMD_SEND_EXTCSD, 0, (uint8_t *) conf, sizeof(MMC_CSD));
        break;
    default:
        rc = -1;
        break;
    }
    return rc;
}

/*!
 * \brief Initialize MMC driver.
 *
 * Applications should not directly call this function. It is 
 * automatically executed during during device registration by 
 * NutRegisterDevice().
 *
 * \param dev Identifies the device to initialize.
 *
 * \return Always zero.
 */
int SpiMmcInit(NUTDEVICE * dev)
{
    memset(dummy_tx_buf, 0xFF, MMC_BLOCK_SIZE);
    return 0;
}

/*@}*/
