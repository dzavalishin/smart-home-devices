/*
 * Copyright (C) 2005-2007 by egnite Software GmbH. All rights reserved.
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
 * $Id: ih_at91pioc.c 3449 2011-05-31 19:08:15Z mifi $
 */

#include <arch/arm.h>
#include <dev/irqreg.h>

#if defined(PIOC_ID)

#ifndef NUT_IRQPRI_PIOC
#define NUT_IRQPRI_PIOC 4
#endif

static int PortIoIrqCtl(int cmd, void *param);

IRQ_HANDLER sig_PIOC = {
#ifdef NUT_PERFMON
    0,                  /* Interrupt counter, ir_count. */
#endif
    NULL,               /* Passed argument, ir_arg. */
    NULL,               /* Handler subroutine, ir_handler. */
    PortIoIrqCtl        /* Interrupt control, ir_ctl. */
};

/*!
 * \brief Port I/O interrupt entry.
 */
static void PortIoIrqEntry(void) __attribute__ ((naked));
void PortIoIrqEntry(void)
{
    IRQ_ENTRY();
#ifdef NUT_PERFMON
    sig_PIOC.ir_count++;
#endif
    if (sig_PIOC.ir_handler) {
        (sig_PIOC.ir_handler) (sig_PIOC.ir_arg);
    }
    IRQ_EXIT();
}

/*!
 * \brief Port I/O interrupt control.
 *
 * \param cmd   Control command.
 *              - NUT_IRQCTL_INIT Initialize and disable interrupt.
 *              - NUT_IRQCTL_STATUS Query interrupt status.
 *              - NUT_IRQCTL_ENABLE Enable interrupt.
 *              - NUT_IRQCTL_DISABLE Disable interrupt.
 *              - NUT_IRQCTL_GETMODE Query interrupt mode.
 *              - NUT_IRQCTL_SETMODE Set interrupt mode (NUT_IRQMODE_LEVEL or NUT_IRQMODE_EDGE).
 *              - NUT_IRQCTL_GETPRIO Query interrupt priority.
 *              - NUT_IRQCTL_SETPRIO Set interrupt priority.
 *              - NUT_IRQCTL_GETCOUNT Query and clear interrupt counter.
 * \param param Pointer to optional parameter.
 *
 * \return 0 on success, -1 otherwise.
 */
static int PortIoIrqCtl(int cmd, void *param)
{
    int rc = 0;
    unsigned int *ival = (unsigned int *)param;
    int_fast8_t enabled = inr(AIC_IMR) & _BV(PIOC_ID);

    /* Disable interrupt. */
    if (enabled) {
        outr(AIC_IDCR, _BV(PIOC_ID));
    }

    switch(cmd) {
    case NUT_IRQCTL_INIT:
        /* Set the vector. */
        outr(AIC_SVR(PIOC_ID), (unsigned int)PortIoIrqEntry);
        /* Initialize to edge triggered with defined priority. */
        outr(AIC_SMR(PIOC_ID), AIC_SRCTYPE_INT_EDGE_TRIGGERED | NUT_IRQPRI_PIOC);
        /* Clear interrupt */
        outr(AIC_ICCR, _BV(PIOC_ID));
        break;
    case NUT_IRQCTL_STATUS:
        if (enabled) {
            *ival |= 1;
        }
        else {
            *ival &= ~1;
        }
        break;
    case NUT_IRQCTL_ENABLE:
        enabled = 1;
        break;
    case NUT_IRQCTL_DISABLE:
        enabled = 0;
        break;
    case NUT_IRQCTL_GETMODE:
        {
            unsigned int val = inr(AIC_SMR(PIOC_ID)) & AIC_SRCTYPE;
            if (val == AIC_SRCTYPE_INT_LEVEL_SENSITIVE || val == AIC_SRCTYPE_EXT_HIGH_LEVEL) {
                *ival = NUT_IRQMODE_LEVEL;
            } else  {
                *ival = NUT_IRQMODE_EDGE;
            }
        }
        break;
    case NUT_IRQCTL_SETMODE:
        if (*ival == NUT_IRQMODE_LEVEL) {
            outr(AIC_SMR(PIOC_ID), (inr(AIC_SMR(PIOC_ID)) & ~AIC_SRCTYPE) | AIC_SRCTYPE_INT_LEVEL_SENSITIVE);
        } else if (*ival == NUT_IRQMODE_EDGE) {
            outr(AIC_SMR(PIOC_ID), (inr(AIC_SMR(PIOC_ID)) & ~AIC_SRCTYPE) | AIC_SRCTYPE_INT_EDGE_TRIGGERED);
        } else  {
            rc = -1;
        }
        break;
    case NUT_IRQCTL_GETPRIO:
        *ival = inr(AIC_SMR(PIOC_ID)) & AIC_PRIOR;
        break;
    case NUT_IRQCTL_SETPRIO:
        outr(AIC_SMR(PIOC_ID), (inr(AIC_SMR(PIOC_ID)) & ~AIC_PRIOR) | *ival);
        break;
#ifdef NUT_PERFMON
    case NUT_IRQCTL_GETCOUNT:
        *ival = (unsigned int)sig_PIOC.ir_count;
        sig_PIOC.ir_count = 0;
        break;
#endif
    default:
        rc = -1;
        break;
    }

    /* Enable interrupt. */
    if (enabled) {
        outr(AIC_IECR, _BV(PIOC_ID));
    }
    return rc;
}

#endif /* PIOC_ID */
