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
#ifndef _DEV_PCA9555_H_
#define	_DEV_PCA9555_H_

#include <cfg/pca9555.h>
#include <dev/twif.h>

#define IOXP_PORT0  0x80
#define IOXP_PORT1  0x81

#define IOXP_PIN0   0
#define IOXP_PIN1   1
#define IOXP_PIN2   2
#define IOXP_PIN3   3
#define IOXP_PIN4   4
#define IOXP_PIN5   5
#define IOXP_PIN6   6
#define IOXP_PIN7   7

#define GPIO_CFG_INVERT 0x00010000
#define GPIO_CFG_NORM   0x00020000

__BEGIN_DECLS
/* Prototypes */
extern int IOExpInit( void );
extern int IOExpPinConfigSet( int bank, int bit, uint32_t flags);
extern int IOExpRawWrite ( int bank, int value );
extern int IOExpRawRead ( int bank, int *value );
extern int IOExpSetBitLow( int bank, int bit );
extern int IOExpSetBitHigh( int bank, int bit );
extern int IOExpGetBit ( int bank, int bit, int *value );
extern int IOExpSetBit( int bank, int bit, int value );

__END_DECLS
/* End of prototypes */
#endif
