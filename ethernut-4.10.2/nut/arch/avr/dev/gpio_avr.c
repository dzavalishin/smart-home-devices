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
 * $Id: gpio_avr.c 2700 2009-09-17 10:48:13Z haraldkipp $
 */

#include <arch/avr.h>

#include <stdlib.h>
#include <string.h>

#include <dev/gpio.h>

int GpioPinGet(int bank, int bit)
{
    switch(bank) {
#ifdef PINA
    case AVRPORTA:
        return bit_is_set(PINA, bit) != 0;
#endif
#ifdef PINB
    case AVRPORTB:
        return bit_is_set(PINB, bit) != 0;
#endif
#ifdef PINC
    case AVRPORTC:
        return bit_is_set(PINC, bit) != 0;
#endif
#ifdef PIND
    case AVRPORTD:
        return bit_is_set(PIND, bit) != 0;
#endif
#ifdef PINE
    case AVRPORTE:
        return bit_is_set(PINE, bit) != 0;
#endif
#ifdef PINF
    case AVRPORTF:
        return bit_is_set(PINF, bit) != 0;
#endif
#ifdef PING
    case AVRPORTG:
        return bit_is_set(PING, bit) != 0;
#endif
#ifdef PINH
    case AVRPORTH:
        return bit_is_set(PINH, bit) != 0;
#endif
#ifdef PINI
    case AVRPORTI:
        return bit_is_set(PINI, bit) != 0;
#endif
#ifdef PINJ
    case AVRPORTJ:
        return bit_is_set(PINJ, bit) != 0;
#endif
#ifdef PINK
    case AVRPORTK:
        return bit_is_set(PINK, bit) != 0;
#endif
#ifdef PINL
    case AVRPORTL:
        return bit_is_set(PINL, bit) != 0;
#endif
    }
    return 0;
}

void GpioPinSetLow(int bank, int bit)
{
    switch(bank) {
#ifdef PORTA
    case AVRPORTA:
        cbi(PORTA, bit);
        break;
#endif
#ifdef PORTB
    case AVRPORTB:
        cbi(PORTB, bit);
        break;
#endif
#ifdef PORTC
    case AVRPORTC:
        cbi(PORTC, bit);
        break;
#endif
#ifdef PORTD
    case AVRPORTD:
        cbi(PORTD, bit);
        break;
#endif
#ifdef PORTE
    case AVRPORTE:
        cbi(PORTE, bit);
        break;
#endif
#ifdef PORTF
    case AVRPORTF:
        cbi(PORTF, bit);
        break;
#endif
#ifdef PORTG
    case AVRPORTG:
        cbi(PORTG, bit);
        break;
#endif
#ifdef PORTH
    case AVRPORTH:
        cbi(PORTH, bit);
        break;
#endif
#ifdef PORTI
    case AVRPORTI:
        cbi(PORTI, bit);
        break;
#endif
#ifdef PORTJ
    case AVRPORTJ:
        cbi(PORTJ, bit);
        break;
#endif
#ifdef PORTK
    case AVRPORTK:
        cbi(PORTK, bit);
        break;
#endif
#ifdef PORTL
    case AVRPORTL:
        cbi(PORTL, bit);
        break;
#endif
    }
}

void GpioPinSetHigh(int bank, int bit)
{
    switch(bank) {
#ifdef PORTA
    case AVRPORTA:
        sbi(PORTA, bit);
        break;
#endif
#ifdef PORTB
    case AVRPORTB:
        sbi(PORTB, bit);
        break;
#endif
#ifdef PORTC
    case AVRPORTC:
        sbi(PORTC, bit);
        break;
#endif
#ifdef PORTD
    case AVRPORTD:
        sbi(PORTD, bit);
        break;
#endif
#ifdef PORTE
    case AVRPORTE:
        sbi(PORTE, bit);
        break;
#endif
#ifdef PORTF
    case AVRPORTF:
        sbi(PORTF, bit);
        break;
#endif
#ifdef PORTG
    case AVRPORTG:
        sbi(PORTG, bit);
        break;
#endif
#ifdef PORTH
    case AVRPORTH:
        sbi(PORTH, bit);
        break;
#endif
#ifdef PORTI
    case AVRPORTI:
        sbi(PORTI, bit);
        break;
#endif
#ifdef PORTJ
    case AVRPORTJ:
        sbi(PORTJ, bit);
        break;
#endif
#ifdef PORTK
    case AVRPORTK:
        sbi(PORTK, bit);
        break;
#endif
#ifdef PORTL
    case AVRPORTL:
        sbi(PORTL, bit);
        break;
#endif
    }
}

void GpioPinSet(int bank, int bit, int value)
{
    if (value) {
        GpioPinSetHigh(bank, bit);
    }
    else {
        GpioPinSetLow(bank, bit);
    }
}

unsigned int GpioPortGet(int bank)
{
    switch(bank) {
#ifdef PINA
    case AVRPORTA:
        return inb(PINA);
#endif
#ifdef PINB
    case AVRPORTB:
        return inb(PINB);
#endif
#ifdef PINC
    case AVRPORTC:
        return inb(PINC);
#endif
#ifdef PIND
    case AVRPORTD:
        return inb(PIND);
#endif
#ifdef PINE
    case AVRPORTE:
        return inb(PINE);
#endif
#ifdef PINF
    case AVRPORTF:
        return inb(PINF);
#endif
#ifdef PING
    case AVRPORTG:
        return inb(PING);
#endif
#ifdef PINH
    case AVRPORTH:
        return inb(PINH);
#endif
#ifdef PINI
    case AVRPORTI:
        return inb(PINI);
#endif
#ifdef PINJ
    case AVRPORTJ:
        return inb(PINJ);
#endif
#ifdef PINK
    case AVRPORTK:
        return inb(PINK);
#endif
#ifdef PINL
    case AVRPORTL:
        return inb(PINL);
#endif
    }
    return 0;
}

void GpioPortSet(int bank, unsigned int value)
{
    switch(bank) {
#ifdef PORTA
    case AVRPORTA:
        outb(PORTA, (uint8_t)value);
        break;
#endif
#ifdef PORTB
    case AVRPORTB:
        outb(PORTB, (uint8_t)value);
        break;
#endif
#ifdef PORTC
    case AVRPORTC:
        outb(PORTC, (uint8_t)value);
        break;
#endif
#ifdef PORTD
    case AVRPORTD:
        outb(PORTD, (uint8_t)value);
        break;
#endif
#ifdef PORTE
    case AVRPORTE:
        outb(PORTE, (uint8_t)value);
        break;
#endif
#ifdef PORTF
    case AVRPORTF:
        outb(PORTF, (uint8_t)value);
        break;
#endif
#ifdef PORTG
    case AVRPORTG:
        outb(PORTG, (uint8_t)value);
        break;
#endif
#ifdef PORTH
    case AVRPORTH:
        outb(PORTH, (uint8_t)value);
        break;
#endif
#ifdef PORTI
    case AVRPORTI:
        outb(PORTI, (uint8_t)value);
        break;
#endif
#ifdef PORTJ
    case AVRPORTJ:
        outb(PORTJ, (uint8_t)value);
        break;
#endif
#ifdef PORTK
    case AVRPORTK:
        outb(PORTK, (uint8_t)value);
        break;
#endif
#ifdef PORTL
    case AVRPORTL:
        outb(PORTL, (uint8_t)value);
        break;
#endif
    }
}

void GpioPortSetLow(int bank, unsigned int mask)
{
    GpioPortSet(bank, GpioPortGet(bank) & ~mask);
}

void GpioPortSetHigh(int bank, unsigned int mask)
{
    GpioPortSet(bank, GpioPortGet(bank) | mask);
}

uint32_t GpioPinConfigGet(int bank, int bit)
{
    uint32_t rc = 0;

    switch(bank) {
#ifdef DDRA
    case AVRPORTA:
        if (inb(DDRA) & _BV(bit)) {
            rc |= GPIO_CFG_OUTPUT;
        }
        else if (inb(PORTA) & _BV(bit)) {
            rc |= GPIO_CFG_PULLUP;
        }
        break;
#endif
#ifdef DDRB
    case AVRPORTB:
        if (inb(DDRB) & _BV(bit)) {
            rc |= GPIO_CFG_OUTPUT;
        }
        else if (inb(PORTB) & _BV(bit)) {
            rc |= GPIO_CFG_PULLUP;
        }
        break;
#endif
#ifdef DDRC
    case AVRPORTC:
        if (inb(DDRC) & _BV(bit)) {
            rc |= GPIO_CFG_OUTPUT;
        }
        else if (inb(PORTC) & _BV(bit)) {
            rc |= GPIO_CFG_PULLUP;
        }
        break;
#endif
#ifdef DDRD
    case AVRPORTD:
        if (inb(DDRD) & _BV(bit)) {
            rc |= GPIO_CFG_OUTPUT;
        }
        else if (inb(PORTD) & _BV(bit)) {
            rc |= GPIO_CFG_PULLUP;
        }
        break;
#endif
#ifdef DDRE
    case AVRPORTE:
        if (inb(DDRE) & _BV(bit)) {
            rc |= GPIO_CFG_OUTPUT;
        }
        else if (inb(PORTE) & _BV(bit)) {
            rc |= GPIO_CFG_PULLUP;
        }
        break;
#endif
#ifdef DDRF
    case AVRPORTF:
        if (inb(DDRF) & _BV(bit)) {
            rc |= GPIO_CFG_OUTPUT;
        }
        else if (inb(PORTF) & _BV(bit)) {
            rc |= GPIO_CFG_PULLUP;
        }
        break;
#endif
#ifdef DDRG
    case AVRPORTG:
        if (inb(DDRG) & _BV(bit)) {
            rc |= GPIO_CFG_OUTPUT;
        }
        else if (inb(PORTG) & _BV(bit)) {
            rc |= GPIO_CFG_PULLUP;
        }
        break;
#endif
#ifdef DDRH
    case AVRPORTH:
        if (inb(DDRH) & _BV(bit)) {
            rc |= GPIO_CFG_OUTPUT;
        }
        else if (inb(PORTH) & _BV(bit)) {
            rc |= GPIO_CFG_PULLUP;
        }
        break;
#endif
#ifdef DDRI
    case AVRPORTI:
        if (inb(DDRI) & _BV(bit)) {
            rc |= GPIO_CFG_OUTPUT;
        }
        else if (inb(PORTI) & _BV(bit)) {
            rc |= GPIO_CFG_PULLUP;
        }
        break;
#endif
#ifdef DDRJ
    case AVRPORTJ:
        if (inb(DDRJ) & _BV(bit)) {
            rc |= GPIO_CFG_OUTPUT;
        }
        else if (inb(PORTJ) & _BV(bit)) {
            rc |= GPIO_CFG_PULLUP;
        }
        break;
#endif
#ifdef DDRK
    case AVRPORTK:
        if (inb(DDRK) & _BV(bit)) {
            rc |= GPIO_CFG_OUTPUT;
        }
        else if (inb(PORTK) & _BV(bit)) {
            rc |= GPIO_CFG_PULLUP;
        }
        break;
#endif
#ifdef DDRL
    case AVRPORTL:
        if (inb(DDRL) & _BV(bit)) {
            rc |= GPIO_CFG_OUTPUT;
        }
        else if (inb(PORTL) & _BV(bit)) {
            rc |= GPIO_CFG_PULLUP;
        }
        break;
#endif
    }
    return rc;
}

int GpioPortConfigSet(int bank, unsigned int mask, uint32_t flags)
{
    switch(bank) {
#ifdef DDRA
    case AVRPORTA:
        if (flags & GPIO_CFG_PULLUP) {
            outb(PORTA, inb(PORTA) | mask);
        }
        else {
            outb(PORTA, inb(PORTA) & ~mask);
        }
        if (flags & GPIO_CFG_OUTPUT) {
            outb(DDRA, inb(DDRA) | mask);
        }
        else {
            outb(DDRA, inb(DDRA) & ~mask);
        }
        break;
#endif
#ifdef DDRB
    case AVRPORTB:
        if (flags & GPIO_CFG_PULLUP) {
            outb(PORTB, inb(PORTB) | mask);
        }
        else {
            outb(PORTB, inb(PORTB) & ~mask);
        }
        if (flags & GPIO_CFG_OUTPUT) {
            outb(DDRB, inb(DDRB) | mask);
        }
        else {
            outb(DDRB, inb(DDRB) & ~mask);
        }
        break;
#endif
#ifdef DDRC
    case AVRPORTC:
        if (flags & GPIO_CFG_PULLUP) {
            outb(PORTC, inb(PORTC) | mask);
        }
        else {
            outb(PORTC, inb(PORTC) & ~mask);
        }
        if (flags & GPIO_CFG_OUTPUT) {
            outb(DDRC, inb(DDRC) | mask);
        }
        else {
            outb(DDRC, inb(DDRC) & ~mask);
        }
        break;
#endif
#ifdef DDRD
    case AVRPORTD:
        if (flags & GPIO_CFG_PULLUP) {
            outb(PORTD, inb(PORTD) | mask);
        }
        else {
            outb(PORTD, inb(PORTD) & ~mask);
        }
        if (flags & GPIO_CFG_OUTPUT) {
            outb(DDRD, inb(DDRD) | mask);
        }
        else {
            outb(DDRD, inb(DDRD) & ~mask);
        }
        break;
#endif
#ifdef DDRE
    case AVRPORTE:
        if (flags & GPIO_CFG_PULLUP) {
            outb(PORTE, inb(PORTE) | mask);
        }
        else {
            outb(PORTE, inb(PORTE) & ~mask);
        }
        if (flags & GPIO_CFG_OUTPUT) {
            outb(DDRE, inb(DDRE) | mask);
        }
        else {
            outb(DDRE, inb(DDRE) & ~mask);
        }
        break;
#endif
#ifdef DDRF
    case AVRPORTF:
        if (flags & GPIO_CFG_PULLUP) {
            outb(PORTF, inb(PORTF) | mask);
        }
        else {
            outb(PORTF, inb(PORTF) & ~mask);
        }
        if (flags & GPIO_CFG_OUTPUT) {
            outb(DDRF, inb(DDRF) | mask);
        }
        else {
            outb(DDRF, inb(DDRF) & ~mask);
        }
        break;
#endif
#ifdef DDRG
    case AVRPORTG:
        if (flags & GPIO_CFG_PULLUP) {
            outb(PORTG, inb(PORTG) | mask);
        }
        else {
            outb(PORTG, inb(PORTG) & ~mask);
        }
        if (flags & GPIO_CFG_OUTPUT) {
            outb(DDRG, inb(DDRG) | mask);
        }
        else {
            outb(DDRG, inb(DDRG) & ~mask);
        }
        break;
#endif
#ifdef DDRH
    case AVRPORTH:
        if (flags & GPIO_CFG_PULLUP) {
            outb(PORTH, inb(PORTH) | mask);
        }
        else {
            outb(PORTH, inb(PORTH) & ~mask);
        }
        if (flags & GPIO_CFG_OUTPUT) {
            outb(DDRH, inb(DDRH) | mask);
        }
        else {
            outb(DDRH, inb(DDRH) & ~mask);
        }
        break;
#endif
#ifdef DDRI
    case AVRPORTI:
        if (flags & GPIO_CFG_PULLUP) {
            outb(PORTI, inb(PORTI) | mask);
        }
        else {
            outb(PORTI, inb(PORTI) & ~mask);
        }
        if (flags & GPIO_CFG_OUTPUT) {
            outb(DDRI, inb(DDRI) | mask);
        }
        else {
            outb(DDRI, inb(DDRI) & ~mask);
        }
        break;
#endif
#ifdef DDRJ
    case AVRPORTJ:
        if (flags & GPIO_CFG_PULLUP) {
            outb(PORTJ, inb(PORTJ) | mask);
        }
        else {
            outb(PORTJ, inb(PORTJ) & ~mask);
        }
        if (flags & GPIO_CFG_OUTPUT) {
            outb(DDRJ, inb(DDRJ) | mask);
        }
        else {
            outb(DDRJ, inb(DDRJ) & ~mask);
        }
        break;
#endif
#ifdef DDRK
    case AVRPORTK:
        if (flags & GPIO_CFG_PULLUP) {
            outb(PORTK, inb(PORTK) | mask);
        }
        else {
            outb(PORTK, inb(PORTK) & ~mask);
        }
        if (flags & GPIO_CFG_OUTPUT) {
            outb(DDRK, inb(DDRK) | mask);
        }
        else {
            outb(DDRK, inb(DDRK) & ~mask);
        }
        break;
#endif
#ifdef DDRL
    case AVRPORTL:
        if (flags & GPIO_CFG_PULLUP) {
            outb(PORTL, inb(PORTL) | mask);
        }
        else {
            outb(PORTL, inb(PORTL) & ~mask);
        }
        if (flags & GPIO_CFG_OUTPUT) {
            outb(DDRL, inb(DDRL) | mask);
        }
        else {
            outb(DDRL, inb(DDRL) & ~mask);
        }
        break;
#endif
    }
    return 0;
}

int GpioPinConfigSet(int bank, int bit, uint32_t flags)
{
    switch(bank) {
#ifdef DDRA
    case AVRPORTA:
        if (flags & GPIO_CFG_OUTPUT) {
            sbi(DDRA, bit);
        }
        else {
            if (flags & GPIO_CFG_PULLUP) {
                sbi(PORTA, bit);
            }
            else {
                cbi(PORTA, bit);
            }
            cbi(DDRA, bit);
        }
        break;
#endif
#ifdef DDRB
    case AVRPORTB:
        if (flags & GPIO_CFG_OUTPUT) {
            sbi(DDRB, bit);
        }
        else {
            if (flags & GPIO_CFG_PULLUP) {
                sbi(PORTB, bit);
            }
            else {
                cbi(PORTB, bit);
            }
            cbi(DDRB, bit);
        }
        break;
#endif
#ifdef DDRC
    case AVRPORTC:
        if (flags & GPIO_CFG_OUTPUT) {
            sbi(DDRC, bit);
        }
        else {
            if (flags & GPIO_CFG_PULLUP) {
                sbi(PORTC, bit);
            }
            else {
                cbi(PORTC, bit);
            }
            cbi(DDRC, bit);
        }
        break;
#endif
#ifdef DDRD
    case AVRPORTD:
        if (flags & GPIO_CFG_OUTPUT) {
            sbi(DDRD, bit);
        }
        else {
            if (flags & GPIO_CFG_PULLUP) {
                sbi(PORTD, bit);
            }
            else {
                cbi(PORTD, bit);
            }
            cbi(DDRD, bit);
        }
        break;
#endif
#ifdef DDRE
    case AVRPORTE:
        if (flags & GPIO_CFG_OUTPUT) {
            sbi(DDRE, bit);
        }
        else {
            if (flags & GPIO_CFG_PULLUP) {
                sbi(PORTE, bit);
            }
            else {
                cbi(PORTE, bit);
            }
            cbi(DDRE, bit);
        }
        break;
#endif
#ifdef DDRF
    case AVRPORTF:
        if (flags & GPIO_CFG_OUTPUT) {
            sbi(DDRF, bit);
        }
        else {
            if (flags & GPIO_CFG_PULLUP) {
                sbi(PORTF, bit);
            }
            else {
                cbi(PORTF, bit);
            }
            cbi(DDRF, bit);
        }
        break;
#endif
    default:
        /* Use port wide function for registers above I/O space. */
        GpioPortConfigSet(bank, _BV(bit), flags);
        break;
    }

    /* Check the result. */
    if (GpioPinConfigGet(bank, bit) != flags) {
        return -1;
    }
    return 0;
}

int GpioRegisterIrqHandler(GPIO_SIGNAL * sig, int bit, void (*handler) (void *), void *arg)
{
    return -1;
}

int GpioIrqEnable(GPIO_SIGNAL * sig, int bit)
{
    return -1;
}

int GpioIrqDisable(GPIO_SIGNAL * sig, int bit)
{
    return -1;
}
