/*
 * Copyright (C) 2001-2006 by egnite Software GmbH
 * Copyright (C) 2010-2011 by egnite GmbH
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
 * \file arch/arm/dev/debug_at91.c
 * \brief AT91 debug output device.
 *
 * \verbatim
 * $Id: debug_at91.c 3538 2011-08-10 16:34:10Z haraldkipp $
 * \endverbatim
 */

#include <cfg/uart.h>
#include <arch/arm/atmel/debug_at91.h>

#include <dev/debug.h>
#include <sys/timer.h>


/*!
 * \addtogroup xgDevDebugAt91
 */
/*@{*/

/*!
 * \brief Calculate divisor for a given baud rate.
 *
 * \param baud Baud rate.
 *
 * \return Calculated divisor.
 */
uint32_t At91BaudRateDiv(uint32_t baud)
{
    return (NutClockGet(NUT_HWCLK_PERIPHERAL) / (8 * baud) + 1) / 2;
}

/*!
 * \brief Handle I/O controls for debug device 2.
 *
 * The debug device supports UART_SETSPEED only.
 *
 * \return 0 on success, -1 otherwise.
 */
int At91DevDebugIOCtl(NUTDEVICE * dev, int req, void *conf)
{
    if(req == UART_SETSPEED) {
        outr(dev->dev_base + US_BRGR_OFF, At91BaudRateDiv(*((uint32_t *)conf)));
        return 0;
    }
    return -1;
}

/*!
 * \brief Send a single character to debug device 0.
 *
 * A newline character will be automatically prepended
 * by a carriage return.
 */
static void DebugPut(CONST NUTDEVICE * dev, char ch)
{
    if (ch == '\n') {
	    while ((inr(dev->dev_base + US_CSR_OFF) & US_TXRDY) == 0);
        outr(dev->dev_base + US_THR_OFF, '\r');
    }
    while ((inr(dev->dev_base + US_CSR_OFF) & US_TXRDY) == 0);
    outr(dev->dev_base + US_THR_OFF, ch);
}

/*!
 * \brief Send characters to debug device 0.
 *
 * A newline character will be automatically prepended
 * by a carriage return.
 *
 * \return Number of characters sent.
 */
int At91DevDebugWrite(NUTFILE * fp, CONST void *buffer, int len)
{
    int c = len;
    CONST char *cp = buffer;

    while (c--) {
        DebugPut(fp->nf_dev, *cp++);
    }
    return len;
}

#ifdef NUT_DEV_DEBUG_READ
/*!
 * \brief Read characters from debug device.
 *
 * This function is called by the low level input routines of the
 * \ref xrCrtLowio "C runtime library", using the _NUTDEVICE::dev_read
 * entry.
 *
 * The function will block the calling thread until at least one
 * character has been received.
 *
 * \param fp     Pointer to a \ref _NUTFILE structure, obtained by a
 *               previous call to At91DevDebugOpen().
 * \param buffer Pointer to the buffer that receives the data. If zero,
 *               then all characters in the input buffer will be
 *               removed.
 * \param size   Maximum number of bytes to read.
 *
 * \return The number of bytes read, which may be less than the number
 *         of bytes specified. A return value of -1 indicates an error,
 *         while zero is returned in case of a timeout.
 */
int At91DevDebugRead(NUTFILE * fp, void *buffer, int size)
{
    int rc;
    unsigned int ch;
    char *bp = (char *) buffer;

    /* Wait for the first character, forever. */
    for (rc = 0; rc < size; rc++) {
        while ((inr(fp->nf_dev->dev_base + US_CSR_OFF) & US_RXRDY) == 0) {
            NutSleep(1);
            if ((rc || bp == NULL) && 
                (inr(fp->nf_dev->dev_base + US_CSR_OFF) & US_RXRDY) == 0) {
                return rc;
            }
        }
        ch = inr(fp->nf_dev->dev_base + US_RHR_OFF);
        if (bp) {
            if (ch == '\r') {
                *bp++ = '\n';
            } else {
                *bp++ = (char) ch;
            }
        }
    }
    return rc;
}

/*!
 * \brief Retrieves the number of characters in input buffer.
 *
 * This function is called by the low level size routine of the C runtime
 * library, using the _NUTDEVICE::dev_size entry.
 *
 * \param fp     Pointer to a \ref _NUTFILE structure, obtained by a
 *               previous call to UsartOpen().
 *
 * \return The number of bytes currently stored in input buffer.
 */
long At91DevDebugSize(NUTFILE *fp)
{
    if (inr(fp->nf_dev->dev_base + US_CSR_OFF) & US_RXRDY) {
        return 1;
    }
    return 0;
}

#endif

/*!
 * \brief Open debug device.
 *
 * \return Pointer to a static NUTFILE structure.
 */
NUTFILE *At91DevDebugOpen(NUTDEVICE * dev, CONST char *name, int mode, int acc)
{
    NUTFILE *fp = (NUTFILE *) (dev->dev_dcb);

    fp->nf_next = 0;
    fp->nf_dev = dev;
    fp->nf_fcb = 0;

    return fp;
}

/*!
 * \brief Close debug device.
 *
 * \return Always 0.
 */
int At91DevDebugClose(NUTFILE * fp)
{
    return 0;
}

/*@}*/
