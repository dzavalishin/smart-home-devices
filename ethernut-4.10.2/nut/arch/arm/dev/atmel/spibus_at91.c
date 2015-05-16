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
 * \file arch/arm/dev/spibus_at91.c
 * \brief General AT91 SPI bus controller routines.
 *
 * \verbatim
 * $Id: spibus_at91.c 3449 2011-05-31 19:08:15Z mifi $
 * \endverbatim
 */

#include <cfg/spi.h>

#include <sys/timer.h>
#include <sys/nutdebug.h>

#include <stdlib.h>
#include <memdebug.h>

#include <dev/spibus_at91.h>

#define SPI_DOUBLE_BUFFER_MIN_TRANSFER_SIZE 4

#if defined(SPIBUS0_DOUBLE_BUFFER) || defined(SPIBUS1_DOUBLE_BUFFER)

/*!
 * \brief AT91 SPI interrupt handler.
 */
static void At91SpiInterrupt(void *arg)
{
    NutEventPostFromIrq((void **)arg);
}
#endif /* SPIBUS0_DOUBLE_BUFFER */

/*!
 * \brief Update SPI shadow registers.
 *
 * \param node Specifies the SPI bus node.
 *
 * \return Always 0.
 */
int At91SpiSetup(NUTSPINODE * node)
{
    uint32_t clk;
    uint32_t clkdiv;
    AT91SPIREG *spireg;

    NUTASSERT(node != NULL);
    NUTASSERT(node->node_stat != NULL);
    NUTASSERT(node->node_bus != NULL);
    NUTASSERT(node->node_bus->bus_base != 0);
    spireg = node->node_stat;

    spireg->at91spi_mr &= ~(SPI_MODFDIS | SPI_LLB);
    if ((node->node_mode & SPI_MODE_FAULT) == 0) {
        spireg->at91spi_mr |= SPI_MODFDIS;
    }
    if (node->node_mode & SPI_MODE_LOOPBACK) {
        spireg->at91spi_mr |= SPI_LLB;
    }

    spireg->at91spi_csr &= ~(SPI_BITS | SPI_CPOL | SPI_NCPHA | SPI_CSAAT | SPI_SCBR);
    if (node->node_bits) {
        spireg->at91spi_csr |= ((uint32_t)(node->node_bits - 8) << SPI_BITS_LSB) & SPI_BITS;
    }
    if (node->node_mode & SPI_MODE_CPOL) {
        spireg->at91spi_csr |= SPI_CPOL;
    }
    if ((node->node_mode & SPI_MODE_CPHA) == 0) {
        spireg->at91spi_csr |= SPI_NCPHA;
    }
    if (node->node_mode & SPI_MODE_CSKEEP) {
        spireg->at91spi_csr |= SPI_CSAAT;
    }

    /* Query peripheral clock. */
    clk = NutClockGet(NUT_HWCLK_PERIPHERAL);
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
    spireg->at91spi_csr |= clkdiv << SPI_SCBR_LSB;

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
int At91SpiBusNodeInit(NUTSPINODE * node)
{
    int rc;
    NUTSPIBUS *bus;

    /* Sanity check. */
    NUTASSERT(node != NULL);
    NUTASSERT(node->node_bus != NULL);
    bus = node->node_bus;

    /* Try to deactivate the node's chip select. */
#if defined(SPI1_BASE)
    if (bus->bus_base == SPI1_BASE) {
        rc = At91Spi1ChipSelect(node->node_cs, (node->node_mode & SPI_MODE_CSHIGH) == 0);
    } else
#endif
    rc = At91Spi0ChipSelect(node->node_cs, (node->node_mode & SPI_MODE_CSHIGH) == 0);

    /* It should not hurt us being called more than once. Thus, we
       ** check wether any initialization had been taken place already. */
    if (rc == 0 && node->node_stat == NULL) {
        /* Allocate and set our shadow registers. */
        AT91SPIREG *spireg = malloc(sizeof(AT91SPIREG));
        if (spireg) {
            /* Set interface defaults. */
            spireg->at91spi_mr = SPI_MODFDIS | SPI_MSTR;
            switch (node->node_cs) {
            case 0:
                spireg->at91spi_mr |= SPI_PCS_0;
                break;
            case 1:
                spireg->at91spi_mr |= SPI_PCS_1;
                break;
            case 2:
                spireg->at91spi_mr |= SPI_PCS_2;
                break;
            case 3:
                spireg->at91spi_mr |= SPI_PCS_3;
                break;
            }
            spireg->at91spi_csr = 0;
            /* Update with node's defaults. */
            node->node_stat = (void *)spireg;
            At91SpiSetup(node);

            /* 
             * Register and enable SPI interrupt handler. 
             */
#if !defined(SPIBUS1_POLLING_MODE) && defined(SPI1_BASE)
            if (bus->bus_base == SPI1_BASE) {
#if defined(SPIBUS1_DOUBLE_BUFFER)
                NutRegisterIrqHandler(bus->bus_sig, At91SpiInterrupt, &bus->bus_ready);
#else
                NutRegisterIrqHandler(bus->bus_sig, At91SpiBus1Interrupt, &bus->bus_ready);
#endif
                outr(bus->bus_base + SPI_IDR_OFF, (unsigned int) - 1);
                NutIrqEnable(bus->bus_sig);
            } else
#endif /* !SPIBUS1_POLLING_MODE */

            {
#if !defined(SPIBUS0_POLLING_MODE)
#if defined(SPIBUS0_DOUBLE_BUFFER)
                NutRegisterIrqHandler(bus->bus_sig, At91SpiInterrupt, &bus->bus_ready);
#else
                NutRegisterIrqHandler(bus->bus_sig, At91SpiBus0Interrupt, &bus->bus_ready);
#endif
                outr(bus->bus_base + SPI_IDR_OFF, (unsigned int) - 1);
                NutIrqEnable(bus->bus_sig);
#endif /* !SPIBUS0_POLLING_MODE */
            }
        } else {
            /* Out of memory? */
            rc = -1;
        }
    }
    return rc;
}

#if defined(SPIBUS0_POLLING_MODE) || defined(SPIBUS1_POLLING_MODE) || defined(SPIBUS0_DOUBLE_BUFFER_HEURISTIC) || defined(SPIBUS1_DOUBLE_BUFFER_HEURISTIC)
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
int At91SpiBusPollTransfer(NUTSPINODE * node, CONST void *txbuf, void *rxbuf, int xlen)
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
        outr(base + SPI_TDR_OFF, b);
        /* Wait for receiver data register full. */
        while((inr(base + SPI_SR_OFF) & SPI_RDRF) == 0);
        /* Read incoming data. */
        b = (uint8_t)inr(base + SPI_RDR_OFF);
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
 * A device must have been selected by calling At91SpiSelect().
 *
 * \todo Not yet done. Given up after SAM7SE SDRAM problems.
 *       Currently working fine on SAM7X platform
 * \todo Is this working asynchronously? Old comments mentioned that 
 *       the transfer might be still active when function returns.
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
int At91SpiBusDblBufTransfer(NUTSPINODE * node, CONST void *txbuf, void *rxbuf, int xlen)
{
    uintptr_t base;
    uint32_t cr;
    uint32_t ir = 0;

#if defined(SPIBUS0_DOUBLE_BUFFER_HEURISTIC) || defined(SPIBUS1_DOUBLE_BUFFER_HEURISTIC)
    if (xlen < SPI_DOUBLE_BUFFER_MIN_TRANSFER_SIZE) {
        return At91SpiBusPollTransfer(node, txbuf, rxbuf, xlen);
    }
#endif

    /* Sanity check. */
    NUTASSERT(node != NULL);
    NUTASSERT(node->node_bus != NULL);
    NUTASSERT(node->node_bus->bus_base != 0);
    base = node->node_bus->bus_base;

    outr(base + PERIPH_PTCR_OFF, PDC_TXTDIS | PDC_RXTDIS);

    if (xlen) {
        /* Set first transmit pointer and counter. */
        if (txbuf) {
            outr(base + PERIPH_TPR_OFF, (uint32_t)txbuf);
        } else {
            outr(base + PERIPH_TPR_OFF, (uint32_t)rxbuf);
        }
        cr = PDC_TXTEN;
        outr(base + PERIPH_TCR_OFF, xlen);
        /* Set first receive pointer and counter. */
        if (rxbuf) {
            outr(base + PERIPH_RPR_OFF, (uint32_t)rxbuf);
            outr(base + PERIPH_RCR_OFF, xlen);
            cr |= PDC_RXTEN;
            ir = SPI_RXBUFF;
        } else {
            cr |= PDC_RXTDIS;
            ir = SPI_TXBUFE;
            outr(base + PERIPH_RPR_OFF, 0);
            outr(base + PERIPH_RCR_OFF, 0);
        }
        outr(base + PERIPH_TNPR_OFF, 0);
        outr(base + PERIPH_TNCR_OFF, 0);
        outr(base + PERIPH_RNPR_OFF, 0);
        outr(base + PERIPH_RNCR_OFF, 0);

        outr(base + SPI_IDR_OFF, (unsigned int) - 1);
        outr(base + SPI_IER_OFF, ir);
        outr(base + PERIPH_PTCR_OFF, cr);
        
        NutEventWait(&node->node_bus->bus_ready, NUT_WAIT_INFINITE);
        outr(base + PERIPH_PTCR_OFF, PDC_TXTDIS | PDC_RXTDIS);
    }
    return 0;
}
#endif

#if defined(SPIBUS0_DOUBLE_BUFFER) || defined(SPIBUS1_DOUBLE_BUFFER)
/*! 
 * \brief Wait until all SPI bus transfers are done.
 *
 * \param node Specifies the SPI bus node.
 * \param tmo  Timeout in milliseconds. To disable timeout, set this
 *             parameter to NUT_WAIT_INFINITE.
 *
 * \return Always 0.
 */
int At91SpiBusWait(NUTSPINODE * node, uint32_t tmo)
{
    while ((inr(node->node_bus->bus_base + SPI_SR_OFF) & SPI_RXBUFF) == 0) {
        if (NutEventWait(&node->node_bus->bus_ready, tmo)) {
            return -1;
        }
    }
    return 0;
}
#endif /* SPIBUS_POLLING_MODE */
