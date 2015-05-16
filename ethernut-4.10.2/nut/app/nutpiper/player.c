/*
 * Copyright (C) 2003-2006 by egnite Software GmbH
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
 * $Id: player.c 2955 2010-04-03 13:54:45Z haraldkipp $
 */


#include <sys/heap.h>
#include <sys/event.h>
#include <sys/timer.h>
#include <sys/thread.h>

#include <dev/vs1001k.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "player.h"
#include <sys/bankmem.h>

#define MAX_WAITSTREAM      20
#define BIGBUF_WATERMARK    65535UL


PLAYERINFO player;

#if defined(__AVR__)

/*
 * Process embedded meta data.
 */
static void ClearMetaData(void)
{
    if (player.psi_metatitle) {
        free(player.psi_metatitle);
        player.psi_metatitle = 0;
    }
    if (player.psi_metaurl) {
        free(player.psi_metaurl);
        player.psi_metaurl = 0;
    }
}

/*
 * Process embedded meta data.
 */
static int ProcessMetaData(void)
{
    uint8_t blks = 0;
    uint16_t cnt;
    int got;
    int rc = 0;
    uint8_t to_cnt = 0;
    char *mbuf;
    char *mn1;
    char *mn2;
    char *md1;
    char *md2;

    /*
     * Wait for the lenght byte.
     */
    while (player.psi_status == PSI_RUNNING) {
        if ((got = NutTcpReceive(player.psi_sock, &blks, 1)) == 1)
            break;
        if (got < 0 || to_cnt++ > MAX_WAITSTREAM) {
            printf("[NoLen]");
            return -1;
        }
    }
    if (blks) {
        if (blks > 32) {
            printf("[Blks=%u]", blks);
            return -1;
        }

        cnt = blks * 16;
        if ((mbuf = malloc(cnt + 1)) == 0) {
            printf("[NoMem]");
            return -1;
        }

        /*
         * Receive the metadata block.
         */
        while (player.psi_status == PSI_RUNNING) {
            if ((got = NutTcpReceive(player.psi_sock, mbuf + rc, cnt)) < 0) {
                printf("[RxFail]");
                return -1;
            }
            if (got) {
                to_cnt = 0;
                if ((cnt -= got) == 0)
                    break;
                rc += got;
                mbuf[rc] = 0;
            } else if (to_cnt++ > MAX_WAITSTREAM) {
                printf("[RxTo]");
                return -1;
            }
        }

        ClearMetaData();
        printf("\nMeta='%s'\n", mbuf);
        mn1 = mbuf;
        while (mn1) {
            if ((mn2 = strchr(mn1, ';')) != 0)
                *mn2++ = 0;
            if ((md1 = strchr(mn1, '=')) != 0) {
                *md1++ = 0;
                while (*md1 == ' ' || *md1 == '\'')
                    md1++;
                if ((md2 = strrchr(md1, '\'')) != 0)
                    *md2 = 0;
                if (strcasecmp(mn1, "StreamTitle") == 0 && player.psi_metatitle == 0) {
                    player.psi_metatitle = malloc(strlen(md1) + 1);
                    strcpy(player.psi_metatitle, md1);
                } else if (strcasecmp(mn1, "StreamUrl") == 0 && player.psi_metaurl == 0) {
                    player.psi_metaurl = malloc(strlen(md1) + 1);
                    strcpy(player.psi_metaurl, md1);
                }
            }
            mn1 = mn2;
        }
        free(mbuf);
        player.psi_metaupdate = 1;
    }
    return 0;
}


/*
 * Background thread for playing stream.
 */
THREAD(Player, arg)
{
    size_t rbytes;
    char  *mp3buf;
    uint8_t to_cnt = 0;
    int got;
    uint8_t ief;

    /*
     * Nut/OS threads run forever.
     */
    for (;;) {
        /*
         * Idle loop.
         */
        for (;;) {
            /* Broadcast idle status. */
            printf("[IDLE]");
            ClearMetaData();
            player.psi_status = PSI_IDLE;
            NutEventBroadcast(&player.psi_stsevt);

            /* Wait for start event. */
            NutEventWait(&player.psi_cmdevt, 0);
            printf("[EVT%u]", player.psi_status);
            if (player.psi_status == PSI_START)
                break;
        }

        /* Broadcast running event. */
        printf("[RUN]");
        player.psi_status = PSI_RUNNING;
        player.psi_mp3left = player.psi_metaint;
        NutEventBroadcast(&player.psi_stsevt);

        /* Reset decoder buffer. */
        ief = VsPlayerInterrupts(0);
        NutSegBufReset();
        VsPlayerInterrupts(ief);

        /*
         * Running loop.
         */
        while (player.psi_status == PSI_RUNNING) {

            /*
             * Query number of byte available in MP3 buffer. If it is
             * zero, then the player may have stopped running.
             */
            ief = VsPlayerInterrupts(0);
            mp3buf = NutSegBufWriteRequest(&rbytes);
            VsPlayerInterrupts(ief);
            if (rbytes < 1024) {
                if (VsGetStatus() != VS_STATUS_RUNNING)
                    VsPlayerKick();
                if (rbytes == 0) {
                    NutSleep(125);
                    continue;
                }
            }

            /* Do not read pass metadata. */
            if (player.psi_metaint && rbytes > player.psi_mp3left)
                rbytes = player.psi_mp3left;

            /*
             * Loop until MP3 buffer space is filled.
             */
            while (rbytes) {

                /* Read data directly into the MP3 buffer. */
                if ((got = NutTcpReceive(player.psi_sock, mp3buf, rbytes)) < 0) {
                    /* This is fatal. Return to idle state. */
                    printf("[RXFAIL]");
                    player.psi_status = PSI_IDLE;
                    break;
                }

                /* 
                 * Got some MP3 data. 
                 */
                if (got) {
                    /* Commit the buffer. */
                    ief = VsPlayerInterrupts(0);
                    mp3buf = NutSegBufWriteCommit(got);
                    VsPlayerInterrupts(ief);

                    /* Reset timeout counter and reduce number of buffer bytes. */
                    to_cnt = 0;
                    rbytes -= got;

                    /* Process meta data. */
                    if (player.psi_metaint) {
                        player.psi_mp3left -= got;
                        if (player.psi_mp3left == 0) {
                            if (ProcessMetaData()) {
                                printf("[METAFAIL]");
                                //player.psi_status = PSI_IDLE;
                                //break;
                            }
                            player.psi_mp3left = player.psi_metaint;
                        }
                    }

                    /* Start early with large buffers. */
                    if (VsGetStatus() != VS_STATUS_RUNNING) {
                        ief = VsPlayerInterrupts(0);
                        if ((NutSegBufUsed() > 2048 && player.psi_start) ||
                            (NutSegBufUsed() > BIGBUF_WATERMARK && NutSegBufAvailable() < BIGBUF_WATERMARK))
                            VsPlayerKick();
                        else
                            VsPlayerInterrupts(ief);
                    }
                    player.psi_start = 0;
                }

                /*
                 * Got no MP3 data. 
                 */
                else {
                    printf("[T%u, %u]", to_cnt, NutHeapAvailable());
                    NutSleep(100);
                    if (to_cnt++ > MAX_WAITSTREAM) {
                        /* If timeouts reach our limit, go back to idle state. */
                        printf("[RXTO]");
                        player.psi_status = PSI_IDLE;
                        break;
                    }
                }

                /*
                 * If we won't yield the CPU, we may consume the complete
                 * TCP buffer before being stopped in NutTcpReceive. That
                 * would result in bumpy feeding.
                 */
                NutThreadYield();
            }

            /* If play loop is not entered, we may omit the watchdog 
               update here and force a system reset. */
            NutThreadYield();
        }
        /* Flush decoder and wait until finished. */
        printf("[FLUSH]");
        VsPlayerFlush();
        while (VsGetStatus() == VS_STATUS_RUNNING) {
            NutSleep(63);
        }

        /* Reset the decoder. */
        printf("[RESET]");
        VsPlayerReset(0);
    }
}

/*!
 * \brief Initialize the MP3 player.
 *
 * Initializes the decoder and the decoder buffer and starts the 
 * player background thread.
 */
int PlayerInit(void)
{
    /* Init decoder. */
    if (VsPlayerInit() || VsPlayerReset(0))
        return -1;

    /* Start player thread. */
    if (NutThreadCreate("player", Player, 0, 512) == 0)
        return -1;

    return 0;
}

/*!
 * \brief Stop MP3 player.
 *
 * Force player into idle mode.
 */
int PlayerStop(uint32_t tmo)
{
    while (player.psi_status != PSI_IDLE) {
        printf("[STOP]");
        player.psi_status = PSI_STOP;
        NutEventPost(&player.psi_cmdevt);
        if (NutEventWait(&player.psi_stsevt, tmo)) {
            printf("[TOP]");
            return -1;
        }
    }
    printf("[STOPPED]");
    return 0;
}

/*!
 * \brief Start MP3 player.
 *
 * If the player is currently running, it will be stopped first.
 */
int PlayerStart(TCPSOCKET * sock, uint32_t metaint, uint32_t tmo)
{
    if (PlayerStop(tmo))
        return -1;

    /* Clear event queue and send start command. */
    NutEventBroadcast(&player.psi_stsevt);
    printf("[START]");
    player.psi_status = PSI_START;
    player.psi_sock = sock;
    player.psi_metaint = metaint;
    NutEventPost(&player.psi_cmdevt);

    /* The next event will be the result of our start command. */
    if (NutEventWait(&player.psi_stsevt, tmo) || player.psi_status != PSI_RUNNING) {
        printf("[TOS]");
        return -1;
    }
    return 0;
}

#endif /* __AVR__ */
