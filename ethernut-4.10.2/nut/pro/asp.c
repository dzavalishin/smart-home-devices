/****************************************************************************
*  Copyright (c) 2004 by Michael Fischer. All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*  
*  1. Redistributions of source code must retain the above copyright 
*     notice, this list of conditions and the following disclaimer.
*  2. Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in the 
*     documentation and/or other materials provided with the distribution.
*  3. Neither the name of the author nor the names of its contributors may 
*     be used to endorse or promote products derived from this software 
*     without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL 
*  THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS 
*  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
*  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
*  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF 
*  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
*  SUCH DAMAGE.
*
****************************************************************************
*  History:
*
*  31.03.04  mifi   First Version
*  02.10.04  mifi   Add function to register a user callback
*  04.08.05  or     Adopted to new new mime-handler framework
****************************************************************************/

/*
 * $Log$
 * Revision 1.6  2009/02/13 14:52:05  haraldkipp
 * Include memdebug.h for heap management debugging support.
 *
 * Revision 1.5  2009/02/06 15:40:29  haraldkipp
 * Using newly available strdup() and calloc().
 * Replaced NutHeap routines by standard malloc/free.
 * Replaced pointer value 0 by NULL.
 *
 * Revision 1.4  2008/05/16 01:46:43  thiagocorrea
 * Minor spellcheck fix on the documentation.
 *
 * Revision 1.3  2006/03/16 15:25:38  haraldkipp
 * Changed human readable strings from u_char to char to stop GCC 4 from
 * nagging about signedness.
 *
 * Revision 1.2  2005/08/10 09:26:38  olereinhardt
 * Corrected documentation
 *
 * Revision 1.1  2005/08/05 11:21:02  olereinhardt
 * Added Michael Fischers ASP support. Initial checkin
 *
 */  

/*!
 * \addtogroup xgHTTPD
 */
/*@{*/


#include <sys/heap.h>
#include <sys/confnet.h>
#include <sys/version.h>

#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <memdebug.h>

#include <pro/httpd.h>
#include <pro/asp.h>
#include <arpa/inet.h>

extern CONFNET confnet;

/*==========================================================*/
/*  DEFINE: All Structures and Common Constants             */
/*==========================================================*/
/*
 * Use the half size of the normal buffer, because we need
 * 2 buffers. To save memory, we have divide the size too.
 */
#define MAX_BUFFER_SIZE     256

/*
 * Do NOT use more than 250 for the len.
 * Because we use a unsigned char for the ASPFuncLen
 */
#define MAX_ASP_FUNC_SIZE   64

enum {
    ASP_STATE_IDLE = 0,
    ASP_STATE_START,
    ASP_STATE_COPY_FUNC
};

/*==========================================================*/
/*  DEFINE: Definition of all local Data                    */
/*==========================================================*/
static int (*asp_callback)(char *, FILE *) = NULL;

/*==========================================================*/
/*  DEFINE: Definition of all local Procedures              */
/*==========================================================*/
/************************************************************/
/*  ProcessASPFunction                                      */
/************************************************************/
static void ProcessAspFunction(char *pASPFunction, FILE * stream)
{
    if (strstr(pASPFunction, "nut_version") != NULL) {
        fprintf(stream, "%s", NutVersionString());
        return;
    }

    if (strstr(pASPFunction, "nut_mac_addr") != NULL) {
        fprintf(stream, "%02X:%02X:%02X:%02X:%02X:%02X",
                confnet.cdn_mac[0], confnet.cdn_mac[1], confnet.cdn_mac[2],
                confnet.cdn_mac[3], confnet.cdn_mac[4], confnet.cdn_mac[5]);
        return;
    }

    if (strstr(pASPFunction, "nut_ip_addr") != NULL) {
        fputs(inet_ntoa(confnet.cdn_ip_addr), stream);
        return;
    }

    if (strstr(pASPFunction, "nut_ip_mask") != NULL) {
        fputs(inet_ntoa(confnet.cdn_ip_mask), stream);
        return;
    }

    if (strstr(pASPFunction, "nut_gateway") != NULL) {
        fputs(inet_ntoa(confnet.cdn_gateway), stream);
        return;
    }

    /*
     * Check if the user has registered an ASPCallback
     */
    if (asp_callback != NULL) {
        asp_callback(pASPFunction, stream);
    }
}

/*==========================================================*/
/*  DEFINE: All code exported                               */
/*==========================================================*/
/************************************************************/
/*  NutHttpCheckAsp                                         */
/************************************************************/
void NutHttpProcessAsp(FILE * stream, int fd, int file_len, char* http_root, REQUEST *req)
{
    int n;
    char *pReadBuffer = NULL;
    char *pWriteBuffer = NULL;
    char *pASPFunction = NULL;
    char Data;
    int Size;
    long lFileLen;
    unsigned char bASPState = ASP_STATE_IDLE;
    int ReadCount;
    int WriteCount;
    unsigned char bASPFuncLen;

    lFileLen = _filelength(fd);

    Size = MAX_BUFFER_SIZE;
    WriteCount = 0;
    bASPFuncLen = 0;
    pASPFunction = malloc(MAX_ASP_FUNC_SIZE);
    pReadBuffer = malloc(Size);
    /*
     * For our VERY SPECIAL case, the size of the WriteBuffer must have one char more
     * as the ReadBuffer. Because we must be able to save one more char from the round before.
     */
    pWriteBuffer = malloc(Size + 1);

    if ((pReadBuffer != NULL) && (pWriteBuffer != NULL) && (pASPFunction != NULL)) {

        while (lFileLen) {
            if (lFileLen < MAX_BUFFER_SIZE) {
                Size = (int) lFileLen;
            }

            n = _read(fd, pReadBuffer, Size);

            for (ReadCount = 0; ReadCount < n; ReadCount++) {

                Data = pReadBuffer[ReadCount];

                switch (bASPState) {
                
                case ASP_STATE_IDLE:
                    if (Data == '<') {
                        bASPState = ASP_STATE_START;
                    }
                    pWriteBuffer[WriteCount] = Data;
                    WriteCount++;
                    break;
                    /* ASP_STATE_IDLE */

                case ASP_STATE_START:
                    if (Data == '%') {
                        bASPState = ASP_STATE_COPY_FUNC;
                        if (WriteCount) {
                            WriteCount--;
                        }

                        /*
                         * Write the data up to the ASPFunction
                         */
                        if (WriteCount) {
                            fwrite(pWriteBuffer, 1, WriteCount, stream);
                            WriteCount = 0;
                        }
                    } else {
                        bASPState = ASP_STATE_IDLE;
                        pWriteBuffer[WriteCount] = Data;
                        WriteCount++;
                    }
                    break;
                    /* ASP_STATE_START_1 */

                case ASP_STATE_COPY_FUNC:
                    if (Data == '>') {
                        bASPState = ASP_STATE_IDLE;
                        pASPFunction[bASPFuncLen] = 0;

                        ProcessAspFunction(pASPFunction, stream);
                        bASPFuncLen = 0;
                    } else {
                        /*
                         * Skip over the END of an ASPFunction
                         */
                        if (Data == '%') {
                            Data = 0;
                        }

                        pASPFunction[bASPFuncLen] = Data;
                        bASPFuncLen++;
                        if (bASPFuncLen >= MAX_ASP_FUNC_SIZE) {
                            /*
                             * This make no sense, but protect our stack
                             */
                            bASPFuncLen = 0;
                        }
                    }
                    break;
                    /* ASP_STATE_COPY_FUNC */
                } /* end switch (bASPState) */
            } /* end for */

            /*
             * If data are available in the WriteBuffer, 
             * send it out...
             */
            if (WriteCount) {
                /*
                 * Now we must test a VERY SPECIAL case !
                 * It could be possible that the last char in the buffer is a '<'.
                 * Now the State is ASP_STATE_START. If the next chunk starts with a
                 * '%' it is to late, because we have send out the '<'.
                 * Therefore we must test if the last char is a '<', in this case send all the
                 * rest and save the '<' for the next round.
                 */

                if (pWriteBuffer[WriteCount - 1] == '<') {
                    WriteCount--;
                    fwrite(pWriteBuffer, 1, WriteCount, stream);

                    /*
                     * Now put the '<' in the buffer for the next round
                     */
                    pWriteBuffer[0] = '<';
                    WriteCount = 1;
                } else {
                    fwrite(pWriteBuffer, 1, WriteCount, stream);
                    WriteCount = 0;
                }
            }

            lFileLen -= (long) n;
        }
    }

    if (pReadBuffer != NULL) {
        free(pReadBuffer);
    }
    if (pWriteBuffer != NULL) {
        free(pWriteBuffer);
    }
    if (pASPFunction != NULL) {
        free(pASPFunction);
    }
}

/************************************************************/
/*  NutRegisterAspCallback                                  */
/*                                                          */
/*  return 0 on success, -1 otherwise.                      */
/************************************************************/
int NutRegisterAspCallback(int (*func) (char *, FILE *))
{
    register int result = 0;

    if (asp_callback == NULL) {
        asp_callback = func;
    } else {
        result = -1;
    }

    return (result);
}

/*!
 * \brief Register ASP handler for asp files.
 *
 * asp files may use the following syntax:
 *
 * <%my_function%>
 */

void NutRegisterAsp(void)
{
    NutSetMimeHandler(".asp", NutHttpProcessAsp);
}

/*@}*/

