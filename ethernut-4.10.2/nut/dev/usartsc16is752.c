/*
 * Copyright (C) 2010 by Ulrich Prinz (uprinz2@netscape.net)
 * Copyright (C) 2009 by Rittal GmbH & Co. KG. All rights reserved.
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
 *
 * Revision 1.0  2009/04/13 ulrichprinz
 * First checkin, new twi driver for SC16IS752 dual usart chip
 * (currently SAM7X256 is tested only)
 *
 */


// #define PRINT_DEBUG
#if defined (PRINT_DEBUG)
#include <stdio.h>

/*

#define MYPRINT(fmt, ...) printf ("%s:%s(%d)\t" fmt "\n", \
   		                       __FILE__,__FUNCTION__,__LINE__, ##__VA_ARGS__);
*/
#define MYPRINT(fmt, ...) printf ("%s(%d)\t" fmt "\n", \
   		                       __FUNCTION__,__LINE__, ##__VA_ARGS__);


#else
#define MYPRINT(fmt, ...)
#endif

// #include <assert.h>
#include <cfg/clock.h>

#include <sys/atom.h>
#include <sys/event.h>
#include <sys/timer.h>

#include <dev/irqreg.h>

#include <dev/usartsc16is752.h>

#include <dev/twif.h>

/*!
 * \addtogroup xgNutArchArmAt91Us
 */
/*@{*/

#ifdef US_OVRE
#undef US_OVRE
#endif
#define US_OVRE                 0x00000020      /*!< \brief Overrun error */


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


int Sc16is752RegWrite(uint8_t dev, uint8_t ch, uint8_t reg, uint8_t val);
int Sc16is752RegRead(uint8_t dev, uint8_t ch, uint8_t reg, uint8_t *val);

static int setBaudrate(uint8_t dev, uint8_t ch, int baud);
int selectRegisterSet(uint8_t dev, uint8_t ch, uint8_t reg);
static uint8_t selectSpecialRegisterSet(uint8_t dev, uint8_t ch);
static uint8_t selectEnhancedRegisterSet(uint8_t dev, uint8_t ch);
static uint8_t selectDefaultRegisterSet(uint8_t dev, uint8_t ch);
static uint8_t switchEnhancedFunctions(uint8_t dev, uint8_t ch, uint8_t flag);
static uint8_t switchTcrTlrSelect(uint8_t dev, uint8_t ch, uint8_t flag);
static int Sc16is752RegRawRead(uint8_t dev, uint8_t ch, uint8_t reg, uint8_t *val);
static int Sc16is752RegRawWrite(uint8_t dev, uint8_t ch, uint8_t reg, uint8_t val);


/*!
 * \brief Receiver error flags.
 */
static ureg_t rx_errors;

/*!
 * \brief Enables software flow control if not equal zero.
 */
static uint_fast8_t flow_control;

/*!
 * \brief Transmit address frame, if not zero.
 */
//static uint_fast8_t tx_aframe;


//#if defined(UART_RTS_BIT) || defined(US_MODE_HWHANDSHAKE)
/*!
 * \brief Enables RTS control if not equal zero.
 *
 * This variable exists only if the hardware configuration defines a
 * port bit to control the RTS signal.
 */
//static uint_fast8_t rts_control;
//#endif

//#if defined(UART_CTS_BIT) || defined(US_MODE_HWHANDSHAKE)
/*!
 * \brief Enables CTS sense if not equal zero.
 *
 * This variable exists only if the hardware configuration defines a
 * port bit to sense the CTS signal.
 */
//static uint_fast8_t cts_sense;
//#endif

/** Incoming interrupt events */
static HANDLE irqEvents[DEV_MAX];
static NUTDEVICE *deviceList[DEV_MAX][CH_MAX];

/*
 * \brief USARTn transmitter ready interrupt handler.
 *
 * \param rbf Pointer to the transmitter ring buffer.
 */
static void Sc16is752UsartTxReady(uint8_t dev, uint8_t ch, RINGBUF *rbf)
{
    register uint8_t *cp = rbf->rbf_tail;
    uint8_t v;
//
//     /*
//      * Process pending software flow controls first.
//      */
//     if (flow_control & (XON_PENDING | XOFF_PENDING)) {
//         if (flow_control & XON_PENDING) {
//             outr(USARTn_BASE + US_THR_OFF, ASCII_XOFF);
//             flow_control |= XOFF_SENT;
//         } else {
//             outr(USARTn_BASE + US_THR_OFF, ASCII_XON);
//             flow_control &= ~XOFF_SENT;
//         }
//         flow_control &= ~(XON_PENDING | XOFF_PENDING);
//         return;
//     }
//
//     if (flow_control & XOFF_RCVD) {
//         /*
//          * If XOFF has been received, we disable the transmit interrupts
//          * and return without sending anything.
//          */
//         outr(USARTn_BASE + US_IDR_OFF, US_TXRDY);
//         return;
// 	}
//
    if (rbf->rbf_cnt) {
//
// #ifdef UART_CTS_BIT
//         /*
//          * If CTS has been disabled, we disable the transmit interrupts
//          * and return without sending anything.
//          */
//         if (cts_sense && bit_is_set(UART_CTS_PIN, UART_CTS_BIT)) {
//             outr(USARTn_BASE + US_IDR_OFF, US_TXRDY);
//             sbi(EIMSK, UART_CTS_BIT);
//             return;
//         }
// #endif
        rbf->rbf_cnt--;

//         /*
//          * Send address in multidrop mode.
//          */
//         if (tx_aframe) {
//             outr(USARTn_BASE + US_CR_OFF, US_SENDA);
//         }
//
        /*
         * Start transmission of the next character.
         */
		Sc16is752RegWrite(dev, ch, THR, *cp);
		MYPRINT("sended %c\n",*cp);
		NutMicroDelay(500);
        /*
         * Wrap around the buffer pointer if we reached its end.
         */
        if (++cp == rbf->rbf_last) {
            cp = rbf->rbf_start;
        }
        rbf->rbf_tail = cp;
        if (rbf->rbf_cnt == rbf->rbf_lwm) {
            NutEventPost(&rbf->rbf_que);
        }
    }

    /*
     * Nothing left to transmit, disable 'THR empty' interrupt.
     */
    else {
		Sc16is752RegRead(dev, ch, IER, &v);
		v &= ~(1<<1);
		Sc16is752RegWrite(dev, ch, IER, v);
        rbf->rbf_cnt = 0;
        NutEventPost(&rbf->rbf_que);
    }
}


/*
 * \brief USARTn receiver ready interrupt handler.
 *
 *
 * \param rbf Pointer to the receiver ring buffer.
 */

static void Sc16is752UsartRxReady(uint8_t dev, uint8_t ch, RINGBUF *rbf)
{
    register size_t cnt;
    uint8_t c, e;

    /*
     * We read the received character as early as possible to avoid overflows
     * caused by interrupt latency.
     */
    Sc16is752RegRead(dev, ch, RHR, &c);
//printf("r:%c\n", (char)c);
    /* Collect receiver errors. */
    Sc16is752RegRead(dev, ch, LSR, &e);

    rx_errors |= e & ((1<<3) | (1<<2) | (1<<1));  // Framing, parity, overrun error

//     /*
//      * Handle software handshake. We have to do this before checking the
//      * buffer, because flow control must work in write-only mode, where
//      * there is no receive buffer.
//      */
//     if (flow_control) {
//         /* XOFF character disables transmit interrupts. */
//         if (c == ASCII_XOFF) {
//             outr(USARTn_BASE + US_IDR_OFF, US_TXRDY);
//             flow_control |= XOFF_RCVD;
//             return;
//         }
//         /* XON enables transmit interrupts. */
//         else if (c == ASCII_XON) {
//             outr(USARTn_BASE + US_IER_OFF, US_TXRDY);
//             flow_control &= ~XOFF_RCVD;
//             return;
//         }
//     }

    /*
     * Check buffer overflow.
     */
    cnt = rbf->rbf_cnt;
    if (cnt >= rbf->rbf_siz) {
        rx_errors |= US_OVRE;
        return;
    }

    /* Wake up waiting threads if this is the first byte in the buffer. */
    if (cnt++ == 0){
        NutEventPost(&rbf->rbf_que);
    }

//     /*
//      * Check the high watermark for software handshake. If the number of
//      * buffered bytes is above this mark, then send XOFF.
//      */
//     else if (flow_control) {
//         if(cnt >= rbf->rbf_hwm) {
//             if((flow_control & XOFF_SENT) == 0) {
//                 if (inr(USARTn_BASE + US_CSR_OFF) & US_TXRDY) {
//                     outb(USARTn_BASE + US_THR_OFF, ASCII_XOFF);
//                     flow_control |= XOFF_SENT;
//                     flow_control &= ~XOFF_PENDING;
//                 } else {
//                     flow_control |= XOFF_PENDING;
//                 }
//             }
//         }
//     }
//
// #ifdef UART_RTS_BIT
//     /*
//      * Check the high watermark for hardware handshake. If the number of
//      * buffered bytes is above this mark, then disable RTS.
//      */
//     else if (rts_control && cnt >= rbf->rbf_hwm) {
//         sbi(UART_RTS_PORT, UART_RTS_BIT);
//     }
// #endif

    /*
     * Store the character and increment and the ring buffer pointer.
     */
    *rbf->rbf_head++ = c;
    if (rbf->rbf_head == rbf->rbf_last) {
        rbf->rbf_head = rbf->rbf_start;
    }

    /* Update the ring buffer counter. */
    rbf->rbf_cnt = cnt;
}



void Sc16is752UsartProcessChannelInterrupt(uint8_t dev, uint8_t ch, uint8_t iir)
{
    USARTDCB *dcb = deviceList[dev][ch]->dev_dcb;
    switch(iir)
    {
		case 0x6:   // (0b00 0110) receiver line status Overrun Error (OE), Framing Error (FE), Parity Error
				// (PE), or Break Interrupt (BI) errors occur in characters in the RX FIFO
			break;
		case 0xc:   // (0b00 1100) RX time-out stale data in RX FIFO
		//	break;
		case 0x4:   // (0b00 0100) RHR interrupt receive data ready (FIFO disable) or
				// RX FIFO above trigger level (FIFO enable)
			Sc16is752UsartRxReady(dev, ch, &dcb->dcb_rx_rbf);
			break;
		case 0x2:   // (0b00 0010) THR interrupt transmit FIFO empty (FIFO disable) or
				// TX FIFO passes above trigger level (FIFO enable)
			Sc16is752UsartTxReady(dev, ch, &dcb->dcb_tx_rbf);
			break;
		case 0x0:   // (0b00 0000) modem status change of state of modem input pins
			break;
		case 0xe:   // (0b00 1110) I/O pins input pins change of state
			break;
		case 0x10:  // (0b01 0000) Xoff interrupt receive Xoff character(s)/special character
			break;
		case 0x20:  // (0b10 0000) CTS, RTS RTS pin or CTS pin change state from active (LOW)
				//to inactive (HIGH)
			break;
    }
}

/**
 * UART Interrupt Processing Thread
 */
THREAD(Sc16is752UsartInterruptProcessing, arg)
{
    uint8_t dev = (uint8_t)(((int)arg)&0xff);
    uint8_t iir;
    NutThreadSetPriority(50);
    for(;;)
    {
		NutEventWait(&irqEvents[dev], NUT_WAIT_INFINITE);
		MYPRINT("irq proccess\n");
		Sc16is752RegRead(dev, 0, IIR, &iir);
		MYPRINT("1\n");
		if (!(iir&1))
		{
			MYPRINT("1\n");
			Sc16is752UsartProcessChannelInterrupt(dev, 0, iir);
		}
		Sc16is752RegRead(dev, 1, IIR, &iir);
		if (!(iir&1))
		{
			MYPRINT("1\n");
			Sc16is752UsartProcessChannelInterrupt(dev, 1, iir);
		}
    }
}



#if !defined(__linux__)
/*!
 * \brief USART interrupt handler.
 *
 * \param arg Pointer to the device specific control block.
 */
static void Sc16is752UsartInterrupt(void *arg)
{
     NUTDEVICE *nutDev= (NUTDEVICE *)arg;
     uint8_t dev = ((nutDev->dev_base)>>8) & 0xff;
	MYPRINT("irq\n");
     // Trigger interrupt processing thread (I2C access not allowed from ISR)
     NutEventPostFromIrq(&irqEvents[dev]);
}
#endif


/*!
 * \brief Carefully enable USART hardware functions.
 *
 * Always enabale transmitter and receiver, even on read-only or
 * write-only mode. So we can support software flow control.
 */
void Sc16is752UsartEnable(uint8_t dev, uint8_t ch)
{
#if defined(__linux__)
	printf("%s\n", __func__);
#endif
//     NutEnterCritical();
//
//     /* Enable UART receiver and transmitter. */
//     outr(USARTn_BASE + US_CR_OFF, US_RXEN | US_TXEN);
//
//     /* Enable UART receiver and transmitter interrupts. */
//     outr(USARTn_BASE + US_IER_OFF, US_RXRDY | US_TXRDY);
//     NutIrqEnable(&SIG_UART);
//
//     NutExitCritical();
}


/*!
 * \brief Carefully disable USART hardware functions.
 */
void Sc16is752UsartDisable(uint8_t dev, uint8_t ch)
{
#if defined(__linux__)
	printf("%s\n", __func__);
#endif
//     /*
//      * Disable USART interrupts.
//      */
//     NutEnterCritical();
//     outr(USARTn_BASE + US_IDR_OFF, 0xFFFFFFFF);
//     NutExitCritical();
//
//     /*
//      * Allow incoming or outgoing character to finish.
//      */
//     NutDelay(10);
//
//     /*
//      * Disable USART transmit and receive.
//      */
//     outr(USARTn_BASE + US_CR_OFF, US_RXDIS | US_TXDIS);
}

/*!
 * \brief Query the USART hardware for the selected speed.
 *
 * This function is called by ioctl function of the upper level USART
 * driver through the USARTDCB jump table.
 *
 * \return The currently selected baudrate.
 */
uint32_t Sc16is752UsartGetSpeed(uint8_t dev, uint8_t ch)
{
#if defined(__linux__)
	printf("%s\n", __func__);
#endif
//     ureg_t cs = inr(USARTn_BASE + US_MR_OFF);
//     uint32_t clk;
//
//     clk = NutClockGet(NUT_HWCLK_PERIPHERAL);
//     if ((cs & US_CLKS) == US_CLKS_MCK8) {
//         clk /= 8;
//     }
//     else if ((cs & US_CLKS) != US_CLKS_MCK) {
//         clk = 0;
//     }
//     return clk / (16UL * (inr(USARTn_BASE + US_BRGR_OFF) & 0xFFFF));
    return 0;
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
int Sc16is752UsartSetSpeed(uint32_t rate, uint8_t dev, uint8_t ch)
{
#if defined(__linux__)
	printf("%s\n", __func__);
#endif
    Sc16is752UsartDisable(dev, ch);
//     outr(USARTn_BASE + US_BRGR_OFF, (NutClockGet(NUT_HWCLK_PERIPHERAL) / (8 * (rate)) + 1) / 2);
    Sc16is752UsartEnable(dev, ch);

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
uint8_t Sc16is752UsartGetDataBits(uint8_t dev, uint8_t ch)
{
#if defined(__linux__)
	printf("%s\n", __func__);
#endif
//     ureg_t val = inr(USARTn_BASE + US_MR_OFF);
//
//     if ((val & US_PAR) == US_PAR_MULTIDROP) {
//         val = 9;
//     }
//     else {
//         val &= US_CHRL;
//         if (val == US_CHRL_5) {
//             val = 5;
//         }
//         else if (val == US_CHRL_6) {
//             val = 6;
//         }
//         else if (val == US_CHRL_7) {
//             val = 7;
//         }
//         else {
//             val = 8;
//         }
//     }
//     return (uint8_t)val;
    return 0;
}

/*!
 * \brief Set the USART hardware to the number of data bits.
 *
 * This function is called by ioctl function of the upper level USART
 * driver through the USARTDCB jump table.
 *
 * \return 0 on success, -1 otherwise.
 */
int Sc16is752UsartSetDataBits(uint8_t bits, uint8_t dev, uint8_t ch)
{
#if defined(__linux__)
	printf("%s\n", __func__);
#endif
//     ureg_t val = inr(USARTn_BASE + US_MR_OFF);
//
//     if (bits == 9) {
//         val &= ~US_PAR;
//         val |= US_PAR_MULTIDROP;
//     }
//     else {
//         val &= ~US_CHRL;
//         if (bits == 5) {
//             val |= US_CHRL_5;
//         }
//         else if (bits == 6) {
//             val |= US_CHRL_6;
//         }
//         else if (bits == 7) {
//             val |= US_CHRL_7;
//         }
//         else if (bits == 8) {
//             val |= US_CHRL_8;
//         }
//     }
//
//     At91UsartDisable();
//     outr(USARTn_BASE + US_MR_OFF, val);
//     At91UsartEnable();
//
//     /*
//      * Verify the result.
//      */
//     if (At91UsartGetDataBits() != bits) {
//         return -1;
//     }
    return 0;
}

/*!
 * \brief Query the USART hardware for the parity mode.
 *
 * This routine is called by ioctl function of the upper level USART
 * driver through the USARTDCB jump table.
 *
 * \return Parity mode, either 0 (disabled), 1 (odd), 2 (even) or 9 (multidrop).
 */
uint8_t Sc16is752UsartGetParity(uint8_t dev, uint8_t ch)
{
#if defined(__linux__)
	printf("%s\n", __func__);
#endif
//     ureg_t val = inr(USARTn_BASE + US_MR_OFF) & US_PAR;
//
//     if ((val & US_PAR) == US_PAR_MULTIDROP) {
//         val = 9;
//     }
//     else {
//         if (val == US_PAR_ODD) {
//             val = 1;
//         }
//         else if (val == US_PAR_EVEN) {
//             val = 2;
//         }
//         else {
//             val = 0;
//         }
//     }
//     return (uint8_t)val;
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
int Sc16is752UsartSetParity(uint8_t mode, uint8_t dev, uint8_t ch)
{
#if defined(__linux__)
	printf("%s\n", __func__);
#endif
//     ureg_t val = inr(USARTn_BASE + US_MR_OFF) & ~US_PAR;
//
//     switch (mode) {
//     case 0:
//         val |= US_PAR_NO;
//         break;
//     case 1:
//         val |= US_PAR_ODD;
//         break;
//     case 2:
//         val |= US_PAR_EVEN;
//         break;
//     }
//     At91UsartDisable();
//     outr(USARTn_BASE + US_MR_OFF, val);
//     At91UsartEnable();
//
//     /*
//      * Verify the result.
//      */
//     if (At91UsartGetParity() != mode) {
//         return -1;
//     }
    return 0;
}

/*!
 * \brief Query the USART hardware for the number of stop bits.
 *
 * This routine is called by ioctl function of the upper level USART
 * driver through the USARTDCB jump table.
 *
 * \return The number of stop bits set, either 1, 2 or 3 (1.5 bits).
 */
uint8_t Sc16is752UsartGetStopBits(uint8_t dev, uint8_t ch)
{
#if defined(__linux__)
	printf("%s\n", __func__);
#endif
//     ureg_t val = inr(USARTn_BASE + US_MR_OFF) & US_NBSTOP;
//     if (val == US_NBSTOP_1) {
//         val = 1;
//     }
//     else if (val == US_NBSTOP_2) {
//         val = 2;
//     }
//     else {
//         val = 3;
//     }
//     return (uint8_t)val;
    return 0;
}

/*!
 * \brief Set the USART hardware to the number of stop bits.
 *
 * This routine is called by ioctl function of the upper level USART
 * driver through the USARTDCB jump table.
 *
 * \return 0 on success, -1 otherwise.
 */
int Sc16is752UsartSetStopBits(uint8_t bits, uint8_t dev, uint8_t ch)
{
#if defined(__linux__)
	printf("%s\n", __func__);
#endif
//     ureg_t val = inr(USARTn_BASE + US_MR_OFF) & ~US_NBSTOP;
//
//     switch(bits) {
//     case 1:
//         val |= US_NBSTOP_1;
//         break;
//     case 2:
//         val |= US_NBSTOP_2;
//         break;
//     case 3:
//         val |= US_NBSTOP_1_5;
//         break;
//     }
//     At91UsartDisable();
//     outr(USARTn_BASE + US_MR_OFF, val);
//     At91UsartEnable();
//
//     /*
//      * Verify the result.
//      */
//     if (At91UsartGetStopBits() != bits) {
//         return -1;
//     }
    return 0;
}

/*!
 * \brief Query the USART hardware status.
 *
 * \return Status flags.
 */
uint32_t Sc16is752UsartGetStatus(uint8_t dev, uint8_t ch)
{
    uint32_t rc = 0;
#if defined(__linux__)
	printf("%s\n", __func__);
#endif
//     /*
//      * Set receiver error flags.
//      */
//     if ((rx_errors & US_FRAME) != 0) {
//         rc |= UART_FRAMINGERROR;
//     }
//     if ((rx_errors & US_OVRE) != 0) {
//         rc |= UART_OVERRUNERROR;
//     }
//     if ((rx_errors & US_PARE) != 0) {
//         rc |= UART_PARITYERROR;
//     }
//
//     /*
//      * Determine software handshake status. The flow control status may
//      * change during interrupt, but this doesn't really hurt us.
//      */
//     if (flow_control) {
//         if (flow_control & XOFF_SENT) {
//             rc |= UART_RXDISABLED;
//         }
//         if (flow_control & XOFF_RCVD) {
//             rc |= UART_TXDISABLED;
//         }
//     }
//
//     /*
//      * Determine hardware handshake control status.
//      */
//
//     /*
//      * Determine hardware handshake sense status.
//      */
//
//     /*
//      * Determine hardware modem sense status.
//      */
//
//     /*
//      * If transmitter and receiver haven't been detected disabled by any
//      * of the checks above, then they are probably enabled.
//      */
//     if ((rc & UART_RXDISABLED) == 0) {
//         rc |= UART_RXENABLED;
//     }
//     if ((rc & UART_TXDISABLED) == 0) {
//         rc |= UART_TXENABLED;
//     }
//
//     /*
//      * Process multidrop setting.
//      */
//     if (tx_aframe) {
//         rc |= UART_TXADDRFRAME;
//     } else {
//         rc |= UART_TXNORMFRAME;
//     }
    return rc;
}

/*!
 * \brief Set the USART hardware status.
 *
 * \param flags Status flags.
 *
 * \return 0 on success, -1 otherwise.
 */
int Sc16is752UsartSetStatus(uint32_t flags, uint8_t dev, uint8_t ch)
{
#if defined(__linux__)
	printf("%s\n", __func__);
#endif
//     /*
//      * Process software handshake control.
//      */
//     if (flow_control) {
//
//         /* Access to the flow control status must be atomic. */
//         NutEnterCritical();
//
//         /*
//          * Enabling or disabling the receiver means to behave like
//          * having sent a XON or XOFF character resp.
//          */
//         if (flags & UART_RXENABLED) {
//             flow_control &= ~XOFF_SENT;
//         } else if (flags & UART_RXDISABLED) {
//             flow_control |= XOFF_SENT;
//         }
//
//         /*
//          * Enabling or disabling the transmitter means to behave like
//          * having received a XON or XOFF character resp.
//          */
//         if (flags & UART_TXENABLED) {
//             flow_control &= ~XOFF_RCVD;
//         } else if (flags & UART_TXDISABLED) {
//             flow_control |= XOFF_RCVD;
//         }
//         NutExitCritical();
//     }
//
//     /*
//      * Process hardware handshake control.
//      */
//
//     /*
//      * Process hardware modem control.
//      */
//
//     /*
//      * Process multidrop setting.
//      */
//     if (flags & UART_TXADDRFRAME) {
//         tx_aframe = 1;
//     }
//     if (flags & UART_TXNORMFRAME) {
//         tx_aframe = 0;
//     }
//
//     /*
//      * Clear UART receive errors.
//      */
//     if (flags & UART_ERRORS) {
//         outr(USARTn_BASE + US_CR_OFF, US_RSTSTA);
//     }
//
//     /*
//      * Verify the result.
//      */
//     if ((At91UsartGetStatus() & ~UART_ERRORS) != flags) {
//         return -1;
//     }
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
uint8_t Sc16is752UsartGetClockMode(uint8_t dev, uint8_t ch)
{
    uint8_t rc = 0;
#if defined(__linux__)
	printf("%s\n", __func__);
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
int Sc16is752UsartSetClockMode(uint8_t mode, uint8_t dev, uint8_t ch)
{
#if defined(__linux__)
	printf("%s\n", __func__);
#endif
    /*
     * Verify the result.
     */
    if (Sc16is752UsartGetClockMode(dev, ch) != mode) {
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
uint32_t Sc16is752UsartGetFlowControl(uint8_t dev, uint8_t ch)
{
    uint32_t rc = 0;
#if defined(__linux__)
	printf("%s\n", __func__);
#endif

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
int Sc16is752UsartSetFlowControl(uint32_t flags, uint8_t dev, uint8_t ch)
{
#if defined(__linux__)
	printf("%s\n", __func__);
#endif
    /*
     * Set software handshake mode.
     */


    /*
     * Verify the result.
     */
    if (Sc16is752UsartGetFlowControl(dev, ch) != flags) {
        return -1;
    }
    return 0;
}




//////////////////////////////////////////////////////////////////////////////

/*!
 * \brief Start the USART transmitter hardware.
 *
 * The upper level USART driver will call this function through the
 * USARTDCB jump table each time it added one or more bytes to the
 * transmit buffer.
 */
void Sc16is752UsartTxStart(uint8_t dev, uint8_t ch)
{
    uint8_t ier;
#if defined(__linux__)
	printf("%s\n", __func__);
#endif
    /* Enable transmit interrupts. */
    ///TODO Check if Tx interrupt is generated immediately or if a first char needs to sent
    Sc16is752RegRead(dev, ch, IER, &ier);
    ier |= (1<<1);
    Sc16is752RegWrite(dev, ch, IER, ier);
}


/*!
 * \brief Start the USART receiver hardware.
 *
 * The upper level USART driver will call this function through the
 * USARTDCB jump table each time it removed enough bytes from the
 * receive buffer. Enough means, that the number of bytes left in
 * the buffer is below the low watermark.
 */
void Sc16is752UsartRxStart(uint8_t dev, uint8_t ch)
{
#if defined(__linux__)
	printf("%s\n", __func__);
#endif
    /*
     * Do any required software flow control.
     */
//     if (flow_control && (flow_control & XOFF_SENT) != 0) {
//         NutEnterCritical();
//         if ((inr(USARTn_BASE + US_CSR_OFF) & US_TXRDY)) {
//             outr(USARTn_BASE + US_THR_OFF, ASCII_XON);
//             flow_control &= ~XON_PENDING;
//         } else {
//             flow_control |= XON_PENDING;
//         }
//         flow_control &= ~(XOFF_SENT | XOFF_PENDING);
//         NutExitCritical();
//     }
}



/*!
 * \brief Initialize the USART hardware driver.
 *
 * This function is called during device registration by the upper level
 * USART driver through the USARTDCB jump table.
 *
 * \return 0 on success, -1 otherwise.
 */
int Sc16is752UsartInit(uint8_t dev, uint8_t ch, NUTDEVICE *nutDev, IRQ_HANDLER *irq)
{

    deviceList[dev][ch] = nutDev; // needs to be stored here for interrupt processing
    uint8_t ier;

#if defined(__linux__)
	printf("%s\n", __func__);
#endif
	MYPRINT("1\n");
    /*
     * Register receive and transmit interrupts.
     */

    // Register IRQ1 for dev=0 and IRQ1 for dev=1 and ch=0
    // This requires to keep the init sequence: ch=0, then ch=1
    // Deinit must happen vice versa: first ch=1, then ch=0
    if (ch==0)
    {
		/* Start Message Handler Thread. */
		char threadname[]="usartirq0";
		threadname[8] = '0'+dev;  // Create unique name
		MYPRINT("1\n");
		if (NutThreadCreate(threadname, Sc16is752UsartInterruptProcessing, (void *)(int)dev, 512)==0)
			return -1;
		MYPRINT("1\n");
#if !defined(__linux__)
 		if (NutRegisterIrqHandler(irq, Sc16is752UsartInterrupt, nutDev))
 	    	return -1;
		NutIrqSetMode(irq, NUT_IRQMODE_FALLINGEDGE);
		MYPRINT("1\n");
 		NutIrqEnable(irq);
#endif
    }
#if 1
MYPRINT("1\n");
    /* Reset UART. */
//TOE  Sc16is752RegWrite(dev, ch, IOControl, (1<<3));  // SW Reset
    /* Enable receive interrupts. */
    ///TODO Check if Rx interrupt is generated immediately or if a first char needs to sent
    Sc16is752RegRead(dev, ch, IER, &ier);
	MYPRINT("ier:%d\n",ier);
    ier |= (1<<0);  // Enable Rx interrupt
	MYPRINT("ier:%d\n",ier);
    Sc16is752RegWrite(dev, ch, IER, ier);
MYPRINT("1\n");
    /* Set initial baudrate */
    setBaudrate(dev, ch, INIT_BAUDRATE);
MYPRINT("1\n");
    /* Set UART mode to 8 data bits, no parity and 1 stop bit. */
    Sc16is752RegWrite(dev, ch, LCR, 3);
	MYPRINT("1\n");
#endif
#if 0
{
uint8_t	reg;
	uint8_t val;

	/* This register causes trubble */
    /* Reset UART. */
//	buf[0] = ( IOControl << 3 );
//	buf[1] = 0x08;												// Software reset
//	TwMasterTransact( sla, buf, 2, 0, 0, 50 );

	reg = LCR & 0x1F;
	val = 0x80;												// eable special registers
	Sc16is752RegRawWrite( dev, ch, reg, val );

    /* Set initial baudrate */
	// div = 3; // XTAL / INIT_BAUDRATE / 16;

	reg = DLL & 0x1F;
	val = 3; // div & 0xff;
	Sc16is752RegRawWrite( dev, ch, reg, val );
	reg = DLH & 0x1F;
	val = 0; //div >> 8;											// set baud rate
	Sc16is752RegRawWrite( dev, ch, reg, val );

	/* Set 8N1 */
	reg = LCR & 0x1F;
	val = 0x03;												// 8N1
	Sc16is752RegRawWrite( dev, ch, reg, val );
    // ensure clock divisor is off

	reg = FCR & 0x1F;
	val = 0x01; 												// FIFO enable
	Sc16is752RegRawWrite( dev, ch, reg, val );

	reg = IER & 0x1F;
	val = 0x05; 												// FIFO enable
	Sc16is752RegRawWrite( dev, ch, reg, val );
	MYPRINT("val:%d\n",val);
}
#endif



    return 0;
}

/*!
 * \brief Deinitialize the USART hardware driver.
 *
 * This function is called during device deregistration by the upper
 * level USART driver through the USARTDCB jump table.
 *
 * \return 0 on success, -1 otherwise.
 */
int Sc16is752UsartDeinit(uint8_t dev, uint8_t ch, IRQ_HANDLER *irq)
{
#if defined(__linux__)
	printf("%s\n", __func__);
#endif
    /* Reset UART (this implicitely disables all interrupts). */
//TOE    Sc16is752RegWrite(dev, ch, IOControl, (1<<3));  // SW Reset

    /* Deregister receive and transmit interrupts. */
    // unregister interrupt,
    if (ch==0)
    {
#if !defined(__linux__)
 	NutRegisterIrqHandler(irq, 0, 0);
#endif
    }


    return 0;
}


//////////////
#if 1
int setBaudrate(uint8_t dev, uint8_t ch, int baud)
{
    uint8_t v;
#if defined(__linux__)
	printf("%s\n", __func__);
#endif
    // ensure clock divisor is off
    Sc16is752RegRead(dev, ch, MCR, &v);
    v &= ~(1<<8);
    Sc16is752RegWrite(dev, ch, MCR, v);
    // set baudrate

    int div = XTAL / baud / 16;
	Sc16is752RegRead(dev, ch, LCR, &v);

	Sc16is752RegWrite(dev, ch, LCR, 0x80);
    Sc16is752RegWrite(dev, ch, DLL, div & 0xff);
    Sc16is752RegWrite(dev, ch, DLH, div >> 8);
   	Sc16is752RegWrite(dev, ch, LCR, v);
	 return 0;
}
#endif

/*
 * I2C register access wrapper
 */


static regselstate_t regselState[DEV_MAX][CH_MAX] = { {{0,0}, {0,0}}, {{0,0}, {0,0}}};

static uint8_t i2caddr[DEV_MAX] =
{
    USART_DEVICE0_I2C_ADDR,
    USART_DEVICE1_I2C_ADDR
};



int Sc16is752RegWrite(uint8_t dev, uint8_t ch, uint8_t reg, uint8_t val)
{
    // check valid range of arguments
//     assert(dev<=DEV_MAX);
//     assert(ch<=CH_MAX);

	//selectRegisterSet(dev, ch, reg);
    // Now, perform the register access
    return Sc16is752RegRawWrite(dev, ch, reg, val);
}

int Sc16is752RegRead(uint8_t dev, uint8_t ch, uint8_t reg, uint8_t *val)
{
    // check valid range of arguments
//     assert(dev<=DEV_MAX);
//     assert(ch<=CH_MAX);

 //   selectRegisterSet(dev, ch, reg);
    // Now, perform the register access
    return Sc16is752RegRawRead(dev, ch, reg, val);
}



/*****
 * Some Utility functions to select the different register sets
 */


/**
 * Select the register set depending on the register address
 * The device can only address 16 registers directly.
 * The other registers are accessible indirectly only.
 */
int selectRegisterSet(uint8_t dev, uint8_t ch, uint8_t reg)
{
	MYPRINT("selectRegSet");
    switch(reg&REGSEL_MASK)
    {
	case DEFSEL:
	    if(((reg==MSR) || (reg==SPR)) && regselState[dev][ch].flags & TCRBIT)
	    {
			MYPRINT("switchTcrTlrSelect\n");
			// turn off TCR/TLR register mode
			switchTcrTlrSelect(dev, ch, 0);
	    }
	    else if(regselState[dev][ch].state != DEFSEL)
	    {
			MYPRINT("selectDefaultRegisterSet\n");
			// select default register set
			selectDefaultRegisterSet(dev, ch);
	    }
	case TCRSEL:
		MYPRINT("switchTcrTlrSelect\n");
	    // select TCR/TLR registers
	    switchTcrTlrSelect(dev, ch, 1);
	    break;
	case SRSSEL:
		MYPRINT("selectSpecialRegisterSet\n");
	    // select Special Registers (DLL/DLH)
	    selectSpecialRegisterSet(dev, ch);
	    break;
	case ERSSEL:
		MYPRINT("selectEnhancedRegisterSet\n");
	    // select Enhanced Registers (EFR; XONx, XOFFx)
	    selectEnhancedRegisterSet(dev, ch);
	    break;

    }
    return 0;
}


/**
 * Switches to Special Register Set
 * DLL and DLH are accessible ar register address 0 and 1
 * This operation modifies the content of the LCR register
 * The original content is passed as return value
 */
static uint8_t selectSpecialRegisterSet(uint8_t dev, uint8_t ch)
{
    uint8_t v;
    Sc16is752RegRawRead(dev, ch, LCR, &v);
    Sc16is752RegRawWrite(dev, ch, LCR, 0x80);
    regselState[dev][ch].state = SRSSEL;
    return v;
}

/**
 * Switches to Enhanced Register Set
 * Maps register EFR, XON1, XON2, XOFF1 and XOFF2
 * This operation modifies the content of the LCR register
 * The original content is passed as return value
 */
static uint8_t selectEnhancedRegisterSet(uint8_t dev, uint8_t ch)
{
    uint8_t v=0;
    Sc16is752RegRawRead(dev, ch, LCR, &v);
    Sc16is752RegRawWrite(dev, ch, LCR, 0xbf);
    regselState[dev][ch].state = ERSSEL;
    return v;
}

/**
 * Switches to Default Register Set
 * This operation modifies the content of the LCR register
 * The original content is passed as return value
 */
static uint8_t selectDefaultRegisterSet(uint8_t dev, uint8_t ch)
{
    uint8_t v=0;
    Sc16is752RegRawRead(dev, ch, LCR, &v);
    v &= 0x7f;   // turn off 'divisor latch enable' bit
    Sc16is752RegRawWrite(dev, ch, LCR, v);
    regselState[dev][ch].state = DEFSEL;
    return v;
}

/**
 * Turn on/off the enhanced functions bit in the EFR register
 * This requires temporarily selecting the enhanced register set
 * The original register set selection is restored via the original
 * content of the LCR register
 */
static uint8_t switchEnhancedFunctions(uint8_t dev, uint8_t ch, uint8_t flag)
{
    uint8_t oldLcr;
    uint8_t v=0;
    oldLcr = selectEnhancedRegisterSet(dev, ch);
    Sc16is752RegRawRead(dev, ch, EFR, &v);
    if(flag)
    {
		v |= (1<<4);   // switch on enhanced functions bit
		regselState[dev][ch].flags |= EEFBIT;
    }
    else
    {
		v &= ~(1<<4);   // switch off enhanced functions bit
		regselState[dev][ch].flags &= ~EEFBIT;
    }
    Sc16is752RegRawWrite(dev, ch, EFR, v);
    Sc16is752RegRawWrite(dev, ch, LCR, oldLcr);  // select original register set
    return 0;
}


static uint8_t switchTcrTlrSelect(uint8_t dev, uint8_t ch, uint8_t flag)
{
    uint8_t v=0;
    switchEnhancedFunctions(dev, ch, 1);
    Sc16is752RegRawRead(dev, ch, MCR, &v);
    if(flag)
    {
		v |= (1<<2);   // switch on TCR/TLR enable
		regselState[dev][ch].flags |= TCRBIT;
    }
    else
    {
		v &= ~(1<<2);   // switch off TCR/TLR enable
		regselState[dev][ch].flags &= ~TCRBIT;
    }
    Sc16is752RegRawWrite(dev, ch, MCR, v);
    return 0;
}


static int Sc16is752RegRawWrite(uint8_t dev, uint8_t ch, uint8_t reg, uint8_t val)
{
    uint8_t wbuf[2];
    wbuf[0] = REGADDR(reg & 0xf, ch);
    wbuf[1] = val;
#if defined(__linux__)
    printf("%s(%d,%d,0x%02x,0x%02x) => slave addr:0x%02x data 0x%02x\n",
		 __func__, dev, ch, reg, val, i2caddr[dev], wbuf[0]);
	return 0;
#else
	 MYPRINT("%s(%d,%d,0x%02x,0x%02x) => slave addr:0x%02x data 0x%02x\n",
		 __func__, dev, ch, reg, val, i2caddr[dev], wbuf[0]);
#endif
    return TwMasterTransact(i2caddr[dev], wbuf, 2, 0, 0, 0);
}

static int Sc16is752RegRawRead(uint8_t dev, uint8_t ch, uint8_t reg, uint8_t *val)
{
    uint8_t wbuf, rc;

    wbuf = REGADDR(reg & 0xf, ch);
#if defined(__linux__)
    printf("%s(%d,%d,0x%02x) => slave addr:0x%02x data 0x%02x\n",
		 __func__, dev, ch, reg, i2caddr[dev], wbuf);
	return 0;
#else
  //  return TwMasterTransact(i2caddr[dev], wbuf, 1, val, 1, 0);
	 rc = TwMasterRegRead (i2caddr[dev], wbuf, 1, val, 1, 0);
	 MYPRINT("%s(%d,%d,0x%02x,0x%02x) => slave addr:0x%02x data 0x%02x\n",
		 __func__, dev, ch, reg, *val, i2caddr[dev], wbuf);
	return rc;
#endif
}


/*@}*/
