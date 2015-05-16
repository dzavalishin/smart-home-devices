/*
 * Copyright (C) 2001-2004 by egnite Software GmbH. All rights reserved.
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
 * $Log$
 * Revision 1.7  2007/05/02 11:30:33  haraldkipp
 * Mapped _write_P to _write for non Harvard architectures.
 *
 * Revision 1.6  2006/10/08 16:48:09  haraldkipp
 * Documentation fixed
 *
 * Revision 1.5  2005/08/05 11:18:00  olereinhardt
 * Added support for _seek, _tell, fseek, ftell functions
 *
 * Revision 1.4  2005/08/02 17:46:47  haraldkipp
 * Major API documentation update.
 *
 * Revision 1.3  2004/07/30 19:10:42  drsung
 * First proposal to define some global ioctl commands.
 *
 */

#ifndef _NUT_IO_H_
#define	_NUT_IO_H_
 
 
#include <sys/types.h>

/*!
 * \addtogroup xgCrtLowio
 */
/*@{*/

/* 
 * Some global definitions for ioctl commands.
 */
#define IOCTL_GETSTATUS                 0x0001
#define IOCTL_SETSTATUS                 0x0002
#define IOCTL_SETBUFFERMODE             0x0003
#define IOCTL_GETBUFFERMODE             0x0004
#define IOCTL_GETOUTBUFSIZE             0x0005
#define IOCTL_SETOUTBUFSIZE             0x0006
#define IOCTL_GETINBUFSIZE              0x0007
#define IOCTL_SETINBUFSIZE              0x0008
/*! \brief Flush the output buffer.
 *
 * Currently not provided by any known device.
 */
#define IOCTL_FLUSHOUTBUF               0x0009
/*! \brief Return the number of bytes currently available in the input buffer.
 */
#define IOCTL_GETFILESIZE               0x000A
/*! \brief Return the number of bytes currently available in the input buffer.
 */
#define IOCTL_GETOUTBUFCOUNT            0x000B
/*! \brief Return the number of bytes currently available in the output buffer.
 */
#define IOCTL_GETINBUFCOUNT             0x000C

/*@}*/

extern int _close(int fd);
extern int _open(CONST char *name, int mode);
extern int _read(int fd, void *buffer, size_t count);
extern int _write(int fd, CONST void *buffer, size_t count);
#ifdef __HARVARD_ARCH__
extern int _write_P(int fd, PGM_P buffer, size_t count);
#else
#define _write_P(fd, buffer, count) _write(fd, buffer, count)
#endif
extern int _seek(int fd, long offset, int origin);
extern long _tell(int fd);

extern int _ioctl(int fd, int cmd, void *buffer);
extern long _filelength(int fd);

#endif
