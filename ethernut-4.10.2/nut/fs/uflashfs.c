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

#include <fs/fs.h>
#include <sys/nutdebug.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>

#include <dev/spi_at45dib.h>
#include <fs/uflashfs.h>

#if 0
/* Use for local debugging. */
#define NUTDEBUG
#include <stdio.h>
#endif

/*!
 * \file fs/uflashfs.c
 * \brief UFLASH File System.
 *
 * The UFLASH file system has been specifically designed for serial
 * flash memory chips used on tiny embedded systems.
 *
 * Such memory chips do not allow to program single bytes. Instead,
 * only full pages can be programmed. On the other hand, these chips
 * provide one or two integrated RAM buffers. They allow to modify
 * single bytes without transfering full pages between the memory
 * chip and the microcontroller's RAM.
 *
 * Although no mechanism has been implemented to replace worn out pages, 
 * the file system maintains a very well balanced wear levelling. This
 * includes regular movement of static files.
 *
 * Furthermore, previously unfinished write accesses are automatically 
 * detected and reverted when mounting a volume. However, this features
 * still needs to be tested more thoroughly.
 *
 * Note, that directories do not really exist in the UFLASH file system.
 * They are mimicked in a limited way. When scanning a directory, all
 * subdirectories will appear again for each file they contain.
 *
 * \verbatim
 * $Id$
 * \endverbatim
 */

/*!
 * \addtogroup xgUFlash
 */
/*@{*/

/* Wheter to use time stamps. Requires, that the system time had been set. */
//#define UFLASH_USE_TIMESTAMP

/* Wheter to use IDs instead of file names. Not implemented. */
//#define UFLASH_ID_NAMES

/*
 * File system configuration.
 */
#ifndef UFLASH_MAX_BLOCKS
/*! \brief Max. number of blocks per file system. */
#define UFLASH_MAX_BLOCKS       8192
#endif

#ifndef UFLASH_BLOCK_UNITS
/*! \brief Number of units per block. */
#define UFLASH_BLOCK_UNITS      4
#endif

#ifndef UFLASH_ENTRIES
/*! \brief Number of file system entries. */
#define UFLASH_ENTRIES          128
#endif

#ifndef UFLASH_USAGE_CACHE
/*! \brief Size of the least used buffer. */
#define UFLASH_USAGE_CACHE      32
#endif

#if UFLASH_MAX_BLOCKS <= 255
typedef uint8_t blknum_t;
typedef uint_fast8_t blknum_fast_t;
#elif UFLASH_MAX_BLOCKS <= 65535
typedef uint16_t blknum_t;
typedef uint_fast16_t blknum_fast_t;
#else
typedef uint32_t blknum_t;
typedef uint_fast32_t blknum_fast_t;
#endif

#ifndef UFLASH_MAX_PATH
/*! \brief Maximum lenght of a full absolute path.
 *
 * If not defined, a variable path name length is used. Note, that
 * the total size of the name is still limited, because the block
 * header and the entry header must fit into the first unit of the
 * file system block.
 */
#define UFLASH_MAX_PATH      0
#endif

#define UFLASH_BLOCK_INVALID    ((blknum_t)-1)

/*!
 * \brief Internal block usage cache structure.
 */
typedef struct _BLOCK_USAGE {
    uint8_t use_cnt;
    blknum_t use_phyblk;
} BLOCK_USAGE;

/*!
 * \brief Internal block usage cache.
 */
static BLOCK_USAGE min_used[UFLASH_USAGE_CACHE];

/*!
 * \brief UFLASH block header.
 *
 * Each used block starts with a BLOCKHEAD structure.
 */
typedef struct __attribute__ ((packed)) _BLOCKHEAD {
    blknum_t bh_logblk;         /*!< \brief Logical block number. */
    blknum_t bh_version;        /*!< \brief Logical block version. */
    blknum_t bh_entblk;         /*!< \brief Logical entry block number. */
    blknum_t bh_entseq;         /*!< \brief Block sequence. */
} BLOCKHEAD;

/*!
 * \brief UFLASH block footer.
 *
 * Each block ends with a BLOCKFOOT structure.
 */
typedef struct __attribute__ ((packed)) _BLOCKFOOT {
#ifdef UFLASH_USE_TIMESTAMP
    time_t bf_time;             /*!< \brief Last update time. */
#endif
    uint8_t bf_wear;            /*!< \brief Erase counter. */
} BLOCKFOOT;

/*
 * UFLASH entry header.
 *
 * On each entry block an ENTRYHEAD structure follows the BLOCKHEAD.
 * The ENTRYHEAD is followed by the variable length name of this entry.
 * After the name comes the data, immediatly followed by an optional 
 * timestamp.
 */
typedef struct __attribute__ ((packed)) _ENTRYHEAD {
    /* Entry attributes. */
    uint8_t eh_attr;
    /* Name length. */
    uint8_t eh_nlen;
} ENTRYHEAD;

/*!
 * \brief File system entry descriptor.
 */
typedef struct _UFLASHENTRY {
    /*! \brief Logical block number of the entry. */
    blknum_t ent_id;
    /*! \brief Current data pointer position. */
    uint32_t ent_pos;
    /*! \brief Current logical block number. */
    blknum_t ent_bidx;
    /*! \brief Current sequence. */
    uint16_t ent_sidx;
    /*! \brief Current unit index within the current block. */
    uint16_t ent_uidx;
    /*! \brief Current position within the current unit. */
    uint16_t ent_upos;

    /*! \brief File open mode flags. */
    uint32_t ent_mode;
    /*! \brief Entry attributes. */
    uint8_t ent_attr;
    /*! \brief Name length. */
    uint8_t ent_namlen;
    /*! \brief Total data size. */
    uint32_t ent_size;

    /*! \brief Total file data offset of the entry block. */
    uint16_t ent_eoff;
} UFLASHENTRY;

#define UFLASH_VOLF_FIXED       0x80

/*! \brief Volume info structure type. */
typedef struct _UFLASHVOLUME UFLASHVOLUME;

/*!
 * \brief Volume info structure.
 */
struct _UFLASHVOLUME {
    /*! \brief I/O interface. */
    NUTSERIALFLASH *vol_ifc;
    /*! \brief Exclusive access queue. */
    HANDLE vol_mutex;
    /*! \brief Attribute flags. */
    uint8_t vol_attrib;
    /*! \brief Number of blocks in this volume. */
    blknum_t vol_blocks;
    /*! \brief Block translation table. */
    blknum_t *vol_l2p;
};

/*!
 * \brief Entry search structure.
 */
typedef struct _UFLASHFIND {
    blknum_t uff_lbe;
    char *uff_path;
} UFLASHFIND;

/*!
 * \brief Copy a given unit from one block to another.
 */
static int FlashUnitCopy(NUTSERIALFLASH * ifc, blknum_t b_src, blknum_t b_dst, sf_unit_t unit)
{
    unit += ifc->sf_rsvbot;
    return (*ifc->sf_copy) (ifc, b_src * UFLASH_BLOCK_UNITS + unit, b_dst * UFLASH_BLOCK_UNITS + unit);
}

/*!
 * \brief Commit changes of a given unit.
 */
static void FlashUnitCommit(NUTSERIALFLASH * ifc, blknum_t b, sf_unit_t unit)
{
    (*ifc->sf_commit) (ifc, b * UFLASH_BLOCK_UNITS + unit + ifc->sf_rsvbot);
}

/*!
 * \brief Read unit.
 */
static int FlashUnitRead(NUTSERIALFLASH * ifc, blknum_t b, sf_unit_t unit, int upos, void *data, int len)
{
    int rc = ifc->sf_unit_size - upos;

    if (unit == UFLASH_BLOCK_UNITS - 1) {
        rc -= sizeof(BLOCKFOOT);
    }
    if (rc > len) {
        rc = len;
    }
    (*ifc->sf_read) (ifc, b * UFLASH_BLOCK_UNITS + unit + ifc->sf_rsvbot, upos, data, rc);

    return rc;
}

/*!
 * \brief Write unit.
 */
static int FlashUnitWrite(NUTSERIALFLASH * ifc, blknum_t b, sf_unit_t unit, int upos, CONST void *data, int len)
{
    int rc = ifc->sf_unit_size - upos;

    if (unit == UFLASH_BLOCK_UNITS - 1) {
        rc -= sizeof(BLOCKFOOT);
    }
    if (rc > len) {
        rc = len;
    }
    (*ifc->sf_write) (ifc, b * UFLASH_BLOCK_UNITS + unit + ifc->sf_rsvbot, upos, data, rc);

    return rc;
}

/*!
 * \brief Verify CRC of a given block.
 */
static int FlashCheckBlock(NUTSERIALFLASH * ifc, blknum_t b)
{
    return (*ifc->sf_check) (ifc, b * UFLASH_BLOCK_UNITS + ifc->sf_rsvbot, UFLASH_BLOCK_UNITS);
}

/*!
 * \brief Read header of a given block.
 */
static int FlashReadBlockHead(NUTSERIALFLASH * ifc, blknum_t b, BLOCKHEAD * bh)
{
    return (*ifc->sf_read) (ifc, b * UFLASH_BLOCK_UNITS + ifc->sf_rsvbot, 0, bh, sizeof(BLOCKHEAD));
}

/*!
 * \brief Write header of a given block.
 */
static int FlashWriteBlockHead(NUTSERIALFLASH * ifc, blknum_t b, BLOCKHEAD * bh)
{
    /* Update the block's version number. */
    bh->bh_version++;
    return (*ifc->sf_write) (ifc, b * UFLASH_BLOCK_UNITS + ifc->sf_rsvbot, 0, bh, sizeof(BLOCKHEAD));
}

/*!
 * \brief Read entry of a given block.
 */
static int FlashReadEntry(NUTSERIALFLASH * ifc, blknum_t b, ENTRYHEAD * eh, char **name)
{
    int rc;

    /* Read the entry header, which contains the name length. */
    rc = (*ifc->sf_read) (ifc, b * UFLASH_BLOCK_UNITS + ifc->sf_rsvbot, sizeof(BLOCKHEAD), eh, sizeof(ENTRYHEAD));
    if (rc == 0 && name) {
        *name = malloc(eh->eh_nlen + 1);
        if (*name == NULL) {
            rc = -1;
        } else {
            *(*name + eh->eh_nlen) = '\0';
            rc = (*ifc->sf_read) (ifc, b * UFLASH_BLOCK_UNITS + ifc->sf_rsvbot, sizeof(BLOCKHEAD) + sizeof(ENTRYHEAD), *name,
                                  eh->eh_nlen);
        }
    }
    return rc;
}

/*!
 * \brief Compare entry name of a given block.
 */
static int FlashBlockCmpEntryName(NUTSERIALFLASH * ifc, blknum_t b, CONST char *name, uint8_t len)
{
    return (*ifc->sf_compare) (ifc, b * UFLASH_BLOCK_UNITS + ifc->sf_rsvbot, sizeof(BLOCKHEAD) + sizeof(ENTRYHEAD), name, len);
}

/*!
 * \brief Write entry of a given block.
 */
static int FlashWriteEntry(NUTSERIALFLASH * ifc, blknum_t b, CONST ENTRYHEAD * eh, CONST char *name)
{
    int rc;

    rc = (*ifc->sf_write) (ifc, b * UFLASH_BLOCK_UNITS + ifc->sf_rsvbot, sizeof(BLOCKHEAD), eh, sizeof(ENTRYHEAD));
    if (rc == 0) {
        rc = (*ifc->sf_write) (ifc, b * UFLASH_BLOCK_UNITS + ifc->sf_rsvbot, sizeof(BLOCKHEAD) + sizeof(ENTRYHEAD), name,
                               eh->eh_nlen);
    }
    return rc;
}

/*!
 * \brief Read footer of a given block.
 */
static int FlashReadBlockFoot(NUTSERIALFLASH * ifc, blknum_t b, BLOCKFOOT * bf)
{
    return (*ifc->sf_read) (ifc, (b + 1) * UFLASH_BLOCK_UNITS - 1 + ifc->sf_rsvbot, -(int) sizeof(BLOCKFOOT), bf,
                            sizeof(BLOCKFOOT));
}

/*!
 * \brief Write footer of a given block.
 */
static int FlashWriteBlockFoot(NUTSERIALFLASH * ifc, blknum_t b, BLOCKFOOT * bf)
{
    bf->bf_wear++;
#ifdef UFLASH_USE_TIMESTAMP
    bf->bf_time = time(NULL);
#endif
    return (*ifc->sf_write) (ifc, (b + 1) * UFLASH_BLOCK_UNITS - 1 + ifc->sf_rsvbot, -(int) sizeof(BLOCKFOOT), bf,
                             sizeof(BLOCKFOOT));
}

/*!
 * \brief Erase first unit of a given block.
 */
static int FlashEraseEntry(NUTSERIALFLASH * ifc, blknum_t b)
{
    return (*ifc->sf_erase) (ifc, b * UFLASH_BLOCK_UNITS + ifc->sf_rsvbot, 1);
}

/*!
 * \brief Erase data units of a given block.
 */
static int FlashEraseBlockData(NUTSERIALFLASH * ifc, blknum_t b)
{
    return (*ifc->sf_erase) (ifc, b * UFLASH_BLOCK_UNITS + 1 + ifc->sf_rsvbot, UFLASH_BLOCK_UNITS - 1);
}

/*!
 * \brief Erase a given number of blocks.
 */
static int FlashEraseBlocks(NUTSERIALFLASH * ifc, int n)
{
    return (*ifc->sf_erase) (ifc, ifc->sf_rsvbot, n * UFLASH_BLOCK_UNITS);
}

/*!
 * \brief Collect least used blocks.
 *
 * \param vol Pointer to the information structure of a mounted volume.
 *
 * TODO: We may optionally exclude static files from recycling.
 */
static void CollectLeastUsed(UFLASHVOLUME * vol, uint8_t * usage)
{
    uint8_t rmin;
    uint8_t rmax;
    blknum_t b;

    /* Determine the range of values. */
    rmin = usage[0];
    rmax = rmin;
    for (b = 1; b < vol->vol_blocks; b++) {
        if (usage[b] < rmin) {
            rmin = usage[b];
            if (rmax - rmin >= 128) {
                break;
            }
        } else if (usage[b] > rmax) {
            rmax = usage[b];
            if (rmax - rmin >= 128) {
                break;
            }
        }
    }

    /*
       ** If the range of values is equal or greater than half of the full 
       ** range, then some values overflowed. In this case we shift all
       ** values by half of the full range to get a consecutive sequence.
     */
    if (b < vol->vol_blocks) {
        rmax = 0;
        for (b = 0; b < vol->vol_blocks; b++) {
            usage[b] += 128;
            if (usage[b] > rmax) {
                rmax = usage[b];
            }
        }
    }

    /*
       ** Collect a sorted table of minimums.
     */
    memset(min_used, 0xff, sizeof(min_used));
    for (b = 0; b < vol->vol_blocks; b++) {
        int j = UFLASH_USAGE_CACHE - 1;

        if (min_used[j].use_phyblk > vol->vol_blocks || min_used[j].use_cnt > usage[b]) {
            while (--j >= 0) {
                if (usage[b] >= min_used[j].use_cnt && min_used[j].use_phyblk < vol->vol_blocks) {
                    break;
                }
                min_used[j + 1] = min_used[j];
            }
            min_used[j + 1].use_cnt = usage[b];
            min_used[j + 1].use_phyblk = b;
        }
    }
}

static blknum_t PhysBlkAllocate(UFLASHVOLUME * vol, int level);

/*!
 * \brief Move a physical block.
 *
 * \param vol Pointer to the information structure of a mounted volume.
 * \param src Physical source block number. On return, this block will
 *            be available again.
 * \param dst Physical destination block number.
 */
static int PhysBlkMove(UFLASHVOLUME * vol, blknum_t src, blknum_t dst)
{
    int rc = 0;
    BLOCKHEAD bh;
    BLOCKFOOT bf;
    uint_fast8_t u;

    /* Read the header of the source block. */
    FlashReadBlockHead(vol->vol_ifc, src, &bh);
    /* Read the footer of the destination block. */
    FlashReadBlockFoot(vol->vol_ifc, dst, &bf);
    /* Erase the tail of the destination. */
    FlashEraseBlockData(vol->vol_ifc, dst);
    /* Copy unit by unit. */
    for (u = 0; rc == 0 && u < UFLASH_BLOCK_UNITS; u++) {
        FlashUnitCopy(vol->vol_ifc, src, dst, u);
        /* If this is the first unit, write a new version of the header. */
        if (u == 0) {
            FlashWriteBlockHead(vol->vol_ifc, dst, &bh);
        }
        /* If this is the last unit, write back an updated footer. */
        else if (u == UFLASH_BLOCK_UNITS - 1) {
            FlashWriteBlockFoot(vol->vol_ifc, dst, &bf);
        }
        /* Commit the unit. */
        FlashUnitCommit(vol->vol_ifc, dst, u);
    }
    /* If all done, erase the first unit of the source block. This will
       ** make the block available again. */
    FlashEraseEntry(vol->vol_ifc, src);

    return 0;
}

/*!
 * \brief Get the first unused block.
 *
 * \param vol Pointer to the information structure of a mounted volume.
 */
static blknum_t PhysBlkUnused(UFLASHVOLUME * vol)
{
    BLOCKHEAD bh;
    blknum_t b;

    for (b = 0; b < vol->vol_blocks; b++) {
        FlashReadBlockHead(vol->vol_ifc, b, &bh);
        if (bh.bh_logblk >= vol->vol_blocks) {
            break;
        }
    }
    return b;
}

/*!
 * \brief Reallocate an unused physical block.
 *
 * \param vol Pointer to the information structure of a mounted volume.
 */
static int PhysBlkRequest(UFLASHVOLUME * vol, int level, blknum_t n)
{
    blknum_t b;

    /* Check if this block is in use. */
    for (b = 0; b < vol->vol_blocks; b++) {
        if (vol->vol_l2p[b] == n) {
            blknum_t nn;

            nn = PhysBlkAllocate(vol, level + 1);
            if (nn >= vol->vol_blocks) {
                return -1;
            }
            vol->vol_l2p[b] = nn;
            PhysBlkMove(vol, n, nn);
            break;
        }
    }
    return 0;
}

/*!
 * \brief Allocate an unused physical block.
 *
 * \param vol Pointer to the information structure of a mounted volume.
 */
static blknum_t PhysBlkAllocate(UFLASHVOLUME * vol, int level)
{
    uint8_t *phy_use;
    int i;

    for (;;) {
        if (level || min_used[UFLASH_USAGE_CACHE - (UFLASH_USAGE_CACHE / 4)].use_phyblk < vol->vol_blocks) {
            for (i = 0; i < UFLASH_USAGE_CACHE; i++) {
                blknum_t n = min_used[i].use_phyblk;

                if (n < vol->vol_blocks) {
                    min_used[i].use_phyblk = UFLASH_BLOCK_INVALID;
                    if (PhysBlkRequest(vol, level, n)) {
                        return UFLASH_BLOCK_INVALID;
                    }
                    return n;
                }
            }
        }
        /* Cache exhausted. */
        phy_use = calloc(vol->vol_blocks, 1);
        if (phy_use == NULL) {
            break;
        }
        for (i = 0; i < vol->vol_blocks; i++) {
            BLOCKFOOT bf;

            FlashReadBlockFoot(vol->vol_ifc, i, &bf);
            phy_use[i] = bf.bf_wear;
        }
        for (i = 0; i < vol->vol_blocks; i++) {
            if (vol->vol_l2p[i] < vol->vol_blocks) {
                phy_use[vol->vol_l2p[i]] += UFLASH_USAGE_CACHE - 1;
            }
        }
        CollectLeastUsed(vol, phy_use);
        free(phy_use);
        if (level) {
            break;
        }
    }
    return PhysBlkUnused(vol);
}

/*!
 * \brief Allocate a new logical block.
 *
 * \param vol Pointer to the information structure of a mounted volume.
 */
static blknum_t LogBlkAllocate(UFLASHVOLUME * vol, blknum_t lb)
{
    int lb_last = lb ? vol->vol_blocks : UFLASH_ENTRIES;

    /* Find an unused logical block. */
    for (; lb < lb_last; lb++) {
        if (vol->vol_l2p[lb] >= vol->vol_blocks) {
            /* Found, assign a physical block. */
            vol->vol_l2p[lb] = PhysBlkAllocate(vol, 0);
            break;
        }
    }
    return lb;
}

/*!
 * \brief Reallocate a logical block.
 *
 * \param vol Pointer to the information structure of a mounted volume.
 */
static blknum_t LogBlkReallocate(UFLASHVOLUME * vol, blknum_t lb)
{
    blknum_t b_old;
    blknum_t b_new;

    /* Allocate a new physical block. Note, that the currently assigned
       ** physical block may move during this allocation. */
    b_new = PhysBlkAllocate(vol, 0);
    if (b_new >= vol->vol_blocks) {
        b_old = b_new;
    } else {
        b_old = vol->vol_l2p[lb];
        vol->vol_l2p[lb] = b_new;
    }
    return b_old;
}

/*!
 * \brief Find existing data block by sequence.
 *
 * \param vol Pointer to the information structure of a mounted volume.
 */
static int LogBlkRelease(UFLASHVOLUME * vol, blknum_t lb)
{
    blknum_t b;

    b = vol->vol_l2p[lb];
    vol->vol_l2p[lb] = UFLASH_BLOCK_INVALID;

    return FlashEraseEntry(vol->vol_ifc, b);
}

/*!
 * \brief Find existing data block by sequence.
 *
 * \param vol Pointer to the information structure of a mounted volume.
 * \param lbe Logical block of the entry.
 * \param seq Sequence number to search.
 *
 * \return Logical block number of the requested data block. If this is
 *         equal to the number of blocks available, then the sequence
 *         doesn't exist.
 */
static blknum_t EntrySeek(UFLASHVOLUME * vol, blknum_t lbe, blknum_t seq)
{
    blknum_t lb = lbe;

    /* Data block sequence 0 is the same as the entry block. No need to
       ** search for it. */
    if (seq) {
        /* Walking through the translation table requires exclusive access. */
        NutEventWait(&vol->vol_mutex, 0);
        /* No need to search the entry blocks. */
        for (lb = UFLASH_ENTRIES; lb < vol->vol_blocks; lb++) {
            blknum_t b = vol->vol_l2p[lb];

            /* If this block is allocated, read its header. */
            if (b < vol->vol_blocks) {
                BLOCKHEAD bh;

                FlashReadBlockHead(vol->vol_ifc, b, &bh);
                if (bh.bh_entblk == lbe && bh.bh_entseq == seq) {
                    /* Block found. */
                    break;
                }
            }
        }
        /* Release the exclusive access. */
        NutEventPost(&vol->vol_mutex);
    }
    return lb;
}

/*!
 * \brief Update the position indices.
 */
static void EntryPosSet(NUTFILE * nfp, long pos)
{
    uint16_t seq;
    uint32_t off;
    UFLASHENTRY *ent;
    UFLASHVOLUME *vol;

    ent = (UFLASHENTRY *) nfp->nf_fcb;
    vol = (UFLASHVOLUME *) nfp->nf_dev->dev_dcb;

    /* Current position within this entry. */
    ent->ent_pos = pos;
    off = ent->ent_eoff + ent->ent_pos;

    /* Current block sequence within this entry. */
    seq = (uint16_t) (off / (vol->vol_ifc->sf_unit_size * UFLASH_BLOCK_UNITS - (sizeof(BLOCKHEAD) + sizeof(BLOCKFOOT))));
    if (seq != ent->ent_sidx) {
        ent->ent_sidx = seq;
        ent->ent_bidx = UFLASH_BLOCK_INVALID;
    }

    off -= ent->ent_sidx * (vol->vol_ifc->sf_unit_size * UFLASH_BLOCK_UNITS - (sizeof(BLOCKHEAD) + sizeof(BLOCKFOOT)));

    /* Page number within the current block. */
    ent->ent_uidx = (uint16_t) ((off + sizeof(BLOCKHEAD)) / vol->vol_ifc->sf_unit_size);
    off -= ent->ent_uidx * vol->vol_ifc->sf_unit_size;

    /* Position with the current page. */
    ent->ent_upos = (uint16_t) off + sizeof(BLOCKHEAD);
}

static void EntryPosInc(NUTFILE * nfp, int step)
{
    UFLASHENTRY *ent;
    UFLASHVOLUME *vol;

    ent = (UFLASHENTRY *) nfp->nf_fcb;
    vol = (UFLASHVOLUME *) nfp->nf_dev->dev_dcb;

    ent->ent_pos += step;
    ent->ent_upos += step;
    if (ent->ent_uidx < UFLASH_BLOCK_UNITS - 1) {
        step = vol->vol_ifc->sf_unit_size - ent->ent_upos;
    } else {
        step = vol->vol_ifc->sf_unit_size - sizeof(BLOCKFOOT) - ent->ent_upos;
    }
    if (step == 0) {
        ent->ent_uidx++;
        if (ent->ent_uidx >= UFLASH_BLOCK_UNITS) {
            ent->ent_upos = sizeof(BLOCKHEAD);
            ent->ent_uidx = 0;
            ent->ent_sidx++;
            ent->ent_bidx = UFLASH_BLOCK_INVALID;
        } else {
            ent->ent_upos = 0;
        }
    }
}

/*!
 * \brief Scan for all blocks associated to a given entry.
 *
 * \param vol Pointer to the information structure of a mounted volume.
 */
static long EntryScan(UFLASHVOLUME * vol, blknum_t lbe, time_t * mod)
{
    long siz;
    int p;
    int i;
    int off;
    BLOCKHEAD bh;
    blknum_t lb;
    blknum_t lbs = lbe;
    blknum_t seq = 0;

#ifdef UFLASH_USE_TIMESTAMP
    if (mod) {
        BLOCKFOOT bf;

        FlashReadBlockFoot(vol->vol_ifc, vol->vol_l2p[lbe] / UFLASH_BLOCK_UNITS, &bf);
        *mod = bf.bf_time;
    }
#endif

    /* Find the highest sequence block. */
    for (lb = UFLASH_ENTRIES; lb < vol->vol_blocks; lb++) {
        p = vol->vol_l2p[lb];
        if (p < vol->vol_blocks) {
            p *= UFLASH_BLOCK_UNITS;
            FlashReadBlockHead(vol->vol_ifc, p / UFLASH_BLOCK_UNITS, &bh);
            if (bh.bh_entblk == lbe) {
                if (seq < bh.bh_entseq) {
                    seq = bh.bh_entseq;
                    lbs = lb;
                }
#ifdef UFLASH_USE_TIMESTAMP
                if (mod) {
                    BLOCKFOOT bf;

                    FlashReadBlockFoot(vol->vol_ifc, p / UFLASH_BLOCK_UNITS, &bf);
                    if (*mod < bf.bf_time) {
                        *mod = bf.bf_time;
                    }
                }
#endif
            }
        }
    }
    /* Size of full blocks. */
    siz = seq * (vol->vol_ifc->sf_unit_size * UFLASH_BLOCK_UNITS - (sizeof(BLOCKHEAD) + sizeof(BLOCKFOOT)));

    /* Number of bytes in the highest sequence block. */
    p = vol->vol_l2p[lbs] * UFLASH_BLOCK_UNITS;
    off = -(int) sizeof(BLOCKFOOT);
    for (i = UFLASH_BLOCK_UNITS; --i >= 0;) {
        int s = (*vol->vol_ifc->sf_used) (vol->vol_ifc, p + i + vol->vol_ifc->sf_rsvbot, off);

        off = 0;
        if (s) {
            siz += s + i * vol->vol_ifc->sf_unit_size - sizeof(BLOCKHEAD);
            break;
        }
    }
    return siz;
}

/*!
 * \brief Find entry by name.
 *
 * This routine can be either used to find an exact match or to find
 * the next entry that starts with the given name.
 *
 * \param vol  Pointer to the volume information structure.
 * \param name Full path name of the requested entry. A leading slash
 *             will be ignored.
 * \param eh   Pointer to the entry header.
 * \param lbs  Logical block to start searching. If set to UFLASH_ENTRIES,
 *             all entry blocks will be searched for an exact match. 
 *             Otherwise the search starts at the specified block for the
 *             next entry that starts with the given name.
 */
static int EntrySearchNext(UFLASHVOLUME * vol, CONST char *name, ENTRYHEAD * eh, blknum_t lbs)
{
    blknum_t lbe;
    blknum_t b;
    int nlen;

    nlen = strlen(name);
    /* Walking through the translation table requires exclusive access. */
    NutEventWait(&vol->vol_mutex, 0);
    for (lbe = lbs == UFLASH_ENTRIES ? 0 : lbs; lbe < UFLASH_ENTRIES; lbe++) {
        b = vol->vol_l2p[lbe];
        if (b < vol->vol_blocks) {
            /* Read the entry header, which contains the path length. */
            if (FlashReadEntry(vol->vol_ifc, b, eh, NULL) == 0) {
                /* If the path lengths are equal, then compare the names. */
                if (eh->eh_nlen == nlen || (lbs != UFLASH_ENTRIES && eh->eh_nlen >= nlen)) {
                    if (FlashBlockCmpEntryName(vol->vol_ifc, b, name, nlen) == 0) {
                        NutEventPost(&vol->vol_mutex);
                        return lbe;
                    }
                }
            }
        }
    }
    NutEventPost(&vol->vol_mutex);
    return UFLASH_ENTRIES;
}

/*!
 * \brief Find existing entry by name.
 *
 * \param vol Pointer to the information structure of a mounted volume.
 */
static int EntrySearch(UFLASHVOLUME * vol, CONST char *name, ENTRYHEAD * eh)
{
    return EntrySearchNext(vol, name, eh, UFLASH_ENTRIES);
}

/*!
 * \brief Erase all data blocks of a given entry.
 *
 * \param vol Pointer to the information structure of a mounted volume.
 */
static int EntryTruncateSeq(UFLASHVOLUME * vol, int lbe)
{
    blknum_t lb;
    BLOCKHEAD bh;

    for (lb = UFLASH_ENTRIES; lb < vol->vol_blocks; lb++) {
        if (vol->vol_l2p[lb] < vol->vol_blocks) {
            FlashReadBlockHead(vol->vol_ifc, vol->vol_l2p[lb], &bh);
            if (bh.bh_entblk == lbe) {
                LogBlkRelease(vol, lb);
            }
        }
    }
    return 0;
}

/*!
 * \brief Truncate an entry.
 *
 * \param vol Pointer to the information structure of a mounted volume.
 */
static int EntryTruncate(UFLASHVOLUME * vol, blknum_t lbe)
{
    BLOCKHEAD bh;
    ENTRYHEAD eh;
    BLOCKFOOT bf;
    blknum_t b;
    blknum_t b_old;
    char *name = NULL;

    EntryTruncateSeq(vol, lbe);

    /* Allocate a new entry block. */
    b = PhysBlkAllocate(vol, 0);
    if (b >= vol->vol_blocks) {
        return -1;
    }
    b_old = vol->vol_l2p[lbe];

    vol->vol_l2p[lbe] = b;

    /* Read the block footer and erase the remaining units. */
    FlashReadBlockFoot(vol->vol_ifc, b, &bf);
    FlashEraseBlockData(vol->vol_ifc, b);

    /* Copy block and entry headers, incrementing the version.
     * Note, that we cannot use a unit copy, because we also must
     * get rid of the data in the first block. */
    FlashReadBlockHead(vol->vol_ifc, b_old, &bh);
    FlashReadEntry(vol->vol_ifc, b_old, &eh, &name);
    FlashWriteBlockHead(vol->vol_ifc, b, &bh);
    FlashWriteEntry(vol->vol_ifc, b, &eh, name);
    FlashUnitCommit(vol->vol_ifc, b, 0);

    /* Update block footer in the last unit. */
    FlashWriteBlockFoot(vol->vol_ifc, b, &bf);
    FlashUnitCommit(vol->vol_ifc, b, UFLASH_BLOCK_UNITS - 1);

    /* Remove the old entry. */
    FlashEraseEntry(vol->vol_ifc, b_old);

    return 0;
}

/*!
 * \brief Create a new entry.
 *
 * \param vol Pointer to the information structure of a mounted volume.
 */
static int EntryCreate(UFLASHVOLUME * vol, blknum_t lbe, ENTRYHEAD * eh, CONST char *name)
{
    BLOCKHEAD bh;
    BLOCKFOOT bf;
    blknum_t b;

    b = vol->vol_l2p[lbe];

    /* Read the footer before erasing the block. */
    FlashReadBlockFoot(vol->vol_ifc, b, &bf);
    FlashEraseBlockData(vol->vol_ifc, b);

    /* Write block header. */
    bh.bh_logblk = lbe;
    bh.bh_version = UFLASH_BLOCK_INVALID;
    bh.bh_entblk = lbe;
    bh.bh_entseq = 0;
    FlashWriteBlockHead(vol->vol_ifc, b, &bh);

    /* Write the entry header and name. */
    FlashWriteEntry(vol->vol_ifc, b, eh, name);
    FlashUnitCommit(vol->vol_ifc, b, 0);

    /* Update block footer. */
    FlashWriteBlockFoot(vol->vol_ifc, b, &bf);
    FlashUnitCommit(vol->vol_ifc, b, UFLASH_BLOCK_UNITS - 1);

    return 0;
}

/*!
 * \brief Remove an entry.
 */
static int UFlashFileRemove(NUTDEVICE * dev, CONST char *name)
{
    blknum_t lbe;
    ENTRYHEAD eh;
    UFLASHVOLUME *vol;

    vol = (UFLASHVOLUME *) dev->dev_dcb;

    while (*name == '/') {
        name++;
    }
    lbe = EntrySearch(vol, name, &eh);
    if (lbe >= UFLASH_ENTRIES || vol->vol_l2p[lbe] > vol->vol_blocks) {
        return -1;
    }
    NutEventWait(&vol->vol_mutex, 0);
    EntryTruncateSeq(vol, lbe);
    LogBlkRelease(vol, lbe);
    NutEventPost(&vol->vol_mutex);

    return 0;
}

/*!
 * \brief Retrieve the status of an entry.
 */
static int UFlashFileStatus(NUTDEVICE * dev, CONST char *name, struct stat *st)
{
    blknum_t lbe;
    ENTRYHEAD eh;
    UFLASHVOLUME *vol;
    uint_fast8_t partial = 0;

    vol = (UFLASHVOLUME *) dev->dev_dcb;

    while (*name == '/') {
        name++;
    }
    lbe = EntrySearch(vol, name, &eh);
    if (lbe >= UFLASH_ENTRIES || vol->vol_l2p[lbe] > vol->vol_blocks) {
        lbe = EntrySearchNext(vol, name, &eh, 0);
        if (lbe >= UFLASH_ENTRIES || vol->vol_l2p[lbe] > vol->vol_blocks) {
            return -1;
        }
        partial = 1;
    }
    memset(st, 0, sizeof(struct stat));
    if (partial) {
        st->st_mode = 1;
#ifdef UFLASH_USE_TIMESTAMP
        EntryScan(vol, lbe, &st->st_mtime);
#endif
    } else {
        NutEventWait(&vol->vol_mutex, 0);
#ifdef UFLASH_USE_TIMESTAMP
        st->st_size = EntryScan(vol, lbe, &st->st_mtime);
#else
        st->st_size = EntryScan(vol, lbe, NULL);
#endif
        NutEventPost(&vol->vol_mutex);
#if UFLASH_MAX_PATH
        st->st_size -= sizeof(ENTRYHEAD) + UFLASH_MAX_PATH;
#else
        st->st_size -= sizeof(ENTRYHEAD) + eh.eh_nlen;
#endif
    }
    return 0;
}

/*!
 * \brief Open a directory.
 *
 * \param dev   Specifies the file system device.
 * \param dpath Full absolute pathname of the directory to open.
 *
 * \return Pointer to a NUTFILE structure if successful or NUTFILE_EOF otherwise.
 */
static NUTFILE *UFlashDirOpen(NUTDEVICE * dev, CONST char *dpath)
{
    NUTFILE *ndp;
    UFLASHFIND *uff;

    ndp = malloc(sizeof(NUTFILE));
    if (ndp) {
        uff = malloc(sizeof(UFLASHFIND));
        if (uff) {
            uff->uff_lbe = 0;
            uff->uff_path = strdup(dpath + (*dpath == '/'));
            if (uff->uff_path) {
                ndp->nf_dev = dev;
                ndp->nf_fcb = uff;

                return ndp;
            }
            free(uff);
        }
        free(ndp);
    }
    return NUTFILE_EOF;
}

/*!
 * \brief Close a directory.
 */
static int UFlashDirClose(NUTFILE * ndp)
{
    UFLASHFIND *uff;

    uff = (UFLASHFIND *) ndp->nf_fcb;
    free(uff->uff_path);
    free(uff);
    free(ndp);

    return 0;
}

/*!
 * \brief Read next entry of a directory.
 */
static int UFlashDirRead(DIR * dir)
{
    struct dirent *ent;
    NUTFILE *ndp;
    NUTDEVICE *dev;
    UFLASHFIND *uff;
    UFLASHVOLUME *vol;
    ENTRYHEAD eh;
    char *name = NULL;
    char *cp;

    ndp = dir->dd_fd;
    uff = (UFLASHFIND *) ndp->nf_fcb;
    dev = ndp->nf_dev;
    vol = (UFLASHVOLUME *) dev->dev_dcb;

    uff->uff_lbe = EntrySearchNext(vol, uff->uff_path, &eh, uff->uff_lbe);
    if (uff->uff_lbe >= UFLASH_ENTRIES) {
        return -1;
    }

    ent = (struct dirent *) dir->dd_buf;
    memset(dir->dd_buf, 0, sizeof(struct dirent));
    FlashReadEntry(vol->vol_ifc, vol->vol_l2p[uff->uff_lbe], &eh, &name);
    cp = strchr(name + strlen(uff->uff_path), '/');
    if (cp) {
        *cp = '\0';
        ent->d_type = 1;
    } else {
        ent->d_type = 0;
    }
    strcpy(ent->d_name, name + strlen(uff->uff_path));
    ent->d_namlen = strlen(name + strlen(uff->uff_path));
    free(name);

    uff->uff_lbe++;

    return 0;
}

/*!
 * \brief Rename an entry.
 *
 * \param dev      Specifies the file system device.
 * \param old_path Old name and path of the file.
 * \param new_path New name and path of the file.
 *
 * \return 0 on success, -1 otherwise.
 */
static int UFlashFileRename(NUTDEVICE * dev, CONST char *old_path, CONST char *new_path)
{
    blknum_t lbe;
    blknum_t b;
    blknum_t b_old;
    uint_fast8_t u;
    ENTRYHEAD eh;
    UFLASHVOLUME *vol;
    BLOCKHEAD bh;
    BLOCKFOOT bf;

    NUTASSERT(dev != NULL);
    NUTASSERT(old_path != NULL);
#if UFLASH_MAX_PATH
    NUTASSERT(strlen(old_path) <= UFLASH_MAX_PATH);
#endif
    NUTASSERT(new_path != NULL);
#if UFLASH_MAX_PATH
    NUTASSERT(strlen(new_path) <= UFLASH_MAX_PATH);
#endif
    vol = (UFLASHVOLUME *) dev->dev_dcb;
    NUTASSERT(vol != NULL);

    vol = (UFLASHVOLUME *) dev->dev_dcb;

    while (*old_path == '/') {
        old_path++;
    }
    while (*new_path == '/') {
        new_path++;
    }
    lbe = EntrySearch(vol, old_path, &eh);
    if (lbe >= UFLASH_ENTRIES || vol->vol_l2p[lbe] > vol->vol_blocks) {
        return -1;
    }
#if UFLASH_MAX_PATH == 0
    if (eh.eh_nlen < strlen(new_path)) {
        return -1;
    }
#endif
    eh.eh_nlen = strlen(new_path);

    /* Exclusive access from here. */
    NutEventWait(&vol->vol_mutex, 0);

    /* Allocate a new entry block. */
    b = PhysBlkAllocate(vol, 0);
    if (b >= vol->vol_blocks) {
        /* No more blocks available. */
        NutEventPost(&vol->vol_mutex);
        return -1;
    }

    /* Replace the physical block in the translation table. */
    b_old = vol->vol_l2p[lbe];
    vol->vol_l2p[lbe] = b;

    /* Keep the footer and erase the pages of the new block, except the 
       first. The first page of unallocated blocks is already erased. */
    FlashReadBlockFoot(vol->vol_ifc, b, &bf);
    FlashEraseBlockData(vol->vol_ifc, b);

    /* Copy all pages of the entry block. */
    for (u = 0; u < UFLASH_BLOCK_UNITS; u++) {
        FlashUnitCopy(vol->vol_ifc, b_old, b, u);
        /* If this is the first unit, write a new version of the header. */
        if (u == 0) {
            /* Separately copy the block header, because this will 
               increment the block's version number. */
            FlashReadBlockHead(vol->vol_ifc, b, &bh);
            FlashWriteBlockHead(vol->vol_ifc, b, &bh);

            /* Fill in the new path name. */
            FlashWriteEntry(vol->vol_ifc, b, &eh, new_path);
        }
        /* If this is the last unit, write back an updated footer. */
        else if (u == UFLASH_BLOCK_UNITS - 1) {
            FlashWriteBlockFoot(vol->vol_ifc, b, &bf);
        }
        /* Commit the unit. */
        FlashUnitCommit(vol->vol_ifc, b, u);
    }

    /* Finally remove the old entry. */
    FlashEraseEntry(vol->vol_ifc, b_old);

    /* Release exclusive access. */
    NutEventPost(&vol->vol_mutex);

    return 0;
}

/*!
 * \brief Retrieve the size of a previously opened UFLASH file.
 *
 * This function is called by the low level size routine of the C runtime 
 * library, using the _NUTDEVICE::dev_size entry.
 *
 * \param nfp Pointer to a \ref _NUTFILE structure, obtained by a 
 *            previous call to UFlashFileOpen().
 *
 * \return Size of the file.
 */
static long UFlashFileSize(NUTFILE * nfp)
{
    NUTASSERT(nfp != NULL);
    NUTASSERT(nfp->nf_fcb != NULL);

    return ((UFLASHENTRY *) nfp->nf_fcb)->ent_size;
}

/*!
 * \brief Open a UFLASH file.
 *
 * This function is called by the low level open routine of the C runtime 
 * library, using the _NUTDEVICE::dev_open entry.
 *
 * \param dev  Specifies the file system device.
 * \param path Pathname of the file to open.
 * \param mode Operation mode.
 * \param acc  File attribute, currently ignored.
 *
 * \return Pointer to a NUTFILE structure if successful or NUTFILE_EOF otherwise.
 */
static NUTFILE *UFlashFileOpen(NUTDEVICE * dev, CONST char *path, int mode, int acc)
{
    int rc = -1;
    NUTFILE *nfp = NUTFILE_EOF;
    UFLASHENTRY *ent;
    UFLASHVOLUME *vol;
    blknum_t lbe;
    ENTRYHEAD eh;
    long datsiz = 0;

    NUTASSERT(dev != NULL);
    NUTASSERT(path != NULL);
    vol = (UFLASHVOLUME *) dev->dev_dcb;
    NUTASSERT(vol != NULL);

    while (*path == '/') {
        path++;
    }
    /* Allocate a private entry information structure. */
    ent = calloc(1, sizeof(UFLASHENTRY));
    if (ent == NULL) {
        return NUTFILE_EOF;
    }
    ent->ent_mode = mode;

    lbe = EntrySearch(vol, path, &eh);

    NutEventWait(&vol->vol_mutex, 0);
    /* 
       ** Entry exists. 
     */
    if (lbe < UFLASH_ENTRIES) {
        if ((mode & (_O_CREAT | _O_EXCL)) == (_O_CREAT | _O_EXCL)) {
            errno = EEXIST;
        } else {
            /* Truncate. */
            if ((mode & _O_TRUNC) == _O_TRUNC) {
                rc = EntryTruncate(vol, lbe);
            }
            /* Do not truncate. */
            else {
                rc = 0;
                datsiz = EntryScan(vol, lbe, NULL);
#if UFLASH_MAX_PATH
                datsiz -= sizeof(ENTRYHEAD) + UFLASH_MAX_PATH;
#else
                datsiz -= sizeof(ENTRYHEAD) + eh.eh_nlen;
#endif
            }
        }
    }

    /*
       ** Entry doesn't exist.
     */
    else {
        if ((mode & _O_CREAT) == _O_CREAT) {
            lbe = LogBlkAllocate(vol, 0);
            if (lbe < UFLASH_ENTRIES) {
                eh.eh_nlen = strlen(path);
                eh.eh_attr = 0;
                rc = EntryCreate(vol, lbe, &eh, path);
            }
        } else {
            errno = ENOENT;
        }
    }

    if (rc) {
        free(ent);
    } else {
        ent->ent_attr = eh.eh_attr;
        ent->ent_namlen = eh.eh_nlen;
#if UFLASH_MAX_PATH
        ent->ent_eoff = sizeof(ENTRYHEAD) + UFLASH_MAX_PATH;
#else
        ent->ent_eoff = sizeof(ENTRYHEAD) + eh.eh_nlen;
#endif
        ent->ent_id = lbe;
        ent->ent_size = datsiz;

        /* Allocate a file information structure. */
        nfp = malloc(sizeof(NUTFILE));
        if (nfp == NULL) {
            free(ent);
            NutEventPost(&vol->vol_mutex);
            return NUTFILE_EOF;
        }
        nfp->nf_next = NULL;
        nfp->nf_dev = dev;
        nfp->nf_fcb = ent;

        if ((mode & _O_APPEND) == _O_APPEND) {
            ent->ent_bidx = UFLASH_BLOCK_INVALID;
            EntryPosSet(nfp, datsiz);
        } else {
            ent->ent_bidx = lbe;
            EntryPosSet(nfp, 0);
        }
    }
    NutEventPost(&vol->vol_mutex);
    return nfp;
}

/*!
 * \brief Close a UFLASH file.
 *
 * \param nfp Pointer to a \ref NUTFILE structure, obtained by a previous 
 *            call to UFlashFileOpen().
 *
 * \return Always 0.
 */
static int UFlashFileClose(NUTFILE * nfp)
{
    NUTASSERT(nfp != NULL);
    NUTASSERT(nfp->nf_dev != NULL);
    NUTASSERT(nfp->nf_fcb != NULL);

    free(nfp->nf_fcb);
    free(nfp);

    return 0;
}

/*!
 * \brief Read data from a UFLASH file.
 *
 * \param nfp  Pointer to a \ref NUTFILE structure, obtained by a previous 
 *             call to UFlashFileOpen().
 * \param data Pointer to the data buffer to fill.
 * \param size Maximum number of bytes to read.
 *
 * \return The number of bytes actually read. A return value of -1 indicates 
 *         an error.
 */
static int UFlashFileRead(NUTFILE * nfp, void *data, int size)
{
    int u;
    int step;
    int rc;
    uint8_t *dptr;
    UFLASHENTRY *ent;
    UFLASHVOLUME *vol;

    /* Ignore input flush. */
    if (data == NULL || size == 0) {
        return 0;
    }
    dptr = (uint8_t *) data;

    NUTASSERT(nfp != NULL);
    NUTASSERT(nfp->nf_dev != NULL);
    ent = (UFLASHENTRY *) nfp->nf_fcb;
    NUTASSERT(ent != NULL);
    vol = (UFLASHVOLUME *) nfp->nf_dev->dev_dcb;
    NUTASSERT(vol != NULL);

    /* Do not read beyond the file end. */
    if (size > (int) (ent->ent_size - ent->ent_pos)) {
        size = (int) (ent->ent_size - ent->ent_pos);
    }

    /*
       ** Loop until we read the requested size.
     */
    for (rc = 0; rc < size;) {
        /* Is the current logical block number still valid? */
        if (ent->ent_bidx >= vol->vol_blocks) {
            /* If not, seek for the right block. */
            ent->ent_bidx = EntrySeek(vol, ent->ent_id, ent->ent_sidx);
            if (ent->ent_bidx >= vol->vol_blocks) {
                /* Seek failed. */
                errno = EIO;
                return -1;
            }
        }
        /* Read unit by unit. */
        for (u = ent->ent_uidx; rc < size && u < UFLASH_BLOCK_UNITS; u++) {
            NutEventWait(&vol->vol_mutex, 0);
            step = FlashUnitRead(vol->vol_ifc, vol->vol_l2p[ent->ent_bidx], u, ent->ent_upos, dptr, size - rc);
            NutEventPost(&vol->vol_mutex);
            if (step < 0) {
                /* Read failed. */
                errno = EIO;
                return -1;
            }
            /* Increment pointers and indices. */
            rc += step;
            dptr += step;
            EntryPosInc(nfp, step);
        }
    }
    return rc;
}

/*!
 * \brief Write data to a UFLASH file.
 *
 * \param nfp    Pointer to a \ref NUTFILE structure, obtained by a previous 
 *               call to UFlashFileOpen().
 * \param buffer Pointer to the data to be written. If zero, then the
 *               output buffer will be flushed.
 * \param len    Number of bytes to write.
 *
 * \return The number of bytes written. A return value of -1 indicates an 
 *         error.
 */
static int UFlashFileWrite(NUTFILE * nfp, CONST void *data, int len)
{
    int u;
    blknum_t b;
    blknum_t b_old;
    int step;
    int siz = len;
    uint8_t *dptr;
    BLOCKHEAD bh;
    BLOCKFOOT bf;
    UFLASHENTRY *ent;
    UFLASHVOLUME *vol;

    /* Ignore output flush. */
    if (data == NULL || len == 0) {
        return 0;
    }
    dptr = (uint8_t *) data;

    /* Sanity check. */
    NUTASSERT(nfp != NULL);
    NUTASSERT(nfp->nf_fcb != NULL);
    NUTASSERT(nfp->nf_dev != NULL);
    NUTASSERT(nfp->nf_dev->dev_dcb != NULL);

    ent = (UFLASHENTRY *) nfp->nf_fcb;
    vol = (UFLASHVOLUME *) nfp->nf_dev->dev_dcb;

    /*
       ** Loop until all data has been written.
     */
    while (len) {
        /* Is the current logical block number still valid? */
        if (ent->ent_bidx >= vol->vol_blocks) {
            /* If not, seek for the right block. The routine will return an
               ** invalid block number, if we need a new block. */
            ent->ent_bidx = EntrySeek(vol, ent->ent_id, ent->ent_sidx);
        }

        /*
           ** If the current logical block exists, then we reallocate a
           ** physical block.
         */
        NutEventWait(&vol->vol_mutex, 0);
        if (ent->ent_bidx < vol->vol_blocks) {
            b_old = LogBlkReallocate(vol, ent->ent_bidx);
            if (b_old >= vol->vol_blocks) {
                /* No more blocks available. */
                NutEventPost(&vol->vol_mutex);
                return -1;
            }
            /* Read header of old block and increment the version number. */
            FlashReadBlockHead(vol->vol_ifc, b_old, &bh);
        }

        /*
           ** If the current logical block doesn't exists, then we allocate
           ** a new one.
         */
        else {
            ent->ent_bidx = LogBlkAllocate(vol, UFLASH_ENTRIES);
            if (ent->ent_bidx >= vol->vol_blocks) {
                /* No more blocks available. */
                NutEventPost(&vol->vol_mutex);
                return -1;
            }
            /* No reallocation, old and new are the same block. */
            b_old = vol->vol_l2p[ent->ent_bidx];
            /* Initial block header. */
            bh.bh_logblk = ent->ent_bidx;
            bh.bh_entblk = ent->ent_id;
            bh.bh_entseq = ent->ent_sidx;
            bh.bh_version = UFLASH_BLOCK_INVALID;
        }
        /* Current physical block. */
        b = vol->vol_l2p[ent->ent_bidx];

        /* Read the block footer. */
        FlashReadBlockFoot(vol->vol_ifc, b, &bf);
        /* The first unit had been erased when the block was released.
           ** Now erase the remaining units. */
        FlashEraseBlockData(vol->vol_ifc, b);

        /* 
           ** Write unit by unit.
         */
        for (u = 0; u < UFLASH_BLOCK_UNITS; u++) {
            /* Make a copy of the old unit. If old and new are
               ** the same, this won't hurt. */
            FlashUnitCopy(vol->vol_ifc, b_old, b, u);
            /* Write the block header to the first unit. */
            if (u == 0) {
                FlashWriteBlockHead(vol->vol_ifc, b, &bh);
            }
            /* If this is the current unit to write to, fill it with data. */
            if (len && u == ent->ent_uidx) {
                step = FlashUnitWrite(vol->vol_ifc, b, u, ent->ent_upos, dptr, len);
                /* Increment pointers and indices. */
                len -= step;
                dptr += step;
                EntryPosInc(nfp, step);
                if (ent->ent_size < ent->ent_pos) {
                    ent->ent_size = ent->ent_pos;
                }
            }
            /* Write the block footer to the last unit. */
            if (u == UFLASH_BLOCK_UNITS - 1) {
                FlashWriteBlockFoot(vol->vol_ifc, b, &bf);
            }
            FlashUnitCommit(vol->vol_ifc, b, u);
        }
        /* If we reallocated the block, then release the old one now.
           ** It is essential to do this after having successfully written
           ** the new block. If the new block with a higher version number
           ** remains incomplete, then the old version still exists and the
           ** incomplete block will be removed during mounting. */
        if (b != b_old) {
            FlashEraseEntry(vol->vol_ifc, b_old);
        }
        NutEventPost(&vol->vol_mutex);
    }
    return siz;
}

#ifdef __HARVARD_ARCH__
/*! 
 * \brief Write data from program space to a file.
 *
 * This function is not yet implemented and will always return -1.
 *
 * Similar to UFlashFileWrite() except that the data is located in 
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
static int UFlashFileWrite_P(NUTFILE * nfp, PGM_P buffer, int len)
{
    return -1;
}
#endif

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
static int UFlashFileSeek(NUTFILE * nfp, long *pos, int whence)
{
    int rc = 0;
    long npos;
    UFLASHENTRY *ent;

    NUTASSERT(nfp != NULL);
    NUTASSERT(nfp->nf_fcb != NULL);

    ent = (UFLASHENTRY *) nfp->nf_fcb;

    NUTASSERT(pos != NULL);
    npos = *pos;

    switch (whence) {
    case SEEK_CUR:
        /* Relative to current position. */
        npos += ent->ent_pos;
        break;
    case SEEK_END:
        /* Relative to file end. */
        npos += ent->ent_size;
        break;
    }

    /* Make sure that we are within limits. */
    if (npos < 0 || npos > (long) ent->ent_size) {
        errno = EINVAL;
        rc = -1;
    } else {
        EntryPosSet(nfp, npos);
        *pos = npos;
    }
    return rc;
}

/*!
 * \brief Mount a UFLASH volume.
 *
 * \param dev Specifies the file system device.
 *
 * \return 0 on success or -1 in case of an error.
 */
static int UFlashMount(NUTDEVICE * dev)
{
    int b;
    BLOCKHEAD bh;
    UFLASHVOLUME *vol;

    NUTASSERT(dev != NULL);
    NUTASSERT(dev->dev_icb != NULL);
    NUTASSERT(dev->dev_dcb == NULL);

    /* Allocate the volume information structure  */
    vol = calloc(1, sizeof(UFLASHVOLUME));
    if (vol == NULL) {
        return -1;
    }
    dev->dev_dcb = vol;
    vol->vol_ifc = (NUTSERIALFLASH *) dev->dev_icb;

    /* Determine the total number of blocks. Honor any reserved units. */
    vol->vol_blocks =
        (blknum_t) ((vol->vol_ifc->sf_units - vol->vol_ifc->sf_rsvbot - vol->vol_ifc->sf_rsvtop) / UFLASH_BLOCK_UNITS);
    if (vol->vol_blocks > UFLASH_MAX_BLOCKS) {
        vol->vol_blocks = UFLASH_MAX_BLOCKS;
    }

    /* Allocate the block translation table. */
    vol->vol_l2p = malloc(vol->vol_blocks * sizeof(blknum_t));
    if (vol->vol_l2p == NULL) {
        free(vol);
        return -1;
    }
    memset(vol->vol_l2p, 0xff, vol->vol_blocks * sizeof(blknum_t));

    memset(min_used, 0xff, sizeof(min_used));

    /*
       ** Fill the block translation table.
     */
    for (b = 0; b < vol->vol_blocks; b++) {
        FlashReadBlockHead(vol->vol_ifc, b, &bh);
        /* Check if this is a logical block. */
        if (bh.bh_logblk < vol->vol_blocks) {
            /* Verify the CRC. */
            if (FlashCheckBlock(vol->vol_ifc, b)) {
                /* Bad block, remove it. */
                FlashEraseEntry(vol->vol_ifc, b);
                /* Mark the volume fixed. */
                vol->vol_attrib |= UFLASH_VOLF_FIXED;
            } else {
                /* Check if we already found another version. */
                if (vol->vol_l2p[bh.bh_logblk] < vol->vol_blocks) {
                    BLOCKHEAD bho;

                    /* Read the previous entry. */
                    FlashReadBlockHead(vol->vol_ifc, vol->vol_l2p[bh.bh_logblk], &bho);
                    /* Compare versions. */
                    if (bho.bh_version > bh.bh_version) {
                        if (bho.bh_version - bh.bh_version > vol->vol_blocks) {
                            /* The current version overflowed. */
                            FlashEraseEntry(vol->vol_ifc, b);
                        } else {
                            FlashEraseEntry(vol->vol_ifc, vol->vol_l2p[bh.bh_logblk]);
                            vol->vol_l2p[bh.bh_logblk] = b;
                        }
                    } else if (bh.bh_version - bho.bh_version < vol->vol_blocks) {
                        /* The current version is higher. */
                        FlashEraseEntry(vol->vol_ifc, b);
                    } else {
                        FlashEraseEntry(vol->vol_ifc, vol->vol_l2p[bh.bh_logblk]);
                        vol->vol_l2p[bh.bh_logblk] = b;
                    }
                    /* Mark the volume fixed. */
                    vol->vol_attrib |= UFLASH_VOLF_FIXED;
                } else {
                    /* First version of this block. */
                    vol->vol_l2p[bh.bh_logblk] = b;
                }
            }
        }
    }
    /* Initialize the mutex. */
    NutEventPost(&vol->vol_mutex);
    return 0;
}

/*!
 * \brief Unmount a UFLASH volume.
 *
 * \param dev Specifies the file system device.
 *
 * \return Always 0.
 */
static int UFlashUnmount(NUTDEVICE * dev)
{
    UFLASHVOLUME *vol;

    /* Sanity check. */
    NUTASSERT(dev != NULL);

    /* Get the volume information structure. */
    NUTASSERT(dev->dev_dcb != NULL);
    vol = (UFLASHVOLUME *) dev->dev_dcb;

    /* Release allocated heap space. */
    NUTASSERT(vol->vol_l2p != NULL);
    free(vol->vol_l2p);
    free(vol);

    /* Mark the file system driver unmounted. */
    dev->dev_dcb = NULL;

    return 0;
}

/*!
 * \brief Attach an UFLASH file system to a serial flash device.
 *
 * This function will initialize the hardware and mount the
 * volume.
 *
 * \param dev Specifies the file system device.
 * \param sfi Specifies the serial flash interface.
 * \param bus Specifies the SPI bus driver.
 *
 * \return 0 on success, -1 on errors.
 */
int UFlashAttach(NUTDEVICE * dev, NUTSERIALFLASH * sfi, NUTSPIBUS * bus)
{
    /* Sanity checks. */
    NUTASSERT(dev != NULL);
    NUTASSERT(sfi != NULL);
    NUTASSERT(sfi->sf_node != NULL);
    NUTASSERT(sfi->sf_init != NULL);
    NUTASSERT(bus != NULL);
    NUTASSERT(bus->bus_initnode != NULL);

    sfi->sf_node->node_bus = bus;
    if ((*bus->bus_initnode) (sfi->sf_node)) {
        return -1;
    }
    NutEventPost(&sfi->sf_node->node_bus->bus_mutex);

    if ((*sfi->sf_init) (sfi) == 0) {
        dev->dev_icb = (void *) sfi;
        if (UFlashMount(dev) == 0) {
            return 0;
        }
        dev->dev_icb = NULL;
    }
    return -1;
}

/*!
 * \brief Detach an UFLASH file system from a serial flash device.
 *
 * This function will unmount the volume and release all occupied resources.
 *
 * \param dev Specifies the file system device.
 *
 * \return 0 on success, -1 on errors.
 */
void UFlashDetach(NUTDEVICE * dev)
{
    NUTSERIALFLASH *sfi;

    UFlashUnmount(dev);

    /* Release and detach the serial flash interface. */
    sfi = (NUTSERIALFLASH *) dev->dev_icb;
    NUTASSERT(sfi != NULL);
    NUTASSERT(sfi->sf_exit != NULL);
    (*sfi->sf_exit) (sfi);
    dev->dev_icb = NULL;
}

/*!
 * \brief Format an UFLASH volume.
 *
 * Formatting an UFLASH volume is easy: All blocks are simply erased.
 * Therefore, formatting is required only to remove existing contents,
 * which is incompatible with the current format options. The caller 
 * must make sure, that the volume is not mounted.
 *
 * \param dev Specifies the file system device.
 * \param sfi Specifies the serial flash interface.
 * \param bus Specifies the SPI bus driver.
 *
 * \return 0 on success, -1 on errors.
 */
int UFlashFormat(NUTDEVICE * dev, NUTSERIALFLASH * sfi, NUTSPIBUS * bus)
{
    int rc;
    blknum_t n;

    /* Sanity checks. */
    NUTASSERT(dev != NULL);
    NUTASSERT(sfi != NULL);
    NUTASSERT(dev->dev_dcb == NULL);

    sfi->sf_node->node_bus = bus;
    if ((*sfi->sf_node->node_bus->bus_initnode) (sfi->sf_node)) {
        return -1;
    }
    NutEventPost(&sfi->sf_node->node_bus->bus_mutex);

    rc = (*sfi->sf_init) (sfi);
    if (rc == 0) {
        n = (blknum_t) ((sfi->sf_units - sfi->sf_rsvbot - sfi->sf_rsvtop) / UFLASH_BLOCK_UNITS);
        if (n > UFLASH_MAX_BLOCKS) {
            n = UFLASH_MAX_BLOCKS;
        }
        FlashEraseBlocks(sfi, n);
        (*sfi->sf_exit) (sfi);
    }
    return 0;
}

/*!
 * \brief UFLASH specific functions.
 *
 * \param dev  Identifies the file system device that receives the
 *             control function.
 * \param req  Requested control function. May be set to one of the
 *             following constants:
 *             - FS_STATUS
 *             - FS_DIR_CREATE, ignored
 *             - FS_DIR_REMOVE, ignored
 *             - FS_DIR_OPEN
 *             - FS_DIR_CLOSE
 *             - FS_DIR_READ
 *             - FS_FILE_DELETE
 *             - FS_FILE_SEEK
 *             - FS_RENAME
 *             - FS_VOL_MOUNT
 *             - FS_VOL_UNMOUNT
 * \param conf Points to a buffer that contains any data required for
 *             the given control function or receives data from that
 *             function.
 *
 * \return 0 on success, -1 otherwise.
 */
static int UFlashIOCtl(NUTDEVICE * dev, int req, void *conf)
{
    int rc = -1;

    switch (req) {
    case FS_STATUS:
        NUTASSERT(conf != NULL);
        {
            FSCP_STATUS *par = (FSCP_STATUS *) conf;
            rc = UFlashFileStatus(dev, par->par_path, par->par_stp);
        }
        break;
    case FS_DIR_CREATE:
        rc = 0;
        break;
    case FS_DIR_REMOVE:
        rc = 0;
        break;
    case FS_DIR_OPEN:
        /* Open a directory for reading entries. */
        NUTASSERT(conf != NULL);
        {
            DIR *dir = (DIR *) conf;

            if ((dir->dd_fd = UFlashDirOpen(dev, dir->dd_buf)) != NUTFILE_EOF) {
                rc = 0;
            }
        }
        break;
    case FS_DIR_CLOSE:
        NUTASSERT(conf != NULL);
        rc = UFlashDirClose(((DIR *) conf)->dd_fd);
        break;
    case FS_DIR_READ:
        rc = UFlashDirRead((DIR *) conf);
        break;
    case FS_FILE_DELETE:
        rc = UFlashFileRemove(dev, (char *) conf);
        break;
    case FS_FILE_SEEK:
        NUTASSERT(conf != NULL);
        UFlashFileSeek((NUTFILE *) ((IOCTL_ARG3 *) conf)->arg1, /* */
                       (long *) ((IOCTL_ARG3 *) conf)->arg2,    /* */
                       (int) ((IOCTL_ARG3 *) conf)->arg3);
        break;
    case FS_RENAME:
        /* Rename an existing file. */
        NUTASSERT(conf != NULL);
        {
            FSCP_RENAME *par = (FSCP_RENAME *) conf;
            rc = UFlashFileRename(dev, par->par_old, par->par_new);
        }
        break;
    case FS_VOL_MOUNT:
        /* Mount a volume. */
        rc = UFlashMount(dev);
        break;
    case FS_VOL_UNMOUNT:
        /* Unmount a volume. */
        rc = UFlashUnmount(dev);
        break;
    }
    return rc;
}

/*!
 * \brief Initialize the UFLASH device file system driver.
 *
 * This routine is called during device registration.
 *
 * \param dev Specifies the file system device.
 *
 * \return Zero on success. Otherwise an error code is returned.
 */
static int UFlashInit(NUTDEVICE * dev)
{
    return 0;
}

/*!
 * \brief UFLASH0 file system device.
 */
NUTDEVICE devUFlash0 = {
    0,                          /*!< \brief Pointer to next device, dev_next. */
    {'U', 'F', 'L', 'A', 'S', 'H', '0', 0, 0}
    ,                           /*!< \brief Unique device name, dev_name. */
    IFTYP_FS,                   /*!< \brief Type of device, dev_type. Obsolete. */
    0,                          /*!< \brief Chip select, dev_base. */
    0,                          /*!< \brief First interrupt number, dev_irq. Unused. */
    NULL,                       /*!< \brief Attached serial flash interface, dev_icb. */
    NULL,                       /*!< \brief Mounted volume information, dev_dcb. */
    UFlashInit,                 /*!< \brief Driver initialization routine, dev_init. */
    UFlashIOCtl,                /*!< \brief Driver specific control function, dev_ioctl. */
    UFlashFileRead,             /*!< \brief Read data from a file, dev_read. */
    UFlashFileWrite,            /*!< \brief Write data to a file, dev_write. */
#ifdef __HARVARD_ARCH__
    UFlashFileWrite_P,          /*!< \brief Write data from program space to a file, dev_write_P. */
#endif
    UFlashFileOpen,             /*!< \brief Open a file, dev_open. */
    UFlashFileClose,            /*!< \brief Close a file, dev_close. */
    UFlashFileSize              /*!< \brief Return file size, dev_size. */
};

/*!
 * \brief UFLASH1 file system device.
 */
NUTDEVICE devUFlash1 = {
    0,                          /*!< \brief Pointer to next device, dev_next. */
    {'U', 'F', 'L', 'A', 'S', 'H', '1', 0, 0}
    ,                           /*!< \brief Unique device name, dev_name. */
    IFTYP_FS,                   /*!< \brief Type of device, dev_type. Obsolete. */
    1,                          /*!< \brief Chip select, dev_base. */
    0,                          /*!< \brief First interrupt number, dev_irq. Unused. */
    NULL,                       /*!< \brief Attached serial flash interface, dev_icb. */
    NULL,                       /*!< \brief Mounted volume information, dev_dcb. */
    UFlashInit,                 /*!< \brief Driver initialization routine, dev_init. */
    UFlashIOCtl,                /*!< \brief Driver specific control function, dev_ioctl. */
    UFlashFileRead,             /*!< \brief Read data from a file, dev_read. */
    UFlashFileWrite,            /*!< \brief Write data to a file, dev_write. */
#ifdef __HARVARD_ARCH__
    UFlashFileWrite_P,          /*!< \brief Write data from program space to a file, dev_write_P. */
#endif
    UFlashFileOpen,             /*!< \brief Open a file, dev_open. */
    UFlashFileClose,            /*!< \brief Close a file, dev_close. */
    UFlashFileSize              /*!< \brief Return file size, dev_size. */
};

/*@}*/
