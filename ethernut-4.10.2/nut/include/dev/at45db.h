#ifndef _DEV_AT45DB_H_
#define _DEV_AT45DB_H_
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
 * \file dev/at45db.h
 * \brief Dataflash helper routines.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.5  2009/01/17 11:26:47  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.4  2008/08/11 06:59:59  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.3  2008/02/15 17:10:44  haraldkipp
 * At45dbPageErase selected the wrong bank. Fixed. Parameter pgn (page number)
 * of At45dbPageWrite() changed from unsigned int to unsigned long.
 * New routines At45dbPages() and At45dbPageSize() allow to determine the
 * chip's layout.
 *
 * Revision 1.2  2006/10/08 16:48:09  haraldkipp
 * Documentation fixed
 *
 * Revision 1.1  2006/09/29 12:41:55  haraldkipp
 * Added support for AT45 serial DataFlash memory chips. Currently limited
 * to AT91 builds.
 *
 *
 * \endverbatim
 */

#include <sys/types.h>
#include <stdint.h>

__BEGIN_DECLS
/* Prototypes */
extern int At45dbSendCmd(int dd, uint8_t op, uint32_t parm, int len, CONST void *tdata, void *rdata, int datalen);
extern uint8_t At45dbGetStatus(int dd);
extern int At45dbWaitReady(int dd, uint32_t tmo, int poll);
extern int At45dbInit(unsigned int spibas, unsigned int spipcs);
extern int At45dbPageErase(int dd, uint32_t pgn);
extern int At45dbChipErase(void);
extern int At45dbPageRead(int dd, uint32_t pgn, void *data, unsigned int len);
extern int At45dbPageWrite(int dd, uint32_t pgn, CONST void *data, unsigned int len);
extern uint32_t At45dbPages(int dd);
extern unsigned int At45dbPageSize(int dd);
extern int At45dbParamRead(unsigned int pos, void *data, unsigned int len);
extern int At45dbParamWrite(unsigned int pos, CONST void *data, unsigned int len);

__END_DECLS
/* End of prototypes */
#endif
