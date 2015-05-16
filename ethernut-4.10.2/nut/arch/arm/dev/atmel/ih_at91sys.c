/*
 * Copyright (C) 2007 by egnite Software GmbH. All rights reserved.
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
 * Revision 1.2  2008/08/11 06:59:11  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.1  2007/02/15 16:11:14  haraldkipp
 * Support for system controller interrupts added.
 *
 */

#include <arch/arm.h>
#include <dev/irqreg.h>

#ifndef NUT_IRQPRI_SYS
#define NUT_IRQPRI_SYS  0
#endif

SYSIRQ_HANDLER syssig_DBGU;
SYSIRQ_HANDLER syssig_MC;
SYSIRQ_HANDLER syssig_PIT;
SYSIRQ_HANDLER syssig_PMC;
SYSIRQ_HANDLER syssig_RSTC;
SYSIRQ_HANDLER syssig_RTT;
SYSIRQ_HANDLER syssig_WDT;


static int SystemIrqCtl(int cmd, void *param);

IRQ_HANDLER sig_SYS = {
#ifdef NUT_PERFMON
    0,                  /* Interrupt counter, ir_count. */
#endif
    NULL,               /* Passed argument, ir_arg. */
    NULL,               /* Handler subroutine, ir_handler. */
    SystemIrqCtl /* Interrupt control, ir_ctl. */
};

/*!
 * \brief System interrupt entry.
 */
static void SystemIrqEntry(void) __attribute__ ((naked));
void SystemIrqEntry(void)
{
    IRQ_ENTRY();
#ifdef NUT_PERFMON
    sig_SYS.ir_count++;
#endif
    if (syssig_DBGU.sir_enabled && syssig_DBGU.sir_handler) {
        /* Call debug unit interrupt handler. */
        (syssig_DBGU.sir_handler) (syssig_DBGU.sir_arg);
    }
    if (syssig_MC.sir_enabled && syssig_MC.sir_handler) {
        /* Call memory controller interrupt handler. */
        (syssig_MC.sir_handler) (syssig_MC.sir_arg);
    }
    if (syssig_PMC.sir_enabled && syssig_PMC.sir_handler) {
        /* Call power management controller interrupt handler. */
        (syssig_PMC.sir_handler) (syssig_PMC.sir_arg);
    }
    if (syssig_RSTC.sir_enabled && syssig_RSTC.sir_handler) {
        /* Call reset controller interrupt handler. */
        (syssig_RSTC.sir_handler) (syssig_RSTC.sir_arg);
    }
    if (syssig_RTT.sir_enabled && syssig_RTT.sir_handler) {
        /* Call real-time timer interrupt handler. */
        (syssig_RTT.sir_handler) (syssig_RTT.sir_arg);
    }
    if (syssig_WDT.sir_enabled && syssig_WDT.sir_handler) {
        /* Call watchdog timer interrupt handler. */
        (syssig_WDT.sir_handler) (syssig_WDT.sir_arg);
    }
    if ((inr(PIT_MR) & PIT_PITIEN) != 0 && (inr(PIT_SR) & PIT_PITS) != 0) {
        if (syssig_PIT.sir_handler) {
            /* Call periodic interval timer interrupt handler. */
            (syssig_PIT.sir_handler) (syssig_PIT.sir_arg);
        }
        inr(PIT_PIVR);
    }
    IRQ_EXIT();
}

/*!
 * \brief Timer/Counter 0 interrupt control.
 *
 * \param cmd   Control command.
 *              - NUT_IRQCTL_INIT Initialize and disable interrupt.
 *              - NUT_IRQCTL_STATUS Query interrupt status.
 *              - NUT_IRQCTL_ENABLE Enable interrupt.
 *              - NUT_IRQCTL_DISABLE Disable interrupt.
 *              - NUT_IRQCTL_GETPRIO Query interrupt priority.
 *              - NUT_IRQCTL_SETPRIO Set interrupt priority.
 *              - NUT_IRQCTL_GETCOUNT Query and clear interrupt counter.
 * \param param Pointer to optional parameter.
 *
 * \return 0 on success, -1 otherwise.
 */
static int SystemIrqCtl(int cmd, void *param)
{
    int rc = 0;
    unsigned int *ival = (unsigned int *)param;
    int_fast8_t enabled = inr(AIC_IMR) & _BV(SYSC_ID);

    /* Disable interrupt. */
    if (enabled) {
        outr(AIC_IDCR, _BV(SYSC_ID));
    }

    switch(cmd) {
    case NUT_IRQCTL_INIT:
        /* Set the vector. */
        outr(AIC_SVR(SYSC_ID), (unsigned int)SystemIrqEntry);
        /* Initialize to level triggered with defined priority. */
        outr(AIC_SMR(SYSC_ID), AIC_SRCTYPE_INT_LEVEL_SENSITIVE | NUT_IRQPRI_SYS);
        /* Clear interrupt */
        outr(AIC_ICCR, _BV(SYSC_ID));
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
    case NUT_IRQCTL_GETPRIO:
        *ival = inr(AIC_SMR(SYSC_ID)) & AIC_PRIOR;
        break;
    case NUT_IRQCTL_SETPRIO:
        outr(AIC_SMR(SYSC_ID), (inr(AIC_SMR(SYSC_ID)) & ~AIC_PRIOR) | *ival);
        break;
#ifdef NUT_PERFMON
    case NUT_IRQCTL_GETCOUNT:
        *ival = (unsigned int)sig_SYS.ir_count;
        sig_SYS.ir_count = 0;
        break;
#endif
    default:
        rc = -1;
        break;
    }

    /* Enable interrupt. */
    if (enabled) {
        outr(AIC_IECR, _BV(SYSC_ID));
    }
    return rc;
}

/*!
 * \brief Enable a specified system interrupt.
 *
 * \param sysirq Interrupt to enable.
 *
 * \return 0 on success, -1 otherwise.
 */
int NutSysIrqEnable(SYSIRQ_HANDLER * sysirq)
{
    sysirq->sir_enabled = 1;

    return SystemIrqCtl(NUT_IRQCTL_ENABLE, NULL);
}

/*!
 * \brief Disable a specified system interrupt.
 *
 * \param sysirq Interrupt to disable.
 *
 * \return 0 on success, -1 otherwise.
 */
int NutSysIrqDisable(SYSIRQ_HANDLER * sysirq)
{
    sysirq->sir_enabled = 0;

    return 0;
}

/*!
 * \brief Register a system interrupt handler.
 *
 * This call will also disable any previously registered handler.
 *
 * If enabled with NutSysIrqEnable(), the registered handler will be 
 * called on every system interrupt. The handler must check, if the 
 * interrupt was triggered by the corresponding device.
 * 
 * This function is typically called by device drivers, but applications 
 * may also implement their local interrupt handlers.
 *
 * \param irq     Interrupt to be associated with this handler.
 * \param handler This routine will be called by Nut/OS, when the 
 *                specified interrupt occurs.
 * \param arg     Argument to be passed to the interrupt handler.
 *
 * \return 0 on success, -1 otherwise.
 */
int NutRegisterSysIrqHandler(SYSIRQ_HANDLER * sysirq, void (*handler) (void *), void *arg)
{
    int rc;

    /* Disable any previously registered handler. */
    NutSysIrqDisable(sysirq);

    /* Initialize this interrupt. */
    rc = SystemIrqCtl(NUT_IRQCTL_INIT, NULL);

    if (rc == 0) {
        /* Set the interrupt handler. */
        sysirq->sir_arg = arg;
        sysirq->sir_handler = handler;
    }
    return rc;
}
