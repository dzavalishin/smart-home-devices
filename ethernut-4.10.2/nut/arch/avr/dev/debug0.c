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
 */

/*!
 * \file arch/avr/dev/debug0.c
 * \brief AVR debug output device.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.5  2008/08/11 06:59:14  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.4  2006/10/08 16:48:07  haraldkipp
 * Documentation fixed
 *
 * Revision 1.3  2005/10/17 08:46:53  hwmaier
 * Setting baudrate function changed: For CPUs w/ 12 and 16 MHz xtal double rate mode is now used (only if set by NUT_CPU_FREQ)
 *
 * Revision 1.2  2005/08/02 17:46:45  haraldkipp
 * Major API documentation update.
 *
 * Revision 1.1  2005/07/26 18:02:27  haraldkipp
 * Moved from dev.
 *
 * Revision 1.4  2005/02/06 16:36:59  haraldkipp
 * Fixes ICCAVR V7 baudrate miscalculation.
 *
 * Revision 1.3  2004/02/25 16:19:09  haraldkipp
 * Support baudrate settings
 *
 * Revision 1.2  2003/11/03 17:17:47  haraldkipp
 * Comments added
 *
 * Revision 1.1.1.1  2003/05/09 14:40:36  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.2  2003/05/06 18:29:42  harald
 * ICCAVR port
 *
 * Revision 1.1  2003/04/07 12:15:37  harald
 * First release
 *
 * \endverbatim
 */

#include <dev/debug.h>

#include <cfg/os.h>
#include <sys/device.h>
#include <sys/file.h>
#include <sys/timer.h>

/*!
 * \addtogroup xgDevDebugAvr
 */
/*@{*/

static NUTFILE dbgfile;

/*!
 * \brief Handle I/O controls for debug device 0.
 *
 * The debug device doesn't support any.
 *
 * \return Always -1.
 */
static int DebugIOCtl(NUTDEVICE * dev, int req, void *conf)
{
    if(req == UART_SETSPEED) {
#if defined(__AVR_ENHANCED__) && ((NUT_CPU_FREQ == 12000000) || (NUT_CPU_FREQ == 16000000))
        /* On enhanced MCUs with 12.0 or 16.0 MHz we use double rate mode,
         * so we can use 115200 bps with 12.0 MHz crystals
         * and 57600 with 16.0 MHz crystals.
         */
        sbi(UCSR0A, U2X0);
        outb(UBRR, (uint8_t) ((((2UL * NutGetCpuClock()) / (*((uint32_t *)conf) * 8UL)) + 1UL) / 2UL) - 1UL);
#else
        outb(UBRR, (uint8_t) ((((2UL * NutGetCpuClock()) / (*((uint32_t *)conf) * 16UL)) + 1UL) / 2UL) - 1UL);
#endif
        return 0;
    }
    return -1;
}

/*!
 * \brief Initialize debug device 0.
 *
 * Simply enable the device. Baudrate divisor set to 7 for
 * 115.2 kBaud at 14.7456 MHz.
 *
 * \return Always 0.
 */
static int DebugInit(NUTDEVICE * dev)
{
    /* Note: Default baudrate has been set in nutinit.c */
    UCR = BV(RXEN) | BV(TXEN);
    return 0;
}

/*!
 * \brief Send a single character to debug device 0.
 *
 * A carriage return character will be automatically appended
 * to any linefeed.
 */
static void DebugPut(char ch)
{
    if(ch == '\n') {
	    while((USR & BV(UDRE)) == 0);
	    UDR = '\r';
    }
    while((USR & BV(UDRE)) == 0);
    UDR = ch;
}

/*!
 * \brief Send characters to debug device 0.
 *
 * A carriage return character will be automatically appended
 * to any linefeed.
 *
 * \return Number of characters sent.
 */
static int DebugWrite(NUTFILE * fp, CONST void *buffer, int len)
{
    int c = len;
    CONST char *cp = buffer;

    while(c--)
        DebugPut(*cp++);
    return len;
}

/*!
 * \brief Send characters from progam memory to debug device 0.
 *
 * A carriage return character will be automatically appended
 * to any linefeed.
 *
 * \return Number of characters sent.
 */
static int DebugWrite_P(NUTFILE * fp, PGM_P buffer, int len)
{
    int c = len;
    PGM_P cp = buffer;

    while(c--) {
        DebugPut(PRG_RDB(cp));
        cp++;
    }
    return len;
}

/*!
 * \brief Open debug device 0.
 *
 * \return Pointer to a static NUTFILE structure.
 */
static NUTFILE *DebugOpen(NUTDEVICE * dev, CONST char *name, int mode, int acc)
{
    dbgfile.nf_next = 0;
    dbgfile.nf_dev = dev;
    dbgfile.nf_fcb = 0;

    return &dbgfile;
}

/*!
 * \brief Close debug device 0.
 *
 * \return Always 0.
 */
static int DebugClose(NUTFILE * fp)
{
    return 0;
}

/*!
 * \brief Debug device 0 information structure.
 */
NUTDEVICE devDebug0 = {
    0,                          /*!< Pointer to next device. */
    {'u', 'a', 'r', 't', '0', 0, 0, 0, 0},      /*!< Unique device name. */
    0,                          /*!< Type of device. */
    0,                          /*!< Base address. */
    0,                          /*!< First interrupt number. */
    0,                          /*!< Interface control block. */
    0,                          /*!< Driver control block. */
    DebugInit,                  /*!< Driver initialization routine. */
    DebugIOCtl,                 /*!< Driver specific control function. */
    0,
    DebugWrite,
    DebugWrite_P,
    DebugOpen,
    DebugClose,
    0
};

/*@}*/
