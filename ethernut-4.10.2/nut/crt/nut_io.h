#ifndef _CRT_NUT_IO_H_
#define _CRT_NUT_IO_H_

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
 * Revision 1.4  2008/08/11 06:59:40  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.3  2008/02/15 16:51:21  haraldkipp
 * Serious bug fix. Reconfigured FOPEN_MAX was not honored by all modules.
 *
 * Revision 1.2  2004/04/15 10:23:24  haraldkipp
 * Distinguish between read error and end of file
 *
 * Revision 1.1.1.1  2003/05/09 14:40:30  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.1  2003/02/04 17:49:08  harald
 * *** empty log message ***
 *
 */

#include <cfg/crt.h>
#include <sys/types.h>

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

/*!
 * \addtogroup xgCrt
 */
/*@{*/

#define _IOUNG	    0x08    /*!< \internal Unget buffer filled. */
#define _IOERR	    0x10    /*!< \internal Error occured. */
#define _IOEOF      0x20    /*!< \internal End of file reached. */
#define _IOPGM	    0x40    /*!< \internal Input from program memory. */

#ifndef BUFSIZ
#define	BUFSIZ	    128	    /*!< \brief Default maximum size of stream buffers.
				 Currently not used. 
				 \showinitializer */
#endif

#ifndef FOPEN_MAX
#define	FOPEN_MAX	8   /*!< \brief Default maximum number of open streams.
				 \showinitializer */
#endif

#ifndef FILENAME_MAX
#define	FILENAME_MAX	255 /*!< \brief Default maximum size of filenames. 
				 \showinitializer */
#endif

/*! \internal
 * \brief Stores the current status of a stream.
 */
struct __iobuf {
    int     iob_fd;		/*!< \internal Associated file, device or socket descriptor. */
    uint16_t iob_mode;		/*!< \internal Access mode, see fcntl.h. */
    uint8_t iob_flags;		/*!< \internal Status flags. */
    int     iob_unget;		/*!< \internal Unget buffer. */
};

/*@}*/


extern int _fmode(CONST char *mode);

extern int _putf(int _putb(int, CONST void *, size_t), int fd, CONST char *fmt, va_list ap);
extern int _getf(int _getb(int, void *, size_t), int fd, CONST char *fmt, va_list ap);

#endif
