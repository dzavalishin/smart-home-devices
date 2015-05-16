/*
 * Copyright (C) 2009 by egnite GmbH
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
 * \file arch/avr/dev/spibus0gpio.c
 * \brief GPIO bit banging SPI bus 0 driver.
 *
 * \verbatim
 * $Id: spibus0gpio.c 2405 2009-01-18 16:44:08Z haraldkipp $
 * \endverbatim
 */

#include <cfg/spi.h>
#include <cfg/arch/gpio.h>

#include <dev/gpio.h>
#include <dev/spibus_gpio.h>
#include <sys/event.h>
#include <sys/timer.h>
#include <sys/nutdebug.h>

#include <errno.h>
#include <stdlib.h>

/* Targets with single ports may not have set the port number. */
#if defined(SBBI0_MISO_BIT) && !defined(SBBI0_MISO_PORT)
#define SBBI0_MISO_PORT NUTGPIO_PORT
#endif
#if defined(SBBI0_MOSI_BIT) && !defined(SBBI0_MOSI_PORT)
#define SBBI0_MOSI_PORT NUTGPIO_PORT
#endif
#if defined(SBBI0_SCK_BIT) && !defined(SBBI0_SCK_PORT)
#define SBBI0_SCK_PORT  NUTGPIO_PORT
#endif
#if defined(SBBI0_CS0_BIT) && !defined(SBBI0_CS0_PORT)
#define SBBI0_CS0_PORT  NUTGPIO_PORT
#endif
#if defined(SBBI0_CS1_BIT) && !defined(SBBI0_CS1_PORT)
#define SBBI0_CS1_PORT  NUTGPIO_PORT
#endif
#if defined(SBBI0_CS2_BIT) && !defined(SBBI0_CS2_PORT)
#define SBBI0_CS2_PORT  NUTGPIO_PORT
#endif
#if defined(SBBI0_CS3_BIT) && !defined(SBBI0_CS3_PORT)
#define SBBI0_CS3_PORT  NUTGPIO_PORT
#endif

static GSPIREG gspi_reg0;
#if defined(SBBI0_CS1_BIT)
static GSPIREG gspi_reg1;
#endif
#if defined(SBBI0_CS2_BIT)
static GSPIREG gspi_reg2;
#endif
#if defined(SBBI0_CS3_BIT)
static GSPIREG gspi_reg3;
#endif

/*!
 * \brief Set the specified chip select to a given level.
 */
static GSPIREG *GpioSpi0ChipSelect(uint_fast8_t cs, uint_fast8_t hi)
{
    GSPIREG *rc;

    switch (cs) {
    case 0:
        /* If CS0 is undefined, we assume permanent selection. */
#if defined(SBBI0_CS0_BIT)
        GpioPinSet(SBBI0_CS0_PORT, SBBI0_CS0_BIT, hi);
        GpioPinConfigSet(SBBI0_CS0_PORT, SBBI0_CS0_BIT, GPIO_CFG_OUTPUT);
#endif
        rc = &gspi_reg0;
        break;
#if defined(SBBI0_CS1_BIT)
    case 1:
        GpioPinSet(SBBI0_CS1_PORT, SBBI0_CS1_BIT, hi);
        GpioPinConfigSet(SBBI0_CS1_PORT, SBBI0_CS1_BIT, GPIO_CFG_OUTPUT);
        rc = &gspi_reg1;
        break;
#endif
#if defined(SBBI0_CS2_BIT)
    case 2:
        GpioPinSet(SBBI0_CS2_PORT, SBBI0_CS2_BIT, hi);
        GpioPinConfigSet(SBBI0_CS2_PORT, SBBI0_CS2_BIT, GPIO_CFG_OUTPUT);
        rc = &gspi_reg2;
        break;
#endif
#if defined(SBBI0_CS3_BIT)
    case 3:
        GpioPinSet(SBBI0_CS3_PORT, SBBI0_CS3_BIT, hi);
        GpioPinConfigSet(SBBI0_CS3_PORT, SBBI0_CS3_BIT, GPIO_CFG_OUTPUT);
        rc = &gspi_reg3;
        break;
#endif
    default:
        errno = EIO;
        rc = NULL;
        break;
    }
    return rc;
}

/* Idle clock is low and data is captured on the rising edge. */
static void SpiMode0Transfer(GSPIREG *gspi, CONST uint8_t *txbuf, uint8_t *rxbuf, int xlen)
{
#if defined(SBBI0_SCK_BIT)
    uint_fast8_t mask;

    while (xlen--) {
        for (mask = 0x80; mask; mask >>= 1) {
#if defined(SBBI0_MOSI_BIT)
            if (txbuf) {
                GpioPinSet(SBBI0_MOSI_PORT, SBBI0_MOSI_BIT, (*txbuf & mask) != 0);
            }
#endif /* SBBI0_MOSI_BIT */
            NutMicroDelay(gspi->gspi_dly_rate);
            GpioPinSetHigh(SBBI0_SCK_PORT, SBBI0_SCK_BIT);
#if defined(SBBI0_MISO_BIT)
            if (rxbuf) {
                if (GpioPinGet(SBBI0_MISO_PORT, SBBI0_MISO_BIT)) {
                    *rxbuf |= mask;
                }
                else {
                    *rxbuf &= ~mask;
                }
            }
#endif /* SBBI0_MISO_BIT */
            NutMicroDelay(gspi->gspi_dly_rate);
            GpioPinSetLow(SBBI0_SCK_PORT, SBBI0_SCK_BIT);
        }
        if (txbuf) {
            txbuf++;
        }
        if (rxbuf) {
            rxbuf++;
        }
    }
#endif /* SBBI0_SCK_BIT */
}

/* Idle clock is low and data is captured on the falling edge. */
static void SpiMode1Transfer(GSPIREG *gspi, CONST uint8_t *txbuf, uint8_t *rxbuf, int xlen)
{
#if defined(SBBI0_SCK_BIT)
    uint_fast8_t mask;

    while (xlen--) {
        for (mask = 0x80; mask; mask >>= 1) {
            NutMicroDelay(gspi->gspi_dly_rate);
            GpioPinSetHigh(SBBI0_SCK_PORT, SBBI0_SCK_BIT);
#if defined(SBBI0_MOSI_BIT)
            if (txbuf) {
                GpioPinSet(SBBI0_MOSI_PORT, SBBI0_MOSI_BIT, (*txbuf & mask) != 0);
            }
#endif /* SBBI0_MOSI_BIT */
            NutMicroDelay(gspi->gspi_dly_rate);
            GpioPinSetLow(SBBI0_SCK_PORT, SBBI0_SCK_BIT);
#if defined(SBBI0_MISO_BIT)
            if (rxbuf) {
                if (GpioPinGet(SBBI0_MISO_PORT, SBBI0_MISO_BIT)) {
                    *rxbuf |= mask;
                }
                else {
                    *rxbuf &= ~mask;
                }
            }
#endif /* SBBI0_MISO_BIT */
        }
        if (txbuf) {
            txbuf++;
        }
        if (rxbuf) {
            rxbuf++;
        }
    }
#endif /* SBBI0_SCK_BIT */
}

/* Idle clock is high and data is captured on the falling edge. */
static void SpiMode2Transfer(GSPIREG *gspi, CONST uint8_t *txbuf, uint8_t *rxbuf, int xlen)
{
#if defined(SBBI0_SCK_BIT)
    uint_fast8_t mask;

    while (xlen--) {
        for (mask = 0x80; mask; mask >>= 1) {
#if defined(SBBI0_MOSI_BIT)
            if (txbuf) {
                GpioPinSet(SBBI0_MOSI_PORT, SBBI0_MOSI_BIT, (*txbuf & mask) != 0);
            }
#endif /* SBBI0_MOSI_BIT */
            NutMicroDelay(gspi->gspi_dly_rate);
            GpioPinSetLow(SBBI0_SCK_PORT, SBBI0_SCK_BIT);
#if defined(SBBI0_MISO_BIT)
            if (rxbuf) {
                if (GpioPinGet(SBBI0_MISO_PORT, SBBI0_MISO_BIT)) {
                    *rxbuf |= mask;
                }
                else {
                    *rxbuf &= ~mask;
                }
            }
#endif /* SBBI0_MISO_BIT */
            NutMicroDelay(gspi->gspi_dly_rate);
            GpioPinSetHigh(SBBI0_SCK_PORT, SBBI0_SCK_BIT);
        }
        if (txbuf) {
            txbuf++;
        }
        if (rxbuf) {
            rxbuf++;
        }
    }
#endif /* SBBI0_SCK_BIT */
}

/* Idle clock is high and data is captured on the rising edge. */
static void SpiMode3Transfer(GSPIREG *gspi, CONST uint8_t *txbuf, uint8_t *rxbuf, int xlen)
{
#if defined(SBBI0_SCK_BIT)
    uint_fast8_t mask;

    while (xlen--) {
        for (mask = 0x80; mask; mask >>= 1) {
            NutMicroDelay(gspi->gspi_dly_rate);
            GpioPinSetLow(SBBI0_SCK_PORT, SBBI0_SCK_BIT);
#if defined(SBBI0_MOSI_BIT)
            if (txbuf) {
                GpioPinSet(SBBI0_MOSI_PORT, SBBI0_MOSI_BIT, (*txbuf & mask) != 0);
            }
#endif /* SBBI0_MOSI_BIT */
            NutMicroDelay(gspi->gspi_dly_rate);
            GpioPinSetHigh(SBBI0_SCK_PORT, SBBI0_SCK_BIT);
#if defined(SBBI0_MISO_BIT)
            if (rxbuf) {
                if (GpioPinGet(SBBI0_MISO_PORT, SBBI0_MISO_BIT)) {
                    *rxbuf |= mask;
                }
                else {
                    *rxbuf &= ~mask;
                }
            }
#endif /* SBBI0_MISO_BIT */
        }
        if (txbuf) {
            txbuf++;
        }
        if (rxbuf) {
            rxbuf++;
        }
    }
#endif /* SBBI0_SCK_BIT */
}

/*! 
 * \brief Transfer data on the SPI bus.
 *
 * A device must have been selected by calling GpioSpi0Select().
 *
 * \param node  Specifies the SPI bus node.
 * \param txbuf Pointer to the transmit buffer. If NULL, undetermined
 *              byte values are transmitted.
 * \param rxbuf Pointer to the receive buffer. If NULL, then incoming
 *              data is discarded.
 * \param xlen  Number of bytes to transfer.
 *
 * \return Always 0.
 */
int GpioSpiBus0Transfer(NUTSPINODE * node, CONST void *txbuf, void *rxbuf, int xlen)
{
    GSPIREG *gspi;

    /* Sanity check. */
    NUTASSERT(node != NULL);
    NUTASSERT(node->node_stat != NULL);
    gspi = (GSPIREG *)node->node_stat;

    /* We use dedicated static routines for each mode in the hope that 
    ** this improves the compiler's optimization for the inner loop. */
    switch (node->node_mode & SPI_MODE_3) {
    case SPI_MODE_0:
        SpiMode0Transfer(gspi, txbuf, rxbuf, xlen);
        break;
    case SPI_MODE_1:
        SpiMode1Transfer(gspi, txbuf, rxbuf, xlen);
        break;
    case SPI_MODE_2:
        SpiMode2Transfer(gspi, txbuf, rxbuf, xlen);
        break;
    case SPI_MODE_3:
        SpiMode3Transfer(gspi, txbuf, rxbuf, xlen);
        break;
    }
    return 0;
}

/*! 
 * \brief Initialize an SPI bus node. 
 *
 * This routine is called for each SPI node, which is registered via 
 * NutRegisterSpiDevice().
 *
 * \param node Specifies the SPI bus node.
 *
 * \return 0 on success or -1 if there is no valid chip select.
 */
int GpioSpiBus0NodeInit(NUTSPINODE * node)
{
    /* Sanity check. */
    NUTASSERT(node != NULL);

    /* Try to deactivate the node's chip select. */
    node->node_stat = GpioSpi0ChipSelect(node->node_cs, (node->node_mode & SPI_MODE_CSHIGH) == 0);
    if (node->node_stat == NULL) {
        /* Chip select not configured. */
        return -1;
    }
    return GpioSpiSetup(node);
}

/*! \brief Select a device on the SPI bus.
 *
 * Locks and activates the bus for the specified node.
 *
 * \param node Specifies the SPI bus node.
 * \param tmo Timeout in milliseconds. To disable timeout, set this
 *            parameter to NUT_WAIT_INFINITE.
 *
 * \return 0 on success. In case of an error, -1 is returned and the bus 
 *         is not locked.
 */
int GpioSpiBus0Select(NUTSPINODE * node, uint32_t tmo)
{
    int rc;

    /* Sanity check. */
    NUTASSERT(node != NULL);
    NUTASSERT(node->node_stat != NULL);

    /* Allocate the bus. */
    rc = NutEventWait(&node->node_bus->bus_mutex, tmo);
    if (rc) {
        errno = EIO;
    } else {
        /* Do the update, if the mode update bit is set. */
        if (node->node_mode & SPI_MODE_UPDATE) {
            GpioSpiSetup(node);
        }
        /* Set clock output using the correct idle mode level. */
#if defined(SBBI0_SCK_BIT)
        GpioPinSetLow(SBBI0_SCK_PORT, (node->node_mode & SPI_MODE_CPOL) != 0);
        GpioPinConfigSet(SBBI0_SCK_PORT, SBBI0_SCK_BIT, GPIO_CFG_OUTPUT);
#endif
        /* Enable MOSI output and MISO input. */
#if defined(SBBI0_MOSI_BIT)
        GpioPinConfigSet(SBBI0_MOSI_PORT, SBBI0_MOSI_BIT, GPIO_CFG_OUTPUT);
#endif
#if defined(SBBI0_MISO_BIT)
        GpioPinConfigSet(SBBI0_MISO_PORT, SBBI0_MISO_BIT, 0);
#endif
        /* Activate the node's chip select. */
        if (GpioSpi0ChipSelect(node->node_cs, (node->node_mode & SPI_MODE_CSHIGH) != 0) == NULL) {
            /* Release the bus in case of an error. */
            NutEventPost(&node->node_bus->bus_mutex);
            rc = -1;
        }
    }
    return rc;
}

/*! \brief Deselect a device on the SPI bus.
 *
 * Deactivates the chip select and unlocks the bus.
 *
 * \param node Specifies the SPI bus node.
 *
 * \return Always 0.
 */
int GpioSpiBus0Deselect(NUTSPINODE * node)
{
    /* Sanity check. */
    NUTASSERT(node != NULL);
    NUTASSERT(node->node_bus != NULL);

    /* Deactivate the node's chip select. */
    GpioSpi0ChipSelect(node->node_cs, (node->node_mode & SPI_MODE_CSHIGH) == 0);

    /* Release the bus. */
    NutEventPost(&node->node_bus->bus_mutex);

    return 0;
}

/*!
 * \brief AVR SPI bus driver implementation structure.
 */
NUTSPIBUS spiBus0Gpio = {
    NULL,                       /*!< Bus mutex semaphore (bus_mutex). */
    NULL,                       /*!< Bus ready signal (bus_ready). */
    0,                          /*!< Unused bus base address (bus_base). */
    NULL,                       /*!< Bus interrupt handler (bus_sig). */
    GpioSpiBus0NodeInit,        /*!< Initialize the bus (bus_initnode). */
    GpioSpiBus0Select,          /*!< Select the specified device (bus_alloc). */
    GpioSpiBus0Deselect,        /*!< Deselect the specified device (bus_release). */
    GpioSpiBus0Transfer,        /*!< Transfer data to and from a specified device (bus_transfer). */
    NutSpiBusWait,              /*!< Wait for bus transfer ready (bus_wait). */
    NutSpiBusSetMode,           /*!< Set SPI mode of a specified device (bus_set_mode). */
    NutSpiBusSetRate,           /*!< Set clock rate of a specified device (bus_set_rate). */
    NutSpiBusSetBits            /*!< Set number of data bits of a specified device (bus_set_bits). */
};
