#ifndef _DEV_NVMEM_H_
#define _DEV_NVMEM_H_

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

/*!
 * \file dev/nvmem.h
 * \brief Header file for non-volatile memory access.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.4  2009/01/17 11:26:47  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.3  2006/07/05 07:45:29  haraldkipp
 * Split on-chip interface definitions.
 *
 * Revision 1.2  2006/05/25 09:09:57  haraldkipp
 * API documentation updated and corrected.
 *
 * Revision 1.1  2006/01/23 17:26:19  haraldkipp
 * Platform independant routines added, which provide generic access to
 * non-volatile memory.
 *
 *
 * \endverbatim
 */

#include <cfg/eeprom.h>
#include <sys/types.h>

__BEGIN_DECLS
/* Function prototypes */

extern int NutNvMemLoad(unsigned int addr, void *buff, size_t siz);
extern int NutNvMemSave(unsigned int addr, CONST void *buff, size_t len);

extern int OnChipNvMemLoad(unsigned int addr, void *buff, size_t siz);
extern int OnChipNvMemSave(unsigned int addr, CONST void *buff, size_t len);

extern int NutNvMemLock(void);
extern int NutNvMemUnlock(void);

__END_DECLS
/* End of prototypes */

#endif
