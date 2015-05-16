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

/*
 * $Log$
 * Revision 1.8  2009/03/05 22:16:57  freckle
 * use __NUT_EMULATION instead of __APPLE__, __linux__, or __CYGWIN__
 *
 * Revision 1.7  2008/07/26 09:38:02  haraldkipp
 * Added support for NUT_IRQMODE_NONE and NUT_IRQMODE_LEVEL.
 *
 * Revision 1.6  2006/10/08 16:48:09  haraldkipp
 * Documentation fixed
 *
 * Revision 1.5  2006/01/05 16:51:54  haraldkipp
 * NutIrqSetPriority() didn't correctly return the previous priority. This
 * bug has been fixed.
 * New function NutIrqSetMode() allows to modify the interrupt modes.
 *
 * Revision 1.4  2005/10/24 10:17:24  haraldkipp
 * New API functions added to create platform independant drivers.
 * Interrupt counting requires NUT_PERFMON to be defined.
 *
 * Revision 1.3  2005/08/02 17:46:47  haraldkipp
 * Major API documentation update.
 *
 * Revision 1.2  2005/07/26 16:30:58  haraldkipp
 * Copyright added.
 * Temporarily exclude NutRegisterIrqHandler() from Linux builds.
 *
 */

#include <sys/atom.h>
#include <dev/irqreg.h>

/*!
 * \addtogroup xgInterrupt
 */
/*@{*/

/*!
 * \brief Call a registered interrupt handler.
 */
void CallHandler(IRQ_HANDLER * irh)
{
#ifdef NUT_PERFMON
    irh->ir_count++;
#endif
    if (irh->ir_handler)
        (irh->ir_handler) (irh->ir_arg);
}

/*!
 * \brief Register an interrupt handler.
 *
 * This function is typically called by device drivers, but
 * applications may also implement their local interrupt
 * handlers.
 *
 * \param irq     Interrupt to be associated with this handler.
 * \param handler This routine will be called by Nut/OS, when the 
 *                specified interrupt occurs.
 * \param arg     Argument to be passed to the interrupt handler.
 *
 * \return 0 on success, -1 otherwise.
 */
#ifndef __NUT_EMULATION__
int NutRegisterIrqHandler(IRQ_HANDLER * irq, void (*handler) (void *), void *arg)
{
    int rc = 0;

    /* Initialize this interrupt. */
    if (irq->ir_ctl) {
        rc = (irq->ir_ctl) (NUT_IRQCTL_INIT, NULL);
    }

    /* Set the interrupt handler. */
    irq->ir_arg = arg;
    irq->ir_handler = handler;

    return rc;
}

/*!
 * \brief Enable a specified interrupt.
 *
 * \param irq Interrupt to enable.
 *
 * \return 0 on success, -1 otherwise.
 */
int NutIrqEnable(IRQ_HANDLER * irq)
{
    int rc = -1;

    if (irq->ir_ctl) {
        rc = (irq->ir_ctl) (NUT_IRQCTL_ENABLE, NULL);
    }
    return rc;
}

/*!
 * \brief Disable a specified interrupt.
 *
 * \param irq Interrupt to disable.
 *
 * \return 0 on success, -1 otherwise.
 */
int NutIrqDisable(IRQ_HANDLER * irq)
{
    int rc = -1;

    if (irq->ir_ctl) {
        rc = (irq->ir_ctl) (NUT_IRQCTL_DISABLE, NULL);
    }
    return rc;
}

/*!
 * \brief Modify the priority level of an interrupt.
 *
 * The function returns the old priority, which makes it easy to 
 * temporarily switch to another priority and later set back the 
 * old one.
 *
 * \note Not all targets support dynamic interrupt prioritization.
 *       Check the hardware data sheet for valid levels.
 *
 * \param irq   Interrupt to modify.
 * \param level New priority level.
 *
 * \return Old priority level or -1 in case of an error.
 */
int NutIrqSetPriority(IRQ_HANDLER * irq, int level)
{
    int rc = -1;

    if (irq->ir_ctl) {
        int prev;

        rc = (irq->ir_ctl) (NUT_IRQCTL_GETPRIO, &prev);
        if (rc == 0 && (rc = (irq->ir_ctl) (NUT_IRQCTL_SETPRIO, &level)) == 0) {
            rc = prev;
        }
    }
    return rc;
}

/*!
 * \brief Modify the interrupt mode.
 *
 * The function returns the old mode, which makes it easy to 
 * temporarily switch to another mode and later set back the 
 * old one.
 *
 * \note Not all targets support all modes. Check the hardware 
 *       data sheet for valid levels.
 *
 * \param irq  Interrupt to modify.
 * \param mode New interrupt mode:
 *             - NUT_IRQMODE_NONE No change. Used to query current mode.
 *             - NUT_IRQMODE_LOWLEVEL Low level sensitive.
 *             - NUT_IRQMODE_HIGHLEVEL High level sensitive.
 *             - NUT_IRQMODE_FALLINGEDGE Negative edge triggered.
 *             - NUT_IRQMODE_RISINGEDGE Positive edge triggered.
 *             - NUT_IRQMODE_EDGE Triggers on any edge or active internal edge.
 *             - NUT_IRQMODE_LEVEL Triggers on level change or active internal level.
 *
 *
 * \return Old mode or -1 in case of an error.
 */
int NutIrqSetMode(IRQ_HANDLER * irq, int mode)
{
    int rc = -1;

    if (irq->ir_ctl) {
        int prev;

        rc = (irq->ir_ctl) (NUT_IRQCTL_GETMODE, &prev);
        if (rc == 0 && (mode == NUT_IRQMODE_NONE || (rc = (irq->ir_ctl) (NUT_IRQCTL_SETMODE, &mode)) == 0)) {
            rc = prev;
        }
    }
    return rc;
}

#endif

/*@}*/
