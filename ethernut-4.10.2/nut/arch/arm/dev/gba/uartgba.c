/*
 * Copyright (C) 2001-2005 by egnite Software GmbH. All rights reserved.
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
 * Revision 1.5  2008/08/11 06:59:13  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.4  2007/08/29 07:43:52  haraldkipp
 * Documentation updated and corrected.
 *
 * Revision 1.3  2005/10/24 17:59:19  haraldkipp
 * Use correct header file, arm, not gba.
 *
 * Revision 1.2  2005/08/02 17:46:45  haraldkipp
 * Major API documentation update.
 *
 * Revision 1.1  2005/07/26 18:02:26  haraldkipp
 * Moved from dev.
 *
 * Revision 1.1  2005/04/05 18:01:43  haraldkipp
 * This code is buggy! Anyone willing to help?
 *
 */

#include <sys/atom.h>
#include <sys/event.h>
#include <sys/timer.h>

#include <dev/irqreg.h>

#include <arch/arm.h>
#include <dev/usart.h>

#include <stdio.h>
/*!
 * \addtogroup xgUartGba
 */
/*@{*/

/* \brief ASCII code for software flow control, starts transmitter. */
#define ASCII_XON   0x11
/* \brief ASCII code for software flow control, stops transmitter. */
#define ASCII_XOFF  0x13

/* \brief XON transmit pending flag. */
#define XON_PENDING     0x10
/* \brief XOFF transmit pending flag. */
#define XOFF_PENDING    0x20
/* \brief XOFF sent flag. */
#define XOFF_SENT       0x40
/* \brief XOFF received flag. */
#define XOFF_RCVD       0x80


/*!
 * \brief Receiver error flags.
 */
//static ureg_t rx_errors;

/*!
 * \brief Enables software flow control if not equal zero.
 */
static ureg_t flow_control;

static volatile ureg_t tx_stop = 1;

static USARTDCB dcb_uart;

/*
 * \brief USART0 transmit data register empty interrupt handler.
 *
 * \param arg Pointer to the transmitter ring buffer.
 */
static void GbaUartTxEmpty(RINGBUF * rbf)
{
    register uint8_t *cp = rbf->rbf_tail;

    /*
     * Process pending software flow controls first.
     */
    if (flow_control & (XON_PENDING | XOFF_PENDING)) {
        if (flow_control & XON_PENDING) {
            outw(REG_SIODATA8, ASCII_XOFF);
            flow_control |= XOFF_SENT;
        } else {
            outw(REG_SIODATA8, ASCII_XON);
            flow_control &= ~XOFF_SENT;
        }
        flow_control &= ~(XON_PENDING | XOFF_PENDING);
        return;
    }

    if (flow_control & XOFF_RCVD) {
        /* 
         * If XOFF has been received, we disable the transmit interrupts
         * and return without sending anything.
         */
        tx_stop = 1;
        outw(REG_SIOCNT, inw(REG_SIOCNT) & ~SIO_SEND_ENA);
        return;
    }

    if (rbf->rbf_cnt) {

        rbf->rbf_cnt--;

        /*
         * Start transmission of the next character.
         */
        outw(REG_SIODATA8, *cp);

        /*
         * Wrap around the buffer pointer if we reached its end.
         */
        if (++cp == rbf->rbf_last) {
            cp = rbf->rbf_start;
        }
        rbf->rbf_tail = cp;
        if (rbf->rbf_cnt == rbf->rbf_lwm) {
            NutEventPostFromIrq(&rbf->rbf_que);
        }
    }

    /*
     * Nothing left to transmit, disable interrupt.
     */
    else {
        tx_stop = 1;
        outw(REG_SIOCNT, (inw(REG_SIOCNT) & ~SIO_SEND_ENA) | SIO_TX_FULL);
        rbf->rbf_cnt = 0;
        NutEventPostFromIrq(&rbf->rbf_que);
    }
}

/*
 * \brief USART0 receive complete interrupt handler.
 *
 *
 * \param arg Pointer to the receiver ring buffer.
 */
static void GbaUartRxFull(RINGBUF * rbf)
{
    register size_t cnt;
    register uint8_t ch;

    /*
     * We read the received character as early as possible to avoid overflows
     * caused by interrupt latency. However, reading the error flags must come 
     * first, because reading the ATmega128 data register clears the status.
     */
    //TODO rx_errors |= inb(UCSRnA);
    ch = (uint8_t) inw(REG_SIODATA8);

    /*
     * Handle software handshake. We have to do this before checking the
     * buffer, because flow control must work in write-only mode, where
     * there is no receive buffer.
     */
    if (flow_control) {
        /* XOFF character disables transmit interrupts. */
        if (ch == ASCII_XOFF) {
            tx_stop = 1;
            outw(REG_SIOCNT, inw(REG_SIOCNT) & ~SIO_SEND_ENA);
            flow_control |= XOFF_RCVD;
            return;
        }
        /* XON enables transmit interrupts. */
        else if (ch == ASCII_XON) {
            tx_stop = 0; //TODO
            outw(REG_SIOCNT, inw(REG_SIOCNT) | SIO_SEND_ENA);
            flow_control &= ~XOFF_RCVD;
            return;
        }
    }

    /*
     * Check buffer overflow.
     */
    cnt = rbf->rbf_cnt;
    if (cnt >= rbf->rbf_siz) {
        //TODO rx_errors |= _BV(DOR);
        return;
    }

    /* Wake up waiting threads if this is the first byte in the buffer. */
    if (cnt++ == 0) {
        NutEventPostFromIrq(&rbf->rbf_que);
    }

    /*
     * Check the high watermark for software handshake. If the number of 
     * buffered bytes is above this mark, then send XOFF.
     */
    else if (flow_control) {
        if (cnt >= rbf->rbf_hwm) {
            if ((flow_control & XOFF_SENT) == 0) {
                if (inw(REG_SIOCNT) & SIO_TX_FULL) {
                    flow_control |= XOFF_PENDING;
                } else {
                    outw(REG_SIODATA8, ASCII_XOFF);
                    flow_control |= XOFF_SENT;
                    flow_control &= ~XOFF_PENDING;
                }
            }
        }
    }

    /* 
     * Store the character and increment and the ring buffer pointer. 
     */
    *rbf->rbf_head++ = ch;
    if (rbf->rbf_head == rbf->rbf_last) {
        rbf->rbf_head = rbf->rbf_start;
    }

    /* Update the ring buffer counter. */
    rbf->rbf_cnt = cnt;
}

static void GbaUartIsr(void *arg)
{
    /* Clear interrupt. */
    outw(REG_IF, INT_SIO);

    if ((inw(REG_SIOCNT) & SIO_RX_EMPTY) == 0) {
        GbaUartRxFull(&((USARTDCB *) arg)->dcb_rx_rbf);
    }
    if ((inw(REG_SIOCNT) & SIO_TX_FULL) == 0) {
        GbaUartTxEmpty(&((USARTDCB *) arg)->dcb_tx_rbf);
    }
}

/*!
 * \brief Carefully enable USART hardware functions.
 *
 * Always enabale transmitter and receiver, even on read-only or
 * write-only mode. So we can support software flow control.
 */
static void GbaUartEnable(void)
{
    NutEnterCritical();

    //TODO outb(UCSRnB, _BV(RXCIE) | _BV(UDRIE) | _BV(RXEN) | _BV(TXEN));

    NutExitCritical();
}

/*!
 * \brief Carefully disable USART hardware functions.
 */
static void GbaUartDisable(void)
{
    /*
     * Disable USART interrupts.
     */
    //TODO NutEnterCritical();
    //TODO cbi(UCSRnB, RXCIE);
    //TODO cbi(UCSRnB, TXCIE);
    //TODO cbi(UCSRnB, UDRIE);
    //TODO NutExitCritical();

    /*
     * Allow incoming or outgoing character to finish.
     */
    //TODO NutDelay(10);

    /*
     * Disable USART transmit and receive.
     */
    //TODO cbi(UCSRnB, RXEN);
    //TODO cbi(UCSRnB, TXEN);
}

/*!
 * \brief Query the USART hardware for the selected speed.
 *
 * This function is called by ioctl function of the upper level USART 
 * driver through the USARTDCB jump table.
 *
 * \return The currently selected baudrate.
 */
static uint32_t GbaUartGetSpeed(void)
{
    uint16_t sv = inw(REG_SIOCNT);

    if ((sv & SIO_BAUD_115200) == SIO_BAUD_115200) {
        return 115200UL;
    }
    if ((sv & SIO_BAUD_57600) == SIO_BAUD_57600) {
        return 57600UL;
    }
    if ((sv & SIO_BAUD_38400) == SIO_BAUD_38400) {
        return 38400L;
    }
    return 9600UL;
}

/*!
 * \brief Set the USART hardware bit rate.
 *
 * This function is called by ioctl function of the upper level USART 
 * driver through the USARTDCB jump table.
 *
 * \param rate Number of bits per second.
 *
 * \return 0 on success, -1 otherwise.
 */
static int GbaUartSetSpeed(uint32_t rate)
{
    uint16_t sv;

    GbaUartDisable();
    sv = inw(REG_SIOCNT) & ~SIO_BAUD_115200;
    if (rate == 115200) {
        sv |= SIO_BAUD_115200;
    } else {
        if (rate == 57600) {
            sv |= SIO_BAUD_57600;
        } else if (rate == 38400) {
            sv |= SIO_BAUD_38400;
        }
    }
    outw(REG_SIOCNT, sv);
    GbaUartEnable();

    return 0;
}

/*!
 * \brief Query the USART hardware for the number of data bits.
 *
 * This function is called by ioctl function of the upper level USART 
 * driver through the USARTDCB jump table.
 *
 * \return The number of data bits set.
 */
static uint8_t GbaUartGetDataBits(void)
{
    return 8;
}

/*!
 * \brief Set the USART hardware to the number of data bits.
 *
 * This function is called by ioctl function of the upper level USART 
 * driver through the USARTDCB jump table.
 *
 * \return 0 on success, -1 otherwise.
 */
static int GbaUartSetDataBits(uint8_t bits)
{
    GbaUartDisable();
    GbaUartEnable();

    /*
     * Verify the result.
     */
    if (GbaUartGetDataBits() != bits) {
        return -1;
    }
    return 0;
}

/*!
 * \brief Query the USART hardware for the parity mode.
 *
 * This routine is called by ioctl function of the upper level USART 
 * driver through the USARTDCB jump table.
 *
 * \return Parity mode, either 0 (disabled), 1 (odd) or 2 (even).
 */
static uint8_t GbaUartGetParity(void)
{
    return 0;
}

/*!
 * \brief Set the USART hardware to the specified parity mode.
 *
 * This routine is called by ioctl function of the upper level USART 
 * driver through the USARTDCB jump table.
 *
 * \param mode 0 (disabled), 1 (odd) or 2 (even)
 *
 * \return 0 on success, -1 otherwise.
 */
static int GbaUartSetParity(uint8_t mode)
{
    GbaUartDisable();
    GbaUartEnable();

    /*
     * Verify the result.
     */
    if (GbaUartGetParity() != mode) {
        return -1;
    }
    return 0;
}

/*!
 * \brief Query the USART hardware for the number of stop bits.
 *
 * This routine is called by ioctl function of the upper level USART 
 * driver through the USARTDCB jump table.
 *
 * \return The number of stop bits set, either 1 or 2.
 */
static uint8_t GbaUartGetStopBits(void)
{
    return 1;
}

/*!
 * \brief Set the USART hardware to the number of stop bits.
 *
 * This routine is called by ioctl function of the upper level USART 
 * driver through the USARTDCB jump table.
 *
 * \return 0 on success, -1 otherwise.
 */
static int GbaUartSetStopBits(uint8_t bits)
{
    GbaUartDisable();
    GbaUartEnable();

    /*
     * Verify the result.
     */
    if (GbaUartGetStopBits() != bits) {
        return -1;
    }
    return 0;
}

/*!
 * \brief Query the USART hardware status.
 *
 * \return Status flags.
 */
static uint32_t GbaUartGetStatus(void)
{
    uint32_t rc = 0;

    /*
     * Set receiver error flags.
     */

    /*
     * Determine software handshake status. The flow control status may
     * change during interrupt, but this doesn't really hurt us.
     */
    if (flow_control) {
        if (flow_control & XOFF_SENT) {
            rc |= UART_RXDISABLED;
        }
        if (flow_control & XOFF_RCVD) {
            rc |= UART_TXDISABLED;
        }
    }

    /*
     * If transmitter and receiver haven't been detected disabled by any
     * of the checks above, then they are probably enabled.
     */
    if ((rc & UART_RXDISABLED) == 0) {
        rc |= UART_RXENABLED;
    }
    if ((rc & UART_TXDISABLED) == 0) {
        rc |= UART_TXENABLED;
    }
    return rc;
}

/*!
 * \brief Set the USART hardware status.
 *
 * \param flags Status flags.
 *
 * \return 0 on success, -1 otherwise.
 */
static int GbaUartSetStatus(uint32_t flags)
{
    /*
     * Process software handshake control.
     */
    if (flow_control) {

        /* Access to the flow control status must be atomic. */
        NutEnterCritical();

        /*
         * Enabling or disabling the receiver means to behave like 
         * having sent a XON or XOFF character resp.
         */
        if (flags & UART_RXENABLED) {
            flow_control &= ~XOFF_SENT;
        } else if (flags & UART_RXDISABLED) {
            flow_control |= XOFF_SENT;
        }

        /*
         * Enabling or disabling the transmitter means to behave like 
         * having received a XON or XOFF character resp.
         */
        if (flags & UART_TXENABLED) {
            flow_control &= ~XOFF_RCVD;
        } else if (flags & UART_TXDISABLED) {
            flow_control |= XOFF_RCVD;
        }
        NutExitCritical();
    }

    /*
     * Verify the result.
     */
    if ((GbaUartGetStatus() & ~UART_ERRORS) != flags) {
        return -1;
    }
    return 0;
}

/*!
 * \brief Query the USART hardware for synchronous mode.
 *
 * This function is called by ioctl function of the upper level USART 
 * driver through the USARTDCB jump table.
 *
 * \return Or-ed combination of \ref UART_SYNC, \ref UART_MASTER, 
 *         \ref UART_NCLOCK and \ref UART_HIGHSPEED.
 */
static uint8_t GbaUartGetClockMode(void)
{
    return 0;
}

/*!
 * \brief Set asynchronous or synchronous mode.
 *
 * This function is called by ioctl function of the upper level USART 
 * driver through the USARTDCB jump table.
 *
 * \param mode Must be an or-ed combination of USART_SYNC, USART_MASTER,
 *             USART_NCLOCK and USART_HIGHSPEED. 
 *
 * \return 0 on success, -1 otherwise.
 */
static int GbaUartSetClockMode(uint8_t mode)
{
    /*
     * Verify the result.
     */
    if (GbaUartGetClockMode() != mode) {
        return -1;
    }
    return 0;
}

/*!
 * \brief Query flow control mode.
 *
 * This routine is called by ioctl function of the upper level USART 
 * driver through the USARTDCB jump table.
 *
 * \return See UsartIOCtl().
 */
static uint32_t GbaUartGetFlowControl(void)
{
    uint32_t rc = 0;

    if (flow_control) {
        rc |= USART_MF_XONXOFF;
    } else {
        rc &= ~USART_MF_XONXOFF;
    }

    return rc;
}

/*!
 * \brief Set flow control mode.
 *
 * This function is called by ioctl function of the upper level USART 
 * driver through the USARTDCB jump table.
 *
 * \param flags See UsartIOCtl().
 *
 * \return 0 on success, -1 otherwise.
 */
static int GbaUartSetFlowControl(uint32_t flags)
{
    /*
     * Set software handshake mode.
     */
    if (flags & USART_MF_XONXOFF) {
        if (flow_control == 0) {
            NutEnterCritical();
            flow_control = 1 | XOFF_SENT;       /* force XON to be sent on next read */
            NutExitCritical();
        }
    } else {
        NutEnterCritical();
        flow_control = 0;
        NutExitCritical();
    }

    /*
     * Verify the result.
     */
    if (GbaUartGetFlowControl() != flags) {
        return -1;
    }
    return 0;
}

/*!
 * \brief Start the USART transmitter hardware.
 *
 * The upper level USART driver will call this function through the 
 * USARTDCB jump table each time it added one or more bytes to the 
 * transmit buffer.
 */
static void GbaUartTxStart(void)
{
    RINGBUF *rbf = &dcb_uart.dcb_tx_rbf;
    register uint8_t *cp = rbf->rbf_tail;

    NutEnterCritical();
    if(tx_stop) {
        if (rbf->rbf_cnt) {
            rbf->rbf_cnt--;
            outw(REG_SIODATA8, *cp);
            if (++cp == rbf->rbf_last) {
                cp = rbf->rbf_start;
            }
            rbf->rbf_tail = cp;
            tx_stop = 0;
        }
    }
    outw(REG_SIOCNT, inw(REG_SIOCNT) | SIO_SEND_ENA);
    NutExitCritical();
}

/*!
 * \brief Start the USART receiver hardware.
 *
 * The upper level USART driver will call this function through the 
 * USARTDCB jump table each time it removed enough bytes from the 
 * receive buffer. Enough means, that the number of bytes left in
 * the buffer is below the low watermark.
 */
static void GbaUartRxStart(void)
{
    /*
     * Do any required software flow control.
     */
    NutEnterCritical();
    if (flow_control && (flow_control & XOFF_SENT) != 0) {
        if (inw(REG_SIOCNT) & SIO_TX_FULL) {
            flow_control |= XON_PENDING;
        } else {
            outw(REG_SIODATA8, ASCII_XON);
            flow_control &= ~XON_PENDING;
        }
        flow_control &= ~(XOFF_SENT | XOFF_PENDING);
    }
    outw(REG_SIOCNT, inw(REG_SIOCNT) | SIO_RECV_ENA);
    NutExitCritical();
}

/*
 * \brief Initialize the USART hardware driver.
 *
 * This function is called during device registration by the upper level 
 * USART driver through the USARTDCB jump table.
 *
 * \return 0 on success, -1 otherwise.
 */
static int GbaUartInit(void)
{
    int rc;

    /*
     * Activate MBV2 UART mode by setting SC low and SD high for at 
     * least 50 milliseconds.
     */
    outw(REG_RCNT, 0x8032);
    NutSleep(100);

    /* Disable master interrupt. */
    outw(REG_IME, 0);

    /* Register our interrupt service. */
    if((rc = NutRegisterIrqHandler(&sig_SIO, GbaUartIsr, &dcb_uart)) == 0) {

        /* Enable UART mode. */
        outw(REG_RCNT, 0x0000);

        /* Set UART mode */
        outw(REG_SIOCNT, SIO_IRQ_ENA | SIO_MODE_UART | SIO_DATA_8BIT | SIO_BAUD_38400);

        /* Clear receive/transmit */
        outw(REG_SIOCNT, inw(REG_SIOCNT) | SIO_RX_EMPTY | SIO_TX_FULL);

        /* Enable FIFO. */
        //outw(REG_SIOCNT, inw(REG_SIOCNT) | SIO_FIFO_ENA);

        /* Enable SIO interrupts. */
        outw(REG_IE, inw(REG_IE) | INT_SIO);
    }

    printf("I[%04X]", inw(REG_SIOCNT));

    /* Enable master interrupt. */
    outw(REG_IME, 1);

    return rc;
}

/*
 * \brief Deinitialize the USART hardware driver.
 *
 * This function is called during device deregistration by the upper 
 * level USART driver through the USARTDCB jump table.
 *
 * \return 0 on success, -1 otherwise.
 */
static int GbaUartDeinit(void)
{
    /* Deregister receive and transmit interrupts. */
    NutRegisterIrqHandler(&sig_SIO, 0, 0);

    return 0;
}

#if 1
/*!
 * \brief UART0 device control block structure.
 */
static USARTDCB dcb_uart = {
    0,                          /* dcb_modeflags */
    0,                          /* dcb_statusflags */
    0,                          /* dcb_rtimeout */
    0,                          /* dcb_wtimeout */
    {0, 0, 0, 0, 0, 0, 0, 0},   /* dcb_tx_rbf */
    {0, 0, 0, 0, 0, 0, 0, 0},   /* dcb_rx_rbf */
    0,                          /* dbc_last_eol */
    GbaUartInit,                /* dcb_init */
    GbaUartDeinit,              /* dcb_deinit */
    GbaUartTxStart,             /* dcb_tx_start */
    GbaUartRxStart,             /* dcb_rx_start */
    GbaUartSetFlowControl,      /* dcb_set_flow_control */
    GbaUartGetFlowControl,      /* dcb_get_flow_control */
    GbaUartSetSpeed,            /* dcb_set_speed */
    GbaUartGetSpeed,            /* dcb_get_speed */
    GbaUartSetDataBits,         /* dcb_set_data_bits */
    GbaUartGetDataBits,         /* dcb_get_data_bits */
    GbaUartSetParity,           /* dcb_set_parity */
    GbaUartGetParity,           /* dcb_get_parity */
    GbaUartSetStopBits,         /* dcb_set_stop_bits */
    GbaUartGetStopBits,         /* dcb_get_stop_bits */
    GbaUartSetStatus,           /* dcb_set_status */
    GbaUartGetStatus,           /* dcb_get_status */
    GbaUartSetClockMode,        /* dcb_set_clock_mode */
    GbaUartGetClockMode,        /* dcb_get_clock_mode */
};

/*!
 * \name AVR USART0 Device
 */
/*@{*/
/*!
 * \brief USART0 device information structure.
 *
 * An application must pass a pointer to this structure to
 * NutRegisterDevice() before using the serial communication
 * driver of the AVR's on-chip USART0.
 *
 * The device is named \b uart0.
 *
 * \showinitializer
 */
NUTDEVICE devUartGba = {
    0,                          /* Pointer to next device, dev_next. */
    {'u', 'a', 'r', 't', '0', 0, 0, 0, 0},      /* Unique device name, dev_name. */
    IFTYP_CHAR,                 /* Type of device, dev_type. */
    0,                          /* Base address, dev_base (not used). */
    0,                          /* First interrupt number, dev_irq (not used). */
    0,                          /* Interface control block, dev_icb (not used). */
    &dcb_uart,                  /* Driver control block, dev_dcb. */
    UsartInit,                  /* Driver initialization routine, dev_init. */
    UsartIOCtl,                 /* Driver specific control function, dev_ioctl. */
    UsartRead,                  /* Read from device, dev_read. */
    UsartWrite,                 /* Write to device, dev_write. */
    UsartOpen,                  /* Open a device or file, dev_open. */
    UsartClose,                 /* Close a device or file, dev_close. */
    UsartSize                   /* Request file size, dev_size. */
};
#endif

/*@}*/
/*@}*/
