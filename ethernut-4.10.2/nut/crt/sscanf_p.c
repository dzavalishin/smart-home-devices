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
 * Revision 1.2  2008/04/18 13:22:27  haraldkipp
 * Added type casts to fix ICCAVR V7.16 compile errors.
 *
 * Revision 1.1.1.1  2003/05/09 14:40:33  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.1  2003/02/04 17:49:09  harald
 * *** empty log message ***
 *
 */

#include "nut_io.h"

/*!
 * \addtogroup xgCrtStdio
 */
/*@{*/

/*!
 * \brief Read formatted data from a string.
 *
 * Similar to sscanf() except that the format string is located in
 * program space.
 *
 * \param string Pointer to the string that contains the data.
 * \param fmt    Format string in program space containing conversion 
 *               specifications.
 *
 * \return The number of fields successfully converted and assigned.
 *         The return value is EOF, if an error occurs or if the end 
 *         of the stream is reached before the first conversion.
 */
int sscanf_P(CONST char *string, CONST char *fmt, ...)
{
    int rc;
    va_list ap;

    va_start(ap, fmt);
    rc = vsscanf_P(string, (PGM_P)fmt, ap);
    va_end(ap);

    return rc;
}

/*@}*/
