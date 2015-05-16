#ifndef _FS_FS_H_
#define _FS_FS_H_

/*
 * Copyright (C) 2004-2005 by egnite Software GmbH. All rights reserved.
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
 *
 */

/*!
 * \file fs/fs.h
 * \brief File system access.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.5  2008/08/11 07:00:19  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.4  2007/08/29 13:34:14  haraldkipp
 * Added function for renaming files (contrib by ZACK).
 *
 * Revision 1.3  2006/01/05 16:45:25  haraldkipp
 * New ioctl functions.
 *
 * Revision 1.2  2005/08/05 11:28:25  olereinhardt
 * Corrected typo
 *
 * Revision 1.1  2005/02/05 20:37:18  haraldkipp
 * Peanut added
 *
 *
 * \endverbatim
 */

#include <sys/file.h>
#include <stdint.h>

/*!
 * \addtogroup xgFS
 */
/*@{*/

/*!
 * \name File System I/O Control Codes
 */
/*@{*/

/*!
 * \brief Obtain information about a specified file entry.
 */
#define FS_STATUS       0x1101

/*!
 * \brief Create a new directory entry.
 */
#define FS_DIR_CREATE   0x1111

/*!
 * \brief Remove a previously created directory entry.
 */
#define FS_DIR_REMOVE   0x1112

/*!
 * \brief Open a directory stream.
 */
#define FS_DIR_OPEN     0x1113

/*!
 * \brief Close a directory stream.
 */
#define FS_DIR_CLOSE    0x1114

/*!
 * \brief Read the next directory entry.
 */
#define FS_DIR_READ     0x1115

/*!
 * \brief Obtain information about an opened file.
 */
#define FS_FILE_STATUS  0x1121

/*!
 * \brief Remove a previously created file.
 */
#define FS_FILE_DELETE  0x1122

/*!
 * \brief Set a file pointer position.
 */
#define FS_FILE_SEEK    0x1123

/*@}*/

#define FS_VOL_MOUNT         0x1130
#define FS_VOL_UNMOUNT       0x1131
#define FS_RENAME        0x1132
#define NUTFS_UNLINK        0x1133
#define NUTFS_MKDIR         0x1134
#define NUTFS_RMDIR         0x1135

/*!
 * \brief General structure for two arguments.
 *
 * Used to pass two arguments to an I/O control function.
 */
typedef struct {
    void *arg1;
    void *arg2;
} IOCTL_ARG2;

/*!
 * \brief General structure for three arguments.
 *
 * Used to pass three arguments to an I/O control function.
 */
typedef struct {
    void *arg1;
    void *arg2;
    void *arg3;
} IOCTL_ARG3;

typedef struct _FSCP_VOL_MOUNT {
    /*! \brief Block device mount. */
    NUTFILE *fscp_bmnt;
    /*! \brief Partition type. */
    uint8_t fscp_part_type;
} FSCP_VOL_MOUNT;

typedef struct _FSCP_RENAME {
    CONST char *par_old;
    CONST char *par_new;
} FSCP_RENAME;

typedef struct _FSCP_STATUS {
    CONST char *par_path;
    struct stat *par_stp;
} FSCP_STATUS;

/*@}*/

#endif
