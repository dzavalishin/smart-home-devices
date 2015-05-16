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
 * Revision 1.5  2009/01/17 15:37:52  haraldkipp
 * Added some NUTASSERT macros to check function parameters.
 *
 * Revision 1.4  2008/09/18 09:45:25  haraldkipp
 * Reading 0xFF returned EOF. Fixed.
 *
 * Revision 1.3  2008/08/11 06:59:40  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.2  2007/04/12 09:09:57  haraldkipp
 * Support for feof() and ferror() was missing.
 *
 * Revision 1.1.1.1  2003/05/09 14:40:25  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.2  2003/04/21 17:33:28  harald
 * Return correct result for character 0xFF
 *
 * Revision 1.1  2003/02/04 17:49:04  harald
 * *** empty log message ***
 *
 */

#include "nut_io.h"

#include <sys/nutdebug.h>
#include <io.h>

/*!
 * \addtogroup xgCrtStdio
 */
/*@{*/

/*!
 * \brief Read a character from a stream.
 *
 * \param stream Pointer to a previously opened stream.
 *
 * \return Character read or EOF to indicate an error or end of file. In 
 *         the latter case feof() or ferror() can be used to determine
 *         the cause of the failure.
 *
 * \warning The function will not check, if the stream pointer points
 *          to a valid stream.
 */
int fgetc(FILE * stream)
{
    uint8_t ch;
    int rc;

    NUTASSERT(stream != NULL);
    if (stream->iob_flags & _IOUNG) {
        stream->iob_flags &= ~_IOUNG;
        return stream->iob_unget;
    }
    if ((rc = _read(stream->iob_fd, &ch, 1)) != 1) {
        if (rc) {
            /* Error. */
            stream->iob_flags |= _IOERR;
        }
        else {
            /* Time out or end of file. */
            stream->iob_flags |= _IOEOF;
        }
        return EOF;
    }
    return (int) ch;
}

/*@}*/
