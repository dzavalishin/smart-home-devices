#ifndef FS_PHATUTIL_H_
#define FS_PHATUTIL_H_

/*
 * Copyright (C) 2005 by egnite Software GmbH. All rights reserved.
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
 * \file fs/phatutil.h
 * \brief PHAT file system.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.3  2008/08/11 07:00:19  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.2  2006/02/23 15:47:18  haraldkipp
 * PHAT file system now supports configurable number of sector buffers.
 *
 * Revision 1.1  2006/01/05 16:33:06  haraldkipp
 * First check-in.
 *
 *
 * \endverbatim
 */

#include <fs/phatfs.h>

__BEGIN_DECLS
/* Prototypes */
extern void GetDosTimeStamp(uint16_t * dostim, uint16_t * dosdat);

extern int MakePhatName(CONST char *src, uint8_t * dst);
extern void MakeVisibleName(CONST uint8_t * src, char *dst);

extern char *GetParentPath(CONST char *path, CONST char **comp);

extern int IsFixedRootDir(NUTFILE * ndp);

extern void PhatFilePosRewind(PHATFILE * fcb);
extern int PhatFilePosSet(NUTFILE * nfp, uint32_t pos);

__END_DECLS
/* End of prototypes */
#endif
