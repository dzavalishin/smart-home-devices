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
 * \file fs/phatdir.c
 * \brief PHAT File System.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.14  2009/02/13 14:52:05  haraldkipp
 * Include memdebug.h for heap management debugging support.
 *
 * Revision 1.13  2009/01/17 11:26:46  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.12  2008/08/11 06:59:42  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.11  2006/10/08 16:42:56  haraldkipp
 * Not optimal, but simple and reliable exclusive access implemented.
 * Fixes bug #1486539. Furthermore, bug #1567790, which had been rejected,
 * had been reported correctly and is now fixed.
 *
 * Revision 1.10  2006/10/05 17:23:14  haraldkipp
 * Fixes bug #1571269. Thanks to Dirk Boecker for reporting this.
 *
 * Revision 1.9  2006/09/08 16:48:28  haraldkipp
 * Directory entries honor daylight saving time. Thanks to Dirk Boecker for
 * this fix.
 *
 * Revision 1.8  2006/07/05 16:00:30  haraldkipp
 * Two bugs fixed. Renaming a file's path (including directory change) failed.
 * Thanks to Dirk Boecker, who reported this.
 * The second bug was a memory hole, which had been discovered by
 * Ralf Spettel.
 *
 * Revision 1.7  2006/06/28 17:24:29  haraldkipp
 * Bugfix. Directories with long filename entries may not expand when
 * crossing cluster boundaries.
 *
 * Revision 1.6  2006/06/18 16:37:10  haraldkipp
 * No need to set errno after GetParentPath() returns an error.
 * No need to set errno after malloc failed.
 * New local helper function PhatOpenParentPath() reduces code size.
 * Support for long filenames (VFAT) added.
 * New function PhatDirReleaseChain() simplifies code.
 * Static function PhatDirOpenParentPath() replaced by global PhatDirOpenParent().
 * Added const attribute to path parameter of PhatDirOpen().
 * Now PhatDirEntryUpdate() clears the dirty flag for the root directory.
 * Calls to PhatFilePosSet() with position zero replaced by calls to PhatFilePosRewind().
 *
 * Revision 1.5  2006/04/07 12:52:24  haraldkipp
 * Memory hole fixed. Flag for normal / directory files now set correctly.
 *
 * Revision 1.4  2006/02/23 15:45:21  haraldkipp
 * PHAT file system now supports configurable number of sector buffers.
 * This dramatically increased write rates of no-name cards.
 * AVR compile errors corrected.
 *
 * Revision 1.3  2006/01/25 18:47:42  haraldkipp
 * Fixes wrong implementation of readdir() and simplifies the code.
 * Thanks to Jesper Hansen.
 *
 * Revision 1.2  2006/01/22 17:40:51  haraldkipp
 * Now mkdir() fails, if the directory exists already.
 * Now rmdir() returns an error when trying to delete subdirectories, which
 * are not empty.
 * Now PhatDirEntryStatus() sets correct errno value, if out of memory.
 *
 * Revision 1.1  2006/01/05 16:31:32  haraldkipp
 * First check-in.
 *
 *
 * \endverbatim
 */

#include <fs/fs.h>
#include <fs/phatfs.h>
#include <fs/phatvol.h>
#include <fs/phatio.h>
#include <fs/phatutil.h>
#include <fs/phatdir.h>

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <memdebug.h>

#if 0
/* Use for local debugging. */
#define NUTDEBUG
#include <stdio.h>
#include <fs/phatdbg.h>
#endif

/*!
 * \addtogroup xgPhatDir
 */
/*@{*/

/*! \brief Maximum number of directory entries. */
#define PHAT_MAXDIRENT   65536

/*!
 * \brief Create the short name's checksum.
 *
 * The checksum is calculated by rotating the previous sum by one
 * bit and adding the next character.
 */
static INLINE uint8_t GenShortNameChecksum(char *sfn)
{
    uint8_t rc = 0;
    int i;

    for (i = 0; i < 11; i++) {
        rc = ((rc >> 1) | ((rc & 1) << 7)) + (uint8_t)(sfn[i]);
    }
    return rc;
}

/*!
 * \brief Create a short unique file name from a given long file name.
 *
 * \param ndp Descriptor of the directory, which will be used for the
 *            short name.
 * \param lfn Long file name.
 * \param sfn Points to the buffer that will receive the short name in
 *            FCB format. Eleven characters will be stored in this buffer.
 *
 * \return 0 on success, -1 otherwise.
 */
static int GenShortName(NUTFILE * ndp, CONST char *lfn, char *sfn)
{
    int rc = -1;
    int i;
    int got;
    PHATDIRENT *entry;
    char *xp;

    /* Fill the buffer with spaces. */
    memset(sfn, ' ', 11);

    /* Get the position of the last dot in the long name. */
    xp = strrchr(lfn, '.');

    /* Now collect up to 8 characters for the basename and 3 characters
     * for the extension. */
    for (i = 0; i < 11 && *lfn; lfn++) {
        if (*lfn == '.') {
            /* We reached the first dot. The basename ends here. */
            if (lfn == xp) {
                /* This is also the last dot. Start copying the extension. */
                lfn++;
                if (*lfn) {
                    sfn[8] = toupper((unsigned char)*lfn);
                }
                i = 9;
            }
        }
        else if (i == 8) {
            /* First 8 characters collected. */
            if (xp == NULL) {
                /* No dot, no extension. */
                break;
            }
            lfn = xp + 1;
            if (*lfn) {
                sfn[i++] = toupper((unsigned char)*lfn);
            }
        }
        else if (*lfn != ' ') {
            if (strrchr("+,;=[]", *lfn)) {
                sfn[i++] = '_';
            }
            else {
                sfn[i++] = toupper((unsigned char)*lfn);
            }
        }
    }

    /*
     * Select a unique short name by verifying existing entries in the
     * specified directory.
     */
    if ((entry = malloc(sizeof(PHATDIRENT))) != NULL) {
        xp = sfn;
        for (i = 0; i < 6 && *xp != ' '; i++) {
            xp++;
        }
        /*
         * We try up to 99 unique short names only, but this should be
         * sufficient for our tiny system - hopefully.
         */
        for (i = 1; i <= 99 && rc; i++) {
            if (i == 10) {
                xp--;
            }
            *xp = '~';
            if (i > 9) {
                *(xp + 1) = '0' + i / 10;
                *(xp + 2) = '0' + i % 10;
            }
            else {
                *(xp + 1) = '0' + i;
            }
            PhatFilePosRewind((PHATFILE *)ndp->nf_fcb);
            for (;;) {
                /* Read next entry. */
                if ((got = PhatFileRead(ndp, entry, sizeof(PHATDIRENT))) != sizeof(PHATDIRENT)) {
                    if (got) {
                        /* Read error, stop searching. */
                        i = 9;
                    }
                    else {
                        /* End of directory reached, entry is unique. */
                        rc = 0;
                    }
                    break;
                }
                if ((entry->dent_attr & PHAT_FATTR_VOLID) == 0) {
                    if (entry->dent_name[0] == 0) {
                        /* End of directory reached, entry is unique. */
                        rc = 0;
                        break;
                    }
                    if (memcmp(sfn, (char *)entry->dent_name, 11) == 0) {
                        /* Entry exists, try next one. */
                        break;
                    }
                }
            }
        }
        free(entry);
    }
    return rc;
}

/*!
 * \brief Allocate a directory entry.
 *
 * \param ndp   Parent directory of the new entry.
 * \param fname Name of the new entry.
 * \param entry Information structure of the new entry is stored here.
 *
 * \return 0 on success, -1 otherwise.
 */
static int PhatDirEntryAlloc(NUTFILE * ndp, CONST char *fname, PHATDIRENT * entry)
{
    int rc;
    int pos = 0;
    int got;
    uint8_t sect;
    uint8_t *temp;
    NUTDEVICE *dev = ndp->nf_dev;
    PHATFILE *dfcb = ndp->nf_fcb;
    PHATVOL *vol = (PHATVOL *) dev->dev_dcb;
    PHATDIRENT *dent;
    PHATXDIRENT *xdent;
    int nwant = 1;
    int ngot = 0;
    int npos = 0;
    int i;
    int ci;
    uint8_t cks = 0;

    /* Convert the dotted name to a space filled one. */
    if ((rc = MakePhatName(fname, entry->dent_name)) == 1) {
        /* Reject name with wildcards. */
        errno = EINVAL;
        return -1;
    }
    if (rc == 0) {
        char vname[16];

        MakeVisibleName(entry->dent_name, vname);
        if (strcmp(fname, vname)) {
            rc = 1;
        }
    }
    if (rc) {
        if (GenShortName(ndp, fname, (char *)entry->dent_name)) {
            errno = EINVAL;
            return -1;
        }
        nwant = (strlen(fname) + 12) / 13 + 1;
        cks = GenShortNameChecksum((char *)entry->dent_name);
    }

    /* Allocate a temporary entry buffer. */
    if ((dent = malloc(sizeof(PHATDIRENT))) == NULL) {
        return -1;
    }

    /*
     * Find free entries, starting from the beginning.
     */
    PhatFilePosRewind(dfcb);
    rc = -1;
    for (;;) {
        /* Memorize the current position and try to read the next entry. */
        npos = dfcb->f_pos;
        if ((got = PhatFileRead(ndp, dent, sizeof(PHATDIRENT))) < 0) {
            /* Read failed. */
            break;
        }
        if (got != sizeof(PHATDIRENT)) {
            /*
             * End of directory reached. Try to expand it.
             */
            if (IsFixedRootDir(ndp) ||  /* */
                dfcb->f_pos >= PHAT_MAXDIRENT * sizeof(PHATDIRENT)) {
                /* Failed. Either PHAT12/16 root dir or max. size reached. */
                errno = EFBIG;
                break;
            }

            /* Fill a new cluster with zeros. */
            if ((temp = malloc(vol->vol_sectsz)) == NULL) {
                break;
            }
            memset(temp, 0, vol->vol_sectsz);
            for (sect = vol->vol_clustsz; sect; sect--) {
                if (PhatFileWrite(ndp, temp, vol->vol_sectsz) < 0) {
                    /* Write failed. */
                    break;
                }
            }
            free(temp);
            /* End of directory reached and expanded by a new cluster. */
            if (sect == 0) {
                if (ngot == 0) {
                    pos = npos;
                }
                rc = 0;
            }
            break;
        }
        if (dent->dent_name[0] == PHAT_REM_DIRENT || dent->dent_name[0] == 0) {
            /* Empty entry found. */
            if (ngot == 0) {
                pos = npos;
            }
            ngot++;
            if (ngot >= nwant) {
                rc = 0;
                break;
            }
        }
        else {
            ngot = 0;
        }
    }
    free(dent);

    if (rc == 0) {
        /* Return to the memorized position and write the new entry. */
        PhatFilePosSet(ndp, pos);
        if ((xdent = malloc(sizeof(PHATXDIRENT))) == NULL) {
            return -1;
        }
        for (ngot = nwant - 1; ngot; ngot--) {
            memset(xdent, 0xFF, sizeof(PHATXDIRENT));
            xdent->xdent_seq = ngot;
            if (ngot == nwant - 1) {
                xdent->xdent_seq |= 0x40;
            }
            xdent->xdent_attr = PHAT_FATTR_LFN;
            xdent->xdent_cks = cks;
            xdent->xdent_clust = 0;
            xdent->xdent_rsvd = 0;
            /* Simplified unicode conversion, ignores double byte characters. */
            ci = (ngot - 1) * 13;
            for (i = 0; i < 13; i++) {
                if (ci + i <= strlen(fname)) {
                    if (i < 5) {
                        xdent->xdent_uname_1_5[i] = fname[ci + i];
                    }
                    else if (i < 11) {
                        xdent->xdent_uname_6_11[i - 5] = fname[ci + i];
                    }
                    else {
                        xdent->xdent_uname_12_13[i - 11] = fname[ci + i];
                    }
                }
            }
            if (PhatFileWrite(ndp, xdent, sizeof(PHATXDIRENT)) < 0) {
                /* Write error. */
                rc = -1;
                break;
            }
        }
        if (PhatFileWrite(ndp, entry, sizeof(PHATDIRENT)) < 0) {
            /* Write error. */
            rc = -1;
        }
        free(xdent);
    }
    return rc;
}

/*!
 * \brief Release a directory entry.
 *
 * In the PHAT file system released entries will be marked with the
 * special character 0xE5 as the first character in the file name.
 *
 * \param ndp    The parent directory of the entry that should be removed.
 * \param pos    Byte position of that entry.
 * \param lfncnt Number of long file entries.
 *
 * \return 0 on success, -1 otherwsie.
 */
static int PhatDirEntryRelease(NUTFILE * ndp, uint32_t pos, int lfncnt)
{
    uint8_t ch = PHAT_REM_DIRENT;
    int i;

    for (i = lfncnt; i; i--) {
        PhatFilePosSet(ndp, pos - i * sizeof(PHATXDIRENT));
        if (PhatFileWrite(ndp, &ch, 1) < 0) {
            return -1;
        }
    }
    PhatFilePosSet(ndp, pos);
    if (PhatFileWrite(ndp, &ch, 1) < 0) {
        return -1;
    }
    return 0;
}

/*!
 * \brief Create a new directory entry.
 *
 * \param ndp    Handle to the parent directory.
 * \param name   Name of the new entry.
 * \param acc    Attributes of the new entry.
 * \param dirent Information structure of the new entry.
 *
 * \return 0 on success. Otherwise -1 is returned.
 */
int PhatDirEntryCreate(NUTFILE * ndp, CONST char *name, int acc, PHATDIRENT * dirent)
{
    dirent->dent_attr = (uint8_t) acc;
    GetDosTimeStamp(&dirent->dent_ctime, &dirent->dent_cdate);
    dirent->dent_adate = dirent->dent_cdate;
    dirent->dent_mtime = dirent->dent_ctime;
    dirent->dent_mdate = dirent->dent_cdate;

    if (PhatDirEntryAlloc(ndp, name, dirent)) {
        return -1;
    }
#ifdef NUTDEBUG
    PhatDbgFileInfo(stdout, "New Dir Entry", ndp->nf_fcb);
#endif
    return 0;
}

/*!
 * \brief Update directory entry of an opened file or directory.
 *
 * \param nfp File of which the directory entry will be updated.
 *
 * \return 0 on success. Otherwise -1 is returned.
 */
int PhatDirEntryUpdate(NUTFILE * nfp)
{
    int sbn;
    NUTDEVICE *dev = nfp->nf_dev;
    PHATVOL *vol = (PHATVOL *) dev->dev_dcb;
    PHATFILE *fcb = nfp->nf_fcb;

    if (fcb->f_de_dirty) {
        /*
         * The file may specify the root directory, in which case
         * the updated is skipped.
         */
        if (fcb->f_de_sect) {
#ifdef NUTDEBUG
            PhatDbgDirEntry(stdout, "PhatDirEntryUpdate", &fcb->f_dirent);
#endif
            if ((sbn = PhatSectorLoad(dev, fcb->f_de_sect)) < 0) {
                return -1;
            }
            memcpy(vol->vol_buf[sbn].sect_data + fcb->f_de_offs, &fcb->f_dirent, sizeof(PHATDIRENT));
            vol->vol_buf[sbn].sect_dirty = 1;
        }
        fcb->f_de_dirty = 0;
    }
    return 0;
}

/*!
 * \brief Read the next directory entry.
 *
 * \param ndp  Specifies the directory to read.
 * \param srch Pointer to a structure which receives the result.
 *
 * \return 0 on success. Otherwise -1 is returned.
 */
static int PhatDirEntryRead(NUTFILE * ndp, PHATFIND * srch)
{
    PHATDIRENT *entry = &srch->phfind_ent;
    PHATFILE *fcb = ndp->nf_fcb;
    PHATXDIRENT *xentry;
    char *lfn = NULL;
    int lfnpos = 0;
    int nxtseq = 0;
    int lfncnt = 0;
    int i;

    for (;;) {
        /* Read next entry. */
        if (PhatFileRead(ndp, entry, sizeof(PHATDIRENT)) != sizeof(PHATDIRENT)) {
            break;
        }
        /* Skip removed entries. */
        if (entry->dent_name[0] == PHAT_REM_DIRENT) {
            if (lfn) {
                free(lfn);
                lfn = NULL;
            }
        }
        /* Process long filename entries. */
        else if (entry->dent_attr == PHAT_FATTR_LFN) {
            xentry = (PHATXDIRENT *)entry;
            lfnpos = (xentry->xdent_seq & 0x3F);
            /* Make sure we are in sequence. */
            if ((nxtseq == 0 && (xentry->xdent_seq & 0x40) != 0) || nxtseq == lfnpos) {
                nxtseq = --lfnpos;
                lfnpos *= 13;
                if (lfnpos + 13 > PHAT_MAX_NAMELEN) {
                    errno = EINVAL;
                    break;
                }
                if (xentry->xdent_seq & 0x40) {
                    if (lfn == NULL) {
                        lfn = malloc(PHAT_MAX_NAMELEN + 1);
                    }
                    lfn[lfnpos + 13] = 0;
                    lfncnt = 0;
                }
                lfncnt++;
                /* Simplified unicode conversion, ignores double byte characters. */
                for (i = 0; i < 5; i++) {
                    lfn[lfnpos + i] = (char)xentry->xdent_uname_1_5[i];
                }
                for (i = 0; i < 6; i++) {
                    lfn[lfnpos + 5 + i] = (char)xentry->xdent_uname_6_11[i];
                }
                for (i = 0; i < 2; i++) {
                    lfn[lfnpos + 11 + i] = (char)xentry->xdent_uname_12_13[i];
                }
            }
        }

        /* Skip volume IDs. */
        else if ((entry->dent_attr & PHAT_FATTR_VOLID) == 0) {
            /* Stop searching if last entry reached. */
            if (entry->dent_name[0] == 0) {
                break;
            }
            /* Valid entry found. Return success. */
            srch->phfind_pos = fcb->f_pos - sizeof(PHATDIRENT);
            if (lfn && lfnpos == 0) {
                strcpy(srch->phfind_name, lfn);
                srch->phfind_xcnt = lfncnt;
            }
            else {
                MakeVisibleName(entry->dent_name, srch->phfind_name);
                srch->phfind_xcnt = 0;
            }
            if (lfn) {
                free(lfn);
            }
            nxtseq = 0;
            lfncnt = 0;
            return 0;
        }
        else if (lfn) {
            free(lfn);
            lfn = NULL;
            nxtseq = 0;
            lfncnt = 0;
        }
    }
    if (lfn) {
        free(lfn);
    }
    return -1;
}

/*!
 * \brief Find a directory entry with a specified name.
 *
 * \param ndp    Pointer to a previously opened directory.
 * \param spec   Name of the entry to retrieve.
 * \param attmsk Attribute mask. Entries with attributes not specified
 *               in this mask will be ignored.
 * \param srch   Optional pointer to a structure which receives the
 *               search result.
 *
 * \return 0 if an entry was found, otherwise -1 is returned.
 */
int PhatDirEntryFind(NUTFILE * ndp, CONST char *spec, uint32_t attmsk, PHATFIND * srch)
{
    int rc;
    PHATFIND *temps;

    /* Allocate a temporary structure to store the search result. */
    if ((temps = malloc(sizeof(PHATFIND))) == NULL) {
        return -1;
    }

    /*
     * Loop until the specified entry was found or until we reach the
     * end of the directory.
     */
    PhatFilePosRewind((PHATFILE *)ndp->nf_fcb);
    while ((rc = PhatDirEntryRead(ndp, temps)) == 0) {
        if ((temps->phfind_ent.dent_attr | attmsk) == attmsk) {
            if (strcasecmp(temps->phfind_name, spec) == 0) {
                /* Specified entry found. */
                if (srch) {
                    *srch = *temps;
                }
                break;
            }
        }
    }
    free(temps);

    return rc;
}

/*!
 * \brief Open parent directory of a given path.
 *
 * \param dev      Specifies the file system device.
 * \param path     Full path.
 * \param basename Points to a pointer which will be set to the last component
 *                 within the full path.
 *
 * \return Pointer to a NUTFILE structure if successful or NUTFILE_EOF otherwise.
 */
NUTFILE *PhatDirOpenParent(NUTDEVICE * dev, CONST char *path, CONST char **basename)
{
    NUTFILE *rc = NUTFILE_EOF;
    char *parent;

    if ((parent = GetParentPath(path, basename)) != NULL) {
        rc = PhatDirOpen(dev, parent);
        free(parent);
    }
    return rc;
}

/*!
 * \brief Rename file.
 *
 * \param dev      Specifies the file system device.
 * \param old_path Old name and path of the file.
 * \param new_path New name and path of the file.
 *
 * \return 0 on success, -1 otherwise.
 */
int PhatDirRenameEntry(NUTDEVICE * dev, CONST char *old_path, CONST char *new_path)
{
    int rc = -1;
    CONST char *fname;
    NUTFILE *old_ndp;
    NUTFILE *new_ndp;
    PHATFIND *srch;

    /*
     * Open directory of the old file.
     */
    if ((old_ndp = PhatDirOpenParent(dev, old_path, &fname)) == NUTFILE_EOF) {
        return -1;
    }

    if ((srch = malloc(sizeof(PHATFIND))) != NULL) {
        if ((rc = PhatDirEntryFind(old_ndp, fname, PHAT_FATTR_FILEMASK, srch)) == 0) {
            rc = -1;
            if ((new_ndp = PhatDirOpenParent(dev, new_path, &fname)) != NUTFILE_EOF) {
                if (PhatDirEntryFind(new_ndp, fname, PHAT_FATTR_FILEMASK, NULL) == 0) {
                    errno = EEXIST;
                } else {
                    if ((rc = PhatDirEntryAlloc(new_ndp, fname, &srch->phfind_ent)) == 0) {
                        rc = PhatDirEntryRelease(old_ndp, srch->phfind_pos, srch->phfind_xcnt);
                    }
                }
                PhatFileClose(new_ndp);
            }
        }
        else {
            errno = ENOENT;
        }
        free(srch);
    }
    PhatFileClose(old_ndp);

    return rc;
}

/*!
 * \brief Release a cluster chain of a specified directory entry.
 *
 * \param dev  Specifies the file system device.
 * \param dent Directory entry.
 *
 * \return 0 on success or -1 on failure.
 */
int PhatDirReleaseChain(NUTDEVICE * dev, PHATDIRENT *dent)
{
    int rc = 0;
    PHATVOL *vol = (PHATVOL *) dev->dev_dcb;
    uint32_t clust;

    /* Do not remove clusters of files with RDONLY attribute. */
    if (dent->dent_attr & PHAT_FATTR_RDONLY) {
        errno = EACCES;
        return -1;
    }

    /* Get the first cluster of this file. The directory entry stores this
       value in two positions. */
    clust = dent->dent_clusthi;
    clust <<= 16;
    clust |= dent->dent_clust;

    /* The data area starts at cluster 2. With empty files the first cluster
       is set to zero. A value of one is suspicious and will be ignored. */
    if (clust >= 2) {
        /* Call the format specific release routine. */
        if (vol->vol_type == 32) {
            rc = Phat32ReleaseChain(dev, clust);
        } else if (vol->vol_type == 16) {
            rc = Phat16ReleaseChain(dev, clust);
        } else {
            rc = Phat12ReleaseChain(dev, clust);
        }
    }
    return rc;
}

/*!
 * \brief Remove a directory entry.
 *
 * Allocated clusters are released.
 *
 * \param dev   Specifies the file system device.
 * \param path  Name of the entry to remove.
 * \param flags Attributes of the entry to remove.
 *
 * \return 0 if successful. Otherwise returns an error code.
 */
int PhatDirDelEntry(NUTDEVICE * dev, CONST char *path, uint32_t flags)
{
    int rc = -1;
    PHATFIND *srch;
    NUTFILE *ndp;
    CONST char *fname;

    /* Open the parent directory. */
    if ((ndp = PhatDirOpenParent(dev, path, &fname)) != NUTFILE_EOF) {
        if ((srch = malloc(sizeof(PHATFIND))) != NULL) {
            /* Find the specified file name. */
            if (PhatDirEntryFind(ndp, fname, flags, srch) == 0) {
                if (PhatDirReleaseChain(dev, &srch->phfind_ent) == 0) {
                    rc = PhatDirEntryRelease(ndp, srch->phfind_pos, srch->phfind_xcnt);
                }
            }
            else {
                errno = ENOENT;
            }
            free(srch);
        }
        PhatFileClose(ndp);
    }
    return rc;
}

/*!
 * \brief Open a directory.
 *
 * \param dev   Specifies the file system device.
 * \param dpath Full absolute pathname of the directory to open.
 *
 * \return Pointer to a NUTFILE structure if successful or NUTFILE_EOF otherwise.
 */
NUTFILE *PhatDirOpen(NUTDEVICE * dev, CONST char *dpath)
{
    NUTFILE *ndp;
    PHATFILE *dfcb;
    PHATFIND *srch;
    char *comp;
    char *cp;
    int sz;
    PHATVOL *vol = (PHATVOL *) dev->dev_dcb;

    /* Make sure the volume is mounted. */
    if (vol == NULL) {
        errno = ENODEV;
        return NUTFILE_EOF;
    }

    /* Allocate the structure to return. */
    if ((ndp = malloc(sizeof(NUTFILE))) == NULL) {
        return NUTFILE_EOF;
    }
    if ((ndp->nf_fcb = malloc(sizeof(PHATFILE))) == NULL) {
        free(ndp);
        return NUTFILE_EOF;
    }
    memset(ndp->nf_fcb, 0, sizeof(PHATFILE));
    ndp->nf_next = NULL;
    ndp->nf_dev = dev;

    dfcb = ndp->nf_fcb;
    dfcb->f_dirent.dent_attr = PHAT_FATTR_DIR;
    dfcb->f_mode = _O_RDONLY;

    /* We start at the root directory. */
    dfcb->f_clust = vol->vol_root_clust;
    dfcb->f_dirent.dent_clusthi = (uint16_t) (vol->vol_root_clust >> 16);
    dfcb->f_dirent.dent_clust = (uint16_t) vol->vol_root_clust;

    if (*dpath == '/') {
        dpath++;
    }
    if (*dpath) {
        /*
         * We are looking for a subdirectory.
         */
        if ((comp = malloc(PHAT_MAX_NAMELEN + 1)) == NULL) {
            free(dfcb);
            free(ndp);
            return NUTFILE_EOF;
        }
        if ((srch = malloc(sizeof(PHATFIND))) == NULL) {
            free(comp);
            free(dfcb);
            free(ndp);
            return NUTFILE_EOF;
        }

        /*
         * Walk down the path.
         */
        while (*dpath) {
            /* Fetch the next component from the path. */
            cp = comp;
            sz = 0;
            while (*dpath) {
                if (*dpath == '/') {
                    dpath++;
                    break;
                }
                if (++sz > PHAT_MAX_NAMELEN) {
                    break;
                }
                *cp++ = *dpath++;
            }
            *cp = 0;

            /* Search component's entry in the current directory. */
            if (sz > PHAT_MAX_NAMELEN || PhatDirEntryFind(ndp, comp, PHAT_FATTR_FILEMASK, srch)) {
                errno = ENOENT;
                free(dfcb);
                free(ndp);
                ndp = NUTFILE_EOF;
                break;
            }

            /*
             * Next component found. Mimic the open by updating the existing
             * file control block structure.
             */
            dfcb->f_de_sect = PhatClusterSector(ndp, dfcb->f_clust) + dfcb->f_clust_pos;
            dfcb->f_de_offs = dfcb->f_sect_pos - 32;

            /* Set the cluster of our directory entry. */
            dfcb->f_pde_clusthi = dfcb->f_dirent.dent_clusthi;
            dfcb->f_pde_clust = dfcb->f_dirent.dent_clust;

            dfcb->f_dirent = srch->phfind_ent;
#ifdef NUTDEBUG
            PhatDbgFileInfo(stdout, "Component", dfcb);
#endif

            /*
             * Handle root directory.
             */
            if (dfcb->f_dirent.dent_attr & PHAT_FATTR_DIR) {
                if (dfcb->f_dirent.dent_clust == 0 && dfcb->f_dirent.dent_clusthi == 0) {
                    if (vol->vol_type != 32) {
                        dfcb->f_de_sect = 0;
                    }
                    dfcb->f_dirent.dent_clusthi = (uint16_t) (vol->vol_root_clust >> 16);
                    dfcb->f_dirent.dent_clust = (uint16_t) vol->vol_root_clust;
                }
            }

            /*
             * Reset position.
             */
            PhatFilePosRewind(dfcb);
            dfcb->f_clust_prv = 0;
            dfcb->f_mode = _O_RDONLY;
        }
        free(srch);
        free(comp);
    }
    return ndp;
}

/*!
 * \brief Read the next directory entry.
 *
 * \param dir Pointer to the internal directory information structure.
 *
 * \return 0 on success, -1 otherwise.
 */
int PhatDirRead(DIR * dir)
{
    PHATFIND *srch;
    struct dirent *ent;

    if ((srch = malloc(sizeof(PHATFIND))) == NULL) {
        return -1;
    }
    if (PhatDirEntryRead(dir->dd_fd, srch)) {
        free(srch);
        return -1;
    }
#ifdef NUTDEBUG
    PhatDbgDirEntry(stdout, "Read entry", &srch->phfind_ent);
#endif

    ent = (struct dirent *) dir->dd_buf;
    memset(dir->dd_buf, 0, sizeof(struct dirent));
    ent->d_namlen = (uint8_t) strlen(srch->phfind_name);
    strcpy(ent->d_name, srch->phfind_name);
    if (srch->phfind_ent.dent_attr & PHAT_FATTR_DIR) {
        ent->d_type = 1;
    }
    free(srch);

    return 0;
}

/*!
 * \brief Create a new subdirectory.
 *
 * One cluster is allocated, initialized to zero and two directory
 * entries are created, '.' and '..'.
 *
 * \param dev  Specifies the file system device.
 * \param path Full path to the directory.
 *
 * \return 0 on success. Otherwise -1 is returned.
 */
int PhatDirCreate(NUTDEVICE * dev, char *path)
{
    NUTFILE *ndp;
    PHATVOL *vol = (PHATVOL *) dev->dev_dcb;
    PHATFILE *dfcb;
    PHATDIRENT *entry;
    uint8_t *buf;
    uint32_t sect;
    uint32_t clust;

    /*
     * Create the new directory like a normal file with a special attribute.
     */
    if ((ndp = PhatFileOpen(dev, path, _O_CREAT | _O_RDWR | _O_EXCL, PHAT_FATTR_DIR)) == NUTFILE_EOF) {
        return -1;
    }
    dfcb = ndp->nf_fcb;

    /*
     * Allocate a first cluster and initialize it with zeros.
     */
    if ((clust = AllocFirstCluster(ndp)) < 2) {
        PhatFileClose(ndp);
        return -1;
    }
    dfcb->f_clust_prv = clust;
    dfcb->f_clust = clust;
    if ((buf = malloc(vol->vol_sectsz)) == NULL) {
        PhatFileClose(ndp);
        return -1;
    }
    memset(buf, 0, vol->vol_sectsz);
    for (sect = vol->vol_clustsz; sect; sect--) {
        if (PhatFileWrite(ndp, buf, vol->vol_sectsz) < 0) {
            /* Write failed. */
            free(buf);
            PhatFileClose(ndp);
            return -1;
        }
    }
    free(buf);

    /*
     * Write the dot entry.
     */
    entry = malloc(sizeof(PHATDIRENT));
    *entry = dfcb->f_dirent;
    memset(entry->dent_name, ' ', sizeof(entry->dent_name));
    entry->dent_name[0] = '.';
    PhatFilePosRewind(dfcb);
    if (PhatFileWrite(ndp, entry, sizeof(PHATDIRENT)) != sizeof(PHATDIRENT)) {
        PhatFileClose(ndp);
        free(entry);
        return -1;
    }

    /*
     * Write the double dot entry. If it points to the root cluster,
     * then the cluster number in the directory entry must be zero.
     */
    if ((uint16_t) vol->vol_root_clust == dfcb->f_pde_clust &&   /* */
        vol->vol_root_clust >> 16 == dfcb->f_pde_clusthi) {
        entry->dent_clust = 0;
        entry->dent_clusthi = 0;
    } else {
        entry->dent_clust = dfcb->f_pde_clust;
        entry->dent_clusthi = dfcb->f_pde_clusthi;
    }
    entry->dent_name[1] = '.';
    if (PhatFileWrite(ndp, entry, sizeof(PHATDIRENT)) != sizeof(PHATDIRENT)) {
        PhatFileClose(ndp);
        free(entry);
        return -1;
    }
    free(entry);

    return PhatFileClose(ndp);
}

/*!
 * \brief Remove a specified subdirectory.
 *
 * \param dev  Specifies the file system device.
 * \param path Full path to the directory.
 *
 * \return 0 on success. Otherwise -1 is returned.
 */
int PhatDirRemove(NUTDEVICE * dev, char *path)
{
    int rc = -1;
    PHATDIRENT *entry;
    NUTFILE *ndp;

    /* Never remove the root directory */
    if (path[0] == '/' && path[1] == 0) {
        errno = EBUSY;
        return -1;
    }

    if ((entry = malloc(sizeof(PHATDIRENT))) == NULL) {
        return -1;
    }

    /*
     * Make sure, that the directory we want to remove is empty. The dot
     * and double dot entries are ignored.
     */
    if ((ndp = PhatFileOpen(dev, path, _O_RDONLY, 0)) != NUTFILE_EOF) {
        rc = 0;
        for (;;) {
            rc = PhatFileRead(ndp, entry, sizeof(PHATDIRENT));
            if (rc < 0) {
                break;
            }
            /* Check for end of directory. */
            if (rc < sizeof(PHATDIRENT) || entry->dent_name[0] == 0) {
                rc = 0;
                break;
            }
            /* Skip removed entries. */
            if (entry->dent_name[0] == PHAT_REM_DIRENT) {
                continue;
            }
            /* Ignore entries which are not files. */
            if ((entry->dent_attr | PHAT_FATTR_FILEMASK) != PHAT_FATTR_FILEMASK) {
                continue;
            }
            /* Ignore dot and double dot entries. */
            if (entry->dent_name[0] == '.' &&   /* */
                (entry->dent_name[1] == '.' || entry->dent_name[1] == ' ')) {
                if (memcmp("         ", &entry->dent_name[2], 9) == 0) {
                    continue;
                }
            }
            errno = ENOTEMPTY;
            rc = -1;
            break;
        }
        PhatFileClose(ndp);
    }
    free(entry);

    /* If the empty check was successful, then remove the entry. */
    if (rc == 0) {
        rc = PhatDirDelEntry(dev, path, PHAT_FATTR_DIR);
    }
    return rc;
}

/*!
 * \brief Retrieve status of a specified file.
 *
 * \param dev File system device.
 * \param path Path name to the file to query.
 * \param stp  Pointer to a structure which receives the result.
 *
 * return 0 on success, -1 otherwise.
 */
int PhatDirEntryStatus(NUTDEVICE * dev, CONST char *path, struct stat *stp)
{
    int rc;
    CONST char *fname;
    NUTFILE *ndp;
    PHATFIND *srch;
    unsigned int val;

    /* Open parent directory. */
    if ((ndp = PhatDirOpenParent(dev, path, &fname)) == NUTFILE_EOF) {
        return -1;
    }

    if ((srch = malloc(sizeof(PHATFIND))) == NULL) {
        PhatFileClose(ndp);
        return -1;
    }
    if ((rc = PhatDirEntryFind(ndp, fname, PHAT_FATTR_FILEMASK, srch)) == 0) {
        struct _tm t;

        memset(&t, 0, sizeof(struct _tm));
        val = srch->phfind_ent.dent_mtime;
        t.tm_sec = (val & 0x1F) << 1;
        t.tm_min = (val >> 5) & 0x3F;
        t.tm_hour = (val >> 11) & 0x1F;
        val = srch->phfind_ent.dent_mdate;
        t.tm_mday = val & 0x1F;
        t.tm_mon = ((val >> 5) & 0x0F);
        if (t.tm_mon) {
            t.tm_mon--;
        }
        t.tm_year = ((val >> 9) & 0x7F) + 80;
        t.tm_isdst = _daylight;
        stp->st_mtime = mktime(&t);

        stp->st_ino = 0;
        stp->st_mode = (srch->phfind_ent.dent_attr & PHAT_FATTR_DIR) != 0;
        stp->st_nlink = 0;
        stp->st_size = srch->phfind_ent.dent_fsize;
    }
    free(srch);
    PhatFileClose(ndp);

    return rc;
}

/*@}*/
