#ifndef _DEV_SPI_NODE_AT45D_H_
#define _DEV_SPI_NODE_AT45D_H_
/*
 * Copyright (C) 2008-2011 by egnite GmbH
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
 * \file include/dev/spi_node_at45d.h
 * \brief Low level access for Atmel AT45D SPI Flash.
 *
 * \verbatim
 * $Id$
 * \endverbatim
 */

#include <dev/spibus.h>
#include <dev/serialflash.h>
#include <dev/at45d.h>

extern NUTSPINODE nodeAt45d0;
extern NUTSPINODE nodeAt45d1;
extern NUTSPINODE nodeAt45d2;
extern NUTSPINODE nodeAt45d3;

extern NUTSERIALFLASH flashAt45d0;
extern NUTSERIALFLASH flashAt45d1;
extern NUTSERIALFLASH flashAt45d2;
extern NUTSERIALFLASH flashAt45d3;

extern NUTDEVICE devSpiBlkAt45d0;
extern NUTDEVICE devSpiBlkAt45d1;
extern NUTDEVICE devSpiBlkAt45d2;
extern NUTDEVICE devSpiBlkAt45d3;

__BEGIN_DECLS
/* Prototypes */
extern AT45D_INFO *At45dNodeProbe(NUTSPINODE *node);
extern int At45dNodeLock(NUTSPINODE * node);
extern void At45dNodeUnlock(NUTSPINODE * node);
extern int At45dNodeTransfer(NUTSPINODE * node, uint8_t op, uint32_t parm, uint_fast8_t oplen, CONST void *txbuf, void *rxbuf, int xlen);
extern int At45dNodeCommand(NUTSPINODE * node, uint8_t op, uint32_t parm, uint_fast8_t oplen);
extern int At45dNodeWaitReady(NUTSPINODE * node, uint32_t tmo, int poll);
extern uint8_t At45dNodeStatus(NUTSPINODE * node);
/* Prototypes */
__END_DECLS

#endif
