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
 * $Id: playmp3.c 3538 2011-08-10 16:34:10Z haraldkipp $
 */

#include <dev/board.h>
#include <dev/vs1001k.h>
#include <dev/debug.h>
#include <dev/urom.h>

#include <sys/version.h>
#include <sys/heap.h>
#include <sys/event.h>
#include <sys/timer.h>
#include <sys/thread.h>
#include <sys/bankmem.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <errno.h>

#if defined(__AVR__)
static int PlayMp3File(char *path);
#endif

/*!
 * \example playmp3/playmp3.c
 *
 * To run this example code, you need to attach the Medianut Board to the
 * Ethernut or use a similar hardware design based on the VS1001K MP3 decoder.
 *
 * This sample application plays MP3 files from the UROM filesystem. It 
 * demonstrates how to use the global segmented buffer and the MP3 decoder 
 * driver and can provide a basis for talking equipment, alarm sound output 
 * etc.
 *
 * The UROM filesystem is located in the CPU's flash ROM. No external file
 * storage device is required. Use the crurom utility to create a C source 
 * file named urom.c from the MP3 files located in subdirectory sounds. 
 * Here's how to call crurom:
 * 
 * crurom -r -ourom.c sounds
 *
 * The created file will then be compiled and linked to the application 
 * code.
 *
 * UART0 is used for debug output.
 */
int main(void)
{
    /* Baudrate for debug output. */
    uint32_t baud = 115200;

    /*
     * Register our devices.
     */
    NutRegisterDevice(&devUrom, 0, 0);
    NutRegisterDevice(&DEV_CONSOLE, 0, 0);

    /*
     * Assign stdout to the debug device.
     */
    freopen(DEV_CONSOLE_NAME, "w", stdout);
    _ioctl(_fileno(stdout), UART_SETSPEED, &baud);

    /*
     * Print a banner.
     */
    printf("\n\nPlay MP3 files on Nut/OS %s\n", NutVersionString());

#if defined(__AVR__)

    /*
     * Initialize the MP3 buffer. The NutSegBuf routines provide a global
     * system buffer, which works with banked and non-banked systems.
     */
    if (NutSegBufInit(8192) == 0) {
        puts("NutSegBufInit: Fatal error");
    }

    /* 
     * Initialize the MP3 decoder hardware.
     */
    if (VsPlayerInit() || VsPlayerReset(0)) {
        puts("VsPlayer: Fatal error");
    }

    /*
     * Play the MP3 files in an endless loop. For each file set the volume 
     * of the left and right channel, call the local routine PlayMp3File() 
     * and sleep one second before playing the next sound file.
     */
    for (;;) {
        VsSetVolume(0, 254);
        PlayMp3File("UROM:sound1a.mp3");
        NutSleep(1000);

        VsSetVolume(0, 0);
        PlayMp3File("UROM:sound2a.mp3");
        NutSleep(1000);

        VsSetVolume(254, 0);
        PlayMp3File("UROM:sound3a.mp3");
        NutSleep(1000);

        VsSetVolume(0, 0);
        PlayMp3File("UROM:sound4a.mp3");
        NutSleep(1000);
    }
#else /* !__AVR__ */
    for (;;);
#endif /* !__AVR__ */
    return 0;
}

#if defined(__AVR__)

/*
 * Play MP3 file from local file system.
 *
 * \param path Pathname of the MP3 file to play.
 *
 * \return 0 on success, -1 if opening the file failed.
 */
static int PlayMp3File(char *path)
{
    int fd;
    size_t rbytes;
    char  *mp3buf;
    int    got;
    uint8_t ief;

    /*
     * Open the MP3 file.
     */
    printf("Play %s: ", path);
    if ((fd = _open(path, _O_RDONLY | _O_BINARY)) == -1) {
        printf("Error %d\n", errno);
        return -1;
    }
    puts("OK");

    /* 
     * Reset decoder buffer.
     */
    printf("[B.RST]");
    ief = VsPlayerInterrupts(0);
    NutSegBufReset();
    VsPlayerInterrupts(ief);

    for (;;) {
        /*
         * Query number of byte available in MP3 buffer.
         */
        ief = VsPlayerInterrupts(0);
        mp3buf = NutSegBufWriteRequest(&rbytes);
        VsPlayerInterrupts(ief);

        /* 
         * Read data directly into the MP3 buffer. 
         */
        if (rbytes) {
            printf("[B.RD%d]", rbytes);
            if ((got = _read(fd, mp3buf, rbytes)) > 0) {
                printf("[B.CMT%d]", got);
                ief = VsPlayerInterrupts(0);
                mp3buf = NutSegBufWriteCommit(got);
                VsPlayerInterrupts(ief);
            } else {
                printf("[EOF]");
                break;
            }
        }

        /*
         * If the player is not running, kick it.
         */
        if (VsGetStatus() != VS_STATUS_RUNNING) {
            printf("[P.KICK]");
            VsPlayerKick();
        }

        /*
         * Allow background threads to take over.
         */
        NutThreadYield();
    }

    _close(fd);

    /* 
     * Flush decoder and wait until finished. 
     */
    printf("[P.FLUSH]");
    VsPlayerFlush();
    while (VsGetStatus() != VS_STATUS_EMPTY) {
        NutSleep(1);
    }

    /*
     * Reset the decoder. 
     */
    printf("[P.RST]");
    VsPlayerReset(0);

    printf("\nDone, %u bytes free\n", NutHeapAvailable());
    return 0;
}

#endif /* !__AVR__ */
