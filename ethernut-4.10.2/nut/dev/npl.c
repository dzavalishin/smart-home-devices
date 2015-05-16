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
 */

/*!
 * \brief Nut Programmable Logic
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.4  2009/01/17 11:26:46  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.3  2008/08/11 06:59:42  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.2  2006/05/25 09:30:23  haraldkipp
 * Compiles for AVR. Still not tested, though.
 *
 * Revision 1.1  2006/01/05 16:30:57  haraldkipp
 * First check-in.
 *
 *
 * \endverbatim
 */

#include <dev/npl.h>

#if 0
/* Use for local debugging. */
#define NUTDEBUG
#include <stdio.h>
#endif

/*!
 * \addtogroup xgNpl
 */
/*@{*/

static int npl_registered;

/*
 * NIC interrupt entry.
 */
static void NplInterrupt(void *arg)
{
    uint16_t slr;

    /* Read signal latch register and clear all enabled interrupts. */
    slr = inw(NPL_SLR) & inw(NPL_IMR);

    /* RS232 CTS activated. */
    if (slr & NPL_RSCTS) {
        if (sig_RSCTS.ir_handler) {
            (sig_RSCTS.ir_handler) (sig_RSCTS.ir_arg);
        }
    }
    /* RS232 DSR activated. */
    if (slr & NPL_RSDSR) {
        if (sig_RSDSR.ir_handler) {
            (sig_RSDSR.ir_handler) (sig_RSDSR.ir_arg);
        }
    }
    /* RS232 DCD activated. */
    if (slr & NPL_RSDCD) {
        if (sig_RSDCD.ir_handler) {
            (sig_RSDCD.ir_handler) (sig_RSDCD.ir_arg);
        }
    }
    /* RS232 RI activated. */
    if (slr & NPL_RSRI) {
        if (sig_RSRI.ir_handler) {
            (sig_RSRI.ir_handler) (sig_RSRI.ir_arg);
        }
    }
    /* RTC Alarm activated. */
    if (slr & NPL_RTCALARM) {
        if (sig_RTCALARM.ir_handler) {
            (sig_RTCALARM.ir_handler) (sig_RTCALARM.ir_arg);
        }
    }
    /* LAN wakeup activated. */
    if (slr & NPL_LANWAKEUP) {
        if (sig_LANWAKEUP.ir_handler) {
            (sig_LANWAKEUP.ir_handler) (sig_LANWAKEUP.ir_arg);
        }
    }
    /* FLASH ready activated. */
    if (slr & NPL_FMBUSY) {
        if (sig_FMBUSY.ir_handler) {
            (sig_FMBUSY.ir_handler) (sig_FMBUSY.ir_arg);
        }
    }
    /* RS232 signal valid. */
    if (slr & NPL_RSINVAL) {
        if (sig_RSINVAL.ir_handler) {
            (sig_RSINVAL.ir_handler) (sig_RSINVAL.ir_arg);
        }
    }
    /* RS232 signal invalid. */
    if (slr & NPL_NRSINVAL) {
        if (sig_NRSINVAL.ir_handler) {
            (sig_NRSINVAL.ir_handler) (sig_NRSINVAL.ir_arg);
        }
    }
    /* Multimedia card inserted. */
    if (slr & NPL_MMCD) {
        if (sig_MMCD.ir_handler) {
            (sig_MMCD.ir_handler) (sig_MMCD.ir_arg);
        }
    }
    /* Multimedia card removed. */
    if (slr & NPL_NMMCD) {
        if (sig_NMMCD.ir_handler) {
            (sig_NMMCD.ir_handler) (sig_NMMCD.ir_arg);
        }
    }
    outw(NPL_SCR, slr);
}

static int NplIrqCtl(int cmd, void *param, IRQ_HANDLER * irq, unsigned int mask)
{
    int rc = 0;
    unsigned int *ival = (unsigned int *) param;
    uint16_t enabled = inw(NPL_IMR) & mask;

    /* Disable interrupt. */
    if (enabled) {
        outw(NPL_IMR, enabled & ~mask);
    }
    switch (cmd) {
    case NUT_IRQCTL_INIT:
        /* Clear interrupt */
        outw(NPL_SCR, mask);
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
        *ival = (unsigned int) irq->ir_count;
        irq->ir_count = 0;
        break;
#endif
    default:
        rc = -1;
        break;
    }

    /* Enable interrupt. */
    if (enabled) {
        outw(NPL_IMR, inw(NPL_IMR) | mask);
    }
    return rc;
}


static int NplRsCtsCtl(int cmd, void *param)
{
    return NplIrqCtl(cmd, param, &sig_RSCTS, NPL_RSCTS);
}

static int NplRsDsrCtl(int cmd, void *param)
{
    return NplIrqCtl(cmd, param, &sig_RSDSR, NPL_RSDSR);
}

static int NplRsDcdCtl(int cmd, void *param)
{
    return NplIrqCtl(cmd, param, &sig_RSDCD, NPL_RSDCD);
}

static int NplRsRiCtl(int cmd, void *param)
{
    return NplIrqCtl(cmd, param, &sig_RSRI, NPL_RSRI);
}

static int NplRtcAlarmCtl(int cmd, void *param)
{
    return NplIrqCtl(cmd, param, &sig_RTCALARM, NPL_RTCALARM);
}

static int NplLanWakeupCtl(int cmd, void *param)
{
    return NplIrqCtl(cmd, param, &sig_LANWAKEUP, NPL_LANWAKEUP);
}

static int NplFmBusyCtl(int cmd, void *param)
{
    return NplIrqCtl(cmd, param, &sig_FMBUSY, NPL_FMBUSY);
}

static int NplRsInvalCtl(int cmd, void *param)
{
    return NplIrqCtl(cmd, param, &sig_RSINVAL, NPL_RSINVAL);
}

static int NplRsValidCtl(int cmd, void *param)
{
    return NplIrqCtl(cmd, param, &sig_NRSINVAL, NPL_NRSINVAL);
}

static int NplMmcInsertCtl(int cmd, void *param)
{
    return NplIrqCtl(cmd, param, &sig_MMCD, NPL_MMCD);
}

static int NplMmcRemoveCtl(int cmd, void *param)
{
    return NplIrqCtl(cmd, param, &sig_NMMCD, NPL_NMMCD);
}


/*!
 * \brief Register an NPL interrupt handler.
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
int NplRegisterIrqHandler(IRQ_HANDLER * irq, void (*handler) (void *), void *arg)
{
    int rc;

    /* Initialize this interrupt. */
    rc = (irq->ir_ctl) (NUT_IRQCTL_INIT, NULL);

    if (rc == 0) {
        /* Set the interrupt handler. */
        irq->ir_arg = arg;
        irq->ir_handler = handler;

        if (!npl_registered) {
            npl_registered = 1;
#if defined(ETHERNUT3)
            outr(PIO_PDR, _BV(9));
#endif
            if ((rc = NutRegisterIrqHandler(&sig_INTERRUPT0, NplInterrupt, NULL)) == 0) {
                NutIrqSetMode(&sig_INTERRUPT0, NUT_IRQMODE_LOWLEVEL);
                NutIrqEnable(&sig_INTERRUPT0);
            }
        }
    }
    return rc;
}

/*!
 * \brief Enable a specified NPL interrupt.
 *
 * \param irq Interrupt to enable.
 *
 * \return 0 on success, -1 otherwise.
 */
int NplIrqEnable(IRQ_HANDLER * irq)
{
    return (irq->ir_ctl) (NUT_IRQCTL_ENABLE, NULL);
}

/*!
 * \brief Disable a specified NPL interrupt.
 *
 * \param irq Interrupt to disable.
 *
 * \return 0 on success, -1 otherwise.
 */
int NplIrqDisable(IRQ_HANDLER * irq)
{
    return (irq->ir_ctl) (NUT_IRQCTL_DISABLE, NULL);
}

/*!
 * \brief RS232 CTS interrupt handler info.
 */
IRQ_HANDLER sig_RSCTS = {
#ifdef NUT_PERFMON
    0,                          /* Interrupt counter, ir_count. */
#endif
    NULL,                       /* Passed argument, ir_arg. */
    NULL,                       /* Handler subroutine, ir_handler. */
    NplRsCtsCtl                 /* Interrupt control, ir_ctl. */
};

/*!
 * \brief RS232 DSR interrupt handler info.
 */
IRQ_HANDLER sig_RSDSR = {
#ifdef NUT_PERFMON
    0,                          /* Interrupt counter, ir_count. */
#endif
    NULL,                       /* Passed argument, ir_arg. */
    NULL,                       /* Handler subroutine, ir_handler. */
    NplRsDsrCtl                 /* Interrupt control, ir_ctl. */
};

/*!
 * \brief RS232 DCD interrupt handler info.
 */
IRQ_HANDLER sig_RSDCD = {
#ifdef NUT_PERFMON
    0,                          /* Interrupt counter, ir_count. */
#endif
    NULL,                       /* Passed argument, ir_arg. */
    NULL,                       /* Handler subroutine, ir_handler. */
    NplRsDcdCtl                 /* Interrupt control, ir_ctl. */
};

/*!
 * \brief RS232 RI interrupt handler info.
 */
IRQ_HANDLER sig_RSRI = {
#ifdef NUT_PERFMON
    0,                          /* Interrupt counter, ir_count. */
#endif
    NULL,                       /* Passed argument, ir_arg. */
    NULL,                       /* Handler subroutine, ir_handler. */
    NplRsRiCtl                  /* Interrupt control, ir_ctl. */
};

/*!
 * \brief RTC alarm interrupt handler info.
 */
IRQ_HANDLER sig_RTCALARM = {
#ifdef NUT_PERFMON
    0,                          /* Interrupt counter, ir_count. */
#endif
    NULL,                       /* Passed argument, ir_arg. */
    NULL,                       /* Handler subroutine, ir_handler. */
    NplRtcAlarmCtl              /* Interrupt control, ir_ctl. */
};

/*!
 * \brief LAN wakeup interrupt handler info.
 */
IRQ_HANDLER sig_LANWAKEUP = {
#ifdef NUT_PERFMON
    0,                          /* Interrupt counter, ir_count. */
#endif
    NULL,                       /* Passed argument, ir_arg. */
    NULL,                       /* Handler subroutine, ir_handler. */
    NplLanWakeupCtl             /* Interrupt control, ir_ctl. */
};

/*!
 * \brief Flash memory busy interrupt handler info.
 */
IRQ_HANDLER sig_FMBUSY = {
#ifdef NUT_PERFMON
    0,                          /* Interrupt counter, ir_count. */
#endif
    NULL,                       /* Passed argument, ir_arg. */
    NULL,                       /* Handler subroutine, ir_handler. */
    NplFmBusyCtl                /* Interrupt control, ir_ctl. */
};

/*!
 * \brief RS232 signal invalid interrupt handler info.
 */
IRQ_HANDLER sig_RSINVAL = {
#ifdef NUT_PERFMON
    0,                          /* Interrupt counter, ir_count. */
#endif
    NULL,                       /* Passed argument, ir_arg. */
    NULL,                       /* Handler subroutine, ir_handler. */
    NplRsInvalCtl               /* Interrupt control, ir_ctl. */
};

/*!
 * \brief RS232 signal valid interrupt handler info.
 */
IRQ_HANDLER sig_NRSINVAL = {
#ifdef NUT_PERFMON
    0,                          /* Interrupt counter, ir_count. */
#endif
    NULL,                       /* Passed argument, ir_arg. */
    NULL,                       /* Handler subroutine, ir_handler. */
    NplRsValidCtl               /* Interrupt control, ir_ctl. */
};

/*!
 * \brief Multimedia card insertion interrupt handler info.
 */
IRQ_HANDLER sig_MMCD = {
#ifdef NUT_PERFMON
    0,                          /* Interrupt counter, ir_count. */
#endif
    NULL,                       /* Passed argument, ir_arg. */
    NULL,                       /* Handler subroutine, ir_handler. */
    NplMmcInsertCtl             /* Interrupt control, ir_ctl. */
};

/*!
 * \brief Multimedia card removal interrupt handler info.
 */
IRQ_HANDLER sig_NMMCD = {
#ifdef NUT_PERFMON
    0,                          /* Interrupt counter, ir_count. */
#endif
    NULL,                       /* Passed argument, ir_arg. */
    NULL,                       /* Handler subroutine, ir_handler. */
    NplMmcRemoveCtl             /* Interrupt control, ir_ctl. */
};

/*@}*/
