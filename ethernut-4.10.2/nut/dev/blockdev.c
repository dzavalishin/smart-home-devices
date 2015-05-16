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
 * \file dev/blockdev.c
 * \brief Generic block I/O device routines.
 *
 * \verbatim
 * $Id: blockdev.c 2463 2009-02-13 14:52:05Z haraldkipp $
 * \endverbatim
 */

#include <cfg/os.h>
#include <cfg/memory.h>

#include <dev/blockdev.h>
#include <sys/nutdebug.h>
#include <fs/fs.h>

#include <stdlib.h>
#include <errno.h>
#include <memdebug.h>

/*!
 * \brief Block volume information structure type.
 */
typedef struct _BLOCKVOLUME BLOCKVOLUME;

/*!
 * \brief Block volume information structure.
 */
struct _BLOCKVOLUME {
    /*! \brief Attached file system device. 
     */
    NUTDEVICE *vol_fsdev;

    /*! \brief Number of blocks available to the file system. 
     */
    uint32_t vol_blk_cnt;

    /*! \brief Number of bytes in a block. 
     */
    int vol_blk_len;

    /*! \brief First page used by the file system. 
     */
    uint32_t vol_blk_off;

    /*! \brief Next block number to read.
     *
     * The file system driver will send a NUTBLKDEV_SEEK control command
     * to set this value before calling the read or the write routine.
     *
     * The number is zero based.
     */
    uint32_t vol_blk_num;

    /*! \brief Internal block buffer.
     *
     * A file system driver may use this one or optionally provide it's 
     * own buffers.
     *
     * Minimal systems may share their external bus interface with
     * device I/O lines, in which case the buffer must be located
     * in internal memory.
     */
    uint8_t *vol_blk_buf;
};

/*!
 * \brief Find device entry by type.
 *
 * \param type Device type.
 *
 * \return Pointer to the \ref NUTDEVICE structure.
 */
static NUTDEVICE *NutDeviceLookupType(uint_fast8_t type)
{
    NUTDEVICE *dev;

    for (dev = nutDeviceList; dev; dev = dev->dev_next) {
        if (dev->dev_type == type) {
            break;
        }
    }
    return dev;
}

/*!
 * \brief Initialize the block I/O device.
 *
 * This dummy routine may be used by drivers, which do not need any
 * specific initialization.
 *
 * \param dev Specifies the device.
 *
 * \return Always 0.
 */
int NutBlockDeviceInit(NUTDEVICE * dev)
{
    NUTASSERT(dev != NULL);

    return 0;
}

/*!
 * \brief Mount a volume.
 *
 * Nut/OS doesn't provide specific routines for mounting. Instead routines
 * for opening files are used.
 *
 * Applications should not directly call this function, but use the high
 * level stdio routines for opening a file.
 *
 * \param dev  Specifies the block I/O device.
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
NUTFILE *NutBlockDeviceOpen(NUTDEVICE * dev, CONST char *name, int mode, int acc)
{
    NUTDEVICE *fsdev;

    /* Parse name for a file system driver, skip partition number. */
    NUTASSERT(name != NULL);
    if (*name) {
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
    if (*name) {
        fsdev = NutDeviceLookup(name);
    } else {
        fsdev = NutDeviceLookupType(IFTYP_FS);
    }

    if (fsdev == NULL) {
        errno = ENODEV;
    } else {
        BLOCKVOLUME *fcb = malloc(sizeof(BLOCKVOLUME));

        if (fcb) {
            NUTBLOCKIO *blkio;

            NUTASSERT(dev != NULL);

            blkio = (NUTBLOCKIO *) (dev->dev_dcb);
            NUTASSERT(blkio != NULL);

            fcb->vol_fsdev = fsdev;
            fcb->vol_blk_off = blkio->blkio_vol_bot;
            fcb->vol_blk_cnt = blkio->blkio_blk_cnt - blkio->blkio_vol_bot - blkio->blkio_vol_top;
            fcb->vol_blk_num = 0;
            fcb->vol_blk_len = blkio->blkio_blk_siz;
            fcb->vol_blk_buf = malloc(fcb->vol_blk_len);
            if (fcb->vol_blk_buf) {
                NUTFILE *nfp = malloc(sizeof(NUTFILE));

                if (nfp) {
                    FSCP_VOL_MOUNT mparm;

                    nfp->nf_next = NULL;
                    nfp->nf_dev = dev;
                    nfp->nf_fcb = fcb;
                    /* Mount the file system volume. */
                    mparm.fscp_bmnt = nfp;
                    mparm.fscp_part_type = 0;
                    if (fsdev->dev_ioctl(fsdev, FS_VOL_MOUNT, &mparm) == 0) {
                        /* Successful return. */
                        return nfp;
                    }
                    free(nfp);
                }
            }
            free(fcb);
        }
    }
    return NUTFILE_EOF;
}

/*!
 * \brief Unmount a currently mounted volume.
 *
 * \param nfp File pointer to a previously mounted volume.
 *
 * \return 0 on success, -1 otherwise.
 */
int NutBlockDeviceClose(NUTFILE * nfp)
{
    int rc = -1;
    BLOCKVOLUME *fcb;

    NUTASSERT(nfp != NULL);
    fcb = (BLOCKVOLUME *) nfp->nf_fcb;
    if (fcb) {
        NUTASSERT(fcb->vol_fsdev != NULL);
        NUTASSERT(fcb->vol_fsdev->dev_ioctl != NULL);
        rc = fcb->vol_fsdev->dev_ioctl(fcb->vol_fsdev, FS_VOL_UNMOUNT, NULL);
        free(fcb);
    }
    free(nfp);

    return rc;
}

/*!
 * \brief Perform block I/O device control functions.
 *
 * This function is called by the ioctl() function of the C runtime
 * library. Applications should not directly call this function.
 *
 * Unkown requests are passed to the block I/O device.
 *
 * \param dev  Identifies the device that receives the device-control
 *             function.
 * \param req  Requested control function. May be set to one of the
 *             following constants:
 *             - \ref NUTBLKDEV_INFO, conf points to a \ref BLKPAR_INFO structure.
 *             - \ref NUTBLKDEV_SEEK, conf points to a \ref BLKPAR_SEEK structure.
 *
 * \param conf Points to a buffer that contains any data required for
 *             the given control function or receives data from that
 *             function.
 *
 * \return 0 on success, -1 otherwise.
 */
int NutBlockDeviceIOCtl(NUTDEVICE * dev, int req, void *conf)
{
    int rc = 0;

    switch (req) {
    case NUTBLKDEV_SEEK:
        {
            BLKPAR_SEEK *par = (BLKPAR_SEEK *) conf;
            BLOCKVOLUME *fcb;

            /* Sanity check. */
            NUTASSERT(conf != NULL);
            NUTASSERT(par->par_nfp != NULL);
            NUTASSERT(par->par_nfp->nf_fcb != NULL);

            fcb = (BLOCKVOLUME *) par->par_nfp->nf_fcb;
            fcb->vol_blk_num = par->par_blknum;
        }
        break;
    case NUTBLKDEV_INFO:
        {
            BLKPAR_INFO *par;
            BLOCKVOLUME *fcb;

            NUTASSERT(conf != NULL);
            par = (BLKPAR_INFO *) conf;
            NUTASSERT(par->par_nfp != NULL);
            NUTASSERT(par->par_nfp->nf_fcb != NULL);
            fcb = (BLOCKVOLUME *) par->par_nfp->nf_fcb;

            par->par_nblks = fcb->vol_blk_cnt;
            par->par_blksz = fcb->vol_blk_len;
            par->par_blkbp = fcb->vol_blk_buf;
        }
        break;
    default:
        {
            NUTBLOCKIO *blkio;

            NUTASSERT(dev != NULL);
            NUTASSERT(dev->dev_dcb != NULL);
            blkio = (NUTBLOCKIO *) (dev->dev_dcb);

            NUTASSERT(blkio->blkio_ioctl != NULL);
            rc = blkio->blkio_ioctl(dev, req, conf);
        }
        break;
    }
    return rc;
}

/*!
 * \brief Read data blocks from a mounted volume.
 *
 * The current position may have been set by a previous I/O control
 * \ref NUTBLKDEV_SEEK.
 *
 * Applications should not call this function directly, but use the
 * stdio interface.
 *
 * \param nfp    File pointer to a previously mounted volume.
 * \param buffer Pointer to the data buffer to fill.
 * \param num    Number of blocks to read.
 *
 * \return The number of blocks actually read. The current position is 
 *         set to the next unread block. A return value of -1 indicates 
 *         an error.
 */
int NutBlockDeviceRead(NUTFILE * nfp, void *buffer, int num)
{
    int rc;
    int cnt;
    uint8_t *bp = buffer;
    NUTBLOCKIO *blkio;
    BLOCKVOLUME *fcb;

    /* Sanity checks. */
    if (num == 0) {
        return 0;
    }
    NUTASSERT(buffer != NULL);
    NUTASSERT(nfp != NULL);
    NUTASSERT(nfp->nf_fcb != NULL);
    NUTASSERT(nfp->nf_dev != NULL);
    NUTASSERT(nfp->nf_dev->dev_dcb != NULL);

    blkio = nfp->nf_dev->dev_dcb;
    fcb = (BLOCKVOLUME *) nfp->nf_fcb;
    NUTASSERT(blkio->blkio_read != NULL);

    for (rc = 0; rc < num; rc++) {
        if (fcb->vol_blk_num >= fcb->vol_blk_cnt) {
            break;
        }
        cnt = (*blkio->blkio_read) (nfp->nf_dev, fcb->vol_blk_num + fcb->vol_blk_off, bp, fcb->vol_blk_len);
        if (cnt != fcb->vol_blk_len) {
            break;
        }
        fcb->vol_blk_num++;
        bp += fcb->vol_blk_len;
    }
    return rc ? rc : -1;
}

/*!
 * \brief Write data blocks to a mounted volume.
 *
 * The current position may have been set by a previous I/O control
 * \ref NUTBLKDEV_SEEK.
 *
 * Applications should not call this function directly, but use the
 * stdio interface.
 *
 * \param nfp    File pointer to a previously mounted volume.
 * \param buffer Pointer to the data to be written.
 * \param num    Number of blocks to write.
 *
 * \return The number of blocks actually written. The current position is 
 *         set to the next block. A return value of -1 indicates an error.
 */
int NutBlockDeviceWrite(NUTFILE * nfp, CONST void *buffer, int num)
{
    int rc;
    int cnt;
    CONST uint8_t *bp = buffer;
    NUTBLOCKIO *blkio;
    BLOCKVOLUME *fcb;

    /* Sanity checks. */
    if (num == 0) {
        return 0;
    }
    NUTASSERT(nfp != NULL);
    NUTASSERT(nfp->nf_fcb != NULL);
    NUTASSERT(nfp->nf_dev != NULL);
    NUTASSERT(nfp->nf_dev->dev_dcb != NULL);

    fcb = (BLOCKVOLUME *) nfp->nf_fcb;
    blkio = nfp->nf_dev->dev_dcb;
    NUTASSERT(blkio->blkio_write != NULL);

    for (rc = 0; rc < num; rc++) {
        if (fcb->vol_blk_num >= fcb->vol_blk_cnt) {
            break;
        }
        cnt = (*blkio->blkio_write) (nfp->nf_dev, fcb->vol_blk_num + fcb->vol_blk_off, bp, fcb->vol_blk_len);
        if (cnt != fcb->vol_blk_len) {
            break;
        }
        fcb->vol_blk_num++;
        bp += fcb->vol_blk_len;
    }
    return rc ? rc : -1;
}

#ifdef __HARVARD_ARCH__
/*! 
 * \brief Write data blocks from program space to a mounted volume.
 *
 * Similar to NutBlockDeviceWrite() except that the data is located in 
 * program memory.
 *
 * Applications should not call this function directly, but use the
 * stdio interface.
 *
 * \param nfp    File pointer to a previously mounted volume.
 * \param buffer Pointer to the data bytes in program space to be written.
 * \param num    Number of blocks to write.
 *
 * \return The number of blocks actually written. The current position is 
 *         set to the next block. A return value of -1 indicates an error.
 */
int NutBlockDeviceWrite_P(NUTFILE * nfp, PGM_P buffer, int num)
{
    int rc = 0;
    int cnt;
    PGM_P bp = buffer;
    NUTBLOCKIO *blkio;
    BLOCKVOLUME *fcb;

    /* Sanity checks. */
    if (num == 0) {
        return 0;
    }
    NUTASSERT(nfp != NULL);
    NUTASSERT(nfp->nf_fcb != NULL);
    NUTASSERT(nfp->nf_dev != NULL);
    NUTASSERT(nfp->nf_dev->dev_dcb != NULL);

    fcb = (BLOCKVOLUME *) nfp->nf_fcb;
    blkio = nfp->nf_dev->dev_dcb;
    NUTASSERT(blkio->blkio_write != NULL);

    for (rc = 0; rc < num; rc++) {
        if (fcb->vol_blk_num >= fcb->vol_blk_cnt) {
            break;
        }
        cnt = (*blkio->blkio_write_P) (nfp->nf_dev, fcb->vol_blk_num + fcb->vol_blk_off, bp, fcb->vol_blk_len);
        if (cnt != fcb->vol_blk_len) {
            break;
        }
        fcb->vol_blk_num++;
        bp += fcb->vol_blk_len;
    }
    return rc ? rc : -1;
}
#endif

/*!
 * \brief Return the size of a mounted volume.
 *
 * \param nfp File pointer to a previously mounted volume.
 *
 * \return Total number of bytes.
 */
long NutBlockDeviceSize(NUTFILE * nfp)
{
    BLOCKVOLUME *fcb;

    NUTASSERT(nfp != NULL);
    fcb = (BLOCKVOLUME *) nfp->nf_fcb;
    NUTASSERT(fcb != NULL);

    return (long) fcb->vol_blk_cnt * (long) fcb->vol_blk_len;
}
