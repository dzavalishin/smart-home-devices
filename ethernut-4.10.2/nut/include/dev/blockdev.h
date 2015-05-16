#ifndef DEV_BLOCKDEV_H_
#define DEV_BLOCKDEV_H_

/*
 * Copyright (C) 2005 by egnite Software GmbH. All rights reserved.
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
 */

/*!
 * \file dev/blockdev.h
 * \brief Block device driver definitions.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.4  2009/01/09 17:59:05  haraldkipp
 * Added target independent AT45D block device drivers and non-volatile
 * memory support based on the new bus controllers.
 *
 * Revision 1.3  2008/08/11 06:59:59  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.2  2006/04/07 12:57:48  haraldkipp
 * Added ioctl(NUTBLKDEV_MEDIAAVAIL).
 *
 * Revision 1.1  2006/01/05 16:32:05  haraldkipp
 * First check-in.
 *
 *
 * \endverbatim
 */

#include <sys/types.h>
#include <stdint.h>
#include <sys/device.h>
#include <sys/file.h>

/*!
 * \addtogroup xgBlockDev
 */
/*@{*/

/*!
 * \name Control Codes
 */
/*@{*/

/*! \brief Query for media change. */
#define NUTBLKDEV_MEDIAAVAIL   0x1200
/*! \brief Query for media change. */
#define NUTBLKDEV_MEDIACHANGE  0x1201
/*! \brief Retrieve device information. */
#define NUTBLKDEV_INFO         0x1202
/*! \brief Block seek request. */
#define NUTBLKDEV_SEEK         0x1203

/*@}*/

/*!
 * \brief Block seek parameter structure.
 *
 * Used with \ref NUTBLKDEV_SEEK ioctl.
 */
typedef struct _BLKPAR_SEEK {
    NUTFILE *par_nfp;
    uint32_t par_blknum;
} BLKPAR_SEEK;

/*!
 * \brief Device information parameter structure.
 *
 * Used with \ref NUTBLKDEV_INFO ioctl.
 */
typedef struct _BLKPAR_INFO {
    NUTFILE *par_nfp;
    uint32_t par_nblks;
    uint32_t par_blksz;
    uint8_t *par_blkbp;
} BLKPAR_INFO;

/*! \brief Generic block I/O device interface structure type. */
typedef struct _NUTBLOCKIO NUTBLOCKIO;

/*!
 * \brief Generic block I/O device interface structure.
 *
 * This is a virtual structure, used by the generic block I/O device driver. 
 *
 * \note Any real implementation must start with the same layout. If this
 *       structure is changed, we must update all implementations.
 */
struct _NUTBLOCKIO {
    /*!
     * \brief Device specific information.
     */
    void *blkio_info;

    /*!
     * \brief Total number of blocks on this device.
     */
    uint32_t blkio_blk_cnt;

    /*!
     * \brief Number of bytes per block.
     */
    uint32_t blkio_blk_siz;

    /*!
     * \brief First block for file system mount.
     */
    uint32_t blkio_vol_bot;

    /*!
     * \brief Number of blocks reserved on top of file system mount.
     */
    uint32_t blkio_vol_top;
    
    /*! 
     * \brief Read from block I/O device, starting at the specified block.
     */
    int (*blkio_read) (NUTDEVICE *, uint32_t, void *, int);

    /*! 
     * \brief Write to block I/O device, starting at the specified block.
     */
    int (*blkio_write) (NUTDEVICE *, uint32_t, CONST void *, int);

#ifdef __HARVARD_ARCH__
    /*! 
     * \brief Write program memory to block I/O device, starting at the specified block.
     */
    int (*blkio_write_P) (NUTDEVICE *, uint32_t, PGM_P, int);
#endif

    /*! 
     * \brief Control functions.
     */
    int (*blkio_ioctl)(NUTDEVICE *, int, void *);
};

/*@}*/

__BEGIN_DECLS
/* Prototypes */
extern int NutBlockDeviceInit(NUTDEVICE * dev);
extern NUTFILE *NutBlockDeviceOpen(NUTDEVICE * dev, CONST char *name, int mode, int acc);
extern int NutBlockDeviceClose(NUTFILE * nfp);
extern int NutBlockDeviceIOCtl(NUTDEVICE * dev, int req, void *conf);
extern int NutBlockDeviceRead(NUTFILE * nfp, void *buffer, int num);
extern int NutBlockDeviceWrite(NUTFILE *nfp, CONST void *buffer, int num);
extern long NutBlockDeviceSize(NUTFILE *nfp);
#ifdef __HARVARD_ARCH__
extern int NutBlockDeviceWrite_P(NUTFILE * nfp, PGM_P buffer, int num);
#endif
/* Prototypes */
__END_DECLS

#endif
