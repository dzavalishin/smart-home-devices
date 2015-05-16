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
 * Revision 1.2  2009/01/17 15:37:52  haraldkipp
 * Added some NUTASSERT macros to check function parameters.
 *
 * Revision 1.1.1.1  2003/05/09 14:40:34  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.1  2003/02/04 17:49:09  harald
 * *** empty log message ***
 *
 */

#include "nut_io.h"

#include <string.h>
#include <io.h>

#include <sys/nutdebug.h>
#include <sys/heap.h>

/*!
 * \addtogroup xgCrtStdio
 */
/*@{*/

/*!
 * \brief Write argument list to a stream using a given format.
 *
 * Similar to vfprintf() except that the format string is located in
 * program memory.
 *
 * \param stream Pointer to a previously opened stream.
 * \param fmt    Format string in program space containing conversion 
 *               specifications.
 * \param ap     List of arguments.
 *
 * \return The number of characters written or a negative value to
 *         indicate an error.
 */
int vfprintf_P(FILE * stream, PGM_P fmt, va_list ap)
{
    int rc;
    char *rp;
    size_t rl;

    NUTASSERT(stream != NULL);

    rl = strlen_P(fmt) + 1;
    if ((rp = NutHeapAlloc(rl)) == 0)
        return -1;
    memcpy_P(rp, fmt, rl);
    rc = _putf(_write, stream->iob_fd, rp, ap);
    NutHeapFree(rp);

    return rc;
}

/*@}*/
