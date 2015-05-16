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
 * Revision 1.4  2009/02/13 14:52:05  haraldkipp
 * Include memdebug.h for heap management debugging support.
 *
 * Revision 1.3  2008/08/11 06:59:39  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.2  2004/09/08 10:19:05  haraldkipp
 * Todo: We need a better check for stdio channels!
 *
 * Revision 1.1.1.1  2003/05/09 14:40:24  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.1  2003/02/04 17:49:04  harald
 * *** empty log message ***
 *
 */


#include "nut_io.h"

#include <errno.h>
#include <io.h>
#include <stdlib.h>
#include <memdebug.h>

/*!
 * \addtogroup xgCrtStdio
 */
/*@{*/

/*!
 * \brief Close a stream.
 *
 * The calling thread may be suspended until all buffered output data 
 * has been written.
 *
 * \param stream Pointer to a previously opened stream.
 *
 * \return 0 if the stream is successfully closed, EOF otherwise.
 */
int fclose(FILE * stream)
{
    int rc = EOF;
    uint_fast8_t i;

    /*
     * Search the list first to detect bad stream pointer.
     */
    if (stream == 0) {
        errno = EBADF;
        return EOF;
    }
    for (i = 0; __iob[i] != stream;) {
        if (++i >= FOPEN_MAX) {
            errno = EBADF;
            return EOF;
        }
    }

    /*
     * Ignore unopened standard streams.
     */
    //if ((void *) stream < RAMSTART)
    //    return 0;

    /*
     * Close the file or device.
     */
    if (_close(stream->iob_fd) == 0)
        rc = 0;
    free(stream);
    __iob[i] = 0;

    return rc;
}

/*@}*/
