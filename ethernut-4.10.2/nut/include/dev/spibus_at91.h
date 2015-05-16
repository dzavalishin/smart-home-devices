#ifndef _DEV_SPIBUS_AT91_H_
#define _DEV_SPIBUS_AT91_H_
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
 * \file dev/spibus_at91.h
 * \brief AT91 specific SPI bus declarations.
 *
 * \verbatim
 * $Id: spibus_at91.h 2614 2009-04-15 13:53:30Z haraldkipp $
 * \endverbatim
 */

#include <dev/spibus.h>

typedef struct _AT91SPIREG AT91SPIREG;

struct _AT91SPIREG {
    uint32_t at91spi_mr;
    uint32_t at91spi_csr;
};

extern NUTSPIBUS spiBus0At91;
extern NUTSPIBUS spiBus1At91;

__BEGIN_DECLS
/* Prototypes */
extern int At91Spi0ChipSelect(uint_fast8_t cs, uint_fast8_t hi);
extern void At91SpiBus0Interrupt(void *arg);

extern int At91Spi1ChipSelect(uint_fast8_t cs, uint_fast8_t hi);
extern void At91SpiBus1Interrupt(void *arg);

extern int At91SpiSetup(NUTSPINODE * node);
extern int At91SpiBusNodeInit(NUTSPINODE * node);
extern int At91SpiBusTransfer(NUTSPINODE * node, CONST void *txbuf, void *rxbuf, int xlen);
extern int At91SpiBusDblBufTransfer(NUTSPINODE * node, CONST void *txbuf, void *rxbuf, int xlen);
extern int At91SpiBusPollTransfer(NUTSPINODE * node, CONST void *txbuf, void *rxbuf, int xlen);
extern int At91SpiBusWait(NUTSPINODE * node, uint32_t tmo);
/* Prototypes */
__END_DECLS
#endif
