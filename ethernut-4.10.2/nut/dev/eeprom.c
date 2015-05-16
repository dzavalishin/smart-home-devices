/*
 * Copyright (C) 2009 by Rittal GmbH & Co. KG,
 * Ulrich Prinz <prinz.u@rittal.de> All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY EMBEDDED IT AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL EMBEDDED IT
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * For additional information see http://www.ethernut.de/
 *
 */

/*
 * $Log$
 *
 * Revision 1.0  2009/04/13 ulrichprinz
 * First checkin, abstraction interface for EEPROM chips (currently SAM7X256 is 
 * tested only)
 *
 */
#ifndef _DEV_EEPROM_H_
#define _DEV_EEPROM_H_

#include <stdint.h>
#include <dev/twif.h>
#include <dev/at24c.h>
#include <dev/eeprom.h>

static struct at24c at24c32s;

/****************************************************************************/
int EEInit( void )
/****************************************************************************/
{
    uint8_t dummy;
    at24c32s.PageSize = 32;
    at24c32s.NumOfPage = 128;
    at24c32s.EepromSize = 32*128;
    at24c32s.SlaveAddress = NUT_CONFIG_AT24_ADR;
	at24c32s.IAddrW = 2;
	at24c32s.Timeout = 20;
    
    //strcpy (at24c32s.EepromName, "AT24C32" );
    
    /* Do a dummy read for communication test */
    return At24cRead( &at24c32s, &dummy, 1, 0);
}

/****************************************************************************/
int EEWriteData( uint16_t addr, CONST void *data, uint16_t len )
/****************************************************************************/
{
	return At24cWrite( &at24c32s, (uint8_t *)data, len, addr );
}

/****************************************************************************/
int EEReadData( uint16_t addr, void *data, uint16_t len )
/****************************************************************************/
{
	return At24cRead( &at24c32s, (uint8_t *)data, len, addr );
}
#endif //_DEV_EEPROM_H_
