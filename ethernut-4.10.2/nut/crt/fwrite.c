/*
 * Copyright (C) 2001-2005 by egnite Software GmbH. All rights reserved.
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
 * Revision 1.3  2005/10/24 10:09:36  haraldkipp
 * Integer division hack for ARM without CRT removed.
 *
 * Revision 1.2  2005/04/05 17:58:02  haraldkipp
 * Avoid integer division on ARM platform as long as we run without crtlib.
 *
 * Revision 1.1.1.1  2003/05/09 14:40:28  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.1  2003/02/04 17:49:06  harald
 * *** empty log message ***
 *
 */

#include <cfg/arch.h>

#include "nut_io.h"

#include <sys/nutdebug.h>
#include <io.h>

/*!
 * \addtogroup xgCrtStdio
 */
/*@{*/

/*!
 * \brief Write data to a stream.
 *
 * \param data   Pointer to items to be written.
 * \param size   Item size in bytes.
 * \param count  Number of items to write.
 * \param stream Pointer to a previously opened stream.
 *
 * \return The number of items written, which may be less than the 
 *         specified number.
 *
 * \warning The function will not check, if the stream pointer points
 *          to a valid stream.
 */
size_t fwrite(CONST void *data, size_t size, size_t count, FILE * stream)
{
    size_t rc;

    NUTASSERT(stream != NULL);
    if (size > 1)
        count *= size;
    if ((int) (rc = (size_t) _write(stream->iob_fd, data, count)) <= 0)
        return 0;
    if (size > 1)
        rc /= size;
    return rc;
}

/*@}*/
