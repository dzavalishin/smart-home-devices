/*
 * Copyright (C) 2001-2005 by EmbeddedIT, 
 * Ole Reinhardt <ole.reinhardt@embedded-it.de> All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY EMBEDDED IT AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL EMBEDDED IT
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * For additional information see http://www.ethernut.de/
 *
 */

/*
 * $Log$
 * Revision 1.3  2008/08/11 06:59:12  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.2  2008/07/26 09:43:01  haraldkipp
 * Added support for retrieving and setting the interrupt mode.
 *
 * Revision 1.1  2007/09/06 19:36:00  olereinhardt
 * First checkin, new twi driver for at91 (currently SAM7X256 is supported
 * only)
 *
 *
 */

#include <arch/arm.h>
#include <dev/irqreg.h>

#if defined(TWI_ID)

#ifndef NUT_IRQPRI_TWI
#define NUT_IRQPRI_TWI  4
#endif

static int TwoWireIrqCtl(int cmd, void *param);

IRQ_HANDLER sig_TWI = {
#ifdef NUT_PERFMON
    0,                  /* Interrupt counter, ir_count. */
#endif
    NULL,               /* Passed argument, ir_arg. */
    NULL,               /* Handler subroutine, ir_handler. */
    TwoWireIrqCtl       /* Interrupt control, ir_ctl. */
};

/*!
 * \brief Two Wire Interface interrupt entry.
 */
static void TwoWireIrqEntry(void) __attribute__ ((naked));
void TwoWireIrqEntry(void)
{
    IRQ_ENTRY();
#ifdef NUT_PERFMON
    sig_TWI.ir_count++;
#endif
    if (sig_TWI.ir_handler) {
        (sig_TWI.ir_handler) (sig_TWI.ir_arg);
    }
    IRQ_EXIT();
}

/*!
 * \brief Two Wire Interface interrupt control.
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
static int TwoWireIrqCtl(int cmd, void *param)
{
    int rc = 0;
    unsigned int *ival = (unsigned int *)param;
    int_fast8_t enabled = inr(AIC_IMR) & _BV(TWI_ID);

    /* Disable interrupt. */
    if (enabled) {
        outr(AIC_IDCR, _BV(TWI_ID));
    }

    switch(cmd) {
    case NUT_IRQCTL_INIT:
        /* Set the vector. */
        outr(AIC_SVR(TWI_ID), (unsigned int)TwoWireIrqEntry);
        /* Initialize to edge triggered with defined priority. */
        outr(AIC_SMR(TWI_ID), AIC_SRCTYPE_INT_EDGE_TRIGGERED | NUT_IRQPRI_TWI);
        /* Clear interrupt */
        outr(AIC_ICCR, _BV(TWI_ID));
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
            unsigned int val = inr(AIC_SMR(TWI_ID)) & AIC_SRCTYPE;
            if (val == AIC_SRCTYPE_INT_LEVEL_SENSITIVE || val == AIC_SRCTYPE_EXT_HIGH_LEVEL) {
                *ival = NUT_IRQMODE_LEVEL;
            } else  {
                *ival = NUT_IRQMODE_EDGE;
            }
        }
        break;
    case NUT_IRQCTL_SETMODE:
        if (*ival == NUT_IRQMODE_LEVEL) {
            outr(AIC_SMR(TWI_ID), (inr(AIC_SMR(TWI_ID)) & ~AIC_SRCTYPE) | AIC_SRCTYPE_INT_LEVEL_SENSITIVE);
        } else if (*ival == NUT_IRQMODE_EDGE) {
            outr(AIC_SMR(TWI_ID), (inr(AIC_SMR(TWI_ID)) & ~AIC_SRCTYPE) | AIC_SRCTYPE_INT_EDGE_TRIGGERED);
        } else  {
            rc = -1;
        }
        break;
    case NUT_IRQCTL_GETPRIO:
        *ival = inr(AIC_SMR(TWI_ID)) & AIC_PRIOR;
        break;
    case NUT_IRQCTL_SETPRIO:
        outr(AIC_SMR(TWI_ID), (inr(AIC_SMR(TWI_ID)) & ~AIC_PRIOR) | *ival);
        break;
#ifdef NUT_PERFMON
    case NUT_IRQCTL_GETCOUNT:
        *ival = (unsigned int)sig_TWI.ir_count;
        sig_TWI.ir_count = 0;
        break;
#endif
    default:
        rc = -1;
        break;
    }

    /* Enable interrupt. */
    if (enabled) {
        outr(AIC_IECR, _BV(TWI_ID));
    }
    return rc;
}

#endif /* TWI_ID */
