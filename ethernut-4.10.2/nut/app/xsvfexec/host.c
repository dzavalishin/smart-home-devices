/*
 * Copyright (C) 2004-2007 by egnite Software GmbH
 * Copyright (C) 2008 by egnite GmbH
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
 * and Xilinx Application Note XAPP058.
 */

/*
 * $Log$
 * Revision 1.1  2008/10/20 13:10:05  haraldkipp
 * Checked in.
 *
 */

#include <stdio.h>
#include <io.h>
#include <fcntl.h>

#if defined(ETHERNUT2) || defined(ETHERNUT3)

#include <sys/atom.h>
#include <sys/timer.h>
#include <sys/version.h>

#include <dev/debug.h>
#include <dev/urom.h>

#elif defined(_MSC_VER)

#include <winsock2.h>

#endif

#include "xsvf.h"
#include "tapsm.h"
#include "host.h"

/*!
 * \file host.c
 * \brief Platform dependant routines..
 */

/*!
 * \addtogroup xgHost
 */
/*@{*/


#ifdef XSVF_DEBUG

/*!
 * \brief XSVF command names.
 *
 * Used for debugging output.
 */
static char *cmd_names[] = {
    "XCOMPLETE",
    "XTDOMASK",
    "XSIR",
    "XSDR",
    "XRUNTEST",
    "UNKNOWN",
    "UNKNOWN",
    "XREPEAT",
    "XSDRSIZE",
    "XSDRTDO",
    "XSETSDRMASKS",
    "XSDRINC",
    "XSDRB",
    "XSDRC",
    "XSDRE",
    "XSDRTDOB",
    "XSDRTDOC",
    "XSDRTDOE",
    "XSTATE",
    "XENDIR",
    "XENDDR",
    "XSIR2",
    "XCOMMENT",
    "XWAIT",
    "UNKNOWN"
};

/*!
 * \brief TAP state names.
 *
 * Used for debugging output.
 */
static char *tap_names[] = {
    "Test-Logic-Reset",
    "Run-Test-Idle",
    "Select-DR-Scan",
    "Capture-DR",
    "Shift-DR",
    "Exit1-DR",
    "Pause-DR",
    "Exit2-DR",
    "Update-DR",
    "Select-IR-Scan",
    "Capute-IR",
    "Shift-IR",
    "Exit1-IR",
    "Pause-IR",
    "Exit2-IR",
    "Update-IR",
    "Unknown"
};

#endif

/*!
 * \brief Handle of XSVF file.
 */
static int fh;

/*!
 * \brief Last error occured in this module.
 */
static int xsvf_err;

/*!
 * \brief Initialize the platform dependant interface.
 *
 * All required hardware initializations should be done in this
 * routine. We may also initiate debug output. If the XSVF date
 * is located in a file system, then the file will be opened
 * here as well.
 *
 * \return Zero on success, otherwise an error code is returned.
 */
int XsvfInit(void)
{
#if defined(ETHERNUT2) || defined(ETHERNUT3)
    uint32_t baud = 115200;

    /*
     * Prepare standard output and display a banner.
     */
    NutRegisterDevice(&devDebug0, 0, 0);
    freopen("uart0", "w", stdout);
    _ioctl(_fileno(stdout), UART_SETSPEED, &baud);
    printf("\n100 XSVF-Executor " XSVFEXEC_VERSION " on Nut/OS %s", NutVersionString());
    NutSleep(100);

    /*
     * Register our device for the file system.
     */
    NutRegisterDevice(&devUrom, 0, 0);

#ifdef ETHERNUT2
    /*
     * Disable JTAG. Ethernut 2 is using
     *
     * PF4 for target TCK.
     * PF5 for target TMS.
     * PF6 for target TDI.
     * PF7 for target TDO.
     */
    NutEnterCritical();
    outb(MCUCSR, _BV(JTD));
    outb(MCUCSR, _BV(JTD));
    NutExitCritical();
    outb(DDRF, 0x70);
#endif

#ifdef ETHERNUT3
    /*
     * Ethernut 3 is using
     *
     * P18 for target TCK.
     * P16 for target TMS.
     * P23 for target TDI.
     * P19 for target TDO.
     */
    outr(PIO_OER, _BV(16) | _BV(18) | _BV(23));
    outr(PIO_ODR, _BV(19));
#endif

#elif defined(_WIN32)

    printf("\n100 XSVF-Executor " XSVFEXEC_VERSION " on WIN32");

#endif

    /* The XSVF file is expected in the UROM file system. */
    fh = _open(XSVFNAME, _O_BINARY | _O_RDONLY);

    return (fh == -1) ? XE_DATAUNDERFLOW : 0;
}

/*!
 * \brief Shutdown the platform dependant interface.
 *
 * On most embedded platforms this routine will never return.
 *
 * \param rc Programming result code.
 */
void XsvfExit(int rc)
{
    /* Close XSVF file. */
    if(fh != -1) {
        _close(fh);
    }

#ifdef ETHERNUT3
    /* Re-enable UART0. */
    outb(0x21000000, 0x20);
    NutSleep(100);
#endif
    /* Display programming result. */
    if(rc) {
        printf("\n4%02d ERROR\n", rc);
    }
    else {
        puts("\n199 OK");
    }

#if defined(ETHERNUT2) || defined(ETHERNUT3)
    /* Nut/OS applications never return. */
    for (;;) {
        NutSleep(1000);
    }
#endif
}

/*!
 * \brief Retrieve the last error occured in this module.
 *
 * \return Error code or 0 if no error occured.
 */
int XsvfGetError(void)
{
    return xsvf_err;
}

/*!
 * \brief Get next byte from XSVF buffer.
 *
 * Call XsvfGetError() to check for errors,
 *
 * \return Byte value.
 */
uint8_t XsvfGetByte(void)
{
    uint8_t rc;

    if(_read(fh, &rc, sizeof(rc)) != sizeof(rc)) {
        xsvf_err = XE_DATAUNDERFLOW;
    }
#ifdef XSVF_DEBUG
    printf("[%u]", rc);
#endif

    return rc;
}

/*!
 * \brief Get next command byte from XSVF buffer.
 *
 * \return XSVF command or XUNKNOWN if an error occured.
 */
uint8_t XsvfGetCmd(void)
{
    uint8_t rc;
#ifdef ETHERNUT2
    static char cnt;
    if (++cnt == 0) {
        NutSleep(1);
    }
#endif

    if(_read(fh, &rc, sizeof(rc)) != sizeof(rc) || rc >= XUNKNOWN) {
        rc = XUNKNOWN;
    }

#ifdef XSVF_DEBUG
    printf("\n2%02d %s", rc, cmd_names[rc]);
#endif

    return rc;
}

/*!
 * \brief Get next byte from XSVF buffer and select a TAP state.
 *
 * \param state0 Returned state, if the byte value is zero.
 * \param state1 Returned state, if the byte value is one.
 * 
 * \return TAP state or UNKNOWN_STATE if an error occured.
 */
uint8_t XsvfGetState(uint8_t state0, uint8_t state1)
{
    uint8_t rc;

    if(_read(fh, &rc, sizeof(rc)) != sizeof(rc) || rc > 1) {
        rc = UNKNOWN_STATE;
    }
    else if(rc) {
        rc = state1;
    }
    else {
        rc = state0;
    }

#ifdef XSVF_DEBUG
    printf("{%s}", tap_names[rc]);
#endif

    return rc;
}

/*!
 * \brief Get next short value from XSVF buffer.
 *
 * Call XsvfGetError() to check for errors,
 *
 * \return Short value.
 */
short XsvfGetShort(void)
{
    short rc;

    if(_read(fh, &rc, sizeof(rc)) != sizeof(rc)) {
        xsvf_err = XE_DATAUNDERFLOW;
        return -1;
    }
    rc = ntohs(rc);

#ifdef XSVF_DEBUG
    printf("[%d]", rc);
#endif

    return rc;
}

/*!
 * \brief Get next long value from XSVF buffer.
 *
 * Call XsvfGetError() to check for errors,
 *
 * \return Long value.
 */
long XsvfGetLong(void)
{
    long rc;

    if(_read(fh, &rc, sizeof(rc)) != sizeof(rc)) {
        xsvf_err = XE_DATAUNDERFLOW;
        rc = 0;
    }
    else {
        rc = ntohl(rc);
    }

#ifdef XSVF_DEBUG
    printf("[%ld]", rc);
#endif

    return rc;
}

/*!
 * \brief Read a specified number of bits from XSVF buffer.
 *
 * \param buf Pointer to the buffer which receives the bit string.
 * \param num Number of bits to read.
 *
 * \return Error code or 0 if no error occured.
 */
int XsvfReadBitString(void *buf, int num)
{
    int len = (num + 7) / 8;

    if (len > MAX_BITVEC_BYTES) {
        xsvf_err = len = XE_DATAOVERFLOW;
    }
    else if(_read(fh, buf, len) < len) {
        xsvf_err = len = XE_DATAUNDERFLOW;
    }

#ifdef XSVF_DEBUG
    else {
        uint8_t *cp = buf;

        printf("[%u:", num);
        while(len-- > 0) {
            printf("%02X", *cp);
            cp++;
        }
        printf("]");
    }
#endif

    return len;
}

/*!
 * \brief Skip comment in the XSVF buffer.
 *
 * \return Error code or 0 if no error occured.
 */
int XsvfSkipComment(void)
{
    uint8_t ch;

    for(;;) {
        if(_read(fh, &ch, sizeof(ch)) != sizeof(ch)) {
            return (xsvf_err = XE_DATAUNDERFLOW);
        }
        if(ch == 0) {
            break;
        }
#ifdef XSVF_DEBUG
        putchar(ch);
#endif
    }
    return 0;
}

/*!
 * \brief Microsecond delay.
 *
 * \param usecs Number of microseconds.
 */
void XsvfDelay(long usecs)
{
#if defined(ETHERNUT2) || defined(ETHERNUT3)

    usecs = usecs / 1000L;
    NutSleep(usecs + 1);

#elif defined(_MSC_VER)

    Sleep((usecs + 500L) / 1000L);

#endif
}

/*@}*/
