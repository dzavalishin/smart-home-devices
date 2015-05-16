#ifndef FS_PHATDIR_H_
#define FS_PHATDIR_H_

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
 * \file fs/phatdir.h
 * \brief PHAT file system.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.4  2008/08/11 07:00:19  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.3  2006/10/08 16:42:56  haraldkipp
 * Not optimal, but simple and reliable exclusive access implemented.
 * Fixes bug #1486539. Furthermore, bug #1567790, which had been rejected,
 * had been reported correctly and is now fixed.
 *
 * Revision 1.2  2006/06/18 16:41:55  haraldkipp
 * Support for long filenames (VFAT) added.
 * New function PhatDirReleaseChain() simplifies code.
 * Static function PhatDirOpenParentPath() replaced by global
 * PhatDirOpenParent().
 * Added const attribute to path parameter of PhatDirOpen().
 *
 * Revision 1.1  2006/01/05 16:32:52  haraldkipp
 * First check-in.
 *
 *
 * \endverbatim
 */

#include <dirent.h>
#include <sys/stat.h>

/*!
 * \addtogroup xgPhatDir
 */
/*@{*/

/*!
 * \brief Maximum length of a base file name.
 */
#ifndef PHAT_MAX_NAMELEN
#define PHAT_MAX_NAMELEN    255
#endif

#define PHAT_REM_DIRENT  0xE5
#define PHAT_REM_NAMENT  0x05

/*!
 * \brief Structure of a directory entry.
 *
 * Once created, this structure will never change if it points
 * to another directory. Except if the entry is removed.
 */
typedef struct __attribute__ ((packed)) _PHATDIRENT {
    /*! \brief File name, padded with blanks. */
    uint8_t dent_name[11];
    /*! \brief File attributes. 
     *
     * See PHAT_FATTR_ flags. 
     */
    uint8_t dent_attr;
    /*! \brief Reserved byte, possibly used by NT. */
    uint8_t dent_rsvdnt;
    /*! \brief Hundredth of seconds of file creation time. */
    uint8_t dent_ctsecs;
    /*! \brief File creation time. */
    uint16_t dent_ctime;
    /*! \brief File creation date. */
    uint16_t dent_cdate;
    /*! \brief Last file access date. */
    uint16_t dent_adate;
    /*! \brief High bytes of first cluster of this file.
     *
     * If used with the double dot entry, dent_clusthi and dent_clust
     * must be zero if the entry points to the root directory.
     *
     * The same is true when pointing to a normal file with zero
     * length.
     */
    uint16_t dent_clusthi;
    /*! \brief Last file modification time. */
    uint16_t dent_mtime;
    /*! \brief Last file modification date. */
    uint16_t dent_mdate;
    /*! \brief First cluster used.
     *
     * This is zero for empty files.
     */
    uint16_t dent_clust;
    /*! \brief Size of the file in bytes. 
     *
     * For directories, this is always zero.
     */
    uint32_t dent_fsize;
} PHATDIRENT;

/*!
 * \brief Structure of an extended directory entry.
 *
 * Used for long filenames.
 */
typedef struct __attribute__ ((packed)) _PHATXDIRENT {
    /*! \brief Sequence number. */
    uint8_t xdent_seq;
    /*! \brief Unicode characters 1-5. */
    uint16_t xdent_uname_1_5[5];
    /*! \brief Attribut. */
    uint8_t xdent_attr;
    /*! \brief Type. */
    uint8_t xdent_rsvd;
    /*! \brief Checksum. */
    uint8_t xdent_cks;
    /*! \brief Unicode characters 6-11. */
    uint16_t xdent_uname_6_11[6];
    /*! \brief Starting cluster. */
    uint16_t xdent_clust;
    /*! \brief Unicode characters 12-13. */
    uint16_t xdent_uname_12_13[2];
} PHATXDIRENT;

typedef struct _PHATFIND {
    PHATDIRENT phfind_ent;
    uint32_t phfind_pos;
    /*! Number of entries used for the long filename. */
    int phfind_xcnt;
    char phfind_name[PHAT_MAX_NAMELEN];
} PHATFIND;

/*@}*/

__BEGIN_DECLS
/* Prototypes */
extern int PhatDirEntryCreate(NUTFILE * ndp, CONST char *name, int acc, PHATDIRENT * dirent);

extern int PhatDirEntryUpdate(NUTFILE * ndp);

extern int PhatDirReleaseChain(NUTDEVICE * dev, PHATDIRENT * dent);

extern int PhatDirDelEntry(NUTDEVICE * dev, CONST char *path, uint32_t flags);
extern int PhatDirRenameEntry(NUTDEVICE * dev, CONST char *old_path, CONST char *new_path);

extern NUTFILE *PhatDirOpen(NUTDEVICE * dev, CONST char *dpath);
extern NUTFILE *PhatDirOpenParent(NUTDEVICE * dev, CONST char *path, CONST char **basename);
extern int PhatDirEntryFind(NUTFILE * nfp, CONST char *spec, uint32_t flags, PHATFIND * srch);
extern int PhatDirRead(DIR * dir);

extern int PhatDirCreate(NUTDEVICE * dev, char *path);
extern int PhatDirRemove(NUTDEVICE * dev, char *path);

extern int PhatDirEntryStatus(NUTDEVICE * dev, CONST char *path, struct stat *s);

__END_DECLS
/* End of prototypes */
#endif
