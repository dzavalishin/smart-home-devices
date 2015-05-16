#ifndef _DEV_TWIF_H_
#define _DEV_TWIF_H_

/*
 * Copyright (C) 2001-2005 by egnite Software GmbH. All rights reserved.
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
 * Revision 1.3  2008/08/11 06:59:59  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.2  2005/10/24 10:56:30  haraldkipp
 * Added const modifier to transmit data pointer in TwMasterTransact().
 *
 * Revision 1.1.1.1  2003/05/09 14:41:09  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.2  2003/03/31 14:53:23  harald
 * Prepare release 3.1
 *
 */

#include <sys/types.h>
#include <stdint.h>

#define TWI_SETSPEED		0x0401	/*!< \brief Set transfer speed. */
#define TWI_GETSPEED		0x0402	/*!< \brief Query transfer speed. */
#define TWI_SETSLAVEADDRESS	0x0403	/*!< \brief Set local slave address. */
#define TWI_GETSLAVEADDRESS	0x0404	/*!< \brief Query local slave address. */
#define TWI_SETSTATUS		0x0409	/*!< \brief Set status. */
#define TWI_GETSTATUS		0x040a	/*!< \brief Query status. */


#define TWERR_OK		0	/*!< \brief No error occured. */
#define TWERR_TIMEOUT		1	/*!< \brief Interface timeout. */
#define TWERR_BUS		2	/*!< \brief Bus error. */
#define TWERR_IF_LOCKED		3	/*!< \brief Interface locked. */
#define TWERR_SLA_NACK		4	/*!< \brief No slave response. */
#define TWERR_DATA_NACK		5	/*!< \brief Data not acknowledged. */


#define TWSLA_MIN		17	/*!< \brief Lowest slave address.
					 * Addresses below are reserved
					 * for special purposes.
					 */
#define TWSLA_MAX		79	/*!< \brief Lowest slave address.
					 * Addresses above are reserved
					 * for special purposes.
					 */
#define TWSLA_BCAST		0	/*!< \brief Broadcast slave address. */
#define TWSLA_HOST		16	/*!< \brief Host slave address. */
#define TWSLA_DEFAULT		193	/*!< \brief Default slave address. */

extern int TwInit(uint8_t sla);
extern int TwIOCtl(int req, void *conf);

extern int TwMasterTransact(uint8_t sla, CONST void *txdata, uint16_t txlen, void *rxdata, uint16_t rxsiz, uint32_t tmo);
//extern int TwMasterRegRead(uint8_t sla, uint32_t iadr, uint8_t iadrlen, void *rxdata, uint8_t rxsiz, uint32_t tmo);
extern int TwMasterRegRead(uint8_t sla, uint32_t iadr, uint8_t iadrlen, void *rxdata, uint16_t rxsiz, uint32_t tmo);
//extern int TwMasterRegWrite(uint8_t sla, uint32_t iadr, uint8_t iadrlen, void *txdata, uint8_t txsiz, uint32_t tmo);
extern int TwMasterRegWrite(uint8_t sla, uint32_t iadr, uint8_t iadrlen, CONST void *txdata, uint16_t txsiz, uint32_t tmo);
extern int TwMasterError(void);
extern uint16_t TwMasterIndexes( uint8_t idx);

extern int TwSlaveListen(uint8_t *sla, void *rxdata, uint16_t rxsiz, uint32_t tmo);
extern int TwSlaveRespond(void *txdata, uint16_t txlen, uint32_t tmo);
extern int TwSlaveError(void);


#endif
