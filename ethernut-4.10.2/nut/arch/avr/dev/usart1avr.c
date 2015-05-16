/*
 * Copyright (C) 2001-2007 by egnite Software GmbH. All rights reserved.
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
 * Revision 1.9  2008/08/11 11:51:19  thiagocorrea
 * Preliminary Atmega2560 compile options, but not yet supported.
 * It builds, but doesn't seam to run properly at this time.
 *
 * Revision 1.8  2008/08/11 06:59:17  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.7  2008/04/29 02:28:34  thiagocorrea
 * Add configurable DTR pin to AVR USART driver.
 *
 * Revision 1.6  2007/08/29 07:43:53  haraldkipp
 * Documentation updated and corrected.
 *
 * Revision 1.5  2007/02/15 16:19:14  haraldkipp
 * Can use PORTG for half duplex control.
 *
 * Revision 1.4  2006/05/30 18:34:46  beutel
 * added #include <cfg/os.h> in compliance with usart0avr.c
 *
 * Revision 1.3  2006/02/08 15:20:06  haraldkipp
 * ATmega2561 Support
 *
 * Revision 1.2  2005/10/07 22:05:00  hwmaier
 * Using __AVR_ENHANCED__ macro instead of __AVR_ATmega128__ to support also AT90CAN128 MCU
 *
 * Revision 1.1  2005/07/26 18:02:40  haraldkipp
 * Moved from dev.
 *
 * Revision 1.8  2005/07/22 08:07:08  freckle
 * added experimental improvements to usart driver. see ChangeLog for details
 *
 * Revision 1.7  2005/01/24 22:34:49  freckle
 * Added new tracer by Phlipp Blum <blum@tik.ee.ethz.ch>
 *
 * Revision 1.6  2005/01/22 19:25:48  haraldkipp
 * Changed AVR port configuration names from PORTx to AVRPORTx.
 *
 * Revision 1.5  2004/10/22 18:04:35  freckle
 * added #ifdef check to support old-style CTS definition
 * (old style: setting CTS_SIGNAL, CTS_BIT, CTS_PORT, CTS_PIN and CTS_DDR)
 * instead of the new single CTS_IRQ definition
 *
 * Revision 1.4  2004/10/03 18:43:44  haraldkipp
 * Some drivers may require the base address set to 1
 *
 * Revision 1.3  2004/09/22 08:14:48  haraldkipp
 * Made configurable
 *
 * Revision 1.2  2004/05/24 20:17:15  drsung
 * Added function UsartSize to return number of chars in input buffer.
 *
 * Revision 1.1  2003/12/15 19:25:33  haraldkipp
 * New USART driver added
 *
 */

#include <cfg/os.h>
#include <cfg/arch/avr.h>

#include <string.h>

#include <sys/atom.h>
#include <sys/event.h>
#include <sys/timer.h>

#include <dev/irqreg.h>
#include <dev/usartavr.h>

/*!
 * \name UART1 RTS Handshake Control
 *
 * UART1_RTS_BIT must be defined in arch/avr.h
 */
#ifdef UART1_RTS_BIT

#if (UART1_RTS_AVRPORT == AVRPORTB)
#define UART_RTS_PORT   PORTB
#define UART_RTS_DDR    DDRB

#elif (UART1_RTS_AVRPORT == AVRPORTD)
#define UART_RTS_PORT   PORTD
#define UART_RTS_DDR    DDRD

#elif (UART1_RTS_AVRPORT == AVRPORTE)
#define UART_RTS_PORT   PORTE
#define UART_RTS_DDR    DDRE

#elif (UART1_RTS_AVRPORT == AVRPORTF)
#define UART_RTS_PORT   PORTF
#define UART_RTS_DDR    DDRF

#elif (UART1_RTS_AVRPORT == AVRPORTG)
#define UART_RTS_PORT   PORTG
#define UART_RTS_DDR    DDRG

#elif (UART1_RTS_AVRPORT == AVRPORTH)
#define UART_RTS_PORT   PORTH
#define UART_RTS_DDR    DDRH

#endif
#define UART_RTS_BIT    UART1_RTS_BIT

#endif /* UART1_RTS_BIT */

#ifdef UART1_DTR_BIT

#if (UART1_DTR_AVRPORT == AVRPORTB)
#define UART_DTR_PORT  PORTB
#define UART_DTR_DDR   DDRB

#elif (UART1_DTR_AVRPORT == AVRPORTD)
#define UART_DTR_PORT  PORTD
#define UART_DTR_DDR   DDRD

#elif (UART1_DTR_AVRPORT == AVRPORTE)
#define UART_DTR_PORT  PORTE
#define UART_DTR_DDR   DDRE

#elif (UART1_DTR_AVRPORT == AVRPORTF)
#define UART_DTR_PORT  PORTF
#define UART_DTR_DDR   DDRF

#elif (UART1_DTR_AVRPORT == AVRPORTG)
#define UART_DTR_PORT  PORTG
#define UART_DTR_DDR   DDRG

#elif (UART1_DTR_AVRPORT == AVRPORTH)
#define UART_DTR_PORT  PORTH
#define UART_DTR_DDR   DDRH

#endif

#define UART_DTR_BIT    UART1_DTR_BIT

#endif /* UART1_DTR_BIT */

/*!
 * \name UART1 Half Duplex Control
 *
 * UART1_HDX_BIT must be defined in arch/avr.h
 */
#ifdef UART1_HDX_BIT

#if (UART1_HDX_AVRPORT == AVRPORTB)
#define UART_HDX_PORT   PORTB
#define UART_HDX_DDR    DDRB

#elif (UART1_HDX_AVRPORT == AVRPORTD)
#define UART_HDX_PORT   PORTD
#define UART_HDX_DDR    DDRD

#elif (UART1_HDX_AVRPORT == AVRPORTE)
#define UART_HDX_PORT   PORTE
#define UART_HDX_DDR    DDRE

#elif (UART1_HDX_AVRPORT == AVRPORTF)
#define UART_HDX_PORT   PORTF
#define UART_HDX_DDR    DDRF

#elif (UART1_HDX_AVRPORT == AVRPORTG)
#define UART_HDX_PORT  PORTG
#define UART_HDX_DDR   DDRG

#elif (UART1_HDX_AVRPORT == AVRPORTH)
#define UART_HDX_PORT  PORTH
#define UART_HDX_DDR   DDRH

#endif /* UART1_HDX_AVRPORT */
#define UART_HDX_BIT    UART1_HDX_BIT

#endif /* UART1_HDX_BIT */


#ifdef __AVR_ENHANCED__

/*
 * Local function prototypes.
 */
static uint32_t AvrUsartGetSpeed(void);
static int AvrUsartSetSpeed(uint32_t rate);
static uint8_t AvrUsartGetDataBits(void);
static int AvrUsartSetDataBits(uint8_t bits);
static uint8_t AvrUsartGetParity(void);
static int AvrUsartSetParity(uint8_t mode);
static uint8_t AvrUsartGetStopBits(void);
static int AvrUsartSetStopBits(uint8_t bits);
static uint32_t AvrUsartGetFlowControl(void);
static int AvrUsartSetFlowControl(uint32_t flags);
static uint32_t AvrUsartGetStatus(void);
static int AvrUsartSetStatus(uint32_t flags);
static uint8_t AvrUsartGetClockMode(void);
static int AvrUsartSetClockMode(uint8_t mode);
static void AvrUsartTxStart(void);
static void AvrUsartRxStart(void);
static int AvrUsartInit(void);
static int AvrUsartDeinit(void);

/*!
 * \addtogroup xgUsartAvr
 */
/*@{*/

/*!
 * \brief USART1 device control block structure.
 */
static USARTDCB dcb_usart1 = {
    0,                          /* dcb_modeflags */
    0,                          /* dcb_statusflags */
    0,                          /* dcb_rtimeout */
    0,                          /* dcb_wtimeout */
    {0, 0, 0, 0, 0, 0, 0, 0},   /* dcb_tx_rbf */
    {0, 0, 0, 0, 0, 0, 0, 0},   /* dcb_rx_rbf */
    0,                          /* dbc_last_eol */
    AvrUsartInit,               /* dcb_init */
    AvrUsartDeinit,             /* dcb_deinit */
    AvrUsartTxStart,            /* dcb_tx_start */
    AvrUsartRxStart,            /* dcb_rx_start */
    AvrUsartSetFlowControl,     /* dcb_set_flow_control */
    AvrUsartGetFlowControl,     /* dcb_get_flow_control */
    AvrUsartSetSpeed,           /* dcb_set_speed */
    AvrUsartGetSpeed,           /* dcb_get_speed */
    AvrUsartSetDataBits,        /* dcb_set_data_bits */
    AvrUsartGetDataBits,        /* dcb_get_data_bits */
    AvrUsartSetParity,          /* dcb_set_parity */
    AvrUsartGetParity,          /* dcb_get_parity */
    AvrUsartSetStopBits,        /* dcb_set_stop_bits */
    AvrUsartGetStopBits,        /* dcb_get_stop_bits */
    AvrUsartSetStatus,          /* dcb_set_status */
    AvrUsartGetStatus,          /* dcb_get_status */
    AvrUsartSetClockMode,       /* dcb_set_clock_mode */
    AvrUsartGetClockMode,       /* dcb_get_clock_mode */
};

/*!
 * \name AVR USART1 Device
 */
/*@{*/
/*!
 * \brief USART1 device information structure.
 *
 * An application must pass a pointer to this structure to
 * NutRegisterDevice() before using the serial communication
 * driver of the AVR's on-chip USART1.
 *
 * The device is named \b uart1.
 *
 * \showinitializer
 */
NUTDEVICE devUsartAvr1 = {
    0,                          /* Pointer to next device, dev_next. */
    {'u', 'a', 'r', 't', '1', 0, 0, 0, 0},    /* Unique device name, dev_name. */
    IFTYP_CHAR,                 /* Type of device, dev_type. */
    1,                          /* Base address, dev_base. */
    0,                          /* First interrupt number, dev_irq (not used). */
    0,                          /* Interface control block, dev_icb (not used). */
    &dcb_usart1,                /* Driver control block, dev_dcb. */
    UsartInit,                  /* Driver initialization routine, dev_init. */
    UsartIOCtl,                 /* Driver specific control function, dev_ioctl. */
    UsartRead,                  /* Read from device, dev_read. */
    UsartWrite,                 /* Write to device, dev_write. */
    UsartWrite_P,               /* Write data from program space to device, dev_write_P. */
    UsartOpen,                  /* Open a device or file, dev_open. */
    UsartClose,                 /* Close a device or file, dev_close. */
    UsartSize                   /* Request file size, dev_size. */
};
/*@}*/

/*!
 * \name UART1 CTS Handshake Sense
 *
 * \ref UART1_CTS_IRQ must be defined in arch/avr.h
 */

// added extra ifdef as test below is true even if UART1_CTS_IRQ is undef
#ifdef UART1_CTS_IRQ

#if (UART1_CTS_IRQ == INT0)
#define UART_CTS_SIGNAL sig_INTERRUPT0
#define UART_CTS_BIT    0
#define UART_CTS_PORT   PORTD
#define UART_CTS_PIN    PIND
#define UART_CTS_DDR    DDRD

#elif (UART1_CTS_IRQ == INT1)
#define UART_CTS_SIGNAL sig_INTERRUPT1
#define UART_CTS_BIT    1
#define UART_CTS_PORT   PORTD
#define UART_CTS_PIN    PIND
#define UART_CTS_DDR    DDRD

#elif (UART1_CTS_IRQ == INT2)
#define UART_CTS_SIGNAL sig_INTERRUPT2
#define UART_CTS_BIT    2
#define UART_CTS_PORT   PORTD
#define UART_CTS_PIN    PIND
#define UART_CTS_DDR    DDRD

#elif (UART1_CTS_IRQ == INT3)
#define UART_CTS_SIGNAL sig_INTERRUPT3
#define UART_CTS_BIT    3
#define UART_CTS_PORT   PORTD
#define UART_CTS_PIN    PIND
#define UART_CTS_DDR    DDRD

#elif (UART1_CTS_IRQ == INT4)
#define UART_CTS_SIGNAL sig_INTERRUPT4
#define UART_CTS_BIT    4
#define UART_CTS_PORT   PORTE
#define UART_CTS_PIN    PINE
#define UART_CTS_DDR    DDRE

#elif (UART1_CTS_IRQ == INT5)
#define UART_CTS_SIGNAL sig_INTERRUPT5
#define UART_CTS_BIT    5
#define UART_CTS_PORT   PORTE
#define UART_CTS_PIN    PINE
#define UART_CTS_DDR    DDRE

#elif (UART1_CTS_IRQ == INT6)
#define UART_CTS_SIGNAL sig_INTERRUPT6
#define UART_CTS_BIT    6
#define UART_CTS_PORT   PORTE
#define UART_CTS_PIN    PINE
#define UART_CTS_DDR    DDRE

#elif (UART1_CTS_IRQ == INT7)
#define UART_CTS_SIGNAL sig_INTERRUPT7
#define UART_CTS_BIT    7
#define UART_CTS_PORT   PORTE
#define UART_CTS_PIN    PINE
#define UART_CTS_DDR    DDRE

#endif

#else

// alternate way to specify the cts line
#define UART_CTS_PORT   UART1_CTS_PORT
#define UART_CTS_PIN    UART1_CTS_PIN
#define UART_CTS_DDR    UART1_CTS_DDR
// only set CTS_BIT if used and IRQ available
#ifdef UART1_CTS_BIT
#define UART_CTS_SIGNAL UART1_CTS_SIGNAL
#define UART_CTS_BIT    UART1_CTS_BIT
#endif

#endif

/*@}*/
/*@}*/

#define UDRn    UDR1
#define UCSRnA  UCSR1A
#define UCSRnB  UCSR1B
#define UCSRnC  UCSR1C
#define UBRRnL  UBRR1L
#define UBRRnH  UBRR1H

#ifdef __IMAGECRAFT__
#define TXB8    TXB81
#if defined(ATMega2560) || defined(ATMega2561)
#define UMSEL   UMSEL01
#else
#define UMSEL   UMSEL1
#endif
#define U2X     U2X1
#define UCSZ2   UCSZ12
#define UCSZ1   UCSZ11
#define UCSZ0   UCSZ10
#define UPM0    UPM10
#define UPM1    UPM11
#define USBS    USBS1
#define UPE     UPE1
#define MPCM    MPCM1
#define UCPOL   UCPOL1
#endif

#define sig_UART_RECV   sig_UART1_RECV
#define sig_UART_DATA   sig_UART1_DATA
#define sig_UART_TRANS  sig_UART1_TRANS

#define SIG_UART_RECV   SIG_UART1_RECV
#define SIG_UART_DATA   SIG_UART1_DATA
#define SIG_UART_TRANS  SIG_UART1_TRANS

#define dcb_usart   dcb_usart1

#ifdef NUTTRACER
#define TRACE_INT_UART_CTS TRACE_INT_UART1_CTS
#define TRACE_INT_UART_RXCOMPL TRACE_INT_UART1_RXCOMPL
#define TRACE_INT_UART_TXEMPTY TRACE_INT_UART1_TXEMPTY
#endif

#ifdef UART1_READMULTIBYTE
#define UART_READMULTIBYTE
#endif

#ifdef USE_USART1
#define USE_USART
#endif

#ifdef UART1_NO_SW_FLOWCONTROL
#define UART_NO_SW_FLOWCONTROL
#endif

#include "usartavr.c"

#endif
