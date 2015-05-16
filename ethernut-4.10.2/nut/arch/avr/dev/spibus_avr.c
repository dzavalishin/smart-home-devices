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
 * \file arch/avr/dev/spibus_avr.c
 * \brief General AVR SPI bus driver rotuines.
 *
 * \verbatim
 * $Id: spibus_avr.c 2375 2009-01-09 17:54:38Z haraldkipp $
 * \endverbatim
 */

#include <sys/timer.h>

#include <dev/spibus_avr.h>

/*!
 * \brief Update SPI shadow registers.
 */
int AvrSpiSetup(NUTSPINODE * node)
{
    uint_fast8_t i;
    uint32_t clk;
    AVRSPIREG *spireg;

    spireg = node->node_stat;

    /* Select SPI master mode. */
    spireg->avrspi_spcr = _BV(SPE) | _BV(MSTR) | (uint8_t)(node->node_mode & SPI_MODE_3) << 2;

    /* Find the frequency that is below or equal the requested 
       ** one, using the double speed bit if available. */
    clk = NutClockGet(NUT_HWCLK_PERIPHERAL);
#if defined(SPI2X)
    for (i = 0; i < 7; i++) {
        clk >>= 1;
        if (clk <= node->node_rate) {
            break;
        }
    }
    spireg->avrspi_spcr |= (i >> 1);
    if (i < 6) {
        spireg->avrspi_spsr = ~i & _BV(SPI2X);
    }
#else
    for (i = 0; i < 3; i++) {
        clk >>= 2;
        if (clk <= node->node_rate) {
            break;
        }
    }
    spireg->avrspi_spcr |= i;
#endif

    /* Update interface parameters. */
    node->node_rate = clk;
    node->node_mode &= ~SPI_MODE_UPDATE;

    return 0;
}
