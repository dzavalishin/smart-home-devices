#ifndef _DEV_GPIO_H_
#define _DEV_GPIO_H_

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
 * \file dev/gpio.h
 * \brief General purpose I/O.
 *
 * \verbatim
 * $Id: gpio.h 2560 2009-03-18 04:00:06Z thiagocorrea $
 * \endverbatim
 */

#include <cfg/arch/gpio.h>
#include <dev/irqreg.h>

#define NUTGPIO_PORT    0
#define NUTGPIO_PORTA   1
#define NUTGPIO_PORTB   2
#define NUTGPIO_PORTC   3
#define NUTGPIO_PORTD   4
#define NUTGPIO_PORTE   5
#define NUTGPIO_PORTF   6
#define NUTGPIO_PORTG   7
#define NUTGPIO_PORTH   8
#define NUTGPIO_PORTI   9
#define NUTGPIO_PORTJ   10
#define NUTGPIO_PORTK   11
#define NUTGPIO_PORTL   12

#define NUTGPIO_EXTINT0     1
#define NUTGPIO_EXTINT1     2
#define NUTGPIO_EXTINT2     3
#define NUTGPIO_EXTINT3     4
#define NUTGPIO_EXTINT4     5
#define NUTGPIO_EXTINT5     6
#define NUTGPIO_EXTINT6     7
#define NUTGPIO_EXTINT7     8
#define NUTGPIO_EXTFIQ0    -1

/*!
 * \brief GPIO disabled.
 *
 * Will activate the pins alternate function if set. This may not work
 * on all platforms.
 */
#define GPIO_CFG_DISABLED   0x00000001

/*!
 * \brief GPIO output direction enabled.
 *
 * If set, the pin is configured as an output. Otherwise it is in
 * input mode or z-state.
 */
#define GPIO_CFG_OUTPUT     0x00000002

/*!
 * \brief GPIO pull-up enabled.
 */
#define GPIO_CFG_PULLUP     0x00000004

/*!
 * \brief GPIO open drain output feature enabled.
 *
 * If not set, the output will use push pull mode.
 */
#define GPIO_CFG_MULTIDRIVE 0x00000008

/*!
 * \brief GPIO input glitch filter enabled.
 */
#define GPIO_CFG_DEBOUNCE   0x00000010

/*!
 * \brief GPIO Peripheral 0 mux.
 * Switch pin function to peripheral 0.
 */
#define GPIO_CFG_PERIPHERAL0 0x00000020

/*!
 * \brief GPIO Peripheral 1 mux.
 * Switch pin function to peripheral 1.
 */
#define GPIO_CFG_PERIPHERAL1 0x00000040

/*!
 * \brief GPIO Peripheral 2 mux.
 * Switch pin function to peripheral 2.
 */
#define GPIO_CFG_PERIPHERAL2 0x00000080

/*!
 * \brief GPIO Peripheral 3 mux.
 * Switch pin function to peripheral 3.
 */
#define GPIO_CFG_PERIPHERAL3 0x00000100

typedef struct {
    void (*iov_handler) (void *);
    void *iov_arg;
} GPIO_VECTOR;

typedef struct {
    IRQ_HANDLER *ios_sig;
    void (*ios_handler) (void *);
    int (*ios_ctl) (int cmd, void *param, int bit);
    GPIO_VECTOR *ios_vector;
} GPIO_SIGNAL;

#if defined(PIO_ISR)
extern GPIO_SIGNAL sig_GPIO;
#endif
#if defined(PIOA_ISR)
extern GPIO_SIGNAL sig_GPIO1;
#endif
#if defined(PIOB_ISR)
extern GPIO_SIGNAL sig_GPIO2;
#endif
#if defined(PIOC_ISR)
extern GPIO_SIGNAL sig_GPIO3;
#endif

__BEGIN_DECLS
/* Prototypes */

extern uint32_t GpioPinConfigGet(int bank, int bit);
extern int GpioPinConfigSet(int bank, int bit, uint32_t flags);
extern int GpioPortConfigSet(int bank, unsigned int mask, uint32_t flags);

extern int GpioPinGet(int bank, int bit);
extern void GpioPinSet(int bank, int bit, int value);
extern void GpioPinSetLow(int bank, int bit);
extern void GpioPinSetHigh(int bank, int bit);

extern unsigned int GpioPortGet(int bank);
extern void GpioPortSet(int bank, unsigned int value);
extern void GpioPortSetLow(int bank, unsigned int mask);
extern void GpioPortSetHigh(int bank, unsigned int mask);

extern int GpioRegisterIrqHandler(GPIO_SIGNAL * sig, int bit, void (*handler) (void *), void *arg);
extern int GpioIrqEnable(GPIO_SIGNAL * sig, int bit);
extern int GpioIrqDisable(GPIO_SIGNAL * sig, int bit);

__END_DECLS
/* End of prototypes */
#endif
