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
 * \file fs/phatfs.c
 * \brief PHAT File System.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.15  2009/02/13 14:52:05  haraldkipp
 * Include memdebug.h for heap management debugging support.
 *
 * Revision 1.14  2008/08/25 14:26:32  haraldkipp
 * Append to existing files failed when file size was larger than one cluster.
 *
 * Revision 1.13  2008/08/22 16:22:22  olereinhardt
 * Check if filesystem is mounted when opening a file.
 * Prevents a system lockup because of a nullpointer access
 *
 * Revision 1.12  2008/08/11 06:59:42  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.11  2008/04/02 09:39:29  haraldkipp
 * Fixed another PHAT file pointer bug.
 *
 * Revision 1.10  2008/04/01 10:14:58  haraldkipp
 * Fixes bug #1903303. Still not fully correct, because fopen(name, "a+")
 * should set the read pointer at position 0 while all writes should go
 * to the end of the file. But setting the write pointer at the end will
 * work for now.
 *
 * Revision 1.9  2006/10/08 16:42:56  haraldkipp
 * Not optimal, but simple and reliable exclusive access implemented.
 * Fixes bug #1486539. Furthermore, bug #1567790, which had been rejected,
 * had been reported correctly and is now fixed.
 *
 * Revision 1.8  2006/07/11 12:20:57  haraldkipp
 * Added mutual exclusion protection during flush.
 * Honor Nut/OS file flushing, which uses a NULL pointer on read/write.
 *
 * Revision 1.7  2006/07/05 16:02:23  haraldkipp
 * Typically Nut/OS doesn't check parameters, but this one is missed often,
 * closing a file which points to NUTFILE_EOF.
 *
 * Revision 1.6  2006/06/18 16:38:28  haraldkipp
 * No need to set errno after malloc failed.
 * Support for long filenames (VFAT) added.
 * New function PhatDirReleaseChain() simplifies code.
 * Fixed positioning bug, which caused several problems like limiting
 * directories to one cluster.
 *
 * Revision 1.5  2006/05/15 11:47:18  haraldkipp
 * Added support for file seek.
 *
 * Revision 1.4  2006/03/02 19:59:05  haraldkipp
 * Added implementation of dev_size makes _filelength() work, which in turn
 * enables the use of these file systems in pro/httpd.c.
 *
 * Revision 1.3  2006/02/23 15:45:22  haraldkipp
 * PHAT file system now supports configurable number of sector buffers.
 * This dramatically increased write rates of no-name cards.
 * AVR compile errors corrected.
 *
 * Revision 1.2  2006/01/22 17:42:08  haraldkipp
 * Now file delete requests fail if used on directory entries.
 *
 * Revision 1.1  2006/01/05 16:31:39  haraldkipp
 * First check-in.
 *
 *
 * \endverbatim
 */

#include <fs/fs.h>

#include <dirent.h>

#include <dev/blockdev.h>

#include <sys/event.h>

#include <fs/phatfs.h>
#include <fs/phatvol.h>
#include <fs/phatio.h>
#include <fs/phatutil.h>
#include <fs/phatdir.h>

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <memdebug.h>

#if 0
/* Use for local debugging. */
#define NUTDEBUG
#include <stdio.h>
#include <fs/phatdbg.h>
#endif

#ifndef SEEK_SET
#  define SEEK_SET        0     /* Seek from beginning of file.  */
#  define SEEK_CUR        1     /* Seek from current position.  */
#  define SEEK_END        2     /* Set file pointer to EOF plus "offset" */
#endif

/*!
 * \addtogroup xgPhatFs
 */
/*@{*/

/*!
 * \brief Search free cluster.
 *
 * \param dev   Specifies the file system device.
 * \param first First cluster to check. Must not be lower than 2.
 * \param last  Last cluster to check.
 *
 * \return Number of the free cluster found. If an error occured,  the
 *         returned value will be lower than the first cluster searched.
 */
static uint32_t SearchFreeCluster(NUTDEVICE * dev, uint32_t first, uint32_t last)
{
    int rc = -1;
    uint32_t clust;
    uint32_t link = 1;
    PHATVOL *vol = (PHATVOL *) dev->dev_dcb;

    if (vol->vol_type == 32) {
        for (clust = first; clust < last; clust++) {
            if ((rc = Phat32GetClusterLink(dev, clust, &link)) < 0 || link == 0) {
                break;
            }
        }
    } else if (vol->vol_type == 16) {
        for (clust = first; clust < last; clust++) {
            if ((rc = Phat16GetClusterLink(dev, clust, &link)) < 0 || link == 0) {
                break;
            }
        }
    } else {
        for (clust = first; clust < last; clust++) {
            if ((rc = Phat12GetClusterLink(dev, clust, &link)) < 0 || link == 0) {
                break;
            }
        }
    }

    if (rc || link) {
        return 0;
    }
    return clust;
}

/*!
 * \brief Allocate a new cluster.
 *
 * \param dev Specifies the file system device.
 *
 * \return Number of the allocated cluster, which is not lower than 2.
 *         Any value lower than 2 indicates an error.
 */
static uint32_t AllocCluster(NUTDEVICE * dev)
{
    uint32_t clust;
    PHATVOL *vol = (PHATVOL *) dev->dev_dcb;

    /*
     * If the hint for the next free cluster is invalid, start from the beginning,
     */
    if (vol->vol_nxtfree < 2 || vol->vol_nxtfree >= vol->vol_last_clust) {
        vol->vol_nxtfree = 2;
    }
    if ((clust = SearchFreeCluster(dev, vol->vol_nxtfree, vol->vol_last_clust)) < 2) {
        if ((clust = SearchFreeCluster(dev, 2, vol->vol_nxtfree)) < 2) {
            vol->vol_nxtfree = 0;
            errno = ENOSPC;
            return 0;
        }
    }
    vol->vol_nxtfree = clust;

    return clust;
}

/*!
 * \brief Allocate the first cluster of a file.
 *
 * \param nfp The file for which a cluster is allocated.
 *
 * \return Number of the allocated cluster, which is not lower than 2.
 *         Any value lower than 2 indicates an error.
 */
uint32_t AllocFirstCluster(NUTFILE * nfp)
{
    uint32_t clust;
    NUTDEVICE *dev = nfp->nf_dev;
    PHATVOL *vol = (PHATVOL *) dev->dev_dcb;
    PHATFILE *fcb = nfp->nf_fcb;

    if ((clust = AllocCluster(dev)) < 2) {
        return 0;
    }

    /* Set the pointer to the first cluster in out directory entry. */
    fcb->f_dirent.dent_clusthi = (uint16_t) (clust >> 16);
    fcb->f_dirent.dent_clust = (uint16_t) clust;
    fcb->f_de_dirty = 1;

    /* The first cluster entry will be set to EOC. */
    if (vol->vol_type == 32) {
        if (Phat32SetClusterLink(dev, clust, PHAT32CMASK)) {
            return 0;
        }
    } else if (vol->vol_type == 16) {
        if (Phat16SetClusterLink(dev, clust, PHAT16CMASK)) {
            return 0;
        }
    } else if (Phat12SetClusterLink(dev, clust, PHAT12CMASK)) {
        return 0;
    }
    vol->vol_numfree--;

    return clust;
}

/*!
 * \brief Allocate the next cluster of a file.
 *
 * \param  The file for which a cluster is allocated.
 *
 * \return Number of the allocated cluster, which is not lower than 2.
 *         Any value lower than 2 indicates an error.
 */
static uint32_t AllocNextCluster(NUTFILE * nfp)
{
    uint32_t clust;
    NUTDEVICE *dev = nfp->nf_dev;
    PHATFILE *fcb = nfp->nf_fcb;
    PHATVOL *vol = (PHATVOL *) dev->dev_dcb;

    /* Allocate a free cluster. */
    if ((clust = AllocCluster(dev)) < 2) {
        return 0;
    }

    /* Link the previous cluster to the new one and set
     * the entry of the new one to EOC.
     */
    if (vol->vol_type == 32) {
        if (Phat32SetClusterLink(dev, fcb->f_clust, clust)) {
            return 0;
        }
        if (Phat32SetClusterLink(dev, clust, PHAT32CMASK)) {
            return 0;
        }
    } else if (vol->vol_type == 16) {
        if (Phat16SetClusterLink(dev, fcb->f_clust, clust)) {
            return 0;
        }
        if (Phat16SetClusterLink(dev, clust, PHAT16CMASK)) {
            return 0;
        }
    } else if (Phat12SetClusterLink(dev, fcb->f_clust, clust)) {
        return 0;
    } else if (Phat12SetClusterLink(dev, clust, PHAT12CMASK)) {
        return 0;
    }
    vol->vol_numfree--;

    return clust;
}

/*!
 * \brief Flush file buffers.
 *
 * \param nfp Specifies the file.
 *
 * \return 0 on success, -1 otherwise.
 */
static int PhatFileFlush(NUTFILE * nfp)
{
    int rc;
    NUTDEVICE *dev = nfp->nf_dev;
    PHATVOL *vol = (PHATVOL *) dev->dev_dcb;

    /* Update the file's directory entry. */
    if ((rc = PhatDirEntryUpdate(nfp)) == 0) {
        /* Gain mutex access. */
        NutEventWait(&vol->vol_iomutex, 0);
        /* Flush sector buffers. */
        rc = PhatSectorFlush(nfp->nf_dev, -1);
        /* Release mutex access. */
        NutEventPost(&vol->vol_iomutex);
    }
    return rc;
}

/*!
 * \brief Close a file.
 *
 * \param nfp File descriptor.
 *
 * \return 0 on success, -1 otherwise.
 */
int PhatFileClose(NUTFILE * nfp)
{
    int rc;

    if (nfp == NULL || nfp == NUTFILE_EOF) {
        errno = EBADF;
        return -1;
    }
#ifdef NUTDEBUG
    PhatDbgFileInfo(stdout, "Close file", (PHATFILE *) nfp->nf_fcb);
#endif
    rc = PhatFileFlush(nfp);
    if (nfp->nf_fcb) {
        free(nfp->nf_fcb);
    }
    free(nfp);

    return rc;
}

/*!
 * \brief Open a file.
 *
 * This function is called by the low level open routine of the C runtime 
 * library, using the _NUTDEVICE::dev_open entry.
 *
 * \param dev  Specifies the file system device.
 * \param path Pathname of the file to open. If the last character is a
 *             slash, then a directory will be opened.
 * \param mode Operation mode.
 * \param acc  File attribute.
 *
 * \return Pointer to a NUTFILE structure if successful or NUTFILE_EOF otherwise.
 *
 * \bug Append mode not working as expected.
 */
NUTFILE *PhatFileOpen(NUTDEVICE * dev, CONST char *path, int mode, int acc)
{
    NUTFILE *nfp = NUTFILE_EOF;
    NUTFILE *ndp = NUTFILE_EOF;
    PHATFILE *ffcb;
    PHATFILE *dfcb;
    PHATFIND *srch;
    CONST char *fname;

    /* Open the parent directory and return the basename. */
    if ((ndp = PhatDirOpenParent(dev, path, &fname)) == NUTFILE_EOF) {
        return NUTFILE_EOF;
    }

    /*
     * We successfully opened the directory. If no file name had been specified,
     * then the caller wants to open the directory itself. In this case, simply
     * return the NUTFILE for it.
     */
    dfcb = ndp->nf_fcb;
    if (*fname == 0) {
        dfcb->f_mode = mode;
        return ndp;
    }

    /*
     * Allocate a file information and a temporary search structure.
     */
    nfp = malloc(sizeof(NUTFILE));
    ffcb = malloc(sizeof(PHATFILE));
    srch = malloc(sizeof(PHATFIND));
    if (nfp == NULL || ffcb == NULL || srch == NULL) {
        PhatFileClose(ndp);
        if (nfp) {
            free(nfp);
        }
        if (ffcb) {
            free(ffcb);
        }
        if (srch) {
            free(srch);
        }
        return NUTFILE_EOF;
    }

    memset(ffcb, 0, sizeof(PHATFILE));
    nfp->nf_next = 0;
    nfp->nf_dev = dev;
    nfp->nf_fcb = ffcb;

    /*
     * Check if the specified file already exists.
     *
     * Note, that directories are handled differently in PhatDirCreate(),
     * where the first cluster is initialized with zeros.
     */
    if (PhatDirEntryFind(ndp, fname, PHAT_FATTR_FILEMASK, srch)) {
        /* 
         * File doesn't exist. Does the opening mode allow to create 
         * a new file? 
         */
        if ((mode & _O_CREAT) == 0) {
            free(srch);
            PhatFileClose(ndp);
            PhatFileClose(nfp);
            errno = ENOENT;
            return NUTFILE_EOF;
        }
        /* Create a new directory entry. */
        if (PhatDirEntryCreate(ndp, fname, acc, &ffcb->f_dirent)) {
            free(srch);
            PhatFileClose(ndp);
            PhatFileClose(nfp);
            return NUTFILE_EOF;
        }
        ffcb->f_de_dirty = 1;
#ifdef NUTDEBUG
        PhatDbgFileInfo(stdout, "New entry", ffcb);
#endif
    } else {
        /*
         * We return an error, if the file exists and _O_EXCL has been
         * set with _O_CREAT.
         */
        if ((mode & (_O_CREAT | _O_EXCL)) == (_O_CREAT | _O_EXCL)) {
            free(srch);
            PhatFileClose(ndp);
            PhatFileClose(nfp);
            errno = EEXIST;
            return NUTFILE_EOF;
        }
#ifdef NUTDEBUG
        PhatDbgFileInfo(stdout, "Existing entry", ffcb);
#endif
        /*
         * Truncate an existing file.
         */
        if (mode & _O_TRUNC) {
            /*
             * Relase all clusters allocated by this entry.
             */
            if (PhatDirReleaseChain(dev, &srch->phfind_ent)) {
                PhatFileClose(ndp);
                PhatFileClose(nfp);
                free(srch);
                return NUTFILE_EOF;
            }
            memset(ffcb, 0, sizeof(PHATFILE));
            memcpy(ffcb->f_dirent.dent_name, srch->phfind_ent.dent_name, sizeof(ffcb->f_dirent.dent_name));
            ffcb->f_dirent.dent_attr = srch->phfind_ent.dent_attr;
            ffcb->f_dirent.dent_rsvdnt = srch->phfind_ent.dent_rsvdnt;
            ffcb->f_dirent.dent_ctsecs = srch->phfind_ent.dent_ctsecs;
            ffcb->f_dirent.dent_ctime = srch->phfind_ent.dent_ctime;
            ffcb->f_dirent.dent_cdate = srch->phfind_ent.dent_cdate;
            ffcb->f_de_dirty = 1;
        }
        else {
            ffcb->f_dirent = srch->phfind_ent;
        }
    }
    free(srch);

    /* Store position of our directory entry. */
    ffcb->f_de_sect = PhatClusterSector(ndp, dfcb->f_clust) + dfcb->f_clust_pos;
    ffcb->f_de_offs = dfcb->f_sect_pos - 32;
    /* Store first cluster of parent. */
    ffcb->f_pde_clusthi = dfcb->f_dirent.dent_clusthi;
    ffcb->f_pde_clust = dfcb->f_dirent.dent_clust;
    /* Set the current cluster. */
    ffcb->f_clust = ffcb->f_dirent.dent_clusthi;
    ffcb->f_clust <<= 16;
    ffcb->f_clust += ffcb->f_dirent.dent_clust;
    /* Store the opening mode. */
    ffcb->f_mode = mode;

    /* Close the directory. */
    PhatFileClose(ndp);

    /*
     * Append to an existing file.
     */
    if ((mode & _O_APPEND) != 0 && ffcb->f_dirent.dent_fsize) {
        if (PhatFilePosSet(nfp, ffcb->f_dirent.dent_fsize)) {
            PhatFileClose(nfp);
            return NUTFILE_EOF;
        }
    }

#ifdef NUTDEBUG
    PhatDbgFileInfo(stdout, "File opened", ffcb);
#endif
    return nfp;
}

/*!
 * \brief Write data to a file.
 *
 * \param nfp    Pointer to a \ref NUTFILE structure, obtained by a previous 
 *               call to PnutFileOpen().
 * \param buffer Pointer to the data to be written. If zero, then the
 *               output buffer will be flushed.
 * \param len    Number of bytes to write.
 *
 * \return The number of bytes written. A return value of -1 indicates an 
 *         error.
 */
int PhatFileWrite(NUTFILE * nfp, CONST void *buffer, int len)
{
    int rc;
    int step;
    uint32_t clust;
    int sbn;
    uint8_t *buf = (uint8_t *) buffer;
    NUTDEVICE *dev = nfp->nf_dev;
    PHATFILE *fcb = nfp->nf_fcb;
    PHATVOL *vol = (PHATVOL *) dev->dev_dcb;

    /*
     * Refuse to write to files with RDONLY attribute set.
     */
    if (fcb->f_dirent.dent_attr & PHAT_FATTR_RDONLY) {
        errno = EACCES;
        return -1;
    }

    /*
     * Flush file if buffer is a NULL pointer.
     */
    if (buf == NULL || len == 0) {
        return PhatFileFlush(nfp);
    }

    /*
     * In case of normal files, check for sufficient space.
     */
    if ((fcb->f_dirent.dent_attr & PHAT_FATTR_DIR) == 0) {
        /* Bytes per cluster. */
        uint32_t num = vol->vol_sectsz * vol->vol_clustsz;
        /* Number of clusters already used. */
        uint32_t cur = (fcb->f_dirent.dent_fsize + num - 1) / num;

        /* Number of clusters used after writing. */
        num = (fcb->f_pos + len + num - 1) / num;
        /* If additional clusters are required, are they available? */
        if (num > cur && num - cur > vol->vol_numfree) {
            errno = ENOSPC;
            return -1;
        }

        /* If the file is empty, allocate the first cluster. */
        if (fcb->f_dirent.dent_fsize == 0) {
            if ((clust = AllocFirstCluster(nfp)) < 2) {
                return -1;
            }
            fcb->f_clust_prv = clust;
            fcb->f_clust = clust;
        }
    }


    /*
     * Write the data.
     */
    for (rc = 0, step = 0; rc < len; rc += step) {
        /* Did we reach the end of a sector? */
        if (fcb->f_sect_pos >= vol->vol_sectsz) {
            /* Move to the next sector within the cluster. */
            if (IsFixedRootDir(nfp)) {
                if (fcb->f_clust_pos + 1 >= vol->vol_rootsz) {
                    /* End of root directory, abort writing. */
                    break;
                }
                fcb->f_clust_pos++;
            }
            else {
                /* Did we reach the last sector of this cluster? */
                if (fcb->f_clust_pos  + 1 >= vol->vol_clustsz) {
                    /* Move to the next cluster. */
                    if (vol->vol_type == 32) {
                        if (Phat32GetClusterLink(dev, fcb->f_clust, &clust)) {
                            rc = -1;
                            break;
                        }
                        if (clust >= (PHATEOC & PHAT32CMASK)) {
                            if ((clust = AllocNextCluster(nfp)) < 2) {
                                rc = -1;
                                break;
                            }
                        }
                    } else if (vol->vol_type == 16) {
                        if (Phat16GetClusterLink(dev, fcb->f_clust, &clust)) {
                            rc = -1;
                            break;
                        }
                        if (clust >= (PHATEOC & PHAT16CMASK)) {
                            if ((clust = AllocNextCluster(nfp)) < 2) {
                                rc = -1;
                                break;
                            }
                        }
                    } else if (Phat12GetClusterLink(dev, fcb->f_clust, &clust)) {
                        rc = -1;
                        break;
                    } else if (clust >= (PHATEOC & PHAT12CMASK)) {
                        if ((clust = AllocNextCluster(nfp)) < 2) {
                            rc = -1;
                            break;
                        }
                    }
                    fcb->f_clust_pos = 0;
                    fcb->f_clust_prv = fcb->f_clust;
                    fcb->f_clust = clust;
                }
                else {
                    fcb->f_clust_pos++;
                }
            }
            fcb->f_sect_pos = 0;
        }

        /* Load the sector we want to write to. */
        if ((sbn = PhatSectorLoad(nfp->nf_dev, PhatClusterSector(nfp, fcb->f_clust) + fcb->f_clust_pos)) < 0) {
            rc = -1;
            break;
        }
        /* The number of bytes we write to this sector. */
        step = (int) (vol->vol_sectsz - fcb->f_sect_pos);
        if (step > len - rc) {
            step = len - rc;
        }
        /* Copy data to this sector. */
        memcpy(&vol->vol_buf[sbn].sect_data[fcb->f_sect_pos], &buf[rc], step);
        vol->vol_buf[sbn].sect_dirty = 1;
        /* Advance file pointers. */
        fcb->f_pos += step;
        fcb->f_sect_pos += step;
    }

    if (rc > 0) {
        /*
         * Update directory entry. Note that directory entries of directories
         * are never updated in a PHAT file system.
         */
        if ((fcb->f_dirent.dent_attr & PHAT_FATTR_DIR) == 0) {
            GetDosTimeStamp(&fcb->f_dirent.dent_mtime, &fcb->f_dirent.dent_mdate);
            fcb->f_dirent.dent_adate = fcb->f_dirent.dent_mdate;
            fcb->f_dirent.dent_attr |= PHAT_FATTR_ARCHIV;
            if(fcb->f_dirent.dent_fsize < fcb->f_pos) {
                fcb->f_dirent.dent_fsize = fcb->f_pos;
            }
            fcb->f_de_dirty = 1;
        }
    }
    return rc;
}

#ifdef __HARVARD_ARCH__
/*! 
 * \brief Write data from program space to a file.
 *
 * This function is not yet implemented and will always return -1.
 *
 * Similar to PhatFileWrite() except that the data is located in 
 * program memory.
 *
 * \param nfp    Pointer to a \ref NUTFILE structure, obtained by a previous 
 *               call to PnutFileOpen().
 * \param buffer Pointer to the data in program space. If zero, then the
 *               output buffer will be flushed.
 * \param len    Number of bytes to write.
 *
 * \return The number of bytes written. A return value of -1 indicates an 
 *         error.
 */
int PhatFileWrite_P(NUTFILE * nfp, PGM_P buffer, int len)
{
    return -1;
}
#endif

/*!
 * \brief Read data from a file.
 *
 * \param nfp    Pointer to a ::NUTFILE structure, obtained by a previous 
 *               call to PnutFileOpen().
 * \param buffer Pointer to the data buffer to fill.
 * \param size   Maximum number of bytes to read.
 *
 * \return The number of bytes actually read. A return value of -1 indicates 
 *         an error.
 */
int PhatFileRead(NUTFILE * nfp, void *buffer, int size)
{
    int rc;
    int step;
    int sbn;
    uint8_t *buf = (uint8_t *) buffer;
    NUTDEVICE *dev = nfp->nf_dev;
    PHATVOL *vol = (PHATVOL *) dev->dev_dcb;
    PHATFILE *fcb = nfp->nf_fcb;

    /*
     * Ignore input flush.
     */
    if (buf == NULL || size == 0) {
        return 0;
    }

    /* Respect the end of normal files. */
    if ((fcb->f_dirent.dent_attr & PHAT_FATTR_DIR) == 0) {
        if (fcb->f_pos + size >= fcb->f_dirent.dent_fsize) {
            size = fcb->f_dirent.dent_fsize - fcb->f_pos;
        }
    }
    for (rc = 0, step = 0; rc < size; rc += step) {
        /* Did we reach the end of a sector? */
        if (fcb->f_sect_pos >= vol->vol_sectsz) {
            /* Move to the next sector. */
            if (IsFixedRootDir(nfp)) {
                if (fcb->f_clust_pos + 1 >= vol->vol_rootsz) {
                    /* End of root directory, abort reading. */
                    break;
                }
                fcb->f_clust_pos++;
            }
            else {
                /* Did we reach the last sector of this cluster? */
                if (fcb->f_clust_pos + 1 >= vol->vol_clustsz) {
                    /* Move to the next cluster. */
                    uint32_t clust;

                    if (vol->vol_type == 32) {
                        if (Phat32GetClusterLink(dev, fcb->f_clust, &clust)) {
                            break;
                        }
                        if (clust >= (PHATEOC & PHAT32CMASK)) {
                            break;
                        }
                    } else if (vol->vol_type == 16) {
                        if (Phat16GetClusterLink(dev, fcb->f_clust, &clust)) {
                            break;
                        }
                        if (clust >= (PHATEOC & PHAT16CMASK)) {
                            break;
                        }
                    } else if (Phat12GetClusterLink(dev, fcb->f_clust, &clust)) {
                        break;
                    }
                    else if (clust >= (PHATEOC & PHAT12CMASK)) {
                        break;
                    }
                    fcb->f_clust_pos = 0;
                    fcb->f_clust_prv = fcb->f_clust;
                    fcb->f_clust = clust;
                }
                else {
                    fcb->f_clust_pos++;
                }
            }
            fcb->f_sect_pos = 0;
        }

        /* Make sure that the required sector is loaded. */
        if ((sbn = PhatSectorLoad(nfp->nf_dev, PhatClusterSector(nfp, fcb->f_clust) + fcb->f_clust_pos)) < 0) {
            rc = -1;
            break;
        }
        step = (int) (vol->vol_sectsz - fcb->f_sect_pos);
        if (step > size - rc) {
            step = size - rc;
        }
        memcpy(&buf[rc], &vol->vol_buf[sbn].sect_data[fcb->f_sect_pos], step);
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
 *            previous call to PhatFileOpen().
 *
 * \return Size of the file.
 */
static long PhatFileSize(NUTFILE *nfp)
{
    PHATFILE *fcb = nfp->nf_fcb;

    return fcb->f_dirent.dent_fsize;
}

static int PhatFileSeek(NUTFILE * nfp, long *pos, int whence)
{
    int rc = 0;
    long npos = *pos;
    PHATFILE *fcb = nfp->nf_fcb;

    switch (whence) {
    case SEEK_CUR:
        npos += fcb->f_pos;
        break;
    case SEEK_END:
        npos += PhatFileSize(nfp);
        break;
    }

    if (npos < 0 || npos > PhatFileSize(nfp)) {
        rc = EINVAL;
    } else {
        rc = PhatFilePosSet(nfp, npos);
        *pos = fcb->f_pos;
    }
    return rc;
}

/*!
 * \brief File system specific functions.
 * \param dev Specifies the file system device.
 */
static int PhatIOCtl(NUTDEVICE * dev, int req, void *conf)
{
    int rc = -1;

    switch (req) {
    case FS_STATUS:
        {
            FSCP_STATUS *par = (FSCP_STATUS *) conf;

            rc = PhatDirEntryStatus(dev, par->par_path, par->par_stp);
        }
        break;
    case FS_DIR_CREATE:
        rc = PhatDirCreate(dev, (char *) conf);
        break;
    case FS_DIR_REMOVE:
        rc = PhatDirRemove(dev, (char *) conf);
        break;
    case FS_DIR_OPEN:
        /* Open a directory for reading entries. */
        {
            DIR *dir = (DIR *) conf;

            if ((dir->dd_fd = PhatDirOpen(dev, dir->dd_buf)) != NUTFILE_EOF) {
                rc = 0;
            }
        }
        break;
    case FS_DIR_CLOSE:
        rc = PhatFileClose(((DIR *) conf)->dd_fd);
        break;
    case FS_DIR_READ:
        rc = PhatDirRead((DIR *) conf);
        break;
    case FS_FILE_STATUS:
        /* TODO */
        break;
    case FS_FILE_DELETE:
        rc = PhatDirDelEntry(dev, (char *) conf, PHAT_FATTR_FILEMASK & ~PHAT_FATTR_DIR);
        break;
    case FS_FILE_SEEK:
        PhatFileSeek((NUTFILE *) ((IOCTL_ARG3 *) conf)->arg1,  /* */
                     (long *) ((IOCTL_ARG3 *) conf)->arg2,      /* */
                     (int) ((IOCTL_ARG3 *) conf)->arg3);
        break;
    case FS_RENAME:
        /* Rename an existing file or directory. */
        {
            FSCP_RENAME *par = (FSCP_RENAME *) conf;

            rc = PhatDirRenameEntry(dev, par->par_old, par->par_new);
        }
        break;

    case FS_VOL_MOUNT:
        {
            /* Mount a volume. */
            FSCP_VOL_MOUNT *par = (FSCP_VOL_MOUNT *) conf;

            rc = PhatVolMount(dev, par->fscp_bmnt, par->fscp_part_type);
            if (rc) {
                /* Release resources on failures. */
                PhatVolUnmount(dev);
            }
        }
        break;
    case FS_VOL_UNMOUNT:
        /* Unmount a volume. */
        rc = PhatVolUnmount(dev);
        break;
    }
    return rc;
}

/*!
 * \brief Initialize the PHAT file system driver.
 *
 * This routine is called during device registration.
 *
 * \param dev Specifies the file system device.
 *
 * \return Zero on success. Otherwise an error code is returned.
 */
static int PhatInit(NUTDEVICE * dev)
{
    /* Nothing to do. */
    return 0;
}

/*!
 * \brief Reentrant variant of PhatFileOpen().
 */
static NUTFILE *PhatApiFileOpen(NUTDEVICE * dev, CONST char *path, int mode, int acc)
{
    NUTFILE *rc;
    PHATVOL *vol = (PHATVOL *) dev->dev_dcb;

    /* Make sure the volume is mounted. */
    if (vol == NULL) {
        errno = ENOENT;
        return NUTFILE_EOF;
    }

    /* Lock filesystem access. */
    NutEventWait(&vol->vol_fsmutex, 0);
    /* Call worker routine. */
    rc = PhatFileOpen(dev, path, mode, acc);
    /* Release filesystem lock. */
    NutEventPost(&vol->vol_fsmutex);

    return rc;
}

/*!
 * \brief Reentrant variant of PhatFileClose().
 */
static int PhatApiFileClose(NUTFILE * nfp)
{
    int rc;
    NUTDEVICE *dev = nfp->nf_dev;
    PHATVOL *vol = (PHATVOL *) dev->dev_dcb;

    /* Lock filesystem access. */
    NutEventWait(&vol->vol_fsmutex, 0);
    /* Call worker routine. */
    rc = PhatFileClose(nfp);
    /* Release filesystem lock. */
    NutEventPost(&vol->vol_fsmutex);

    return rc;
}

/*!
 * \brief Reentrant variant of PhatFileWrite().
 */
static int PhatApiFileWrite(NUTFILE * nfp, CONST void *buffer, int len)
{
    int rc;
    NUTDEVICE *dev = nfp->nf_dev;
    PHATVOL *vol = (PHATVOL *) dev->dev_dcb;

    /* Lock filesystem access. */
    NutEventWait(&vol->vol_fsmutex, 0);
    /* Call worker routine. */
    rc = PhatFileWrite(nfp, buffer, len);
    /* Release filesystem lock. */
    NutEventPost(&vol->vol_fsmutex);

    return rc;
}

#ifdef __HARVARD_ARCH__
/*!
 * \brief Reentrant variant of PhatFileWrite_P().
 */
static int PhatApiFileWrite_P(NUTFILE * nfp, PGM_P buffer, int len)
{
    int rc;
    NUTDEVICE *dev = nfp->nf_dev;
    PHATVOL *vol = (PHATVOL *) dev->dev_dcb;

    /* Lock filesystem access. */
    NutEventWait(&vol->vol_fsmutex, 0);
    /* Call worker routine. */
    rc = PhatFileWrite_P(nfp, buffer, len);
    /* Release filesystem lock. */
    NutEventPost(&vol->vol_fsmutex);

    return rc;
}
#endif

/*!
 * \brief Reentrant variant of PhatFileRead().
 */
static int PhatApiFileRead(NUTFILE * nfp, void *buffer, int size)
{
    int rc;
    NUTDEVICE *dev = nfp->nf_dev;
    PHATVOL *vol = (PHATVOL *) dev->dev_dcb;

    /* Lock filesystem access. */
    NutEventWait(&vol->vol_fsmutex, 0);
    /* Call worker routine. */
    rc = PhatFileRead(nfp, buffer, size);
    /* Release filesystem lock. */
    NutEventPost(&vol->vol_fsmutex);

    return rc;
}

/*!
 * \brief Reentrant variant of PhatIOCtl().
 */
static int PhatApiIOCtl(NUTDEVICE * dev, int req, void *conf)
{
    int rc;
    PHATVOL *vol = (PHATVOL *) dev->dev_dcb;

    /* Lock filesystem access. */
    if (req != FS_VOL_MOUNT && vol) {
        NutEventWait(&vol->vol_fsmutex, 0);
    }
    /* Call worker routine. */
    rc = PhatIOCtl(dev, req, conf);
    /* Release filesystem lock. */
    if (req != FS_VOL_MOUNT && req != FS_VOL_UNMOUNT && vol) {
        NutEventPost(&vol->vol_fsmutex);
    }
    return rc;
}

/*!
 * \brief PHAT file system driver information structure.
 *
 * A pointer to this structure must be passed to NutRegisterDevice() 
 * to bind this file system driver to the Nut/OS kernel.
 * An application may then call
 * /verbatim
 * _open("MMC0:1/PHAT0", _O_RDWR | _O_BINARY);
 * /endverbatim
 * to mount partition 1 on the previously registered block
 * device (devMmc0 in this example).
 */
NUTDEVICE devPhat0 = {
    0,                          /*!< Pointer to next device, dev_next. */
    {'P', 'H', 'A', 'T', '0', 0, 0, 0, 0}
    ,                           /*!< Unique device name, dev_name. */
    IFTYP_FS,                   /*!< Type of device, dev_type. Obsolete. */
    0,                          /*!< Base address, dev_base. Unused. */
    0,                          /*!< First interrupt number, dev_irq. Unused. */
    0,                          /*!< Mounted block device partition, dev_icb. */
    0,                          /*!< Volume information, dev_dcb. */
    PhatInit,                   /*!< Driver initialization routine, dev_init. */
    PhatApiIOCtl,               /*!< Driver specific control function, dev_ioctl. */
    PhatApiFileRead,            /*!< Read data from a file, dev_read. */
    PhatApiFileWrite,           /*!< Write data to a file, dev_write. */
#ifdef __HARVARD_ARCH__
    PhatApiFileWrite_P,         /*!< Write data from program space to a file, dev_write_P. */
#endif
    PhatApiFileOpen,            /*!< Open a file, dev_open. */
    PhatApiFileClose,           /*!< Close a file, dev_close. */
    PhatFileSize                /*!< Return file size, dev_size. */
};

NUTDEVICE devPhat1 = {
    0,                          /*!< Pointer to next device, dev_next. */
    {'P', 'H', 'A', 'T', '1', 0, 0, 0, 0}
    ,                           /*!< Unique device name, dev_name. */
    IFTYP_FS,                   /*!< Type of device, dev_type. Obsolete. */
    0,                          /*!< Base address, dev_base. Unused. */
    0,                          /*!< First interrupt number, dev_irq. Unused. */
    0,                          /*!< Mounted block device partition, dev_icb. */
    0,                          /*!< Volume information, dev_dcb. */
    PhatInit,                   /*!< Driver initialization routine, dev_init. */
    PhatApiIOCtl,               /*!< Driver specific control function, dev_ioctl. */
    PhatApiFileRead,            /*!< Read data from a file, dev_read. */
    PhatApiFileWrite,           /*!< Write data to a file, dev_write. */
#ifdef __HARVARD_ARCH__
    PhatApiFileWrite_P,         /*!< Write data from program space to a file, dev_write_P. */
#endif
    PhatApiFileOpen,            /*!< Open a file, dev_open. */
    PhatApiFileClose,           /*!< Close a file, dev_close. */
    PhatFileSize                /*!< Return file size, dev_size. */
};

/*@}*/
