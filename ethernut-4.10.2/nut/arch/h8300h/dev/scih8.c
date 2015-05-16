/*
 * Copyright (C) 2004 by Jan Dubiec. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY JAN DUBIEC AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL JAN DUBIEC
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * $Log$
 * Revision 1.2  2005/10/19 09:34:25  hwmaier
 * Changed doxygen group to xgUartH8 so it doesn't
 * overload arch/avr/dev/usartavr.c's documentation
 *
 * Revision 1.1  2005/07/26 18:02:40  haraldkipp
 * Moved from dev.
 *
 * Revision 1.4  2005/01/24 21:12:02  freckle
 * renamed NutEventPostFromIRQ into NutEventPostFromIrq
 *
 * Revision 1.3  2005/01/21 16:49:46  freckle
 * Seperated calls to NutEventPostAsync between Threads and IRQs
 *
 * Revision 1.2  2004/03/18 14:06:52  haraldkipp
 * Deprecated header file replaced
 *
 * Revision 1.1  2004/03/16 16:48:27  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 */

#include <stdio.h>
#include <string.h>

#include <sys/atom.h>
#include <sys/heap.h>
#include <sys/event.h>
#include <sys/timer.h>
#include <sys/device.h>

#include <dev/irqreg.h>
#include <dev/scih8.h>

#include <fcntl.h>

#include <h83068f.h>

/* #include <sys/osdebug.h> */

/*
 * Not nice because stdio already defined them. But in order to save memory,
 * we do the whole buffering and including stdio here would be more weird.
 */
#ifndef _IOFBF
#define _IOFBF      0x00
#define _IOLBF      0x01
#define _IONBF      0x02
#endif

/*!
 * \addtogroup xgUartH8
 */
/*@{*/

/* Baud rate defintions for a 18.432Mhz clock source. */
#define BR_150          150UL
#define BR_300          300UL
#define BR_600          600UL
#define BR_1200         1200UL
#define BR_2400         2400UL
#define BR_4800         4800UL
#define BR_9600         9600UL
#define BR_19200        19200UL
#define BR_38400        38400UL
#define BR_57600        57600UL
#define BR_115200       115200UL
#define BR_230400       230400UL

/* SCR bit definitions. */
#define TXI_ENABLE      0x80
#define RXI_ENABLE      0x40
#define TX_ENABLE       0x20
#define RX_ENABLE       0x10
#define MPI_ENABLE      0x08
#define TXEI_ENABLE     0x04
#define RXEI_ENABLE     0x02
#define CK0_ENABLE      0x01

/* SSR bit definitions. */
#define TDRE            0x80
#define RDRF            0x40
#define ORER            0x20
#define FER             0x10
#define PER             0x08
#define TEND            0x04

/* SMR bit definitions. */
#define DATABITS_8      0
#define DATABITS_7      0x40
#define NOPARITY        0
#define EVENPARITY      0x20
#define ODDPARITY       0x30
#define STOP1BIT        0
#define STOP2BITS       0x08
#define NODIVIDE        0
#define DIVIDEBY4       1
#define DIVIDEBY16      2
#define DIVIDEBY64      3


/*
 * Returns H8/300H SCI structure pointer based on NUTDEVICE->dev_base
 * or NULL if there is no such device.
 */
#define GET_SCI(p) ( ((p) == 0) ? &SCI0 : ( ((p) == 1) ? &SCI1 : \
                                          ( ((p) == 2) ? &SCI2 : NULL ) ) )

/*
 * Handle H8/300H SCI transmit complete.
 */
static void TxComplete(void *arg)
{
    NUTDEVICE *dev = (NUTDEVICE *) arg;
    IFSTREAM *ifs = dev->dev_icb;
    UARTDCB *dcb;
    volatile struct st_sci *sci = GET_SCI(dev->dev_base);

    if (ifs->if_tx_idx != ifs->if_wr_idx) {
        sci->TDR = ifs->if_tx_buf[ifs->if_tx_idx];
        sci->SSR.BYTE &= ~TDRE;
        ifs->if_tx_idx++;
    } else {
        /* Disable SCI interrupts or they just keep coming */
        sci->SCR.BYTE &= ~TXI_ENABLE;

        ifs->if_tx_act = 0;
        dcb = dev->dev_dcb;
        NutEventPostFromIrq(&dcb->dcb_tx_rdy);
    }
}

/*
 * Handle H8/300H SCI receive complete.
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
    volatile struct st_sci *sci = GET_SCI(dev->dev_base);

    ifs->if_rx_buf[ifs->if_rx_idx] = sci->RDR;
    sci->SSR.BYTE &= ~RDRF;

    if (ifs->if_rd_idx == ifs->if_rx_idx++) {
        dcb = dev->dev_dcb;
        NutEventPostFromIrq(&dcb->dcb_rx_rdy);
    }
}

/*!
 * \brief Wait for input.
 *
 * This function checks the input buffer for any data. If
 * the buffer is empty, the calling \ref xrThread "thread"
 * will be blocked until at least one new character is
 * received or a timeout occurs.
 *
 * \param dev Indicates the SCI device.
 *
 * \return 0 on success, -1 on timeout.
 */
int SciH8Input(NUTDEVICE * dev)
{
    int rc = 0;
    IFSTREAM *ifs = dev->dev_icb;
    UARTDCB *dcb;

    NutEnterCritical();
    if (ifs->if_rd_idx == ifs->if_rx_idx) {
        dcb = dev->dev_dcb;
        /*
         * Changing if into a while loop fixes a serious bug:
         * Previous receiver events without any waiting thread
         * set the event handle to the signaled state. So the
         * wait returns immediately. Unfortunately the calling
         * routines rely on a filled buffer when we return 0.
         * Thanks to Mike Cornelius, who found this bug.
         */
        do {
            rc = NutEventWait(&dcb->dcb_rx_rdy, dcb->dcb_rtimeout);
        } while (rc == 0 && ifs->if_rd_idx == ifs->if_rx_idx);
    }
    NutExitCritical();

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
 * \param dev Indicates the SCI device.
 *
 * \return 0 on success, -1 otherwise.
 */
int SciH8Output(NUTDEVICE * dev)
{
    IFSTREAM *ifs = dev->dev_icb;
    volatile struct st_sci *sci = GET_SCI(dev->dev_base);

    NutEnterCritical();
    if (ifs->if_tx_act == 0 && ifs->if_tx_idx != ifs->if_wr_idx) {
        ifs->if_tx_act = 1;

        sci->TDR = ifs->if_tx_buf[ifs->if_tx_idx];
        sci->SCR.BYTE |= TXI_ENABLE;
        sci->SSR.BYTE &= ~TDRE;

        ifs->if_tx_idx++;
    }
    NutExitCritical();

    return 0;
}

/*!
 * \brief Wait for output buffer empty.
 *
 * If the output buffer contains any data, the calling
 * thread is suspended until all data has been transmitted.
 *
 * \param dev Indicates the SCI device.
 *
 * \return 0 on success, -1 otherwise.
 */
int SciH8Flush(NUTDEVICE * dev)
{
    IFSTREAM *ifs = dev->dev_icb;
    UARTDCB *dcb = dev->dev_dcb;

    /*
     * Start any pending output.
     */
    if (SciH8Output(dev))
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
 * \param dev Indicates the SCI device.
 *
 * \return 0 on success, -1 otherwise.
 */
static int SciH8GetStatus(NUTDEVICE * dev, u_long * status)
{
    IFSTREAM *ifs = dev->dev_icb;
    u_char us;
    volatile struct st_sci *sci = GET_SCI(dev->dev_base);

    *status = 0;

    us = sci->SSR.BYTE;

    if (us & FER)
        *status |= UART_FRAMINGERROR;
    if (us & ORER)
        *status |= UART_OVERRUNERROR;
    if (ifs->if_tx_idx == ifs->if_wr_idx)
        *status |= UART_TXBUFFEREMPTY;
    if (ifs->if_rd_idx == ifs->if_rx_idx)
        *status |= UART_RXBUFFEREMPTY;
    return 0;
}

/*
 * Carefully enable SCI functions.
 */
static void SciH8Enable(u_short base)
{
    volatile struct st_sci *sci = GET_SCI(base);

    NutEnterCritical();

    //sci->SCR.BYTE |= (TXI_ENABLE | /*RXI_ENABLE |*/ RX_ENABLE | TX_ENABLE);
    sci->SCR.BYTE |= (TXI_ENABLE | RXI_ENABLE | RX_ENABLE | TX_ENABLE);

    NutExitCritical();
}

/*
 * Carefully disable SCI functions.
 */
static void SciH8Disable(u_short base)
{
    volatile struct st_sci *sci = GET_SCI(base);

    /*
     * Disable SCI interrupts.
     */
    NutEnterCritical();
    sci->SCR.BYTE &= ~(TXI_ENABLE | RXI_ENABLE);
    NutExitCritical();

    /*
     * Allow incoming or outgoing character to finish.
     */
    NutDelay(10);

    /*
     * Now disable SCI functions.
     */
    sci->SCR.BYTE &= ~(RX_ENABLE | TX_ENABLE);
}

/*!
 * \brief Perform on-chip UART control functions.
 *
 * \param dev  Identifies the device that receives the device-control
 *             function.
 * \param req  Requested control function. May be set to one of the
 *             following constants:
 *             - UART_SETSPEED, conf points to an u_long value containing the baudrate.
 *             - UART_GETSPEED, conf points to an u_long value receiving the current baudrate.
 *             - UART_SETDATABITS, conf points to an u_long value containing the number of data bits, 5, 6, 7 or 8.
 *             - UART_GETDATABITS, conf points to an u_long value receiving the number of data bits, 5, 6, 7 or 8.
 *             - UART_SETPARITY, conf points to an u_long value containing the parity, 0 (no), 1 (odd) or 2 (even).
 *             - UART_GETPARITY, conf points to an u_long value receiving the parity, 0 (no), 1 (odd) or 2 (even).
 *             - UART_SETSTOPBITS, conf points to an u_long value containing the number of stop bits 1 or 2.
 *             - UART_GETSTOPBITS, conf points to an u_long value receiving the number of stop bits 1 or 2.
 *             - UART_SETSTATUS
 *             - UART_GETSTATUS
 *             - UART_SETREADTIMEOUT, conf points to an u_long value containing the read timeout.
 *             - UART_GETREADTIMEOUT, conf points to an u_long value receiving the read timeout.
 *             - UART_SETWRITETIMEOUT, conf points to an u_long value containing the write timeout.
 *             - UART_GETWRITETIMEOUT, conf points to an u_long value receiving the write timeout.
 *             - UART_SETLOCALECHO, conf points to an u_long value containing 0 (off) or 1 (on).
 *             - UART_GETLOCALECHO, conf points to an u_long value receiving 0 (off) or 1 (on).
 *             - UART_SETFLOWCONTROL, conf points to an u_long value containing combined UART_FCTL_ values.
 *             - UART_GETFLOWCONTROL, conf points to an u_long value containing receiving UART_FCTL_ values.
 *             - UART_SETCOOKEDMODE, conf points to an u_long value containing 0 (off) or 1 (on).
 *             - UART_GETCOOKEDMODE, conf points to an u_long value receiving 0 (off) or 1 (on).
 *
 * \param conf Points to a buffer that contains any data required for
 *             the given control function or receives data from that
 *             function.
 * \return 0 on success, -1 otherwise.
 *
 * \warning Timeout values are given in milliseconds and are limited to
 *          the granularity of the system timer.
 *
 * \bug For ATmega103, only 8 data bits, 1 stop bit and no parity are allowed.
 *
 */
int SciH8IOCtl(NUTDEVICE * dev, int req, void *conf)
{
    int rc = 0;
    UARTDCB *dcb;
    u_long *lvp = (u_long *) conf;
    u_long lv = *lvp;
    u_char bv = (u_char) lv;
    u_char devnum;
    volatile struct st_sci *sci = GET_SCI(dev->dev_base);

    if (dev == 0)
        dev = &devSci0;

    devnum = dev->dev_base;
    dcb = dev->dev_dcb;

    switch (req) {
    case UART_SETSPEED:
        SciH8Disable(devnum);
        u_char cks = 0;
        u_long brr = ((NutGetCpuClock() / (64UL / 2UL)) / *lvp) - 1;
        if (brr > 0xff) {
            cks = 1;
            brr = ((NutGetCpuClock() / (64UL * 2UL)) / *lvp) - 1;
            if (brr > 0xff) {
                cks = 2;
                brr = ((NutGetCpuClock() / (64UL * 8UL)) / *lvp) - 1;
                if (brr > 0xff) {
                    cks = 3;
                    brr = ((NutGetCpuClock() / (64UL * 32UL)) / *lvp) - 1;
                }
            }
        }
        sci->SMR.BIT.CKS = cks;
        sci->BRR = (u_char) brr;
        dcb->dcb_baudSelect = brr;
        NutDelay(2);            /* wait for a while */
        SciH8Enable(devnum);
        break;

    case UART_GETSPEED:
        switch (sci->SMR.BIT.CKS) {
        case 0:
            *lvp = NutGetCpuClock() / ((64UL / 2UL) * (u_long) (sci->BRR + 1));
            break;
        case 1:
            *lvp = NutGetCpuClock() / ((64UL * 2UL) * (u_long) (sci->BRR + 1));
            break;
        case 2:
            *lvp = NutGetCpuClock() / ((64UL * 8UL) * (u_long) (sci->BRR + 1));
            break;
        case 3:
            *lvp = NutGetCpuClock() / ((64UL * 32UL) * (u_long) (sci->BRR + 1));
            break;
        }
        break;

    case UART_SETDATABITS:
        SciH8Disable(devnum);
        switch (bv) {
        case 8:                /* 8 data bits */
            sci->SMR.BIT.CHR = 0;
            break;
        case 7:                /* 7 data bits */
            sci->SMR.BIT.CHR = 1;
            break;
        default:
            rc = -1;
        }
        SciH8Enable(devnum);
        break;

    case UART_GETDATABITS:
        switch (sci->SMR.BIT.CHR) {
        case 0:                /* 8 data bits */
            *lvp = 8;
            break;
        case 1:                /* 7 data bits */
            *lvp = 7;
            break;
        }
        break;

    case UART_SETPARITY:
        SciH8Disable(devnum);
        switch (bv) {
        case 'n':              /* none */
            sci->SMR.BIT.PE = 0;
            break;
        case 'e':              /* even */
            sci->SMR.BIT.PE = 1;
            sci->SMR.BIT.OE = 0;
            break;
        case 'o':              /* odd */
            sci->SMR.BIT.PE = 1;
            sci->SMR.BIT.OE = 1;
            break;
        default:
            rc = -1;
        }
        SciH8Enable(devnum);
        break;

    case UART_GETPARITY:
        if (sci->SMR.BIT.PE == 0) {     /* none */
            bv = 'n';
        } else if (sci->SMR.BIT.OE == 0) {      /* even */
            bv = 'e';
        } else {                /* odd */
            bv = 'o';
        }
        break;

    case UART_SETSTOPBITS:
        SciH8Disable(devnum);
        switch (bv) {
        case 1:                /* 1 stop bit */
            sci->SMR.BIT.STOP = 0;
            break;
        case 2:                /* 2 stop bits */
            sci->SMR.BIT.STOP = 1;
            break;
        default:
            rc = -1;
        }
        SciH8Enable(devnum);
        break;

    case UART_GETSTOPBITS:
        switch (sci->SMR.BIT.STOP) {
        case 0:                /* 1 stop bit */
            *lvp = 1;
            break;
        case 1:                /* 2 stop bits */
            *lvp = 2;
            break;
        }
        break;

    case UART_GETSTATUS:
        SciH8GetStatus(dev, lvp);
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
 * Enables SCI transmitter and receiver interrupts.
 *
 * \param dev  Identifies the device to initialize.
 *
 * \return 0 on success, -1 otherwise.
 */
int SciH8Init(NUTDEVICE * dev)
{
    IFSTREAM *ifs;
    UARTDCB *dcb;
    u_long baudrate = 9600;
    volatile struct st_sci *sci = GET_SCI(dev->dev_base);

    /*
     * We only support character devices for on-chip SCIs.
     */
    if (dev->dev_type != IFTYP_STREAM || dev->dev_irq != 0 || (dev->dev_base != 0 && dev->dev_base != 1 && dev->dev_base != 2))
        return -1;

    /*
     * Initialize interface control block.
     */
    ifs = dev->dev_icb;
    memset(ifs, 0, sizeof(IFSTREAM));
    ifs->if_input = SciH8Input;
    ifs->if_output = SciH8Output;
    ifs->if_flush = SciH8Flush;

    /*
     * Initialize driver control block.
     */
    dcb = dev->dev_dcb;
    memset(dcb, 0, sizeof(UARTDCB));
/*     dcb->dcb_baudSelect = 7; */
    dcb->dcb_modeflags = UART_MF_NOBUFFER;

    /*
     * Register interrupt handler.
     */
    if (dev->dev_base == 2) {
        if (NutRegisterIrqHandler(&sig_RXI2, RxComplete, dev))
            return -1;
        if (NutRegisterIrqHandler(&sig_TXI2, TxComplete, dev))
            return -1;
    } else if (dev->dev_base == 1) {
        if (NutRegisterIrqHandler(&sig_RXI1, RxComplete, dev))
            return -1;
        if (NutRegisterIrqHandler(&sig_TXI1, TxComplete, dev))
            return -1;
    } else if (dev->dev_base == 0) {
        if (NutRegisterIrqHandler(&sig_RXI0, RxComplete, dev))
            return -1;
        if (NutRegisterIrqHandler(&sig_TXI0, TxComplete, dev))
            return -1;
    }
    sci->SCR.BYTE = 0;
    /* Set for Async, data bits, parity, stop bit and an undivided clock. */
    sci->SMR.BYTE = DATABITS_8 | NOPARITY | STOP1BIT | NODIVIDE;
    /* Clear relevant status bits. */
    sci->SSR.BYTE = ~(PER | FER | ORER | RDRF);
    sci->SCR.BYTE = (RXI_ENABLE | RX_ENABLE | TX_ENABLE);

    /*
     * Set baudrate and handshake default. This will also
     * enable the UART functions.
     */
    SciH8IOCtl(dev, UART_SETSPEED, &baudrate);

    return 0;
}

/*!
 * \brief Read from device.
 */
int SciH8Read(NUTFILE * fp, void *buffer, int size)
{
    int rc;
    NUTDEVICE *dev;
    IFSTREAM *ifs;
    UARTDCB *dcb;
    u_char elmode;
    u_char ch;
    u_char *cp = buffer;

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
                if (SciH8Input(dev)) {
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
int SciH8Put(NUTDEVICE * dev, CONST void *buffer, int len, int pflg)
{
    int rc;
    IFSTREAM *ifs;
    UARTDCB *dcb;
    CONST u_char *cp;
    u_char lbmode;
    u_char elmode;
    u_char ch;

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
        rc = SciH8Flush(dev);
        return rc;
    }

    /*
     * Put characters in transmit buffer.
     */
    cp = buffer;
    for (rc = 0; rc < len;) {
        if ((u_char) (ifs->if_wr_idx + 1) == ifs->if_tx_idx) {
            if (SciH8Flush(dev)) {
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
        if (SciH8Flush(dev))
            rc = -1;
    }
    return rc;
}

int SciH8Write(NUTFILE * fp, CONST void *buffer, int len)
{
    return SciH8Put(fp->nf_dev, buffer, len, 0);
}

/*!
 * \brief Open a device or file.
 */
NUTFILE *SciH8Open(NUTDEVICE * dev, CONST char *name, int mode, int acc)
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
int SciH8Close(NUTFILE * fp)
{
    NutHeapFree(fp);

    return 0;
}

/*@}*/
