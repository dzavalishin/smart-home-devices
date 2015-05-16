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
 */

/*!
 * \file fs/phatvol.c
 * \brief Volume related routines of the PHAT file system.
 *
 * When mounting a partition, we expect the block device driver to call
 * the related mount routine of the file system driver.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.11  2009/02/13 14:52:05  haraldkipp
 * Include memdebug.h for heap management debugging support.
 *
 * Revision 1.10  2008/08/11 06:59:42  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.9  2006/10/08 16:42:56  haraldkipp
 * Not optimal, but simple and reliable exclusive access implemented.
 * Fixes bug #1486539. Furthermore, bug #1567790, which had been rejected,
 * had been reported correctly and is now fixed.
 *
 * Revision 1.8  2006/07/11 12:20:19  haraldkipp
 * PHAT file system failed when accessed from multiple threads. A mutual
 * exclusion semaphore fixes this.
 *
 * Revision 1.7  2006/07/10 08:48:47  haraldkipp
 * Automatically detect FAT12 and FAT16 volumes when no partition table
 * is provided.
 *
 * Revision 1.6  2006/06/18 16:40:34  haraldkipp
 * No need to set errno after malloc failed.
 *
 * Revision 1.5  2006/05/15 11:49:47  haraldkipp
 * Added support for media formats without partition table like USB sticks.
 *
 * Revision 1.4  2006/04/07 12:56:18  haraldkipp
 * Several memory holes fixed.
 *
 * Revision 1.3  2006/02/23 15:45:22  haraldkipp
 * PHAT file system now supports configurable number of sector buffers.
 * This dramatically increased write rates of no-name cards.
 * AVR compile errors corrected.
 *
 * Revision 1.2  2006/01/22 17:38:06  haraldkipp
 * If mounting fails, the occupied resources are no longer released in
 * PhatVolMount(). Instead the caller, PhatIOCtl() in this case, calls
 * PhatVolUnmount(). This reduces the code size and makes sure, the all
 * resources are released in all cases.
 *
 * Revision 1.1  2006/01/05 16:31:56  haraldkipp
 * First check-in.
 *
 *
 * \endverbatim
 */

#include <dev/blockdev.h>

#include <sys/event.h>

#include <fs/dospart.h>
#include <fs/phatio.h>
#include <fs/phatutil.h>
#include <fs/phatvol.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <memdebug.h>


#if 0
/* Use for local debugging. */
#define NUTDEBUG
#include <stdio.h>
#include <fs/phatdbg.h>
#endif

/*!
 * \addtogroup xgPhatVol
 */
/*@{*/

/*!
 * \brief Counts the number of free clusters in a volume.
 *
 * \param dev Specifies the file system device.
 *
 * \return The number of free clusters.
 */
static uint32_t PhatCountFreeClusters(NUTDEVICE * dev)
{
    uint32_t rc = 0;
    uint32_t i = 2;
    uint32_t link;
    PHATVOL *vol = (PHATVOL *) dev->dev_dcb;

    if (vol->vol_type == 32) {
        while (i < vol->vol_last_clust) {
            if (Phat32GetClusterLink(dev, i, &link)) {
                break;
            }
            if (link == 0) {
                rc++;
            }
            i++;
        }
    } else if (vol->vol_type == 16) {
        while (i < vol->vol_last_clust) {
            if (Phat16GetClusterLink(dev, i, &link)) {
                break;
            }
            if (link == 0) {
                rc++;
            }
            i++;
        }
    } else {
        while (i < vol->vol_last_clust) {
            if (Phat12GetClusterLink(dev, i, &link)) {
                break;
            }
            if (link == 0) {
                rc++;
            }
            i++;
        }
    }
    return rc;
}

/*!
 * \brief Mount a PHAT volume.
 *
 * This routine is called by the block device driver while mounting a
 * partition. It reads and verifies the volume boot record, which is
 * located in the first sector of a volume.
 *
 * The routine may also initializes any caching mechanism. Thus, it must
 * be called before any other read or write access.
 *
 * \param dev       Specifies the file system device.
 * \param blkmnt    Handle of the block device's partition mount.
 * \param part_type Partition type:
 *                  - PTYPE_FAT32
 *                  - PTYPE_FAT32_LBA
 *                  - PTYPE_FAT16
 *                  - PTYPE_FAT16_BIG
 *                  - PTYPE_FAT16_LBA
 *                  - PTYPE_FAT12
 *
 * \return 0 on success or -1 in case of an error.
 */
int PhatVolMount(NUTDEVICE * dev, NUTFILE * blkmnt, uint8_t part_type)
{
    PHATVOL *vol;
    PHATVBR *vbr;
    BLKPAR_INFO pari;
    int sbn;
    NUTDEVICE *blkdev = blkmnt->nf_dev;

    /*
     * Allocate the volume information structure 
     */
    if ((dev->dev_dcb = malloc(sizeof(PHATVOL))) == 0) {
        return -1;
    }
    vol = (PHATVOL *) memset(dev->dev_dcb, 0, sizeof(PHATVOL));

    /*
     * Determine the PHAT type.
     */
    switch (part_type) {
    case PTYPE_FAT32:
    case PTYPE_FAT32_LBA:
        vol->vol_type = 32;
        break;
    case PTYPE_FAT16:
    case PTYPE_FAT16_BIG:
    case PTYPE_FAT16_LBA:
        vol->vol_type = 16;
        break;
    case PTYPE_FAT12:
        vol->vol_type = 12;
        break;
    }

    /*
     * Query information from the block device driver.
     */
    pari.par_nfp = blkmnt;
    if ((*blkdev->dev_ioctl) (blkdev, NUTBLKDEV_INFO, &pari)) {
        free(vol);
        errno = ENODEV;
        return -1;
    }
#if PHAT_SECTOR_BUFFERS
    for (sbn = 0; sbn < PHAT_SECTOR_BUFFERS; sbn++) {
        if ((vol->vol_buf[sbn].sect_data = malloc(pari.par_blksz)) == NULL) {
            PhatVolUnmount(dev);
            return -1;
        }
        vol->vol_buf[sbn].sect_num = (uint32_t)-1;
    }
#else
    vol->vol_buf[0].sect_data = pari.par_blkbp;
#endif
    sbn = 0;

    /*
     * We use PhatSectorRead() instead of PhatSectorLoad() for our 
     * very first read to properly initialize the caching status.
     */
    if (PhatSectorRead(blkmnt, 0, vol->vol_buf[sbn].sect_data)) {
        PhatVolUnmount(dev);
        return -1;
    }
    vol->vol_buf[sbn].sect_num = 0;
    vbr = (PHATVBR *) vol->vol_buf[sbn].sect_data;

    /*
     * PHAT32 doesn't have a fixed root directory. At this point
     * we reliably know wether we got PHAT32 or not. After having
     * determined the total number of clusters later, we can check
     * for PHAT12 or PHAT16.
     */
    if (vol->vol_type == 0 && vbr->bios_rootsz == 0) {
        vol->vol_type = 32;
    }

    /* Convert to PHAT12/PHAT16 layout. */
    if (vol->vol_type != 32) {
        memcpy(&vbr->boot_drive, &vbr->bios_tabsz_big, 26);
        memset(&vbr->bios_tabsz_big, 0, 28);
    }
#ifdef NUTDEBUG
    PhatDbgVbr(stdout, "Volume Boot Record", vbr);
#endif

    /*
     * Verify the VBR signature.
     */
    if (vol->vol_buf[sbn].sect_data[510] != 0x55 || vol->vol_buf[sbn].sect_data[511] != 0xAA) {
        PhatVolUnmount(dev);
        errno = ENODEV;
        return -1;
    }

    /*
     * Make sure we got a valid media type.
     */
    if (vbr->bios_media != 0xF0 && vbr->bios_media < 0xF8) {
        PhatVolUnmount(dev);
        errno = ENODEV;
        return -1;
    }

    /*
     * Examine the informations found in the boot record.
     */
    /* Bytes per sector. */
    vol->vol_sectsz = vbr->bios_sectsz;
    if (vol->vol_sectsz < 512 || vol->vol_sectsz & 0xFF) {
        PhatVolUnmount(dev);
        errno = ENODEV;
        return -1;
    }
    /* Sectors per cluster. */
    if ((vol->vol_clustsz = vbr->bios_clustsz) == 0) {
        PhatVolUnmount(dev);
        errno = ENODEV;
        return -1;
    }
    /* Allocation table size and position. */
    if (vbr->bios_tabsz) {
        vol->vol_tabsz = vbr->bios_tabsz;
    } else {
        vol->vol_tabsz = vbr->bios_tabsz_big;
    }
    vol->vol_tab_sect[0] = vbr->bios_rsvd_sects;
    if (vbr->bios_ntabs > 1) {
        vol->vol_tab_sect[1] = vol->vol_tab_sect[0] + vol->vol_tabsz;
    }
    /* Root directory size and position. */
    vol->vol_rootsz = (vbr->bios_rootsz * sizeof(PHATDIRENT) +  /* */
                       vol->vol_sectsz - 1) / vol->vol_sectsz;
    vol->vol_root_sect = vbr->bios_rsvd_sects + /* */
        vbr->bios_ntabs * vol->vol_tabsz;
    if (vol->vol_type == 32) {
        vol->vol_root_clust = vbr->bios_root_clust;
    }
    /* First data sector and number of data clusters. */
    vol->vol_data_sect = vol->vol_root_sect + vol->vol_rootsz;
    if (vbr->bios_volsz) {
        vol->vol_last_clust = vbr->bios_volsz - vol->vol_data_sect;
    } else {
        vol->vol_last_clust = vbr->bios_volsz_big - vol->vol_data_sect;
    }
    vol->vol_last_clust /= vol->vol_clustsz;
    /* First cluster number is 2. */
    vol->vol_last_clust += 2;

    /* 
     * Having calculated the total number of clusters allows us to 
     * distinguish between PHAT12 and PHAT16. 
     */
    if (vol->vol_type == 0) {
        if (vol->vol_last_clust > 4086) {
            vol->vol_type = 16;
        }
        else {
            vol->vol_type = 12;
        }
    }
#ifdef NUTDEBUG
    printf("\n%lu cluster -> PHAT%d\n", vol->vol_last_clust, vol->vol_type);
#endif

    dev->dev_icb = blkmnt;

    /* Initialize mutual exclusion semaphores. */
    NutEventPost(&vol->vol_fsmutex);
    NutEventPost(&vol->vol_iomutex);

    vol->vol_numfree = PhatCountFreeClusters(dev);

    return 0;
}

/*!
 * \brief Unmount a PHAT volume.
 *
 * This routine is called by the block device driver while unmounting a
 * partition.
 *
 * \param dev Specifies the file system device.
 *
 * \return 0 on success or -1 in case of an error.
 */
int PhatVolUnmount(NUTDEVICE * dev)
{
    PHATVOL *vol = (PHATVOL *) dev->dev_dcb;

    if (vol) {
#if PHAT_SECTOR_BUFFERS
        int sbn;

        for (sbn = 0; sbn < PHAT_SECTOR_BUFFERS; sbn++) {
            if (vol->vol_buf[sbn].sect_data) {
                free(vol->vol_buf[sbn].sect_data);
            }
        }
#endif
        free(vol);
    }
    return 0;
}

/*
 * \brief Get first sector of a specified cluster.
 *
 * \param nfp   File descriptor.
 * \param clust Specified cluster.
 */
uint32_t PhatClusterSector(NUTFILE * nfp, uint32_t clust)
{
    NUTDEVICE *dev = nfp->nf_dev;
    PHATVOL *vol = (PHATVOL *) dev->dev_dcb;

    /*
     * If the file descriptor specifies the root directory, then the first
     * sector is located after the reserved sectors and the allocation table.
     */
    if (IsFixedRootDir(nfp)) {
        return vol->vol_root_sect;
    }

    /*
     * For all other files/directories the sector is located in the data
     * area of the volume.
     */
    if (clust >= 2) {
        clust -= 2;
    }
    return vol->vol_data_sect + clust * vol->vol_clustsz;
}

/*@}*/
