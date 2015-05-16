#ifndef _SYS_HEAP_H_
#define _SYS_HEAP_H_

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
 * $Id: heap.h 2609 2009-04-15 13:41:35Z haraldkipp $
 */

#include <cfg/memory.h>
#include <sys/types.h>


/*!
 * \file sys/heap.h
 * \brief Heap management definitions.
 */

/*!
 * \struct _HEAPNODE heap.h sys/heap.h
 * \brief Heap memory node information structure.
 */
typedef struct _HEAPNODE HEAPNODE;

/*!
 * \typedef HEAPNODE
 * \brief Heap memory node type.
 */
struct _HEAPNODE {
    size_t hn_size;     /*!< \brief Size of this node. */
#ifdef NUTDEBUG_HEAP
    HEAPNODE *ht_next;
    size_t ht_size;
    CONST char *ht_file;
    int ht_line;
#endif
    HEAPNODE *hn_next;  /*!< \brief Link to next free node. */
};


extern HEAPNODE *heapFreeList;

#define NutHeapAdd(a, s)                NutHeapRootAdd(&heapFreeList, a, s)
#define NutHeapAvailable()              NutHeapRootAvailable(&heapFreeList)
#define NutHeapRegionAvailable()        NutHeapRootRegionAvailable(&heapFreeList)

#ifdef NUTDEBUG_HEAP
#define NutHeapAlloc(s)                 NutHeapDebugRootAlloc(&heapFreeList, s, __FILE__, __LINE__)
#define NutHeapAllocClear(s)            NutHeapDebugRootAllocClear(&heapFreeList, s, __FILE__, __LINE__)
#define NutHeapFree(p)                  NutHeapDebugRootFree(&heapFreeList, p, __FILE__, __LINE__)
#define NutHeapRealloc(p, s)            NutHeapDebugRootRealloc(&heapFreeList, p, s, __FILE__, __LINE__)
#else
#define NutHeapAlloc(s)                 NutHeapRootAlloc(&heapFreeList, s)
#define NutHeapAllocClear(s)            NutHeapRootAllocClear(&heapFreeList, s)
#define NutHeapFree(p)                  NutHeapRootFree(&heapFreeList, p)
#define NutHeapRealloc(p, s)            NutHeapRootRealloc(&heapFreeList, p, s)
#endif

#if defined(NUTMEM_STACKHEAP)
/* Dedicated stack memory. */
#ifndef NUTMEM_SPLIT_FAST
#define NUTMEM_SPLIT_FAST
#endif
#define NutStackAdd(a, s)               NutHeapFastMemAdd(a, s)
#define NutStackAlloc(s)                NutHeapFastMemAlloc(s)
#define NutStackFree(p)                 NutHeapFastMemFree(p)
#else /* NUTMEM_STACKHEAP */
/* Thread stacks resides in normal heap. */
#define NutStackAlloc(s)                NutHeapAlloc(s)
#define NutStackFree(p)                 NutHeapFree(p)
#endif /* NUTMEM_STACKHEAP */

#ifdef NUTMEM_SPLIT_FAST

extern HEAPNODE *heapFastMemFreeList;

#define NutHeapFastMemAdd(a, s)         NutHeapRootAdd(&heapFastMemFreeList, a, s)
#define NutHeapFastMemAvailable()       NutHeapRootAvailable(&heapFastMemFreeList)
#define NutHeapFastMemRegionAvailable() NutHeapRootRegionAvailable(&heapFastMemFreeList)

#ifdef NUTDEBUG_HEAP
#define NutHeapFastMemAlloc(s)          NutHeapDebugRootAlloc(&heapFastMemFreeList, s, __FILE__, __LINE__)
#define NutHeapFastMemAllocClear(s)     NutHeapDebugRootAllocClear(&heapFastMemFreeList, s, __FILE__, __LINE__)
#define NutHeapFastMemFree(p)           NutHeapDebugRootFree(&heapFastMemFreeList, p, __FILE__, __LINE__)
#define NutHeapFastMemRealloc(p, s)     NutHeapDebugRootRealloc(&heapFastMemFreeList, p, s, __FILE__, __LINE__)
#else
#define NutHeapFastMemAlloc(s)          NutHeapRootAlloc(&heapFastMemFreeList, s)
#define NutHeapFastMemAllocClear(s)     NutHeapRootAllocClear(&heapFastMemFreeList, s)
#define NutHeapFastMemFree(p)           NutHeapRootFree(&heapFastMemFreeList, p)
#define NutHeapFastMemRealloc(p, s)     NutHeapRootRealloc(&heapFastMemFreeList, p, s)
#endif

#endif /* NUTMEM_SPLIT_FAST */

__BEGIN_DECLS
/* Prototypes */

extern void NutHeapRootAdd(HEAPNODE** root, void *addr, size_t size);
extern size_t NutHeapRootAvailable(HEAPNODE** root);
extern size_t NutHeapRootRegionAvailable(HEAPNODE** root);

#ifdef NUTDEBUG_HEAP
extern void *NutHeapDebugRootAlloc(HEAPNODE** root, size_t size, CONST char *file, int line);
extern void *NutHeapDebugRootAllocClear(HEAPNODE** root, size_t size, CONST char *file, int line);
extern int NutHeapDebugRootFree(HEAPNODE** root, void *block, CONST char *file, int line);
extern void *NutHeapDebugRootRealloc(HEAPNODE** root, void * block, size_t size, CONST char *file, int line);
#else
extern void *NutHeapRootAlloc(HEAPNODE** root, size_t size);
extern void *NutHeapRootAllocClear(HEAPNODE** root, size_t size);
extern int NutHeapRootFree(HEAPNODE** root, void *block);
extern void *NutHeapRootRealloc(HEAPNODE** root, void * block, size_t size);
#endif

extern int NutHeapCheck(void);
extern void NutHeapDump(void * stream);

/* Prototypes */
__END_DECLS

#endif
