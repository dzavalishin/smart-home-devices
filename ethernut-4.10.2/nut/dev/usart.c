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
 */

/*
 * $Log$
 * Revision 1.10  2009/02/13 14:52:05  haraldkipp
 * Include memdebug.h for heap management debugging support.
 *
 * Revision 1.9  2008/08/11 06:59:42  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.8  2007/03/17 14:33:21  haraldkipp
 * Workaround for AVRGCC 4.1.1 bug, which failed to compile UsartIOCtl().
 *
 * Revision 1.7  2006/10/05 17:20:54  haraldkipp
 * Added a comment to warn the user about ioctl() functions, that may not be
 * supported.
 *
 * Revision 1.6  2006/08/23 09:20:47  freckle
 * fix bug #1541139
 *
 * Revision 1.5  2004/10/14 16:43:06  drsung
 * Fixed compiler warning "comparison between signed and unsigned"
 *
 * Revision 1.4  2004/05/24 20:17:15  drsung
 * Added function UsartSize to return number of chars in input buffer.
 *
 * Revision 1.3  2004/05/20 09:05:07  drsung
 * Memory was allocated twice for NUTFILE in UsartOpen.
 *
 * Revision 1.2  2004/03/18 13:59:14  haraldkipp
 * Comment updated
 *
 * Revision 1.1  2003/12/15 19:25:33  haraldkipp
 * New USART driver added
 *
 */

#include <compiler.h>
#include <stdlib.h>
#include <string.h>
#include <memdebug.h>

#include <sys/atom.h>
#include <sys/heap.h>
#include <sys/event.h>
#include <sys/timer.h>

#include <dev/irqreg.h>
#include <dev/usart.h>

#include <fcntl.h>

/*
 * Not nice because stdio already defined them. But in order to save memory,
 * we do the whole buffering and including stdio here would be more weird.
 */
#ifndef _IOFBF
#define _IOFBF  0x00
#define _IOLBF  0x01
#define _IONBF  0x02
#endif

/*!
 * \addtogroup xgUsart
 */
/*@{*/

/*!
 * \brief Initialize the USART device.
 *
 * This function is called by NutRegisterDevice(), using the 
 * _NUTDEVICE::dev_init entry. It will call the low level
 * driver's _USARTDCB::dcb_init routine to initialize the hardware.
 *
 * \param dev Identifies the device to initialize.
 *
 * \return 0 on success, -1 otherwise.
 *
 * \todo Read initial settings from EEPROM.
 */
int UsartInit(NUTDEVICE * dev)
{
    int rc;
    USARTDCB *dcb = dev->dev_dcb;

    /* Initialize the low level hardware driver. */
    if ((rc = (*dcb->dcb_init) ()) == 0) {
        /* Ignore errors on initial configuration. */
        (*dcb->dcb_set_speed) (USART_INITSPEED);
    }
    return rc;
}

/*!
 * \brief Reset an USART ring buffer.
 *
 * \param rbf Pointer to the ring buffer structure.
 * \param size Buffer size.
 * \param lowm Low watermark.
 * \param hiwm High watermark.
 *
 * \return 0 on success, -1 if changing the buffer size fails.
 */
static int UsartResetBuffer(RINGBUF * rbf, size_t size, size_t lowm, size_t hiwm)
{
    uint8_t *xbp = rbf->rbf_start;
    size_t xsz = rbf->rbf_siz;

    /* Disable further buffer usage. */
    NutEnterCritical();
    rbf->rbf_siz = 0;
    NutExitCritical();

    /* Resize the buffer, if required. */
    if (xsz != size) {
        if (xsz && xbp) {
            free(xbp);
        }
        if (size && (xbp = malloc(size)) == 0) {
            return -1;
        }
    }

    /* Update ring buffer status. */
    if (size) {
        rbf->rbf_start = xbp;
        rbf->rbf_head = xbp;
        rbf->rbf_tail = xbp;
        rbf->rbf_last = xbp + size;
        rbf->rbf_lwm = lowm;
        rbf->rbf_hwm = hiwm;
        rbf->rbf_cnt = 0;

        /* Re-enable buffer usage. */
        NutEnterCritical();
        rbf->rbf_siz = size;
        NutExitCritical();
    }
    return 0;
}

/*!
 * \brief Read from device.
 *
 * This function is called by the low level input routines of the
 * \ref xrCrtLowio "C runtime library", using the _NUTDEVICE::dev_read
 * entry.
 *
 * The function may block the calling thread until at least one
 * character has been received or a timeout occurs.
 *
 * It is recommended to set a proper read timeout with software handshake.
 * In this case a timeout may occur, if the communication peer lost our
 * last XON character. The application may then use ioctl() to disable the
 * receiver and do the read again. This will send out another XON.
 *
 * \param fp     Pointer to a \ref _NUTFILE structure, obtained by a
 *               previous call to UsartOpen().
 * \param buffer Pointer to the buffer that receives the data. If zero,
 *               then all characters in the input buffer will be
 *               removed.
 * \param size   Maximum number of bytes to read.
 *
 * \return The number of bytes read, which may be less than the number
 *         of bytes specified. A return value of -1 indicates an error,
 *         while zero is returned in case of a timeout.
 */
int UsartRead(NUTFILE * fp, void *buffer, int size)
{
    size_t rc;
    size_t avail;
    size_t taken = 0;
    uint8_t ch;
    uint8_t *cp = buffer;
    NUTDEVICE *dev = fp->nf_dev;
    USARTDCB *dcb = dev->dev_dcb;
    RINGBUF *rbf = &dcb->dcb_rx_rbf;

    /*
     * No buffer allocated, this device is read only.
     */
    if (rbf->rbf_siz == 0) {
        return -1;
    }

    /*
     * Call without data pointer discards receive buffer.
     */
    if (buffer == 0) {
        UsartResetBuffer(rbf, rbf->rbf_siz, rbf->rbf_lwm, rbf->rbf_hwm);
        (*dcb->dcb_rx_start) ();
        return 0;
    }

    /*
     * Wait until at least one character is buffered or until a read
     * timeout occured.
     */
    for (;;) {
        /* Atomic access to the ring buffer counter. */
        NutEnterCritical();
        avail = rbf->rbf_cnt;
        NutExitCritical();
        if (avail) {
            break;
        }
        /*
         * This will enable RTS hardware handshake or re-enable the
         * remote transmitter by sending a XON character.
         */
        (*dcb->dcb_rx_start) ();
        if (NutEventWait(&rbf->rbf_que, dcb->dcb_rtimeout)) {
            return 0;
        }
        /* Device closed by another thread. */
        if (rbf->rbf_siz == 0) {
            return -1;
        }
    }

    /*
     * Get cooked characters from receive buffer.
     */
    if (dcb->dcb_modeflags & USART_MF_COOKEDMODE) {
        for (rc = 0; rc < (size_t) size;) {
            if (taken >= avail) {
                break;
            }
            ch = *rbf->rbf_tail++;
            if (rbf->rbf_tail == rbf->rbf_last) {
                rbf->rbf_tail = rbf->rbf_start;
            }
            taken++;
            if (ch == '\r' || ch == '\n') {
                if (dcb->dcb_last_eol == 0 || dcb->dcb_last_eol == ch) {
                    dcb->dcb_last_eol = ch;
                    *cp++ = '\n';
                    rc++;
                }
            } else {
                dcb->dcb_last_eol = 0;
                *cp++ = ch;
                rc++;
            }
        }
    }

    /*
     * Get raw characters from receive buffer.
     */
    else {
        if ((rc = size) > avail)
            rc = avail;
        for (taken = 0; taken < rc; taken++) {
            *cp++ = *rbf->rbf_tail++;
            if (rbf->rbf_tail == rbf->rbf_last) {
                rbf->rbf_tail = rbf->rbf_start;
            }
        }
    }

    if (taken) {
        NutEnterCritical();
        rbf->rbf_cnt -= taken;
        NutExitCritical();
        if (rbf->rbf_cnt < rbf->rbf_lwm) {
            (*dcb->dcb_rx_start) ();
        }
    }
    return (int) rc;
}

/*!
 * \brief Flush output buffer.
 *
 * The current thread will be blocked until all characters upto a specified
 * value have been written or until a timeout occured.
 *
 * \param  rbf   Pointer to a ring buffer structure.
 * \param  added Number of bytes to add to the ring buffer counter. The
 *               characters must have been added without updating the
 *               counter. Because access to the counter has to be atomic,
 *               this parameter simplifies the calling routine a bit.
 * \param  left  The number of bytes left in the buffer before this
 *               function returns.
 * \param  tmo   Timeout in milliseconds.
 *
 * \return Number of bytes left in the output buffer, which is greater
 *         than the specified value in case of a timeout.
 */
static size_t UsartFlushOutput(USARTDCB *dcb, size_t added, size_t left)
{
    size_t rc;
    RINGBUF *rbf = &dcb->dcb_tx_rbf;

    /*
     * Add the new characters to the buffer count.
     */
    NutEnterCritical();
    rbf->rbf_cnt += added;
    rc = rbf->rbf_cnt;
    NutExitCritical();

    while (rc > left) {
        /* Start transmitter and wait for the next event. */
        (*dcb->dcb_tx_start) ();
        if (NutEventWait(&rbf->rbf_que, dcb->dcb_wtimeout)) {
            break;
        }
        /* Refresh the count. */
        NutEnterCritical();
        rc = rbf->rbf_cnt;
        NutExitCritical();
    };
    return rc;
}

/*!
 * \brief Write to device.
 *
 * \param dev    Pointer to a previously registered NUTDEVICE structure.
 * \param buffer Pointer the data to write.
 * \param len    Number of data bytes to write.
 * \param pflg   If this flag is set, then the buffer is located in program
 *               space.
 *
 * \return The number of bytes written. In case of a write timeout, this
 *         may be less than the specified length.
 */
static int UsartPut(NUTDEVICE * dev, CONST void *buffer, int len, int pflg)
{
    int rc;
    CONST uint8_t *cp;
    uint8_t lbmode;
    ureg_t cooked;
    uint8_t ch;
    size_t cnt;
    size_t added;
    USARTDCB *dcb = dev->dev_dcb;
    RINGBUF *rbf = &dcb->dcb_tx_rbf;

    /*
     * No output ring buffer allocated, this device is read only.
     */
    if (rbf->rbf_siz == 0) {
        return -1;
    }

    /*
     * Call without data pointer flushes the buffer. In this case a return
     * value not equal zero indicates write timeout.
     */
    if (buffer == 0) {
        return UsartFlushOutput(dcb, 0, 0);
    }

    if (dcb->dcb_modeflags & USART_MF_LINEBUFFER)
        lbmode = 1;
    else
        lbmode = 0;

    if (dcb->dcb_modeflags & USART_MF_COOKEDMODE)
        cooked = 1;
    else
        cooked = 0;

    /*
     * Get the number of buffered bytes. The transmit interrupt will modify
     * this value, so make the query atomic.
     */
    NutEnterCritical();
    cnt = rbf->rbf_cnt;
    NutExitCritical();

    /*
     * Move bytes to the transmit buffer.
     */
    cp = buffer;
    added = 0;
    for (rc = 0; rc < len;) {
        /*
         * If we reached the high watermark, then kick the hardware driver
         * to start transmission and wait until the low watermark is reached.
         */
        if (cnt + added >= rbf->rbf_hwm) {
            cnt = UsartFlushOutput(dcb, added, rbf->rbf_lwm);
            added = 0;
            /* If still above the mark, then a timeout occured. */
            if(cnt > rbf->rbf_lwm) {
                break;
            }
        }

        /*
         * Get the next data byte. If the pflg parameter is set, then data
         * is located in program space.
         */
        ch = pflg ? PRG_RDB(cp) : *cp;

        /*
         * In cooked mode we prepend a carriage return to any linefeed
         * character.
         */
        if (cooked == 1 && ch == '\n') {
            cooked = 2;
            ch = '\r';
            if (lbmode == 1)
                lbmode = 2;
        } else {
            if (cooked == 2)
                cooked = 1;
            cp++;
            rc++;
        }
        *rbf->rbf_head++ = ch;
        if (rbf->rbf_head == rbf->rbf_last) {
            rbf->rbf_head = rbf->rbf_start;
        }
        added++;
    }

    if (added) {
        NutEnterCritical();
        rbf->rbf_cnt += added;
        NutExitCritical();
        (*dcb->dcb_tx_start) ();
    }

    return rc;
}

/*!
 * \brief Write a device or file.
 *
 * This function is called by the low level output routines of the
 * \ref xrCrtLowio "C runtime library", using the
 * \ref _NUTDEVICE::dev_write entry.
 *
 * The function may block the calling thread.
 *
 * \param fp     Pointer to a _NUTFILE structure, obtained by a previous
 *               call to UsartOpen().
 * \param buffer Pointer to the data to be written. If zero, then the
 *               output buffer will be flushed.
 * \param len    Number of bytes to write.
 *
 * \return The number of bytes written, which may be less than the number
 *         of bytes specified if a timeout occured. A return value of -1
 *         indicates an error.
 */
int UsartWrite(NUTFILE * fp, CONST void *buffer, int len)
{
    return UsartPut(fp->nf_dev, buffer, len, 0);
}

/*!
 * \brief Write a device or file.
 *
 * Similar to UsartWrite() except that the data is located in program
 * memory.
 *
 * This function is called by the low level output routines of the
 * \ref xrCrtLowio "C runtime library", using the _NUTDEVICE::dev_write_P
 * entry.
 *
 * The function may block the calling thread.
 *
 * \param fp     Pointer to a NUTFILE structure, obtained by a previous
 *               call to UsartOpen().
 * \param buffer Pointer to the data in program space to be written.
 * \param len    Number of bytes to write.
 *
 * \return The number of bytes written, which may be less than the number
 *         of bytes specified if a timeout occured. A return value of -1
 *         indicates an error.
 */
int UsartWrite_P(NUTFILE * fp, PGM_P buffer, int len)
{
    return UsartPut(fp->nf_dev, (CONST char *) buffer, len, 1);
}

/*!
 * \brief Close an USART device.
 *
 * This function is called by the low level close routine of the C runtime
 * library, using the _NUTDEVICE::dev_close entry.
 *
 * \param fp Pointer to a _NUTFILE structure, obtained by a previous call
 *           to UsartOpen().
 *
 * \return 0 on success or -1 otherwise.
 *
 * \todo We may support shared open and use dev_irq as an open counter.
 */
int UsartClose(NUTFILE * fp)
{
    NUTDEVICE *dev = fp->nf_dev;
    USARTDCB *dcb = dev->dev_dcb;

    if (fp == 0 || fp == NUTFILE_EOF)
        return -1;

    (*dcb->dcb_set_status) (UART_RTSDISABLED);
    free(fp);
    UsartResetBuffer(&dcb->dcb_tx_rbf, 0, 0, 0);
    UsartResetBuffer(&dcb->dcb_rx_rbf, 0, 0, 0);
    /* Wake-up all threads waiting for incoming data. */
    NutEventBroadcast(&dcb->dcb_rx_rbf.rbf_que);

    return 0;
}

/*!
 * \brief Open an USART device.
 *
 * This function is called by the low level open routine of the C runtime
 * library, using the _NUTDEVICE::dev_open entry.
 *
 * \param dev Pointer to the NUTDEVICE structure.
 * \param name Ignored, should point to an empty string.
 * \param mode Operation mode. Any of the following values may be or-ed:
 * - \ref _O_BINARY
 * - \ref _O_RDONLY
 * - \ref _O_WRONLY
 * \param acc Ignored, should be zero.
 *
 * \return Pointer to a NUTFILE structure if successful or NUTFILE_EOF otherwise.
 *
 * \todo We may support shared open and use dev_irq as an open counter.
 */
NUTFILE *UsartOpen(NUTDEVICE * dev, CONST char *name, int mode, int acc)
{
    USARTDCB *dcb = dev->dev_dcb;
    NUTFILE *fp;

    /*
     * Create the tranmit buffer unless this is used for read only.
     */
    if ((mode & 0x0003) != _O_RDONLY) {
        if (UsartResetBuffer(&dcb->dcb_tx_rbf, USART_TXBUFSIZ, USART_TXLOWMARK, USART_TXHIWMARK)) {
            return NUTFILE_EOF;
        }
    }

    /*
     * Create the receive buffer unless this is used for write only.
     */
    if ((mode & 0x0003) != _O_WRONLY) {
        if (UsartResetBuffer(&dcb->dcb_rx_rbf, USART_RXBUFSIZ, USART_RXLOWMARK, USART_RXHIWMARK)) {
            free(dcb->dcb_tx_rbf.rbf_start);
            return NUTFILE_EOF;
        }
    }

    /*
     * Allocate memory for the file structure.
     */
    if ((fp = malloc(sizeof(NUTFILE))) == 0) {
        free(dcb->dcb_tx_rbf.rbf_start);
        free(dcb->dcb_rx_rbf.rbf_start);
        return NUTFILE_EOF;
    }

    /* Set proper device modes. */
    if ((mode & 0xC000) == _O_BINARY) {
        dcb->dcb_modeflags &= ~USART_MF_COOKEDMODE;
    } else {
        dcb->dcb_modeflags |= USART_MF_COOKEDMODE;
    }

    /*
     * For now we do the initialization here. Later we may implement
     * a file creation routine to get a linked list of all opened
     * files in the system.
     */
    fp->nf_next = 0;
    fp->nf_dev = dev;
    fp->nf_fcb = 0;

    if ((mode & 0x0003) != _O_WRONLY) {
        (*dcb->dcb_rx_start) ();
    }

    return fp;
}

/*!
 * \brief Perform USART control functions.
 *
 * This function is called by the ioctl() function of the C runtime
 * library.
 *
 * \param dev  Identifies the device that receives the device-control
 *             function.
 * \param req  Requested control function. May be set to one of the
 *             following constants:
 *             - \ref UART_SETSPEED
 *             - \ref UART_GETSPEED
 *             - \ref UART_SETDATABITS
 *             - \ref UART_GETDATABITS
 *             - \ref UART_SETPARITY
 *             - \ref UART_GETPARITY
 *             - \ref UART_SETSTOPBITS
 *             - \ref UART_GETSTOPBITS
 *             - \ref UART_SETSTATUS
 *             - \ref UART_GETSTATUS
 *             - \ref UART_SETREADTIMEOUT
 *             - \ref UART_GETREADTIMEOUT
 *             - \ref UART_SETWRITETIMEOUT
 *             - \ref UART_GETWRITETIMEOUT
 *             - \ref UART_SETLOCALECHO
 *             - \ref UART_GETLOCALECHO
 *             - \ref UART_SETFLOWCONTROL
 *             - \ref UART_GETFLOWCONTROL
 *             - \ref UART_SETCOOKEDMODE
 *             - \ref UART_GETCOOKEDMODE
 *             - \ref UART_SETHDPXMODE
 *             - \ref UART_GETHDPXMODE
 *             - \ref UART_SETCLOCKMODE
 *             - \ref UART_GETCLOCKMODE
 *             - \ref UART_SETTXBUFSIZ
 *             - \ref UART_GETTXBUFSIZ
 *             - \ref UART_SETRXBUFSIZ
 *             - \ref UART_GETRXBUFSIZ
 *             - \ref UART_SETTXBUFLWMARK
 *             - \ref UART_GETTXBUFLWMARK
 *             - \ref UART_SETTXBUFHWMARK
 *             - \ref UART_GETTXBUFHWMARK
 *             - \ref UART_SETRXBUFLWMARK
 *             - \ref UART_GETRXBUFLWMARK
 *             - \ref UART_SETRXBUFHWMARK
 *             - \ref UART_GETRXBUFHWMARK
 *
 * \param conf Points to a buffer that contains any data required for
 *             the given control function or receives data from that
 *             function.
 * \return 0 on success, -1 otherwise.
 *
 * \note Not all control functions may be supported on all platforms.
 *       In any case applications should check the returned result.
 *
 * \todo Hardware handshake is not available with AT91 targets.
 *
 * \warning Timeout values are given in milliseconds and are limited to
 *          the granularity of the system timer. To disable timeout,
 *          set the parameter to NUT_WAIT_INFINITE.
 */
int UsartIOCtl(NUTDEVICE * dev, int req, void *conf)
{
    int rc = 0;
    USARTDCB *dcb;
    RINGBUF *rbf;
    uint32_t *lvp = (uint32_t *) conf;
    uint32_t lv = *lvp;
    uint8_t bv = (uint8_t) lv;

    dcb = dev->dev_dcb;

    switch (req) {
    case UART_SETSPEED:
        rc = (*dcb->dcb_set_speed) (lv);
        break;
    case UART_GETSPEED:
        *lvp = (*dcb->dcb_get_speed) ();
        break;

    case UART_SETDATABITS:
        rc = (*dcb->dcb_set_data_bits) (bv);
        break;
    case UART_GETDATABITS:
        *lvp = (*dcb->dcb_get_data_bits) ();
        break;

    case UART_SETPARITY:
        rc = (*dcb->dcb_set_parity) (bv);
        break;
    case UART_GETPARITY:
        *lvp = (*dcb->dcb_get_parity) ();
        break;

    case UART_SETSTOPBITS:
        rc = (*dcb->dcb_set_stop_bits) (bv);
        break;
    case UART_GETSTOPBITS:
        *lvp = (*dcb->dcb_get_stop_bits) ();
        break;

    case UART_SETSTATUS:
        /*
         * We are not changing the buffer size. Thus, we can safely ignore the
         * result of UsartResetBuffer(). This way we found a work around for
         * the AVRGCC 4.1.1 bug, which appeared here previously.
         */
        if (lv & UART_RXBUFFEREMPTY) {
            rbf = &dcb->dcb_rx_rbf;
            UsartResetBuffer(rbf, rbf->rbf_siz, rbf->rbf_lwm, rbf->rbf_hwm);
            (*dcb->dcb_rx_start) ();
        }
        if (lv & UART_TXBUFFEREMPTY) {
            rbf = &dcb->dcb_tx_rbf;
            UsartResetBuffer(rbf, rbf->rbf_siz, rbf->rbf_lwm, rbf->rbf_hwm);
        }
        rc = (*dcb->dcb_set_status) (lv & ~(UART_RXBUFFEREMPTY | UART_TXBUFFEREMPTY));
        break;
    case UART_GETSTATUS:
        *lvp = (*dcb->dcb_get_status) ();
        /*
         * Determine buffer status.
         */
        if (dcb->dcb_rx_rbf.rbf_cnt == 0) {
            *lvp |= UART_RXBUFFEREMPTY;
        }
        if (dcb->dcb_tx_rbf.rbf_cnt == 0) {
            *lvp |= UART_TXBUFFEREMPTY;
        }
        break;

    case UART_SETREADTIMEOUT:
        dcb->dcb_rtimeout = lv;
        break;
    case UART_GETREADTIMEOUT:
        *lvp = dcb->dcb_rtimeout;
        break;

    case UART_SETWRITETIMEOUT:
        dcb->dcb_wtimeout = lv;
        break;
    case UART_GETWRITETIMEOUT:
        *lvp = dcb->dcb_wtimeout;
        break;

    case UART_SETLOCALECHO:
        if (bv)
            dcb->dcb_modeflags |= USART_MF_LOCALECHO;
        else
            dcb->dcb_modeflags &= ~USART_MF_LOCALECHO;
        break;
    case UART_GETLOCALECHO:
        if (dcb->dcb_modeflags & USART_MF_LOCALECHO)
            *lvp = 1;
        else
            *lvp = 0;
        break;

    case UART_SETFLOWCONTROL:
        rc = (*dcb->dcb_set_flow_control) (lv);
        break;
    case UART_GETFLOWCONTROL:
        *lvp = (*dcb->dcb_get_flow_control) ();
        break;

    case UART_SETCOOKEDMODE:
        if (bv)
            dcb->dcb_modeflags |= USART_MF_COOKEDMODE;
        else
            dcb->dcb_modeflags &= ~USART_MF_COOKEDMODE;
        break;
    case UART_GETCOOKEDMODE:
        if (dcb->dcb_modeflags & USART_MF_COOKEDMODE)
            *lvp = 1;
        else
            *lvp = 0;
        break;

	case UART_SETHDPXMODE:
		if (bv)
			dcb->dcb_modeflags |= USART_MF_HALFDUPLEX;
		else
			dcb->dcb_modeflags &= ~USART_MF_HALFDUPLEX;
		break;
	case UART_GETHDPXMODE:
		if (dcb->dcb_modeflags & USART_MF_HALFDUPLEX)
			*lvp = 1;
		else
			*lvp = 0;
		break;

    case UART_SETCLOCKMODE:
        rc = (*dcb->dcb_set_clock_mode) (lv);
        break;
    case UART_GETCLOCKMODE:
        *lvp = (*dcb->dcb_get_clock_mode) ();
        break;

    case UART_SETTXBUFSIZ:
        rbf = &dcb->dcb_tx_rbf;
        rc = UsartResetBuffer(rbf, (size_t) lv, rbf->rbf_lwm, rbf->rbf_hwm);
        if (rc == 0) {
            (*dcb->dcb_rx_start) ();
        }
        break;
    case UART_GETTXBUFSIZ:
        *lvp = dcb->dcb_tx_rbf.rbf_siz;
        break;

    case UART_SETRXBUFSIZ:
        rbf = &dcb->dcb_rx_rbf;
        rc = UsartResetBuffer(rbf, (size_t) lv, rbf->rbf_lwm, rbf->rbf_hwm);
        break;
    case UART_GETRXBUFSIZ:
        *lvp = dcb->dcb_rx_rbf.rbf_siz;
        break;

    case UART_SETTXBUFLWMARK:
        NutEnterCritical();
        dcb->dcb_tx_rbf.rbf_lwm = (size_t) lv;
        NutExitCritical();
        break;
    case UART_GETTXBUFLWMARK:
        *lvp = dcb->dcb_tx_rbf.rbf_lwm;
        break;

    case UART_SETTXBUFHWMARK:
        NutEnterCritical();
        dcb->dcb_tx_rbf.rbf_hwm = (size_t) lv;
        NutExitCritical();
        break;
    case UART_GETTXBUFHWMARK:
        *lvp = dcb->dcb_tx_rbf.rbf_hwm;
        break;

    case UART_SETRXBUFLWMARK:
        NutEnterCritical();
        dcb->dcb_rx_rbf.rbf_lwm = (size_t) lv;
        NutExitCritical();
        break;
    case UART_GETRXBUFLWMARK:
        *lvp = dcb->dcb_rx_rbf.rbf_lwm;
        break;

    case UART_SETRXBUFHWMARK:
        NutEnterCritical();
        dcb->dcb_rx_rbf.rbf_hwm = (size_t) lv;
        NutExitCritical();
        break;
    case UART_GETRXBUFHWMARK:
        *lvp = dcb->dcb_rx_rbf.rbf_hwm;
        break;

    default:
        rc = -1;
        break;
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
long UsartSize (NUTFILE *fp)
{
    long avail;
    NUTDEVICE *dev = fp->nf_dev;
    USARTDCB *dcb = dev->dev_dcb;
    RINGBUF *rbf = &dcb->dcb_rx_rbf;

    /* Atomic access to the ring buffer counter. */
    NutEnterCritical();
    avail = rbf->rbf_cnt;
    NutExitCritical();

    return avail;
}


/*@}*/
