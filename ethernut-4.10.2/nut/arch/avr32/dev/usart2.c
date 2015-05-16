/*!
 * Copyright (C) 2001-2010 by egnite Software GmbH
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
 * $Log: usart2.c,v $
 *
 */

#include <cfg/os.h>
#include <cfg/clock.h>
#include <cfg/arch.h>
#include <cfg/uart.h>
#include <cfg/arch/gpio.h>

#include <string.h>

#include <sys/atom.h>
#include <sys/event.h>
#include <sys/timer.h>

#include <dev/irqreg.h>
#include <dev/usartavr32.h>
#include <dev/gpio.h>

/*
 * Local function prototypes.
 */
static uint32_t Avr32UsartGetSpeed(void);
static int Avr32UsartSetSpeed(uint32_t rate);
static uint8_t Avr32UsartGetDataBits(void);
static int Avr32UsartSetDataBits(uint8_t bits);
static uint8_t Avr32UsartGetParity(void);
static int Avr32UsartSetParity(uint8_t mode);
static uint8_t Avr32UsartGetStopBits(void);
static int Avr32UsartSetStopBits(uint8_t bits);
static uint32_t Avr32UsartGetFlowControl(void);
static int Avr32UsartSetFlowControl(uint32_t flags);
static uint32_t Avr32UsartGetStatus(void);
static int Avr32UsartSetStatus(uint32_t flags);
static uint8_t Avr32UsartGetClockMode(void);
static int Avr32UsartSetClockMode(uint8_t mode);
static void Avr32UsartTxStart(void);
static void Avr32UsartRxStart(void);
static int Avr32UsartInit(void);
static int Avr32UsartDeinit(void);

/*!
 * \addtogroup xgNutArchArmAvr32Us
 */
/*@{*/

/*!
 * \brief USART1 device control block structure.
 */
static USARTDCB dcb_usart2 = {
    0,                          /* dcb_modeflags */
    0,                          /* dcb_statusflags */
    0,                          /* dcb_rtimeout */
    0,                          /* dcb_wtimeout */
    {0, 0, 0, 0, 0, 0, 0, 0},   /* dcb_tx_rbf */
    {0, 0, 0, 0, 0, 0, 0, 0},   /* dcb_rx_rbf */
    0,                          /* dbc_last_eol */
    Avr32UsartInit,             /* dcb_init */
    Avr32UsartDeinit,           /* dcb_deinit */
    Avr32UsartTxStart,          /* dcb_tx_start */
    Avr32UsartRxStart,          /* dcb_rx_start */
    Avr32UsartSetFlowControl,   /* dcb_set_flow_control */
    Avr32UsartGetFlowControl,   /* dcb_get_flow_control */
    Avr32UsartSetSpeed,         /* dcb_set_speed */
    Avr32UsartGetSpeed,         /* dcb_get_speed */
    Avr32UsartSetDataBits,      /* dcb_set_data_bits */
    Avr32UsartGetDataBits,      /* dcb_get_data_bits */
    Avr32UsartSetParity,        /* dcb_set_parity */
    Avr32UsartGetParity,        /* dcb_get_parity */
    Avr32UsartSetStopBits,      /* dcb_set_stop_bits */
    Avr32UsartGetStopBits,      /* dcb_get_stop_bits */
    Avr32UsartSetStatus,        /* dcb_set_status */
    Avr32UsartGetStatus,        /* dcb_get_status */
    Avr32UsartSetClockMode,     /* dcb_set_clock_mode */
    Avr32UsartGetClockMode,     /* dcb_get_clock_mode */
};

/*!
 * \name Avr32 USART1 Device
 */
/*@{*/
/*!
 * \brief USART1 device information structure.
 *
 * An application must pass a pointer to this structure to
 * NutRegisterDevice() before using the serial communication
 * driver of the Avr32's on-chip USART1.
 *
 * The device is named \b uart1.
 *
 * \showinitializer
 */
NUTDEVICE devUsartAvr322 = {
    0,                          /* Pointer to next device, dev_next. */
    {'u', 'a', 'r', 't', '2', 0, 0, 0, 0},      /* Unique device name, dev_name. */
    IFTYP_CHAR,                 /* Type of device, dev_type. */
    0,                          /* Base address, dev_base (not used). */
    0,                          /* First interrupt number, dev_irq (not used). */
    0,                          /* Interface control block, dev_icb (not used). */
    &dcb_usart2,                /* Driver control block, dev_dcb. */
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

#define SIG_UART        sig_UART2
#define dcb_usart       dcb_usart2

#define USARTn_BASE         AVR32_USART2

#if UART2_ALT_PINSET == ALTERNATE_PIN_SET2
# if defined(AVR32_USART2_TXD_0_0_PIN)
#  define USART_RX_PIN        AVR32_USART2_RXD_0_1_PIN
#  define USART_RX_FUNCTION   AVR32_USART2_RXD_0_1_FUNCTION
#  define USART_TX_PIN        AVR32_USART2_TXD_0_1_PIN
#  define USART_TX_FUNCTION   AVR32_USART2_TXD_0_1_FUNCTION
# elif defined(AVR32_USART2_TXD_0_PIN)
#  define USART_RX_PIN        AVR32_USART2_RXD_1_PIN
#  define USART_RX_FUNCTION   AVR32_USART2_RXD_1_FUNCTION
#  define USART_TX_PIN        AVR32_USART2_TXD_1_PIN
#  define USART_TX_FUNCTION   AVR32_USART2_TXD_1_FUNCTION
# endif
#elif UART2_ALT_PINSET == ALTERNATE_PIN_SET3
# if defined(AVR32_USART2_TXD_0_0_PIN)
#  define USART_RX_PIN        AVR32_USART2_RXD_0_2_PIN
#  define USART_RX_FUNCTION   AVR32_USART2_RXD_0_2_FUNCTION
#  define USART_TX_PIN        AVR32_USART2_TXD_0_2_PIN
#  define USART_TX_FUNCTION   AVR32_USART2_TXD_0_2_FUNCTION
# elif defined(AVR32_USART2_TXD_0_PIN)
#  define USART_RX_PIN        AVR32_USART2_RXD_2_PIN
#  define USART_RX_FUNCTION   AVR32_USART2_RXD_2_FUNCTION
#  define USART_TX_PIN        AVR32_USART2_TXD_2_PIN
#  define USART_TX_FUNCTION   AVR32_USART2_TXD_2_FUNCTION
# endif
#else // ALTERNATE_PIN_SET1
# if defined(AVR32_USART2_TXD_0_0_PIN)
#  define USART_RX_PIN        AVR32_USART2_RXD_0_0_PIN
#  define USART_RX_FUNCTION   AVR32_USART2_RXD_0_0_FUNCTION
#  define USART_TX_PIN        AVR32_USART2_TXD_0_0_PIN
#  define USART_TX_FUNCTION   AVR32_USART2_TXD_0_0_FUNCTION
# elif defined(AVR32_USART2_TXD_0_PIN)
#  define USART_RX_PIN        AVR32_USART2_RXD_0_PIN
#  define USART_RX_FUNCTION   AVR32_USART2_RXD_0_FUNCTION
#  define USART_TX_PIN        AVR32_USART2_TXD_0_PIN
#  define USART_TX_FUNCTION   AVR32_USART2_TXD_0_FUNCTION
# endif
#endif

#include "usart.c"
