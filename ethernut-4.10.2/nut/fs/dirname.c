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

#include <errno.h>
#include <libgen.h>
#include <stdlib.h>
#include <string.h>
#include <memdebug.h>

#if !defined(MAXPATHLEN)
#define MAXPATHLEN  256
#endif

/*!
 * \file fs/dirname.c
 * \brief Extracts the directory part of a pathname.
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

/*!
 * \brief Extract the directory part of a given pathname.
 *
 * This is the converse of basename().
 *
 * \param path Pointer to the pathname.
 *
 * \return A pointer to the parent directory of the pathname. Any 
 *         trailing slash is not counted as part of the directory
 *         name. If the specified pathname is a null pointer, an 
 *         empty string, or contains no slash character, then a
 *         single dot is returned. On failures, a NULL pointer
 *         is returned and the global variable errno is set to 
 *         indicate the error.
 */
char *dirname(CONST char *path)
{
    static char *bname;
    CONST char *endp;

    if (bname == NULL) {
        bname = (char *) malloc(MAXPATHLEN);
        if (bname == NULL)
            return (NULL);
    }

    /* Empty or NULL string gets treated as "." */
    if (path == NULL || *path == '\0') {
        strncpy(bname, ".", MAXPATHLEN);
        return (bname);
    }

    /* Strip trailing slashes */
    endp = path + strlen(path) - 1;
    while (endp > path && *endp == '/')
        endp--;

    /* Find the start of the dir */
    while (endp > path && *endp != '/')
        endp--;

    /* Either the dir is "/" or there are no slashes */
    if (endp == path) {
        strncpy(bname, *endp == '/' ? "/" : ".", MAXPATHLEN);
        return (bname);
    }

    do {
        endp--;
    } while (endp > path && *endp == '/');

    if (endp - path + 2 > MAXPATHLEN) {
        errno = ENAMETOOLONG;
        return (NULL);
    }
    strcpy(bname, path);
    bname[(endp - path) + 1] = 0;

    return (bname);
}
