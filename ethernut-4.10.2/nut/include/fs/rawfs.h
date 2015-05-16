#ifndef _FS_RAWFS_H_
#define _FS_RAWFS_H_
/*
 * Copyright (C) 2008-2009 by egnite GmbH.
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

/*!
 * \file fs/rawfs.h
 * \brief Raw device file system.
 *
 * \verbatim
 * $Id: rawfs.h 2376 2009-01-09 17:54:42Z haraldkipp $
 * \endverbatim
 */

#include <sys/device.h>

/*!
 * \addtogroup xgRawFs
 */
/*@{*/

typedef struct _RAWVOLUME RAWVOLUME;

/*!
 * \brief Volume info structure.
 */
struct _RAWVOLUME {
    /*! \brief Number of free clusters. */
    uint32_t vol_numfree;
    /*! \brief Mutual exclusion filesystem access semaphore. */
    HANDLE vol_fsmutex;
    /*! \brief Mutual exclusion I/O semaphore. */
    HANDLE vol_iomutex;
    /*! \brief Number of sectors in this volume. */
    uint32_t vol_sect_cnt;
    /*! \brief Bytes per sector. */
    size_t vol_sect_len;
    /*! \brief Sector data buffer. */
    uint8_t *vol_sect_buf;
    /*! \brief Buffered sector number. */
    uint32_t vol_sect_num;
    /*! \brief If not zero, buffer needs to be written. */
    int vol_sect_dirty;
};

/*!
 * \brief Raw device file descriptor.
 */
typedef struct _RAWFILE {
    /*! \brief File open mode flags. */
    uint32_t f_mode;
    /*! \brief Current position into the file. */
    uint32_t f_pos;
    /*! \brief Current sector. */
    uint32_t f_sect_num;
    /*! \brief Current position within sector. */
    size_t f_sect_pos;
    /*! \brief Sector buffer. */
    uint8_t *f_sect_buf;
} RAWFILE;

/*@}*/

extern NUTDEVICE devRawFs0;

#endif
