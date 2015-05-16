#ifndef _COMPILER_H_
#define _COMPILER_H_

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
 * Revision 1.12  2009/03/05 22:16:57  freckle
 * use __NUT_EMULATION instead of __APPLE__, __linux__, or __CYGWIN__
 *
 * Revision 1.11  2005/08/02 17:46:47  haraldkipp
 * Major API documentation update.
 *
 * Revision 1.10  2005/07/26 15:49:59  haraldkipp
 * Cygwin support added.
 *
 * Revision 1.9  2005/02/10 07:06:50  hwmaier
 * Changes to incorporate support for AT90CAN128 CPU
 *
 * Revision 1.8  2004/04/07 12:13:57  haraldkipp
 * Matthias Ringwald's *nix emulation added
 *
 * Revision 1.7  2004/03/18 15:51:45  haraldkipp
 * ICCAVR failed to compile
 *
 * Revision 1.6  2004/03/16 16:48:27  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 * Revision 1.5  2004/02/01 18:49:47  haraldkipp
 * Added CPU family support
 *
 */

#ifdef  __cplusplus
# define __BEGIN_DECLS  extern "C" {
# define __END_DECLS    }
#else
# define __BEGIN_DECLS
# define __END_DECLS
#endif

#ifdef __NUT_EMULATION__
#include <arch/unix.h>
#elif defined(__AVR__) || defined(ATMEGA)
#include <arch/avr.h>
#elif defined(__arm__)
#include <arch/arm.h>
#elif defined(__AVR32__)
#include <arch/avr32.h>
#elif defined(__H8300__) || defined(__H8300H__) || defined(__H8300S__)
#include <arch/h8.h>
#elif defined(__m68k__)
#include <arch/m68k.h>
#endif

// Platform independent compiler macros
#ifndef NUT_DEPRECATED
#ifdef __GNUC__
#define NUT_DEPRECATED __attribute__ ((deprecated))
#else
// Fallback
#define NUT_DEPRECATED
#endif
#endif

#endif // _COMPILER_H_
