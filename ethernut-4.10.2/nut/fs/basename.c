/*
 * Copyright (c) 1997 Todd C. Miller <Todd.Miller@courtesan.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
 * THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*!
 * \file fs/basename.c
 * \brief Returns last component of a pathname.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.2  2009/02/13 14:52:05  haraldkipp
 * Include memdebug.h for heap management debugging support.
 *
 * Revision 1.1  2006/08/01 07:42:56  haraldkipp
 * New functions extract last component and parent directory from pathnames.
 *
 *
 * \endverbatim
 */

#include <errno.h>
#include <libgen.h>
#include <stdlib.h>
#include <string.h>
#include <memdebug.h>

#if !defined(MAXPATHLEN)
#define MAXPATHLEN  256
#endif

/*!
 * \brief Return the last component from a given pathname.
 *
 * Trailing slashes are removed. 
 *
 * \note This function returns a pointer to internal static storage space 
 *       that will be overwritten by subsequent calls.
 *
 * \param path Pointer to the pathname.
 *
 * \return Pointer to the last component. If the specified pathname contains 
 *         slashes only, a single slash is returned. If the pathname is empty 
 *         or a NULL pointer, then a single dot is returned. In case of any
 *         error, a NULL pointer is returned and the global variable errno is 
 *         set to indicate the error.
 */
char *basename(CONST char *path)
{
    static char *bname = NULL;
    CONST char *endp, *startp;

    if (bname == NULL) {
        bname = (char *) malloc(MAXPATHLEN);
        if (bname == NULL)
            return (NULL);
    }

    /* Empty or NULL string gets treated as "." */
    if (path == NULL || *path == '\0') {
        strcpy(bname, ".");
        return (bname);
    }

    /* Strip trailing slashes */
    endp = path + strlen(path) - 1;
    while (endp > path && *endp == '/')
        endp--;

    /* All slashes becomes "/" */
    if (endp == path && *endp == '/') {
        strcpy(bname, "/");
        return (bname);
    }

    /* Find the start of the base */
    startp = endp;
    while (startp > path && *(startp - 1) != '/')
        startp--;

    if ((endp - startp) + 2 >= MAXPATHLEN) {
        errno = ENAMETOOLONG;
        return (NULL);
    }
    strcpy(bname, startp);
    bname[(endp - startp) + 1] = 0;

    return (bname);
}
