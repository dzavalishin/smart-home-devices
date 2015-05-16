#ifndef _SYS_VERSION_H_
#define _SYS_VERSION_H_

/*
 * Copyright (C) 2001-2003 by egnite Software GmbH. All rights reserved.
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
 * Revision 1.2  2008/08/11 07:00:28  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.1.1.1  2003/05/09 14:41:23  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.1  2003/02/04 18:13:09  harald
 * Version 3 released
 *
 */

#include <compiler.h>
#include <sys/types.h>
#include <stdint.h>

#define NUT_VERSION_MAJOR   4
#define NUT_VERSION_MINOR   10
#define NUT_VERSION_RELEASE 2
#define NUT_VERSION_BUILD   1

#define OS_VERSION_NUMBER   (((uint32_t)NUT_VERSION_MAJOR << 24) | \
                            ((uint32_t)NUT_VERSION_MINOR << 16) | \
                            ((uint32_t)NUT_VERSION_RELEASE << 8) | \
                            (uint32_t)NUT_VERSION_BUILD)

__BEGIN_DECLS
/* */
extern uint32_t NutVersion(void);
extern CONST char *NutVersionString(void);

__END_DECLS
/* */
#endif

