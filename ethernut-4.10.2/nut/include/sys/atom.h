#ifndef _SYS_ATOM_H_
#define _SYS_ATOM_H_

/*
 * Copyright (C) 2001-2005 by egnite Software GmbH. All rights reserved.
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
 * Revision 1.17  2009/03/05 22:16:57  freckle
 * use __NUT_EMULATION instead of __APPLE__, __linux__, or __CYGWIN__
 *
 * Revision 1.16  2008/08/11 07:00:24  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.15  2005/07/26 15:49:59  haraldkipp
 * Cygwin support added.
 *
 * Revision 1.14  2005/06/06 10:44:42  haraldkipp
 * Header files moved to re-enable building apart from source tree.
 *
 * Revision 1.13  2005/05/27 17:42:27  drsung
 * Platform dependant files were moved to /arch directory.
 *
 * Revision 1.12  2005/05/26 15:47:50  drsung
 * Fixed cvs commit comment from last commit. The
 * combination / and * made some compilers to produce errors... (ugh).
 * Thanks to Daniel Hobi who discovered this.
 *
 * Revision 1.11  2005/05/26 10:10:03  drsung
 * Moved all platform dependend code from this file to /include/arch/xxx_atom.h
 *
 * Revision 1.10  2005/05/13 21:47:07  chaac
 * Entering a critical section should now be faster on AVR's when
 * calling NutEnterCritical().
 *
 * Revision 1.9  2005/04/05 17:42:45  haraldkipp
 * ARM7 implementation of critical sections added.
 *
 * Revision 1.8  2005/02/21 12:37:59  phblum
 * Removed tabs and added semicolons after NUTTRACER macros
 *
 * Revision 1.7  2005/02/16 19:51:15  haraldkipp
 * Enable tracer configuration.
 *
 * Revision 1.6  2005/02/10 07:06:48  hwmaier
 * Changes to incorporate support for AT90CAN128 CPU
 *
 * Revision 1.5  2005/01/24 22:34:46  freckle
 * Added new tracer by Phlipp Blum <blum@tik.ee.ethz.ch>
 *
 * Revision 1.4  2004/04/07 12:13:57  haraldkipp
 * Matthias Ringwald's *nix emulation added
 *
 * Revision 1.3  2004/03/17 11:30:22  haraldkipp
 * Bugfix for ICCAVR
 *
 * Revision 1.2  2004/03/16 16:48:28  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 * Revision 1.1.1.1  2003/05/09 14:41:18  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.6  2003/05/06 17:53:56  harald
 * Force critical inline
 *
 * Revision 1.5  2003/02/04 18:00:51  harald
 * Version 3 released
 *
 * Revision 1.4  2002/06/26 17:29:28  harald
 * First pre-release with 2.4 stack
 *
 */

#include <cfg/os.h>
#include <sys/types.h>
#include <stdint.h>

#ifdef NUTTRACER_CRITICAL
#include <sys/tracer.h>
#endif

/*
 * The folowing macros must at least be defined for each supported platform.
 *
 * NutEnterCritical()
 * Starts a critical section.
 * The global interrupt flag must be disabled to protect the folowing code from 
 * interruption. The state of the global interrupt flag must be saved, to 
 * later restore.
 *
 * NutExitCritical()
 * Finalize a critical section. Must be used only at the end of a critical section 
 * to restore the global interrupt flag to the state saved by NutEnterCritical()
 *
 * NutJumpOutCritical()
 * Macro for early leaving the critical section. Must be used in the middle on a 
 * critical section if you want to terminate the critical section.
 *
 */

#ifdef __NUT_EMULATION__
#include <arch/unix/atom.h>
#elif defined(__AVR__)
#include <arch/avr/atom.h>
#elif defined(__arm__)
#include <arch/arm/atom.h>
#elif defined(__AVR32__)
#include <arch/avr32/atom.h>
#elif defined(__H8300H__) || defined(__H8300S__)
#include <arch/h8300h/atom.h>
#elif defined(__m68k__)
#include <arch/m68k/atom.h>
#endif

#endif
