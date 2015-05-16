#ifndef _DIRENT_H_
#define _DIRENT_H_

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
 * \file dirent.h
 * \brief Filesystem directory structure.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.3  2008/08/11 06:59:58  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.2  2006/01/05 16:45:20  haraldkipp
 * Dynamic NUTFILE allocation for detached block device.
 *
 * Revision 1.1  2005/02/05 20:37:17  haraldkipp
 * Peanut added
 *
 *
 * \endverbatim
 */

#include <stdint.h>
#include <sys/file.h>

/*!
 * \addtogroup xgFS
 */
/*@{*/

/*!
 * \name File System Directory Configuration
 *
 * The Nut/OS Configurator may be used to override the default values.
 */
/*@{*/

/*!
 * \brief UDP port of DHCP server.
 *
 * \showinitializer
 */
#ifndef MAXNAMLEN
#define MAXNAMLEN       255
#endif

/*@}*/

/*!
 * \brief Directory entry structure.
 *
 * This structure is returned by readdir(). It is not optimal
 * for Nut/OS, but compatible to *nix systems.
 */
struct dirent {
    uint32_t d_fileno;            /*!< \brief File number, unused. */
    uint16_t d_reclen;           /*!< \brief Record length, unused. */
    uint8_t d_type;              /*!< \brief File type, 0=regular, 1=directory. */
    uint8_t d_namlen;            /*!< \brief Length of string in d_name. */
    char d_name[MAXNAMLEN + 1]; /*!< \brief Name of this entry. */
};

/*!
 * \brief Internally used directory information structure.
 *
 * Applications should ignore its contents.
 */
typedef struct _dirdesc {
    NUTFILE *dd_fd;             /*!< \brief File descriptor associated with directory */
    char *dd_buf;               /*!< \brief Data buffer */
    size_t dd_len;              /*!< \brief Size of data buffer */
} DIR;

/*@}*/

__BEGIN_DECLS
/* */
extern DIR *opendir(CONST char *);
extern struct dirent *readdir(DIR *);
extern int closedir(DIR *);
__END_DECLS
/* */
#endif
