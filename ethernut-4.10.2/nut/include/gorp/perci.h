#ifndef GORP_PERCI_H
#define GORP_PERCI_H

/*
 * Copyright 2010 by egnite GmbH
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
 * \file include/gorp/perci.h
 * \brief Persistent circular buffer definitions.
 *
 * \verbatim
 * $Id$
 * \endverbatim
 */

#include <cfg/perci.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

/*!
 * \addtogroup xgPerci
 */
/*@{*/

#ifndef PERCI_MAX_RECORDS
#define PERCI_MAX_RECORDS   256
#endif

#if PERCI_MAX_RECORDS <= 256
typedef uint8_t perci_recnum_t;
typedef uint_fast8_t perci_fast_recnum_t;
#elif PERCI_MAX_RECORDS <= 65536
typedef uint16_t perci_recnum_t;
typedef uint_fast16_t perci_fast_recnum_t;
#else
typedef uint32_t perci_recnum_t;
typedef uint_fast32_t perci_fast_recnum_t;
#endif


#ifndef PERCI_RECSIZE
#define PERCI_RECSIZE       256
#endif

#if PERCI_RECSIZE <= 256
typedef uint8_t perci_reclen_t;
typedef uint_fast8_t perci_fast_reclen_t;
#elif PERCI_RECSIZE <= 65536
typedef uint16_t perci_reclen_t;
typedef uint_fast16_t perci_fast_reclen_t;
#else
typedef uint32_t perci_reclen_t;
typedef uint_fast32_t perci_fast_reclen_t;
#endif

#define PERCI_DATASIZE  (PERCI_RECSIZE - sizeof(perci_reclen_t))

typedef struct __attribute__ ((packed)) _PERCI_RECORD {
    perci_reclen_t pcd_len;
    uint8_t pcd_data[PERCI_RECSIZE];
} PERCI_RECORD;

typedef struct _PERCI_WRITER {
    int pcw_fd;
    long pcw_size;
    HANDLE pcw_mutex;
    perci_fast_recnum_t pcw_recnum;
    PERCI_RECORD pcw_rec;
} PERCI_WRITER;

typedef struct _PERCI_READER {
    PERCI_WRITER *pcr_cil;
    perci_fast_recnum_t pcr_recnum;
    perci_fast_reclen_t pcr_reclen;
    perci_fast_reclen_t pcr_recpos;
} PERCI_READER;

__BEGIN_DECLS
/* Prototypes */

extern int PerCiInit(char *path, int recs);

extern PERCI_WRITER *PerCiOpen(char *path);
extern void PerCiClose(PERCI_WRITER *writer);
extern void PerCiFlush(PERCI_WRITER * writer);

extern int PerCiWrite(PERCI_WRITER *writer, CONST char *data, int len);
extern int PerCiWriteFormat(PERCI_WRITER * writer, CONST char *fmt, ...);
extern int PerCiWriteVarList(PERCI_WRITER * writer, CONST char *fmt, va_list ap);

extern PERCI_READER *PerCiAttachReader(PERCI_WRITER *writer);
extern void PerCiDetachReader(PERCI_READER *reader);

extern int PerCiRead(PERCI_READER *reader, char *data, int len);
extern int PerCiReadLine(PERCI_READER * reader, char *line, int len);

extern void PerCiDump(FILE *stream, char *path);

__END_DECLS
/* End of prototypes */
#endif
