/*
 * Copyright (C) 2001-2003 by egnite Software GmbH. All rights reserved.
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

/*
 * $Log$
 * Revision 1.11  2009/02/13 14:52:05  haraldkipp
 * Include memdebug.h for heap management debugging support.
 *
 * Revision 1.10  2009/02/06 15:40:29  haraldkipp
 * Using newly available strdup() and calloc().
 * Replaced NutHeap routines by standard malloc/free.
 * Replaced pointer value 0 by NULL.
 *
 * Revision 1.9  2009/01/17 11:26:46  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.8  2008/04/18 13:22:27  haraldkipp
 * Added type casts to fix ICCAVR V7.16 compile errors.
 *
 * Revision 1.7  2006/03/02 19:57:34  haraldkipp
 * ICCARM insists on a (void *) typecast for the second parameter of memcpy().
 *
 * Revision 1.6  2006/01/05 16:45:20  haraldkipp
 * Dynamic NUTFILE allocation for detached block device.
 *
 * Revision 1.5  2005/08/05 11:29:07  olereinhardt
 * Added IOCTL function with support for seek
 *
 * Revision 1.4  2004/03/18 11:37:06  haraldkipp
 * Deprecated functions removed
 *
 * Revision 1.3  2004/03/16 16:48:27  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 * Revision 1.2  2003/07/20 19:27:59  haraldkipp
 * Patch by Alessandro Zummo. Moves the urom filesystem filenames to
 * AVR's flash memory.
 *
 * Revision 1.1.1.1  2003/05/09 14:41:02  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.12  2003/04/21 16:58:20  harald
 * Make use of predefined eof
 *
 * Revision 1.11  2003/02/04 17:57:14  harald
 * Version 3 released
 *
 * Revision 1.10  2002/11/02 15:16:09  harald
 * Compiler warning avoided
 *
 * Revision 1.9  2002/06/26 17:29:13  harald
 * First pre-release with 2.4 stack
 *
 */

#include <sys/heap.h>
#include <sys/file.h>
#include <sys/device.h>

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <memdebug.h>

#include <fs/fs.h>
#include <dev/urom.h>
#include <fs/uromfs.h>

/*
static int UromRead(NUTFILE * fp, void *buffer, int size);
static int UromWrite(NUTFILE * fp, CONST void *buffer, int len);
#ifdef __HARVARD_ARCH__
static int UromWrite_P(NUTFILE * fp, PGM_P buffer, int len);
#endif
static NUTFILE *UromOpen(NUTDEVICE * dev, CONST char *name, int mode,
                         int acc);
static int UromClose(NUTFILE * fp);
static long UromSize(NUTFILE * fp);
*/
/*!
 * \addtogroup xgurom
 */
/*@{*/

static int UromSeek(NUTFILE * fp, long *pos, int whence)
{
    ROMFILE *romf = fp->nf_fcb;
    ROMENTRY *rome = romf->romf_entry;

    int rc = 0;
    long npos = *pos;

    switch (whence) {
    case SEEK_CUR:
        npos += romf->romf_pos;
        break;
    case SEEK_END:
        npos += rome->rome_size;
        break;
    }

    if (npos < 0 || npos > rome->rome_size) {
        rc = EINVAL;
    } else {
        romf->romf_pos = npos;
        *pos = npos;
    }
    return rc;
}

/*!
 * \brief Read from device.
 */
static int UromRead(NUTFILE * fp, void *buffer, int size)
{
    ROMFILE *romf = fp->nf_fcb;
    ROMENTRY *rome = romf->romf_entry;

    if ((unsigned int) size > rome->rome_size - romf->romf_pos)
        size = rome->rome_size - romf->romf_pos;
    if (size) {
        memcpy_P(buffer, (PGM_P)(rome->rome_data + romf->romf_pos), size);
        romf->romf_pos += size;
    }
    return size;
}

/*!
 * \brief Write data to a file.
 *
 * \return Always returns -1.
 */
static int UromWrite(NUTFILE * fp, CONST void *buffer, int len)
{
    return -1;
}

/*!
 * \brief Write data from progam space to a file.
 *
 * \return Always returns -1.
 */
#ifdef __HARVARD_ARCH__
static int UromWrite_P(NUTFILE * fp, PGM_P buffer, int len)
{
    return -1;
}
#endif


/*!
 * \brief Open a file.
 */
static NUTFILE *UromOpen(NUTDEVICE * dev, CONST char *name, int mode,
                         int acc)
{
    NUTFILE *fp = malloc(sizeof(NUTFILE));
    ROMENTRY *rome;
    ROMFILE *romf = 0;

    if (fp == 0) {
        errno = ENOMEM;
        return NUTFILE_EOF;
    }

    for (rome = romEntryList; rome; rome = rome->rome_next) {
        if (strcmp_P(name, rome->rome_name) == 0)
            break;
    }
    if (rome) {
        if ((romf = calloc(1, sizeof(ROMFILE))) != 0)
            romf->romf_entry = rome;
        else
            errno = ENOMEM;
    } else
        errno = ENOENT;

    if (romf) {
        fp->nf_next = 0;
        fp->nf_dev = dev;
        fp->nf_fcb = romf;
    } else {
        free(fp);
        fp = NUTFILE_EOF;
    }

    return fp;
}

/*!
 * \brief Close a file.
 */
static int UromClose(NUTFILE * fp)
{
    if (fp && fp != NUTFILE_EOF) {
        if (fp->nf_fcb)
            free(fp->nf_fcb);
        free(fp);
    }
    return 0;
}

/*!
 * \brief Query file size.
 */
static long UromSize(NUTFILE * fp)
{
    ROMFILE *romf = fp->nf_fcb;
    ROMENTRY *rome = romf->romf_entry;

    return (long) rome->rome_size;
}

/*!
 * \brief Device specific functions.
 */
int UromIOCtl(NUTDEVICE * dev, int req, void *conf)
{
    int rc = -1;

    switch (req) {
    case FS_FILE_SEEK:
        UromSeek((NUTFILE *) ((IOCTL_ARG3 *) conf)->arg1,      /* */
                     (long *) ((IOCTL_ARG3 *) conf)->arg2,      /* */
                     (int) ((IOCTL_ARG3 *) conf)->arg3);
        break;
    }
    return rc;
}


/*!
 * \brief UROM device information structure.
 *
 * All this UROM stuff is so simple, that it even mimics
 * its own device driver.
 */
NUTDEVICE devUrom = {
    0,                          /*!< Pointer to next device. */
    {'U', 'R', 'O', 'M', 0, 0, 0, 0, 0},        /*!< Unique device name. */
    IFTYP_ROM,                  /*!< Type of device. Obsolete. */
    0,                          /*!< Base address. Unused. */
    0,                          /*!< First interrupt number. Unused. */
    0,                          /*!< Interface control block. Unused. */
    0,                          /*!< Driver control block. Unused. */
    0,                          /*!< Driver initialization routine. Not supported. */
    UromIOCtl,                  /*!< Driver specific control function. Not supported. */
    UromRead,                   /*!< Read data from a file. */
    UromWrite,                  /*!< Write data to a file. */
#ifdef __HARVARD_ARCH__
    UromWrite_P,                /*!< Write data from program space to a file. */
#endif
    UromOpen,                   /*!< Open a file. */
    UromClose,                  /*!< Close a file. */
    UromSize                    /*!< Return file size. */
};

/*@}*/
