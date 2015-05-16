/*
 * Copyright (C) 2008 by egnite GmbH. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
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
 * \file pro/uxmlparse.c
 * \brief Micro XML low level parser routines.
 *
 * \verbatim
 * $Id: uxmlparse.c 2889 2010-02-07 18:20:25Z Astralix $
 * \endverbatim
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <pro/uxml.h>

/*!
 * \addtogroup xgUXML
 */
/*@{*/

/*!
 * \brief Retrieve the next token from an XML tag.
 *
 * \param data Pointer to the next character within the string to parse.
 * \param tkn  Pointer to the buffer that receives the next token.
 * \param size Size of the token buffer.
 *
 * \return Pointer to the character following the one parsed last.
 */
char *UxmlParseTag(char *data, char *tkn, size_t size)
{
    char *rc = NULL;

    /* Skip leading spaces. */
    while (isspace((unsigned char)*data)) {
        data++;
    }
    if (*data) {
        rc = data;

        if (isalnum((unsigned char)*rc)) {
            /* Collect name or number (well, almost). */
            while (isalnum((unsigned char)*rc) || *rc == '-' || *rc == ':' || *rc == '_') {
                if (size > 1) {
                    size--;
                    *tkn++ = *rc;
                }
                rc++;
            }
        } else if (*rc == '"' || *rc == '\'') {
            /* Collect quoted string. */
            rc++;
            while (*rc != *data) {
                if (size > 1) {
                    size--;
                    *tkn++ = *rc;
                }
                rc++;
            }
            rc++;
        } else {
            /* Return anything else as a single character. */
            *tkn++ = *rc++;
        }
        *tkn = 0;
    }
    return rc;
}

/*!
 * \brief Check if name matches filter list.
 *
 * \param name String to check against list.
 * \param filt Array of filter strings.
 *
 * \return 1 if the name matches at list one filter string. Otherwise
 *         0 is returned.
 */
int UxmlFilterMatch(char *name, char **filt)
{
    if (filt == NULL) {
        return 1;
    }
    while (*filt) {
        if (strcasecmp(name, *filt) == 0) {
            return 1;
        }
        filt++;
    }
    return 0;
}

/*@}*/
