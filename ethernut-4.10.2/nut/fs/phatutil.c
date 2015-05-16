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
 * \file fs/phatutil.c
 * \brief PHAT File System.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.7  2009/02/13 14:52:05  haraldkipp
 * Include memdebug.h for heap management debugging support.
 *
 * Revision 1.6  2008/08/11 06:59:42  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.5  2006/10/08 16:48:09  haraldkipp
 * Documentation fixed
 *
 * Revision 1.4  2006/06/18 16:39:46  haraldkipp
 * File modification date changed from GMT to local time for Windows
 * compatibility.
 * No need to set errno after malloc failed.
 * Support for long filenames (VFAT) added.
 * Fixed positioning bug, which caused several problems like limiting
 * directories to one cluster.
 *
 * Revision 1.3  2006/03/02 19:59:56  haraldkipp
 * ICCARM insists on a (void *) typecast for the second parameter of memcpy().
 *
 * Revision 1.2  2006/02/23 15:45:22  haraldkipp
 * PHAT file system now supports configurable number of sector buffers.
 * This dramatically increased write rates of no-name cards.
 * AVR compile errors corrected.
 *
 * Revision 1.1  2006/01/05 16:31:50  haraldkipp
 * First check-in.
 *
 *
 * \endverbatim
 */

#include <fs/phatfs.h>
#include <fs/phatvol.h>
#include <fs/phatdir.h>
#include <fs/phatutil.h>

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>
#include <memdebug.h>

#if 0
/* Use for local debugging. */
#define NUTDEBUG
#include <stdio.h>
#include <fs/phatdbg.h>
#endif

/*!
 * \addtogroup xgPhatUtil
 */
/*@{*/

/*!
 * \brief Create a DOS timestamp of the current time and date.
 *
 * \param dostim Receives the time of day part.
 * \param dosdat Receives the calendar date part.
 */
void GetDosTimeStamp(uint16_t * dostim, uint16_t * dosdat)
{
    time_t now;
    struct _tm *gmt;

    time(&now);
    gmt = localtime(&now);

    if (dosdat) {
        *dosdat = (uint16_t) (gmt->tm_mday | ((gmt->tm_mon + 1) << 5) | ((gmt->tm_year - 80) << 9));
    }
    if (dostim) {
        *dostim = (uint16_t) ((gmt->tm_sec / 2) | (gmt->tm_min << 5) | (gmt->tm_hour << 11));
    }
}

/*!
 * \brief Convert filename to a directory entry name.
 *
 * \param src Original name in the format 'name.ext'.
 * \param dst Converted name in the format 'name    ext'.
 *
 * \return 0 on success, 1 if the name contains wildcards or
 *         -1 if the name is malformed.
 */
int MakePhatName(CONST char *src, uint8_t * dst)
{
    int rc = 0;
    int i;

    /* Fill destination with spaces. */
    memset(dst, ' ', 11);

    /* Skip leading dots. */
    for (i = 0; src[i] == '.'; i++);
    /* Handle the special entries dot and double dot. */
    if (src[i] == 0 && (i == 1 || i == 2)) {
        while (i--) {
            *dst++ = '.';
        }
        return 0;
    }

    /* Convert the special token of removed entries. */
    if (*src == (char)PHAT_REM_DIRENT) {
        dst[0] = PHAT_REM_NAMENT;
        src++;
    } else {
        dst[0] = toupper((unsigned char)*src);
        src++;
    }

    /* Copy the rest of the base name upto the dot, 8 characters max. */
    for (i = 1; i < 8 && *src && *src != '.'; i++, src++) {
        dst[i] = toupper((unsigned char)*src);
    }

    /* More characters? */
    if (*src) {
        /* If we didn't reach a dot, then the base name is too long. */
        if (*src != '.') {
            return -1;
        }
        /* Skip the dot and copy the extension. */
        src++;
        for (i = 8; i < 11 && *src; i++, src++) {
            dst[i] = toupper((unsigned char)*src);
        }
        /* If more characters are available, then the extension is too long. */
        if (*src) {
            return -1;
        }
    }

    /* Finally check our result. */
    for (i = 0; i < 11; i++) {
        /* Reject control codes. */
        if (dst[i] < ' ') {
            return -1;
        }
        /* Reject illegal characters. */
        if (strchr("\"+,./:;<=>[\\]^", dst[i])) {
            return -1;
        }
        /* Wildcard found. */
        if (dst[i] == '?') {
            rc = 1;
        } else if (dst[i] == '*') {
            rc = 1;
            /* Stars in the name will fill the remaining name or
               extension part with '?'. */
            if (i < 8) {
                memset(&dst[i], '?', 8 - i);
            } else {
                memset(&dst[i], '?', 11 - i);
            }
        }
    }
    return rc;
}

/*!
 * \brief Convert a directory entry name to a visible file name.
 *
 * \param src Original name in the format 'name    ext'.
 * \param dst Converted name in the format 'name.ext'.
 *
 * \return 0 on success, 1 if the name contains wildcards or
 *         -1 if the name is malformed.
 */
void MakeVisibleName(CONST uint8_t * src, char *dst)
{
    int i;

    /* Replace the 0x05 cludge. */
    if (src[0] == PHAT_REM_NAMENT) {
        *dst++ = PHAT_REM_DIRENT;
    } else {
        *dst++ = src[0];
    }

    /* Copy the base name part up to the first space. */
    for (i = 1; i < 8 && src[i] != ' '; i++) {
        *dst++ = src[i];
    }

    /* Add the extension up to the first space. */
    for (i = 8; i < 11 && src[i] != ' '; i++) {
        if (i == 8) {
            *dst++ = '.';
        }
        *dst++ = src[i];
    }
    *dst = 0;
}

/*!
 * \brief Chop off the last component of a path.
 *
 * \param path Full path.
 * \param comp Points to a pointer which will be set to the last component
 *             within the full path.
 *
 * \return Pointer to a newly allocated string containing the parent path,
 *         or NULL in case of an error. The allocated buffer must be
 *         released by the caller.
 */
char *GetParentPath(CONST char *path, CONST char **comp)
{
    char *parent;
    int len;

    if ((*comp = strrchr(path, '/')) == NULL) {
        errno = EINVAL;
        return NULL;
    }

    (*comp)++;
    len = strlen(path) - strlen(*comp);
    if (len < 2) {
        len = 2;
    }
    if ((parent = malloc(len)) == NULL) {
        return NULL;
    }
    memcpy(parent, (void *)path, len - 1);
    parent[len - 1] = 0;

    return parent;
}

/*!
 * \brief Test for PHAT12/PHAT16 root directory.
 *
 * \param ndp Handle of an opened file to check.
 *
 * \return 0 if the file is not a fixed root directory.
 */
int IsFixedRootDir(NUTFILE * ndp)
{
    NUTDEVICE *dev = ndp->nf_dev;
    PHATVOL *vol = (PHATVOL *) dev->dev_dcb;
    PHATFILE *fcb;

    /* PHAT32 root directories are expandable. */
    if (vol->vol_type == 32) {
        return 0;
    }

    /* Root directory cluster number is 0 per definition. */
    fcb = ndp->nf_fcb;
    if (fcb->f_de_sect || fcb->f_dirent.dent_clusthi || fcb->f_dirent.dent_clust) {
        return 0;
    }
    return 1;
}

/*!
 * \brief Set file pointer back to zero.
 *
 * \param fcb Specifies the file control block.
 */
void PhatFilePosRewind(PHATFILE * fcb)
{
    /* Reset current pointer into the file. */
    fcb->f_pos = 0;
    /* Reset current cluster to the first cluster. */
    fcb->f_clust = fcb->f_dirent.dent_clusthi;
    fcb->f_clust <<= 16;
    fcb->f_clust += fcb->f_dirent.dent_clust;
    /* Reset position (sector number) within the cluster. */
    fcb->f_clust_pos = 0;
    /* Reset current position into the current sector. */
    fcb->f_sect_pos = 0;
}

/*!
 * \brief Move file pointer to a specified position.
 *
 * Moving beyond the current file size is not supported.
 *
 * \param nfp File descriptor.
 * \param pos Requested file position.
 *
 * \return 0 on success, -1 otherwise. In the latter case the position
 *         is unspecified.
 */
int PhatFilePosSet(NUTFILE * nfp, uint32_t pos)
{
    uint32_t dist;
    uint32_t step;
    uint32_t clust;
    PHATFILE *fcb = nfp->nf_fcb;
    NUTDEVICE *dev = nfp->nf_dev;
    PHATVOL *vol = (PHATVOL *) dev->dev_dcb;

    /* Simple case, rewind the file. */
    if (pos == 0) {
        PhatFilePosRewind(fcb);
        return 0;
    }

    /* We do not support seeking beyond the file size plus one. */
    if ((fcb->f_dirent.dent_attr & PHAT_FATTR_DIR) == 0 && pos > fcb->f_dirent.dent_fsize) {
        return -1;
    }

    /*
     * If the requested position is in front of the currect position,
     * then we start from the beginning.
     * TODO: Should check if we still are in the current cluster.
     */
    if (pos < fcb->f_pos) {
        PhatFilePosRewind(fcb);
        dist = pos;
    } else {
        dist = pos - fcb->f_pos;
    }

    for (;;) {
        if (fcb->f_sect_pos >= vol->vol_sectsz) {
            if (IsFixedRootDir(nfp)) {
                if (fcb->f_clust_pos + 1 >= vol->vol_rootsz) {
                    /* End of root directory, abort reading. */
                    break;
                }
                fcb->f_clust_pos++;
            }
            else {

                /*
                * We reached the end of the current sector. Move to the
                * next sector of the current cluster.
                */
                if (fcb->f_clust_pos + 1 >= vol->vol_clustsz) {
                    /*
                    * We reached the end of the current cluster. Move to
                    * the next cluster.
                    */
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
                } else {
                    fcb->f_clust_pos++;
                }
            }
            fcb->f_sect_pos = 0;
        }
        if (dist == 0) {
            break;
        }

        /* Calculate the number of bytes available in the current sector. */
        step = vol->vol_sectsz - fcb->f_sect_pos;
        if (step > dist) {
            step = dist;
        }
        fcb->f_sect_pos += step;
        fcb->f_pos += step;
        dist -= step;
    }
    return fcb->f_pos == pos ? 0 : -1;
}

/*@}*/
