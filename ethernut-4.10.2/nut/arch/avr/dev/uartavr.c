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

/*
 * $Log$
 * Revision 1.5  2008/08/11 06:59:17  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.4  2006/10/08 16:48:08  haraldkipp
 * Documentation fixed
 *
 * Revision 1.3  2005/10/16 23:21:33  hwmaier
 * Fixed compilation issue regards U2X macro with Imagecraft
 *
 * Revision 1.2  2005/10/07 22:01:27  hwmaier
 * Obsolete dcb_baudSelect removed. Support for double speed (U2X) added (using same method as in usartavr.c).
 *
 * Revision 1.1  2005/07/26 18:02:40  haraldkipp
 * Moved from dev.
 *
 * Revision 1.9  2005/07/14 09:12:20  freckle
 * Rewrote CS in UartAvrInput
 *
 * Revision 1.8  2005/01/24 21:12:05  freckle
 * renamed NutEventPostFromIRQ into NutEventPostFromIrq
 *
 * Revision 1.7  2005/01/21 16:49:46  freckle
 * Seperated calls to NutEventPostAsync between Threads and IRQs
 *
 * Revision 1.6  2004/12/16 08:40:35  haraldkipp
 * Late increment fixes ICCAVR bug.
 *
 * Revision 1.5  2004/05/24 20:15:50  drsung
 * Added function UartAvrSize to return number of chars in input buffer.
 *
 * Revision 1.4  2004/03/18 14:01:07  haraldkipp
 * Deprecated header file replaced
 *
 * Revision 1.3  2004/03/18 10:09:27  haraldkipp
 * Removed deprecated raw I/O
 *
 * Revision 1.2  2003/07/20 18:28:10  haraldkipp
 * Explain how to disable timeout.
 *
 * Revision 1.1.1.1  2003/05/09 14:40:55  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.21  2003/05/06 18:35:06  harald
 * Avoid duplicate initialization
 *
 * Revision 1.20  2003/04/21 16:26:01  harald
 * Use predefined eof
 *
 * Revision 1.19  2003/03/31 14:53:08  harald
 * Prepare release 3.1
 *
 */

#include <string.h>

#include <sys/atom.h>
#include <sys/heap.h>
#include <sys/event.h>
#include <sys/timer.h>
#include <sys/device.h>

#include <dev/irqreg.h>
#include <dev/uartavr.h>

#include <fcntl.h>

/*
 * Not nice because stdio already defined them. But in order to save memory,
 * we do the whole buffering and including stdio here would be more weird.
 */
#ifndef _IOFBF
#define _IOFBF	    0x00
#define _IOLBF	    0x01
#define _IONBF	    0x02
#endif

/*!
 * \addtogroup xgUartAvr
 */
/*@{*/

/*
 * Handle AVR UART transmit complete.
 */
static void TxComplete(void *arg)
{
    NUTDEVICE *dev = (NUTDEVICE *) arg;
    IFSTREAM *ifs = dev->dev_icb;
    UARTDCB *dcb;

    if (ifs->if_tx_idx != ifs->if_wr_idx) {
#ifdef UDR1
        if (dev->dev_base)
            UDR1 = ifs->if_tx_buf[ifs->if_tx_idx];
        else
#endif
            outb(UDR, ifs->if_tx_buf[ifs->if_tx_idx]);
        ifs->if_tx_idx++;
    } else {
        ifs->if_tx_act = 0;
        dcb = dev->dev_dcb;
        NutEventPostFromIrq(&dcb->dcb_tx_rdy);
    }
}

/*
 * Handle AVR UART receive complete.
 *
 * Note, that this function modifies the receive index in
 * interrupt context. This requires, that any non atomic
 * access of this index requires interrupts being disabled.
 * Thanks to Mike Cornelius, who pointed this out.
 */
static void RxComplete(void *arg)
{
    NUTDEVICE *dev = (NUTDEVICE *) arg;
    IFSTREAM *ifs = dev->dev_icb;
    UARTDCB *dcb;

#ifdef UDR1
    if (dev->dev_base)
        ifs->if_rx_buf[ifs->if_rx_idx] = inb(UDR1);
    else
#endif
        ifs->if_rx_buf[ifs->if_rx_idx] = inb(UDR);

    if (ifs->if_rd_idx == ifs->if_rx_idx) {
        dcb = dev->dev_dcb;
        NutEventPostFromIrq(&dcb->dcb_rx_rdy);
    }
    /* Late increment fixes ICCAVR bug on volatile variables. */
    ifs->if_rx_idx++;
}

/*!
 * \brief Wait for input.
 *
 * This function checks the input buffer for any data. If
 * the buffer is empty, the calling \ref xrThread "thread"
 * will be blocked until at least one new character is
 * received or a timeout occurs.
 *
 * \param dev Indicates the UART device.
 *
 * \return 0 on success, -1 on timeout.
 */
int UartAvrInput(NUTDEVICE * dev)
{
    int rc = 0;
    int empty;
    IFSTREAM *ifs = dev->dev_icb;
    UARTDCB *dcb =  dev->dev_dcb;

    NutEnterCritical();
    empty = ifs->if_rd_idx == ifs->if_rx_idx;
    NutExitCritical();

    if (empty) {

        /*
         * Changing if into a while loop fixes a serious bug:
         * Previous receiver events without any waiting thread
         * set the event handle to the signaled state. So the
         * wait returns immediately. Unfortunately the calling
         * routines rely on a filled buffer when we return 0.
         * Thanks to Mike Cornelius, who found this bug.
         *
         * MR (2005-07-14): Reduced critical sections
         */
        do {

            rc = NutEventWait(&dcb->dcb_rx_rdy, dcb->dcb_rtimeout);

            NutEnterCritical();
            empty = ifs->if_rd_idx == ifs->if_rx_idx;
            NutExitCritical();

        } while (rc == 0 && empty);
    }

    return rc;
}

/*!
 * \brief Initiate output.
 *
 * This function checks the output buffer for any data. If
 * the buffer contains at least one character, the transmitter
 * is started, if not already running. The function returns
 * immediately, without waiting for the character being
 * completely transmitted. Any remaining characters in the
 * output buffer are transmitted in the background.
 *
 * \param dev Indicates the UART device.
 *
 * \return 0 on success, -1 otherwise.
 */
int UartAvrOutput(NUTDEVICE * dev)
{
    IFSTREAM *ifs = dev->dev_icb;

    if (ifs->if_tx_act == 0 && ifs->if_tx_idx != ifs->if_wr_idx) {
        ifs->if_tx_act = 1;
#ifdef UDR1
        if (dev->dev_base)
            UDR1 = ifs->if_tx_buf[ifs->if_tx_idx];
        else
#endif
            outb(UDR, ifs->if_tx_buf[ifs->if_tx_idx]);
        ifs->if_tx_idx++;
    }
    return 0;
}

/*!
 * \brief Wait for output buffer empty.
 *
 * If the output buffer contains any data, the calling
 * thread is suspended until all data has been transmitted.
 *
 * \param dev Indicates the UART device.
 *
 * \return 0 on success, -1 otherwise.
 */
int UartAvrFlush(NUTDEVICE * dev)
{
    IFSTREAM *ifs = dev->dev_icb;
    UARTDCB *dcb = dev->dev_dcb;

    /*
     * Start any pending output.
     */
    if (UartAvrOutput(dev))
        return -1;

    /*
     * Wait until output buffer empty.
     */
    NutEnterCritical();
    while (ifs->if_tx_idx != ifs->if_wr_idx)
        NutEventWait(&dcb->dcb_tx_rdy, 100);
    NutExitCritical();

    return 0;
}

/*
 *
 * \param dev Indicates the UART device.
 *
 * \return 0 on success, -1 otherwise.
 */
static int UartAvrGetStatus(NUTDEVICE * dev, uint32_t * status)
{
    IFSTREAM *ifs = dev->dev_icb;
    uint8_t us;

    *status = 0;

#ifdef UDR1
    if (dev->dev_base)
        us = inb(UCSR1A);
    else
#endif
        us = inb(USR);
    if (us & FE)
        *status |= UART_FRAMINGERROR;
    if (us & DOR)
        *status |= UART_OVERRUNERROR;
    if (ifs->if_tx_idx == ifs->if_wr_idx)
        *status |= UART_TXBUFFEREMPTY;
    if (ifs->if_rd_idx == ifs->if_rx_idx)
        *status |= UART_RXBUFFEREMPTY;
    return 0;
}

/*
 * Carefully enable UART functions.
 */
static void UartAvrEnable(uint16_t base)
{
    NutEnterCritical();
#ifdef UCSR1B
    if (base)
        UCSR1B = BV(RXCIE) | BV(TXCIE) | BV(RXEN) | BV(TXEN);
    else
#endif
        UCR = BV(RXCIE) | BV(TXCIE) | BV(RXEN) | BV(TXEN);
    NutExitCritical();
}

/*
 * Carefully disable UART functions.
 */
static void UartAvrDisable(uint16_t base)
{
    /*
     * Disable UART interrupts.
     */
    NutEnterCritical();
#ifdef UCSR1B
    if (base)
        UCSR1B &= ~(BV(RXCIE) | BV(TXCIE));
    else
#endif
        UCR &= ~(BV(RXCIE) | BV(TXCIE));
    NutExitCritical();

    /*
     * Allow incoming or outgoing character to finish.
     */
    NutDelay(10);

    /*
     * Now disable UART functions.
     */
#ifdef UCSR1B
    if (base)
        UCSR1B &= ~(BV(RXEN) | BV(TXEN));
    else
#endif
        UCR &= ~(BV(RXEN) | BV(TXEN));
}

/*!
 * \brief Perform on-chip UART control functions.
 *
 * \param dev  Identifies the device that receives the device-control
 *             function.
 * \param req  Requested control function. May be set to one of the
 *             following constants:
 *             - UART_SETSPEED, conf points to an uint32_t value containing the baudrate.
 *             - UART_GETSPEED, conf points to an uint32_t value receiving the current baudrate.
 *             - UART_SETDATABITS, conf points to an uint32_t value containing the number of data bits, 5, 6, 7 or 8.
 *             - UART_GETDATABITS, conf points to an uint32_t value receiving the number of data bits, 5, 6, 7 or 8.
 *             - UART_SETPARITY, conf points to an uint32_t value containing the parity, 0 (no), 1 (odd) or 2 (even).
 *             - UART_GETPARITY, conf points to an uint32_t value receiving the parity, 0 (no), 1 (odd) or 2 (even).
 *             - UART_SETSTOPBITS, conf points to an uint32_t value containing the number of stop bits 1 or 2.
 *             - UART_GETSTOPBITS, conf points to an uint32_t value receiving the number of stop bits 1 or 2.
 *             - UART_SETSTATUS
 *             - UART_GETSTATUS
 *             - UART_SETREADTIMEOUT, conf points to an uint32_t value containing the read timeout.
 *             - UART_GETREADTIMEOUT, conf points to an uint32_t value receiving the read timeout.
 *             - UART_SETWRITETIMEOUT, conf points to an uint32_t value containing the write timeout.
 *             - UART_GETWRITETIMEOUT, conf points to an uint32_t value receiving the write timeout.
 *             - UART_SETLOCALECHO, conf points to an uint32_t value containing 0 (off) or 1 (on).
 *             - UART_GETLOCALECHO, conf points to an uint32_t value receiving 0 (off) or 1 (on).
 *             - UART_SETFLOWCONTROL, conf points to an uint32_t value containing combined UART_FCTL_ values.
 *             - UART_GETFLOWCONTROL, conf points to an uint32_t value containing receiving UART_FCTL_ values.
 *             - UART_SETCOOKEDMODE, conf points to an uint32_t value containing 0 (off) or 1 (on).
 *             - UART_GETCOOKEDMODE, conf points to an uint32_t value receiving 0 (off) or 1 (on).
 *
 * \param conf Points to a buffer that contains any data required for
 *             the given control function or receives data from that
 *             function.
 * \return 0 on success, -1 otherwise.
 *
 * \warning Timeout values are given in milliseconds and are limited to
 *          the granularity of the system timer. To disable timeout,
 *          set the parameter to NUT_WAIT_INFINITE.
 *
 * \note For ATmega103, only 8 data bits, 1 stop bit and no parity are allowed.
 *
 */
int UartAvrIOCtl(NUTDEVICE * dev, int req, void *conf)
{
    int rc = 0;
    UARTDCB *dcb;
    uint32_t *lvp = (uint32_t *) conf;
    uint32_t lv = *lvp;
    uint8_t bv = (uint8_t) lv;
    uint16_t sv;
    uint8_t devnum;

    if (dev == 0)
        dev = &devUart0;

    devnum = dev->dev_base;
    dcb = dev->dev_dcb;

    switch (req) {
    case UART_SETSPEED:
        UartAvrDisable(devnum);
#ifdef __AVR_ENHANCED__
        if (devnum) {
            if (bit_is_set(UCSR1A, U2X1)) {
                lv <<= 2;
            } else {
                lv <<= 3;
            }
        } else {
            if (bit_is_set(UCSR0A, U2X0)) {
                lv <<= 2;
            } else {
                lv <<= 3;
            }
        }
#else
        lv <<= 3;
#endif
        sv = (uint16_t) ((NutGetCpuClock() / lv + 1UL) / 2UL) - 1;
#ifdef UBRR1H
        if (devnum) {
            UBRR1L = (uint8_t) sv;
            UBRR1H = (uint8_t) (sv >> 8);
        } else {
            UBRR0L = (uint8_t) sv;
            UBRR0H = (uint8_t) (sv >> 8);
        }
#else
        UBRR = (uint8_t) sv;
#endif
        UartAvrEnable(devnum);
        break;

    case UART_GETSPEED:
#ifdef UBRR1H
        if (devnum) {
            if (bit_is_set(UCSR1A, U2X1))
                lv = 8UL;
            else
                lv = 16UL;
            sv = (uint16_t) (UBRR1H) << 8 | UBRR1L;
        }
        else
        {
            if (bit_is_set(UCSR0A, U2X0))
                lv = 8UL;
            else
                lv = 16UL;
            sv = (uint16_t) (UBRR0H) << 8 | UBRR0L;
        }
#else
        sv = UBRR;
        lv = 16UL;
#endif
        *lvp = NutGetCpuClock() / (lv * (uint32_t) (sv + 1));
        break;

    case UART_SETDATABITS:
        UartAvrDisable(devnum);
#ifdef UCSR1C
        if (bv >= 5 && bv <= 8) {
            bv = (bv - 5) << 1;
            if (devnum) {
                UCSR1C = (UCSR1C & 0xF9) | bv;
                UCSR1B &= 0xFB;
            } else {
                UCSR0C = (UCSR0C & 0xF9) | bv;
                UCSR0B &= 0xFB;
            }
        } else
            rc = -1;
#else
        if (bv != 8)
            rc = -1;
#endif
        UartAvrEnable(devnum);
        break;

    case UART_GETDATABITS:
#ifdef UCSR1C
        if (devnum)
            *lvp = ((UCSR1C & 0x06) >> 1) + 5;
        else
            *lvp = ((UCSR0C & 0x06) >> 1) + 5;
#else
        *lvp = 8;
#endif
        break;

    case UART_SETPARITY:
        UartAvrDisable(devnum);
#ifdef UCSR1C
        if (bv <= 2) {
            if (bv == 1)
                bv = 3;
            bv <<= 4;
            if (devnum)
                UCSR1C = (UCSR1C & 0xCF) | bv;
            else
                UCSR0C = (UCSR0C & 0xCF) | bv;
        } else
            rc = -1;
#endif
        if (bv)
            rc = -1;
        UartAvrEnable(devnum);
        break;

    case UART_GETPARITY:
#ifdef UCSR1C
        if (devnum)
            bv = (UCSR1C & 0x30) >> 4;
        else
            bv = (UCSR0C & 0x30) >> 4;
        if (bv == 3)
            bv = 1;
#else
        bv = 0;
#endif
        break;

    case UART_SETSTOPBITS:
        UartAvrDisable(devnum);
#ifdef UCSR1C
        if (bv == 1 || bv == 2) {
            bv = (bv - 1) << 3;
            if (devnum)
                UCSR1C = (UCSR1C & 0xF7) | bv;
            else
                UCSR0C = (UCSR0C & 0xF7) | bv;
        } else
            rc = -1;
#else
        if (bv != 1)
            rc = -1;
#endif
        UartAvrEnable(devnum);
        break;

    case UART_GETSTOPBITS:
#ifdef UCSR1C
        if (devnum)
            *lvp = ((UCSR1C & 0x08) >> 3) + 1;
        else
            *lvp = ((UCSR0C & 0x08) >> 3) + 1;
#else
        *lvp = 1;
#endif
        break;

    case UART_GETSTATUS:
        UartAvrGetStatus(dev, lvp);
        break;
    case UART_SETSTATUS:
        rc = -1;
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
            dcb->dcb_modeflags |= UART_MF_LOCALECHO;
        else
            dcb->dcb_modeflags &= ~UART_MF_LOCALECHO;
        break;
    case UART_GETLOCALECHO:
        if (dcb->dcb_modeflags & UART_MF_LOCALECHO)
            *lvp = 1;
        else
            *lvp = 0;
        break;

    case UART_SETFLOWCONTROL:
        if (bv)
            dcb->dcb_modeflags |= UART_MF_LOCALECHO;
        else
            dcb->dcb_modeflags &= ~UART_MF_LOCALECHO;
        break;
    case UART_GETFLOWCONTROL:
        break;

    case UART_SETCOOKEDMODE:
        if (bv)
            dcb->dcb_modeflags |= UART_MF_COOKEDMODE;
        else
            dcb->dcb_modeflags &= ~UART_MF_COOKEDMODE;
        break;
    case UART_GETCOOKEDMODE:
        if (dcb->dcb_modeflags & UART_MF_COOKEDMODE)
            *lvp = 1;
        else
            *lvp = 0;
        break;

    default:
        rc = -1;
        break;
    }
    return rc;
}

/*!
 * \brief Initialize on chip uart device.
 *
 * Prepares the device for subsequent reading or writing.
 * Enables UART transmitter and receiver interrupts.
 *
 * \param dev  Identifies the device to initialize.
 *
 * \return 0 on success, -1 otherwise.
 */
int UartAvrInit(NUTDEVICE * dev)
{
    IFSTREAM *ifs;
    UARTDCB *dcb;
    uint32_t baudrate = 9600;

    /*
     * We only support character devices for on-chip UARTs.
     */
    if (dev->dev_type != IFTYP_STREAM || dev->dev_irq != 0 ||
        (dev->dev_base != 0 && dev->dev_base != 1))
        return -1;

    /*
     * Initialize interface control block.
     */
    ifs = dev->dev_icb;
    memset(ifs, 0, sizeof(IFSTREAM));
    ifs->if_input = UartAvrInput;
    ifs->if_output = UartAvrOutput;
    ifs->if_flush = UartAvrFlush;

    /*
     * Initialize driver control block.
     */
    dcb = dev->dev_dcb;
    memset(dcb, 0, sizeof(UARTDCB));
    dcb->dcb_modeflags = UART_MF_NOBUFFER;

    /*
     * Register interrupt handler.
     */
    if (dev->dev_base) {
#ifdef UDR1
        if (NutRegisterIrqHandler(&sig_UART1_RECV, RxComplete, dev))
            return -1;
        if (NutRegisterIrqHandler(&sig_UART1_TRANS, TxComplete, dev))
#endif
            return -1;
    } else {
        if (NutRegisterIrqHandler(&sig_UART0_RECV, RxComplete, dev))
            return -1;
        if (NutRegisterIrqHandler(&sig_UART0_TRANS, TxComplete, dev))
            return -1;
    }

    /*
     * Set baudrate and handshake default. This will also
     * enable the UART functions.
     */
    UartAvrIOCtl(dev, UART_SETSPEED, &baudrate);

    return 0;
}

/*!
 * \brief Read from device.
 */
int UartAvrRead(NUTFILE * fp, void *buffer, int size)
{
    int rc;
    NUTDEVICE *dev;
    IFSTREAM *ifs;
    UARTDCB *dcb;
    uint8_t elmode;
    uint8_t ch;
    uint8_t *cp = buffer;

    dev = fp->nf_dev;
    ifs = (IFSTREAM *) dev->dev_icb;
    dcb = dev->dev_dcb;

    if (dcb->dcb_modeflags & UART_MF_COOKEDMODE)
        elmode = 1;
    else
        elmode = 0;

    /*
     * Call without data pointer discards receive buffer.
     */
    if (buffer == 0) {
        ifs->if_rd_idx = ifs->if_rx_idx;
        return 0;
    }

    /*
     * Get characters from receive buffer.
     */
    for (rc = 0; rc < size;) {
        if (ifs->if_rd_idx == ifs->if_rx_idx) {
            if (rc)
                break;
            while (ifs->if_rd_idx == ifs->if_rx_idx) {
                if (UartAvrInput(dev)) {
                    return 0;
		}
	    }
        }
        ch = ifs->if_rx_buf[ifs->if_rd_idx++];
        if (elmode && (ch == '\r' || ch == '\n')) {
            if (ifs->if_last_eol == 0 || ifs->if_last_eol == ch) {
                ifs->if_last_eol = ch;
                *cp++ = '\n';
                rc++;
            }
        } else {
            ifs->if_last_eol = 0;
            *cp++ = ch;
            rc++;
        }
    }
    return rc;
}

/*!
 * \brief Write to device.
 */
int UartAvrPut(NUTDEVICE * dev, CONST void *buffer, int len, int pflg)
{
    int rc;
    IFSTREAM *ifs;
    UARTDCB *dcb;
    CONST uint8_t *cp;
    uint8_t lbmode;
    uint8_t elmode;
    uint8_t ch;

    ifs = dev->dev_icb;
    dcb = dev->dev_dcb;

    if (dcb->dcb_modeflags & UART_MF_LINEBUFFER)
        lbmode = 1;
    else
        lbmode = 0;

    if (dcb->dcb_modeflags & UART_MF_COOKEDMODE)
        elmode = 1;
    else
        elmode = 0;

    /*
     * Call without data pointer starts transmission.
     */
    if (buffer == 0) {
        rc = UartAvrFlush(dev);
        return rc;
    }

    /*
     * Put characters in transmit buffer.
     */
    cp = buffer;
    for (rc = 0; rc < len;) {
        if ((uint8_t) (ifs->if_wr_idx + 1) == ifs->if_tx_idx) {
            if (UartAvrFlush(dev)) {
                return -1;
            }
        }
        ch = pflg ? PRG_RDB(cp) : *cp;
        if (elmode == 1 && ch == '\n') {
            elmode = 2;
            if (lbmode == 1)
                lbmode = 2;
            ch = '\r';
        } else {
            if (elmode == 2)
                elmode = 1;
            cp++;
            rc++;
        }
        ifs->if_tx_buf[ifs->if_wr_idx++] = ch;
    }

    if (lbmode > 1 || (dcb->dcb_modeflags & UART_MF_NOBUFFER) != 0) {
        if (UartAvrFlush(dev))
            rc = -1;
    }
    return rc;
}

int UartAvrWrite(NUTFILE * fp, CONST void *buffer, int len)
{
    return UartAvrPut(fp->nf_dev, buffer, len, 0);
}

int UartAvrWrite_P(NUTFILE * fp, PGM_P buffer, int len)
{
    return UartAvrPut(fp->nf_dev, (CONST char *) buffer, len, 1);
}


/*!
 * \brief Open a device or file.
 */
NUTFILE *UartAvrOpen(NUTDEVICE * dev, CONST char *name, int mode, int acc)
{
    NUTFILE *fp = NutHeapAlloc(sizeof(NUTFILE));
    UARTDCB *dcb;

    if (fp == 0)
        return NUTFILE_EOF;

    dcb = dev->dev_dcb;
    if (mode & _O_BINARY)
        dcb->dcb_modeflags &= ~UART_MF_COOKEDMODE;
    else
        dcb->dcb_modeflags |= UART_MF_COOKEDMODE;

    fp->nf_next = 0;
    fp->nf_dev = dev;
    fp->nf_fcb = 0;

    return fp;
}

/*!
 * \brief Close a device or file.
 */
int UartAvrClose(NUTFILE * fp)
{
    NutHeapFree(fp);

    return 0;
}

/*!
 * \brief Request file size.
 */
long UartAvrSize(NUTFILE * fp)
{
    NUTDEVICE *dev;
    IFSTREAM *ifs;

    dev = fp->nf_dev;
    ifs = (IFSTREAM *) dev->dev_icb;
    return ((uint8_t)(ifs->if_rx_idx - ifs->if_rd_idx));
}


/*@}*/
