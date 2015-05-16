/*
 * Copyright (C) 2005-2007 by egnite Software GmbH
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
 *
 */

/*
 * $Id: uart.c 2935 2010-04-01 12:14:17Z haraldkipp $
 */

#include "utils.h"
#include "uart.h"

#ifndef NUT_CPU_FREQ
#define NUT_CPU_FREQ    73728000
#endif

#define USART_BASE             0xFFFD0000       /*!< \brief USART 0 base address. */
#define US_ID      2            /*!< \brief USART 0 ID. */

#define US_CR      (USART_BASE + 0x00)  /*!< \brief Channel 0 control register address. */
#define US_IDR     (USART_BASE + 0x0C)  /*!< \brief Interrupt disable register address. */
#define US_RCR     (USART_BASE + 0x34)  /*!< \brief Receive counter register address. */
#define US_TCR     (USART_BASE + 0x3C)  /*!< \brief Transmit counter register address. */
#define US_BRGR    (USART_BASE + 0x20)  /*!< \brief USART 0 baud rate register address. */
#define US_MR      (USART_BASE + 0x04)  /*!< \brief Channel 0 mode register address. */
#define US_CSR     (USART_BASE + 0x14)  /*!< \brief Channel 0 status register address. */
#define US_RHR     (USART_BASE + 0x18)  /*!< \brief Channel 0 receiver holding register address. */
#define US_THR     (USART_BASE + 0x1C)  /*!< \brief USART 0 transmitter holding register address. */

#define US_RXRDY                0x00000001      /*!< \brief Receiver ready */
#define US_TXRDY                0x00000002      /*!< \brief Transmitter ready */

#define US_RSTRX                0x00000004      /*!< \brief Reset receiver */
#define US_RSTTX                0x00000008      /*!< \brief Reset transmitter */
#define US_RXEN                 0x00000010      /*!< \brief Receiver enable */
#define US_RXDIS                0x00000020      /*!< \brief Receiver disable */
#define US_TXEN                 0x00000040      /*!< \brief Transmitter enable */
#define US_TXDIS                0x00000080      /*!< \brief Transmitter disable */

#define US_CHRL_8               0x000000C0      /*!< \brief 8 data bits. */
#define US_PAR_NO               0x00000800      /*!< \brief No parity. */
#define US_NBSTOP_1             0x00000000      /*!< \brief 1 stop bit. */
#define US_CHMODE_NORMAL            0x00000000  /*!< \brief Normal mode. */

#define PS_BASE     0xFFFF4000  /*!< \brief PS base address. */
#define PS_PCER     (PS_BASE + 0x04)    /*!< \brief Peripheral clock enable register address. */

#define PIO_BASE    0xFFFF0000  /*!< \brief PIO base address. */
#define PIO_PDR     (PIO_BASE + 0x04)   /*!< \brief PIO disable register. */

#define inr(_reg)   (*((volatile unsigned int *)(_reg)))
#define outr(_reg, _val)  (*((volatile unsigned int *)(_reg)) = (_val))
#define _BV(bit)    (1 << bit)

#define AT91_US_BAUD(baud) ((NUT_CPU_FREQ / (8 * (baud)) + 1) / 2)

void UartInit(void)
{
    /* Enable UART clock. */
    outr(PS_PCER, _BV(US_ID));
    /* Disable GPIO on UART tx/rx pins. */
    outr(PIO_PDR, _BV(14) | _BV(15));
    /* Reset UART. */
    outr(US_CR, US_RSTRX | US_RSTTX | US_RXDIS | US_TXDIS);
    /* Disable all UART interrupts. */
    outr(US_IDR, 0xFFFFFFFF);
    /* Clear UART counter registers. */
    outr(US_RCR, 0);
    outr(US_TCR, 0);
    /* Set UART baud rate generator register. */
    outr(US_BRGR, AT91_US_BAUD(115200));
    /* Set UART mode to 8 data bits, no parity and 1 stop bit. */
    outr(US_MR, US_CHMODE_NORMAL | US_CHRL_8 | US_PAR_NO | US_NBSTOP_1);
    /* Enable UART receiver and transmitter. */
    outr(US_CR, US_RXEN | US_TXEN);

    MicroDelay(1000);
}

int UartRxWait(unsigned int tmo)
{
    while ((inr(US_CSR) & US_RXRDY) == 0) {
        if (tmo-- == 0) {
            return -1;
        }
    }
    return 0;
}

char UartRx(void)
{
    while ((inr(US_CSR) & US_RXRDY) == 0);
    return (char) inr(US_RHR);
}

void UartTx(char ch)
{
    while ((inr(US_CSR) & US_TXRDY) == 0);
    outr(US_THR, ch);
}
