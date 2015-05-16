/*
 * Copyright (C) 2006 by egnite Software GmbH. All rights reserved.
 * Copyright (C) 2008 by egnite GmbH. All rights reserved.
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
 *
 */


#include <cfg/os.h>
#include <cfg/memory.h>

#include <sys/timer.h>

#include <stdlib.h>

#include <dev/twif.h>
#include <dev/at24c.h>

#define AT24C_AT91

//#define AT24C_DEBUG

#ifdef AT24C_DEBUG
#include <stdio.h>
#endif

/*!
 * \brief Helper routine to write to EEPROM with ACK-Polling support
 *
 * \param at24cs Device descriptor.
 * \param buffer Buffer to transfer into EEPROM.
 * \param len    Number of bytes to write.
 * \param addr   Address in EEPROM to place data.
 *
 * \return 0 on success or -1 in case of an error, -2 in case of timeout.
 */
static int lld_at24_write( struct at24c *at24cs, uint8_t *buffer, uint32_t len, uint32_t addr)
{
	uint8_t retry = at24cs->Timeout;
	int tme;

	do {
		TwMasterRegWrite( at24cs->SlaveAddress, addr, at24cs->IAddrW, buffer, len, 500 );
		/* there was an error */
		tme = TwMasterError();
		if( tme == TWERR_OK)
			return 0;

#ifdef AT24C_DEBUG
		printf("MRW %lu: TME:%d t:%d\n", len, tme, TwMasterIndexes( 0));
#endif
		if( tme >= TWERR_SLA_NACK) {
			/* slave might be busy so we retry (ACK-Polling) */
			--retry;

#ifdef AT24C_DEBUG
			printf("RW %u\n", retry);
#else
			NutSleep(1);
#endif
		}
		else {
			/* it was something else than a NACK */
			return -1;
		}
	}
	while( retry);

	return -2;	// we get here if we ran out of delays for ACK of slave address
}

/*!
 * \brief Helper routine to read from EEPROM with ACK-Polling support
 *
 * \param at24cs Device descriptor.
 * \param buffer Buffer to transfer to from EEPROM.
 * \param len    Number of bytes to read.
 * \param addr   Address in EEPROM to read data from.
 *
 * \return 0 on success or -1 in case of an error, -2 in case of timeout.
 */
static int lld_at24_read( struct at24c *at24cs, uint8_t *buffer, uint32_t len, uint32_t addr)
{
	uint8_t retry = at24cs->Timeout;
	int tme;

	do {
		TwMasterRegRead( at24cs->SlaveAddress, addr, at24cs->IAddrW, buffer, len, 500 );
		tme = TwMasterError();
#ifdef AT24C_DEBUG
		printf("MRD l=%lu: TME %d, mt %u, mr %u\n", len, tme, TwMasterIndexes( 0), TwMasterIndexes(1));
#endif
        if( tme == TWERR_OK)
            return 0;		

        /* there was an error */
		if( tme == TWERR_SLA_NACK) {
			--retry;

#ifdef AT24C_DEBUG
			printf("RR %u\n", retry);
#else
			NutSleep(1);
#endif
		}
        else
            return -2;

	} while( retry);

	return -1;
}

/*!
 * \brief Read data from EEPROM with ACK-Polling support
 *
 * \param at24cs Device descriptor.
 * \param buffer Buffer to transfer to from EEPROM.
 * \param len    Number of bytes to read.
 * \param addr   Address in EEPROM where to read from.
 *
 * \return 0 on success or -1 in case of an error.
 */
/****************************************************************************/
int At24cRead( struct at24c *at24cs, uint8_t *buffer, uint32_t len, uint32_t addr )
/****************************************************************************/
{
	/* No, on read we definately do not have to wait for internal programming finished! */
	return lld_at24_read( at24cs, buffer, len, addr);
}

/*!
 * \brief Write data into eeprom memory.
 *
 * \param at24cs Device descriptor.
 * \param buffer Buffer to transfer to EEPROM.
 * \param len    Number of bytes to write.
 * \param addr   Address in EEPROM to place data.
 *
 * \return 0 on success or -1 in case of an error.
 */
/****************************************************************************/
int At24cWrite( struct at24c *at24cs, uint8_t *buffer, uint32_t len, uint32_t addr)
/****************************************************************************/
{
    int rc = 0;
	uint8_t *ptr = buffer;
	uint32_t bulk;
	/* get first bulk of data till page end */
	while( (len>0) && (rc>=0))
	{
		bulk = at24cs->PageSize-(addr%at24cs->PageSize);
		if( bulk > len) bulk = len;
		rc = lld_at24_write( at24cs, ptr, bulk, addr );
		ptr+=bulk; addr+=bulk; len-=bulk;
	}
	return rc;
}

