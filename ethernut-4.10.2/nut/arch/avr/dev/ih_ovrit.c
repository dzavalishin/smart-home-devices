/*
 * Copyright (C) 2005 by egnite Software GmbH. All rights reserved.
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
 * \file arch/avr/dev/ih_ovrit.c
 * \brief CAN overflow interrupt.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.4  2009/01/17 11:26:37  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.3  2008/08/11 06:59:15  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.2  2006/02/28 02:16:56  hwmaier
 * Implemented AvrCanOvrIrqCtl function.
 *
 * Revision 1.1  2006/02/08 15:14:21  haraldkipp
 * Using the vector number as a file name wasn't a good idea.
 * Moved from ivect*.c
 *
 * Revision 1.4  2006/01/25 09:38:51  haraldkipp
 * Applied Thiago A. Correa's patch to fix ICC warnings.
 *
 * Revision 1.3  2005/10/24 09:35:48  haraldkipp
 * New interrupt control function added to allow future platform
 * independant drivers.
 *
 * Revision 1.2  2005/10/04 05:23:37  hwmaier
 * Updated interrupt hooks to reflect new CAN interrupt names of avr-lib 1.2.3
 *
 * Revision 1.1  2005/07/26 18:02:40  haraldkipp
 * Moved from dev.
 *
 * Revision 1.1  2005/02/10 07:06:18  hwmaier
 * Changes to incorporate support for AT90CAN128 CPU
 *
 * \endverbatim
 */

#include <dev/irqreg.h>

/*!
 * \addtogroup xgIrqReg
 */
/*@{*/

#if defined(SIG_CAN_OVERFLOW1) || defined(iv_CAN_TIM_OVR)

static int AvrCanOvrIrqCtl(int cmd, void *param);

IRQ_HANDLER sig_CAN_OVERRUN = {
#ifdef NUT_PERFMON
    0,                          /* Interrupt counter, ir_count. */
#endif
    NULL,                       /* Passed argument, ir_arg. */
    NULL,                       /* Handler subroutine, ir_handler. */
    AvrCanOvrIrqCtl             /* Interrupt control, ir_ctl. */
};

/*!
 * \brief CAN overflow interrupt control.
 *
 * \param cmd   Control command.
 * \param param Pointer to optional parameter.
 *
 * \return 0 on success, -1 otherwise.
 */
static int AvrCanOvrIrqCtl(int cmd, void *param)
{
    int rc = 0;
    unsigned int *ival = (unsigned int *) param;
    int_fast8_t enabled = bit_is_set(CANGIE, ENOVRT);

    /* Disable interrupt. */
    cbi(CANGIE, ENOVRT);

    switch (cmd) {
    case NUT_IRQCTL_INIT:
    case NUT_IRQCTL_DISABLE:
        enabled = 0;
        break;
    case NUT_IRQCTL_ENABLE:
        enabled = 1;
        break;
    case NUT_IRQCTL_GETPRIO:
        *ival = 0;
        break;
#ifdef NUT_PERFMON
    case NUT_IRQCTL_GETCOUNT:
        *ival = (unsigned int) SIG_CAN_OVERFLOW1.ir_count;
        SIG_CAN_OVERFLOW1.ir_count = 0;
        break;
#endif
    default:
        rc = -1;
        break;
    }

    /* Enable interrupt. */
    if (enabled) {
        sbi(CANGIE, ENOVRT);
    }
    return rc;
}

/*! \fn SIG_CAN_OVERFLOW1(void)
 * \brief CAN overflow interrupt entry.
 */
#ifdef __IMAGECRAFT__
#pragma interrupt_handler SIG_CAN_OVERFLOW1:iv_CAN_TIM_OVR
#endif
NUTSIGNAL(SIG_CAN_OVERFLOW1, sig_CAN_OVERRUN)
#endif
/*@}*/
