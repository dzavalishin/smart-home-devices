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
 *-
 * Parts are
 *
 * Copyright (c) 1989, 1993
 *      The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/*!
 * \file fs/dirent.c
 * \brief Directory functions.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.8  2009/02/13 14:52:05  haraldkipp
 * Include memdebug.h for heap management debugging support.
 *
 * Revision 1.7  2008/08/11 06:59:42  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.6  2006/08/01 07:44:22  haraldkipp
 * Already disabled NUTFILE structure allocation finally removed from
 * opendir().
 *
 * Revision 1.5  2006/04/07 12:51:04  haraldkipp
 * Memory hole fixed.
 *
 * Revision 1.4  2006/03/16 15:25:24  haraldkipp
 * Changed human readable strings from u_char to char to stop GCC 4 from
 * nagging about signedness.
 *
 * Revision 1.3  2006/01/05 16:45:20  haraldkipp
 * Dynamic NUTFILE allocation for detached block device.
 *
 * Revision 1.2  2005/02/07 18:57:47  haraldkipp
 * ICCAVR compile errors fixed
 *
 * Revision 1.1  2005/02/05 20:35:21  haraldkipp
 * Peanut added
 *
 *
 * \endverbatim
 */

#include <sys/device.h>

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <memdebug.h>

#include <fs/fs.h>

#include <dirent.h>

/*!
 * \addtogroup xgFSDir
 */
/*@{*/

/*!
 * \brief Open a directory stream.
 *
 * Opens a directory and associates a directory stream with it.
 *
 * \param name Pathname of the directory. Must be the full pathname
 *             including the device, because Nut/OS doesn't support 
 *             relative paths.
 *
 * \return A pointer used to identify the directory stream in subsequent 
 *         operations. NULL is returned if the directory cannot be accessed, 
 *         or if it cannot allocate enough memory.
 */
DIR *opendir(CONST char *name)
{
    DIR *dir = 0;
    NUTDEVICE *dev;
    char dev_name[9];
    uint8_t nidx;
    CONST char *nptr = name;

    /* Extract the device name. */
    for (nidx = 0; *nptr && *nptr != ':' && nidx < 8; nidx++) {
        dev_name[nidx] = *nptr++;
    }
    dev_name[nidx] = 0;

    /* Get device structure of registered device. */
    if ((dev = NutDeviceLookup(dev_name)) == 0) {
        errno = ENODEV;
        return 0;
    }

    /* Allocate and initialize the stream. */
    if ((dir = malloc(sizeof(DIR))) == 0) {
        errno = ENOMEM;
        return 0;
    }
    memset(dir, 0, sizeof(DIR));

    /* Allocate and initialize the data buffer. */
    if ((dir->dd_len = strlen(name + 1)) < sizeof(struct dirent)) {
        dir->dd_len = sizeof(struct dirent);
    }
    if ((dir->dd_buf = malloc(dir->dd_len)) == 0) {
        free(dir);
        errno = ENOMEM;
        return 0;
    }
    if (*nptr == ':') {
        nptr++;
    }
    strcpy(dir->dd_buf, nptr);

    /* Call the proper IOCTL. */
    if ((*dev->dev_ioctl) (dev, FS_DIR_OPEN, dir)) {
        free(dir->dd_buf);
        free(dir);
        dir = 0;
    }
    return dir;
}

/*!
 * \brief Close a directory stream.
 *
 * Closes the given directory stream and frees any allocated memory.
 *
 * \param dir Pointer to the directory stream.
 *
 * \return 0 on success. On failure, -1 is returned and the global 
 *         variable errno is set to indicate the error.
 */
int closedir(DIR * dir)
{
    NUTDEVICE *dev;

    if (dir) {
        dev = dir->dd_fd->nf_dev;
        (*dev->dev_ioctl) (dev, FS_DIR_CLOSE, dir);
        if (dir->dd_buf) {
            free(dir->dd_buf);
        }
        free(dir);
    }
    return 0;
}

/*!
 * \brief Get the next directory entry.
 *
 * \param dir Pointer to the directory stream.
 *
 * \return A pointer to the next directory entry. It returns NULL upon 
 *         reaching the end of the directory.
 */
struct dirent *readdir(DIR * dir)
{
    NUTDEVICE *dev = dir->dd_fd->nf_dev;

    if ((*dev->dev_ioctl) (dev, FS_DIR_READ, dir)) {
        return 0;
    }
    return (struct dirent *) (dir->dd_buf);
}

/*@}*/
