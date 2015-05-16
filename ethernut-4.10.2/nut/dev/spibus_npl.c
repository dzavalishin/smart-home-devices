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
 * \file arch/arm/dev/spibus_npl.c
 * \brief General SPI bus controller routines for Nut Programmable Logic.
 *
 * \verbatim
 * $Id$
 * \endverbatim
 */

#include <cfg/spi.h>

#include <sys/timer.h>
#include <sys/nutdebug.h>

#include <stdlib.h>
#include <memdebug.h>
#include <errno.h>

#include <dev/cy2239x.h>
#include <dev/npl.h>
#include <dev/spibus_npl.h>

#ifndef NPL_MMC_CLOCK
#define NPL_MMC_CLOCK   12500000
#endif

/*!
 * \brief Update SPI shadow registers.
 *
 * \param node Specifies the SPI bus node.
 *
 * \return Always 0.
 */
static int NplSpiSetup(NUTSPINODE * node)
{
#if defined(NUT_PLL_NPLCLK1)
    uint32_t clk;
    uint32_t clkdiv;

    NUTASSERT(node != NULL);

    /* Query the PLL number routed to Clock B. */
    clk = Cy2239xGetPll(NUT_PLL_NPLCLK1);
    /* Get the frequency of this PLL. */
    clk = Cy2239xPllGetFreq((int)clk, 7);
    /* Calculate the required divider value. */
    clkdiv = (clk + NPL_MMC_CLOCK - 10) / NPL_MMC_CLOCK;

    /* 
     * Not sure about the Cy-routines. The DIVSEL bit specifies which
     * divider is used, which is indirectly connected to S2, which is
     * high by default. For now set both dividers. 
     */
    if (Cy2239xSetDivider(NUT_PLL_NPLCLK1, 1, (int)clkdiv)) {
        return -1;
    }
    if (Cy2239xSetDivider(NUT_PLL_NPLCLK1, 0, (int)clkdiv)) {
        return -1;
    }

    /* Update interface parameters. */
    node->node_rate = clk / clkdiv;
    node->node_mode &= ~SPI_MODE_UPDATE;
#endif

    return 0;
}

/*!
 * \brief Set the specified chip select to a given level.
 *
 * \param cs Chip select number, 0 for MMC, 1 of DataFlash
 * \param hi Chip select line is activated if 0, deactivated if 1.
 *
 * \return 0 on success or -1 if the specified chip select number is not
 *         supported.
 */
static int NplSpiChipSelect(uint_fast8_t cs, uint_fast8_t hi)
{
    int rc = 0;

    switch (cs) {
    case 0:
        if (hi) {
            outb(NPL_XER, inb(NPL_XER) | NPL_MMCS);
        } else {
            outb(NPL_XER, inb(NPL_XER) & ~NPL_MMCS);
        }
        break;
    case 1:
        if (hi) {
            outb(NPL_XER, inb(NPL_XER) | NPL_NPCS0);
        } else {
            outb(NPL_XER, inb(NPL_XER) & ~NPL_NPCS0);
        }
        break;
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
int NplSpiBusSelect(NUTSPINODE * node, uint32_t tmo)
{
    int rc;

    /* Sanity check. */
    NUTASSERT(node != NULL);
    NUTASSERT(node->node_bus != NULL);

    /* Allocate the bus. */
    rc = NutEventWait(&node->node_bus->bus_mutex, tmo);
    if (rc) {
        errno = EIO;
    } else {
        /* If the mode update bit is set, then update our shadow registers. */
        if (node->node_mode & SPI_MODE_UPDATE) {
            NplSpiSetup(node);
        }

        /* Finally activate the node's chip select. */
        rc = NplSpiChipSelect(node->node_cs, 0);
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
int NplSpiBusDeselect(NUTSPINODE * node)
{
    /* Sanity check. */
    NUTASSERT(node != NULL);
    NUTASSERT(node->node_bus != NULL);

    /* Deactivate the node's chip select. */
    NplSpiChipSelect(node->node_cs, 1);

    /* Release the bus. */
    NutEventPost(&node->node_bus->bus_mutex);

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
int NplSpiBusNodeInit(NUTSPINODE * node)
{
    int rc;

    /* Sanity check. */
    NUTASSERT(node != NULL);

    /* Try to deactivate the node's chip select. */
    rc = NplSpiChipSelect(node->node_cs, 1);

    if (rc == 0) {
        NplSpiSetup(node);
    }
    return rc;
}

/*! 
 * \brief Transfer data on the SPI bus in polling mode.
 *
 * A device must have been selected by calling NplSpiSelect().
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
int NplSpiBusPollTransfer(NUTSPINODE * node, CONST void *txbuf, void *rxbuf, int xlen)
{
    uint8_t rxc;
    uint8_t txc = 0xFF;
    uint8_t *txp = (uint8_t *) txbuf;
    uint8_t *rxp = (uint8_t *) rxbuf;

    /* Sanity check. */
    NUTASSERT(node != NULL);

    while (xlen--) {
        if (txp) {
            txc = *txp++;
        }
        /* Transmission starts by writing the transmit data. */
        outb(NPL_MMCDR, txc);
        /* Wait for receiver data register full. */
        while ((inb(NPL_SLR) & NPL_MMCREADY) == 0);
        /* Read incoming data. */
        rxc = inb(NPL_MMCDR);
        if (rxp) {
            *rxp++ = rxc;
        }
    }
    return 0;
}

/*!
 * \brief SPI bus driver implementation structure.
 */
NUTSPIBUS spiBusNpl = {
    NULL,                   /*!< Bus mutex semaphore (bus_mutex). */
    NULL,                   /*!< Bus ready signal (bus_ready). */
    0,                      /*!< Bus base address (bus_base). */
    NULL,                   /*!< Bus interrupt handler (bus_sig). */
    NplSpiBusNodeInit,      /*!< Initialize the bus (bus_initnode). */
    NplSpiBusSelect,        /*!< Select the specified device (bus_alloc). */
    NplSpiBusDeselect,      /*!< Deselect the specified device (bus_release). */
    NplSpiBusPollTransfer,  /*!< Transfer data to and from a specified node (bus_transfer). */
    NutSpiBusWait,          /*!< Wait for bus transfer ready (bus_wait). */
    NutSpiBusSetMode,       /*!< Set SPI mode of a specified device (bus_set_mode). */
    NutSpiBusSetRate,       /*!< Set clock rate of a specified device (bus_set_rate). */
    NutSpiBusSetBits        /*!< Set number of data bits of a specified device (bus_set_bits). */
};
