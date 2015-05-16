/*
 * Copyright (C) 2008-2009 by egnite GmbH
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
 * \file arch/arm/dev/spibus0at91.c
 * \brief Primary AT91 SPI bus controller.
 *
 * May be configured as an interrupt driven or polling driver. The
 * interrupt driven version may use single or double buffering.
 *
 * \verbatim
 * $Id: spibus0at91.c 3559 2011-08-16 10:51:04Z haraldkipp $
 * \endverbatim
 */

#include <cfg/spi.h>
#include <cfg/arch/gpio.h>

#include <dev/spibus_at91.h>
#include <dev/irqreg.h>
#include <sys/event.h>
#include <sys/nutdebug.h>

#include <stdlib.h>
#include <errno.h>

#if defined(SPI0_CS0_PIO_BIT)
#if defined(SPI0_CS0_PIO_ID)
#undef GPIO_ID
#define GPIO_ID SPI0_CS0_PIO_ID
#include <cfg/arch/porttran.h>
static INLINE void SPI0_CS0_LO(void)
{
    GPIO_SET_LO(SPI0_CS0_PIO_BIT);
}

static INLINE void SPI0_CS0_HI(void)
{
    GPIO_SET_HI(SPI0_CS0_PIO_BIT);
}

static INLINE void SPI0_CS0_SO(void)
{
    GPIO_ENABLE(SPI0_CS0_PIO_BIT);
    GPIO_OUTPUT(SPI0_CS0_PIO_BIT);
}
#else
#define SPI0_CS0_LO()
#define SPI0_CS0_HI()
#define SPI0_CS0_SO()
#endif
#endif

#if defined(SPI0_CS1_PIO_BIT)
#if defined(SPI0_CS1_PIO_ID)
#undef GPIO_ID
#define GPIO_ID SPI0_CS1_PIO_ID
#include <cfg/arch/porttran.h>
static INLINE void SPI0_CS1_LO(void)
{
    GPIO_SET_LO(SPI0_CS1_PIO_BIT);
}

static INLINE void SPI0_CS1_HI(void)
{
    GPIO_SET_HI(SPI0_CS1_PIO_BIT);
}

static INLINE void SPI0_CS1_SO(void)
{
    GPIO_ENABLE(SPI0_CS1_PIO_BIT);
    GPIO_OUTPUT(SPI0_CS1_PIO_BIT);
}
#else
#define SPI0_CS1_LO()
#define SPI0_CS1_HI()
#define SPI0_CS1_SO()
#endif
#endif

#if defined(SPI0_CS2_PIO_BIT)
#if defined(SPI0_CS2_PIO_ID)
#undef GPIO_ID
#define GPIO_ID SPI0_CS2_PIO_ID
#include <cfg/arch/porttran.h>
static INLINE void SPI0_CS2_LO(void)
{
    GPIO_SET_LO(SPI0_CS2_PIO_BIT);
}

static INLINE void SPI0_CS2_HI(void)
{
    GPIO_SET_HI(SPI0_CS2_PIO_BIT);
}

static INLINE void SPI0_CS2_SO(void)
{
    GPIO_ENABLE(SPI0_CS2_PIO_BIT);
    GPIO_OUTPUT(SPI0_CS2_PIO_BIT);
}
#else
#define SPI0_CS2_LO()
#define SPI0_CS2_HI()
#define SPI0_CS2_SO()
#endif
#endif

#if defined(SPI0_CS3_PIO_BIT)
#if defined(SPI0_CS3_PIO_ID)
#undef GPIO_ID
#define GPIO_ID SPI0_CS3_PIO_ID
#include <cfg/arch/porttran.h>
static INLINE void SPI0_CS3_LO(void)
{
    GPIO_SET_LO(SPI0_CS3_PIO_BIT);
}

static INLINE void SPI0_CS3_HI(void)
{
    GPIO_SET_HI(SPI0_CS3_PIO_BIT);
}

static INLINE void SPI0_CS3_SO(void)
{
    GPIO_ENABLE(SPI0_CS3_PIO_BIT);
    GPIO_OUTPUT(SPI0_CS3_PIO_BIT);
}
#else
#define SPI0_CS3_LO()
#define SPI0_CS3_HI()
#define SPI0_CS3_SO()
#endif
#endif

/*!
 * \brief Set the specified chip select to a given level.
 */
int At91Spi0ChipSelect(uint_fast8_t cs, uint_fast8_t hi)
{
    int rc = 0;

    switch (cs) {
#if defined(SPI0_CS0_PIO_BIT)
    case 0:
        if (hi) {
            SPI0_CS0_HI();
        } else {
            SPI0_CS0_LO();
        }
        SPI0_CS0_SO();
        break;
#endif
#if defined(SPI0_CS1_PIO_BIT)
    case 1:
        if (hi) {
            SPI0_CS1_HI();
        } else {
            SPI0_CS1_LO();
        }
        SPI0_CS1_SO();
        break;
#endif
#if defined(SPI0_CS2_PIO_BIT)
    case 2:
        if (hi) {
            SPI0_CS2_HI();
        } else {
            SPI0_CS2_LO();
        }
        SPI0_CS2_SO();
        break;
#endif
#if defined(SPI0_CS3_PIO_BIT)
    case 3:
        if (hi) {
            SPI0_CS3_HI();
        } else {
            SPI0_CS3_LO();
        }
        SPI0_CS3_SO();
        break;
#endif
    default:
        errno = EIO;
        rc = -1;
        break;
    }
    return rc;
}

/*! \brief Select a device on the first SPI bus.
 *
 * Locks and activates the bus for the specified node.
 *
 * \param node Specifies the SPI bus node.
 * \param tmo  Timeout in milliseconds. To disable timeout, set this
 *             parameter to NUT_WAIT_INFINITE.
 *
 * \return 0 on success. In case of an error, -1 is returned and the bus 
 *         is not locked.
 */
int At91SpiBus0Select(NUTSPINODE * node, uint32_t tmo)
{
    int rc;

    /* Sanity check. */
    NUTASSERT(node != NULL);
    NUTASSERT(node->node_bus != NULL);
    NUTASSERT(node->node_stat != NULL);

    /* Allocate the bus. */
    rc = NutEventWait(&node->node_bus->bus_mutex, tmo);
    if (rc) {
        errno = EIO;
    } else {
        AT91SPIREG *spireg = node->node_stat;

        outr(PMC_PCER, _BV(SPI0_ID));

        /* Enable SPI peripherals and clock. */
        outr(SPI0_PIO_BASE + SPI0_PSR_OFF, SPI0_PINS);
        outr(SPI0_PIO_BASE + PIO_PDR_OFF, SPI0_PINS);

        /* If the mode update bit is set, then update our shadow registers. */
        if (node->node_mode & SPI_MODE_UPDATE) {
            At91SpiSetup(node);
        }

        /* Enable SPI. */
        outr(SPI0_CR, SPI_SPIEN);
        /* Set SPI mode. */
        outr(SPI0_MR, spireg->at91spi_mr);
        outr(SPI0_CSR0 + node->node_cs * 4, spireg->at91spi_csr);

        /* Finally activate the node's chip select. */
        rc = At91Spi0ChipSelect(node->node_cs, (node->node_mode & SPI_MODE_CSHIGH) != 0);
        if (rc) {
            /* Release the bus in case of an error. */
            NutEventPost(&node->node_bus->bus_mutex);
        }
    }
    return rc;
}

/*! \brief Deselect a device on the first SPI bus.
 *
 * Deactivates the chip select and unlocks the bus.
 *
 * \param node Specifies the SPI bus node.
 *
 * \return Always 0.
 */
int At91SpiBus0Deselect(NUTSPINODE * node)
{
    /* Sanity check. */
    NUTASSERT(node != NULL);
    NUTASSERT(node->node_bus != NULL);

#ifdef SPIBUS0_DOUBLE_BUFFER
    At91SpiBusWait(node, NUT_WAIT_INFINITE);
#endif
    /* Deactivate the node's chip select. */
    At91Spi0ChipSelect(node->node_cs, (node->node_mode & SPI_MODE_CSHIGH) == 0);

#ifdef SPIBUS0_PIN_SHARING
    /* Disable SPI peripherals if pins are shared. */
    outr(SPI0_PIO_BASE + PIO_ODR_OFF, SPI0_PINS);
    outr(SPI0_PIO_BASE + PIO_PER_OFF, SPI0_PINS);
#endif

    /* Release the bus. */
    NutEventPost(&node->node_bus->bus_mutex);

    return 0;
}

#if !defined(SPIBUS0_POLLING_MODE) || !defined(SPIBUS0_DOUBLE_BUFFER)

static uint8_t * volatile spi0_txp;
static uint8_t * volatile spi0_rxp;
static volatile size_t spi0_xc;

void At91SpiBus0Interrupt(void *arg)
{
    uint8_t b;

    /* Get the received byte. */
    b = inb(SPI0_RDR);
    if (spi0_xc) {
        if (spi0_rxp) {
            *spi0_rxp++ = b;
        }
        spi0_xc--;
    }
    if (spi0_xc) {
        if (spi0_txp) {
            b = *spi0_txp++;
        }
        outb(SPI0_TDR, b);
    } else {
        NutEventPostFromIrq((void **)arg);
    }
}

/*! 
 * \brief Transfer data on the SPI bus using single buffered interrupt mode.
 *
 * A device must have been selected by calling At91SpiSelect().
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
int At91SpiBus0Transfer(NUTSPINODE * node, CONST void *txbuf, void *rxbuf, int xlen)
{
    uint8_t b = 0xff;
    uintptr_t base;

    /* Sanity check. */
    NUTASSERT(node != NULL);
    NUTASSERT(node->node_bus != NULL);
    NUTASSERT(node->node_bus->bus_base != 0);
    base = node->node_bus->bus_base;

    if (xlen) {
        spi0_txp = (uint8_t *) txbuf;
        spi0_rxp = (uint8_t *) rxbuf;
        spi0_xc = (size_t) xlen;
        if (spi0_txp) {
            b = *spi0_txp++;
        }
        /* Enable and kick interrupts. */
        outr(base + SPI_IER_OFF, SPI_RDRF);
        outr(base + SPI_TDR_OFF, b);
        /* Wait until transfer has finished. */
        NutEventWait(&node->node_bus->bus_ready, NUT_WAIT_INFINITE);
        outr(base + SPI_IDR_OFF, (unsigned int) - 1);
    }
    return 0;
}
#endif

/*!
 * \brief AT91 SPI bus driver implementation structure.
 */
NUTSPIBUS spiBus0At91 = {
    NULL,                       /*!< Bus mutex semaphore (bus_mutex). */
    NULL,                       /*!< Bus ready signal (bus_ready). */
    SPI0_BASE,                  /*!< Bus base address (bus_base). */
    &sig_SPI0,                  /*!< Bus interrupt handler (bus_sig). */
    At91SpiBusNodeInit,         /*!< Initialize the bus (bus_initnode). */
    At91SpiBus0Select,          /*!< Select the specified device (bus_alloc). */
    At91SpiBus0Deselect,        /*!< Deselect the specified device (bus_release). */
#if defined(SPIBUS0_POLLING_MODE)
    At91SpiBusPollTransfer,     /*!< Transfer data to and from a specified node (bus_transfer). */
#elif defined(SPIBUS0_DOUBLE_BUFFER)
    At91SpiBusDblBufTransfer,
#else
    At91SpiBus0Transfer,
#endif
#ifdef SPIBUS0_DOUBLE_BUFFER
    At91SpiBusWait,
#else
    NutSpiBusWait,              /*!< Wait for bus transfer ready (bus_wait). */
#endif
    NutSpiBusSetMode,           /*!< Set SPI mode of a specified device (bus_set_mode). */
    NutSpiBusSetRate,           /*!< Set clock rate of a specified device (bus_set_rate). */
    NutSpiBusSetBits            /*!< Set number of data bits of a specified device (bus_set_bits). */
};
