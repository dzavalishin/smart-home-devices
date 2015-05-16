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
 * Thanks to Lars H. Andersson, who submitted the first idea of this simple function
 */


/*!
 * \file pro/sntp.c
 * \brief Simple Network Time Protocol Client.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.11  2009/02/13 14:52:05  haraldkipp
 * Include memdebug.h for heap management debugging support.
 *
 * Revision 1.10  2009/02/06 15:40:29  haraldkipp
 * Using newly available strdup() and calloc().
 * Replaced NutHeap routines by standard malloc/free.
 * Replaced pointer value 0 by NULL.
 *
 * Revision 1.9  2008/08/11 07:00:36  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.8  2005/08/03 11:46:59  olereinhardt
 * Removed unneeded comment line to fix compile bug
 *
 * Revision 1.7  2005/08/02 17:47:04  haraldkipp
 * Major API documentation update.
 *
 * Revision 1.6  2005/04/08 15:20:51  olereinhardt
 * added <sys/types.h> (__APPLE__) and <netinet/in.h> (__linux__)
 * for htons and simmilar.
 *
 * Revision 1.5  2005/04/05 17:44:57  haraldkipp
 * Made stack space configurable.
 *
 * Revision 1.4  2003/11/26 12:55:35  drsung
 * Portability issues ... again
 *
 * Revision 1.3  2003/11/26 11:16:44  haraldkipp
 * Portability issues
 *
 * Revision 1.2  2003/11/24 21:01:42  drsung
 * Now using UDP packet queue.
 *
 * Revision 1.1  2003/11/24 18:13:22  drsung
 * first release
 *
 * \endverbatim
 */

#include <cfg/sntp.h>

#include <pro/sntp.h>
#include <sys/socket.h>
#include <sys/heap.h>
#include <stdlib.h>
#include <string.h>
#include "../crt/ctime.h"
#include <stdio.h>
#include <memdebug.h>
#include <sys/thread.h>
#include <sys/timer.h>
#include <sys/types.h>
#include <netinet/in.h>

/*!
 * \addtogroup xgSntp
 */
/*@{*/

#ifndef NUT_THREAD_SNTPSTACK
#define NUT_THREAD_SNTPSTACK    256
#endif

typedef struct _sntpframe sntpframe;
struct _sntpframe {
    uint8_t mode;
    uint8_t stratum;
    uint8_t poll;
    uint8_t precision;
    uint32_t root_delay;
    uint32_t root_dispersion;
    uint32_t reference_identifier;
    uint32_t reference_ts_sec;
    uint32_t reference_ts_frac;
    uint32_t originate_ts_sec;
    uint32_t originate_ts_frac;
    uint32_t receive_ts_sec;
    uint32_t receive_ts_frac;
    uint32_t transmit_ts_sec;
    uint32_t transmit_ts_frac;
};


#define NTP_PORT	123
#define SNTP_PORT NTP_PORT

struct SNTP_resync_args {
    uint32_t server_addr;
    uint32_t interval;
};

THREAD(SNTP_resync, arg)
{
    uint32_t server_addr = ((struct SNTP_resync_args *) arg)->server_addr;
    uint32_t interval = ((struct SNTP_resync_args *) arg)->interval;
    uint32_t cur_server_addr = server_addr;
    int retry = 0;
    time_t t;

    free(arg);

    NutThreadSetPriority(63);
    for (;;) {
        if (NutSNTPGetTime(&cur_server_addr, &t)) {     /* if any error retry */
            if (cur_server_addr != server_addr && server_addr == 0xFFFFFFFF) {
                cur_server_addr = server_addr;
                continue;
            }

            if (retry++ >= 3) { /* if numer of retries >= 3 wait 30 secs before next retry sequence ... */
                retry = 0;
                NutSleep(30000);
            } else              /* ... else wait 5 secs for next retry */
                NutSleep(5000);
        } else {                /* no error */
            stime(&t);          /* so set the time */
            retry = 0;
            NutSleep(interval); /* and wait the interval time */
        }
    }
}

int NutSNTPGetTime(uint32_t * server_adr, time_t * t)
{
    /*first check the pointers */
    uint32_t rec_addr;
    UDPSOCKET *sock = NULL;     /* the udp socket */
    sntpframe *data;            /* we're using the heap to save stack space */
    uint16_t port;               /* source port from incoming packet */
    int len;
    int result = -1;
    /* Set UDP input buffer to 256 bytes */
    uint16_t bufsize = 256;


    if (t == NULL)
        return -1;
    if (server_adr == NULL)
        return -1;

    if ((data = calloc(1, sizeof(*data))) == NULL)
        goto error;

    sock = NutUdpCreateSocket(0);       /* allocate new udp socket */
    if (sock == NULL)
        goto error;

    NutUdpSetSockOpt(sock, SO_RCVBUF, &bufsize, sizeof(bufsize));

    data->mode = 0x1B;          /* LI, VN and Mode bit fields (all in u_char mode); */
    if (NutUdpSendTo(sock, *server_adr, SNTP_PORT, data, sizeof(*data)))        /* Send packet to server */
        goto error;             /* on error return -1 */
  retry:
    rec_addr = 0;
    len = NutUdpReceiveFrom(sock, &rec_addr, &port, data, sizeof(*data), 5000); /* Receive packet with timeout of 5s */
    if (len <= 0) {
        goto error;             /* error or timeout occured */
    } 

    if (port != SNTP_PORT || (data->mode & 0xc0) == 0xc0)       /* if source port is not SNTP_PORT or server is not in sync return */
    {
        if (*server_adr == 0xFFFFFFFF)
            goto retry;         /*  unusable packets will be just ignored. */
        else
            goto error;
    }

    *t = ntohl(data->transmit_ts_sec) - (70 * 365 + _LEAP_YEAR_ADJUST) * _DAY_SEC;
    *server_adr = rec_addr;
    result = 0;
  error:
    if (sock)
        NutUdpDestroySocket(sock);
    if (data)
        free(data);
    return result;
}

int NutSNTPStartThread(uint32_t server_addr, uint32_t interval)
{
    struct SNTP_resync_args *arg = malloc(sizeof(struct SNTP_resync_args));
    if (!arg)
        return -1;
    arg->server_addr = server_addr;
    arg->interval = interval;
    if (NutThreadCreate("sntpc", SNTP_resync, arg, 
        (NUT_THREAD_SNTPSTACK * NUT_THREAD_STACK_MULT) + NUT_THREAD_STACK_ADD))
        return 0;
    else {
        free(arg);
        return -1;
    }
}

/*@}*/
