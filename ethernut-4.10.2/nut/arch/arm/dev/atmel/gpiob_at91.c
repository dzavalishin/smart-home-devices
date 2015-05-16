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
 * $Id: gpiob_at91.c 3449 2011-05-31 19:08:15Z mifi $
 */

#include <arch/arm.h>

#include <stdlib.h>
#include <string.h>

#include <dev/gpio.h>

#if defined(PIOB_ISR)

/*!
 * \brief PIOB interrupt service.
 */
static void PioIsrB(void *arg)
{
    GPIO_VECTOR *vct = (GPIO_VECTOR *)arg;
    unsigned int isr;

    for (isr = inr(PIOB_ISR); isr; isr >>= 1, vct++) {
        if ((isr & 1) != 0 && vct->iov_handler) {
            (vct->iov_handler) (vct->iov_arg);
        }
    }
}

/*!
 * \brief PIOB interrupt control.
 */
static int PioCtlB(int cmd, void *param, int bit)
{
    int rc = 0;
    unsigned int *ival = (unsigned int *) param;
    uint32_t enabled = inr(PIOB_IMR) & _BV(bit);

    /* Disable interrupt. */
    if (enabled) {
        outr(PIOB_IDR, _BV(bit));
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
        outr(PIOB_IER, _BV(bit));
    }
    return rc;
}

GPIO_SIGNAL sig_GPIO2 = {
    &sig_PIOB,  /* ios_sig */
    PioIsrB,    /* ios_handler */
    PioCtlB,    /* ios_ctl */
    NULL        /* ios_vector */
};

#endif /* PIOB_ISR */
