#ifndef _MEMDEBUG_H_
#define _MEMDEBUG_H_

/*
 * Copyright (C) 2009 by egnite GmbH
 *
 * All rights reserved.
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

/*
 * $Id: memdebug.h 2932 2010-03-29 12:08:08Z haraldkipp $
 */

#include <compiler.h>
#include <cfg/memory.h>
#include <stdlib.h>
#include <string.h>

#ifdef NUTDEBUG_HEAP
#define malloc(s) dbg_malloc(s, __FILE__, __LINE__)
#define calloc(n, s) dbg_calloc(n, s, __FILE__, __LINE__)
#define free(p) dbg_free(p, __FILE__, __LINE__)
#define realloc(p, s) dbg_realloc(p, s, __FILE__, __LINE__)
#define strdup(s) dbg_strdup(s, __FILE__, __LINE__)
#endif

__BEGIN_DECLS
/* Prototypes. */
extern void *dbg_malloc(size_t len, CONST char *file, int line);
extern void *dbg_calloc(size_t num, size_t size, CONST char *file, int line);
extern void dbg_free(void *p, CONST char *file, int line);
extern void *dbg_realloc(void* ptr, size_t len, CONST char *file, int line);
extern char *dbg_strdup(CONST char *str, CONST char *file, int line);
/* Prototypes. */
__END_DECLS

#endif
