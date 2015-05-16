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
 * $Log: ih_uart2.c,v $
 *
 */

#include <arch/avr32.h>
#include <dev/irqreg.h>
#include <avr32/io.h>

#include <arch/avr32/ihndlr.h>

#ifdef AVR32_USART2_IRQ

#ifndef NUT_IRQPRI_UART2
#define NUT_IRQPRI_UART2  AVR32_INTC_INT3
#endif

static int Uart2IrqCtl(int cmd, void *param);

IRQ_HANDLER sig_UART2 = {
#ifdef NUT_PERFMON
    0,                          /* Interrupt counter, ir_count. */
#endif
    NULL,                       /* Passed argument, ir_arg. */
    NULL,                       /* Handler subroutine, ir_handler. */
    Uart2IrqCtl                 /* Interrupt control, ir_ctl. */
};

/*!
 * \brief UART 1 interrupt entry.
 */
SIGNAL(Uart2IrqEntry)
{
    IRQ_ENTRY();
#ifdef NUT_PERFMON
    sig_UART2.ir_count++;
#endif
    if (sig_UART2.ir_handler) {
        (sig_UART2.ir_handler) (sig_UART2.ir_arg);
    }
    IRQ_EXIT();
}

/*!
 * \brief UART 1 interrupt control.
 *
 * \param cmd   Control command.
 *              - NUT_IRQCTL_INIT Initialize and disable interrupt.
 *              - NUT_IRQCTL_STATUS Query interrupt status.
 *              - NUT_IRQCTL_ENABLE Enable interrupt.
 *              - NUT_IRQCTL_DISABLE Disable interrupt.
 *              - NUT_IRQCTL_GETPRIO Query interrupt priority.
 *              - NUT_IRQCTL_GETCOUNT Query and clear interrupt counter.
 * \param param Pointer to optional parameter.
 *
 * \return 0 on success, -1 otherwise.
 */
static int Uart2IrqCtl(int cmd, void *param)
{
    int rc = 0;
    unsigned int *ival = (unsigned int *) param;
    ureg_t imr = AVR32_USART2.imr;
    static ureg_t enabledIMR = 0;
    int_fast8_t enabled = imr;

    /* Disable interrupt. */
    if (enabled) {
        AVR32_USART2.idr = 0xFFFFFFFF;
        AVR32_USART2.csr;
        enabledIMR = imr;
    }

    switch (cmd) {
    case NUT_IRQCTL_INIT:
        /* Set the vector. */
        register_interrupt(Uart2IrqEntry, AVR32_USART2_IRQ, NUT_IRQPRI_UART2);

        break;
    case NUT_IRQCTL_STATUS:
        if (enabled) {
            *ival |= 1;
        } else {
            *ival &= ~1;
        }
        break;
    case NUT_IRQCTL_ENABLE:
        enabled = 1;
        if (*ival)
            imr = *ival;
        break;
    case NUT_IRQCTL_DISABLE:
        enabled = 0;
        break;
    case NUT_IRQCTL_GETPRIO:
        *ival = NUT_IRQPRI_UART2;
        break;
#ifdef NUT_PERFMON
    case NUT_IRQCTL_GETCOUNT:
        *ival = (unsigned int) sig_UART2.ir_count;
        sig_UART2.ir_count = 0;
        break;
#endif
    default:
        rc = -1;
        break;
    }

    /* Enable interrupt. */
    if (enabled) {
        AVR32_USART2.ier = enabledIMR;
        AVR32_USART2.csr;
    }
    return rc;
}

#endif // AVR32_USART2_IRQ
