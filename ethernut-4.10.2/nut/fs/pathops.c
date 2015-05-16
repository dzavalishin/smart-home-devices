/*
 * Copyright (C) 2004-2007 by egnite Software GmbH. All rights reserved.
 *
 * Copyright (c) 1991
 *      The Regents of the University of California.  All rights reserved.
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
 * \file fs/pathops.c
 * \brief File system path operations.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.10  2009/03/05 22:02:26  freckle
 * unix emulation requires unistd.h as first include
 *
 * Revision 1.9  2009/01/17 11:26:46  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.8  2008/08/11 06:59:42  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.7  2008/04/01 10:16:02  haraldkipp
 * Implemented access() function.
 *
 * Revision 1.6  2007/08/29 13:34:13  haraldkipp
 * Added function for renaming files (contrib by ZACK).
 *
 * Revision 1.5  2006/10/08 16:48:09  haraldkipp
 * Documentation fixed
 *
 * Revision 1.4  2006/10/05 17:22:35  haraldkipp
 * Fixes bug #1281167. Thanks to Jukka Holappa.
 *
 * Revision 1.3  2006/03/16 15:25:24  haraldkipp
 * Changed human readable strings from u_char to char to stop GCC 4 from
 * nagging about signedness.
 *
 * Revision 1.2  2006/01/05 16:52:23  haraldkipp
 * The argument for the FS_STATUS ioctl now uses an individual structure.
 *
 * Revision 1.1  2005/02/05 20:35:21  haraldkipp
 * Peanut added
 *
 *
 * \endverbatim
 */

// unix emulation requires unistd.h as first include
#include <unistd.h>

#include <errno.h>
#include <sys/device.h>
#include <string.h>

#include <fs/fs.h>
#include <sys/stat.h>

/*!
 * \addtogroup xgFS
 */
/*@{*/

static int PathOperation(CONST char *path, int opcode)
{
    NUTDEVICE *dev;
    char dev_name[9];
    uint8_t nidx;
    CONST char *nptr = path;

    /*
     * Extract device name.
     */
    for (nidx = 0; *nptr && *nptr != ':' && nidx < 8; nidx++) {
        dev_name[nidx] = *nptr++;
    }
    dev_name[nidx] = 0;
    nptr++;

    /*
     * Get device structure of registered device. In later releases we
     * try to open a file on a root device.
     */
    if ((dev = NutDeviceLookup(dev_name)) == 0) {
        errno = ENOENT;
        return -1;
    }
    return (*dev->dev_ioctl) (dev, opcode, (void *) nptr);
}

/*!
 * \brief Check the accessibility of a file.
 *
 * \param path Pathname of the file to check.
 * \param what Access permission to check. Set to F_OK for existence
 *             check or any of the following values or'ed:
 *             - R_OK checks read permission
 *             - W_OK checks write permission
 *             - X_OK checks execute permission
 *
 * \return 0 on success, otherwise -1 is returned.
 *
 * \note Access permissions are not supported by all file systems.
 */
int access(CONST char *path, int what)
{
    struct stat s;

    if (stat(path, &s)) {
        return -1;
    }
    return 0;
}

/*!
 * \brief Reposition a file pointer.
 *
 * \param fh     Handle of an open file.
 * \param pos    Positioning value.
 * \param whence Positioning directive.
 *
 * \return Always -1 due to missing implementation.
 */
long lseek(int fh, long pos, int whence)
{
    //IOCTL_ARG3 args;

    //args.arg1 = (void *)fh;
    //args.arg2 = (void *)(unsigned int)pos;
    //args.arg3 = (void *)whence;
    //return (*dev->dev_ioctl) (dev, opcode, (void *)&args);
    return -1;
}

/*!
 * \brief Remove a directory.
 *
 * \param path Pathname of the directory. Must be the full pathname
 *             including the device, because Nut/OS doesn't support 
 *             relative paths.
 *
 * \return 0 if the remove succeeds, otherwise -1 is returned.
 */
int rmdir(CONST char *path)
{
    return PathOperation(path, FS_DIR_REMOVE);
}

/*!
 * \brief Remove a file entry.
 *
 * \return 0 if the remove succeeds, otherwise -1 is returned.
 */
int unlink(CONST char *path)
{
    return PathOperation(path, FS_FILE_DELETE);
}


/*!
 * \brief Get information about a specified file.
 *
 * \return 0 if the query succeeds, otherwise -1 is returned.
 */
int stat(CONST char *path, struct stat *s)
{
    NUTDEVICE *dev;
    char dev_name[9];
    uint8_t nidx;
    CONST char *nptr = path;
    FSCP_STATUS parms;

    /* Extract the device name. */
    for (nidx = 0; *nptr && *nptr != ':' && nidx < 8; nidx++) {
        dev_name[nidx] = *nptr++;
    }
    dev_name[nidx] = 0;

    /* Get device structure of registered device. */
    if ((dev = NutDeviceLookup(dev_name)) != 0) {
        if (*nptr == ':') {
            nptr++;
        }
        parms.par_path = nptr;
        parms.par_stp = s;
        return (*dev->dev_ioctl) (dev, FS_STATUS, (void *) &parms);
    }
    return -1;
}

/*!
 * \brief Get information about an opened file.
 *
 * \return Always -1 due to missing implementation.
 */
int fstat(int fh, struct stat *s)
{
    return -1;
}

/*!
 * \brief Create a directory entry.
 *
 * \return 0 on success, otherwise -1 is returned.
 */
int mkdir(CONST char *path, int mode)
{
    return PathOperation(path, FS_DIR_CREATE);
}

/*!
  * \brief Rename a file.
  *
  * New and old filename must contain the name of a registered device, 
  * followed by a colon and a filename. Moving a file from one device
  * to another is not supported.
  *
  * \param old_name Pathname of an existing file.
  * \param new_name New pathname.
  *
  * \return 0 for success or -1 to indicate an error.
  */
int rename(CONST char *old_name, CONST char *new_name)
{
    int rc = -1;
    NUTDEVICE *dev;
    char old_devname[9];
    char new_devname[9];
    uint8_t nidx;
    CONST char *nptr;
    FSCP_RENAME parms;   /* Structure used for renaming files. */

    /* Extract old file's device name. */
    nptr = old_name;
    for (nidx = 0; *nptr && *nptr != ':' && nidx < 8; nidx++, nptr++) {
        old_devname[nidx] = *nptr;
    }
    old_devname[nidx] = 0;

    /* Make sure a colon follows the device name. */
    if (*nptr++ == ':') {
        /* Assign the old file's name to the file rename structure. */
        parms.par_old = nptr;

        /* Extract new device name. */
        nptr = new_name;

        for (nidx = 0; *nptr && *nptr != ':' && nidx < 8; nidx++, nptr++) {
            new_devname[nidx] = *nptr;
        }
        new_devname[nidx] = 0;

        /* Make sure a colon follows the device name. */
        if (*nptr++ == ':') {
            /* Assign the new file's name to the file rename structure. */
            parms.par_new = nptr;

            /* Make sure both device names are the same. */
            if (strcmp(new_devname, old_devname) == 0) {
                /* Get device structure of registered device. */
                if ((dev = NutDeviceLookup(old_devname)) == 0) {
                    errno = ENOENT;
                } else {
                    rc = (*dev->dev_ioctl) (dev, FS_RENAME, (void *) &parms);
                }
            }
        }
    }
    return rc;
}

/*@}*/
