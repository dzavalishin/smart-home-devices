#ifndef _NET_ERRNO_H_
#define _NET_ERRNO_H_

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
 * -
 * Portions Copyright (c) 1983, 1993 by
 *  The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * $Log$
 * Revision 1.5  2009/02/22 12:26:59  olereinhardt
 * 2009-02-22  Ole Reinhardt <ole.reinhardt@thermotemp.de>
 *
 * Added a deprecated warning.
 * Commended out all defines and include include/errno.h instead.
 * All defines in this file are duplicates of the one defined in
 * include/errno.h, just with an offset of 10000. We don't need these
 * defines and just get conflicts. So remove this file ASAP
 *
 * Revision 1.4  2004/05/10 19:17:17  freckle
 * Only define constants if they are not already defined
 * This is a fix to get the *nix emulation to compile on mac os x, but the
 * file is propably obsolete anyway
 *
 * Revision 1.3  2004/04/14 18:30:58  freckle
 * This is Revision 1.1.1.1 of errno.h. Revision 1.2 was checked in accidently
 *
 * Revision 1.1.1.1  2003/05/09 14:41:11  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.5  2003/02/04 18:00:42  harald
 * Version 3 released
 *
 * Revision 1.4  2002/06/26 17:29:18  harald
 * First pre-release with 2.4 stack
 *
 */

/*!
 * \file net/errno.h
 * \brief Network error definitions.
 */
#include <errno.h>
#warning Using this file is deprecated, just include <errno.h> instead

/* These definitions won't be needed any more... We should remove this deprecated file ASAP
#ifndef EINVAL

#define EINVAL          22      //!< Invalid argument


#define EWOULDBLOCK     10035   //!< Operation would block
#define EINPROGRESS     10036   //!< Operation now in progress
#define EALREADY        10037   //!< Operation already in progress
#define ENOTSOCK        10038   //!< Socket operation on non-socket
#define EDESTADDRREQ    10039   //!< Destination address required
#define EMSGSIZE        10040   //!< Message too long
#define EPROTOTYPE      10041   //!< Protocol wrong type for socket
#define ENOPROTOOPT     10042   //!< Protocol not available
#define EPROTONOSUPPORT 10043   //!< Protocol not supported
#define ESOCKTNOSUPPORT 10044   //!< Socket type not supported
#define EOPNOTSUPP      10045   //!< Operation not supported on socket
#define EPFNOSUPPORT    10046   //!< Protocol family not supported
#define EAFNOSUPPORT    10047   //!< Address family not supported by protocol family
#define EADDRINUSE      10048   //!< Address already in use
#define EADDRNOTAVAIL   10049   //!< Can't assign requested address
#define ENETDOWN        10050   //!< Network is down
#define ENETUNREACH     10051   //!< Network is unreachable
#define ENETRESET       10052   //!< Network dropped connection on reset
#define ECONNABORTED    10053   //!< Software caused connection abort
#define ECONNRESET      10054   //!< Connection reset by peer
#define ENOBUFS         10055   //!< No buffer space available
#define EISCONN         10056   //!< Socket is already connected
#define ENOTCONN        10057   //!< Socket is not connected
#define ESHUTDOWN       10058   //!< Can't send after socket shutdown
#define ETOOMANYREFS    10059   //!< Too many references: can't splice
#define ETIMEDOUT       10060   //!< Connection timed out
#define ECONNREFUSED    10061   //!< Connection refused
#define ELOOP           10062   //!< Too many levels of symbolic links
#define ENAMETOOLONG    10063   //!< File name too long
#define EHOSTDOWN       10064   //!< Host is down
#define EHOSTUNREACH    10065   //!< No route to host
#define ENOTEMPTY       10066   //!< Directory not empty

#endif
#endif
*/ 
#endif
