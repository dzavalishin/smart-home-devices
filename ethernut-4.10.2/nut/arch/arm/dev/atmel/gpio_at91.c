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
 * $Id: gpio_at91.c 3449 2011-05-31 19:08:15Z mifi $
 */

#include <arch/arm.h>

#include <stdlib.h>
#include <string.h>

#include <dev/gpio.h>

/*!
 * \brief Get pin level.
 *
 * \param bank GPIO bank/port number.
 * \param bit  Bit number of the specified bank/port.
 *
 * \return 1 if the pin level is high. 0 is returned if the pin level 
 *         is low or if the pin is undefined.
 */
int GpioPinGet(int bank, int bit)
{
    int rc = 0;

    switch(bank) {
#ifdef PIO_PDSR
    case NUTGPIO_PORT:
        rc = (inr(PIO_PDSR) & _BV(bit)) != 0;
        break;
#endif /* PIO_PDSR */

#ifdef PIOA_PDSR
    case NUTGPIO_PORTA:
        rc = (inr(PIOA_PDSR) & _BV(bit)) != 0;
        break;
#endif /* PIOA_PDSR */

#ifdef PIOB_PDSR
    case NUTGPIO_PORTB:
        rc = (inr(PIOB_PDSR) & _BV(bit)) != 0;
        break;
#endif /* PIOB_PDSR */

#ifdef PIOC_PDSR
    case NUTGPIO_PORTC:
        rc = (inr(PIOC_PDSR) & _BV(bit)) != 0;
        break;
#endif /* PIOC_PDSR */
    }
    return rc;
}

/*!
 * \brief Set pin level to low.
 *
 * Trying to set undefined pins is silently ignored.
 *
 * \param bank GPIO bank/port number.
 * \param bit  Bit number of the specified bank/port.
 */
void GpioPinSetLow(int bank, int bit)
{
    switch(bank) {
#ifdef PIO_CODR
    case NUTGPIO_PORT:
        outr(PIO_CODR, _BV(bit));
        break;
#endif /* PIO_CODR */

#ifdef PIOA_CODR
    case NUTGPIO_PORTA:
        outr(PIOA_CODR, _BV(bit));
        break;
#endif /* PIOA_CODR */

#ifdef PIOB_CODR
    case NUTGPIO_PORTB:
        outr(PIOB_CODR, _BV(bit));
        break;
#endif /* PIOB_CODR */

#ifdef PIOC_CODR
    case NUTGPIO_PORTC:
        outr(PIOC_CODR, _BV(bit));
        break;
#endif /* PIOC_CODR */
    }
}

/*!
 * \brief Set pin level to high.
 *
 * Trying to set undefined pins is silently ignored.
 *
 * \param bank GPIO bank/port number.
 * \param bit  Bit number of the specified bank/port.
 */
void GpioPinSetHigh(int bank, int bit)
{
    switch(bank) {
#ifdef PIO_SODR
    case NUTGPIO_PORT:
        outr(PIO_SODR, _BV(bit));
        break;
#endif /* PIO_SODR */

#ifdef PIOA_SODR
    case NUTGPIO_PORTA:
        outr(PIOA_SODR, _BV(bit));
        break;
#endif /* PIOA_SODR */

#ifdef PIOB_SODR
    case NUTGPIO_PORTB:
        outr(PIOB_SODR, _BV(bit));
        break;
#endif /* PIOB_SODR */

#ifdef PIOC_SODR
    case NUTGPIO_PORTC:
        outr(PIOC_SODR, _BV(bit));
        break;
#endif /* PIOC_SODR */
    }
}

/*!
 * \brief Set pin level.
 *
 * Trying to set undefined pins is silently ignored.
 *
 * \param bank  GPIO bank/port number.
 * \param bit   Bit number of the specified bank/port.
 * \param value Level, 0 for low or any other value for high.
 */
void GpioPinSet(int bank, int bit, int value)
{
    if (value) {
        GpioPinSetHigh(bank, bit);
    }
    else {
        GpioPinSetLow(bank, bit);
    }
}

/*!
 * \brief Get all pin levels of a specified bank/port.
 *
 * \param bank GPIO bank/port number.
 *
 * \return Pin levels. 0 is returned for unknown banks and pins.
 */
unsigned int GpioPortGet(int bank)
{
    unsigned int rc = 0;

    switch(bank) {
#ifdef PIO_PDSR
    case NUTGPIO_PORT:
        rc = inr(PIO_PDSR);
        break;
#endif /* PIO_PDSR */

#ifdef PIOA_PDSR
    case NUTGPIO_PORTA:
        rc = inr(PIOA_PDSR);
        break;
#endif /* PIO_PDSR */

#ifdef PIOB_PDSR
    case NUTGPIO_PORTB:
        rc = inr(PIOB_PDSR);
        break;
#endif /* PIOB_PDSR */

#ifdef PIOC_PDSR
    case NUTGPIO_PORTC:
        rc = inr(PIOC_PDSR);
        break;
#endif /* PIOC_PDSR */
    }
    return rc;
}

/*!
 * \brief Set multiple pin levels of a bank/port to low.
 *
 * \param bank GPIO bank/port number.
 * \param mask Pin levels are set to low, if the corresponding
 *             bit in this mask is 1.
 *
 * \return Levels.
 */
void GpioPortSetLow(int bank, unsigned int mask)
{
    switch(bank) {
#ifdef PIO_CODR
    case NUTGPIO_PORT:
        outr(PIO_CODR, mask);
        break;
#endif /* PIO_CODR */

#ifdef PIOA_CODR
    case NUTGPIO_PORTA:
        outr(PIOA_CODR, mask);
        break;
#endif /* PIOA_CODR */

#ifdef PIOB_CODR
    case NUTGPIO_PORTB:
        outr(PIOB_CODR, mask);
        break;
#endif /* PIOB_CODR */

#ifdef PIOC_CODR
    case NUTGPIO_PORTC:
        outr(PIOC_CODR, mask);
        break;
#endif /* PIOC_CODR */
    }
}

/*!
 * \brief Set multiple pin levels of a bank/port to high.
 *
 * Trying to set undefined ports is silently ignored.
 *
 * \param bank GPIO bank/port number.
 * \param mask Pin levels are set to high, if the corresponding
 *             bit in this mask is 1.
 */
void GpioPortSetHigh(int bank, unsigned int mask)
{
    switch(bank) {
#ifdef PIO_SODR
    case NUTGPIO_PORT:
        outr(PIO_SODR, mask);
        break;
#endif /* PIO_SODR */

#ifdef PIOA_SODR
    case NUTGPIO_PORTA:
        outr(PIOA_SODR, mask);
        break;
#endif /* PIOA_SODR */

#ifdef PIOB_SODR
    case NUTGPIO_PORTB:
        outr(PIOB_SODR, mask);
        break;
#endif /* PIOB_SODR */

#ifdef PIOC_SODR
    case NUTGPIO_PORTC:
        outr(PIOC_SODR, mask);
        break;
#endif /* PIOC_SODR */
    }
}

/*!
 * \brief Set all pin levels of a bank/port.
 *
 * This routine can be used to simultaneously set all pins of a specific
 * port. However, in some implementations the high bit values will be
 * set before the low bit values. If this is a problem, you should use
 * GpioPortSetHigh() and GpioPortSetLow().
 *
 * \param bank  GPIO bank/port number.
 * \param value Pin levels are set to high, if the corresponding
 *              bit in this mask is 1. All other pin levels are
 *              set to low.
 */
void GpioPortSet(int bank, unsigned int value)
{
    if (value) {
        GpioPortSetHigh(bank, value);
    }
    value = ~value;
    if (value) {
        GpioPortSetLow(bank, value);
    }
}

/*!
 * \brief Get pin configuration.
 *
 * \param bank GPIO bank/port number.
 * \param bit  Bit number of the specified bank/port.
 *
 * \return Attribute flags of the pin.
 */
uint32_t GpioPinConfigGet(int bank, int bit)
{
    uint32_t rc = 0;

    switch(bank) {
    case NUTGPIO_PORT:
#ifdef PIO_PSR
        if ((inr(PIO_PSR) & _BV(bit)) == 0) {
            rc |= GPIO_CFG_DISABLED;
        }
#endif /* PIO_PSR */

#ifdef PIO_OSR
        if (inr(PIO_OSR) & _BV(bit)) {
            rc |= GPIO_CFG_OUTPUT;
        }
#endif /* PIO_OSR */

#ifdef PIO_IFSR
        if (inr(PIO_IFSR) & _BV(bit)) {
            rc |= GPIO_CFG_DEBOUNCE;
        }
#endif /* PIO_IFSR */

#ifdef PIO_MDSR
        if (inr(PIO_MDSR) & _BV(bit)) {
            rc |= GPIO_CFG_MULTIDRIVE;
        }
#endif /* PIO_MDSR */

#ifdef PIO_PUSR
        if ((inr(PIO_PUSR) & _BV(bit)) == 0) {
            rc |= GPIO_CFG_PULLUP;
        }
#endif /* PIO_PUSR */
        break;

    case NUTGPIO_PORTA:
#ifdef PIOA_PSR
        if ((inr(PIOA_PSR) & _BV(bit)) == 0) {
            rc |= GPIO_CFG_DISABLED;
        }
#endif /* PIOA_PSR */

#ifdef PIOA_OSR
        if (inr(PIOA_OSR) & _BV(bit)) {
            rc |= GPIO_CFG_OUTPUT;
        }
#endif /* PIOA_OSR */

#ifdef PIOA_IFSR
        if (inr(PIOA_IFSR) & _BV(bit)) {
            rc |= GPIO_CFG_DEBOUNCE;
        }
#endif /* PIOA_IFSR */

#ifdef PIOA_MDSR
        if (inr(PIOA_MDSR) & _BV(bit)) {
            rc |= GPIO_CFG_MULTIDRIVE;
        }
#endif /* PIOA_MDSR */

#ifdef PIOA_PUSR
        if ((inr(PIOA_PUSR) & _BV(bit)) == 0) {
            rc |= GPIO_CFG_PULLUP;
        }
#endif /* PIOA_PUSR */
        break;

    case NUTGPIO_PORTB:
#ifdef PIOB_PSR
        if ((inr(PIOB_PSR) & _BV(bit)) == 0) {
            rc |= GPIO_CFG_DISABLED;
        }
#endif /* PIOB_PSR */

#ifdef PIOB_OSR
        if (inr(PIOB_OSR) & _BV(bit)) {
            rc |= GPIO_CFG_OUTPUT;
        }
#endif /* PIOB_OSR */

#ifdef PIOB_IFSR
        if (inr(PIOB_IFSR) & _BV(bit)) {
            rc |= GPIO_CFG_DEBOUNCE;
        }
#endif /* PIOB_IFSR */

#ifdef PIOB_MDSR
        if (inr(PIOB_MDSR) & _BV(bit)) {
            rc |= GPIO_CFG_MULTIDRIVE;
        }
#endif /* PIOB_MDSR */

#ifdef PIOB_PUSR
        if ((inr(PIOB_PUSR) & _BV(bit)) == 0) {
            rc |= GPIO_CFG_PULLUP;
        }
#endif /* PIOB_PUSR */
        break;

    case NUTGPIO_PORTC:
#ifdef PIOC_PSR
        if ((inr(PIOC_PSR) & _BV(bit)) == 0) {
            rc |= GPIO_CFG_DISABLED;
        }
#endif /* PIOC_PSR */

#ifdef PIOC_OSR
        if (inr(PIOC_OSR) & _BV(bit)) {
            rc |= GPIO_CFG_OUTPUT;
        }
#endif /* PIOC_OSR */

#ifdef PIOC_IFSR
        if (inr(PIOC_IFSR) & _BV(bit)) {
            rc |= GPIO_CFG_DEBOUNCE;
        }
#endif /* PIOC_IFSR */

#ifdef PIOC_MDSR
        if (inr(PIOC_MDSR) & _BV(bit)) {
            rc |= GPIO_CFG_MULTIDRIVE;
        }
#endif /* PIOC_MDSR */

#ifdef PIOC_PUSR
        if ((inr(PIOC_PUSR) & _BV(bit)) == 0) {
            rc |= GPIO_CFG_PULLUP;
        }
#endif /* PIOC_PUSR */
        break;

    }
    return rc;
}

/*!
 * \brief Set port wide pin configuration.
 *
 * \note This function does not check for undefined ports and pins or
 *       invalid attributes. If this is required, use GpioPinConfigSet().
 *
 * \param bank  GPIO bank/port number.
 * \param mask  The given attributes are set for a specific pin, if the 
 *              corresponding bit in this mask is 1.
 * \param flags Attribute flags to set.
 *
 * \return Always 0.
 */
int GpioPortConfigSet(int bank, unsigned int mask, uint32_t flags)
{
    switch(bank) {
    case NUTGPIO_PORT:
#ifdef PIO_PDR
        if (flags & GPIO_CFG_DISABLED) {
            outr(PIO_PDR, mask);
        }
        else {
            outr(PIO_PER, mask);
#ifdef PMC_PCER
            outr(PMC_PCER, _BV(PIO_ID));
#endif /* PMC_PCER */
        }
#endif /* PIO_PDR */

#ifdef PIO_PUER
        if (flags & GPIO_CFG_PULLUP) {
            outr(PIO_PUER, mask);
        }
        else {
            outr(PIO_PUDR, mask);
        }
#endif /* PIO_PUER */

#ifdef PIO_IFER
        if (flags & GPIO_CFG_DEBOUNCE) {
            outr(PIO_IFER, mask);
        }
        else {
            outr(PIO_IFDR, mask);
        }
#endif /* PIO_IFER */

#ifdef PIO_ODR
        if ((flags & GPIO_CFG_OUTPUT) == 0) {
            outr(PIO_ODR, mask);
        }
#endif /* PIO_ODR */

#ifdef PIO_MDER
        if (flags & GPIO_CFG_MULTIDRIVE) {
            outr(PIO_MDER, mask);
        }
        else {
            outr(PIO_MDDR, mask);
        }
#endif /* PIO_MDER */

#ifdef PIO_OER
        if (flags & GPIO_CFG_OUTPUT) {
            outr(PIO_OER, mask);
        }
#endif /* PIO_OER */
        break;

    case NUTGPIO_PORTA:
#ifdef PIOA_PDR
        if (flags & GPIO_CFG_DISABLED) {
            outr(PIOA_PDR, mask);
        }
        else {
            outr(PIOA_PER, mask);
#ifdef PMC_PCER
            outr(PMC_PCER, _BV(PIOA_ID));
#endif /* PMC_PCER */
        }
#endif /* PIOA_PDR */

#ifdef PIOA_PUER
        if (flags & GPIO_CFG_PULLUP) {
            outr(PIOA_PUER, mask);
        }
        else {
            outr(PIOA_PUDR, mask);
        }
#endif /* PIOA_PUER */

#ifdef PIOA_IFER
        if (flags & GPIO_CFG_DEBOUNCE) {
            outr(PIOA_IFER, mask);
        }
        else {
            outr(PIOA_IFDR, mask);
        }
#endif /* PIOA_IFER */

#ifdef PIOA_ODR
        if ((flags & GPIO_CFG_OUTPUT) == 0) {
            outr(PIOA_ODR, mask);
        }
#endif /* PIOA_ODR */

#ifdef PIOA_MDER
        if (flags & GPIO_CFG_MULTIDRIVE) {
            outr(PIOA_MDER, mask);
        }
        else {
            outr(PIOA_MDDR, mask);
        }
#endif /* PIOA_MDER */

#ifdef PIOA_OER
        if (flags & GPIO_CFG_OUTPUT) {
            outr(PIOA_OER, mask);
        }
#endif /* PIOA_OER */
        break;

    case NUTGPIO_PORTB:
#ifdef PIOB_PDR
        if (flags & GPIO_CFG_DISABLED) {
            outr(PIOB_PDR, mask);
        }
        else {
            outr(PIOB_PER, mask);
#ifdef PMC_PCER
            outr(PMC_PCER, _BV(PIOB_ID));
#endif /* PMC_PCER */
        }
#endif /* PIOB_PDR */

#ifdef PIOB_PUER
        if (flags & GPIO_CFG_PULLUP) {
            outr(PIOB_PUER, mask);
        }
        else {
            outr(PIOB_PUDR, mask);
        }
#endif /* PIOB_PUER */

#ifdef PIOB_IFER
        if (flags & GPIO_CFG_DEBOUNCE) {
            outr(PIOB_IFER, mask);
        }
        else {
            outr(PIOB_IFDR, mask);
        }
#endif /* PIOB_IFER */

#ifdef PIOB_ODR
        if ((flags & GPIO_CFG_OUTPUT) == 0) {
            outr(PIOB_ODR, mask);
        }
#endif /* PIOB_ODR */

#ifdef PIOB_MDER
        if (flags & GPIO_CFG_MULTIDRIVE) {
            outr(PIOB_MDER, mask);
        }
        else {
            outr(PIOB_MDDR, mask);
        }
#endif /* PIOB_MDER */

#ifdef PIOB_OER
        if (flags & GPIO_CFG_OUTPUT) {
            outr(PIOB_OER, mask);
        }
#endif /* PIOB_OER */
        break;

    case NUTGPIO_PORTC:
#ifdef PIOC_PDR
        if (flags & GPIO_CFG_DISABLED) {
            outr(PIOC_PDR, mask);
        }
        else {
            outr(PIOC_PER, mask);
#ifdef PMC_PCER
            outr(PMC_PCER, _BV(PIOC_ID));
#endif /* PMC_PCER */
        }
#endif /* PIOC_PDR */

#ifdef PIOC_PUER
        if (flags & GPIO_CFG_PULLUP) {
            outr(PIOC_PUER, mask);
        }
        else {
            outr(PIOC_PUDR, mask);
        }
#endif /* PIOC_PUER */

#ifdef PIOC_IFER
        if (flags & GPIO_CFG_DEBOUNCE) {
            outr(PIOC_IFER, mask);
        }
        else {
            outr(PIOC_IFDR, mask);
        }
#endif /* PIOC_IFER */

#ifdef PIOC_ODR
        if ((flags & GPIO_CFG_OUTPUT) == 0) {
            outr(PIOC_ODR, mask);
        }
#endif /* PIOC_ODR */

#ifdef PIOC_MDER
        if (flags & GPIO_CFG_MULTIDRIVE) {
            outr(PIOC_MDER, mask);
        }
        else {
            outr(PIOC_MDDR, mask);
        }
#endif /* PIOC_MDER */

#ifdef PIOC_OER
        if (flags & GPIO_CFG_OUTPUT) {
            outr(PIOC_OER, mask);
        }
#endif /* PIOC_OER */
        break;
    }
    return 0;
}

/*!
 * \brief Set pin configuration.
 *
 * Applications may also use this function to make sure, that a specific
 * attribute is available for a specific pin.
 *
 * \note GPIO pins are typically initialized to a safe state after power
 *       up. This routine is not able to determine the consequences of
 *       changing pin configurations. In the worst case you may permanently
 *       damage your hardware by bad pin settings.
 *
 * \param bank  GPIO bank/port number.
 * \param bit   Bit number of the specified bank/port.
 * \param flags Attribute flags.
 *
 * \return 0 if all attributes had been set, -1 otherwise.
 */
int GpioPinConfigSet(int bank, int bit, uint32_t flags)
{
    GpioPortConfigSet(bank, _BV(bit), flags);

    /* Check the result. */
    if (GpioPinConfigGet(bank, bit) != flags) {
        return -1;
    }
    return 0;
}

/*!
 * \brief Register a GPIO pin interrupt handler.
 *
 * Generating interrupts on GPIO pin changes is not supported on all 
 * platforms. In this case dedicated external interrupt pins may
 * be used with NutRegisterIrqHandler().
 *
 * Interrupts are triggered on rising and falling edges. Level triggering
 * or triggering on specific edges is not supported.
 *
 * After registering, interrupts are disabled. Calling GpioIrqEnable()
 * is required to activate the interrupt.
 *
 * The following code fragment registers an interrupt handler which is 
 * called on each change of bit 4 of the first GPIO port:
 * \code
 * #include <dev/gpio.h>
 *
 * static void PinChange(void *arg)
 * {
 *     ...
 * }
 *
 * {
 *     ...
 *     GpioPinConfigSet(0, 4, GPIO_CFG_PULLUP);
 *     GpioRegisterIrqHandler(&sig_GPIO, 4, PinChange, NULL);
 *     GpioIrqEnable(&sig_GPIO, 4);
 *     ...
 * }
 * \endcode
 *
 * \param sig     Bank/port interrupt to be associated with this handler.
 * \param bit     Bit number of the specified bank/port.
 * \param handler This routine will be called by Nut/OS, when the specified 
 *                pin changes its state.
 * \param arg     Argument to be passed to the interrupt handler routine.
 *
 * \return 0 on success, -1 otherwise.
 */
int GpioRegisterIrqHandler(GPIO_SIGNAL * sig, int bit, void (*handler) (void *), void *arg)
{
    int rc = 0;

    if (sig->ios_vector == 0) {
        /* This is the first call. Allocate the vector table. */
        sig->ios_vector = malloc(sizeof(GPIO_VECTOR) * 32);
        if (sig->ios_vector) {
            memset(sig->ios_vector, 0, sizeof(GPIO_VECTOR) * 32);
            /* Register our internal PIO interrupt service. */
            rc = NutRegisterIrqHandler(sig->ios_sig, sig->ios_handler, sig->ios_vector);
            if (rc == 0) {
                rc = NutIrqEnable(sig->ios_sig);
            }
        }
        else {
            rc = -1;
        }
    }
    sig->ios_vector[bit].iov_handler = handler;
    sig->ios_vector[bit].iov_arg = arg;

    return rc;
}

/*!
 * \brief Enable a specified GPIO interrupt.
 *
 * A related interrupt handler must have been registered before calling
 * this function. See GpioRegisterIrqHandler().
 *
 * \param sig Interrupt to enable.
 * \param bit Bit number of the specified bank/port.
 *
 * \return 0 on success, -1 otherwise.
 */
int GpioIrqEnable(GPIO_SIGNAL * sig, int bit)
{
    return (sig->ios_ctl) (NUT_IRQCTL_ENABLE, NULL, bit);
}

/*!
 * \brief Disable a specified GPIO interrupt.
 *
 * \param sig Interrupt to disable.
 * \param bit Bit number of the specified bank/port.
 *
 * \return 0 on success, -1 otherwise.
 */
int GpioIrqDisable(GPIO_SIGNAL * sig, int bit)
{
    return (sig->ios_ctl) (NUT_IRQCTL_DISABLE, NULL, bit);
}
