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
 * Revision 1.4  2009/01/18 16:46:47  haraldkipp
 * GPIO header file changed.
 *
 * Revision 1.3  2008/10/23 08:54:07  haraldkipp
 * Include the correct header file.
 *
 * Revision 1.2  2008/08/11 06:59:42  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.1  2007/04/12 09:07:54  haraldkipp
 * Configurable SPI added.
 *
 */

#include <sys/timer.h>

#include <dev/gpio.h>
#include <dev/sbbif0.h>

/* SPI modes of all devices. */
static ureg_t sbbi0_mode[SBBI0_MAX_DEVICES];

/* SPI mode of the currently selected device. */
static ureg_t sel_mode;

/* SPI bit delay of all devices. */
static ureg_t sbbi0_delay[SBBI0_MAX_DEVICES];

/* SPI bit delay of the currently selected device. */
static ureg_t sel_delay;

/*!
 * \brief Set mode for a given device at SPI0.
 *
 * This is typically the first call in order to use the device. It will
 * not directly enable the SPI hardware. This is done when selecting
 * the device.
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
int Sbbi0SetMode(ureg_t ix, ureg_t mode)
{
    if (ix < SBBI0_MAX_DEVICES && mode <= 3) {
        sbbi0_mode[ix] = mode;
        return 0;
    }
    return -1;
}

/*!
 * \brief Set transfer rate for a given device at SPI0.
 *
 * This is currently a dummy and does nothing.
 *
 * \param ix   The device index, starting at 0.
 * \param rate Transfer rate in bits per second.
 * 
 */
void Sbbi0SetSpeed(ureg_t ix, uint32_t rate)
{
    uint32_t fosc = NutGetCpuClock() / 16;

    if (rate) {
        fosc /= rate;
    }
    if (fosc < 5) {
        sbbi0_delay[ix] = 0;
    }
    else if (fosc < 255) {
        sbbi0_delay[ix] = (ureg_t)fosc;
    } else {
        sbbi0_delay[ix] = 255;
    }
}

static INLINE void Sbbi0Delay(void)
{
    if (sel_delay) {
        ureg_t d;

        for (d = sel_delay; d; d--) {
            _NOP();
        }
    }
}

/*!
 * \brief Enable the serial peripheral interface 0.
 *
 * Enables SPI with the parameters previously set by Sbbi0SetMode().
 *
 * \param ix The device index, starting at 0. The routine will not check 
 *           if this is valid.
 */
void Sbbi0Enable(ureg_t ix)
{
    sel_mode = sbbi0_mode[ix];
    sel_delay = sbbi0_delay[ix];

    SBBI0_MOSI_CLR();
    SBBI0_MOSI_ENA();
    SBBI0_MISO_ENA();
    if (sel_mode == 0 || sel_mode == 1) {
        SBBI0_SCK_CLR();
    }
    else {
        SBBI0_SCK_SET();
    }
    SBBI0_SCK_ENA();
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
void Sbbi0ChipReset(ureg_t ix, ureg_t hi)
{
#if defined(SBBI0_RST0_BIT)
    if (ix == 0) {
        if (hi) {
            SBBI0_RST0_SET();
        } else {
            SBBI0_RST0_CLR();
        }
        SBBI0_RST0_ENA();
    }
#endif
#if defined(SBBI0_RST1_BIT)
    if (ix == 1) {
        if (hi) {
            SBBI0_RST1_SET();
        } else {
            SBBI0_RST1_CLR();
        }
        SBBI0_RST1_ENA();
    }
#endif
#if defined(SBBI0_RST2_BIT)
    if (ix == 2) {
        if (hi) {
            SBBI0_RST2_SET();
        } else {
            SBBI0_RST2_CLR();
        }
        SBBI0_RST2_ENA();
    }
#endif
#if defined(SBBI0_RST3_BIT)
    if (ix == 3) {
        if (hi) {
            SBBI0_RST3_SET();
        } else {
            SBBI0_RST3_CLR();
        }
        SBBI0_RST3_ENA();
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
void Sbbi0ChipSelect(ureg_t ix, ureg_t hi)
{
#if defined(SBBI0_CS0_BIT)
    if (ix == 0) {
        if (hi) {
            SBBI0_CS0_SET();
        } else {
            SBBI0_CS0_CLR();
        }
        SBBI0_CS0_ENA();
    }
#endif
#if defined(SBBI0_CS1_BIT)
    if (ix == 1) {
        if (hi) {
            SBBI0_CS1_SET();
        } else {
            SBBI0_CS1_CLR();
        }
        SBBI0_CS1_ENA();
    }
#endif
#if defined(SBBI0_CS2_BIT)
    if (ix == 2) {
        if (hi) {
            SBBI0_CS2_SET();
        } else {
            SBBI0_CS2_CLR();
        }
        SBBI0_CS2_ENA();
    }
#endif
#if defined(SBBI0_CS3_BIT)
    if (ix == 3) {
        if (hi) {
            SBBI0_CS3_SET();
        } else {
            SBBI0_CS3_CLR();
        }
        SBBI0_CS3_ENA();
    }
#endif
}

/*!
 * \brief Select the device at a given chip select.
 *
 * Enables the serial peripheral interface with the parameters
 * previously set for the given device by Sbbi0SetMode() and 
 * Sbbi0SetSpeed(). Then the configured chip select line is
 * driven high.
 *
 * \param ix The device index, starting at 0. The routine will not 
 *           check if this is a valid number.
 */
void Sbbi0SelectDevice(ureg_t ix)
{
    Sbbi0Enable(ix);
    Sbbi0ChipSelect(ix, 1);
}

/*!
 * \brief Deselect the device at a given chip select.
 *
 * The configured chip select line will be driven low.
 *
 * \param ix The device index, starting at 0. The routine will not 
 *           check if this is a valid number.
 */
void Sbbi0DeselectDevice(ureg_t ix)
{
    Sbbi0ChipSelect(ix, 0);
}

/*!
 * \brief Select the device at a given negated chip select.
 *
 * Enables the serial peripheral interface with the parameters
 * previously set for the given device by Sbbi0SetMode() and 
 * Sbbi0SetSpeed(). Then the configured chip select line is
 * driven low.
 *
 * \param ix The device index, starting at 0. The routine will not 
 *           check if this is a valid number.
 */
void Sbbi0NegSelectDevice(ureg_t ix)
{
    Sbbi0Enable(ix);
    Sbbi0ChipSelect(ix, 0);
}

/*!
 * \brief Deselect the device at a given negated chip select.
 *
 * The configured chip select line will be driven high.
 *
 * \param ix The device index, starting at 0. The routine will not 
 *           check if this is a valid number.
 */
void Sbbi0NegDeselectDevice(ureg_t ix)
{
    Sbbi0ChipSelect(ix, 1);
}

/*!
 * \brief Exchange a byte with the currently selected SPI device.
 *
 * \param data Byte to transmit.
 *
 * \return Received byte.
 */
uint8_t Sbbi0Byte(uint8_t data)
{
    ureg_t mask;

    if (sel_mode == 3) {
        /* Mode 3: Leading edge is falling, data sampled on rising edge. */
        for (mask = 0x80; mask; mask >>= 1) {
            SBBI0_SCK_CLR();
            if (data & mask) {
                SBBI0_MOSI_SET();
            } else {
                SBBI0_MOSI_CLR();
            }
            Sbbi0Delay();
            SBBI0_SCK_SET();
            if (SBBI0_MISO_TST()) {
                data |= mask;
            }
            else {
                data &= ~mask;
            }
            Sbbi0Delay();
        }
    }
    else if (sel_mode == 2) {
        /* Mode 2: Leading edge is falling, data sampled on falling edge. */
        for (mask = 0x80; mask; mask >>= 1) {
            SBBI0_SCK_SET();
            if (data & mask) {
                SBBI0_MOSI_SET();
            } else {
                SBBI0_MOSI_CLR();
            }
            Sbbi0Delay();
            SBBI0_SCK_CLR();
            if (SBBI0_MISO_TST()) {
                data |= mask;
            }
            else {
                data &= ~mask;
            }
            Sbbi0Delay();
        }
        SBBI0_SCK_SET();
    }
    else if (sel_mode == 1) {
        /* Mode 1: Leading edge is rising, data sampled on falling edge. */
        for (mask = 0x80; mask; mask >>= 1) {
            SBBI0_SCK_SET();
            if (data & mask) {
                SBBI0_MOSI_SET();
            } else {
                SBBI0_MOSI_CLR();
            }
            Sbbi0Delay();
            SBBI0_SCK_CLR();
            if (SBBI0_MISO_TST()) {
                data |= mask;
            }
            else {
                data &= ~mask;
            }
            Sbbi0Delay();
        }
    }
    else {
        /* Mode 0: Leading edge is rising, data sampled on rising edge. */
        for (mask = 0x80; mask; mask >>= 1) {
            SBBI0_SCK_CLR();
            if (data & mask) {
                SBBI0_MOSI_SET();
            } else {
                SBBI0_MOSI_CLR();
            }
            Sbbi0Delay();
            SBBI0_SCK_SET();
            if (SBBI0_MISO_TST()) {
                data |= mask;
            }
            else {
                data &= ~mask;
            }
            Sbbi0Delay();
        }
        SBBI0_SCK_CLR();
    }
    return data;
}

/*!
 * \brief Exchange a specified number of bytes with the currently selected SPI device.
 *
 * It is assumed, that the serial peripheral interface has been properly
 * initialized by calling Sbbi0SetMode() and optionally Sbbi0SetSpeed().
 *
 * Further it is assumed, that the chip select (if there is one) had
 * been enabled by a previous call to Sbbi0SelectDevice() or 
 * Sbbi0NegSelectDevice().
 *
 * \param wdata Pointer to the data to transmit.
 * \param rdata Pointer to a buffer that receives the data from the device.
 *              Can be set to NULL for transmit only. May also point to the
 *              tranmit buffer, in which case the transmitted bytes are
 *              replaced by the bytes received.
 */
void Sbbi0Transact(CONST void *wdata, void *rdata, size_t len)
{
    CONST uint8_t *wp = (CONST uint8_t *)wdata;

    if (rdata) {
        uint8_t *rp = (uint8_t *)rdata;

        while(len--) {
            *rp++ = Sbbi0Byte(*wp);
            wp++;
        }
    } else {
        while(len--) {
            Sbbi0Byte(*wp);
            wp++;
        }
    }
}

