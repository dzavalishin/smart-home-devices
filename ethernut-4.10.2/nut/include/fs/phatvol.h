#ifndef FS_PHATVOL_H_
#define FS_PHATVOL_H_

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
 * \file fs/phatvol.h
 * \brief PHAT file system.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.6  2009/01/17 11:26:47  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.5  2008/08/11 07:00:19  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.4  2006/10/08 16:42:56  haraldkipp
 * Not optimal, but simple and reliable exclusive access implemented.
 * Fixes bug #1486539. Furthermore, bug #1567790, which had been rejected,
 * had been reported correctly and is now fixed.
 *
 * Revision 1.3  2006/07/11 12:20:19  haraldkipp
 * PHAT file system failed when accessed from multiple threads. A mutual
 * exclusion semaphore fixes this.
 *
 * Revision 1.2  2006/02/23 15:47:18  haraldkipp
 * PHAT file system now supports configurable number of sector buffers.
 *
 * Revision 1.1  2006/01/05 16:33:10  haraldkipp
 * First check-in.
 *
 *
 * \endverbatim
 */

#include <cfg/fs.h>

#include <sys/types.h>
#include <sys/file.h>
#include <sys/device.h>

/*!
 * \addtogroup xgPhatVol
 */
/*@{*/

/*!
 * \brief Volume boot record.
 */
typedef struct __attribute__ ((packed)) _PHATVBR {
    /*! \brief Jump instruction, contains E9xxxx or EBxx90. */
    uint8_t boot_jump[3];
    /*! \brief OEM name and version. */
    uint8_t boot_oem[8];
    /*! \brief Bytes per sector. 
     *
     * The BIOS parameter block starts here.
     */
    uint16_t bios_sectsz;
    /*! \brief Sectors per cluster. */
    uint8_t bios_clustsz;
    /*! \brief Number of reserved sectors. 
     *
     * This includes the boot sector, which is typically the only
     * reserved sector with PHAT12/16.
     */
    uint16_t bios_rsvd_sects;
    /*! \brief Number of allocation tables. */
    uint8_t bios_ntabs;
    /*! \brief Number of root directory entries. */
    uint16_t bios_rootsz;
    /*! \brief Total number of sectors. */
    uint16_t bios_volsz;
    /*! \brief Media descriptor. */
    uint8_t bios_media;
    /*! \brief Number of sectors per allocation table.
     *
     * If zero, then the value is stored in bios_tabsz_big.
     */
    uint16_t bios_tabsz;
    /*! \brief Number of sectors per track. */
    uint16_t bios_sects_p_trk;
    /*! \brief Number of heads. */
    uint16_t bios_heads;
    /*! \brief Number of hidden sectors. 
     *
     * With PHAT12 this field is two bytes only. We ignore it anyway.
     */
    uint32_t bios_sects_hidd;
    /*! \brief Total number of sectors for huge drives. 
     *
     * Valid only if the value in bios_volsz is zero.
     */
    uint32_t bios_volsz_big;

    /*! \brief Number of sectors per allocation table for huge drives.
     *
     * This and the following 6 fields are available with PHAT32 only.
     */
    uint32_t bios_tabsz_big;
    /*! \brief Extended flags. 
     *
     * If bit 7 is set, then bits 0-3 specify the active allocation table.
     * This feature is not yet supported. PHAT32 always updates the first
     * two tables.
     */
    uint16_t bios_xflags;
    /*! \brief File system version. */
    uint16_t bios_fsver;
    /*! \brief First cluster of root directory. */
    uint32_t bios_root_clust;
    /*! \brief File system info sector. */
    uint16_t bios_fsinfo;
    /*! \brief Boot backup sector. */
    uint16_t bios_boot_bak;
    /*! \brief Reserved for future expansion. 
     *
     * End of the BIOS parameter block.
     */
    uint8_t bios_rsvd32[12];

    /*! \brief Logical drive number.
     *
     * With PHAT12 and PHAT16 this and the following fields
     * are at offset 36.
     */
    uint8_t boot_drive;
    /*! \brief Reserved field, used by Windows NT. */
    uint8_t boot_rsvd_nt;
    /*! \brief Extended signature.
     *
     * If 0x28 or 0x29, than the following fields are valid.
     */
    uint8_t boot_xsig;
    /*! \brief Volume serial number. */
    uint32_t boot_vol_id;
    /*! \brief Volume label. */
    uint8_t boot_vol_lbl[11];
    /*! \brief File system. */
    uint8_t boot_vol_fs[8];
} PHATVBR;

/*!
 * \brief Sector buffer structure.
 */
typedef struct _PHATSECTBUF {
    /*! \brief Sector data buffer. */
    uint8_t *sect_data;
    /*! \brief Sector currently stored in the buffer. */
    uint32_t sect_num;
    /*! \brief If not zero, buffer needs to be written. */
    int sect_dirty;
} PHATSECTBUF;

/*!
 * \brief Volume info structure.
 */
typedef struct _PHATVOL {
    /*! \brief Type of volume.
     *
     * Either 12, 16 or 32.
     */
    int vol_type;
    /*! \brief Number of free clusters. */
    uint32_t vol_numfree;
    /*! \brief Possibly next free cluster. */
    uint32_t vol_nxtfree;
    /*! \brief Sector buffer of this volume. */
#if PHAT_SECTOR_BUFFERS
    PHATSECTBUF vol_buf[PHAT_SECTOR_BUFFERS];
    /*! \brief Next buffer to use. Simple round robin scheme. */
    int vol_usenext;
#else
    PHATSECTBUF vol_buf[1];
#endif
    /*! \brief Mutual exclusion filesystem access semaphore. */
    HANDLE vol_fsmutex;
    /*! \brief Mutual exclusion I/O semaphore. */
    HANDLE vol_iomutex;
    /*! \brief Bytes per sector. */
    unsigned int vol_sectsz;
    /*! \brief Sectors per cluster. */
    unsigned int vol_clustsz;

    /*! \brief Number of sectors per allocation table. */
    uint32_t vol_tabsz;
    /*! \brief First sector of each allocation table. 
     *
     * We maintain upto two allocation tables.
     */
    uint32_t vol_tab_sect[2];

    /*! \brief Number of sectors used by the root directory. */
    unsigned int vol_rootsz;
    /*! \brief First sector of the root directory. */
    uint32_t vol_root_sect;
    /*! \brief First cluster of the root directory. */
    uint32_t vol_root_clust;

    /*! \brief Last data cluster number.
     *
     * The first cluster number is 2, so this value is equal to the total
     * number of data clusters plus 2.
     */
    uint32_t vol_last_clust;
    /*! \brief First data sector. 
     *
     * The first sector following the root directory.
     */
    uint32_t vol_data_sect;
} PHATVOL;

/*@}*/

__BEGIN_DECLS
/* Prototypes */
extern int PhatVolMount(NUTDEVICE * dev, NUTFILE * blkmnt, uint8_t part_type);
extern int PhatVolUnmount(NUTDEVICE * dev);
extern uint32_t PhatClusterSector(NUTFILE * nfp, uint32_t clust);

__END_DECLS
/* End of prototypes */
#endif
