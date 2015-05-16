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
 * \file arch/avr/dev/ih_int5.c
 * \brief AVR external interrupt 5.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.3  2008/08/11 06:59:15  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.2  2006/10/08 16:48:07  haraldkipp
 * Documentation fixed
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
 * Revision 1.2  2004/01/30 17:02:19  drsung
 * Separate interrupt stack for avr-gcc only added.
 *
 * Revision 1.1.1.1  2003/05/09 14:40:41  haraldkipp
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

static int AvrInterrupt5Ctl(int cmd, void *param);

IRQ_HANDLER sig_INTERRUPT5 = {
#ifdef NUT_PERFMON
    0,                          /* Interrupt counter, ir_count. */
#endif
    NULL,                       /* Passed argument, ir_arg. */
    NULL,                       /* Handler subroutine, ir_handler. */
    AvrInterrupt5Ctl            /* Interrupt control, ir_ctl. */
};

/*!
 * \brief External interrupt 5 control.
 *
 * \param cmd   Control command.
 *              - NUT_IRQCTL_INIT Initialize and disable interrupt.
 *              - NUT_IRQCTL_STATUS Query interrupt status.
 *              - NUT_IRQCTL_ENABLE Enable interrupt.
 *              - NUT_IRQCTL_DISABLE Disable interrupt.
 *              - NUT_IRQCTL_GETMODE Query interrupt mode.
 *              - NUT_IRQCTL_SETMODE Set interrupt mode.
 *              - NUT_IRQCTL_GETPRIO Query interrupt priority.
 *              - NUT_IRQCTL_SETPRIO Set interrupt priority.
 *              - NUT_IRQCTL_GETCOUNT Query and clear interrupt counter.
 * \param param Pointer to optional parameter.
 *
 * \return 0 on success, -1 otherwise.
 */
static int AvrInterrupt5Ctl(int cmd, void *param)
{
    int rc = 0;
    unsigned int *ival = (unsigned int *) param;
    int_fast8_t enabled = bit_is_set(EIMSK, INT5);
    uint8_t bval;

    /* Disable interrupt. */
    cbi(EIMSK, INT5);

    switch (cmd) {
    case NUT_IRQCTL_INIT:
        /* Initialize to low level triggered. */
        cbi(EICR, ISC50);
        cbi(EICR, ISC51);
    case NUT_IRQCTL_CLEAR:
        /* Clear any pending interrupt. */
        outb(EIFR, _BV(INTF5));
        break;
    case NUT_IRQCTL_STATUS:
        if (bit_is_set(EIFR, INTF5)) {
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
    case NUT_IRQCTL_GETMODE:
        bval = inb(EICR) & _BV(ISC51 | ISC50);
        if (bval == _BV(ISC51)) {
            *ival = NUT_IRQMODE_FALLINGEDGE;
        } else if (bval == _BV(ISC50)) {
            *ival = NUT_IRQMODE_EDGE;
        } else if (bval == _BV(ISC51 | ISC50)) {
            *ival = NUT_IRQMODE_RISINGEDGE;
        } else {
            *ival = NUT_IRQMODE_LOWLEVEL;
        }
        break;
    case NUT_IRQCTL_SETMODE:
        if (*ival == NUT_IRQMODE_LOWLEVEL) {
            cbi(EICR, ISC50);
            cbi(EICR, ISC51);
        } else if (*ival == NUT_IRQMODE_EDGE) {
            sbi(EICR, ISC50);
            cbi(EICR, ISC51);
        } else if (*ival == NUT_IRQMODE_FALLINGEDGE) {
            cbi(EICR, ISC50);
            sbi(EICR, ISC51);
        } else if (*ival == NUT_IRQMODE_RISINGEDGE) {
            sbi(EICR, ISC50);
            sbi(EICR, ISC51);
        } else {
            rc = -1;
        }
        break;
    case NUT_IRQCTL_GETPRIO:
        *ival = 5;
        break;
#ifdef NUT_PERFMON
    case NUT_IRQCTL_GETCOUNT:
        *ival = (unsigned int) sig_INTERRUPT5.ir_count;
        sig_INTERRUPT5.ir_count = 0;
        break;
#endif
    default:
        rc = -1;
        break;
    }

    /* Enable interrupt. */
    if (enabled) {
        sbi(EIMSK, INT5);
    }
    return rc;
}

/*! \fn SIG_INTERRUPT5(void)
 * \brief External interrupt 5 entry.
 */
#ifdef __IMAGECRAFT__
#pragma interrupt_handler SIG_INTERRUPT5:iv_INT5
#endif
NUTSIGNAL(SIG_INTERRUPT5, sig_INTERRUPT5)


/*@}*/
