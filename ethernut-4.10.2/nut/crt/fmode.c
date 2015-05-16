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
 * Revision 1.4  2009/01/17 15:37:52  haraldkipp
 * Added some NUTASSERT macros to check function parameters.
 *
 * Revision 1.3  2008/04/01 10:13:15  haraldkipp
 * Rewritten to fix bug #1903303.
 *
 * Revision 1.2  2006/01/05 16:50:13  haraldkipp
 * Files opened with mode 'a' or 'w' will be created, if they didn't exist.
 * Files opened with 'w' will be truncated, if they exist.
 *
 * Revision 1.1.1.1  2003/05/09 14:40:26  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.1  2003/02/04 17:49:05  harald
 * *** empty log message ***
 *
 */

#include "nut_io.h"

#include <sys/nutdebug.h>
#include <errno.h>
#include <fcntl.h>

/*!
 * \addtogroup xgCrtStdio
 */
/*@{*/

/*! \internal
 * \brief Return the flags for a given mode string.
 *
 * \return Flags or EOF to indicate an error.
 */
int _fmode(CONST char *mode)
{
    int mflags;
    int pflags;
    int bflag = _O_TEXT;

    NUTASSERT(mode != NULL);

    switch (*mode) {
    case 'r':
        mflags = _O_RDONLY;
        pflags = _O_RDWR;
        break;
    case 'w':
        mflags = _O_WRONLY | _O_CREAT | _O_TRUNC;
        pflags = _O_RDWR | _O_CREAT | _O_TRUNC;
        break;
    case 'a':
        mflags = _O_WRONLY | _O_CREAT | _O_APPEND;
        pflags = _O_RDWR | _O_CREAT | _O_APPEND;
        break;
    default:
        errno = EINVAL;
        return EOF;
    }
    while (*++mode) {
        switch (*mode) {
        case '+':
            mflags = pflags;
            break;
        case 'b':
            bflag = _O_BINARY;
            break;
        default:
            errno = EINVAL;
            return EOF;
        }
    }
    return (mflags | bflag);
}

/*@}*/
