/*
 * Copyright (C) 2001-2006 by egnite Software GmbH
 * Copyright (C) 2010 by egnite GmbH
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

/*!
 * \file arch/arm/dev/at91_dbg0.c
 * \brief AT91 debug output device using USART0.
 *
 * \verbatim
 * $Id$
 * \endverbatim
 */

#include <cfg/uart.h>
#include <arch/arm/atmel/debug_at91.h>

#if defined(USART0_BASE)

/*!
 * \addtogroup xgDevDebugAt91
 */
/*@{*/

#ifndef NUT_DEV_DEBUG_SPEED
#ifdef UART0_INIT_BAUDRATE
#define NUT_DEV_DEBUG_SPEED UART0_INIT_BAUDRATE
#else
#define NUT_DEV_DEBUG_SPEED 115200
#endif
#endif

/*!
 * \brief Initialize debug device 0.
 *
 * \return Always 0.
 */
static int Debug0Init(NUTDEVICE * dev)
{
    /* Enable UART clock. */
#if defined (PS_PCER)
    outr(PS_PCER, _BV(US0_ID));
#elif defined (PMC_PCER)
    outr(PMC_PCER, _BV(US0_ID));
#endif
    /* Disable GPIO on UART tx/rx pins. */
#if defined (P15_RXD0) && defined (P14_TXD0)
    outr(PIO_PDR, _BV(P15_RXD0) | _BV(P14_TXD0));
#elif defined (PA0_RXD0_A) && defined (PA1_TXD0_A)
    outr(PIOA_PDR, _BV(PA0_RXD0_A) | _BV(PA1_TXD0_A));
#elif defined (PA5_RXD0_A) && defined (PA6_TXD0_A)
    outr(PIOA_PDR, _BV(PA5_RXD0_A) | _BV(PA6_TXD0_A));
#endif
    /* Reset UART. */
    outr(US0_CR, US_RSTRX | US_RSTTX | US_RXDIS | US_TXDIS);
    /* Disable all UART interrupts. */
    outr(US0_IDR, 0xFFFFFFFF);
#if defined (US0_RCR) && defined(US0_TCR)
    /* Clear UART counter registers. */
    outr(US0_RCR, 0);
    outr(US0_TCR, 0);
#endif
#if NUT_DEV_DEBUG_SPEED
    /* Set UART baud rate generator register. */
    outr(US0_BRGR, At91BaudRateDiv(NUT_DEV_DEBUG_SPEED));
#endif
    /* Set UART mode to 8 data bits, no parity and 1 stop bit. */
    outr(US0_MR, US_CHMODE_NORMAL | US_CHRL_8 | US_PAR_NO | US_NBSTOP_1);
    /* Enable UART receiver and transmitter. */
    outr(US0_CR, US_RXEN | US_TXEN);

    return 0;
}

static NUTFILE dbgfile0;

/*!
 * \brief Debug device 0 information structure.
 */
NUTDEVICE devDebug0 = {
    0,                          /*!< Pointer to next device, dev_next. */
    {'u', 'a', 'r', 't', '0', 0, 0, 0, 0}
    ,                           /*!< Unique device name, dev_name. */
    0,                          /*!< Type of device, dev_type. */
    USART0_BASE,                /*!< Base address, dev_base. */
    0,                          /*!< First interrupt number, dev_irq. */
    0,                          /*!< Interface control block, dev_icb. */
    &dbgfile0,                  /*!< Driver control block, dev_dcb. */
    Debug0Init,                 /*!< Driver initialization routine, dev_init. */
    At91DevDebugIOCtl,          /*!< Driver specific control function, dev_ioctl. */
    0,                          /*!< dev_read. */
    At91DevDebugWrite,          /*!< dev_write. */
    At91DevDebugOpen,           /*!< dev_opem. */
    At91DevDebugClose,          /*!< dev_close. */
    0                           /*!< dev_size. */
};

#endif /* USART0_BASE */

/*@}*/
