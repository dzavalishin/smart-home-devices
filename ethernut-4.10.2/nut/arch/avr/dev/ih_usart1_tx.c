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
 * \file arch/avr/dev/ih_usart1_tx.c
 * \brief USART1 transmit complete interrupt.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.6  2009/01/17 11:26:38  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.5  2008/08/11 06:59:16  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.4  2007/04/12 09:23:15  haraldkipp
 * ATmega2561 uses different interrupt vector names. One day we should
 * switch to the new names used by avr-libc.
 *
 * Revision 1.3  2006/10/08 16:48:08  haraldkipp
 * Documentation fixed
 *
 * Revision 1.2  2006/07/13 05:05:43  hwmaier
 * Fixed typing error for NUT_PERFMON counter
 *
 * Revision 1.1  2006/02/08 15:14:21  haraldkipp
 * Using the vector number as a file name wasn't a good idea.
 * Moved from ivect*.c
 *
 * Revision 1.3  2006/01/25 09:38:51  haraldkipp
 * Applied Thiago A. Correa's patch to fix ICC warnings.
 *
 * Revision 1.2  2005/10/24 09:35:48  haraldkipp
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
 * Revision 1.1.1.1  2003/05/09 14:40:45  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.2  2003/03/31 14:53:07  harald
 * Prepare release 3.1
 *
 * \endverbatim
 */

#include <dev/irqreg.h>

/*!
 * \addtogroup xgIrqReg
 */
/*@{*/

#if defined(SIG_UART1_TRANS) || defined(iv_USART1_TX) || defined(SIG_USART1_TRANS)

static int AvrUart1TxIrqCtl(int cmd, void *param);

IRQ_HANDLER sig_UART1_TRANS = {
#ifdef NUT_PERFMON
    0,                          /* Interrupt counter, ir_count. */
#endif
    NULL,                       /* Passed argument, ir_arg. */
    NULL,                       /* Handler subroutine, ir_handler. */
    AvrUart1TxIrqCtl            /* Interrupt control, ir_ctl. */
};

/*!
 * \brief USART0 transmit complete interrupt control.
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
static int AvrUart1TxIrqCtl(int cmd, void *param)
{
    int rc = 0;
    unsigned int *ival = (unsigned int *) param;
    int_fast8_t enabled = bit_is_set(UCSR1B, TXCIE1);

    /* Disable interrupt. */
    cbi(UCSR1B, TXCIE1);

    switch (cmd) {
    case NUT_IRQCTL_INIT:
        enabled = 0;
    case NUT_IRQCTL_CLEAR:
        /* Clear any pending interrupt. */
        sbi(UCSR1A, TXC1);
        break;
    case NUT_IRQCTL_STATUS:
        if (bit_is_set(UCSR1A, TXC1)) {
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
        *ival = (unsigned int) sig_UART1_TRANS.ir_count; 
        sig_UART1_TRANS.ir_count = 0;             
        break;
#endif
    default:
        rc = -1;
        break;
    }

    /* Enable interrupt. */
    if (enabled) {
        sbi(UCSR1B, TXCIE1);
    }
    return rc;
}

/*! \fn SIG_UART1_TRANS(void)
 * Uart1 transmit complete interrupt entry.
 */
#if defined(SIG_UART1_TRANS) || defined(iv_USART1_TX)
#ifdef __IMAGECRAFT__
#pragma interrupt_handler SIG_UART1_TRANS:iv_USART1_TX
#endif
NUTSIGNAL(SIG_UART1_TRANS, sig_UART1_TRANS)
#elif defined(SIG_USART1_TRANS)
NUTSIGNAL(SIG_USART1_TRANS, sig_UART1_TRANS)
#endif

#endif
/*@}*/
