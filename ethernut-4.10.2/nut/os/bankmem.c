/*
 * Copyright (C) 2004 by egnite Software GmbH. All rights reserved.
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

/*!
 * $Log$
 * Revision 1.7  2009/02/06 15:40:29  haraldkipp
 * Using newly available strdup() and calloc().
 * Replaced NutHeap routines by standard malloc/free.
 * Replaced pointer value 0 by NULL.
 *
 * Revision 1.6  2008/08/11 07:00:33  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.5  2008/02/15 17:08:05  haraldkipp
 * Calling the initialization routine more than once is now possible.
 * By default, half of the available memory will be allocated. Previous
 * versions eat all free memory but 8k.
 *
 * Revision 1.4  2007/04/12 09:08:57  haraldkipp
 * Segmented buffer routines ported to ARM.
 *
 * Revision 1.3  2004/12/17 15:28:33  haraldkipp
 * Bugfix. Comparison of the read and write pointers now includes the segments.
 * Thanks to Pete Allinson and Johan van der Stoel.
 *
 * Revision 1.2  2004/08/18 18:51:56  haraldkipp
 * Made banked memory configurable.
 *
 * Revision 1.1  2003/07/21 18:19:48  haraldkipp
 * First check in
 *
 */

#include <sys/heap.h>

#include <stdint.h>

/*!
 * \addtogroup xgBankMem
 */

/*@{*/

#include <sys/bankmem.h>

static char segbuf_empty;
static uint32_t segbuf_total;
static uint32_t segbuf_used;

static char *segbuf_start;
static char *segbuf_end;


static char *segbuf_wp;
static char segbuf_ws;
static char *segbuf_rp;
static char segbuf_rs;

/*!
 * \brief Reset the segmented buffer.
 *
 * \return Pointer to the first buffer segment.
 */
char *NutSegBufReset(void)
{
    segbuf_rp = segbuf_wp = segbuf_start;
    segbuf_rs = segbuf_ws = 0;
    NutSegBufEnable(0);
    segbuf_empty = 1;
    segbuf_used = 0;

    return segbuf_start;
}

/*!
 * \brief Initialize the segmented buffer.
 *
 * \param size Number of bytes to allocate for the global buffer.
 *             In systems with banked memory this parameter is 
 *             ignored and all banked memory is occupied for the
 *             global buffer. In systems without banked memory,
 *             the specified number of bytes is taken from heap
 *             memory.
 *
 * \return Pointer to the first buffer segment or null on failures.
 */
char *NutSegBufInit(size_t size)
{

#if NUTBANK_COUNT
    segbuf_start = (char *)(NUTBANK_START);
    segbuf_end = (char *)(NUTBANK_START) + NUTBANK_SIZE;
    segbuf_total = (uint32_t) NUTBANK_COUNT *(uint32_t) NUTBANK_SIZE;
#else
    if (size == 0)
        size = NutHeapAvailable() / 2;
    if (segbuf_start) {
        NutHeapFree(segbuf_start);
    }
    if ((segbuf_start = NutHeapAlloc(size)) != NULL)
        segbuf_end = segbuf_start + size;
    segbuf_total = size;
#endif

    return NutSegBufReset();
}

/*!
 * \brief Request segmented buffer space for writing.
 *
 * This call will also enable the current write segment and
 * may disable the current read segment.
 *
 * \param bcp Pointer to a variable, which receives the
 *            number of consecutive bytes available for 
 *            writing.
 *
 * \return Pointer to the next write position.
 */
char *NutSegBufWriteRequest(size_t * bcp)
{
    if (segbuf_empty || segbuf_ws != segbuf_rs || segbuf_wp > segbuf_rp)
        *bcp = segbuf_end - segbuf_wp;
    else
        *bcp = segbuf_rp - segbuf_wp;

    NutSegBufEnable(segbuf_ws);
    return segbuf_wp;
}

/*!
 * \brief Request segmented buffer space for reading.
 *
 * This call will also enable the current read segment and
 * may disable the current write segment.
 *
 * \param bcp Pointer to a variable, which receives the
 *            number of consecutive bytes available for 
 *            reading.
 *
 * \return Pointer to the next read position.
 */
char *NutSegBufReadRequest(size_t * bcp)
{
    if (segbuf_empty)
        *bcp = 0;
    else if (segbuf_ws != segbuf_rs || segbuf_rp >= segbuf_wp)
        *bcp = segbuf_end - segbuf_rp;
    else if ((*bcp = segbuf_wp - segbuf_rp) == 0 && segbuf_ws == segbuf_rs)
        segbuf_empty = 1;

    NutSegBufEnable(segbuf_rs);
    return segbuf_rp;
}

/*!
 * \brief Commit written buffer space.
 *
 * The write pointer will be incremented by the specified number of bytes.
 * If the pointer reaches the end of a segment, the next segment will be
 * enabled and the pointer will point to the start of the new segement.
 *
 * \param bc Number of bytes to commit.
 *
 * \return Pointer to the next write position.
 */
char *NutSegBufWriteCommit(size_t bc)
{
    if (bc) {
        segbuf_wp += bc;
        segbuf_empty = 0;
        segbuf_used += bc;
        if (segbuf_wp == segbuf_end) {
            segbuf_wp = segbuf_start;
#if NUTBANK_COUNT > 0
            if (++segbuf_ws >= NUTBANK_COUNT)
                segbuf_ws = 0;
#endif
            NutSegBufEnable(segbuf_ws);
        }
    }
    return segbuf_wp;
}

/*!
 * \brief Commit read buffer space.
 *
 * The read pointer will be incremented by the specified number of bytes.
 * If the pointer reaches the end of a segment, the next segment will be
 * enabled and the pointer will point to the start of the new segement.
 *
 * \param bc Number of bytes to commit.
 *
 * \return Pointer to the next read position.
 */
char *NutSegBufReadCommit(size_t bc)
{
    if (bc) {
        segbuf_rp += bc;
        segbuf_used -= bc;
        if (segbuf_rp == segbuf_end) {
            segbuf_rp = segbuf_start;
#if NUTBANK_COUNT > 0
            if (++segbuf_rs >= NUTBANK_COUNT)
                segbuf_rs = 0;
#endif
            NutSegBufEnable(segbuf_rs);
        }
        if (segbuf_rp == segbuf_wp  && segbuf_rs == segbuf_ws)
            segbuf_empty = 1;
    }
    return segbuf_rp;
}


/*!
 * \brief Commit written buffer space and finish write access.
 *
 * The write pointer will be incremented by the specified number of bytes.
 * This call will also enable the current read segment and may disable the 
 * current write segment.
 *
 * \param bc Number of bytes to commit.
 */
void NutSegBufWriteLast(size_t bc)
{
    if (bc) {
        segbuf_wp += bc;
        segbuf_used += bc;
        segbuf_empty = 0;
        if (segbuf_wp == segbuf_end) {
            segbuf_wp = segbuf_start;
#if NUTBANK_COUNT > 0
            if (++segbuf_ws >= NUTBANK_COUNT)
                segbuf_ws = 0;
#endif
        }
    }
    NutSegBufEnable(segbuf_rs);
}

/*!
 * \brief Commit written buffer space and finish read access.
 *
 * The write pointer will be incremented by the specified number of bytes.
 * This call will also enable the current read segment and may disable the 
 * current write segment.
 *
 * \param bc Number of bytes to commit.
 */
void NutSegBufReadLast(size_t bc)
{
    if (bc) {
        segbuf_rp += bc;
        segbuf_used -= bc;
        if (segbuf_rp == segbuf_end) {
            segbuf_rp = segbuf_start;
#if NUTBANK_COUNT > 0
            if (++segbuf_rs >= NUTBANK_COUNT)
                segbuf_rs = 0;
#endif
        }
        if (segbuf_rp == segbuf_wp && segbuf_rs == segbuf_ws)
            segbuf_empty = 1;
    }
    NutSegBufEnable(segbuf_ws);
}


/*!
 * \brief Return the available buffer space.
 *
 * \return Total number of free bytes in the buffer.
 */
uint32_t NutSegBufAvailable(void)
{
    return segbuf_total - segbuf_used;
}

/*!
 * \brief Return the used buffer space.
 *
 * \return Total number of used bytes in the buffer.
 */
uint32_t NutSegBufUsed(void)
{
    return segbuf_used;
}

/*@}*/
