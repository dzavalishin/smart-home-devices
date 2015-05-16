/*
 * Copyright (C) 2006 by egnite Software GmbH. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY EGNITE SOFTWARE GMBH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL EGNITE
 * SOFTWARE GMBH OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
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

/*
 * $Log$
 * Revision 1.1  2006/08/31 18:58:47  haraldkipp
 * More general AT91 MAC driver replaces the SAM7X specific version.
 * This had been tested on the SAM9260, but loses Ethernet packets
 * for a yet unknown reason.
 *
 * Revision 1.1  2006/07/05 07:38:44  haraldkipp
 * New Ethernet driver for the AT91SAM7X EMAC and the Davicom DM9161A.
 * This driver is not yet finished. Ethernet link auto-negotiation works
 * and receive interrupts are generated when sending packets to the
 * board. But transmitting packets fails, nothing is sent out.
 *
 */

#ifndef _DEV_AT91SAM7X_EMAC_H_
#define _DEV_AT91SAM7X_EMAC_H_

#include <sys/device.h>
#include <net/if_var.h>

/*
 * Available drivers.
 */
extern NUTDEVICE devAt91Emac;

#ifndef DEV_ETHER
#define DEV_ETHER   devAt91Emac
#endif

#ifndef devEth0
#define devEth0   devAt91Emac
#endif

#endif
