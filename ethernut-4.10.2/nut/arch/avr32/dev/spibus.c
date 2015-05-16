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
 * \file arch/avr32/dev/spibus.c
 * \brief General AVR32 SPI bus controller routines.
 *
 * \verbatim
 * $Id: spibus.c,v 1.3 2009/02/13 14:52:05 haraldkipp Exp $
 * \endverbatim
 */

#include <cfg/spi.h>

#include <sys/timer.h>
#include <sys/nutdebug.h>

#include <stdlib.h>
#include <memdebug.h>

#include <dev/spibus_avr32.h>

#include <avr32/io.h>

#if defined(SPIBUS0_DOUBLE_BUFFER) || defined(SPIBUS1_DOUBLE_BUFFER)
#warning Double buffered SPI not tested for AVR32 targets.
/*!
 * \brief AVR32 SPI interrupt handler.
 */
static void Avr32SpiInterrupt(void *arg)
{
    NutEventPostFromIrq((void **) arg);
}
#endif                          /* SPIBUS0_DOUBLE_BUFFER */

/*!
 * \brief Update SPI shadow registers.
 *
 * \param node Specifies the SPI bus node.
 *
 * \return Always 0.
 */
int Avr32SpiSetup(NUTSPINODE * node)
{
    uint32_t clk;
    uint32_t clkdiv;
    AVR32SPIREG *spireg;

    NUTASSERT(node != NULL);
    NUTASSERT(node->node_stat != NULL);
    NUTASSERT(node->node_bus != NULL);
    NUTASSERT(node->node_bus->bus_base != 0);
    spireg = node->node_stat;

    spireg->mr &= ~(AVR32_SPI_MR_MODFDIS_MASK | AVR32_SPI_MR_LLB_MASK | AVR32_SPI_MR_PCSDEC_MASK);

    if ((node->node_mode & SPI_MODE_FAULT) == 0) {
        spireg->mr |= AVR32_SPI_MR_MODFDIS_MASK;
    }
    if (node->node_mode & SPI_MODE_LOOPBACK) {
        spireg->mr |= AVR32_SPI_MR_LLB_MASK;
    }

    spireg->mr |= 0xe0 << AVR32_SPI_PCS_OFFSET;

    spireg->csr &=
        ~(AVR32_SPI_CSR0_BITS_MASK | AVR32_SPI_CSR0_CPOL_MASK | AVR32_SPI_CSR0_NCPHA_MASK | AVR32_SPI_CSR0_CSAAT_MASK |
          AVR32_SPI_CSR0_SCBR_MASK);
    if (node->node_bits) {
        spireg->csr |= ((uint32_t) (node->node_bits - 8) << AVR32_SPI_CSR0_BITS_OFFSET) & AVR32_SPI_CSR0_BITS_MASK;
    }
    if (node->node_mode & SPI_MODE_CPOL) {
        spireg->csr |= AVR32_SPI_CSR0_CPOL_MASK;
    }
    if ((node->node_mode & SPI_MODE_CPHA) == 0) {
        spireg->csr |= AVR32_SPI_CSR0_NCPHA_MASK;
    }
    if (node->node_mode & SPI_MODE_CSKEEP) {
        spireg->csr |= AVR32_SPI_CSR0_CSAAT_MASK;
    }

    /* Query peripheral clock. */
    clk = NutClockGet(NUT_HWCLK_PERIPHERAL_A);
    /* Calculate the SPI clock divider. Avoid rounding errors. */
    clkdiv = (clk + node->node_rate - 1) / node->node_rate;
    /* The divider value minimum is 1. */
    if (clkdiv < 1) {
        clkdiv++;
    }
    /* The divider value maximum is 255. */
    else if (clkdiv > 255) {
        clkdiv = 255;
    }
    spireg->csr |= clkdiv << AVR32_SPI_CSR0_SCBR_OFFSET;

    /* Update interface parameters. */
    node->node_rate = clk / clkdiv;
    node->node_mode &= ~SPI_MODE_UPDATE;

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
int Avr32SpiBusNodeInit(NUTSPINODE * node)
{
    int rc = 0;
    NUTSPIBUS *bus;

    /* Sanity check. */
    NUTASSERT(node != NULL);
    NUTASSERT(node->node_bus != NULL);
    bus = node->node_bus;

    /* Try to deactivate the node's chip select. */
#if defined(AVR32_SPI1_ADDRESS)
    if (bus->bus_base == AVR32_SPI1_ADDRESS) {
        rc = Avr32Spi1ChipSelect(node->node_cs, (node->node_mode & SPI_MODE_CSHIGH) == 0);
    } else
#endif
        rc = Avr32Spi0ChipSelect(node->node_cs, (node->node_mode & SPI_MODE_CSHIGH) == 0);

    /* It should not hurt us being called more than once. Thus, we
     ** check whether any initialization had been taken place already. */
    if (rc == 0 && node->node_stat == NULL) {
        /* Allocate and set our shadow registers. */
        AVR32SPIREG *spireg = malloc(sizeof(AVR32SPIREG));
        if (spireg) {
            /* Set interface defaults. */
            spireg->mr = AVR32_SPI_MR_MODFDIS_MASK | AVR32_SPI_MR_MSTR_MASK;
            spireg->mr |= (1 << (AVR32_SPI_MR_PCS_OFFSET + node->node_cs));
            spireg->csr = 0;
            /* Update with node's defaults. */
            node->node_stat = (void *) spireg;
            Avr32SpiSetup(node);

            /* 
             * Register and enable SPI interrupt handler. 
             */
#if !defined(SPIBUS1_POLLING_MODE) && defined(AVR32_SPI1_ADDRESS)
            if (bus->bus_base == AVR32_SPI1_ADDRESS) {
#if defined(SPIBUS1_DOUBLE_BUFFER)
                NutRegisterIrqHandler(bus->bus_sig, Avr32SpiInterrupt, &bus->bus_ready);
#else
                NutRegisterIrqHandler(bus->bus_sig, Avr32SpiBus1Interrupt, &bus->bus_ready);
#endif
                outr(bus->bus_base + AVR32_SPI_IDR, (unsigned int) -1);
                NutIrqEnable(bus->bus_sig);
            } else
#endif                          /* !SPIBUS1_POLLING_MODE */

            {
#if !defined(SPIBUS0_POLLING_MODE)
#if defined(SPIBUS0_DOUBLE_BUFFER)
                NutRegisterIrqHandler(bus->bus_sig, Avr32SpiInterrupt, &bus->bus_ready);
#else
                NutRegisterIrqHandler(bus->bus_sig, Avr32SpiBus0Interrupt, &bus->bus_ready);
#endif
                outr(bus->bus_base + AVR32_SPI_IDR, (unsigned int) -1);
                NutIrqEnable(bus->bus_sig);
#endif                          /* !SPIBUS0_POLLING_MODE */
            }
        } else {
            /* Out of memory? */
            rc = -1;
        }
    }
    return rc;
}

#if defined(SPIBUS0_POLLING_MODE) || defined(SPIBUS1_POLLING_MODE)
/*! 
 * \brief Transfer data on the SPI bus in polling mode.
 *
 * A device must have been selected by calling At91SpiSelect().
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
int Avr32SpiBusPollTransfer(NUTSPINODE * node, CONST void *txbuf, void *rxbuf, int xlen)
{
    uint8_t b = 0xff;
    uint8_t *txp = (uint8_t *) txbuf;
    uint8_t *rxp = (uint8_t *) rxbuf;
    uintptr_t base;

    /* Sanity check. */
    NUTASSERT(node != NULL);
    NUTASSERT(node->node_bus != NULL);
    NUTASSERT(node->node_bus->bus_base != 0);
    base = node->node_bus->bus_base;

    while (xlen--) {
        if (txp) {
            b = *txp++;
        }
        /* Transmission starts by writing the transmit data. */
        outr(base + AVR32_SPI_TDR, (b << AVR32_SPI_TDR_TD_OFFSET));
        /* Wait for receiver data register full. */
        while ((inr(base + AVR32_SPI_SR) & AVR32_SPI_RDRF_MASK) == 0);
        /* Read incoming data. */
        b = (uint8_t) inr(base + AVR32_SPI_RDR) >> AVR32_SPI_RDR_RD_OFFSET;
        if (rxp) {
            *rxp++ = b;
        }
    }
    return 0;
}
#endif

#if defined(SPIBUS0_DOUBLE_BUFFER) || defined(SPIBUS1_DOUBLE_BUFFER)
/*! 
 * \brief Transfer data on the SPI bus using double buffered PDC.
 *
 * A device must have been selected by calling Avr32SpiSelect().
 *
 * Note, that the transfer may be still in progress when returning 
 * from this function.
 *
 * \todo Not yet done. Given up after SAM7SE SDRAM problems.
 *
 * \param node Specifies the SPI bus node.
 * \param txbuf Pointer to the transmit buffer. If NULL, undetermined
 *              byte values are transmitted.
 * \param rxbuf Pointer to the receive buffer. If NULL, then incoming
 *              data is discarded.
 * \param xlen  Number of bytes to transfer.
 *
 * \return Always -1.
 */
int Avr32SpiBusDblBufTransfer(NUTSPINODE * node, CONST void *txbuf, void *rxbuf, int xlen)
{
#error not implemented
    return -1;
}
#endif

#if !defined(SPIBUS0_DOUBLE_BUFFER) || !defined(SPIBUS1_DOUBLE_BUFFER)
/*! 
 * \brief Wait until all SPI bus transfers are done.
 *
 * \param node Specifies the SPI bus node.
 * \param tmo  Timeout in milliseconds. To disable timeout, set this
 *             parameter to NUT_WAIT_INFINITE.
 *
 * \return Always 0.
 */
int Avr32SpiBusWait(NUTSPINODE * node, uint32_t tmo)
{
    while ((inr(node->node_bus->bus_base + AVR32_SPI_SR) & AVR32_SPI_SR_RXBUFF) == 0) {
        if (NutEventWait(&node->node_bus->bus_ready, tmo)) {
            return -1;
        }
    }
    return 0;
}
#endif                          /* SPIBUS_POLLING_MODE */
