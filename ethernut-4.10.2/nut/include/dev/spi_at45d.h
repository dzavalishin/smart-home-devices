#ifndef _DEV_SPI_AT45D_H_
#define _DEV_SPI_AT45D_H_
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
 * \file include/dev/spi_at45d.h
 * \brief AT45D
 *
 * \verbatim
 * $Id: spi_at45d.h 2610 2009-04-15 13:45:19Z haraldkipp $
 * \endverbatim
 */

#include <sys/device.h>
#include <dev/spibus.h>

extern NUTSPINODE nodeSpiAt45d0;
extern NUTDEVICE devSpiAt45d0;

extern NUTSPINODE nodeSpiAt45d1;
extern NUTDEVICE devSpiAt45d1;

extern NUTSPINODE nodeSpiAt45d2;
extern NUTDEVICE devSpiAt45d2;

extern NUTSPINODE nodeSpiAt45d3;
extern NUTDEVICE devSpiAt45d3;

__BEGIN_DECLS
/* Prototypes */
extern int SpiAt45dInit(NUTDEVICE *dev);
extern int SpiAt45dCommand(NUTDEVICE *dev, uint8_t op, uint32_t parm, int len, CONST void *tdata, void *rdata, int datalen);
extern uint8_t SpiAt45dStatus(NUTDEVICE *dev);
extern int SpiAt45dWaitReady(NUTDEVICE *dev, uint32_t tmo, int poll);
extern int SpiAt45dPageErase(NUTDEVICE *dev, uint32_t pgn);
extern int SpiAt45dChipErase(NUTDEVICE *dev);
extern int SpiAt45dPageRead(NUTDEVICE *dev, uint32_t pgn, void *data, int len);
extern int SpiAt45dPageWrite(NUTDEVICE *dev, uint32_t pgn, CONST void *data, int len);
extern uint32_t SpiAt45dPages(NUTDEVICE *dev);
extern int SpiAt45dPageSize(NUTDEVICE *dev);
#ifdef __HARVARD_ARCH__
extern int SpiAt45dPageWrite_P(NUTDEVICE *dev, uint32_t pgn, PGM_P data, int len);
#endif
extern int SpiAt45dIOCtl(NUTDEVICE * dev, int req, void *conf);
/* Prototypes */
__END_DECLS

#endif
