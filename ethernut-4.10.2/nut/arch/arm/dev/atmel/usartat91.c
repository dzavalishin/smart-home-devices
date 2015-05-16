/*
 * Copyright 2001-2006 by egnite Software GmbH
 * Copyright 2009 by egnite GmbH
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

/*
 * $Id: usartat91.c 3449 2011-05-31 19:08:15Z mifi $
 *
 * Revision 1.01  2009/09/20 ulrichprinz
 * Added support for using DBGU as limited standard USART.
 */

#include <cfg/clock.h>

#ifdef UART_USES_NPL
#include <dev/npl.h>
#endif

#include <sys/atom.h>
#include <sys/event.h>
#include <sys/timer.h>

#include <dev/irqreg.h>

#include <dev/usartat91.h>

/*
** Initial USART settings.
*/
#ifndef UART_INIT_BAUDRATE
#define UART_INIT_BAUDRATE  115200
#endif

/*
** Add empty placeholders for undefined pin configurations.
*/
#ifndef UART_RXTX_PINS_ENABLE
#define UART_RXTX_PINS_ENABLE()
#endif

#ifndef UART_HDX_PIN_ENABLE
#define UART_HDX_PIN_ENABLE()
#endif
#ifndef UART_HDX_RX
#define UART_HDX_RX()
#endif
#ifndef UART_HDX_TX
#define UART_HDX_TX()
#endif

#ifndef UART_RTS_PIN_ENABLE
#define UART_RTS_PIN_ENABLE()
#endif
#ifndef UART_RTS_ON
#define UART_RTS_ON()
#endif
#ifndef UART_RTS_OFF
#define UART_RTS_OFF()
#endif

#ifndef UART_CTS_PIN_ENABLE
#define UART_CTS_PIN_ENABLE()
#endif
#ifndef UART_CTS_IS_ON
#define UART_CTS_IS_ON()    (1)
#endif

#ifndef UART_MODEM_PINS_ENABLE
#define UART_MODEM_PINS_ENABLE()
#endif


/*
** GPIO controlled half duplex pin.
*/
#if defined(UART_HDX_BIT) && defined(UART_HDX_PIO_ID)

#undef UART_HDX_RX
#undef UART_HDX_TX
#undef UART_HDX_PIN_ENABLE
#undef GPIO_ID
#define GPIO_ID UART_HDX_PIO_ID
#include <cfg/arch/porttran.h>
static INLINE void UART_HDX_PIN_ENABLE(void) {
    GPIO_ENABLE(UART_HDX_BIT);
    GPIO_OUTPUT(UART_HDX_BIT);
}
#ifdef UART_HDX_FLIP_BIT
static INLINE void UART_HDX_RX(void) { GPIO_SET_HI(UART_HDX_BIT); }
static INLINE void UART_HDX_TX(void) { GPIO_SET_LO(UART_HDX_BIT); }
#else
static INLINE void UART_HDX_RX(void) { GPIO_SET_LO(UART_HDX_BIT); }
static INLINE void UART_HDX_TX(void) { GPIO_SET_HI(UART_HDX_BIT); }
#endif

#elif defined(UART_USES_NPL)

#undef UART_HDX_RX
#undef UART_HDX_TX
#if US_ID == US1_ID
#define UART_HDX_RX()   if (inr(NPL_RSCR) & NPL_RSUS1P) sbi(NPL_RSCR, NPL_RSRTS_BIT)
#define UART_HDX_TX()   if (inr(NPL_RSCR) & NPL_RSUS1P) cbi(NPL_RSCR, NPL_RSRTS_BIT)
#else
#define UART_HDX_RX()   if ((inr(NPL_RSCR) & NPL_RSUS1P) == 0) sbi(NPL_RSCR, NPL_RSRTS_BIT)
#define UART_HDX_TX()   if ((inr(NPL_RSCR) & NPL_RSUS1P) == 0) cbi(NPL_RSCR, NPL_RSRTS_BIT)
#endif

#endif /* UART_HDX_BIT */

/*
** GPIO controlled RTS pin.
*/
#if defined(UART_RTS_BIT) && defined(UART_RTS_PIO_ID)

#undef UART_RTS_ON
#undef UART_RTS_OFF
#undef UART_RTS_PIN_ENABLE
#undef GPIO_ID
#define GPIO_ID UART_RTS_PIO_ID
#include <cfg/arch/porttran.h>
static INLINE void UART_RTS_PIN_ENABLE(void) { \
    GPIO_ENABLE(UART_RTS_BIT); \
    GPIO_OUTPUT(UART_RTS_BIT); \
}
#ifdef UART_RTS_FLIP_BIT
static INLINE void UART_RTS_ON(void) { GPIO_SET_HI(UART_RTS_BIT); }
static INLINE void UART_RTS_OFF(void) { GPIO_SET_LO(UART_RTS_BIT); }
#else
static INLINE void UART_RTS_ON(void) { GPIO_SET_LO(UART_RTS_BIT); }
static INLINE void UART_RTS_OFF(void) { GPIO_SET_HI(UART_RTS_BIT); }
#endif

#elif defined(UART_USES_NPL)

#undef UART_RTS_ON
#undef UART_RTS_OFF
#if US_ID == US1_ID
#define UART_RTS_ON()   if (inr(NPL_RSCR) & NPL_RSUS1P) sbi(NPL_RSCR, NPL_RSRTS_BIT)
#define UART_RTS_OFF()  if (inr(NPL_RSCR) & NPL_RSUS1P) cbi(NPL_RSCR, NPL_RSRTS_BIT)
#else
#define UART_RTS_ON()   if ((inr(NPL_RSCR) & NPL_RSUS1P) == 0) sbi(NPL_RSCR, NPL_RSRTS_BIT)
#define UART_RTS_OFF()  if ((inr(NPL_RSCR) & NPL_RSUS1P) == 0) cbi(NPL_RSCR, NPL_RSRTS_BIT)
#endif
#endif

/*
** GPIO monitored CTS pin.
*/
#if defined(UART_CTS_BIT) && defined(UART_CTS_PIO_ID)

#undef UART_CTS_IS_ON
#undef UART_CTS_PIN_ENABLE
#undef GPIO_ID
#define GPIO_ID UART_CTS_PIO_ID
#include <cfg/arch/porttran.h>
#ifdef UART_CTS_FLIP_BIT
static INLINE int UART_CTS_IS_ON(void) { return GPIO_GET(UART_CTS_BIT); }
#else
static INLINE int UART_CTS_IS_ON(void) { return !GPIO_GET(UART_CTS_BIT); }
#endif
static INLINE void UART_CTS_PIN_ENABLE(void) { \
    GPIO_ENABLE(UART_CTS_BIT); \
    GPIO_INPUT(UART_CTS_BIT); \
    GPIO_PULLUP_ON(UART_CTS_BIT); \
}

#elif defined(UART_USES_NPL)

#undef UART_CTS_IS_ON
static INLINE int UART_CTS_IS_ON(void) {
    outw(NPL_SCR, NPL_RSCTS);
    return bit_is_set(NPL_SLR, NPL_RSCTS_BIT);
}

#endif

/*
** Determine if we use built-in hardware handshake.
*/
#if !defined(USE_BUILT_IN_HARDWARE_HANDSHAKE)
#if defined(UART_HARDWARE_HANDSHAKE) && \
    defined(US_MODE_HWHANDSHAKE) && \
    !defined(UART_RTS_BIT) && \
    !defined(UART_CTS_BIT) && \
    !defined(UART_USES_NPL)
#define USE_BUILT_IN_HARDWARE_HANDSHAKE 1
#else
#define USE_BUILT_IN_HARDWARE_HANDSHAKE 0
#endif
#endif

/*
** Determine if we use built-in half duplex mode.
*/
#if !defined(USE_BUILT_IN_HALF_DUPLEX)
#if defined(UART_HARDWARE_HANDSHAKE) && !defined(UART_HDX_BIT) && !defined(UART_USES_NPL)
#define USE_BUILT_IN_HALF_DUPLEX    1
#else
#define USE_BUILT_IN_HALF_DUPLEX    0
#endif
#endif

/* Some function renaming for IRQ handling on uarts
 * that do not have a separate IRQ instance but a
 * merged one with others like DBU on SYS-IRQ or
 * external uarts on GPIO-IRQs
 */
#if (US_ID==SYSC_ID)
#define NutUartIrqRegister  NutRegisterSysIrqHandler
#define NutUartIrqEnable    NutSysIrqEnable
#define NutUartIrqDisable   NutSysIrqDisable
#else
#define NutUartIrqRegister  NutRegisterIrqHandler
#define NutUartIrqEnable    NutIrqEnable
#define NutUartIrqDisable   NutIrqDisable
#endif

/*!
 * \addtogroup xgNutArchArmAt91Us
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
static unsigned int rx_errors;

/*!
 * \brief Enables software flow control if not equal zero.
 */
static uint_fast8_t flow_control;

/*!
 * \brief Transmit address frame, if not zero.
 */
static uint_fast8_t tx_aframe;

/*!
 * \brief Half duplex control flag.
 *
 * Set to 1 if half duplex is enabled. Contains zero otherwise.
 */
static uint_fast8_t hdx_control;

/*!
 * \brief Enables RTS control if not equal zero.
 *
 * This variable exists only if the hardware configuration defines a
 * port bit to control the RTS signal.
 */
static uint_fast8_t rts_control;

/*!
 * \brief Enables CTS sense if not equal zero.
 *
 * This variable exists only if the hardware configuration defines a
 * port bit to sense the CTS signal.
 */
static uint_fast8_t cts_sense;

#if defined(UART_CTS_BIT) || defined(UART_USES_NPL)
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
static void At91UsartCts(void *arg)
{
    /* Disable CTS sense interrupt. */
#if defined(UART_USES_NPL)
    NplIrqDisable(&sig_RSCTS);
    /* Enable transmit interrupt. */
    outr(USARTn_BASE + US_IER_OFF, US_TXRDY);
#else
    if (UART_CTS_IS_ON()) {
        GpioIrqDisable(&UART_CTS_SIGNAL, UART_CTS_BIT);
        /* Enable transmit interrupt. */
        outr(USARTn_BASE + US_IER_OFF, US_TXRDY);
    }
#endif
}
#endif

#if USE_BUILT_IN_HALF_DUPLEX == 0
/*
 * \brief USARTn transmitter empty interrupt handler.
 *
 * Used with half duplex communication to switch from tranmit to receive
 * mode after the last character has been transmitted.
 *
 * \param arg Pointer to the transmitter ring buffer.
 */
static void At91UsartTxEmpty(RINGBUF *rbf)
{
    /*
     * Check if half duplex mode has been enabled and if all characters
     * had been sent out.
     */
    if (hdx_control && (rbf->rbf_cnt == 0)) {
        /*
         * Switch to receiver mode:
         * Enable USART receive, disable transmit.
         * Disable TX-Empty IRQ
         */
        outr(USARTn_BASE + US_CR_OFF, US_RXEN | US_TXDIS);
#if defined(UART_HDX_BIT) || defined(UART_USES_NPL)
        UART_HDX_RX();
#endif
    }
}
#endif

/*
 * \brief USARTn transmitter ready interrupt handler.
 *
 * \param rbf Pointer to the transmitter ring buffer.
 */
static void At91UsartTxReady(RINGBUF *rbf)
{
    register uint8_t *cp = rbf->rbf_tail;

    /*
     * Process pending software flow controls first.
     */
    if (flow_control & (XON_PENDING | XOFF_PENDING)) {
        if (flow_control & XON_PENDING) {
            outr(USARTn_BASE + US_THR_OFF, ASCII_XOFF);
            flow_control |= XOFF_SENT;
        } else {
            outr(USARTn_BASE + US_THR_OFF, ASCII_XON);
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
        outr(USARTn_BASE + US_IDR_OFF, US_TXRDY);
        return;
    }

    /*
     * Check if we have more bytes to transmit.
     */
    if (rbf->rbf_cnt) {
#if defined(UART_CTS_BIT) || defined(UART_USES_NPL)
        /*
         * If CTS has been disabled, we disable the transmit interrupts,
         * enable CTS interrupts and return without sending anything.
         */
        if (cts_sense && !UART_CTS_IS_ON()) {
            outr(USARTn_BASE + US_IDR_OFF, US_TXRDY);
#if defined(UART_CTS_BIT)
            GpioIrqEnable(&UART_CTS_SIGNAL, UART_CTS_BIT);
#else
            NplIrqEnable(&sig_RSCTS);
#endif
            return;
        }
#endif

        /*
         * Send address in multidrop mode.
         */
        if (tx_aframe) {
            outr(USARTn_BASE + US_CR_OFF, US_SENDA);
        }

        /* Start transmission of the next character. */
        outr(USARTn_BASE + US_THR_OFF, *cp);
        /* Decrement the number of available bytes in the buffer. */
        rbf->rbf_cnt--;
        /* Wrap around the buffer pointer if we reached its end. */
        if (++cp == rbf->rbf_last) {
            cp = rbf->rbf_start;
        }
        rbf->rbf_tail = cp;

#if USE_BUILT_IN_HALF_DUPLEX == 0
        /*
         * If software half duplex enabled, we need TX-Empty IRQ for
         * detection if last bit of last byte transmission is finished.
         */
        if( hdx_control && rbf->rbf_cnt == 0) {
            outr(USARTn_BASE + US_IER_OFF, US_TXEMPTY);
        }
#endif

        /* Send an event if we reached the low watermark. */
        if (rbf->rbf_cnt == rbf->rbf_lwm) {
            NutEventPostFromIrq(&rbf->rbf_que);
        }
    }

    /*
     * Nothing left to transmit, disable interrupt.
     */
    else {
        /* Disable transmit interrupts. */
        outr(USARTn_BASE + US_IDR_OFF, US_TXRDY);
        /* Send an event to inform the upper level. */
        NutEventPostFromIrq(&rbf->rbf_que);
    }
}


/*
 * \brief USARTn receiver ready interrupt handler.
 *
 *
 * \param rbf Pointer to the receiver ring buffer.
 */

static void At91UsartRxReady(RINGBUF *rbf)
{
    register size_t cnt;
    register uint8_t ch;

    /*
     * We read the received character as early as possible to avoid overflows
     * caused by interrupt latency.
     */
    ch = inb(USARTn_BASE + US_RHR_OFF);

    /* Collect receiver errors. */
    rx_errors |= inr(USARTn_BASE + US_CSR_OFF) & (US_OVRE | US_FRAME | US_PARE);

    /*
     * Handle software handshake. We have to do this before checking the
     * buffer, because flow control must work in write-only mode, where
     * there is no receive buffer.
     */
    if (flow_control) {
        /* XOFF character disables transmit interrupts. */
        if (ch == ASCII_XOFF) {
            outr(USARTn_BASE + US_IDR_OFF, US_TXRDY);
            flow_control |= XOFF_RCVD;
            return;
        }
        /* XON enables transmit interrupts. */
        else if (ch == ASCII_XON) {
            outr(USARTn_BASE + US_IER_OFF, US_TXRDY);
            flow_control &= ~XOFF_RCVD;
            return;
        }
    }

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
        NutEventPostFromIrq(&rbf->rbf_que);
    }

    /*
     * Check the high watermark for software handshake. If the number of
     * buffered bytes is equal or above this mark, then send XOFF.
     */
    else if (flow_control) {
        if(cnt >= rbf->rbf_hwm) {
            if((flow_control & XOFF_SENT) == 0) {
                if (inr(USARTn_BASE + US_CSR_OFF) & US_TXRDY) {
                    outb(USARTn_BASE + US_THR_OFF, ASCII_XOFF);
                    flow_control |= XOFF_SENT;
                    flow_control &= ~XOFF_PENDING;
                } else {
                    flow_control |= XOFF_PENDING;
                }
            }
        }
    }

#if defined(UART_RTS_BIT) || defined(UART_USES_NPL)
    /*
     * Check the high watermark for GPIO hardware handshake. If the
     * number of buffered bytes is equal or above this mark, then
     * disable RTS.
     */
    else if (rts_control && cnt >= rbf->rbf_hwm) {
        UART_RTS_OFF();
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
}

/*!
 * \brief USART interrupt handler.
 *
 * \param arg Pointer to the device specific control block.
 */
static void At91UsartInterrupt(void *arg)
{
    USARTDCB *dcb = (USARTDCB *)arg;
    unsigned int csr = inr(USARTn_BASE + US_CSR_OFF);

    if (csr & US_RXRDY) {
        At91UsartRxReady(&dcb->dcb_rx_rbf);
    }
    if (csr & US_TXRDY) {
        At91UsartTxReady(&dcb->dcb_tx_rbf);
    }

#if USE_BUILT_IN_HALF_DUPLEX == 0
    if (csr & US_TXEMPTY) {
        At91UsartTxEmpty(&dcb->dcb_tx_rbf);
    }
#endif
}

/*!
 * \brief Carefully enable USART hardware functions.
 *
 * Always enabale transmitter and receiver, even on read-only or
 * write-only mode. So we can support software flow control.
 */
static void At91UsartEnable(void)
{
    /* Enable UART receiver and transmitter. */
    outr(USARTn_BASE + US_CR_OFF, US_RXEN | US_TXEN);
    /* Globally enable UART interrupts. */
    NutUartIrqEnable(&SIG_UART);
}

/*!
 * \brief Carefully disable USART hardware functions.
 *
 * This routine is called before changing cruical settings like
 * baudrate, frame format etc.
 *
 * The previous version uses a 10ms delay to make sure, that any
 * incoming or outgoing character is processed. However, this time
 * depends on the baudrate.
 *
 * In fact we do not need to take care of incoming characters,
 * when changing such settings.
 *
 * For outgoing characters however, settings may be changed on
 * the fly and we should wait, until the last character transmitted
 * with the old settings has left the shift register. While TXRDY
 * is set when the holding register is empty, TXEMPTY is set when the
 * shift register is empty. The bad news is, that both are zero, if
 * the transmitter is disabled. We are not able to determine this
 * state. So we check TXRDY first and, if set, wait for any character
 * currently in the shift register.
 */
static void At91UsartDisable(void)
{
    /* Globally disable UART interrupts. */
    NutUartIrqDisable(&SIG_UART);
    /* Wait until all bits had been shifted out. */
    if (inr(USARTn_BASE + US_CSR_OFF) & US_TXRDY) {
        while((inr(USARTn_BASE + US_CSR_OFF) & US_TXEMPTY) == 0);
    }
    /* Disable USART transmit and receive. */
    outr(USARTn_BASE + US_CR_OFF, US_RXDIS | US_TXDIS);
}

/*!
 * \brief Query the USART hardware for the selected speed.
 *
 * This function is called by ioctl function of the upper level USART
 * driver through the USARTDCB jump table.
 *
 * \return The currently selected baudrate.
 */
static uint32_t At91UsartGetSpeed(void)
{
    unsigned int cs;
    uint32_t clk;

    clk = NutClockGet(NUT_HWCLK_PERIPHERAL);
    cs = inr(USARTn_BASE + US_MR_OFF) & US_CLKS;
    if (cs == US_CLKS_MCK8) {
        clk /= 8;
    }
    else if (cs != US_CLKS_MCK) {
        clk = 0;
    }
    return clk / (16UL * (inr(USARTn_BASE + US_BRGR_OFF) & 0xFFFF));
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
static int At91UsartSetSpeed(uint32_t rate)
{
    At91UsartDisable();
#if !(USARTn_BASE == DBGU_BASE)
    outr(USARTn_BASE + US_BRGR_OFF, (NutClockGet(NUT_HWCLK_PERIPHERAL) / (8 * (rate)) + 1) / 2);
#else
    #if defined(AT91_PLL_MAINCK)
        outr(DBGU_BRGR, (At91GetMasterClock() / (8 * rate) + 1) / 2);
    #else
        outr(DBGU_BRGR, (NutGetCpuClock() / (8 * rate) + 1) / 2);
    #endif
#endif
    At91UsartEnable();
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
static uint8_t At91UsartGetDataBits(void)
{
#if !(USARTn_BASE == DBGU_BASE)
    unsigned int val = inr(USARTn_BASE + US_MR_OFF);

    if ((val & US_PAR) == US_PAR_MULTIDROP) {
        val = 9;
    }
    else {
        val &= US_CHRL;
        if (val == US_CHRL_5) {
            val = 5;
        }
        else if (val == US_CHRL_6) {
            val = 6;
        }
        else if (val == US_CHRL_7) {
            val = 7;
        }
        else {
            val = 8;
        }
    }
    return (uint8_t)val;
#else
    /* Not supported by DBGU, always 8 bits */
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
static int At91UsartSetDataBits(uint8_t bits)
{
#if !(USARTn_BASE == DBGU_BASE)
    unsigned int val = inr(USARTn_BASE + US_MR_OFF);

    if (bits == 9) {
        val &= ~US_PAR;
        val |= US_PAR_MULTIDROP;
    }
    else {
        val &= ~US_CHRL;
        if (bits == 5) {
            val |= US_CHRL_5;
        }
        else if (bits == 6) {
            val |= US_CHRL_6;
        }
        else if (bits == 7) {
            val |= US_CHRL_7;
        }
        else if (bits == 8) {
            val |= US_CHRL_8;
        }
    }

    At91UsartDisable();
    outr(USARTn_BASE + US_MR_OFF, val);
    At91UsartEnable();

    /*
     * Verify the result.
     */
    if (At91UsartGetDataBits() != bits) {
        return -1;
    }
    return 0;
#else
    /* Not supported by DBGU, always 8 bits */
    return -1;
#endif
}

/*!
 * \brief Query the USART hardware for the parity mode.
 *
 * This routine is called by ioctl function of the upper level USART
 * driver through the USARTDCB jump table.
 *
 * \return Parity mode, either 0 (disabled), 1 (odd), 2 (even) or 9 (multidrop).
 */
static uint8_t At91UsartGetParity(void)
{
    unsigned int val = inr(USARTn_BASE + US_MR_OFF) & US_PAR;

    if ((val & US_PAR) == US_PAR_MULTIDROP) {
        val = 9;
    }
    else {
        if (val == US_PAR_ODD) {
            val = 1;
        }
        else if (val == US_PAR_EVEN) {
            val = 2;
        }
        else {
            val = 0;
        }
    }
    return (uint8_t)val;
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
static int At91UsartSetParity(uint8_t mode)
{
    unsigned int val = inr(USARTn_BASE + US_MR_OFF) & ~US_PAR;

    switch (mode) {
    case 0:
        val |= US_PAR_NO;
        break;
    case 1:
        val |= US_PAR_ODD;
        break;
    case 2:
        val |= US_PAR_EVEN;
        break;
    }
    At91UsartDisable();
    outr(USARTn_BASE + US_MR_OFF, val);
    At91UsartEnable();

    /*
     * Verify the result.
     */
    if (At91UsartGetParity() != mode) {
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
 * \return The number of stop bits set, either 1, 2 or 3 (1.5 bits).
 */
static uint8_t At91UsartGetStopBits(void)
{
#if !(USARTn_BASE == DBGU_BASE)
    unsigned int val = inr(USARTn_BASE + US_MR_OFF) & US_NBSTOP;
    if (val == US_NBSTOP_1) {
        val = 1;
    }
    else if (val == US_NBSTOP_2) {
        val = 2;
    }
    else {
        val = 3;
    }
    return (uint8_t)val;
#else
    /* Not supported by DBGU, always 1 */
    return 1;
#endif
}

/*!
 * \brief Set the USART hardware to the number of stop bits.
 *
 * This routine is called by ioctl function of the upper level USART
 * driver through the USARTDCB jump table.
 *
 * \return 0 on success, -1 otherwise.
 */
static int At91UsartSetStopBits(uint8_t bits)
{
#if !(USARTn_BASE == DBGU_BASE)
    unsigned int val = inr(USARTn_BASE + US_MR_OFF) & ~US_NBSTOP;

    switch(bits) {
    case 1:
        val |= US_NBSTOP_1;
        break;
    case 2:
        val |= US_NBSTOP_2;
        break;
    case 3:
        val |= US_NBSTOP_1_5;
        break;
    }
    At91UsartDisable();
    outr(USARTn_BASE + US_MR_OFF, val);
    At91UsartEnable();

    /*
     * Verify the result.
     */
    if (At91UsartGetStopBits() != bits) {
        return -1;
    }
#endif
    return 0;
}

/*!
 * \brief Query the USART hardware status.
 *
 * \return Status flags.
 */
static uint32_t At91UsartGetStatus(void)
{
    uint32_t rc = 0;
#if USE_BUILT_IN_HARDWARE_HANDSHAKE
    uint32_t csr = inr(USARTn_BASE + US_CSR_OFF);
#endif

    /*
     * Set receiver error flags.
     */
    if ((rx_errors & US_FRAME) != 0) {
        rc |= UART_FRAMINGERROR;
    }
    if ((rx_errors & US_OVRE) != 0) {
        rc |= UART_OVERRUNERROR;
    }
    if ((rx_errors & US_PARE) != 0) {
        rc |= UART_PARITYERROR;
    }

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
     * Determine hardware handshake control status.
     */
#if USE_BUILT_IN_HARDWARE_HANDSHAKE
    /* How to find out? */
#elif defined(UART_RTS_BIT)
    /* No definition of the status register? */
#elif defined(UART_USES_NPL)
    if (bit_is_set(NPL_RSCR, NPL_RSRTS_BIT)) {
        rc |= UART_RTSDISABLED;
        if (rts_control) {
            rc |= UART_RXDISABLED;
        }
    } else {
        rc |= UART_RTSENABLED;
    }
#endif

    /*
     * Determine hardware handshake sense status.
     */
#if USE_BUILT_IN_HARDWARE_HANDSHAKE
    if (csr & US_CTS) {
        rc |= UART_CTSDISABLED;
        if (cts_sense) {
            rc |= UART_RXDISABLED;
        }
    } else {
        rc |= UART_CTSENABLED;
    }
#else
    if (UART_CTS_IS_ON()) {
        rc |= UART_CTSENABLED;
    } else {
        rc |= UART_CTSDISABLED;
        if (cts_sense) {
            rc |= UART_RXDISABLED;
        }
    }
#endif

    /*
     * Determine hardware modem sense status.
     */
#if 0
    /* I'm confused. Awful flag mismatch? Why do we have uart.h and usart.h? */
    if (csr & US_RI) {
        rc |= UART_RIDISABLED;
    } else {
        rc |= UART_RIENABLED;
    }
    if (csr & US_DSR) {
        rc |= UART_DSRDISABLED;
    } else {
        rc |= UART_DSRENABLED;
    }
    if (csr & US_DCD) {
        rc |= UART_DCDDISABLED;
    } else {
        rc |= UART_DCDENABLED;
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
    return rc;
}

/*!
 * \brief Set the USART hardware status.
 *
 * \param flags Status flags.
 *
 * \return 0 on success, -1 otherwise.
 */
static int At91UsartSetStatus(uint32_t flags)
{
    /*
     * Process software handshake control.
     */
    if (flow_control) {

        /* Access to the flow control status must be atomic. */
        NutUartIrqDisable(&SIG_UART);

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
        NutUartIrqEnable(&SIG_UART);
    }

    /*
     * Process hardware handshake control.
     */
#if USE_BUILT_IN_HARDWARE_HANDSHAKE
    /* Built-in hardware. */
    if (rts_control) {
        if (flags & UART_RXDISABLED) {
            outr(USARTn_BASE + US_CR_OFF, US_RTSDIS);
        }
        if (flags & UART_RXENABLED) {
            outr(USARTn_BASE + US_CR_OFF, US_RTSEN);
        }
    }
    if (flags & UART_RTSDISABLED) {
        outr(USARTn_BASE + US_CR_OFF, US_RTSDIS);
    }
    if (flags & UART_RTSENABLED) {
        outr(USARTn_BASE + US_CR_OFF, US_RTSEN);
    }
#else
    /* Manually controlled via GPIO. */
    if (rts_control) {
        if (flags & UART_RXDISABLED) {
            UART_RTS_OFF();
        }
        if (flags & UART_RXENABLED) {
            UART_RTS_ON();
        }
    }
    if (flags & UART_RTSDISABLED) {
        UART_RTS_OFF();
    }
    if (flags & UART_RTSENABLED) {
        UART_RTS_ON();
    }
#endif


    /*
     * Process hardware modem control.
     */
#if USE_BUILT_IN_MODEM_CONTROL
    /* Built-in hardware. */
    if (flags & UART_DTRDISABLED) {
        outr(USARTn_BASE + US_CR_OFF, US_DTRDIS);
    }
    if (flags & UART_DTRENABLED) {
        outr(USARTn_BASE + US_CR_OFF, US_DTREN);
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

    /*
     * Clear UART receive errors.
     */
    if (flags & UART_ERRORS) {
        outr(USARTn_BASE + US_CR_OFF, US_RSTSTA);
    }

    /*
     * Verify the result.
     */
    if ((At91UsartGetStatus() & ~UART_ERRORS) != flags) {
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
 * Not implemented for the AT91 USART. Always returns 0.
 *
 * \return Or-ed combination of \ref UART_SYNC, \ref UART_MASTER,
 *         \ref UART_NCLOCK and \ref UART_HIGHSPEED.
 */
static uint8_t At91UsartGetClockMode(void)
{
    return 0;
}

/*!
 * \brief Set asynchronous or synchronous mode.
 *
 * This function is called by ioctl function of the upper level USART
 * driver through the USARTDCB jump table.
 *
 * Not implemented for the AT91 USART. Always returns -1.
 *
 * \param mode Must be an or-ed combination of USART_SYNC, USART_MASTER,
 *             USART_NCLOCK and USART_HIGHSPEED.
 *
 * \return 0 on success, -1 otherwise.
 */
static int At91UsartSetClockMode(uint8_t mode)
{
    return -1;
}

/*!
 * \brief Query flow control mode.
 *
 * This routine is called by ioctl function of the upper level USART
 * driver through the USARTDCB jump table.
 *
 * \return See UsartIOCtl().
 */
static uint32_t At91UsartGetFlowControl(void)
{
    uint32_t rc = 0;

    if (flow_control) {
        rc |= USART_MF_XONXOFF;
    } else {
        rc &= ~USART_MF_XONXOFF;
    }

#if defined(UART_RTS_BIT) || defined(UART_USES_NPL) || defined(UART_HARDWARE_HANDSHAKE)
    if (rts_control) {
        rc |= USART_MF_RTSCONTROL;
    } else {
        rc &= ~USART_MF_RTSCONTROL;
    }
#endif

#if defined(UART_CTS_BIT) || defined(UART_USES_NPL) || defined(UART_HARDWARE_HANDSHAKE)
    if (cts_sense) {
        rc |= USART_MF_CTSSENSE;
    } else {
        rc &= ~USART_MF_CTSSENSE;
    }
#endif

    if (hdx_control) {
        rc |= USART_MF_HALFDUPLEX;
    } else {
        rc &= ~USART_MF_HALFDUPLEX;
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
static int At91UsartSetFlowControl(uint32_t flags)
{
#if USE_BUILT_IN_HARDWARE_HANDSHAKE || USE_BUILT_IN_HALF_DUPLEX
    uint32_t mr = inr(USARTn_BASE + US_MR_OFF) & ~US_MODE;
#endif

    /*
     * Set software handshake mode.
     */
    NutUartIrqDisable(&SIG_UART);
    if (flags & USART_MF_XONXOFF) {
        if(flow_control == 0) {
            flow_control = 1 | XOFF_SENT;  /* force XON to be sent on next read */
        }
    } else {
        flow_control = 0;
    }
    NutUartIrqEnable(&SIG_UART);

    /*
     * Set RTS control mode.
     */
    UART_RTS_PIN_ENABLE();
    UART_RTS_ON();
    if (flags & USART_MF_RTSCONTROL) {
        rts_control = 1;
#if USE_BUILT_IN_HARDWARE_HANDSHAKE
        outr(USARTn_BASE + US_MR_OFF, mr | US_MODE_HWHANDSHAKE);
        cts_sense = 1;
#endif
    } else if (rts_control) {
        rts_control = 0;
#if USE_BUILT_IN_HARDWARE_HANDSHAKE
        outr(USARTn_BASE + US_MR_OFF, mr);
        cts_sense = 0;
#endif
    }

    /*
     * Set CTS sense mode.
     */
    if (flags & USART_MF_CTSSENSE) {
        cts_sense = 1;
        UART_CTS_PIN_ENABLE();
#if USE_BUILT_IN_HARDWARE_HANDSHAKE
        rts_control = 1;
        outr(USARTn_BASE + US_MR_OFF, mr | US_MODE_HWHANDSHAKE);
#endif
    } else if (cts_sense) {
        cts_sense = 0;
#if USE_BUILT_IN_HARDWARE_HANDSHAKE
        rts_control = 0;
        outr(USARTn_BASE + US_MR_OFF, mr);
#endif
    }

    /*
     * Set half duplex mode.
     */
    if (flags & USART_MF_HALFDUPLEX) {
        /* Mark half duplex mode enabled. */
        hdx_control = 1;
        /* Enable half duplex pin. */
        UART_HDX_PIN_ENABLE();
        UART_HDX_RX();
#if USE_BUILT_IN_HALF_DUPLEX
        /* Enable built-in RS-485 mode. */
        outr(USARTn_BASE + US_MR_OFF, mr | US_MODE_RS485);
#else
        /* Enable USART receive, disable transmit. */
        outr(USARTn_BASE + US_CR_OFF, US_RXEN | US_TXDIS);
        /* Enable transmit complete interrupt. */
        outr(USARTn_BASE + US_IDR_OFF, US_TXEMPTY);
#endif
    }
    else if (hdx_control) {
        hdx_control = 0;
#if USE_BUILT_IN_HALF_DUPLEX
        /* Disable built-in RS-485 mode. */
        outr(USARTn_BASE + US_MR_OFF, inr(USARTn_BASE + US_MR_OFF) & ~US_MODE);
#else
        /* Enable USART receive and transmit. */
        outr(USARTn_BASE + US_CR_OFF, US_RXEN | US_TXEN);
        /* Disable transmit complete interrupt. */
        outr(USARTn_BASE + US_IDR_OFF, US_TXEMPTY);
#endif
    }
    /* No need to call At91UsartGetFlowControl. The result is obvious,
    ** because we do not exclude unsupported modes yet. */

    return 0;
}

/*!
 * \brief Start the USART transmitter hardware.
 *
 * The upper level USART driver will call this function through the
 * USARTDCB jump table each time it added one or more bytes to the
 * transmit buffer.
 */
static void At91UsartTxStart(void)
{
#if USE_BUILT_IN_HALF_DUPLEX == 0
    if (hdx_control) {
#if defined(UART_HDX_BIT) || defined(UART_USES_NPL)
        UART_HDX_TX();
#endif
        /* Disable USART receive, enable transmit. */
        outr(USARTn_BASE + US_CR_OFF, US_RXDIS | US_TXEN);
    }
#endif
    /* Enable transmit interrupts. */
    outr(USARTn_BASE + US_IER_OFF, US_TXRDY);
}

/*!
 * \brief Start the USART receiver hardware.
 *
 * The upper level USART driver will call this function through the
 * USARTDCB jump table each time it removed enough bytes from the
 * receive buffer. Enough means, that the number of bytes left in
 * the buffer is below the low watermark.
 */
static void At91UsartRxStart(void)
{
    /*
     * Do any required software flow control.
     */
    if (flow_control && (flow_control & XOFF_SENT) != 0) {
        NutUartIrqDisable(&SIG_UART);
        if ((inr(USARTn_BASE + US_CSR_OFF) & US_TXRDY)) {
            outr(USARTn_BASE + US_THR_OFF, ASCII_XON);
            flow_control &= ~XON_PENDING;
        } else {
            flow_control |= XON_PENDING;
        }
        flow_control &= ~(XOFF_SENT | XOFF_PENDING);
        NutUartIrqEnable(&SIG_UART);
    }

    /* Enable RTS. */
    UART_RTS_ON();
    /* Enable receive interrupts. */
    outr(USARTn_BASE + US_IER_OFF, US_RXRDY);
}

/*
 * \brief Initialize the USART hardware driver.
 *
 * This function is called during device registration by the upper level
 * USART driver through the USARTDCB jump table.
 *
 * \return 0 on success, -1 otherwise.
 */
static int At91UsartInit(void)
{
    /*
     * Register receive and transmit interrupts.
     */
    if (NutUartIrqRegister(&SIG_UART, At91UsartInterrupt, &dcb_usart)) {
        return -1;
    }

    /*
     * Register CTS sense interrupts.
     */
#if defined(UART_CTS_BIT)

    if (GpioRegisterIrqHandler(&sig_GPIO1, UART_CTS_BIT, At91UsartCts, NULL)) {
        return -1;
    }
#if defined(PS_PCER)
    outr(PS_PCER, _BV(UART_CTS_PIO_ID));
#elif defined(PMC_PCER)
    outr(PMC_PCER, _BV(UART_CTS_PIO_ID));
#endif

#elif defined(UART_USES_NPL)

    if (NplRegisterIrqHandler(&sig_RSCTS, At91UsartCts, NULL)) {
        return -1;
    }

#endif

    /* Enable UART clock. */
#if defined(US_ID)
#if defined(PS_PCER)
    outr(PS_PCER, _BV(US_ID));
#elif defined(PMC_PCER)
    outr(PMC_PCER, _BV(US_ID));
#endif
#endif

    /* Disable GPIO on UART tx/rx pins. */
    UART_RXTX_PINS_ENABLE();

    /* Reset UART. */
    outr(USARTn_BASE + US_CR_OFF, US_RSTRX | US_RSTTX | US_RXDIS | US_TXDIS);
    /* Disable all UART interrupts. */
    outr(USARTn_BASE + US_IDR_OFF, 0xFFFFFFFF);

#if defined(US_RCR_OFF) && defined(US_TCR_OFF)
    /* Clear UART PDC counter registers. */
    outr(USARTn_BASE + US_RCR_OFF, 0);
    outr(USARTn_BASE + US_TCR_OFF, 0);
#endif

    /* Set UART baud rate generator register. */
    At91UsartSetSpeed( UART_INIT_BAUDRATE);

    /* Set UART mode to 8 data bits, no parity and 1 stop bit. */
    outr(USARTn_BASE + US_MR_OFF, US_CHMODE_NORMAL | US_CHRL_8 | US_PAR_NO | US_NBSTOP_1);

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
static int At91UsartDeinit(void)
{
    /* Deregister receive and transmit interrupts. */
    NutUartIrqRegister(&SIG_UART, 0, 0);

    /* Reset UART. */
    outr(USARTn_BASE + US_CR_OFF, US_RSTRX | US_RSTTX | US_RXDIS | US_TXDIS);
    /* Disable all UART interrupts. */
    outr(USARTn_BASE + US_IDR_OFF, 0xFFFFFFFF);

    /* Disable UART clock. */
#if defined (US_ID)
#if defined(PS_PCDR)
    outr(PS_PCDR, _BV(US_ID));
#elif defined(PMC_PCDR)
    outr(PMC_PCDR, _BV(US_ID));
#endif
#endif

    /*
     * Disabling flow control shouldn't be required here, because it's up
     * to the upper level to do this on the last close or during
     * deregistration.
     */
    hdx_control = 0;
    UART_HDX_RX();
    cts_sense = 0;
    rts_control = 0;

    return 0;
}

/*@}*/
