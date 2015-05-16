/*
 * Copyright (C) 2001-2003 by egnite Software GmbH. All rights reserved.
 *
 * Copyright (c) 1990, 1993
 *      The Regents of the University of California.  All rights reserved.
 *
 * This code is partly derived from software contributed to Berkeley by
 * Chris Torek, but heavily rewritten for Nut/OS.
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

#ifndef _STDIO_H_

#ifndef _STDIO_VIRTUAL_H_
#define _STDIO_VIRTUAL_H_

#ifdef __NUT_EMULATION__
//  on an emulation platform, we need to have both
//              a) the native stdio headers and libs and
#include "stdio_orig.h"
//              b) the nut os header and implementation
//              the nut os function calls and defines are renamed by the stdio_nut_wrapper.h

// some defines in /usr/include/stdio.h we need to overload
#ifndef NO_STDIO_NUT_WRAPPER

#undef getc
#undef putc
#undef stdin
#undef stdout
#undef stderr
#undef clearerr
#undef feof
#undef ferror
#undef getchar
#undef putchar

#include <stdio_nut_wrapper.h>

#endif                          /* NO_STDIO_NUT_WRAPPER */

#endif                          /* __NUT_EMULATION__ */

#ifndef _STDIO_H_
#define _STDIO_H_
#endif

#ifdef NO_STDIO_NUT_WRAPPER
// this is for unix device drivers, they want to see their native functions
// and don't need nut stdio
#else

#include <sys/types.h>
#include <stdarg.h>

/*!
 * \file stdio.h
 * \brief C Standard I/O
 */

/*!
 * \addtogroup xgCrtStdio
 */
/*@{*/

#ifndef EOF
/*! 
 * \brief End of file.
 *
 * Returned by an input or output operation when the end of a file is 
 * encountered. Some routines return this value to indicate an error.
 */
#define EOF	(-1)
#endif

#ifndef _IOFBF
#define _IOFBF	    0x00        /*!< \brief Fully buffered. */
#define _IOLBF	    0x01        /*!< \brief Line buffered. */
#define _IONBF	    0x02        /*!< \brief Unbuffered. */
#endif

/*!
 * \brief Stream structure type.
 * 
 * A pointer to this type is used for all standard I/O functions to 
 * specify a stream.
 *
 * \note Applications should make any assumptions about the contents
 *       of this structure as it may change without further notice.
 */
typedef struct __iobuf FILE;

extern FILE *__iob[];           /*!< \internal Stream slots. */

#define stdin (__iob[0])        /*!< \brief Standard input stream. */
#define stdout (__iob[1])       /*!< \brief Standard output stream. */
#define stderr (__iob[2])       /*!< \brief Standard error output stream. */

/*@}*/

#ifndef SEEK_SET
#define SEEK_SET 0
#endif

#ifndef SEEK_CUR
#define SEEK_CUR 1
#endif

#ifndef SEEK_END
#define SEEK_END 2
#endif


extern void clearerr(FILE * stream);
extern int fclose(FILE * stream);
extern void fcloseall(void);
extern FILE *_fdopen(int fd, CONST char *mode);
extern int feof(FILE * stream);
extern int ferror(FILE * stream);
extern int fflush(FILE * stream);
extern int fgetc(FILE * stream);
extern char *fgets(char *buffer, int count, FILE * stream);
extern int _fileno(FILE * stream);
extern void _flushall(void);
extern FILE *fopen(CONST char *name, CONST char *mode);
extern int fprintf(FILE * stream, CONST char *fmt, ...);
extern int fpurge(FILE * stream);
extern int fputc(int c, FILE * stream);
extern int fputs(CONST char *string, FILE * stream);
extern size_t fread(void *buffer, size_t size, size_t count, FILE * stream);
extern FILE *freopen(CONST char *name, CONST char *mode, FILE * stream);
extern int fscanf(FILE * stream, CONST char *fmt, ...);
extern int fseek(FILE * stream, long offset, int origin);
extern long ftell(FILE * stream);
extern size_t fwrite(CONST void *data, size_t size, size_t count, FILE * stream);
extern int getc(FILE * stream);
extern int getchar(void);
extern int kbhit(void);
extern char *gets(char *buffer);
extern int printf(CONST char *fmt, ...);
extern int putc(int c, FILE * stream);
extern int putchar(int c);
extern int puts(CONST char *string);
extern int scanf(CONST char *fmt, ...);
extern int sprintf(char *buffer, CONST char *fmt, ...);
extern int sscanf(CONST char *string, CONST char *fmt, ...);
extern int ungetc(int c, FILE * stream);
extern int vfprintf(FILE * stream, CONST char *fmt, va_list ap);
extern int vfscanf(FILE * stream, CONST char *fmt, va_list ap);
extern int vsprintf(char *buffer, CONST char *fmt, va_list ap);
extern int vsscanf(CONST char *string, CONST char *fmt, va_list ap);
extern int rename(CONST char *old_name, CONST char *new_name);

#ifdef __HARVARD_ARCH__
/* Strings in program space need special handling for Harvard architectures. */
extern int fprintf_P(FILE * stream, PGM_P fmt, ...) __attribute__((format(printf, 2, 3)));
extern int fputs_P(PGM_P string, FILE * stream);
extern int fscanf_P(FILE * stream, PGM_P fmt, ...) __attribute__((format(scanf, 2, 3)));
extern size_t fwrite_P(PGM_P data, size_t size, size_t count, FILE * stream);
extern int printf_P(PGM_P fmt, ...) __attribute__((format(printf, 1, 2)));
extern int puts_P(PGM_P string);
extern int scanf_P(PGM_P fmt, ...) __attribute__((format(scanf, 1, 2)));
extern int sprintf_P(char *buffer, PGM_P fmt, ...) __attribute__((format(printf, 2, 3)));
extern int sscanf_P(CONST char *string, CONST char *fmt, ...) __attribute__((format(scanf, 2, 3)));
extern int vfprintf_P(FILE * stream, PGM_P fmt, va_list ap);
extern int vfscanf_P(FILE * stream, PGM_P fmt, va_list ap);
extern int vsprintf_P(char *buffer, PGM_P fmt, va_list ap);
extern int vsscanf_P(CONST char *string, PGM_P fmt, va_list ap);

#else /* __HARVARD_ARCH__ */

#if !defined(NUT_STDIO_PREFIXED)
/* Map to standard functions, if program and data space are equally accessable. */
#define fputs_P(string, stream) fputs(string, stream)
#define fwrite_P(data, size, count, stream) fwrite(data, size, count, stream)
#define puts_P(string) puts(string)
#define vfprintf_P(stream, fmt, ap) vfprintf(stream, fmt, ap)
#define vfscanf_P(stream, fmt, ap) vfscanf(stream, fmt, ap)
#define vsprintf_P(buffer, fmt, ap) vsprintf(buffer, fmt, ap)
#define vsscanf_P(string, fmt, ap) vsscanf(string, fmt, ap)

#if defined(__GNUC__)
#define fprintf_P(...)  fprintf(__VA_ARGS__)
#define fscanf_P(...)   fscanf(__VA_ARGS__)
#define printf_P(...)   printf(__VA_ARGS__)
#define scanf_P(...)    scanf(__VA_ARGS__)
#define sprintf_P(...)  sprintf(__VA_ARGS__)
#define sscanf_P(...)   sscanf(__VA_ARGS__)
#else /* __GNUC__ */
#define fprintf_P       fprintf
#define fscanf_P        fscanf
#define printf_P        printf
#define scanf_P         scanf
#define sprintf_P       sprintf
#define sscanf_P        sscanf
#endif /* __GNUC__ */

#endif /* NUT_STDIO_PREFIXED */

#endif /* __HARVARD_ARCH__ */

#endif /* NO_STDIO_NUT_WRAPPER */

#endif                          /* _STDIO_VIRTUAL_H_ */

#endif                          /* _STDIO_H_ */
