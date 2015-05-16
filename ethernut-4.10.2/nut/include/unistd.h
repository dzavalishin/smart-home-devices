/*
 * Copyright (C) 2004-2005 by egnite Software GmbH. All rights reserved.
 *
 * Copyright (c) 1991 The Regents of the University of California.
 * All rights reserved.
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
 * For additional information see http://www.ethernut.de/
 *
 */

/*!
 * \file unistd.h
 * \brief Miscellaneous function declarations.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.9  2009/03/05 22:16:57  freckle
 * use __NUT_EMULATION instead of __APPLE__, __linux__, or __CYGWIN__
 *
 * Revision 1.8  2008/04/01 13:34:20  haraldkipp
 * Fixed Unix emulation build.
 *
 * Revision 1.6  2008/01/31 09:15:21  haraldkipp
 * Added sleep() prototype.
 *
 * Revision 1.5  2005/04/19 15:46:45  freckle
 * added #include <compiler.h> as the implicit ordering is not sufficient
 * on our red hat machines and the cruisecontrol build fails
 *
 * Revision 1.4  2005/03/24 14:32:02  freckle
 * added creation of include/unistd_orig.h to configure
 * Added NUT_ wrapper for nut's unistd.h functions. Use <unistd_orig.h>
 * instead of "/usr/include/unistd.h". Relevant for unix emulation only.
 *
 * Revision 1.3  2005/02/23 13:42:28  freckle
 * Correctly include /usr/include/unistd.h for unix emulation
 *
 * Revision 1.2  2005/02/21 11:08:45  olereinhardt
 * For unix plattforms I added #include "/usr/include/unistd.h" since some
 * functions needed for unix_nutinit.c are missing in this file...
 *
 * Revision 1.1  2005/02/05 20:37:17  haraldkipp
 * Peanut added
 *
 *
 * \endverbatim
 */

#ifndef _UNISTD_NUT_H_

#include <compiler.h>  // required e.g. for CONST

#ifndef _UNISTD_VIRTUAL_H_
#define _UNISTD_VIRTUAL_H_

/* use native version on unix emulation */
#ifdef __NUT_EMULATION__

#include "unistd_orig.h"

// prefix all unistd calls with NUT_
#define access(...) NUT_access(__VA_ARGS__)
#define lseek(...) NUT_lseek(__VA_ARGS__)
#define rmdir(...) NUT_rmdir(__VA_ARGS__)
#define unlink(...) NUT_unlink(__VA_ARGS__)

#define sleep(...) NUT_sleep(__VA_ARGS__)

/* assure _UNISTD_H_ is set */
#undef  _UNISTD_H_
#define _UNISTD_H_

#define _UNISTD_H_

#else /* unix emulation */

/*!
 * \name Access functions
 */
/*@{*/
#define F_OK        0       /*!< \brief Test for existence of file. */
#define X_OK        0x01    /*!< \brief Test for execute or search permission. */
#define W_OK        0x02    /*!< \brief Test for write permission. */
#define R_OK        0x04    /*!< \brief Test for read permission. */
/*@}*/

#endif /* unix emulation */

__BEGIN_DECLS
/* */
extern int access(CONST char *path, int what);
extern long lseek(int fh, long pos, int whence);
extern int rmdir(CONST char *path);
extern int unlink(CONST char *path);

extern unsigned int sleep(unsigned int);
__END_DECLS
/* */


#endif /* _UNISTD_VIRTUAL_H_ */

#endif /* _UNISTD_H */
