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
 * $Log: ih_rtc.c,v $
  *
 */

#include <arch/avr32.h>
#include <arch/avr32/pm.h>
#include <dev/irqreg.h>
#include <avr32/io.h>

#include <arch/avr32/ihndlr.h>

#ifndef NUT_IRQPRI_RTC
#define NUT_IRQPRI_RTC  AVR32_INTC_INT0
#endif

// Work around differences between ES and production chips.
#ifndef AVR32_RTC_CTRL_CLKEN_MASK
#define AVR32_RTC_CTRL_CLKEN_MASK AVR32_RTC_CTRL_EN_MASK
#endif


static int RealTimeCounterIrqCtl(int cmd, void *param);

IRQ_HANDLER sig_RTC = {
#ifdef NUT_PERFMON
    0,                          /* Interrupt counter, ir_count. */
#endif
    NULL,                       /* Passed argument, ir_arg. */
    NULL,                       /* Handler subroutine, ir_handler. */
    RealTimeCounterIrqCtl       /* Interrupt control, ir_ctl. */
};

/*!
* \brief Real Time Counter interrupt entry.
*/
static SIGNAL(RealTimeCounterIrqEntry)
{
    IRQ_ENTRY();
#ifdef NUT_PERFMON
    sig_RTC.ir_count++;
#endif
    if (sig_RTC.ir_handler) {
        (sig_RTC.ir_handler) (sig_RTC.ir_arg);
    }
    IRQ_EXIT();
}

/*!
 * \brief Real Time Counter interrupt control.
 *
 * \param cmd   Control command.
 *              - NUT_IRQCTL_INIT Initialize and disable interrupt.
 *              - NUT_IRQCTL_STATUS Query interrupt status.
 *              - NUT_IRQCTL_ENABLE Enable interrupt.
 *              - NUT_IRQCTL_DISABLE Disable interrupt.
 *              - NUT_IRQCTL_GETCOUNT Query and clear interrupt counter.
 * \param param Pointer to optional parameter.
 *
 * \return 0 on success, -1 otherwise.
 */
static int RealTimeCounterIrqCtl(int cmd, void *param)
{
    int rc = 0;
    unsigned int *ival = (unsigned int *) param;
    int_fast8_t enabled = AVR32_RTC.imr & AVR32_RTC_IMR_TOPI_MASK;

    /* Disable interrupt. */
    if (enabled) {
        AVR32_RTC.idr = AVR32_RTC_IDR_TOPI_MASK;
        AVR32_RTC.imr;
    }

    switch (cmd) {
    case NUT_IRQCTL_INIT:
        /* Enable 32k oscillator crystal */
        pm_enable_osc32_crystal(&AVR32_PM);
        pm_enable_clk32_no_wait(&AVR32_PM, AVR32_PM_OSCCTRL32_STARTUP_0_RCOSC);

        /* Wait until the rtc CTRL register is up-to-date */
        while ((AVR32_RTC.ctrl & AVR32_RTC_CTRL_BUSY_MASK));

        /* Set the new RTC configuration to 1ms */
        AVR32_RTC.ctrl = 1 << AVR32_RTC_CTRL_CLK32_OFFSET | 4 << AVR32_RTC_CTRL_PSEL_OFFSET | AVR32_RTC_CTRL_CLKEN_MASK;

        /* Wait until the rtc CTRL register is up-to-date */
        while ((AVR32_RTC.ctrl & AVR32_RTC_CTRL_BUSY_MASK));

        register_interrupt(RealTimeCounterIrqEntry, AVR32_RTC_IRQ, NUT_IRQPRI_RTC);

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
        break;
    case NUT_IRQCTL_DISABLE:
        enabled = 0;
        break;
#ifdef NUT_PERFMON
    case NUT_IRQCTL_GETCOUNT:
        *ival = (unsigned int) sig_TC0.ir_count;
        sig_RTC.ir_count = 0;
        break;
#endif
    default:
        rc = -1;
        break;
    }

    /* Enable interrupt. */
    if (enabled) {
        AVR32_RTC.ier = AVR32_RTC_IER_TOPI_MASK;
    }
    return rc;
}
