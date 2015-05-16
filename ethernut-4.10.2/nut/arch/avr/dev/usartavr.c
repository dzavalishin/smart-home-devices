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
 * The 9-bit communication had been contributed by Brett Abbott,
 * Digital Telemetry Limited.
 *
 * Dave Smart contributed the synchronous mode support.
 */

/*
 * $Log$
 * Revision 1.7  2008/08/11 06:59:17  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.6  2008/04/29 02:28:34  thiagocorrea
 * Add configurable DTR pin to AVR USART driver.
 *
 * Revision 1.5  2007/11/13 20:16:33  thiagocorrea
 * Fix a small documentation typo
 *
 * Revision 1.4  2007/03/08 16:59:01  freckle
 * moved Exit Tracer event to end of IRQ
 *
 * Revision 1.3  2006/08/05 11:53:02  haraldkipp
 * Half duplex flow control used the wrong buffer. Many thanks to
 * Andrej Taran for fixing this bug.
 *
 * Revision 1.2  2005/10/07 22:05:00  hwmaier
 * Using __AVR_ENHANCED__ macro instead of __AVR_ATmega128__ to support also AT90CAN128 MCU
 *
 * Revision 1.1  2005/07/26 18:02:40  haraldkipp
 * Moved from dev.
 *
 * Revision 1.10  2005/07/22 08:07:07  freckle
 * added experimental improvements to usart driver. see ChangeLog for details
 *
 * Revision 1.9  2005/02/21 12:38:00  phblum
 * Removed tabs and added semicolons after NUTTRACER macros
 *
 * Revision 1.8  2005/01/24 22:34:46  freckle
 * Added new tracer by Phlipp Blum <blum@tik.ee.ethz.ch>
 *
 * Revision 1.6  2005/01/21 16:49:46  freckle
 * Seperated calls to NutEventPostAsync between Threads and IRQs
 *
 * Revision 1.5  2004/11/12 08:25:51  drsung
 * Bugfix in AvrUsartTxEmpty. Thanks to Grzegorz Plonski and Matthias Ringwald.
 *
 * Revision 1.4  2004/05/26 09:04:17  drsung
 * Bugfix in AvrUsartTxStart. Now the correct port and pin are used for half duplex mode...again...
 * Thanks to Przemyslaw Rudy.
 *
 * Revision 1.3  2004/05/16 14:09:06  drsung
 * Applied bugfixes for half duplex mode an XON/XOFF handling. Thanks to Damian Slee.
 *
 * Revision 1.2  2004/04/07 12:58:52  haraldkipp
 * Bugfix for half duplex mode
 *
 * Revision 1.1  2003/12/15 19:25:33  haraldkipp
 * New USART driver added
 *
 */

#include <sys/atom.h>
#include <sys/event.h>
#include <sys/timer.h>

#include <dev/irqreg.h>

#include <dev/usartavr.h>

#ifdef NUTTRACER
#include <sys/tracer.h>
#endif

/*!
 * \addtogroup xgUsartAvr
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
static ureg_t rx_errors;

/*!
 * \brief Enables software flow control if not equal zero.
 */
static ureg_t flow_control;

/*!
 * \brief Transmit address frame, if not zero.
 */
static ureg_t tx_aframe;

#ifdef UART_HDX_BIT
	/* define in cfg/modem.h */
	#ifdef UART_HDX_FLIP_BIT	/* same as RTS toggle by Windows NT driver */
		#define UART_HDX_TX		cbi
		#define UART_HDX_RX		sbi
	#else						/* previous usage by Ethernut */
		#define UART_HDX_TX		sbi
		#define UART_HDX_RX		cbi
	#endif
#endif


#ifdef UART_HDX_BIT
/*!
 * \brief Enables half duplex control if not equal zero.
 *
 * This variable exists only if the hardware configuration defines a
 * port bit to switch between receive and transmit mode.
 */
static ureg_t hdx_control;
#endif

#ifdef UART_RTS_BIT
/*!
 * \brief Enables RTS control if not equal zero.
 *
 * This variable exists only if the hardware configuration defines a
 * port bit to control the RTS signal.
 */
static ureg_t rts_control;
#endif

#ifdef UART_CTS_BIT
/*!
 * \brief Enables CTS sense if not equal zero.
 *
 * This variable exists only if the hardware configuration defines a
 * port bit to sense the CTS signal.
 */
static ureg_t cts_sense;
#endif

#ifdef UART_CTS_BIT
/*!
 * \brief USARTn CTS sense interrupt handler.
 *
 * This interrupt routine is called when the CTS line level is low.
 * Typical line drivers negate the signal, thus driving our port
 * low when CTS is active.
 *
 * This routine exists only if the hardware configuration defines a
 * port bit to sense the CTS signal.
 */
static void AvrUsartCts(void *arg)
{
    /* Enable transmit interrupt. */
    sbi(UCSRnB, UDRIE);
    /* Disable CTS sense interrupt. */
    cbi(EIMSK, UART_CTS_BIT);
}
#endif

#ifdef UART_HDX_BIT
/*
 * \brief USARTn transmit complete interrupt handler.
 *
 * Used with half duplex communication to switch from tranmit to receive
 * mode after the last character has been transmitted.
 *
 * This routine exists only if the hardware configuration defines a
 * port bit to switch between receive and transmit mode.
 *
 * \param arg Pointer to the transmitter ring buffer.
 */
static void AvrUsartTxComplete(void *arg)
{
    register RINGBUF *rbf = (RINGBUF *) arg;

    /*
     * Check if half duplex mode has been enabled and if all characters
     * had been sent out.
     */
    if (hdx_control && rbf->rbf_cnt == 0) {
        /* Switch to receiver mode. */
        UART_HDX_RX(UART_HDX_PORT, UART_HDX_BIT);
    }
}
#endif

/*
 * \brief USARTn transmit data register empty interrupt handler.
 *
 * \param arg Pointer to the transmitter ring buffer.
 */
#ifdef USE_USART

SIGNAL( SIG_UART_DATA ) {
    register RINGBUF *rbf = &dcb_usart.dcb_tx_rbf;

#else

static void AvrUsartTxEmpty(void *arg) {
    register RINGBUF *rbf = (RINGBUF *) arg;

#endif

    register uint8_t *cp = rbf->rbf_tail;


#ifdef NUTTRACER
    TRACE_ADD_ITEM(TRACE_TAG_INTERRUPT_ENTER,TRACE_INT_UART_TXEMPTY);
#endif

#ifndef UART_NO_SW_FLOWCONTROL

    /*
     * Process pending software flow controls first.
     */
    if (flow_control & (XON_PENDING | XOFF_PENDING)) {
        if (flow_control & XON_PENDING) {
            outb(UDRn, ASCII_XOFF);
            flow_control |= XOFF_SENT;
        } else {
            outb(UDRn, ASCII_XON);
            flow_control &= ~XOFF_SENT;
        }
        flow_control &= ~(XON_PENDING | XOFF_PENDING);
#ifdef NUTTRACER
        TRACE_ADD_ITEM(TRACE_TAG_INTERRUPT_EXIT,TRACE_INT_UART_TXEMPTY);
#endif
        return;
    }

    if (flow_control & XOFF_RCVD) {
        /*
         * If XOFF has been received, we disable the transmit interrupts
         * and return without sending anything.
         */
        cbi(UCSRnB, UDRIE);
#ifdef NUTTRACER
        TRACE_ADD_ITEM(TRACE_TAG_INTERRUPT_EXIT,TRACE_INT_UART_TXEMPTY);
#endif
        return;
	}
#endif /* UART_NO_SW_FLOWCONTROL */

    if (rbf->rbf_cnt) {

#ifdef UART_CTS_BIT
        /*
         * If CTS has been disabled, we disable the transmit interrupts
         * and return without sending anything.
         */
        if (cts_sense && bit_is_set(UART_CTS_PIN, UART_CTS_BIT)) {
            cbi(UCSRnB, UDRIE);
            sbi(EIMSK, UART_CTS_BIT);
#ifdef NUTTRACER
            TRACE_ADD_ITEM(TRACE_TAG_INTERRUPT_EXIT,TRACE_INT_UART_TXEMPTY);
#endif
            return;
        }
#endif
        rbf->rbf_cnt--;

        /*
         * The data sheet doesn't exactly tell us, if this bit is retained
         * or cleared after the character has been sent out. So we do it
         * the save way.
         */
        if (tx_aframe) {
            sbi(UCSRnB, TXB8);
        } else {
            cbi(UCSRnB, TXB8);
        }

        /*
         * Start transmission of the next character.
         */
        outb(UDRn, *cp);

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
        cbi(UCSRnB, UDRIE);
        rbf->rbf_cnt = 0;
        NutEventPostFromIrq(&rbf->rbf_que);
    }
#ifdef NUTTRACER
    TRACE_ADD_ITEM(TRACE_TAG_INTERRUPT_EXIT,TRACE_INT_UART_TXEMPTY);
#endif
}


/*
 * \brief USARTn receive complete interrupt handler.
 *
 *
 * \param arg Pointer to the receiver ring buffer.
 */

#ifdef USE_USART
SIGNAL( SIG_UART_RECV ){
    register RINGBUF *rbf = &dcb_usart.dcb_rx_rbf;

#else

static void AvrUsartRxComplete(void *arg) {
    register RINGBUF *rbf = (RINGBUF *) arg;

#endif

    register size_t cnt;
    register uint8_t ch;


#ifdef NUTTRACER
    TRACE_ADD_ITEM(TRACE_TAG_INTERRUPT_ENTER,TRACE_INT_UART_RXCOMPL);
#endif

#ifdef UART_READMULTIBYTE
    register uint8_t postEvent = 0;
    do {
#endif

        /*
         * We read the received character as early as possible to avoid overflows
         * caused by interrupt latency. However, reading the error flags must come
         * first, because reading the ATmega128 data register clears the status.
         */
        rx_errors |= inb(UCSRnA);
        ch = inb(UDRn);

#ifndef UART_NO_SW_FLOWCONTROL
        /*
         * Handle software handshake. We have to do this before checking the
         * buffer, because flow control must work in write-only mode, where
         * there is no receive buffer.
         */
        if (flow_control) {
            /* XOFF character disables transmit interrupts. */
            if (ch == ASCII_XOFF) {
                cbi(UCSRnB, UDRIE);
                flow_control |= XOFF_RCVD;
#ifdef NUTTRACER
                TRACE_ADD_ITEM(TRACE_TAG_INTERRUPT_EXIT,TRACE_INT_UART_RXCOMPL);
#endif
                return;
            }
            /* XON enables transmit interrupts. */
            else if (ch == ASCII_XON) {
                sbi(UCSRnB, UDRIE);
                flow_control &= ~XOFF_RCVD;
#ifdef NUTTRACER
                TRACE_ADD_ITEM(TRACE_TAG_INTERRUPT_EXIT,TRACE_INT_UART_RXCOMPL);
#endif
                return;
            }
        }
#endif

        /*
         * Check buffer overflow.
         */
        cnt = rbf->rbf_cnt;
        if (cnt >= rbf->rbf_siz) {
            rx_errors |= _BV(DOR);
#ifdef NUTTRACER
            TRACE_ADD_ITEM(TRACE_TAG_INTERRUPT_EXIT,TRACE_INT_UART_RXCOMPL);
#endif
            return;
        }

        /* Wake up waiting threads if this is the first byte in the buffer. */
        if (cnt++ == 0){
#ifdef UART_READMULTIBYTE
            // we do this later, to get the other bytes in time..
            postEvent = 1;
#else
            NutEventPostFromIrq(&rbf->rbf_que);
#endif
        }

#ifndef UART_NO_SW_FLOWCONTROL

        /*
         * Check the high watermark for software handshake. If the number of
         * buffered bytes is above this mark, then send XOFF.
         */
        else if (flow_control) {
            if(cnt >= rbf->rbf_hwm) {
                if((flow_control & XOFF_SENT) == 0) {
                    if (inb(UCSRnA) & _BV(UDRE)) {
                        outb(UDRn, ASCII_XOFF);
                        flow_control |= XOFF_SENT;
                        flow_control &= ~XOFF_PENDING;
                    } else {
                        flow_control |= XOFF_PENDING;
                    }
                }
            }
        }
#endif


#ifdef UART_RTS_BIT
        /*
         * Check the high watermark for hardware handshake. If the number of
         * buffered bytes is above this mark, then disable RTS.
         */
        else if (rts_control && cnt >= rbf->rbf_hwm) {
            sbi(UART_RTS_PORT, UART_RTS_BIT);
        }
#endif

        /*
         * Store the character and increment and the ring buffer pointer.
         */
        *rbf->rbf_head++ = ch;
        if (rbf->rbf_head == rbf->rbf_last) {
            rbf->rbf_head = rbf->rbf_start;
        }

        /* Update the ring buffer counter. */
        rbf->rbf_cnt = cnt;

#ifdef UART_READMULTIBYTE
    } while ( inb(UCSRnA) & _BV(RXC) ); // byte in buffer?

    // Eventually post event to wake thread
    if (postEvent)
        NutEventPostFromIrq(&rbf->rbf_que);
#endif

#ifdef NUTTRACER
    TRACE_ADD_ITEM(TRACE_TAG_INTERRUPT_EXIT,TRACE_INT_UART_RXCOMPL);
#endif
    
}


/*!
 * \brief Carefully enable USART hardware functions.
 *
 * Always enable transmitter and receiver, even on read-only or
 * write-only mode. So we can support software flow control.
 */
static void AvrUsartEnable(void)
{
    NutEnterCritical();

    outb(UCSRnB, _BV(RXCIE) | _BV(UDRIE) | _BV(RXEN) | _BV(TXEN));

#ifdef UART_HDX_BIT
    if (hdx_control) {
        /* Enable transmit complete interrupt. */
        sbi(UCSRnB, TXCIE);
    }
#endif

    NutExitCritical();
}

/*!
 * \brief Carefully disable USART hardware functions.
 */
static void AvrUsartDisable(void)
{
    /*
     * Disable USART interrupts.
     */
    NutEnterCritical();
    cbi(UCSRnB, RXCIE);
    cbi(UCSRnB, TXCIE);
    cbi(UCSRnB, UDRIE);
    NutExitCritical();

    /*
     * Allow incoming or outgoing character to finish.
     */
    NutDelay(10);

    /*
     * Disable USART transmit and receive.
     */
    cbi(UCSRnB, RXEN);
    cbi(UCSRnB, TXEN);
}

/*!
 * \brief Query the USART hardware for the selected speed.
 *
 * This function is called by ioctl function of the upper level USART
 * driver through the USARTDCB jump table.
 *
 * \return The currently selected baudrate.
 */
static uint32_t AvrUsartGetSpeed(void)
{
    uint32_t fct;
    uint16_t sv = (uint16_t) inb(UBRRnL);

#ifdef __AVR_ENHANCED__
    sv |= ((uint16_t) inb(UBRRnH) << 8);

    /* Synchronous mode. */
    if (bit_is_set(UCSRnC, UMSEL)) {
        fct = 2UL;
    }

    /* Double rate mode. */
    else if (bit_is_set(UCSRnA, U2X)) {
        fct = 8UL;
    }

    /* Normal mode. */
    else {
        fct = 16UL;
    }
#else
    fct = 16UL;
#endif

    return NutGetCpuClock() / (fct * ((uint32_t) sv + 1UL));
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
static int AvrUsartSetSpeed(uint32_t rate)
{
    uint16_t sv;

    AvrUsartDisable();

    /*
     * Modified Robert Hildebrand's refined calculation.
     */
#ifdef __AVR_ENHANCED__
    if (bit_is_clear(UCSRnC, UMSEL)) {
        if (bit_is_set(UCSRnA, U2X)) {
            rate <<= 2;
        } else {
            rate <<= 3;
        }
    }
#else
    rate <<= 3;
#endif
    sv = (uint16_t) ((NutGetCpuClock() / rate + 1UL) / 2UL) - 1;

    outb(UBRRnL, (uint8_t) sv);
#ifdef __AVR_ENHANCED__
    outb(UBRRnH, (uint8_t) (sv >> 8));
#endif
    AvrUsartEnable();

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
static uint8_t AvrUsartGetDataBits(void)
{
    if (bit_is_set(UCSRnB, UCSZ2)) {
        return 9;
    }
#ifdef __AVR_ENHANCED__
    if (bit_is_set(UCSRnC, UCSZ1)) {
        if (bit_is_set(UCSRnC, UCSZ0)) {
            return 8;
        } else {
            return 7;
        }
    } else if (bit_is_set(UCSRnC, UCSZ0)) {
        return 6;
    }
    return 5;
#else
    return 8;
#endif
}

/*!
 * \brief Set the USART hardware to the number of data bits.
 *
 * This function is called by ioctl function of the upper level USART
 * driver through the USARTDCB jump table.
 *
 * \return 0 on success, -1 otherwise.
 */
static int AvrUsartSetDataBits(uint8_t bits)
{
    AvrUsartDisable();
    cbi(UCSRnB, UCSZ2);
#ifdef __AVR_ENHANCED__
    cbi(UCSRnC, UCSZ0);
    cbi(UCSRnC, UCSZ1);
    switch (bits) {
    case 6:
        sbi(UCSRnC, UCSZ0);
        break;
    case 9:
        sbi(UCSRnB, UCSZ2);
    case 8:
        sbi(UCSRnC, UCSZ0);
    case 7:
        sbi(UCSRnC, UCSZ1);
        break;
    }
#else
    if(bits == 9) {
        sbi(UCSRnB, UCSZ2);
    }
#endif
    AvrUsartEnable();

    /*
     * Verify the result.
     */
    if (AvrUsartGetDataBits() != bits) {
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
static uint8_t AvrUsartGetParity(void)
{
#ifdef __AVR_ENHANCED__
    if (bit_is_set(UCSRnC, UPM1)) {
        if (bit_is_set(UCSRnC, UPM0)) {
            return 1;
        } else {
            return 2;
        }
    }
#endif
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
static int AvrUsartSetParity(uint8_t mode)
{
#ifdef __AVR_ENHANCED__
    AvrUsartDisable();
    switch (mode) {
    case 0:
        cbi(UCSRnC, UPM0);
        cbi(UCSRnC, UPM1);
        break;
    case 1:
        sbi(UCSRnC, UPM0);
        sbi(UCSRnC, UPM1);
        break;
    case 2:
        cbi(UCSRnC, UPM0);
        sbi(UCSRnC, UPM1);
        break;
    }
    AvrUsartEnable();
#endif

    /*
     * Verify the result.
     */
    if (AvrUsartGetParity() != mode) {
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
static uint8_t AvrUsartGetStopBits(void)
{
#ifdef __AVR_ENHANCED__
    if (bit_is_set(UCSRnC, USBS)) {
        return 2;
    }
#endif
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
static int AvrUsartSetStopBits(uint8_t bits)
{
#ifdef __AVR_ENHANCED__
    AvrUsartDisable();
    if (bits == 1) {
        cbi(UCSRnC, USBS);
    } else if (bits == 2) {
        sbi(UCSRnC, USBS);
    }
    AvrUsartEnable();
#endif

    /*
     * Verify the result.
     */
    if (AvrUsartGetStopBits() != bits) {
        return -1;
    }
    return 0;
}

/*!
 * \brief Query the USART hardware status.
 *
 * \return Status flags.
 */
static uint32_t AvrUsartGetStatus(void)
{
    uint32_t rc = 0;

    /*
     * Set receiver error flags.
     */
    if ((rx_errors & _BV(FE)) != 0) {
        rc |= UART_FRAMINGERROR;
    }
    if ((rx_errors & _BV(DOR)) != 0) {
        rc |= UART_OVERRUNERROR;
    }
#ifdef __AVR_ENHANCED__
    if ((rx_errors & _BV(UPE)) != 0) {
        rc |= UART_PARITYERROR;
    }
#endif

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
#ifdef UART_RTS_BIT
    /*
     * Determine hardware handshake control status.
     */
    if (bit_is_set(UART_RTS_PORT, UART_RTS_BIT)) {
        rc |= UART_RTSDISABLED;
        if (rts_control) {
            rc |= UART_TXDISABLED;
        }
    } else {
        rc |= UART_RTSENABLED;
    }
#endif

#ifdef UART_CTS_BIT
    /*
     * Determine hardware handshake sense status.
     */
    if (bit_is_set(UART_CTS_PIN, UART_CTS_BIT)) {
        rc |= UART_CTSDISABLED;
        if (cts_sense) {
            rc |= UART_RXDISABLED;
        }
    } else {
        rc |= UART_CTSENABLED;
    }
#endif

#ifdef UART_DTR_BIT
	/*
	* Determine DTS status.
	*/
	if ( bit_is_set( UART_DTR_PORT, UART_DTR_BIT ) ) {
		rc |= UART_DTRENABLED;
	} else {
		rc |= UART_DTRDISABLED;
	}
#endif

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

    /*
     * Process multidrop setting.
     */
    if (tx_aframe) {
        rc |= UART_TXADDRFRAME;
    } else {
        rc |= UART_TXNORMFRAME;
    }

#ifdef __AVR_ENHANCED__
    if (bit_is_set(UCSRnA, MPCM)) {
        rc |= UART_RXADDRFRAME;
    } else {
        rc |= UART_RXNORMFRAME;
    }
#else
    rc |= UART_RXNORMFRAME;
#endif

    return rc;
}

/*!
 * \brief Set the USART hardware status.
 *
 * \param flags Status flags.
 *
 * \return 0 on success, -1 otherwise.
 */
static int AvrUsartSetStatus(uint32_t flags)
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
#ifdef UART_RTS_BIT
    /*
     * Process hardware handshake control.
     */
    if (rts_control) {
        if (flags & UART_RXDISABLED) {
            sbi(UART_RTS_PORT, UART_RTS_BIT);
        }
        if (flags & UART_RXENABLED) {
            cbi(UART_RTS_PORT, UART_RTS_BIT);
        }
    }
    if (flags & UART_RTSDISABLED) {
        sbi(UART_RTS_PORT, UART_RTS_BIT);
    }
    if (flags & UART_RTSENABLED) {
        cbi(UART_RTS_PORT, UART_RTS_BIT);
    }
#endif

#ifdef UART_DTR_BIT
	if ( flags & UART_DTRDISABLED ) {
		sbi(UART_DTR_DDR, UART_DTR_BIT);
		sbi(UART_DTR_PORT, UART_DTR_BIT);
	}
	if ( flags & UART_DTRENABLED ) {
		sbi(UART_DTR_DDR, UART_DTR_BIT);
		cbi(UART_DTR_PORT, UART_DTR_BIT);
	}
#endif

    /*
     * Process multidrop setting.
     */
    if (flags & UART_TXADDRFRAME) {
        tx_aframe = 1;
    }
    if (flags & UART_TXNORMFRAME) {
        tx_aframe = 0;
    }
#ifdef __AVR_ENHANCED__
    if (flags & UART_RXADDRFRAME) {
        sbi(UCSRnA, MPCM);
    }
    if (flags & UART_RXNORMFRAME) {
        cbi(UCSRnA, MPCM);
    }
#endif

    /*
     * Clear UART receive errors.
     */
    if (flags & UART_FRAMINGERROR) {
        rx_errors &= ~_BV(FE);
    }
    if (flags & UART_OVERRUNERROR) {
        rx_errors &= ~_BV(DOR);
    }
#ifdef __AVR_ENHANCED__
    if (flags & UART_PARITYERROR) {
        rx_errors &= ~_BV(UPE);
    }
#endif

    /*
     * Verify the result.
     */
    if ((AvrUsartGetStatus() & ~UART_ERRORS) != flags) {
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
static uint8_t AvrUsartGetClockMode(void)
{
    uint8_t rc = 0;

#ifdef __AVR_ENHANCED__
    if (bit_is_set(UCSRnC, UMSEL)) {
        rc |= UART_SYNC;
        if (bit_is_set(DDRE, 2)) {
            rc |= UART_MASTER;
        }
        if (bit_is_set(UCSRnC, UCPOL)) {
            rc |= UART_NCLOCK;
        }
    } else if (bit_is_set(UCSRnA, U2X)) {
        rc |= UART_HIGHSPEED;
    }
#endif

    return rc;
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
static int AvrUsartSetClockMode(uint8_t mode)
{
#ifdef __AVR_ENHANCED__
    AvrUsartDisable();

    /*
     * Handle synchronous mode.
     */
    if (mode & UART_SYNC) {
        if (mode & UART_MASTER) {
            /* Enable master mode. */
            sbi(DDRE, 2);
        } else {
            /* Disable master mode. */
            cbi(DDRE, 2);
        }
        if (mode & UART_NCLOCK) {
            /* Enable negated clock. */
            sbi(UCSRnC, UCPOL);
        } else {
            /* Disable negated clock. */
            cbi(UCSRnC, UCPOL);
        }
        /* Disable high speed. */
        cbi(UCSRnA, U2X);
        /* Enable synchronous mode. */
        sbi(UCSRnC, UMSEL);
    }

    /*
     * Handle asynchronous mode.
     */
    else {
        if (mode & UART_HIGHSPEED) {
            /* Enable high speed. */
            sbi(UCSRnA, U2X);
        } else {
            /* Disable high speed. */
            cbi(UCSRnA, U2X);
        }
        /* Disable negated clock. */
        cbi(UCSRnC, UCPOL);
        /* Disable synchronous mode. */
        cbi(UCSRnC, UMSEL);
    }
    AvrUsartEnable();
#endif

    /*
     * Verify the result.
     */
    if (AvrUsartGetClockMode() != mode) {
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
static uint32_t AvrUsartGetFlowControl(void)
{
    uint32_t rc = 0;

    if (flow_control) {
        rc |= USART_MF_XONXOFF;
    } else {
        rc &= ~USART_MF_XONXOFF;
    }

#ifdef UART_RTS_BIT
    if (rts_control) {
        rc |= USART_MF_RTSCONTROL;
    } else {
        rc &= ~USART_MF_RTSCONTROL;
    }
#endif

#ifdef UART_CTS_BIT
    if (cts_sense) {
        rc |= USART_MF_CTSSENSE;
    } else {
        rc &= ~USART_MF_CTSSENSE;
    }
#endif

#ifdef UART_HDX_BIT
    if (hdx_control) {
        rc |= USART_MF_HALFDUPLEX;
    } else {
        rc &= ~USART_MF_HALFDUPLEX;
    }
#endif

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
static int AvrUsartSetFlowControl(uint32_t flags)
{
    /*
     * Set software handshake mode.
     */
    if (flags & USART_MF_XONXOFF) {
        if(flow_control == 0) {
            NutEnterCritical();
            flow_control = 1 | XOFF_SENT;  /* force XON to be sent on next read */
            NutExitCritical();
        }
    } else {
        NutEnterCritical();
        flow_control = 0;
        NutExitCritical();
    }

#ifdef UART_RTS_BIT
    /*
     * Set RTS control mode.
     */
    if (flags & USART_MF_RTSCONTROL) {
        sbi(UART_RTS_PORT, UART_RTS_BIT);
        sbi(UART_RTS_DDR, UART_RTS_BIT);
        rts_control = 1;
    } else if (rts_control) {
        rts_control = 0;
        cbi(UART_RTS_DDR, UART_RTS_BIT);
    }
#endif

#ifdef UART_CTS_BIT
    /*
     * Set CTS sense mode.
     */
    if (flags & USART_MF_CTSSENSE) {
        /* Register CTS sense interrupt. */
        if (NutRegisterIrqHandler(&UART_CTS_SIGNAL, AvrUsartCts, 0)) {
            return -1;
        }
        sbi(UART_CTS_PORT, UART_CTS_BIT);
        cbi(UART_CTS_DDR, UART_CTS_BIT);
        cts_sense = 1;
    } else if (cts_sense) {
        cts_sense = 0;
        /* Deregister CTS sense interrupt. */
        NutRegisterIrqHandler(&UART_CTS_SIGNAL, 0, 0);
        cbi(UART_CTS_DDR, UART_CTS_BIT);
    }
#endif

#ifdef UART_HDX_BIT
    /*
     * Set half duplex mode.
     */
    if (flags & USART_MF_HALFDUPLEX) {
        /* Register transmit complete interrupt. */
        if (NutRegisterIrqHandler(&sig_UART_TRANS, AvrUsartTxComplete, &dcb_usart.dcb_tx_rbf)) {
            return -1;
        }
        /* Initially enable the receiver. */
        UART_HDX_RX(UART_HDX_PORT, UART_HDX_BIT);
        sbi(UART_HDX_DDR, UART_HDX_BIT);
        hdx_control = 1;
        /* Enable transmit complete interrupt. */
        sbi(UCSRnB, TXCIE);
    } else if (hdx_control) {
        hdx_control = 0;
        /* disable transmit complete interrupt */
        cbi(UCSRnB, TXCIE);
        /* Deregister transmit complete interrupt. */
        NutRegisterIrqHandler(&sig_UART_TRANS, 0, 0);
        cbi(UART_HDX_DDR, UART_HDX_BIT);
    }
#endif

    /*
     * Verify the result.
     */
    if (AvrUsartGetFlowControl() != flags) {
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
static void AvrUsartTxStart(void)
{
#ifdef UART_HDX_BIT
    if (hdx_control) {
        /* Enable half duplex transmitter. */
	UART_HDX_TX(UART_HDX_PORT, UART_HDX_BIT);
    }
#endif
    /* Enable transmit interrupts. */
    sbi(UCSRnB, UDRIE);
}

/*!
 * \brief Start the USART receiver hardware.
 *
 * The upper level USART driver will call this function through the
 * USARTDCB jump table each time it removed enough bytes from the
 * receive buffer. Enough means, that the number of bytes left in
 * the buffer is below the low watermark.
 */
static void AvrUsartRxStart(void)
{
    /*
     * Do any required software flow control.
     */
    if (flow_control && (flow_control & XOFF_SENT) != 0) {
        NutEnterCritical();
        if (inb(UCSRnA) & _BV(UDRE)) {
            outb(UDRn, ASCII_XON);
            flow_control &= ~XON_PENDING;
        } else {
            flow_control |= XON_PENDING;
        }
        flow_control &= ~(XOFF_SENT | XOFF_PENDING);
        NutExitCritical();
    }
#ifdef UART_RTS_BIT
    if (rts_control) {
        /* Enable RTS. */
        cbi(UART_RTS_PORT, UART_RTS_BIT);
    }
#endif
}

/*
 * \brief Initialize the USART hardware driver.
 *
 * This function is called during device registration by the upper level
 * USART driver through the USARTDCB jump table.
 *
 * \return 0 on success, -1 otherwise.
 */
static int AvrUsartInit(void)
{
#ifndef USE_USART
    /*
     * Register receive and transmit interrupts.
     */
    if (NutRegisterIrqHandler(&sig_UART_RECV, AvrUsartRxComplete, &dcb_usart.dcb_rx_rbf))
        return -1;
    if (NutRegisterIrqHandler(&sig_UART_DATA, AvrUsartTxEmpty, &dcb_usart.dcb_tx_rbf)) {
        NutRegisterIrqHandler(&sig_UART_RECV, 0, 0);
        return -1;
    }
#endif

#ifdef UART_RTS_BIT
	sbi(UART_RTS_DDR, UART_RTS_BIT);
#endif

#ifdef UART_DTR_BIT
	sbi(UART_DTR_DDR, UART_DTR_BIT);
#endif

    return 0;
}

/*
 * \brief Deinitialize the USART hardware driver.
 *
 * This function is called during device deregistration by the upper
 * level USART driver through the USARTDCB jump table.
 *
 * \return 0 on success, -1 otherwise.
 */
static int AvrUsartDeinit(void)
{

#ifndef USE_USART
    /* Deregister receive and transmit interrupts. */
    NutRegisterIrqHandler(&sig_UART_RECV, 0, 0);
    NutRegisterIrqHandler(&sig_UART_DATA, 0, 0);
#endif

    /*
     * Disabling flow control shouldn't be required here, because it's up
     * to the upper level to do this on the last close or during
     * deregistration.
     */
#ifdef UART_HDX_BIT
    /* Deregister transmit complete interrupt. */
    if (hdx_control) {
        hdx_control = 0;
        NutRegisterIrqHandler(&sig_UART_TRANS, 0, 0);
    }
#endif

#ifdef UART_CTS_BIT
    if (cts_sense) {
        cts_sense = 0;
        cbi(UART_CTS_DDR, UART_CTS_BIT);
        /* Deregister CTS sense interrupt. */
        NutRegisterIrqHandler(&UART_CTS_SIGNAL, 0, 0);
    }
#endif

#ifdef UART_RTS_BIT
    if (rts_control) {
        rts_control = 0;
        cbi(UART_RTS_DDR, UART_RTS_BIT);
    }
#endif

#ifdef UART_DTR_BIT
	cbi(UART_DTR_DDR, UART_DTR_BIT);
#endif

    return 0;
}

/*@}*/
