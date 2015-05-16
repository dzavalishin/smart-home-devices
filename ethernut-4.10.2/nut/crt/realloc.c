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
 * $Id: realloc.c 2454 2009-02-13 14:42:45Z haraldkipp $
 */

#include <sys/heap.h>

#include <stdlib.h>
#include <errno.h>

/*!
 * \addtogroup xgHeap
 */
/*@{*/

/*!
* \brief Reallocate a block from heap memory.
*
* This function simply calls NutHeapRealloc(). It overrides the function
* of the runtime library, when the application is linked with nutcrt or
* nutcrtf.
*
* \param ptr Pointer to memory block previously allocated with malloc to be reallocated.
*            If this is NULL, a new block is allocated.
* \param len Size of the requested memory block.
*
* \return Pointer to the reallocated memory block if the
*         function is successful or NULL if the requested
*         amount of memory is not available.
*/
void *realloc(void* ptr, size_t len)
{
    void *p;

    if ((p = NutHeapRealloc(ptr, len)) == NULL) {
        errno = ENOMEM;
    }
    return p;
}

/*@}*/
