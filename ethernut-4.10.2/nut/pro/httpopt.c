/*
 * Copyright (C) 2008 by egnite GmbH. All rights reserved.
 * Copyright (C) 2001-2007 by egnite Software GmbH. All rights reserved.
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
 */

/*!
 * \file pro/httpopt.c
 * \brief Optional HTTP routines.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.4  2009/02/13 14:52:05  haraldkipp
 * Include memdebug.h for heap management debugging support.
 *
 * Revision 1.3  2008/08/11 07:00:35  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.2  2008/05/13 21:46:06  thiagocorrea
 * Fix a few documentation typos.
 *
 * Revision 1.1  2008/04/01 10:11:35  haraldkipp
 * Added the new, enhanced httpd API library.
 * Bugs #1839026 and #1839029 fixed.
 *
 *
 * \endverbatim
 */

#include <cfg/arch.h>

#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdlib.h>
#include <memdebug.h>

#include <sys/heap.h>
#include <sys/version.h>

#include "dencode.h"

#include <pro/httpd.h>

/*!
 * \addtogroup xgHTTPD
 */
/*@{*/

extern MIMETYPES mimeTypes[];
extern char *http_root;

/*!
 * \brief Set the mime type handler for a specified file extension.
 *
 * This is the function that handles / sends a specific file type to the
 * client. Specially used for server side includes (shtml files)
 *
 * \param extension Filename extension the handler should be registered for
 * \param handler pointer to a function of the type void (u_char filename)
 * \return 1 on error or 0 on success
 */

uint8_t NutSetMimeHandler(char *extension, void (*handler)(FILE *stream, int fd, int file_len, char *http_root, REQUEST *req))
{
    size_t i;

    if (extension == NULL)
        return 1;
    for (i = 0; mimeTypes[i].mtyp_ext; i++)
        if (strcasecmp(extension, mimeTypes[i].mtyp_ext) == 0) {
            mimeTypes[i].mtyp_handler = handler;
            return 0;
        }
    return 1;
}

/*!
 * \brief URLEncodes a string
 *
 * \param str String to encode
 *
 * \return A new allocated encoded string, or NULL if str is null, or
 *         if there's not enough memory for the new string.
 *
 * \note Remember to free() to the returned string.
 */
char *NutHttpURLEncode(char *str)
{
    static char *hexdigits = "0123456789ABCDEF";
    register char *ptr1, *ptr2;
    char *encstring;
    int numEncs = 0;

    if (!str)
        return NULL;

    /* Calculate how many characters we need to encode */
    for (ptr1 = str; *ptr1; ptr1++) {
        if (!isalnum((unsigned char)*ptr1) || *ptr1 == '%' || *ptr1 == '&'|| *ptr1 == '+' ||
		*ptr1 == ',' || *ptr1 == ':' || *ptr1 == ';'|| *ptr1 == '='|| *ptr1 == '?'|| *ptr1 == '@')
            numEncs++;
    }
    /* Now we can calculate the encoded string length */
    encstring = (char *) malloc(strlen(str) + 2 * numEncs + 1);
    if (encstring) {
        /* Encode the string. ptr1 refers to the original string,
         * and ptr2 refers to the new string. */
        ptr2 = encstring;
        for (ptr1 = str; *ptr1; ptr1++) {
		    if (isalnum((unsigned char)*ptr1) || *ptr1 == '%' || *ptr1 == '&'|| *ptr1 == '+' ||
		    *ptr1 == ',' || *ptr1 == ':' || *ptr1 == ';'|| *ptr1 == '='|| *ptr1 == '?'|| *ptr1 == '@')
                *ptr2++ = *ptr1;
            else {
                *ptr2++ = '%';
                *ptr2++ = hexdigits[(*ptr1 >> 4)];
                *ptr2++ = hexdigits[*ptr1 & 0x0F];
            }
        }
        *ptr2++ = 0;
    }
    return encstring;
}

/*!
 * \brief Parses the QueryString
 *
 * Reads the query from input stream and parses it into
 * name/value table. To save RAM, this method allocated ram and
 * uses req_query to store the input data. Then it creates a table
 * of pointers into the req_query buffer.
 *
 * \param stream Input stream
 * \param req Request object to parse
 */

void NutHttpProcessPostQuery(FILE *stream, REQUEST * req)
{
    int got;
    register int i;
    register char *ptr;

    if (req->req_query != NULL)
        return;

    if (!stream)
        return;

    if (req->req_method == METHOD_POST) {
        req->req_query = malloc(req->req_length+1);
        if (req->req_query == NULL) {
            /* Out of memory */
            req->req_numqptrs = 0;
            return;
        }
        memset(req->req_query, 0, req->req_length+1);
        i = 0;
        while (i < req->req_length) {
            got = fread(&req->req_query[i], 1, req->req_length-i, stream);
            if (got <= 0) {
                free(req->req_query);
                req->req_numqptrs = 0;
                req->req_query = NULL;
                return;
            }
            i += got;
        }
    } else return;

    req->req_numqptrs = 1;
    for (ptr = req->req_query; *ptr; ptr++)
        if (*ptr == '&')
            req->req_numqptrs++;

    req->req_qptrs = (char **) malloc(sizeof(char *) * (req->req_numqptrs * 2));
    if (!req->req_qptrs) {
        /* Out of memory */
        free(req->req_query);
        req->req_numqptrs = 0;
        req->req_query = NULL;
        return;
    }
    req->req_qptrs[0] = req->req_query;
    req->req_qptrs[1] = 0;
    for (ptr = req->req_query, i = 2; *ptr; ptr++) {
        if (*ptr == '&') {
            req->req_qptrs[i] = ptr + 1;
            req->req_qptrs[i + 1] = NULL;
            *ptr = 0;
            i += 2;
        }
    }

    for (i = 0; i < req->req_numqptrs; i++) {
        for (ptr = req->req_qptrs[i * 2]; *ptr; ptr++) {
            if (*ptr == '=') {
                req->req_qptrs[i * 2 + 1] = ptr + 1;
                *ptr = 0;
                NutHttpURLDecode(req->req_qptrs[i * 2 + 1]);
                break;
            }
        }
        NutHttpURLDecode(req->req_qptrs[i * 2]);
    }
}

/*!
 * \brief Gets a request parameter value by name
 *
 * \param req Request object
 * \param name Name of parameter
 *
 * \return Pointer to the parameter value.
 */
char *NutHttpGetParameter(REQUEST * req, char *name)
{
    int i;
    for (i = 0; i < req->req_numqptrs; i++)
        if (strcmp(req->req_qptrs[i * 2], name) == 0)
            return req->req_qptrs[i * 2 + 1];
    return NULL;
}

/*!
 * \brief Gets the number of request parameters
 *
 * The following code fragment retrieves all name/value pairs of the
 * request.
 *
 * \code
 * #include &lt;pro/httpd.h&gt;
 *
 * char *name;
 * char *value;
 * int i;
 * int n = NutHttpGetParameterCount(req);
 *
 * for (i = 0; i &lt; n; i++) {
 *     name = NutHttpGetParameterName(req, i);
 *     value = NutHttpGetParameterValue(req, i);
 * }
 * \endcode
 *
 * \param req Request object
 *
 * \return The number of request parameters
 */
int NutHttpGetParameterCount(REQUEST * req)
{
    return req->req_numqptrs;
}

/*!
 * \brief Gets the name of a request parameter
 *
 * \param req Request object
 * \param index Index of the requested parameter.
 *
 * \return Pointer to the parameter name at the given index,
 *         or NULL if index is out of range.
 */
char *NutHttpGetParameterName(REQUEST * req, int index)
{
    if (index < 0 || index >= NutHttpGetParameterCount(req))
        return NULL;
    return req->req_qptrs[index * 2];
}

/*!
 * \brief Get the value of a request parameter
 *
 * \param req Request object
 * \param index Index to the requested parameter.
 *
 * \return Pointer to the parameter value at the given index,
 *         or NULL if index is out of range.
 */
char *NutHttpGetParameterValue(REQUEST * req, int index)
{
    if (index < 0 || index >= NutHttpGetParameterCount(req))
        return NULL;
    return req->req_qptrs[index * 2 + 1];
}


/*@}*/
