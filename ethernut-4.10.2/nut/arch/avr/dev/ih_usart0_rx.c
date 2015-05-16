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
 *
 */

/*!
 * \file arch/avr/dev/ih_usart0_rx.c
 * \brief USART0 receive complete interrupt.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.5  2008/08/11 06:59:16  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.4  2007/04/12 09:23:15  haraldkipp
 * ATmega2561 uses different interrupt vector names. One day we should
 * switch to the new names used by avr-libc.
 *
 * Revision 1.3  2006/10/08 16:48:07  haraldkipp
 * Documentation fixed
 *
 * Revision 1.2  2006/07/13 05:05:43  hwmaier
 * Fixed typing error for NUT_PERFMON counter
 *
 * Revision 1.1  2006/02/08 15:14:21  haraldkipp
 * Using the vector number as a file name wasn't a good idea.
 * Moved from ivect*.c
 *
 * Revision 1.4  2006/01/25 09:38:50  haraldkipp
 * Applied Thiago A. Correa's patch to fix ICC warnings.
 *
 * Revision 1.3  2005/10/24 18:02:34  haraldkipp
 * Fixes for ATmega103.
 *
 * Revision 1.2  2005/10/24 09:34:30  haraldkipp
 * New interrupt control function added to allow future platform
 * independant drivers.
 *
 * Revision 1.1  2005/07/26 18:02:40  haraldkipp
 * Moved from dev.
 *
 * Revision 1.3  2005/02/10 07:06:18  hwmaier
 * Changes to incorporate support for AT90CAN128 CPU
 *
 * Revision 1.2  2004/01/30 17:02:20  drsung
 * Separate interrupt stack for avr-gcc only added.
 *
 * Revision 1.1.1.1  2003/05/09 14:40:43  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.2  2003/03/31 14:53:06  harald
 * Prepare release 3.1
 *
 * \endverbatim
 */

#include <dev/irqreg.h>

/*!
 * \addtogroup xgIrqReg
 */
/*@{*/

static int AvrUart0RxIrqCtl(int cmd, void *param);

IRQ_HANDLER sig_UART0_RECV = {
#ifdef NUT_PERFMON
    0,                          /* Interrupt counter, ir_count. */
#endif
    NULL,                       /* Passed argument, ir_arg. */
    NULL,                       /* Handler subroutine, ir_handler. */
    AvrUart0RxIrqCtl            /* Interrupt control, ir_ctl. */
};

/*!
 * \brief USART0 receive complete interrupt.
 *
 * \param cmd   Control command.
 *              - NUT_IRQCTL_INIT Initialize and disable interrupt.
 *              - NUT_IRQCTL_CLEAR Clear interrupt.
 *              - NUT_IRQCTL_STATUS Query interrupt status.
 *              - NUT_IRQCTL_ENABLE Enable interrupt.
 *              - NUT_IRQCTL_DISABLE Disable interrupt.
 *              - NUT_IRQCTL_GETPRIO Query interrupt priority.
 *              - NUT_IRQCTL_GETCOUNT Query and clear interrupt counter.
 * \param param Pointer to optional parameter.
 *
 * \return 0 on success, -1 otherwise.
 */
static int AvrUart0RxIrqCtl(int cmd, void *param)
{
    int rc = 0;
    unsigned int *ival = (unsigned int *) param;
    int_fast8_t enabled = bit_is_set(UCR, RXCIE);
    uint8_t bval;

    /* Disable interrupt. */
    cbi(UCR, RXCIE);

    switch (cmd) {
    case NUT_IRQCTL_INIT:
        enabled = 0;
    case NUT_IRQCTL_CLEAR:
        /* Clear any pending interrupt. */
        bval = inb(UDR);
        bval = inb(UDR);
        break;
    case NUT_IRQCTL_STATUS:
        if (bit_is_set(USR, RXC)) {
            *ival = 1;
        } else {
            *ival = 0;
        }
        if (enabled) {
            *ival |= 0x80;
        }
        break;
    case NUT_IRQCTL_ENABLE:
        enabled = 1;
        break;
    case NUT_IRQCTL_DISABLE:
        enabled = 0;
        break;
    case NUT_IRQCTL_GETPRIO:
        *ival = 16;
        break;
#ifdef NUT_PERFMON
    case NUT_IRQCTL_GETCOUNT:
        *ival = (unsigned int) sig_UART0_RECV.ir_count;
        sig_UART0_RECV.ir_count = 0;            
        break;
#endif
    default:
        rc = -1;
        break;
    }

    /* Enable interrupt. */
    if (enabled) {
        sbi(UCR, RXCIE);
    }
    return rc;
}

#if defined(SIG_UART0_RECV) || defined(iv_USART0_RX)

/*! \fn SIG_UART0_RECV(void)
 * \brief Uart0 receive complete interrupt entry.
 */
#ifdef __IMAGECRAFT__
#pragma interrupt_handler SIG_UART0_RECV:iv_USART0_RX
#endif
NUTSIGNAL(SIG_UART0_RECV, sig_UART0_RECV)
#elif defined(SIG_USART0_RECV)

NUTSIGNAL(SIG_USART0_RECV, sig_UART0_RECV)

#else

/*! \fn SIG_UART_RECV(void)
 * \brief Uart0 receive complete interrupt entry.
 */
#ifdef __IMAGECRAFT__
#pragma interrupt_handler SIG_UART_RECV:iv_UART_RX
#endif
NUTSIGNAL(SIG_UART_RECV, sig_UART0_RECV)
#endif
/*@}*/
