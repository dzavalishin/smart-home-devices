#ifndef _DEV_SPIBUS_AVR32_H_
#define _DEV_SPIBUS_AVR32_H_
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
 * \file dev/spibus_avr32.h
 * \brief AVR32 specific SPI bus declarations.
 *
 * \verbatim
 * $Id: spibus_avr32.h,v 1.1 2009/01/09 17:54:38 haraldkipp Exp $
 * \endverbatim
 */

#include <dev/spibus.h>

typedef struct _AVR32SPIREG AVR32SPIREG;

struct _AVR32SPIREG {
	ureg_t mr;
	ureg_t csr;
};

extern NUTSPIBUS spiBus0Avr32;
extern NUTSPIBUS spiBus1Avr32;

__BEGIN_DECLS
/* Prototypes */
extern int Avr32Spi0ChipSelect(uint_fast8_t cs, uint_fast8_t hi);
extern void Avr32SpiBus0Interrupt(void *arg);

extern int Avr32Spi1ChipSelect(uint_fast8_t cs, uint_fast8_t hi);
extern void Avr32SpiBus1Interrupt(void *arg);

extern int Avr32SpiSetup(NUTSPINODE * node);
extern int Avr32SpiBusNodeInit(NUTSPINODE * node);
extern int Avr32SpiBusTransfer(NUTSPINODE * node, CONST void *txbuf, void *rxbuf, int xlen);
extern int Avr32SpiBusDblBufTransfer(NUTSPINODE * node, CONST void *txbuf, void *rxbuf, int xlen);
extern int Avr32SpiBusPollTransfer(NUTSPINODE * node, CONST void *txbuf, void *rxbuf, int xlen);
extern int Avr32SpiBusWait(NUTSPINODE * node, uint32_t tmo);
/* Prototypes */
__END_DECLS
#endif
