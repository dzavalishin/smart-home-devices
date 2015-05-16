/*
 * Copyright (C) 2003 by egnite Software GmbH
 *
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
 * $Id: scanner.c 3307 2011-02-11 16:12:43Z olereinhardt $
 */

#include <dev/board.h>

#include <sys/heap.h>
#include <sys/thread.h>
#include <sys/timer.h>

#include <netinet/tcp.h>
#include <arpa/inet.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "scanner.h"

#ifdef DEV_ETHER

/*
 * Get line from naked TCP stream.
 */
static int GetLine(TCPSOCKET * sock, char * line, uint16_t size)
{
    int rc = 0;
    uint8_t to_cnt = 0;
    int got;
    char *cp = line;

    if (size > 0) {
        for (;;) {
            if ((got = NutTcpReceive(sock, cp, 1)) <= 0) {
                if (got == 0 && to_cnt++ < 10)
                    continue;
                rc = got;
                printf("[GL-TO]");
                break;
            }
            if (*cp == '\n') {
                *cp = 0;
                break;
            }
            if (*cp != '\r' && rc < (int) size) {
                rc++;
                cp++;
            }
        }
    }
    return rc;
}

static int PutString(TCPSOCKET * sock, char * str)
{
    uint16_t len = strlen(str);
    uint16_t n;
    int c;

    for (n = 0; n < len; n += c)
        if ((c = NutTcpSend(sock, str + n, len - n)) < 0)
            return -1;
    return len;
}


/*!
 * \brief Process header from server.
 */
int ScanStreamHeader(TCPSOCKET * sock, RADIOSTATION * rsp)
{
    int rc = -1;
    char *line = malloc(256);
    char *cp;

    /*
     * Send the HTTP request.
     */
    strcpy(line, "GET /");
    if (rsp->rs_url)
        strcat(line, rsp->rs_url);
    strcat(line, " HTTP/1.0\r\n");
    PutString(sock, line);

    sprintf(line, "Host: %s\r\n", inet_ntoa(rsp->rs_ip));
    PutString(sock, line);

    PutString(sock, "User-Agent: WinampMPEG/2.7\r\n" "Accept: */*\r\n" "Icy-MetaData: 1\r\n" "Connection: close\r\n\r\n");

    if (rsp->rs_name) {
        free(rsp->rs_name);
        rsp->rs_name = 0;
    }
    if (rsp->rs_genre) {
        free(rsp->rs_genre);
        rsp->rs_genre = 0;
    }
    rsp->rs_metaint = 0;

    /*
     * Receive the HTTP header.
     */
    if (GetLine(sock, line, 256) > 5) {
        printf("%s\n", line);
        if (strncmp(line, "ICY", 3) == 0) {
            if (atoi(line + 4) == 200) {
                for (;;) {
                    if ((rc = GetLine(sock, line, 256)) <= 0) {
                        break;
                    }
                    if (strncmp(line, "icy-name:", 9) == 0) {
                        cp = line + 9;
                        while (*cp && *cp == ' ')
                            cp++;
                        if (*cp && rsp->rs_name == 0) {
                            rsp->rs_name = malloc(strlen(cp) + 1);
                            strcpy(rsp->rs_name, cp);
                            printf("%s\n", cp);
                        }
                    } else if (strncmp(line, "icy-genre:", 10) == 0) {
                        cp = line + 10;
                        while (*cp && *cp == ' ')
                            cp++;
                        if (*cp && rsp->rs_genre == 0) {
                            rsp->rs_genre = malloc(strlen(cp) + 1);
                            strcpy(rsp->rs_genre, cp);
                        }
                    } else if (strncmp(line, "icy-metaint:", 12) == 0)
                        rsp->rs_metaint = atol(line + 12);
                    else if (strncmp(line, "icy-br:", 7) == 0)
                        rsp->rs_bitrate = atol(line + 7);
                }
            } else
                puts(line);
        }
    }

    free(line);

    printf("\n%s %ukbps %s ", inet_ntoa(rsp->rs_ip), rsp->rs_bitrate, rsp->rs_name);

    return rc;
}

/*
 * Process embedded meta data.
 */
static char *ReadMetaTitle(TCPSOCKET * sock, uint32_t iv)
{
    uint8_t blks = 0;
    uint16_t cnt;
    int got;
    int rc = 0;
    char *title = 0;
    char *buf;
    char *mn1;
    char *mn2;
    char *md1;
    char *md2;

    /* Allocate temporary buffer. */
    if ((buf = malloc(512 + 1)) == 0) {
        return 0;
    }
    for (cnt = 512; iv; iv -= got) {
        if (iv < 512)
            cnt = iv;
        if ((got = NutTcpReceive(sock, buf, cnt)) <= 0)
            break;
    }

    if (iv == 0) {
        /* Get the number of meta data blocks. */
        if ((got = NutTcpReceive(sock, &blks, 1)) == 1) {
            if (blks && blks <= 32) {

                /* Receive the metadata block. */
                for (cnt = blks * 16; cnt; cnt -= got) {
                    if ((got = NutTcpReceive(sock, buf + rc, cnt)) < 0)
                        break;
                    rc += got;
                    buf[rc] = 0;
                }
                if (cnt == 0) {
                    mn1 = buf;
                    while (mn1) {
                        if ((mn2 = strchr(mn1, ';')) != 0)
                            *mn2++ = 0;
                        if ((md1 = strchr(mn1, '=')) != 0) {
                            *md1++ = 0;
                            while (*md1 == ' ' || *md1 == '\'')
                                md1++;
                            if ((md2 = strrchr(md1, '\'')) != 0)
                                *md2 = 0;
                            if (strcasecmp(mn1, "StreamTitle") == 0) {
                                title = malloc(strlen(md1) + 1);
                                strcpy(title, md1);
                                break;
                            }
                        }
                        mn1 = mn2;
                    }
                }
            } else
                printf("[ML=%u]", blks);
        }
    } else
        puts("[SMFAIL]");
    free(buf);
    return title;
}



/*
 * Background thread for playing stream.
 */
THREAD(Scanner, arg)
{
    TCPSOCKET *sock;
    RADIOSTATION *rsp;
    uint8_t rs;
    uint32_t rx_to = 10000UL;

    NutThreadSetPriority(128);
    NutSleep(10000);
    for (;;) {
        for (rs = 0; rs < MAXNUM_STATIONS; rs++) {
            NutSleep(2000);
            if (rs == radio.rc_rstation || rs == radio.rc_cstation)
                continue;
            rsp = &station[rs];
            if (rsp->rs_ip == 0 || rsp->rs_port == 0 || radio.rc_off) {
                continue;
            }

            /* Delay if this isn't the first connection. */
            if (rsp->rs_name) {
                printf("%d bytes free\n", NutHeapAvailable());
                NutSleep(30000);
            }

            /* Create a socket. */
            if ((sock = NutTcpCreateSocket()) == 0) {
                break;
            }
            NutTcpSetSockOpt(sock, SO_RCVTIMEO, &rx_to, sizeof(rx_to));

            /* Connect the stream server. */
            printf("[Scan %s:%u]\n", inet_ntoa(rsp->rs_ip), rsp->rs_port);
            if (NutTcpConnect(sock, rsp->rs_ip, rsp->rs_port) == 0) {
                /* Process header from server. */
                if (ScanStreamHeader(sock, rsp) == 0) {
                    if (rsp->rs_scantitle) {
                        free(rsp->rs_scantitle);
                        rsp->rs_scantitle = 0;
                    }
                    if (rsp->rs_metaint) {
                        if ((rsp->rs_scantitle = ReadMetaTitle(sock, rsp->rs_metaint)) != 0) {
                            printf("%03u: %s\n", rs, rsp->rs_scantitle);
                            rsp->rs_scandead = 0;
                        } else
                            rsp->rs_scandead = 1;
                    } else
                        rsp->rs_scandead = 0;
                } else
                    rsp->rs_scandead = 1;
            } else {
                rsp->rs_scandead = 1;
                printf("[SERR=%d]\n", NutTcpError(sock));
            }
            NutTcpCloseSocket(sock);
        }
    }
    NutSleep(30000);
}

/*!
 * \brief Initialize the MP3 player.
 *
 * Initializes the decoder and the decoder buffer and starts the 
 * player background thread.
 */
int ScannerInit(void)
{
    /* Start scanner thread. */
    if (NutThreadCreate("scanner", Scanner, 0, 512) == 0)
        return -1;

    return 0;
}

#endif /* DEV_ETHER */
