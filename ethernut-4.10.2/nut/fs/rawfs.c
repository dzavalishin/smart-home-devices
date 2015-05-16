/*
 * Copyright (C) 2008-2009 by egnite GmbH
 * Copyright (C) 2005-2006 by egnite Software GmbH
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
 * \file fs/rawfs.c
 * \brief Raw device file system.
 *
 * This file system provides a single file entry only, which is mapped to
 * the entire volume. This implies, that there is no file name and that
 * the size of the file is fixed.
 *
 * \verbatim
 * $Id: rawfs.c 2909 2010-03-18 12:59:56Z haraldkipp $
 * \endverbatim
 */

#include <dev/blockdev.h>
#include <sys/event.h>
#include <sys/nutdebug.h>

#include <fs/fs.h>

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <memdebug.h>

#include <fs/rawfs.h>

#if 0
/* Use for local debugging. */
#define NUTDEBUG
#include <stdio.h>
#endif

#ifndef SEEK_SET
#  define SEEK_SET        0     /* Seek from beginning of file.  */
#  define SEEK_CUR        1     /* Seek from current position.  */
#  define SEEK_END        2     /* Set file pointer to EOF plus "offset" */
#endif

/*!
 * \addtogroup xgRawFs
 */
/*@{*/

/*!
 * \brief Flush sector buffer.
 *
 * The volume must be locked when calling this function.
 *
 * \param dev Specifies the file system device.
 *
 * \return 0 on success, -1 on failures.
 */
static int RawFsSectorFlush(NUTDEVICE * dev)
{
    int rc = 0;
    RAWVOLUME *vol = (RAWVOLUME *) dev->dev_dcb;

    /* Write dirty sectors only. */
    if (vol->vol_sect_dirty) {
        BLKPAR_SEEK pars;
        NUTFILE *blkmnt = dev->dev_icb;
        NUTDEVICE *blkdev = blkmnt->nf_dev;

        /* Set the block device's sector position. */
        pars.par_nfp = blkmnt;
        pars.par_blknum = vol->vol_sect_num;
        rc = (*blkdev->dev_ioctl) (blkdev, NUTBLKDEV_SEEK, &pars);
        if (rc == 0) {
            /* Write single block to the device. */
            if ((*blkdev->dev_write) (blkmnt, vol->vol_sect_buf, 1) == 1) {
                /* Success, buffer is clean. */
                vol->vol_sect_dirty = 0;
            } else {
                rc = -1;
            }
        }
    }
    return rc;
}

/*!
 * \brief Load sector.
 *
 * \param dev  Specifies the file system device.
 * \param sect Sector to load.
 *
 * \return 0 on success, -1 on failures.
 */
static int RawFsSectorLoad(NUTDEVICE * dev, uint32_t sect)
{
    int rc = -1;
    RAWVOLUME *vol;

    NUTASSERT(dev != NULL);
    vol = (RAWVOLUME *) dev->dev_dcb;

    /* Gain mutex access. */
    NUTASSERT(vol != NULL);
    NutEventWait(&vol->vol_iomutex, 0);

    /* Nothing to do if sector is already loaded. */
    if (vol->vol_sect_num == sect) {
        rc = 0;
    }
    /* Make sure that the sector buffer is clean. */
    else if (RawFsSectorFlush(dev) == 0) {
        NUTFILE *blkmnt = dev->dev_icb;
        NUTDEVICE *blkdev = blkmnt->nf_dev;
        BLKPAR_SEEK pars;

        blkmnt = dev->dev_icb;
        NUTASSERT(blkmnt != NULL);
        blkdev = blkmnt->nf_dev;
        NUTASSERT(blkdev != NULL);

        /* Set the block device's sector position. */
        pars.par_nfp = blkmnt;
        pars.par_blknum = sect;
        if ((*blkdev->dev_ioctl) (blkdev, NUTBLKDEV_SEEK, &pars) == 0) {
            /* Read a single block from the device. */
            if ((*blkdev->dev_read) (blkmnt, vol->vol_sect_buf, 1) == 1) {
                vol->vol_sect_num = sect;
                rc = 0;
            }
        }
    }

    /* Release mutex access. */
    NutEventPostAsync(&vol->vol_iomutex);

    return rc;
}

/*!
 * \brief Flush file buffers.
 *
 * \param nfp Specifies the file.
 *
 * \return 0 on success, -1 otherwise.
 */
static int RawFsFileFlush(NUTFILE * nfp)
{
    int rc;
    NUTDEVICE *dev;
    RAWVOLUME *vol;

    NUTASSERT(nfp != NULL);
    dev = nfp->nf_dev;
    NUTASSERT(dev != NULL);
    vol = (RAWVOLUME *) dev->dev_dcb;
    NUTASSERT(vol != NULL);

    /* Gain mutex access. */
    NutEventWait(&vol->vol_iomutex, 0);
    /* Flush sector buffer. */
    rc = RawFsSectorFlush(nfp->nf_dev);
    /* Release mutex access. */
    NutEventPost(&vol->vol_iomutex);

    return rc;
}

/*!
 * \brief Open a raw file.
 *
 * This function is called by the low level open routine of the C runtime 
 * library, using the _NUTDEVICE::dev_open entry.
 *
 * \param dev  Specifies the file system device.
 * \param path Pathname of the file to open which is ignored here, because 
 *             the raw file system doesn't support multiple file entries.
 * \param mode Operation mode.
 * \param acc  File attribute, ignored.
 *
 * \return Pointer to a NUTFILE structure if successful or NUTFILE_EOF otherwise.
 *
 * \bug Append mode not working as expected.
 */
NUTFILE *RawFsFileOpen(NUTDEVICE * dev, CONST char *path, int mode, int acc)
{
    NUTFILE *nfp;
    RAWFILE *fcb;

    /* Allocate a private file control block. */
    fcb = malloc(sizeof(RAWFILE));
    if (fcb) {
        memset(fcb, 0, sizeof(RAWFILE));
        fcb->f_mode = mode;
        /* Allocate a file information structure. */
        nfp = malloc(sizeof(NUTFILE));
        if (nfp) {
            nfp->nf_next = 0;
            nfp->nf_dev = dev;
            nfp->nf_fcb = fcb;
            /* Successfully opened. */
            return nfp;
        }
        free(fcb);
    }
    /* Return failure. */
    return NUTFILE_EOF;
}

/*!
 * \brief Close a raw file.
 *
 * \param nfp Pointer to a \ref NUTFILE structure, obtained by a previous 
 *            call to RawFsFileOpen().
 *
 * \return 0 on success, -1 otherwise.
 */
int RawFsFileClose(NUTFILE * nfp)
{
    int rc;

    NUTASSERT(nfp != NULL);

    rc = RawFsFileFlush(nfp);
    if (nfp->nf_fcb) {
        free(nfp->nf_fcb);
    }
    free(nfp);

    return rc;
}

/*!
 * \brief Write data to a file.
 *
 * \param nfp    Pointer to a \ref NUTFILE structure, obtained by a previous 
 *               call to RawFsFileOpen().
 * \param buffer Pointer to the data to be written. If zero, then the
 *               output buffer will be flushed.
 * \param len    Number of bytes to write.
 *
 * \return The number of bytes written. A return value of -1 indicates an 
 *         error.
 */
int RawFsFileWrite(NUTFILE * nfp, CONST void *buffer, int len)
{
    int rc;
    int step;
    uint8_t *buf;
    RAWFILE *fcb;
    RAWVOLUME *vol;

    NUTASSERT(nfp != NULL);

    /* Flush file if buffer is a NULL pointer. */
    if (buffer == NULL || len == 0) {
        return RawFsFileFlush(nfp);
    }

    /* Sanity check. */
    NUTASSERT(nfp->nf_fcb != NULL);
    NUTASSERT(nfp->nf_dev != NULL);
    NUTASSERT(nfp->nf_dev->dev_dcb != NULL);

    fcb = (RAWFILE *) nfp->nf_fcb;
    vol = (RAWVOLUME *) nfp->nf_dev->dev_dcb;

    /*
     * Write the data.
     */
    buf = (uint8_t *) buffer;
    for (rc = 0, step = 0; rc < len; rc += step) {
        /* Did we reach the end of a sector? */
        if (fcb->f_sect_pos >= vol->vol_sect_len) {
            /* Yes, move to the next sector. */
            fcb->f_sect_num++;
            fcb->f_sect_pos -= vol->vol_sect_len;
        }
        /* Load the sector we want to write to. */
        if (RawFsSectorLoad(nfp->nf_dev, fcb->f_sect_num)) {
            rc = -1;
            break;
        }
        /* The number of bytes we write to this sector. */
        step = (int) (vol->vol_sect_len - fcb->f_sect_pos);
        if (step > len - rc) {
            step = len - rc;
        }
        /* Copy data to this sector. */
        memcpy(&vol->vol_sect_buf[fcb->f_sect_pos], &buf[rc], step);
        vol->vol_sect_dirty = 1;
        /* Advance file pointers. */
        fcb->f_pos += step;
        fcb->f_sect_pos += step;
    }
    return rc;
}

#ifdef __HARVARD_ARCH__
/*! 
 * \brief Write data from program space to a file.
 *
 * This function is not yet implemented and will always return -1.
 *
 * Similar to RawFsFileWrite() except that the data is located in 
 * program memory.
 *
 * \param nfp    Pointer to a \ref NUTFILE structure, obtained by a previous 
 *               call to RawFsFileOpen().
 * \param buffer Pointer to the data in program space. If zero, then the
 *               output buffer will be flushed.
 * \param len    Number of bytes to write.
 *
 * \return The number of bytes written. A return value of -1 indicates an 
 *         error.
 */
int RawFsFileWrite_P(NUTFILE * nfp, PGM_P buffer, int len)
{
    return -1;
}
#endif

/*!
 * \brief Read data from a file.
 *
 * \param nfp    Pointer to a \ref NUTFILE structure, obtained by a previous 
 *               call to RawFsFileOpen().
 * \param buffer Pointer to the data buffer to fill.
 * \param size   Maximum number of bytes to read.
 *
 * \return The number of bytes actually read. A return value of -1 indicates 
 *         an error.
 */
int RawFsFileRead(NUTFILE * nfp, void *buffer, int size)
{
    int rc;
    int step;
    uint8_t *buf;
    RAWVOLUME *vol;
    RAWFILE *fcb;

    /* Ignore input flush. */
    if (buffer == NULL || size == 0) {
        return 0;
    }

    NUTASSERT(nfp != NULL);
    NUTASSERT(nfp->nf_dev != NULL);
    fcb = nfp->nf_fcb;
    NUTASSERT(fcb != NULL);
    vol = (RAWVOLUME *) nfp->nf_dev->dev_dcb;
    NUTASSERT(vol != NULL);

    buf = (uint8_t *) buffer;
    for (rc = 0, step = 0; rc < size; rc += step) {
        /* Did we reach the end of a sector? */
        if (fcb->f_sect_pos >= vol->vol_sect_len) {
            /* Yes, move to the next sector. */
            fcb->f_sect_num++;
            fcb->f_sect_pos -= vol->vol_sect_len;
        }
        /* Make sure that the required sector is loaded. */
        if (RawFsSectorLoad(nfp->nf_dev, fcb->f_sect_num)) {
            rc = -1;
            break;
        }
        step = (int) (vol->vol_sect_len - fcb->f_sect_pos);
        if (step > size - rc) {
            step = size - rc;
        }
        memcpy(&buf[rc], &vol->vol_sect_buf[fcb->f_sect_pos], step);
        fcb->f_pos += step;
        fcb->f_sect_pos += step;
    }
    return rc;
}

/*!
 * \brief Retrieve the size of a previously opened file.
 *
 * This function is called by the low level size routine of the C runtime 
 * library, using the _NUTDEVICE::dev_size entry.
 *
 * \param nfp Pointer to a \ref _NUTFILE structure, obtained by a 
 *            previous call to RawFsFileOpen().
 *
 * \return Size of the file.
 */
static long RawFsFileSize(NUTFILE * nfp)
{
    RAWVOLUME *vol;

    NUTASSERT(nfp != NULL);
    NUTASSERT(nfp->nf_dev != NULL);
    vol = (RAWVOLUME *) nfp->nf_dev->dev_dcb;

    return vol->vol_sect_cnt * vol->vol_sect_len;
}

/*!
 * \brief Move file pointer to a specified position.
 *
 * Moving beyond the current file size is not supported.
 *
 * \param nfp    File descriptor.
 * \param pos    Requested file position.
 * \param whence Positioning directive.
 *
 * \return 0 on success, -1 otherwise. In the latter case the position
 *         is unspecified.
 */
static int RawFsFileSeek(NUTFILE * nfp, long *pos, int whence)
{
    int rc = 0;
    long npos;
    RAWFILE *fcb;

    NUTASSERT(nfp != NULL);
    NUTASSERT(nfp->nf_fcb != NULL);
    fcb = nfp->nf_fcb;

    NUTASSERT(pos != NULL);
    npos = *pos;

    switch (whence) {
    case SEEK_CUR:
        /* Relative to current position. */
        npos += fcb->f_pos;
        break;
    case SEEK_END:
        /* Relative to file end. */
        npos += RawFsFileSize(nfp);
        break;
    }

    /* Make sure that we are within limits. */
    if (npos < 0 || npos > RawFsFileSize(nfp)) {
        errno = EINVAL;
        rc = -1;
    } else {
        RAWVOLUME *vol = (RAWVOLUME *) nfp->nf_dev->dev_dcb;

        NUTASSERT(nfp != NULL);
        NUTASSERT(nfp != NULL);
        vol = (RAWVOLUME *) nfp->nf_dev->dev_dcb;

        *pos = npos;
        fcb->f_pos = npos;
        fcb->f_sect_num = 0;
        while (npos >= (long)vol->vol_sect_len) {
            fcb->f_sect_num++;
            npos -= vol->vol_sect_len;
        }
        fcb->f_sect_pos = npos;
    }
    return rc;
}

/*!
 * \brief Unmount a raw volume.
 *
 * This routine is called by the block device driver while unmounting a
 * partition.
 *
 * \param dev Specifies the file system device.
 *
 * \return 0 on success or -1 in case of an error.
 */
int RawFsUnmount(NUTDEVICE * dev)
{
    RAWVOLUME *vol;

    NUTASSERT(dev != NULL);
    vol = (RAWVOLUME *) dev->dev_dcb;

    /* Flush sector buffer. */
    RawFsSectorFlush(dev);
    /* Release the volume information structure  */
    if (vol) {
        free(vol);
    }
    return 0;
}

/*!
 * \brief Mount a raw volume.
 *
 * This routine is called by the block device driver while mounting a
 * partition.
 *
 * The routine may also initializes any caching mechanism. Thus, it must
 * be called before any other read or write access.
 *
 * \param dev       Specifies the file system device.
 * \param blkmnt    Handle of the block device's partition mount.
 * \param part_type Partition type, ignored.
 *
 * \return 0 on success or -1 in case of an error.
 */
int RawFsMount(NUTDEVICE * dev, NUTFILE * blkmnt, uint8_t part_type)
{
    BLKPAR_INFO pari;
    RAWVOLUME *vol;

    NUTASSERT(blkmnt != NULL);
    NUTASSERT(blkmnt->nf_dev != NULL);
    NUTASSERT(dev != NULL);

    /* Allocate the volume information structure  */
    if ((dev->dev_dcb = malloc(sizeof(RAWVOLUME))) == 0) {
        return -1;
    }
    vol = (RAWVOLUME *) memset(dev->dev_dcb, 0, sizeof(RAWVOLUME));

    /*
     * Query information from the block device driver.
     */
    pari.par_nfp = blkmnt;
    if ((*blkmnt->nf_dev->dev_ioctl) (blkmnt->nf_dev, NUTBLKDEV_INFO, &pari)) {
        free(vol);
        errno = ENODEV;
        return -1;
    }
    vol->vol_sect_num = pari.par_nblks;
    vol->vol_sect_cnt = pari.par_nblks;
    vol->vol_sect_len = (size_t) pari.par_blksz;
    vol->vol_sect_buf = pari.par_blkbp;

    /* Initialize mutual exclusion semaphores. */
    NutEventPost(&vol->vol_fsmutex);
    NutEventPost(&vol->vol_iomutex);

    dev->dev_icb = blkmnt;

    return 0;
}

/*!
 * \brief File system specific functions.
 *
 * \param dev  Identifies the file system device that receives the
 *             control function.
 * \param req  Requested control function. May be set to one of the
 *             following constants:
 *             - FS_FILE_SEEK
 *             - FS_VOL_MOUNT, conf points to an FSCP_VOL_MOUNT structure.
 *             - FS_VOL_UNMOUNT, conf should be a NULL pointer.
 * \param conf Points to a buffer that contains any data required for
 *             the given control function or receives data from that
 *             function.
 *
 * \return 0 on success, -1 otherwise.
 */
static int RawFsIOCtl(NUTDEVICE * dev, int req, void *conf)
{
    int rc = -1;

    switch (req) {
    case FS_FILE_SEEK:
        NUTASSERT(conf != NULL);
        RawFsFileSeek((NUTFILE *) ((IOCTL_ARG3 *) conf)->arg1,  /* */
                      (long *) ((IOCTL_ARG3 *) conf)->arg2,     /* */
                      (int) ((IOCTL_ARG3 *) conf)->arg3);
        break;
    case FS_VOL_MOUNT:
        {
            /* Mount a volume. */
            FSCP_VOL_MOUNT *par = (FSCP_VOL_MOUNT *) conf;

            NUTASSERT(par != NULL);
            NUTASSERT(dev != NULL);
            rc = RawFsMount(dev, par->fscp_bmnt, par->fscp_part_type);
            if (rc) {
                /* Release resources on failures. */
                RawFsUnmount(dev);
            }
        }
        break;
    case FS_VOL_UNMOUNT:
        /* Unmount a volume. */
        NUTASSERT(dev != NULL);
        rc = RawFsUnmount(dev);
        break;
    }
    return rc;
}

/*!
 * \brief Initialize the raw device file system driver.
 *
 * This routine is called during device registration.
 *
 * \param dev Specifies the file system device.
 *
 * \return Zero on success. Otherwise an error code is returned.
 */
static int RawFsInit(NUTDEVICE * dev)
{
    /* Nothing to do. */
    return 0;
}

/*!
 * \brief Reentrant variant of RawFsFileOpen().
 */
static NUTFILE *RawFsApiFileOpen(NUTDEVICE * dev, CONST char *path, int mode, int acc)
{
    NUTFILE *rc;
    RAWVOLUME *vol;

    NUTASSERT(dev != NULL);
    vol = (RAWVOLUME *) dev->dev_dcb;
    NUTASSERT(vol != NULL);

    /* Lock filesystem access. */
    NutEventWait(&vol->vol_fsmutex, 0);
    /* Call worker routine. */
    rc = RawFsFileOpen(dev, path, mode, acc);
    /* Release filesystem lock. */
    NutEventPost(&vol->vol_fsmutex);

    return rc;
}

/*!
 * \brief Reentrant variant of RawFsFileClose().
 */
static int RawFsApiFileClose(NUTFILE * nfp)
{
    int rc;
    RAWVOLUME *vol;

    NUTASSERT(nfp != NULL);
    NUTASSERT(nfp->nf_dev != NULL);
    vol = (RAWVOLUME *) nfp->nf_dev->dev_dcb;
    NUTASSERT(vol != NULL);

    /* Lock filesystem access. */
    NutEventWait(&vol->vol_fsmutex, 0);
    /* Call worker routine. */
    rc = RawFsFileClose(nfp);
    /* Release filesystem lock. */
    NutEventPost(&vol->vol_fsmutex);

    return rc;
}

/*!
 * \brief Reentrant variant of RawFsFileWrite().
 */
static int RawFsApiFileWrite(NUTFILE * nfp, CONST void *buffer, int len)
{
    int rc;
    RAWVOLUME *vol;

    NUTASSERT(nfp != NULL);
    NUTASSERT(nfp->nf_dev != NULL);
    vol = (RAWVOLUME *) nfp->nf_dev->dev_dcb;
    NUTASSERT(vol != NULL);

    /* Lock filesystem access. */
    NutEventWait(&vol->vol_fsmutex, 0);
    /* Call worker routine. */
    rc = RawFsFileWrite(nfp, buffer, len);
    /* Release filesystem lock. */
    NutEventPost(&vol->vol_fsmutex);

    return rc;
}

#ifdef __HARVARD_ARCH__
/*!
 * \brief Reentrant variant of RawFsFileWrite_P().
 */
static int RawFsApiFileWrite_P(NUTFILE * nfp, PGM_P buffer, int len)
{
    int rc;
    RAWVOLUME *vol;

    NUTASSERT(nfp != NULL);
    NUTASSERT(nfp->nf_dev != NULL);
    vol = (RAWVOLUME *) nfp->nf_dev->dev_dcb;
    NUTASSERT(vol != NULL);

    /* Lock filesystem access. */
    NutEventWait(&vol->vol_fsmutex, 0);
    /* Call worker routine. */
    rc = RawFsFileWrite_P(nfp, buffer, len);
    /* Release filesystem lock. */
    NutEventPost(&vol->vol_fsmutex);

    return rc;
}
#endif

/*!
 * \brief Reentrant variant of RawFsFileRead().
 */
static int RawFsApiFileRead(NUTFILE * nfp, void *buffer, int size)
{
    int rc;
    RAWVOLUME *vol;

    NUTASSERT(nfp != NULL);
    NUTASSERT(nfp->nf_dev != NULL);
    vol = (RAWVOLUME *) nfp->nf_dev->dev_dcb;
    NUTASSERT(vol != NULL);

    /* Lock filesystem access. */
    NutEventWait(&vol->vol_fsmutex, 0);
    /* Call worker routine. */
    rc = RawFsFileRead(nfp, buffer, size);
    /* Release filesystem lock. */
    NutEventPost(&vol->vol_fsmutex);

    return rc;
}

/*!
 * \brief Reentrant variant of RawFsIOCtl().
 */
static int RawFsApiIOCtl(NUTDEVICE * dev, int req, void *conf)
{
    int rc;
    RAWVOLUME *vol;

    NUTASSERT(dev != NULL);
    vol = (RAWVOLUME *) dev->dev_dcb;

    /* Lock filesystem access. */
    if (req != FS_VOL_MOUNT && vol) {
        NutEventWait(&vol->vol_fsmutex, 0);
    }
    /* Call worker routine. */
    rc = RawFsIOCtl(dev, req, conf);
    /* Release filesystem lock. */
    if (req != FS_VOL_MOUNT && req != FS_VOL_UNMOUNT && vol) {
        NutEventPost(&vol->vol_fsmutex);
    }
    return rc;
}

/*!
 * \brief Raw device file system driver information structure.
 *
 * A pointer to this structure must be passed to NutRegisterDevice() 
 * to bind this file system driver to the Nut/OS kernel.
 * An application may then call
 * /verbatim
 * _open("AT45D0:0/RAWFS0", _O_RDWR | _O_BINARY);
 * /endverbatim
 * to mount the file system on the previously registered block
 * device (devSpiAt45d0 in this example).
 */
NUTDEVICE devRawFs0 = {
    0,                      /*!< Pointer to next device, dev_next. */
    {'R', 'A', 'W', 'F', 'S', '0', 0, 0, 0}
    ,                       /*!< Unique device name, dev_name. */
    IFTYP_FS,               /*!< Type of device, dev_type. Obsolete. */
    0,                      /*!< Base address, dev_base. Unused. */
    0,                      /*!< First interrupt number, dev_irq. Unused. */
    0,                      /*!< Mounted block device partition, dev_icb. */
    0,                      /*!< Volume information, dev_dcb. */
    RawFsInit,              /*!< Driver initialization routine, dev_init. */
    RawFsApiIOCtl,          /*!< Driver specific control function, dev_ioctl. */
    RawFsApiFileRead,       /*!< Read data from a file, dev_read. */
    RawFsApiFileWrite,      /*!< Write data to a file, dev_write. */
#ifdef __HARVARD_ARCH__
    RawFsApiFileWrite_P,    /*!< Write data from program space to a file, dev_write_P. */
#endif
    RawFsApiFileOpen,       /*!< Open a file, dev_open. */
    RawFsApiFileClose,      /*!< Close a file, dev_close. */
    RawFsFileSize           /*!< Return file size, dev_size. */
};

/*@}*/
