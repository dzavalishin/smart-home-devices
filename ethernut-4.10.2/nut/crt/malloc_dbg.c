/*
 * Copyright (C) 2009 by egnite GmbH
 * Copyright (C) 2001-2003 by egnite Software GmbH
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
 * $Id: malloc_dbg.c 2462 2009-02-13 14:51:15Z haraldkipp $
 */

#include <sys/heap.h>

#include <stdlib.h>
#include <errno.h>
#include <memdebug.h>

/*!
 * \addtogroup xgHeap
 */
/*@{*/

#ifdef NUTDEBUG_HEAP

/*!
 * \brief Allocate a block from heap memory.
 *
 * This function simply calls NutHeapAlloc(). It overrides the function
 * of the runtime library, when the application is linked with nutcrt or
 * nutcrtf.
 *
 * \param len Size of the requested memory block.
 *
 * \return Pointer to the allocated memory block if the
 *         function is successful or NULL if the requested
 *         amount of memory is not available.
 */
void *dbg_malloc(size_t len, CONST char *file, int line)
{
    void *p;

    if ((p = NutHeapDebugRootAlloc(&heapFreeList, len, file, line)) == NULL) {
        errno = ENOMEM;
    }
    return p;
}

/*!
 * \brief Return a block to heap memory.
 *
 * This function simply calls NutHeapFree(). It overrides the function
 * of the runtime library, when the application is linked with nutcrt or
 * nutcrtf.
 *
 * \param p Points to a memory block previously allocated
 *          through a call to malloc().
 */
void dbg_free(void *p, CONST char *file, int line)
{
    NutHeapDebugRootFree(&heapFreeList, p, file, line);
}

#endif

/*@}*/
