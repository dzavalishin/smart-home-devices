/*
 * Copyright (C) 2005 by egnite Software GmbH. All rights reserved.
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
 * Revision 0.01  2009/09/20 ulrichprinz
 * First checkin of using DBGU as limited standard USART.
 *
 */

#define NUT_DEPRECATED

#include <cfg/os.h>
#include <cfg/clock.h>
#include <cfg/arch.h>
#include <cfg/uart.h>

#include <string.h>

#include <sys/atom.h>
#include <sys/event.h>
#include <sys/timer.h>

#include <dev/irqreg.h>
#include <dev/debug.h>
#include <arch/arm/atmel/at91_dbgu.h>
#include <dev/usartat91.h>

#ifndef NUT_CPU_FREQ
#ifdef NUT_PLL_CPUCLK
#include <dev/cy2239x.h>
#else /* !NUT_PLL_CPUCLK */
#define NUT_CPU_FREQ    73728000UL
#endif /* !NUT_PLL_CPUCLK */
#endif /* !NUT_CPU_FREQ */

/*
 * Local function prototypes.
 *
 * Commented functions are not supported by DBGU
 *
 */
static uint32_t At91UsartGetSpeed(void);
static int At91UsartSetSpeed(uint32_t rate);
static uint8_t At91UsartGetDataBits(void);
static int At91UsartSetDataBits( uint8_t bits);
static uint8_t At91UsartGetParity(void);
static int At91UsartSetParity(uint8_t mode);
static uint8_t At91UsartGetStopBits(void);
static int At91UsartSetStopBits(uint8_t bits);
static uint32_t At91UsartGetStatus(void);
static int At91UsartSetStatus(uint32_t flags);
static uint8_t At91UsartGetClockMode(void);
static int At91UsartSetClockMode(uint8_t mode);
static void At91UsartTxStart(void);
static void At91UsartRxStart(void);
static int At91UsartInit(void);
static int At91UsartDeinit(void);
static uint32_t At91UsartGetFlowControl(void);
static int At91UsartSetFlowControl(uint32_t flags);

extern IRQ_HANDLER sig_DBGU;

/*!
 * \addtogroup xgNutArchArmAt91Us
 */
/*@{*/

/*!
 * \brief DBGU device control block structure.
 */
static USARTDCB dcb_dbgu = {
    0,                          /* dcb_modeflags */
    0,                          /* dcb_statusflags */
    0,                          /* dcb_rtimeout */
    0,                          /* dcb_wtimeout */
    {0, 0, 0, 0, 0, 0, 0, 0},   /* dcb_tx_rbf */
    {0, 0, 0, 0, 0, 0, 0, 0},   /* dcb_rx_rbf */
    0,                          /* dbc_last_eol */
    At91UsartInit,    	        /* dcb_init */
    At91UsartDeinit,            /* dcb_deinit */
    At91UsartTxStart,           /* dcb_tx_start */
    At91UsartRxStart,           /* dcb_rx_start */
    At91UsartSetFlowControl,    /* dcb_set_flow_control */
    At91UsartGetFlowControl,    /* dcb_get_flow_control */
    At91UsartSetSpeed,          /* dcb_set_speed */
    At91UsartGetSpeed,	        /* dcb_get_speed */
    At91UsartSetDataBits,       /* dcb_set_data_bits */
    At91UsartGetDataBits,       /* dcb_get_data_bits */
    At91UsartSetParity,         /* dcb_set_parity */
    At91UsartGetParity,         /* dcb_get_parity */
    At91UsartSetStopBits,       /* dcb_set_stop_bits */
    At91UsartGetStopBits,       /* dcb_get_stop_bits */
    At91UsartSetStatus,         /* dcb_set_status */
    At91UsartGetStatus,         /* dcb_get_status */
    At91UsartSetClockMode,      /* dcb_set_clock_mode */
    At91UsartGetClockMode,      /* dcb_get_clock_mode */
};

/*!
 * \name AT91 DBGU Device
 */
/*@{*/
/*!
 * \brief USART0 device information structure.
 *
 * An application must pass a pointer to this structure to
 * NutRegisterDevice() before using the serial communication
 * driver of the AT91's on-chip USART0.
 *
 * The device is named \b uart0.
 *
 * \showinitializer
 */
NUTDEVICE devDbguAt91 = {
    0,                          /* Pointer to next device, dev_next. */
    {'u', 'a', 'r', 't', 'd', 0, 0, 0, 0},    /* Unique device name, dev_name. */
    IFTYP_CHAR,                 /* Type of device, dev_type. */
    DBGU_BASE,                  /* Base address, dev_base (not used). */
    0,                          /* First interrupt number, dev_irq (not used). */
    0,                          /* Interface control block, dev_icb (not used). */
    &dcb_dbgu,                  /* Driver control block, dev_dcb. */
    UsartInit,                  /* Driver initialization routine, dev_init. */
    UsartIOCtl,                 /* Driver specific control function, dev_ioctl. */
    UsartRead,                  /* Read from device, dev_read. */
    UsartWrite,                 /* Write to device, dev_write. */
    UsartOpen,                  /* Open a device or file, dev_open. */
    UsartClose,                 /* Close a device or file, dev_close. */
    UsartSize                   /* Request file size, dev_size. */
};

/*@}*/

/*@}*/

/* Modem control includes hardware handshake. */
/*
 * Hardware driven control signals are not available
 * with the DBUG unit of most chips.
 */

#undef UART_MODEM_CONTROL
#undef UART_HARDWARE_HANDSHAKE

#if defined(UART_MODEM_CONTROL)
#define UART_MODEM_CONTROL
#define UART_HARDWARE_HANDSHAKE
#elif defined(UART_HARDWARE_HANDSHAKE)
#define UART_HARDWARE_HANDSHAKE
#endif

/*
** SAM9260 and SAM9XE pins.
*/
#if defined(MCU_AT91SAM9260) || defined(MCU_AT91SAM9XE)

#define UART_RXTX_PINS  (_BV(PB14_DRXD_A) | _BV(PB15_DTXD_A))
#undef UART_HDX_PIN
#undef UART_RTS_PIN
#undef UART_CTS_PIN
#undef UART_MODEM_PINS

#define UART_RXTX_PINS_ENABLE()     outr(PIOB_ASR, UART_RXTX_PINS); \
                                    outr(PIOB_PDR, UART_RXTX_PINS)

#if defined(UART_HARDWARE_HANDSHAKE)
#define UART_HDX_PIN_ENABLE()       outr(PIOB_ASR, UART_HDX_PIN); \
                                    outr(PIOB_PDR, UART_HDX_PIN)
#define UART_RTS_PIN_ENABLE()       outr(PIOB_ASR, UART_RTS_PIN); \
                                    outr(PIOB_PDR, UART_RTS_PIN)
#define UART_CTS_PIN_ENABLE()       outr(PIOB_ASR, UART_CTS_PIN); \
                                    outr(PIOB_PDR, UART_CTS_PIN)
#endif

#if defined(UART_MODEM_CONTROL)
#define UART_MODEM_PINS_ENABLE()    outr(PIOB_ASR, UART_MODEM_PINS); \
                                    outr(PIOB_PDR, UART_MODEM_PINS)
#endif

/*
** SAM7S and SAM7SE pins.
*/
#elif defined(MCU_AT91SAM7S) || defined(MCU_AT91SAM7SE)

#define UART_RXTX_PINS  (_BV(PA9_DRXD_A) | _BV(PA10_DTXD_A))
#undef UART_HDX_PIN
#undef UART_RTS_PIN
#undef UART_CTS_PIN

#define UART_RXTX_PINS_ENABLE() outr(PIOA_ASR, UART_RXTX_PINS); \
                                outr(PIOA_PDR, UART_RXTX_PINS)

#if defined(UART_HARDWARE_HANDSHAKE)
#define UART_HDX_PIN_ENABLE()   outr(PIOA_ASR, UART_HDX_PIN); \
                                outr(PIOA_PDR, UART_HDX_PIN)
#define UART_RTS_PIN_ENABLE()   outr(PIOA_ASR, UART_RTS_PIN); \
                                outr(PIOA_PDR, UART_RTS_PIN)
#define UART_CTS_PIN_ENABLE()   outr(PIOA_ASR, UART_CTS_PIN); \
                                outr(PIOA_PDR, UART_CTS_PIN)
#endif

/*
** SAM7X pins.
*/
#elif defined(MCU_AT91SAM7X)

#define UART_RXTX_PINS  (_BV(PA27_DRXD_A) | _BV(PA28_DTXD_A))
#undef UART_HDX_PIN
#undef UART_RTS_PIN
#undef UART_CTS_PIN

#define UART_RXTX_PINS_ENABLE() outr(PIOA_ASR, UART_RXTX_PINS); \
                                outr(PIOA_PDR, UART_RXTX_PINS)

#if defined(UART_HARDWARE_HANDSHAKE)
#define UART_HDX_PIN_ENABLE()   outr(PIOA_ASR, UART_HDX_PIN); \
                                outr(PIOA_PDR, UART_HDX_PIN)
#define UART_RTS_PIN_ENABLE()   outr(PIOA_ASR, UART_RTS_PIN); \
                                outr(PIOA_PDR, UART_RTS_PIN)
#define UART_CTS_PIN_ENABLE()   outr(PIOA_ASR, UART_CTS_PIN); \
                                outr(PIOA_PDR, UART_CTS_PIN)
#endif
#endif

/*
** CPLD logic, currently used on Ethernut 3 only.
*/
#if defined(ETHERNUT3)
#define UART_USES_NPL   1
#endif

/*
** Determine the CTS GPIO interrupt, based on the port ID.
*/
#if defined(UART0_CTS_BIT) && !defined(UART0_CTS_SIGNAL)
#if UART0_CTS_PIO_ID == PIOA_ID
#define UART0_CTS_SIGNAL    sig_GPIO1
#elif UART0_CTS_PIO_ID == PIOB_ID
#define UART0_CTS_SIGNAL    sig_GPIO2
#elif UART0_CTS_PIO_ID == PIOC_ID
#define UART0_CTS_SIGNAL    sig_GPIO3
#else
#define UART0_CTS_SIGNAL    sig_GPIO
#endif
#endif

/*
** Translate all macros for UART0 to generalized ones used by the
** source that will be included at the end of this file.
*/
#if defined(UARTD_HDX_BIT)
#define UART_HDX_BIT    UARTD_HDX_BIT
#endif
#if defined(UARTD_HDX_PIO_ID)
#define UART_HDX_PIO_ID UARTD_HDX_PIO_ID
#endif

#if defined(UARTD_RTS_BIT)
#define UART_RTS_BIT    UARTD_RTS_BIT
#endif
#if defined(UARTD_RTS_PIO_ID)
#define UART_RTS_PIO_ID UARTD_RTS_PIO_ID
#endif

#if defined(UARTD_CTS_BIT)
#define UART_CTS_BIT    UARTD_CTS_BIT
#endif
#if defined(UARTD_CTS_PIO_ID)
#define UART_CTS_PIO_ID UARTD_CTS_PIO_ID
#endif
#if defined(UARTD_CTS_SIGNAL)
#define UART_CTS_SIGNAL UARTD_CTS_SIGNAL
#endif

#if defined(UARTD_INIT_BAUDRATE)
#define UART_INIT_BAUDRATE  UARTD_INIT_BAUDRATE
#endif

#define USARTn_BASE     DBGU_BASE
#define US_ID           SYSC_ID
#define SIG_UART        syssig_DBGU
#define dcb_usart       dcb_dbgu

#include "usartat91.c"
