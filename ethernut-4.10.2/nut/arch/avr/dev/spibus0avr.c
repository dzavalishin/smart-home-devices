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
 * \file arch/avr/dev/spibus0avr.c
 * \brief AVR SPI bus 0 driver.
 *
 * May be configured as an interrupt driven or polling driver. The
 * interrupt driven version may use single or double buffering.
 *
 * Single buffer interrupt mode is the default.
 *
 * Polling mode requires about 300 bytes less code and is roughly 10%
 * faster. However, it blocks the CPU during the whole transfer.
 *
 * Double buffering additionally requires about 200 bytes of code space
 * and is only slightly faster (less than 1%).
 *
 * \verbatim
 * $Id: spibus0avr.c 2467 2009-02-17 09:30:02Z haraldkipp $
 * \endverbatim
 */

#include <cfg/spi.h>
#include <cfg/arch/gpio.h>

#include <dev/spibus_avr.h>
#include <dev/irqreg.h>
#include <sys/event.h>
#include <sys/nutdebug.h>

#include <errno.h>
#include <stdlib.h>
#include <memdebug.h>

#if defined(SPI0_CS0_PIO_BIT)
#if defined(SPI0_CS0_PIO_ID)
#undef GPIO_ID
#define GPIO_ID SPI0_CS0_PIO_ID
#include <cfg/arch/porttran.h>
static INLINE void SPI0_CS0_LO(void) { GPIO_SET_LO(SPI0_CS0_PIO_BIT); }
static INLINE void SPI0_CS0_HI(void) { GPIO_SET_HI(SPI0_CS0_PIO_BIT); }
static INLINE void SPI0_CS0_SO(void) { GPIO_OUTPUT(SPI0_CS0_PIO_BIT); }
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
static INLINE void SPI0_CS1_LO(void) { GPIO_SET_LO(SPI0_CS1_PIO_BIT); }
static INLINE void SPI0_CS1_HI(void) { GPIO_SET_HI(SPI0_CS1_PIO_BIT); }
static INLINE void SPI0_CS1_SO(void) { GPIO_OUTPUT(SPI0_CS1_PIO_BIT); }
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
static INLINE void SPI0_CS2_LO(void) { GPIO_SET_LO(SPI0_CS2_PIO_BIT); }
static INLINE void SPI0_CS2_HI(void) { GPIO_SET_HI(SPI0_CS2_PIO_BIT); }
static INLINE void SPI0_CS2_SO(void) { GPIO_OUTPUT(SPI0_CS2_PIO_BIT); }
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
static INLINE void SPI0_CS3_LO(void) { GPIO_SET_LO(SPI0_CS3_PIO_BIT); }
static INLINE void SPI0_CS3_HI(void) { GPIO_SET_HI(SPI0_CS3_PIO_BIT); }
static INLINE void SPI0_CS3_SO(void) { GPIO_OUTPUT(SPI0_CS3_PIO_BIT); }
#else
#define SPI0_CS3_LO()
#define SPI0_CS3_HI()
#define SPI0_CS4_SO()
#endif
#endif

/*!
 * \brief Set the specified chip select to a given level.
 */
static int AvrSpi0ChipSelect(uint_fast8_t cs, uint_fast8_t hi)
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

static uint8_t * volatile spi0_txp;
static uint8_t * volatile spi0_rxp;

#ifndef SPIBUS0_POLLING_MODE

static HANDLE spi0_que;
static volatile size_t spi0_xc;

#ifdef SPIBUS0_DOUBLE_BUFFER
/* Buffers used for double buffering. */
static uint8_t * volatile spi0_ntxp;
static uint8_t * volatile spi0_nrxp;
static volatile size_t spi0_nxc;
#endif

/*!
 * \brief AVR SPI interrupt handler.
 */
static void AvrSpi0Interrupt(void *arg)
{
    uint8_t b;

    /* Get the received byte. */
    b = inb(SPDR);
    if (spi0_xc) {
        if (spi0_rxp) {
            *spi0_rxp++ = b;
        }
        spi0_xc--;
    }
    if (spi0_xc == 0) {
#ifdef SPIBUS0_DOUBLE_BUFFER
        if (spi0_nxc) {
            /* More data in secondary buffer. */
            spi0_txp = spi0_ntxp;
            spi0_rxp = spi0_nrxp;
            spi0_xc = spi0_nxc;
            spi0_nxc = 0;
        }
#endif
        NutEventPostFromIrq(&spi0_que);
    }
    if (spi0_xc) {
        if (spi0_txp) {
            b = *spi0_txp++;
        }
        outb(SPDR, b);
    }
}
#endif /* SPIBUS0_POLLING_MODE */

/*! 
 * \brief Transfer data on the SPI bus.
 *
 * A device must have been selected by calling AvrSpi0Select().
 *
 * Depending on the configuration, this routine implemets polling or 
 * interrupt mode. For the latter either single or double buffering
 * may have been selected.
 *
 * When using double buffered interrupt mode, then the transfer may
 * be still in progress when returning from this function.
 *
 * \param node Specifies the SPI bus node.
 * \param txbuf Pointer to the transmit buffer. If NULL, undetermined
 *              byte values are transmitted.
 * \param rxbuf Pointer to the receive buffer. If NULL, then incoming
 *              data is discarded.
 * \param xlen  Number of bytes to transfer.
 *
 * \return Always 0.
 */
int AvrSpiBus0Transfer(NUTSPINODE * node, CONST void *txbuf, void *rxbuf, int xlen)
{
    uint8_t b = 0xff;

    /* Sanity check. */
    NUTASSERT(node != NULL);

#ifdef SPIBUS0_POLLING_MODE
    /*
     * Polling mode.
     */
    spi0_txp = (uint8_t *) txbuf;
    spi0_rxp = (uint8_t *) rxbuf;

    while (xlen--) {
        if (spi0_txp) {
            b = *spi0_txp++;
        }
        outb(SPDR, b);
        while ((inb(SPSR) & 0x80) == 0);
        b = inb(SPDR);
        if (spi0_rxp) {
            *spi0_rxp++ = b;
        }
    }
#else                           /* SPIBUS0_POLLING_MODE */
    if (xlen) {
#ifdef SPIBUS0_DOUBLE_BUFFER
        /*
         * Double buffer interrupt mode.
         */
        cbi(SPCR, SPIE);
        /* Wait until secondary buffer is available. */
        while (spi0_nxc) {
            sbi(SPCR, SPIE);
            NutEventWait(&spi0_que, NUT_WAIT_INFINITE);
            cbi(SPCR, SPIE);
        }
        if (spi0_xc) {
            /* Primary buffer in use. Prepare secondary buffer. */
            spi0_ntxp = (uint8_t *) txbuf;
            spi0_nrxp = (uint8_t *) rxbuf;
            spi0_nxc = (size_t) xlen;
            sbi(SPCR, SPIE);
        } else {
            spi0_txp = (uint8_t *) txbuf;
            spi0_rxp = (uint8_t *) rxbuf;
            spi0_xc = (size_t) xlen;
            if (spi0_txp) {
                b = *spi0_txp++;
            }
            /* Enable and kick interrupts. */
            sbi(SPCR, SPIE);
            outb(SPDR, b);
        }
#else                           /* SPIBUS0_DOUBLE_BUFFER */
        /*
         * Single buffer interrupt mode.
         */
        spi0_txp = (uint8_t *) txbuf;
        spi0_rxp = (uint8_t *) rxbuf;
        spi0_xc = (size_t) xlen;
        if (spi0_txp) {
            b = *spi0_txp++;
        }
        /* Enable and kick interrupts. */
        sbi(SPCR, SPIE);
        outb(SPDR, b);
        /* Wait until transfer has finished. */
        NutEventWait(&spi0_que, NUT_WAIT_INFINITE);
        cbi(SPCR, SPIE);
#endif                          /* SPIBUS0_DOUBLE_BUFFER */
    }
#endif                          /* SPIBUS0_POLLING_MODE */
    return 0;
}

#ifdef SPIBUS0_DOUBLE_BUFFER
/*! 
 * \brief Wait until all SPI bus transfers are done.
 *
 * \param node Specifies the SPI bus node.
 * \param tmo  Timeout in milliseconds. To disable timeout, set this
 *             parameter to NUT_WAIT_INFINITE.
 *
 * \return Always 0.
 */
int AvrSpiBus0Wait(NUTSPINODE * node, uint32_t tmo)
{
    cbi(SPCR, SPIE);
    while (spi0_xc) {
        sbi(SPCR, SPIE);
        if (NutEventWait(&spi0_que, tmo)) {
            return -1;
        }
        cbi(SPCR, SPIE);
    }
    return 0;
}
#endif                          /* SPIBUS0_DOUBLE_BUFFER */

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
int AvrSpiBus0NodeInit(NUTSPINODE * node)
{
    int rc;

    /* Sanity check. */
    NUTASSERT(node != NULL);

    /* Try to deactivate the node's chip select. */
    rc = AvrSpi0ChipSelect(node->node_cs, (node->node_mode & SPI_MODE_CSHIGH) == 0);
    /* It should not hurt us being called more than once. Thus, we
       ** check wether any initialization had been taken place already. */
    if (rc == 0 && node->node_stat == NULL) {
        /* Allocate our shadow registers. */
        node->node_stat = malloc(sizeof(AVRSPIREG));
        if (node->node_stat) {
            AvrSpiSetup(node);
#ifndef SPIBUS0_POLLING_MODE
            /* Register and enable SPI interrupt handler. */
            NutRegisterIrqHandler(node->node_bus->bus_sig, AvrSpi0Interrupt, NULL);
#endif
        } else {
            /* Out of memory? */
            rc = -1;
        }
    }
    return rc;
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
int AvrSpiBus0Select(NUTSPINODE * node, uint32_t tmo)
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
        AVRSPIREG *spireg = node->node_stat;

        /* If the mode update bit is set, then update our shadow registers. */
        if (node->node_mode & SPI_MODE_UPDATE) {
            AvrSpiSetup(node);
        }

        /* Even when set to master mode, the SCK pin is not automatically 
           ** switched to output. Do it manually, maintaining the polarity. */
        if (spireg->avrspi_spcr & _BV(CPOL)) {
            cbi(PORTB, 1);
        } else {
            sbi(PORTB, 1);
        }
        sbi(DDRB, 1);

        /* Also MOSI requires manual setting. */
        cbi(PORTB, 2);
        sbi(DDRB, 2);

        /* When SS is configured as input, we may be forced into slave 
           ** mode if this pin goes low. Enable the pull-up. */
        if (bit_is_clear(DDRB, 0)) {
            sbi(PORTB, 0);
        }

        /* Enable MISO pull-up to avoid floating. */
        sbi(PORTB, 3);

        /* Set SPI mode for this device, using the shadow registers. */
        outb(SPCR, spireg->avrspi_spcr);
#if defined(SPI2X)
        outb(SPSR, spireg->avrspi_spsr);
#endif

        /* Clean-up the status. */
        {
            uint8_t ix = inb(SPSR);
            ix = inb(SPDR);
        }

        /* Finally activate the node's chip select. */
        rc = AvrSpi0ChipSelect(node->node_cs, (node->node_mode & SPI_MODE_CSHIGH) != 0);
        if (rc) {
            /* Release the bus in case of an error. */
            NutEventPost(&node->node_bus->bus_mutex);
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
int AvrSpiBus0Deselect(NUTSPINODE * node)
{
    /* Sanity check. */
    NUTASSERT(node != NULL);
    NUTASSERT(node->node_bus != NULL);

#ifdef SPIBUS0_DOUBLE_BUFFER
    AvrSpiBus0Wait(node, NUT_WAIT_INFINITE);
#endif
    /* Deactivate the node's chip select. */
    AvrSpi0ChipSelect(node->node_cs, (node->node_mode & SPI_MODE_CSHIGH) == 0);

    /* Release the bus. */
    NutEventPost(&node->node_bus->bus_mutex);

    return 0;
}

/*!
 * \brief AVR SPI bus driver implementation structure.
 */
NUTSPIBUS spiBus0Avr = {
    NULL,                       /*!< Bus mutex semaphore (bus_mutex). */
    NULL,                       /*!< Bus ready signal (bus_ready). */
    0,                          /*!< Unused bus base address (bus_base). */
    &sig_SPI,                   /*!< Bus interrupt handler (bus_sig). */
    AvrSpiBus0NodeInit,         /*!< Initialize the bus (bus_initnode). */
    AvrSpiBus0Select,           /*!< Select the specified device (bus_alloc). */
    AvrSpiBus0Deselect,         /*!< Deselect the specified device (bus_release). */
    AvrSpiBus0Transfer,         /*!< Transfer data to and from a specified device (bus_transfer). */
#ifdef SPIBUS0_DOUBLE_BUFFER
    AvrSpiBus0Wait,
#else
    NutSpiBusWait,              /*!< Wait for bus transfer ready (bus_wait). */
#endif
    NutSpiBusSetMode,           /*!< Set SPI mode of a specified device (bus_set_mode). */
    NutSpiBusSetRate,           /*!< Set clock rate of a specified device (bus_set_rate). */
    NutSpiBusSetBits            /*!< Set number of data bits of a specified device (bus_set_bits). */
};
