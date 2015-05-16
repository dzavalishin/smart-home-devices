#ifndef FS_DOSPART_H_
#define FS_DOSPART_H_

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
 * \file fs/dospart.h
 * \brief DOS partition table definitions.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.3  2008/08/11 07:00:19  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.2  2008/07/14 13:09:30  haraldkipp
 * Allow small MultiMedia Cards without partition table.
 *
 * Revision 1.1  2006/01/05 16:32:26  haraldkipp
 * First check-in.
 *
 *
 * \endverbatim
 */

#include <sys/types.h>
#include <stdint.h>

/*!
 * \addtogroup xgPartition
 */
/*@{*/

/*!
 * \name Known partition types.
 */
/*@{*/

#define PTYPE_EMPTY         0x00
#define PTYPE_FAT12         0x01
#define PTYPE_FAT16         0x04
#define PTYPE_DOS_EXT       0x05
#define PTYPE_FAT16_BIG     0x06
#define PTYPE_IFS           0x07
#define PTYPE_OS2V1         0x08
#define PTYPE_AIXDATA       0x09
#define PTYPE_OS2BM         0x0A
#define PTYPE_FAT32         0x0B
#define PTYPE_FAT32_LBA     0x0C
#define PTYPE_FAT16_LBA     0x0E
#define PTYPE_EXT_LBA       0x0F
#define PTYPE_OPUS          0x10
#define PTYPE_FAT12_HIDDEN  0x11
#define PTYPE_OEM           0x12
#define PTYPE_FAT16_HIDDEN  0x14
#define PTYPE_FAT16_BIGH    0x16
#define PTYPE_IFS_HIDDEN    0x17
#define PTYPE_AST_SLEEP     0x18
#define PTYPE_FAT32_HIDDEN  0x1B
#define PTYPE_FAT32_LBAH    0x1C
#define PTYPE_FAT16_LBAH    0x1E
#define PTYPE_DOS33_NEC     0x24
#define PTYPE_JFS           0x35
#define PTYPE_MINIX         0x41
#define PTYPE_DYN           0x42
#define PTYPE_UNIX_SYSV     0x63
#define PTYPE_LINUX_SWAP    0x82
#define PTYPE_LINUX         0x83
#define PTYPE_PM_HIBERNATE  0x84
#define PTYPE_LINUX_EXT     0x85
#define PTYPE_FAT16_MULTI   0x86
#define PTYPE_NTFS_MULTI    0x87
#define PTYPE_LT_HIBERNATE  0xA0
#define PTYPE_BSD           0xA5
#define PTYPE_OPENBSD       0xA6
#define PTYPE_MACOSX        0xA8
#define PTYPE_NETBSD        0xA9
#define PTYPE_MACOSX_BOOT   0xAB
#define PTYPE_BSD_SWAP      0xB8
#define PTYPE_CPM86         0xD8
#define PTYPE_DATA          0xDA
#define PTYPE_DRCPM         0xDB
#define PTYPE_OEM_DELL      0xDE
#define PTYPE_GPT           0xEE
#define PTYPE_EFI           0xEF
#define PTYPE_OEM_IBM       0xFE

/*@}*/

/*!
 * \brief Position of the type specifier in the partition boot record.
 *
 * We expect the letters 'F', 'A' and 'T' at this position.
 */
#define DOSPART_TYPEPOS     54

/*!
 * \brief Position of the partition table in the master boot record.
 */
#define DOSPART_SECTORPOS   446

/*!
 * \brief Position of the cookie in the boot sector.
 *
 * We expect the byte values 0x55 and 0xAA at this position.
 */
#define DOSPART_MAGICPOS    510

/*!
 * \brief Partition table entry.
 *
 * Multibyte values are little endian.
 */
typedef struct __attribute__ ((packed)) _DOSPART {
    /*! \brief Boot descriptor. */
    uint8_t part_state;
    /*! \brief Partition start. */
    uint8_t part_start[3];
    /*! \brief Partition type. See PTYPE_ definitions. */
    uint8_t part_type;
    /*! \brief Partition end. */
    uint8_t part_end[3];
    /*! \brief First linear block address of this partition. */
    uint32_t part_sect_offs;
    /*! \brief Partition size in sectors. */
    uint32_t part_sects;
} DOSPART;

/*@}*/

#endif
