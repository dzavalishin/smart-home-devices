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
 * Revision 1.6  2009/02/13 14:52:05  haraldkipp
 * Include memdebug.h for heap management debugging support.
 *
 * Revision 1.5  2009/01/17 11:26:38  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.4  2008/08/11 06:59:40  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.3  2004/09/08 10:19:05  haraldkipp
 * Todo: We need a better check for stdio channels!
 *
 * Revision 1.2  2004/03/16 16:48:27  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 * Revision 1.1.1.1  2003/05/09 14:40:27  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.1  2003/02/04 17:49:05  harald
 * *** empty log message ***
 *
 */

#include "nut_io.h"

#include <errno.h>
#include <io.h>
#include <fcntl.h>
#include <stdlib.h>
#include <memdebug.h>

/*!
 * \addtogroup xgCrtStdio
 */
/*@{*/

/*!
 * \brief Reassign a stream.
 *
 * \param name The name of a registered device, optionally followed by a
 *             colon and a filename.
 * \param mode Specifies the access mode.
 *	       \li \c "r" Read only.
 *	       \li \c "w" Write only.
 *	       \li \c "a" Write only at the end of file.
 *	       \li \c "r+" Read and write existing file.
 *	       \li \c "w+" Read and write, destroys existing file contents.
 *	       \li \c "a+" Read and write, preserves existing file contents.
 *             \li \c "b" May be appended to any of the above strings to 
 *                        specify binary access.
 *
 * \param stream Pointer to a previously opened stream.
 *
 * \return A pointer to the open stream or a null pointer to indicate 
 *         an error.
 */
FILE *freopen(CONST char *name, CONST char *mode, FILE * stream)
{
    int mflags = _O_TEXT;
    uint_fast8_t i;

    /*
     * Translate file mode.
     */
    if ((mflags = _fmode(mode)) == EOF)
        return 0;

    for (i = 0; __iob[i] != stream;)
        if (++i >= FOPEN_MAX) {
            errno = EBADF;
            return 0;
        }

    //if (stream)
    //    _close(stream->iob_fd);
    //else 
    if ((__iob[i] = malloc(sizeof(FILE))) == 0)
        return 0;

    if ((__iob[i]->iob_fd = _open(name, mflags)) == -1) {
        free(__iob[i]);
        if (i < 3)
            __iob[i] = (FILE *) ((uintptr_t) (i + 1));
        else
            __iob[i] = 0;
        return 0;
    }

    __iob[i]->iob_mode = mflags;
    __iob[i]->iob_flags = 0;
    __iob[i]->iob_unget = 0;

    return __iob[i];
}

/*@}*/
