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
 * Revision 1.2  2008/08/11 06:59:17  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.1  2007/04/12 09:07:54  haraldkipp
 * Configurable SPI added.
 *
 */

#include <cfg/arch/avr.h>
#include <sys/timer.h>
#include <dev/sppif0.h>

/* SPI control shadow registers. */
static uint8_t sppi0_spcr[SPPI0_MAX_DEVICES];

/* SPI status shadow registers. Used to configure the double speed bit. */
#ifdef SPI2X
static uint8_t sppi0_spsr[SPPI0_MAX_DEVICES];
#endif

/*!
 * \brief Set mode for a given device at SPI0.
 *
 * This is typically the first call in order to use the device. It will
 * not directly enable the SPI hardware. This is done when selecting
 * the device.
 *
 * The transfer rate is set to the lowest possible value and can be
 * adjusted by calling Sppi0SetSpeed().
 *
 * \param ix   The device index, starting at 0.
 * \param mode The mode to set.
 *             - 0: Leading edge is rising, data sampled on rising edge.
 *             - 1: Leading edge is rising, data sampled on falling edge.
 *             - 2: Leading edge is falling, data sampled on falling edge.
 *             - 3: Leading edge is falling, data sampled on rising edge.
 *
 * \return 0, if the device index and the mode are both valid. Otherwise
 *         the return value will be -1.
 */
int Sppi0SetMode(uint8_t ix, uint8_t mode)
{
    if (ix >= SPPI0_MAX_DEVICES || mode > 3) {
        return -1;
    }

    /* A bit obfuscated, but compact. A simple shift 
       correctly sets CPHA and CPOL. */
    sppi0_spcr[ix] = _BV(SPE) | _BV(MSTR) | (mode << 2) | _BV(SPR1) | _BV(SPR0);
#if defined(SPI2X)
    sppi0_spsr[ix] = 0;
#endif

    return 0;
}

/*!
 * \brief Set transfer rate for a given device at SPI0.
 *
 * \param ix   The device index, starting at 0.
 * \param rate Transfer rate in bits per second.
 * 
 */
void Sppi0SetSpeed(uint8_t ix, uint32_t rate)
{
    uint32_t fosc;
    uint8_t i;

    fosc = NutGetCpuClock();

    sppi0_spcr[ix] &= ~(_BV(SPR1) | _BV(SPR0));
    /* Find the frequency that is below or equal the requested 
       one, using the double speed bit if available. */
#if defined(SPI2X)
    for (i = 0; i < 7; i++) {
        fosc >>= 1;
        if (fosc <= rate) {
            break;
        }
    }
    sppi0_spcr[ix] |= (i >> 1);
    if (i < 6) {
        sppi0_spsr[ix] = ~i & 1;
    }
#else
    for (i = 0; i < 3; i++) {
        fosc >>= 2;
        if (fosc <= rate) {
            break;
        }
    }
    sppi0_spcr[ix] |= i;
#endif
}

/*!
 * \brief Enable the serial peripheral interface 0.
 *
 * Enables SPI with the parameters previously set by Sppi0SetMode() and 
 * Sppi0SetSpeed().
 *
 * \param ix The device index, starting at 0. The routine will not check 
 *           if this is valid.
 */
void Sppi0Enable(uint8_t ix)
{
    /*
     * When configured as SPI master, MOSI (PB2) and SCK (PB1) 
     * lines are not automatically switched to output.
     */
    if (sppi0_spcr[ix] & _BV(CPOL)) {
        cbi(PORTB, 1);
    } else {
        sbi(PORTB, 1);
    }
    sbi(DDRB, 1);
    cbi(PORTB, 2);
    sbi(DDRB, 2);

    /* Enable MISO pull-up to avoid floating. */
    sbi(PORTB, 3);

    /*
     * When SS (PB0) is configured as input, we will be forced
     * into slave mode if this pin goes low. Enable the pull-up.
     */
    if (bit_is_clear(DDRB, 0)) {
        sbi(PORTB, 0);
    }

    /* Set SPI mode and optionally the double speed bit. */
    outb(SPCR, sppi0_spcr[ix]);
#if defined(SPI2X)
    outb(SPSR, sppi0_spsr[ix]);
#endif

    /* Clean-up the status. */
    ix = inb(SPSR);
    ix = inb(SPDR);
}

/*!
 * \brief Set or clear a configured reset line for a given device.
 *
 * Reset lines must be configured when building the system libraries.
 * This routine silently ignores them, if not configured.
 *
 * \param ix The device index, starting at 0.
 * \param hi If 0, the reset line is driven low. Otherwise the
 *           line is driven high.
 */
void Sppi0ChipReset(uint8_t ix, uint8_t hi)
{
#if defined(SPPI0_RST0_BIT)
    if (ix == 0) {
        if (hi) {
            SPPI0_RST0_SET();
        } else {
            SPPI0_RST0_CLR();
        }
        SPPI0_RST0_ENA();
    }
#endif
#if defined(SPPI0_RST1_BIT)
    if (ix == 1) {
        if (hi) {
            SPPI0_RST1_SET();
        } else {
            SPPI0_RST1_CLR();
        }
        SPPI0_RST1_ENA();
    }
#endif
#if defined(SPPI0_RST2_BIT)
    if (ix == 2) {
        if (hi) {
            SPPI0_RST2_SET();
        } else {
            SPPI0_RST2_CLR();
        }
        SPPI0_RST2_ENA();
    }
#endif
#if defined(SPPI0_RST3_BIT)
    if (ix == 3) {
        if (hi) {
            SPPI0_RST3_SET();
        } else {
            SPPI0_RST3_CLR();
        }
        SPPI0_RST3_ENA();
    }
#endif
}

/*!
 * \brief Set or clear a configured chip select for a given device.
 *
 * Chip selects must be configured when building the system libraries.
 * This routine silently ignores selects, if they are not configured.
 *
 * \param ix The device index, starting at 0. Same as the chip select
 *           number.
 * \param hi If 0, the chip select is driven low. Otherwise the
 *           line is driven high.
 */
void Sppi0ChipSelect(uint8_t ix, uint8_t hi)
{
#if defined(SPPI0_CS0_BIT)
    if (ix == 0) {
        if (hi) {
            SPPI0_CS0_SET();
        } else {
            SPPI0_CS0_CLR();
        }
        SPPI0_CS0_ENA();
    }
#endif
#if defined(SPPI0_CS1_BIT)
    if (ix == 1) {
        if (hi) {
            SPPI0_CS1_SET();
        } else {
            SPPI0_CS1_CLR();
        }
        SPPI0_CS1_ENA();
    }
#endif
#if defined(SPPI0_CS2_BIT)
    if (ix == 2) {
        if (hi) {
            SPPI0_CS2_SET();
        } else {
            SPPI0_CS2_CLR();
        }
        SPPI0_CS2_ENA();
    }
#endif
#if defined(SPPI0_CS3_BIT)
    if (ix == 3) {
        if (hi) {
            SPPI0_CS3_SET();
        } else {
            SPPI0_CS3_CLR();
        }
        SPPI0_CS3_ENA();
    }
#endif
}

/*!
 * \brief Select the device at a given chip select.
 *
 * Enables the serial peripheral interface with the parameters
 * previously set for the given device by Sppi0SetMode() and 
 * Sppi0SetSpeed(). Then the configured chip select line is
 * driven high.
 *
 * \param ix The device index, starting at 0. The routine will not 
 *           check if this is a valid number.
 */
void Sppi0SelectDevice(uint8_t ix)
{
    Sppi0Enable(ix);
    Sppi0ChipSelect(ix, 1);
}

/*!
 * \brief Deselect the device at a given chip select.
 *
 * The configured chip select line will be driven low.
 *
 * \param ix The device index, starting at 0. The routine will not 
 *           check if this is a valid number.
 */
void Sppi0DeselectDevice(uint8_t ix)
{
    Sppi0ChipSelect(ix, 0);
}

/*!
 * \brief Select the device at a given negated chip select.
 *
 * Enables the serial peripheral interface with the parameters
 * previously set for the given device by Sppi0SetMode() and 
 * Sppi0SetSpeed(). Then the configured chip select line is
 * driven low.
 *
 * \param ix The device index, starting at 0. The routine will not 
 *           check if this is a valid number.
 */
void Sppi0NegSelectDevice(uint8_t ix)
{
    Sppi0Enable(ix);
    Sppi0ChipSelect(ix, 0);
}

/*!
 * \brief Deselect the device at a given negated chip select.
 *
 * The configured chip select line will be driven high.
 *
 * \param ix The device index, starting at 0. The routine will not 
 *           check if this is a valid number.
 */
void Sppi0NegDeselectDevice(uint8_t ix)
{
    Sppi0ChipSelect(ix, 1);
}

/*!
 * \brief Exchange a byte with the currently selected SPI device.
 *
 * \param data Byte to transmit.
 *
 * \return Received byte.
 */
uint8_t Sppi0Byte(uint8_t data)
{
    outb(SPDR, data);
    loop_until_bit_is_set(SPSR, SPIF);

    return inb(SPDR);
}

/*!
 * \brief Exchange a specified number of bytes with the currently selected SPI device.
 *
 * It is assumed, that the serial peripheral interface has been properly
 * initialized by calling Sppi0SetMode() and optionally Sppi0SetSpeed().
 *
 * Further it is assumed, that the chip select (if there is one) had
 * been enabled by a previous call to Sppi0SelectDevice() or 
 * Sppi0NegSelectDevice().
 *
 * \param wdata Pointer to the data to transmit.
 * \param rdata Pointer to a buffer that receives the data from the device.
 *              Can be set to NULL for transmit only. May also point to the
 *              tranmit buffer, in which case the transmitted bytes are
 *              replaced by the bytes received.
 */
void Sppi0Transact(CONST void *wdata, void *rdata, size_t len)
{
    CONST uint8_t *wp = (CONST uint8_t *)wdata;

    if (rdata) {
        uint8_t *rp = (uint8_t *)rdata;

        while(len--) {
            *rp++ = Sppi0Byte(*wp);
            wp++;
        }
    } else {
        while(len--) {
            Sppi0Byte(*wp);
            wp++;
        }
    }
}
