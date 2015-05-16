#ifndef _CFG_ARCH_AVR_H_
#define _CFG_ARCH_AVR_H_

/*
 * Copyright (C) 2004 by egnite Software GmbH. All rights reserved.
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
 *
 */

/*
 * $Log$
 * Revision 1.8  2008/10/23 08:53:01  haraldkipp
 * Adding new port identifiers.
 *
 * Revision 1.7  2008/01/31 09:32:16  haraldkipp
 * Added ports H to L.
 *
 * Revision 1.6  2007/04/12 09:10:29  haraldkipp
 * PORTH added.
 *
 * Revision 1.5  2005/08/02 17:46:48  haraldkipp
 * Major API documentation update.
 *
 * Revision 1.4  2005/02/02 20:03:46  haraldkipp
 * All definitions had been moved to avrpio.h in order to fix the broken
 * port I/O without being forced to change other existing modules.
 *
 * Revision 1.3  2005/01/22 19:27:19  haraldkipp
 * Changed AVR port configuration names from PORTx to AVRPORTx.
 *
 * Revision 1.2  2004/09/22 08:18:57  haraldkipp
 * More configurable ports
 *
 * Revision 1.1  2004/08/25 10:58:02  haraldkipp
 * New include directory cfg/arch added, which is used for target specific items,
 * mainly port usage or MCU specific register settings.
 *
 */

/*!
 * \addtogroup xgConfigAvr
 */
/*@{*/

/*!
 * \file include/cfg/arch/avr.h
 * \brief AVR hardware configuration.
 */

#define PIOA_ID     1
#define PIOB_ID     2
#define PIOC_ID     3
#define PIOD_ID     4
#define PIOE_ID     5
#define PIOF_ID     6
#define PIOG_ID     7
#define PIOH_ID     8
#define PIOI_ID     9
#define PIOJ_ID     10
#define PIOK_ID     11
#define PIOL_ID     12

#define AVRPORTA    PIOA_ID
#define AVRPORTB    PIOB_ID
#define AVRPORTC    PIOC_ID
#define AVRPORTD    PIOD_ID
#define AVRPORTE    PIOE_ID
#define AVRPORTF    PIOF_ID
#define AVRPORTG    PIOG_ID
#define AVRPORTH    PIOH_ID
#define AVRPORTI    PIOI_ID
#define AVRPORTJ    PIOJ_ID
#define AVRPORTK    PIOK_ID
#define AVRPORTL    PIOL_ID

#include <cfg/arch/avrpio.h>

/*@}*/

#endif
