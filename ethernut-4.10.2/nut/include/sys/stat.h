#ifndef _SYS_STAT_H_
#define	_SYS_STAT_H_

/*
 * Copyright (C) 2004-2005 by egnite Software GmbH. All rights reserved.
 *
 * Copyright (c) 1982, 1986, 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
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
 */

/*!
 * \file sys/stat.h
 * \brief File system status declarations.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.3  2008/08/11 07:00:27  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.2  2008/04/01 10:16:35  haraldkipp
 * Added S_ISDIR and S_ISREG macro.
 *
 * Revision 1.1  2005/02/05 20:37:18  haraldkipp
 * Peanut added
 *
 *
 * \endverbatim
 */

#include <stdint.h>
#include <time.h>

/*!
 * \addtogroup xgFS
 */
/*@{*/

/*!
 * \brief File status structure.
 */
struct stat {
    short st_ino;               /*!< \brief Block number */
    uint8_t st_mode;             /*!< \brief Mode flags */
    uint16_t st_nlink;           /*!< \brief Number of links */
    uint32_t st_size;             /*!< \brief Size */
    time_t st_mtime;            /*!< \brief Time of last modification */
};

#define S_ISDIR(m)  ((m) != 0)  /*!< \brief Check for directory. */
#define S_ISREG(m)  ((m) == 0)  /*!< \brief Check for regular file. */

/*@}*/

__BEGIN_DECLS
/* */
extern int stat(CONST char *path, struct stat *s);
extern int fstat(int fh, struct stat *s);
extern int mkdir(CONST char *path, int mode);
__END_DECLS
/* */
#endif
