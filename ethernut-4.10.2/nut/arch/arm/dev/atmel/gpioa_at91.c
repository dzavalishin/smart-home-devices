/*!
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
 */

/*!
 * $Id: gpioa_at91.c 3449 2011-05-31 19:08:15Z mifi $
 */

#include <arch/arm.h>

#include <stdlib.h>
#include <string.h>

#include <dev/gpio.h>

/* Reuse this code for single port devices. */
#if defined(PIO_ISR)
#define PIOA_ISR    PIO_ISR
#define PIOA_IMR    PIO_IMR
#define PIOA_IER    PIO_IER
#define PIOA_IDR    PIO_IDR
#endif

#if defined(PIOA_ISR)

/*!
 * \brief PIO interrupt service.
 */
static void PioIsrA(void *arg)
{
    GPIO_VECTOR *vct = (GPIO_VECTOR *)arg;
    unsigned int isr = inr(PIOA_ISR);

    while (isr) {
        if ((isr & 1) != 0 && vct->iov_handler) {
            (vct->iov_handler) (vct->iov_arg);
        }
        isr >>= 1;
        vct++;
    }
}

/*!
 * \brief PIO interrupt control.
 */
static int PioCtlA(int cmd, void *param, int bit)
{
    int rc = 0;
    unsigned int *ival = (unsigned int *) param;
    uint32_t enabled = inr(PIOA_IMR) & _BV(bit);

    /* Disable interrupt. */
    if (enabled) {
        outr(PIOA_IDR, _BV(bit));
    }
    switch (cmd) {
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
    default:
        rc = -1;
        break;
    }

    /* Enable interrupt. */
    if (enabled) {
        outr(PIOA_IER, _BV(bit));
    }
    return rc;
}

#if defined(PIO_ISR)
GPIO_SIGNAL sig_GPIO = {
    &sig_PIO,   /* ios_sig */
    PioIsrA,    /* ios_handler */
    PioCtlA,    /* ios_ctl */
    NULL        /* ios_vector */
};
#else
GPIO_SIGNAL sig_GPIO1 = {
    &sig_PIOA,  /* ios_sig */
    PioIsrA,    /* ios_handler */
    PioCtlA,    /* ios_ctl */
    NULL        /* ios_vector */
};
#endif /* PIO_ISR */

#endif
