#ifndef _DEV_AT49BV_H_
#define	_DEV_AT49BV_H_

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
 * Revision 1.3  2009/01/17 11:26:47  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.2  2008/07/17 11:51:19  olereinhardt
 * Added function AT49bvReadProtectionRegister to read the 64bit factory
 * or user id
 *
 * Revision 1.1  2006/04/07 13:51:36  haraldkipp
 * AT49BV flash memory support added. A single sector may be used to
 * store system configurations in case there is no EEPROM available.
 *
 */

#include <sys/types.h>

__BEGIN_DECLS
/* Prototypes */

extern unsigned long At49bvInit(void);
extern unsigned long long AT49bvReadProtectionRegister(int factory);
extern int At49bvChipErase(void);
extern int At49bvSectorErase(unsigned int off);
extern int At49bvSectorRead(unsigned int off, void *data, unsigned int len);
extern int At49bvSectorWrite(unsigned int off, CONST void *data, unsigned int len);
extern int At49bvParamRead(unsigned int pos, void *data, unsigned int len);
extern int At49bvParamWrite(unsigned int pos, CONST void *data, unsigned int len);

__END_DECLS
/* End of prototypes */
#endif
