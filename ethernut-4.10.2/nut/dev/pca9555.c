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
 * First checkin, new twi driver for pca9555 (currently SAM7X256 is tested
 * only)
 *
 */
#include <compiler.h>
#include <stdlib.h>
#include <string.h>
#include <memdebug.h>
#include <sys/heap.h>
#include <sys/event.h>

#include <cfg/os.h>
#include <dev/twif.h>

#include <dev/gpio.h>
#include <cfg/pca9555.h>
#include <dev/pca9555.h>

#ifndef I2C_SLA_IOEXP
#define I2C_SLA_IOEXP     0x23
#endif

#define PCA_PINP    0   /**< PCA Input register offset */
#define PCA_POUT    2   /**< PCA Output register offset */
#define PCA_PINV    4   /**< PCA Polarity inversion register offset */
#define PCA_CONF    6   /**< PCA Configuration register offset */

typedef struct __attribute__ ((packed))
{
    uint8_t out[2];
    uint8_t pol[2];
    uint8_t con[2];
} pca_regs_t;

pca_regs_t *pca_ctrl = NULL;

/*****************************************************************/
int IOExpInit( void )
/*****************************************************************/
{
    pca_ctrl = malloc( sizeof( pca_regs_t));
    if( pca_ctrl == NULL) return -1;

    memset( pca_ctrl, 0, sizeof( pca_regs_t));
    pca_ctrl->out[0] = 0xff;
    pca_ctrl->out[1] = 0xff;
    pca_ctrl->con[0] = 0xff;
    pca_ctrl->con[1] = 0xff;
    pca_ctrl->pol[0] = 0x00;
    pca_ctrl->pol[1] = 0x00;
    
    if( TwMasterRegWrite( I2C_SLA_IOEXP, PCA_POUT, 1, &pca_ctrl->out[0], 2, 50) == -1)
        return -1;

    if( TwMasterRegWrite( I2C_SLA_IOEXP, PCA_CONF , 1, &pca_ctrl->con[0], 2, 50) == -1)
        return -1;
    
    if( TwMasterRegWrite( I2C_SLA_IOEXP, PCA_PINV , 1, &pca_ctrl->pol[0], 2, 50) == -1)
        return -1;

	return 0;
}

/*****************************************************************/
int IOExpPinConfigSet( int bank, int bit, uint32_t flags)
/*****************************************************************/
{
    bank &= 0xf;
    
    if( flags == 0) /* Input */
        pca_ctrl->con[bank] |= (1<<bit);
    if( flags & GPIO_CFG_OUTPUT)
        pca_ctrl->con[bank] &= ~(1<<bit);
    if( flags & GPIO_CFG_INVERT)
        pca_ctrl->pol[bank] |= (1<<bit);
    if( flags & GPIO_CFG_NORM)
        pca_ctrl->pol[bank] &= ~(1<<bit);
    
    if( TwMasterRegWrite( I2C_SLA_IOEXP, PCA_POUT+bank, 1, &pca_ctrl->out[bank], 1, 50) == -1)
        return -1;

    if( TwMasterRegWrite( I2C_SLA_IOEXP, PCA_CONF+bank, 1, &pca_ctrl->con[bank], 1, 50) == -1)
        return -1;
    
    if( TwMasterRegWrite( I2C_SLA_IOEXP, PCA_PINV+bank, 1, &pca_ctrl->pol[bank], 1, 50) == -1)
        return -1;

    return 0;      
}

/*****************************************************************/
int IOExpRawWrite ( int bank, int value )
/*****************************************************************/
{
    bank &= 0x0f;
	if ( bank > 1 )	return -1;

	pca_ctrl->out[bank] = value;

	if( TwMasterRegWrite( I2C_SLA_IOEXP, PCA_POUT+bank, 1, &pca_ctrl->out[bank], 1, 50 ) == -1 )
		return -1;
	return 0;
}

/*****************************************************************/
int IOExpRawRead ( int bank, int *value )
/*****************************************************************/
{
    bank &= 0x0f;
	if( bank > 1 ) return -1;

	if( TwMasterRegRead( I2C_SLA_IOEXP, PCA_PINP+bank, 1, value, 1, 50) == -1)
		return-1;
	return 0;
}


/*****************************************************************/
int IOExpGetBit ( int bank, int bit, int *value )
/*****************************************************************/
{
	int val;

    bank &= 0x0f;
	if( bank > 1 ) return -1;

	if( TwMasterRegRead( I2C_SLA_IOEXP, PCA_PINP+bank, 1, &val, 1, 50)==-1)
		return -1;
	else
		*value = (( val & ( 1 << bit )) == 0 ) ? 0 : 1;

	return 0;
}

/*****************************************************************/
int IOExpSetBitHigh( int bank, int bit )
/*****************************************************************/
{
    bank &= 0x0f;
	if( bank > 1 ) return -1;

	pca_ctrl->out[bank] |= ( 1 << bit );

	if( TwMasterRegWrite( I2C_SLA_IOEXP, PCA_POUT+bank, 1, &pca_ctrl->out[bank], 1, 50 ) == -1 )
		return -1;
	return 0;
}

/*****************************************************************/
int IOExpSetBitLow( int bank, int bit )
/*****************************************************************/
{
    bank &= 0x0f;
	if( bank > 1 ) return -1;

	pca_ctrl->out[bank] &= ~( 1 << bit );

	if( TwMasterRegWrite( I2C_SLA_IOEXP, PCA_POUT+bank, 1, &pca_ctrl->out[bank], 1, 50 ) == -1 )
		return -1;
	return 0;
}

/*****************************************************************/
int IOExpSetBit( int bank, int bit, int value )
/*****************************************************************/
{
    if( value)
        return IOExpSetBitHigh( bank, bit);
    else
        return IOExpSetBitLow( bank, bit);
}

