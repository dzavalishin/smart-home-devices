/*!
 * Copyright (C) 2001-2010 by egnite Software GmbH
 *
 * All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
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
 * $Id: gpio.c,v 1.4 2009/01/18 16:45:28 haraldkipp Exp $
 */

#include <arch/avr32.h>

#include <stdlib.h>
#include <string.h>

#include <dev/gpio.h>

#include <avr32/io.h>


/*
 * Convert the external bank to the AVR32 representation.
 */
static int get_avr32_bank(int bank)
{
    int avr32_bank = 0;

    switch (bank) {
    default:
    case NUTGPIO_PORT:
    case NUTGPIO_PORTA:
        avr32_bank = 0;
        break;
    case NUTGPIO_PORTB:
        avr32_bank = 1;
        break;
    case NUTGPIO_PORTC:
        avr32_bank = 2;
        break;
    case NUTGPIO_PORTD:
        avr32_bank = 3;
        break;
    case NUTGPIO_PORTE:
        avr32_bank = 4;
        break;
    case NUTGPIO_PORTF:
        avr32_bank = 5;
        break;
    case NUTGPIO_PORTG:
        avr32_bank = 6;
        break;
    case NUTGPIO_PORTH:
        avr32_bank = 7;
        break;
    case NUTGPIO_PORTI:
        avr32_bank = 8;
        break;
    case NUTGPIO_PORTJ:
        avr32_bank = 9;
        break;
    case NUTGPIO_PORTK:
        avr32_bank = 10;
        break;
    case NUTGPIO_PORTL:
        avr32_bank = 11;
        break;
    }

    if (avr32_bank >= AVR32_GPIO_PORT_LENGTH) {
        avr32_bank = AVR32_GPIO_PORT_LENGTH - 1;
    }

    return (avr32_bank);
}


/*!
   \internal
   Switch GPIO Peripheral Mux to specified \a function where function is one
   of the GPIO_CFG_PERIPHERALx values.

   Note that GPIO_CFG_PERIPHERALx values are mutually exclusive.
*/
static int enable_module_pin(int avr32_bank, int mask, unsigned int function)
{
    volatile avr32_gpio_port_t *gpio_port = &AVR32_GPIO.port[avr32_bank];

    // Enable the correct function.
    switch (function) {
    case GPIO_CFG_PERIPHERAL0: // A function.
        gpio_port->pmr0c = mask;
        gpio_port->pmr1c = mask;
        break;

    case GPIO_CFG_PERIPHERAL1: // B function.
        gpio_port->pmr0s = mask;
        gpio_port->pmr1c = mask;
        break;

    case GPIO_CFG_PERIPHERAL2: // C function.
        gpio_port->pmr0c = mask;
        gpio_port->pmr1s = mask;
        break;

    default:
        return -1;
    }

    // Disable GPIO control.
    gpio_port->gperc = mask;

    return 0;
}

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
    int avr32_bank = get_avr32_bank(bank);
    volatile avr32_gpio_port_t *gpio_port = &AVR32_GPIO.port[avr32_bank];

    return (gpio_port->pvr >> (bit & 0x1F)) & 1;
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
    int avr32_bank = get_avr32_bank(bank);
    volatile avr32_gpio_port_t *gpio_port = &AVR32_GPIO.port[avr32_bank];

    gpio_port->ovrc = 1 << (bit & 0x1F);        // Value to be driven on the I/O line: 0.
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
    int avr32_bank = get_avr32_bank(bank);
    volatile avr32_gpio_port_t *gpio_port = &AVR32_GPIO.port[avr32_bank];

    gpio_port->ovrs = 1 << (bit & 0x1F);        // Value to be driven on the I/O line: 1.
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
    int avr32_bank = get_avr32_bank(bank);
    volatile avr32_gpio_port_t *gpio_port = &AVR32_GPIO.port[avr32_bank];

    if (value) {
        gpio_port->ovrs = 1 << (bit & 0x1F);    // Value to be driven on the I/O line: 1.
    } else {
        gpio_port->ovrc = 1 << (bit & 0x1F);    // Value to be driven on the I/O line: 0.
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
    int avr32_bank = get_avr32_bank(bank);
    volatile avr32_gpio_port_t *gpio_port = &AVR32_GPIO.port[avr32_bank];

    return gpio_port->pvr;
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
    int avr32_bank = get_avr32_bank(bank);
    volatile avr32_gpio_port_t *gpio_port = &AVR32_GPIO.port[avr32_bank];

    gpio_port->ovrc = mask;     // Value to be driven on the I/O line: 0.
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
    int avr32_bank = get_avr32_bank(bank);
    volatile avr32_gpio_port_t *gpio_port = &AVR32_GPIO.port[avr32_bank];

    gpio_port->ovrs = mask;     // Value to be driven on the I/O line: 1.
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
    int avr32_bank = get_avr32_bank(bank);
    volatile avr32_gpio_port_t *gpio_port = &AVR32_GPIO.port[avr32_bank];

    gpio_port->ovr = value;
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
    uint8_t is_gpio = 0;
    int avr32_bank = get_avr32_bank(bank);
    volatile avr32_gpio_port_t *gpio_port = &AVR32_GPIO.port[avr32_bank];

    if ((gpio_port->gper & _BV(bit)) == 0) {
        rc |= GPIO_CFG_DISABLED;
    } else {
        is_gpio = 1;
    }

    if (gpio_port->oder & _BV(bit)) {
        rc |= GPIO_CFG_OUTPUT;
    }

    if (gpio_port->gfer & _BV(bit)) {
        rc |= GPIO_CFG_DEBOUNCE;
    }

    if (gpio_port->puer & _BV(bit)) {
        rc |= GPIO_CFG_PULLUP;
    }

    if (is_gpio == 0) {
        if (gpio_port->pmr1 & _BV(bit)) {
            if (gpio_port->pmr0 & _BV(bit)) {
                rc |= GPIO_CFG_PERIPHERAL3;
            } else {
                rc |= GPIO_CFG_PERIPHERAL2;
            }
        } else {
            if (gpio_port->pmr0 & _BV(bit)) {
                rc |= GPIO_CFG_PERIPHERAL1;
            } else {
                rc |= GPIO_CFG_PERIPHERAL0;
            }
        }
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
#define PERIPHERALS_MASK (GPIO_CFG_PERIPHERAL0|GPIO_CFG_PERIPHERAL1|GPIO_CFG_PERIPHERAL2|GPIO_CFG_PERIPHERAL3)

    int avr32_bank = get_avr32_bank(bank);
    volatile avr32_gpio_port_t *gpio_port = &AVR32_GPIO.port[avr32_bank];

    if (flags & GPIO_CFG_PULLUP) {
        gpio_port->puers = mask;
    } else {
        gpio_port->puerc = mask;
    }

    if (flags & GPIO_CFG_DEBOUNCE) {
        gpio_port->gfers = mask;
    } else {
        gpio_port->gferc = mask;
    }

    if (flags & GPIO_CFG_OUTPUT) {
        gpio_port->oders = mask;
    } else {
        gpio_port->oderc = mask;
    }

    if (flags & PERIPHERALS_MASK) {
        enable_module_pin(avr32_bank, mask, flags & PERIPHERALS_MASK);
    }

    if (flags & (GPIO_CFG_DISABLED | PERIPHERALS_MASK)) {
        gpio_port->gperc = mask;
        gpio_port->oderc = mask;
    } else {
        gpio_port->gpers = mask;
    }

#undef PERIPHERALS_MASK
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
 *     GpioIrqEnable(&sig_GPIO, 30);
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
        } else {
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
