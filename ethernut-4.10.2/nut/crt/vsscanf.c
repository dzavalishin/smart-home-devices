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
 * Revision 1.5  2009/01/17 11:26:38  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.4  2006/05/05 15:43:07  freckle
 * Fixes for bugs #1477658 and #1477676
 *
 * Revision 1.3  2004/03/16 16:48:27  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 * Revision 1.2  2003/07/20 16:53:02  haraldkipp
 * Bugfix: sscanf is now working.
 *
 * Revision 1.1.1.1  2003/05/09 14:40:35  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.1  2003/02/04 17:49:09  harald
 * *** empty log message ***
 *
 */

#include "nut_io.h"

#include <string.h>
#include <io.h>

/*!
 * \addtogroup xgCrtStdio
 */
/*@{*/

static int _sgetb(int fd, void *buffer, size_t count)
{
    char **spp = (char **) ((uintptr_t) fd);
    char  *dst = (char*) buffer;
    size_t result = 0;
    
    while( count > 0 && **spp != 0) {
        *dst++ = *(*spp)++;  // increment not spp but *spp
        count--;
        result++;
    }
    
    return result;
}

/*!
 * \brief Read formatted data from a string.
 *
 * \param string Pointer to a string that contains the data.
 * \param fmt    Format string containing conversion specifications.
 * \param ap	 List of pointer arguments.
 *
 *
 * \return The number of fields successfully converted and assigned.
 *         The return value is EOF, if an error occurs or if the end 
 *         of the stream is reached before the first conversion.
 *
 */
int vsscanf(CONST char *string, CONST char *fmt, va_list ap)
{
    /* Bugfix kindly provided by Tomasz Niewegowski. */
    CONST char *ptr = string;

    return _getf(_sgetb, (int) ((uintptr_t) &ptr), fmt, ap);
}

/*@}*/
