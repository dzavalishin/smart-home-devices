#include <cfg/http.h>
#include <sys/heap.h>

#include <stdlib.h>
#include <string.h>
#include <memdebug.h>

#include "httpd_p.h"

/*
 *  W A R N I N G
 *  -------------
 *
 * This file is not part of the Ethernut API.  It exists purely as an
 * implementation detail.  This header file may change from version to
 * version without notice, or even be removed.
 *
 * We mean it.
*/


/*!
 * \brief Default index files.
 *
 * The first entry must contain an empty string.
 */

/*! \brief Default file system. */
#ifndef HTTP_DEFAULT_ROOT
#define HTTP_DEFAULT_ROOT   "UROM:"
#endif

char *http_root;

char *default_files[] = {
    "",
    "/index.html",
    "/index.htm",
    "/default.html",
    "/default.htm",
    "/index.shtml",
    "/index.xhtml",
    "/index.asp",
    "/default.asp",
    NULL
};

/*!
* \brief Create a file path from an URL.
* \internal
* \param url Pointer to the URL string
* \param addon Filename to be appended to the path
*/

char *CreateFilePath(CONST char *url, CONST char *addon)
{
    char *root = http_root ? http_root : HTTP_DEFAULT_ROOT;
    size_t urll = strlen(url);
    char *path = malloc(strlen(root) + urll + strlen(addon) + 1);

    if (path) {
        strcpy(path, root);
        strcat(path, url);
        if (*addon) {
            strcat(path, addon + (urll == 0 || url[urll - 1] == '/'));
        }
    }
    return path;
}

/*!
* \brief Release request info structure.
* \internal
* \param req Pointer to the info structure. If NULL, nothing
*            is released.
*/
void DestroyRequestInfo(REQUEST * req)
{
    if (req) {
        if (req->req_url)
            free(req->req_url);
        if (req->req_query)
            free(req->req_query);
        if (req->req_type)
            free(req->req_type);
        if (req->req_cookie)
            free(req->req_cookie);
        if (req->req_auth)
            free(req->req_auth);
        if (req->req_agent)
            free(req->req_agent);
        if (req->req_qptrs)
            free(req->req_qptrs);
        if (req->req_referer)
            free(req->req_referer);
        if (req->req_host)
            free(req->req_host);
        if (req->req_encoding)
            free(req->req_encoding);
        free(req);
    }
}
