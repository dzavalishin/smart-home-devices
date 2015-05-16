/*
 * Copyright (C) 2001-2004 by Ole Reinhardt <ole.reinhardt@embedded-it.de>. 
 * All rights reserved.
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
 * Revision 1.14  2009/02/13 14:43:11  haraldkipp
 * Memory overflow bug fixed.
 *
 * Revision 1.13  2009/02/06 15:40:29  haraldkipp
 * Using newly available strdup() and calloc().
 * Replaced NutHeap routines by standard malloc/free.
 * Replaced pointer value 0 by NULL.
 *
 * Revision 1.12  2008/08/22 09:23:57  haraldkipp
 * GCC specific implementation removed.
 *
 * Revision 1.11  2008/08/18 11:00:39  olereinhardt
 * Fixed a memory leak. A filename was never freed again
 *
 * Revision 1.10  2008/08/11 07:00:36  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.9  2008/07/26 14:09:29  haraldkipp
 * Fixed another problem with ICCAVR.
 *
 * Revision 1.8  2008/07/25 12:17:26  olereinhardt
 * Imagecraft compilers does not support alloca (to allocate memory from the
 * stack). So we use NutHeapAlloc / NutHeapClear instead for Imagecraft.
 *
 * Revision 1.7  2008/07/17 11:36:34  olereinhardt
 * - Moved some functions used in httpd.c as well as in ssi.c into httpd_p.c
 * - Implemeted $QUERY_STRING parameter in for CGIs included by a ssi file
 *
 * Revision 1.6  2008/05/13 21:24:55  thiagocorrea
 * Fix a few documentation typos.
 *
 * Revision 1.5  2008/04/21 22:16:25  olereinhardt
 * 2008-04-21  Ole Reinhardt <ole.reinhardt@embedded-it.de>
 *         * pro/ssi.c: Nicer Implement query string feature and save some
 *           memory
 *
 * Revision 1.4  2008/04/18 13:22:57  haraldkipp
 * Fixed ICCAVR compile errors. No chance to implement GCC's PSTR().
 *
 * Revision 1.3  2008/02/15 17:09:03  haraldkipp
 * Quick hack provided by Niels. No idea what this is for, but
 * according to the author it is a dirty solution. We urgently
 * need it to get the Elektor Radio working. Sorry in advance
 * for any trouble this change may cause.
 *
 * Revision 1.2  2006/03/16 15:25:39  haraldkipp
 * Changed human readable strings from u_char to char to stop GCC 4 from
 * nagging about signedness.
 *
 * Revision 1.1  2005/08/05 11:22:14  olereinhardt
 * Added Server side include support. Initial checkin
 *
 */  

/*!
 * \addtogroup xgHTTPD
 */
/*@{*/

#include <cfg/arch.h>
#include <sys/types.h>

#include <memdebug.h>
#include <string.h>
#include <stdlib.h>

#include <io.h>
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/heap.h>
#include <sys/version.h>

#include <pro/httpd.h>
#include <pro/ssi.h>
#include "httpd_p.h"
#include "dencode.h"

#define BUFSIZE 512

#define MIN(a,b) (a<b?a:b)

#define SSI_TYPE_FILE    0x01
#define SSI_TYPE_VIRTUAL 0x02
#define SSI_TYPE_EXEC    0x03
#define SSI_TYPE_ECHO    0x04

static char * (*ssivar_handler)(char *, REQUEST *);

static prog_char rsp_not_found_P[] = "404 Not found: %s\r\n";
static prog_char rsp_intern_err_P[] = "500 Internal error\r\n";
static prog_char rsp_bad_req_P[] = "400 Bad request\r\n";

extern char *cgiBinPath;

/*!
 * \brief Send included file to the stream
 *
 * Load a file from filesystem and send ("include") it to the http-stream
 *
 * \param stream Stream of the socket connection, previously opened for 
 *               binary read and write.
 * \param filename Name of the included file. e.g."UROM:test.txt"
 */

static void NutSsiProcessFile(FILE * stream, char *filename)
{
    int fd;
    int n;
    char *data;
    int size;
    long file_len;

    fd = _open(filename, _O_BINARY | _O_RDONLY);

    if (fd == -1) {                     // No such file found... send a 404 string.
        fprintf_P(stream, rsp_not_found_P, filename);
        return;
    }
    
    file_len = _filelength(fd);
    
    size = 512;
    if ((data = malloc(size)) != NULL) {
        while (file_len) {
            if (file_len < 512L)
                size = (int) file_len;

            n = _read(fd, data, size);
            if (fwrite(data, 1, n, stream) == 0)
                break;
            file_len -= (long) n;
        }
        free(data);
    }

    _close(fd);
}


/*!
 * \brief Send a file or cgi with a path relativ to http-root
 *
 * Processes an included local url with a path relativ to http-root. This could also be a 
 * cgi script. Nearly the same as NutHttpProcessFileRequest
 *
 * \param stream Stream of the socket connection, previously opened for 
 *               binary read and write.
 * \param url    URL of the file to be included e.g. "/include/test.js"
 * \param http_root The root path of the http-deamon
 * \param orig_req The http request struct of the top most http_request
 */

static void NutSsiProcessVirtual(FILE * stream, char *url, char* http_root, REQUEST *orig_req)
{
    int fd;
    int i;
    int n;
    char *data;
    int size;
    long file_len;
    char *filename = NULL;
    void (*handler)(FILE *stream, int fd, int file_len, char *http_root, REQUEST *req);
    char *cp;
    REQUEST * req;
    CONST char *cgi_bin = cgiBinPath ? cgiBinPath : "cgi-bin/";
    CONST char * tmp;
    size_t len;

    tmp = cgi_bin;
    if (NutDecodePath(url) == 0) {
        fprintf_P(stream, rsp_bad_req_P);
        return;
    }
  
    /*
     * Process CGI.
     */
    while (tmp) {
        /* Skip leading path separators. */
        while (*cgi_bin == ';')
            cgi_bin++;
        /* Determine the length of the next path component. */
        for (len = 0, cp = (char *)cgi_bin; *cp && *cp != ';'; len++, cp++);
        tmp = cgi_bin;
        if (len && strncasecmp(url, tmp, len) == 0) {
            if ((req = calloc(1, sizeof(REQUEST))) == 0) {
                fprintf_P(stream, rsp_intern_err_P);
                return;
            }
            req->req_method = METHOD_GET;
            req->req_version = orig_req->req_version;
            req->req_length = 0;
            
            if (orig_req->req_agent != NULL) {
                if ((req->req_agent = strdup(orig_req->req_agent)) == NULL) {
                    fprintf_P(stream, rsp_intern_err_P);
                    DestroyRequestInfo(req);
                    return;
                }
            }
            if (orig_req->req_cookie!= NULL) {
                if ((req->req_cookie = strdup(orig_req->req_cookie)) == NULL) {
                    fprintf_P(stream, rsp_intern_err_P);
                    DestroyRequestInfo(req);
                    return;
                }
            }
            if ((cp = strchr(url, '?')) != 0) {
                *cp++ = 0;
                if (strcmp(cp, "$QUERY_STRING") == 0) {
                    uint16_t size;
                    size = 1; /* At least 1 for empty requests. */
                    for (i = 0; i < orig_req->req_numqptrs*2; i ++) {
                        size += strlen(orig_req->req_qptrs[i]) + 1;
                    }
                    if ((req->req_query = malloc(size)) == NULL) {
                        fprintf_P(stream, rsp_intern_err_P);
                        DestroyRequestInfo(req);
                        return;
                    }
                    req->req_query[0] = 0;
                    for (i = 0; i < (orig_req->req_numqptrs * 2); i++) {
                        if(i) {
                            strcat (req->req_query, "&");
                        }
                        strcat (req->req_query, orig_req->req_qptrs[i]);
                        strcat (req->req_query, "=");
                        i++;
                        strcat (req->req_query, orig_req->req_qptrs[i]);
                    }

                } else {
                    if ((req->req_query = strdup(cp)) == NULL) {
                        fprintf_P(stream, rsp_intern_err_P);
                        DestroyRequestInfo(req);
                        return;
                    }
                }
                NutHttpProcessQueryString(req);
            }
            if ((req->req_url = strdup(url)) == NULL) {
                fprintf_P(stream, rsp_intern_err_P);
                DestroyRequestInfo(req);
                return;
            }
            NutCgiProcessRequest(stream, req, len);
            DestroyRequestInfo(req);
            return;
        }
        cgi_bin += len;
        if (*cgi_bin == '\0') {
            break;
        }
    }
    
    /*
     * Process file.
     */
    
    for (n = 0, fd = -1; default_files[n]; n++) {
        filename = CreateFilePath(url, default_files[n]);
        if (filename == NULL) {
            fprintf_P(stream, rsp_intern_err_P);
            return;
        }
        /*
         * Note, that simple file systems may not provide stat() or access(),
         * thus trying to open the file is the only way to check for existence.
         * Another problem is, that PHAT allows to open directories. We use
         * the file length to ensure, that we got a normal file.
         */
        if ((fd = _open(filename, _O_BINARY | _O_RDONLY)) != -1) {
            if (_filelength(fd)) {
                break;
            }
            _close(fd);
        }
        free(filename);
    }
    if (fd == -1) {
        fprintf_P(stream, rsp_not_found_P, url);
        return;
    }
    
    file_len = _filelength(fd);
    handler = NutGetMimeHandler(filename);
    free(filename);
    
    if (handler == NULL) {
        size = 512;                 // If we have not registered a mime handler handle default.
        if ((data = malloc(size)) != NULL) {
            while (file_len) {
                if (file_len < 512L) {
                    size = (int) file_len;
                }

                n = _read(fd, data, size);
                if (fwrite(data, 1, n, stream) == 0) {
                    break;
                }
                file_len -= (long) n;
            }
            free(data);
        }
    } else handler(stream, fd, file_len, http_root, orig_req);
    _close(fd);
    return;
}

/*!
 * \brief Send variable content to the stream.
 *
 * \param stream Stream of the socket connection, previously opened for 
 *               binary read and write.
 * \param varname Name of the variable.
 */
static void NutSsiProcessEcho(FILE * stream, char *name, REQUEST *req)
{
    if (ssivar_handler) {
        char *value = (*ssivar_handler)(name, req);

        if (value) {
            fputs(value, stream);
        }
    }
}

/*!
 * \brief Skip whitespaces in a string from the given position on
 *
 * Whitespaces are defined as \n \r \t and space. Pos will be set to the first character which is not a whitespace
 *
 * \param buffer String to work on
 * \param pos    Start position of the search
 * \param end    last position we should check for
 * \return       Nothing. pos is set to the first character which is not a white space
 */


static void NutSsiSkipWhitespace(char *buffer, uint16_t *pos, uint16_t end)
{
    while ((*pos < end) && (
           (buffer[*pos] == '\n') || (buffer[*pos] == '\r') ||
           (buffer[*pos] == '\t') || (buffer[*pos] == ' ')))
        (*pos) ++;
}

/*!
 * \brief Check if a comment is a ssi directive 
 *
 * Check if a comment is a ssi directive and replace the directive by the included data.
 * Allowed directives are:
 * 
 * <!--#include virtual="/news/news.htm" -->
 * <!--#include file="UROM:/news/news.htm" -->
 * <!--#exec cgi="/cgi-bin/counter.cgi" --> 
 * 
 * \param stream Stream of the socket connection, previously opened for 
 *               binary read and write.
 * \param buffer Current file buffer so search in. The buffer is set to the start of a html comment
 * \param end    End position of the comment.
 * \param http_root The root path of the http-deamon
 * \param req    The http request struct of the top most http_request
 */

static uint8_t NutSsiCheckForSsi(FILE *stream, char *buffer, uint16_t end, char* http_root, REQUEST *req)
{
    uint16_t pos = 4; // First character after comment start
    char * filename;
    uint8_t type;

    pos = 4;
    NutSsiSkipWhitespace(buffer, &pos, end);        // Skip whitespaces after comment start
    if (pos == end) return 0;
    
    if (strncasecmp(&buffer[pos], "#include", 8) == 0) { // Search include directive
        pos += 8;
        type = SSI_TYPE_VIRTUAL;
    } else 
    if (strncasecmp(&buffer[pos], "#exec", 5) == 0) { // Search include or exec directive
        pos += 5;
        type = SSI_TYPE_EXEC;
    } else 
    if (strncasecmp(&buffer[pos], "#echo", 5) == 0) { // Search echo directive
        pos += 5;
        type = SSI_TYPE_ECHO;
    } else return 0;                                // No include or exec found. Skip the rest of this comment...
    if (pos >= end) return 0;

    NutSsiSkipWhitespace(buffer, &pos, end);        // Skip whitespaces after #include directive
    if (pos == end) return 0;
    
    if (type == SSI_TYPE_VIRTUAL) {
        if (strncasecmp(&buffer[pos], "virtual", 7) == 0) {  // Search virtual directive
            pos += 7;
        } else                                      // No virtual found. Test for file...
        if (strncasecmp(&buffer[pos], "file", 4) == 0) {  // Search file directive
            pos += 4;
            type = SSI_TYPE_FILE;
        } else return 0;                            // No file found. Test for file...
    }
    else if (type == SSI_TYPE_ECHO) {
        if (strncasecmp(&buffer[pos], "var", 3) == 0) {  // Search var directive
            pos += 3;
        } else return 0;                            // No file found. Test for file...
    } else {
        if (strncasecmp(&buffer[pos], "cgi", 3) == 0) {  // Search cgi directive
            pos += 3;
        } else return 0;                            // No cgi found. return...
    }
    if (pos >= end) return 0;
    
    NutSsiSkipWhitespace(buffer, &pos, end);        // Skip whitespaces after virtual, file or cgi directive
    if (pos == end) return 0;

    if (buffer[pos] != '=') return 0;               // check for assertion
    pos ++; 

    NutSsiSkipWhitespace(buffer, &pos, end);        // Skip whitespaces after assertion
    if (pos == end) return 0;    
    
    if (buffer[pos] == '"') {                       // Search for filename and pass to output function
        pos ++;
        if (pos == end) return 0;
        filename = &buffer[pos];
        while (buffer[pos] != '"') {
            pos ++;
            if (pos == end) return 0;
        }
        buffer[pos] = '\0';
        switch (type) {
            case SSI_TYPE_FILE:
                NutSsiProcessFile(stream, filename);
                break;
            case SSI_TYPE_VIRTUAL:
                NutSsiProcessVirtual(stream, filename, http_root, req);
                break;
            case SSI_TYPE_EXEC:
                NutSsiProcessVirtual(stream, filename, http_root, req);
                break;
            case SSI_TYPE_ECHO:
                NutSsiProcessEcho(stream, filename, req);
                break;
        }
    }
    return 1;
}

/*!
 * \brief Check a file for html comments 
 *
 * Check a file for html comments and then call NutSsiCheckForSsi to seach a ssi directive
 * Allowed diretives are:
 * 
 * <!--#include virtual="/news/news.htm" -->
 * <!--#include file="UROM:/news/news.htm" -->
 * <!--#exec cgi="/cgi-bin/counter.cgi" --> 
 * 
 * \param stream Stream of the socket connection, previously opened for 
 *               binary read and write.
 * \param fd     Filedescriptor pointing to a just opened file.
 * \param file_len length of this file
 * \param http_root The root path of the http-deamon
 * \param req    The http request struct of the top most http_request
 */


static void NutHttpProcessSHTML(FILE * stream, int fd, int file_len, char* http_root, REQUEST *req)
{
    char * buffer;
    uint8_t in_comment;
    int buffsize;
    int fpos;
    int n;
    char *index;
    uint8_t found;
    buffsize = MIN(BUFSIZE, file_len);
    buffer = malloc(buffsize+1);
    in_comment = 0;
    fpos = 0;
    while (file_len != fpos) {
        memset(buffer, 0, buffsize+1);
        n = _read(fd, buffer, MIN(buffsize, file_len-fpos));
        
        if (!in_comment) {        
            
            index = strstr(buffer, "<!--");
            if (index == NULL) {                    // Nothing found. print next 412 characters, seek to next startpoint.
                if (file_len > buffsize) {
                    fwrite(buffer, 1, MIN(buffsize-100, n), stream);
                    fpos += MIN(buffsize-100, n);
                    _seek(fd, fpos, SEEK_SET);
                } else {
                    fwrite(buffer, 1, n, stream);
                    fpos += n;
                }
                
            } else {
                found = (int)index - (int)buffer;   // We have found a comment initializer. Seek to the startpoint and print the beginning of the buffer.
                fwrite (buffer, 1, found, stream);
                fpos += found;
                _seek(fd, fpos, SEEK_SET);
                in_comment = 1;
            }
        } else {                                    // Ok, we assume we are "into" a comment.    
            index = strstr(buffer, "-->");
            if (index == NULL) {                    // We have not found the end of the comment in the next 512 characters. Byepass this comment.
                fwrite(buffer, 1, MIN(buffsize, n), stream);
                fpos += MIN(buffsize, n);
                in_comment = 0;
            } else {                                // Ok. This seems to be a comment with maximum length of 512 bytes. We now search for ssi code.    
                found = (int)index - (int)buffer;
                if (!NutSsiCheckForSsi(stream, buffer, found, http_root, req)) {
                    fwrite(buffer, 1, found+3, stream);
                }
                fpos += found+3;
                _seek(fd, fpos, SEEK_SET);
                in_comment = 0;
            }
        }
    }
    
    free(buffer);
}

/*!
 * \brief Register SSI handler for shtml files.
 * 
 * shtml files may use the following ssi commands:
 *
 * <!--#include virtual="/news/news.htm" -->
 * <!--#include file="UROM:/news/news.htm" -->
 * <!--#exec cgi="/cgi-bin/counter.cgi" -->
 * <!--#echo var="counter" -->
 */

void NutRegisterSsi(void)
{
    NutSetMimeHandler(".shtml", NutHttpProcessSHTML);
}

/*!
 * \brief Register SSI handler for variables.
 */
int NutRegisterSsiVarHandler(char * (*handler)(char *name, REQUEST *req))
{
    ssivar_handler = handler;

    return 0;
}

/*@}*/
