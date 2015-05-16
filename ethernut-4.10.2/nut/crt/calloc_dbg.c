/*
 * Copyright (C) 2009 by egnite GmbH
 * Copyright (C) 2004 by egnite Software GmbH
 * Copyright (c) 1990 The Regents of the University of California.
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
 */

/*
 * $Id: calloc_dbg.c 2462 2009-02-13 14:51:15Z haraldkipp $
 */

#include <compiler.h>

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <memdebug.h>

/*!
 * \addtogroup xgCrtStdLib
 */
/*@{*/

/*!
 * \brief Allocate space for an array.
 *
 * \param num  Number of elements.
 * \param size Size of a single element.
 *
 * \return A pointer to the allocated space or a null pointer if 
 *         not enough memory space is available.
 */

void *dbg_calloc(size_t num, size_t size, CONST char *file, int line)
{
    register void *p;

#ifdef SIZE_T_MAX
    if (num && size && SIZE_T_MAX / num < size) {
        errno = ENOMEM;
        return NULL;
    }
#endif
    size *= num;
    p = dbg_malloc(size, file, line);
    if (p)
        memset(p, 0, size);
    return (p);
}

/*@}*/
