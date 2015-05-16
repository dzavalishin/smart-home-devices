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
 */

/*
 * $Log$
 * Revision 1.7  2009/03/07 00:18:13  olereinhardt
 * Use base64 decoder from gorp/base64/base64_decode.c instead from dencode.c
 *
 * Revision 1.6  2009/02/13 14:52:05  haraldkipp
 * Include memdebug.h for heap management debugging support.
 *
 * Revision 1.5  2009/02/06 15:40:29  haraldkipp
 * Using newly available strdup() and calloc().
 * Replaced NutHeap routines by standard malloc/free.
 * Replaced pointer value 0 by NULL.
 *
 * Revision 1.4  2008/07/17 11:29:15  olereinhardt
 * Allow authentication for subdirectories
 *
 * Revision 1.3  2006/10/08 16:43:53  haraldkipp
 * Authentication info depended on static memory kept by the caller. Now a
 * local copy is held and NutClearAuth (which should have been named
 * NutHttpAuthClear, btw.) works correctly.
 *
 * Revision 1.2  2006/08/25 13:42:16  olereinhardt
 * added NutClearAuth(void); Thanks to Peter Sondermanns
 *
 * Revision 1.1.1.1  2003/05/09 14:41:56  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.7  2003/02/04 18:17:31  harald
 * Version 3 released
 *
 * Revision 1.6  2002/06/26 17:29:49  harald
 * First pre-release with 2.4 stack
 *
 */

#include <sys/heap.h>

#include <stdlib.h>
#include <string.h>
#include <memdebug.h>

#include <pro/httpd.h>
#include <gorp/base64.h>

/*!
 * \addtogroup xgHTTPD
 */
/*@{*/

AUTHINFO *authList = 0;

/*!
 * \brief Look up an authorization entry.
 */
static AUTHINFO *NutHttpAuthLookup(CONST char *dirname, CONST char *login)
{
    AUTHINFO *auth;

    for (auth = authList; auth; auth = auth->auth_next) {
        if (dirname && (strstr(dirname, auth->auth_dirname) != dirname))
            continue;
        if (login && strcmp(login, auth->auth_login))
            continue;
        break;
    }
    return auth;
}

/*!
 * \brief Register an authorization entry.
 *
 * Protect a specified directory from unauthorized access.
 *
 * \warning Directories not registered by this function are
 *          accessible by anyone.
 *
 * \param dirname Name of the directory to protect.
 * \param login   Required login to access this directory. This
 *                string must contain a user name, followed by
 *                a colon followed by an uncrypted password.
 *
 * \return 0 on success, -1 otherwise.
 */
int NutRegisterAuth(CONST char *dirname, CONST char *login)
{
    AUTHINFO *auth;

    /* Allocate a new list element. */
    if ((auth = malloc(sizeof(AUTHINFO))) != NULL) {
        auth->auth_next = authList;
        /* Allocate the path component. */
        if ((auth->auth_dirname = strdup(dirname)) != NULL) {
            /* Allocate the login component. */
            if ((auth->auth_login = strdup(login)) != NULL) {
                /* Success. Add element to the list and return. */
                authList = auth;
                return 0;
            }
            /* Allocation failed. */
            free(auth->auth_dirname);
        }
        free(auth);
    }
    return -1;
}


/*!
 * \brief Clear all authorization entries.
 *
 * Clears all authorization entries and frees the used ressouces.
 *
 */
void NutClearAuth(void)
{
    AUTHINFO *auth;

    while (authList) {
        auth = authList;
        authList = auth->auth_next;
        free(auth->auth_dirname);
        free(auth->auth_login);
        free(auth);
    }
}

/*!
 * \brief Validate an authorization request.
 *
 * \note This function is automatically called by the HTTP
 *       library on incoming requests. Applications do not
 *       need to call this function.
 *
 * \param req Request to be checked.
 *
 * \return 0, if access granted, -1 otherwise.
 */
int NutHttpAuthValidate(REQUEST * req)
{
    char *realm;
    char *cp = 0;
    int rc = -1;

    /*
     * Get directory by chopping off filename.
     */
    realm = req->req_url;
    if ((cp = strrchr(realm, '/')) != 0)
        *cp = 0;
    else
        realm = ".";

    /*
     * Check if authorization required.
     */
    if (NutHttpAuthLookup(realm, 0)) {
        /*
         * Check authorization.
         */
        if (req->req_auth) {
            /*
             * Acceptint basic authorization only.
             */
            if (strncmp(req->req_auth, "Basic ", 6) == 0) {
                NutDecodeBase64(req->req_auth + 6);
                if (NutHttpAuthLookup(realm, req->req_auth + 6))
                    rc = 0;
            }
        }
    } else
        rc = 0;

    if (cp)
        *cp = '/';

    return rc;
}

/*@}*/
