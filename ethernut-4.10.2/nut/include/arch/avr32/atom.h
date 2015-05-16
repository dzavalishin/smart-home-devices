/*!
 * Copyright (C) 2001-2010 by egnite Software GmbH
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
 * $Log: atom.h,v $
 *
 */

/*
	WHEN CHANGING PLEASE NOTICE:
	Errata 41.5.5.5 reads:
	"Need two NOPs instruction after instructions masking interrupts
	The instructions following in the pipeline the instruction masking the interrupt through SR
	may behave abnormally.
	Fix/Workaround
	Place two NOPs instructions after each SSRF or MTSR instruction setting IxM or GM in SR."
*/

#ifndef _SYS_ATOM_H_
#error "Do not include this file directly. Use sys/atom.h instead!"
#endif

#if (__GNUC__ && __AVR32__)

#include <avr32/io.h>

#if defined(NUT_CRITICAL_NESTING)

#if defined(NUT_CRITICAL_NESTING_STACK)

// This doesn't work right now. Not sure how to fix yet.
// but this is unreacheable since critical neasting can't be enabled in the configurator
// for AVR32
#error Not Working
#define NutEnterCritical()               \
{                                        \
	register int temp_;                  \
	__asm__ volatile (                       \
	"mfsr    r10, %[SR]"                "\n\t" \
	"pushm   r10"     "\n\t"        \
	"ssrf    %[SR_GM_OFFSET]"     "\n\t"        \
	: [temp] "=r" (temp_) \
	: [SR] "i" (AVR32_SR), \
	  [SR_GM_OFFSET] "i" (AVR32_SR_GM_OFFSET) \
	: "memory", "cc", "r10"); \
}

#define NutExitCritical() \
{ \
	register int temp_; \
	__asm__ volatile (             \
	"popm    r10"                                     "\n\t" \
	"andl    r10, LO(%[SR_GM_MASK])"               "\n\t" \
	"andh    r10, HI(%[SR_GM_MASK])"               "\n\t" \
	"cp.w    r10, 0"                               "\n\t" \
	"breq   LABEL_INT_SKIP_ENABLE_INTERRUPTS_%[LINE]" "\n\t" \
	"csrf   %[SR_GM_OFFSET]"                          "\n\t" \
	"LABEL_INT_SKIP_SAVE_CONTEXT_%[LINE]:"            "\n\t" \
	: [temp] "=r" (temp_)                                    \
	: [SR_GM_OFFSET] "i" (AVR32_SR_GM_OFFSET),               \
	  [SR_GM_MASK]   "i" (AVR32_SR_GM_MASK),                 \
	  [LINE] "i" (__LINE__)                                  \
	: "memory", "cc", "r10");                                \
}

#else /* NUT_CRITICAL_NESTING_STACK */

// If we ever enable critical nesting, implement me.
#error Not implemented
extern unsigned int critical_nesting_level;

#define NutEnterCritical()

#define NutExitCritical()

#endif /* NUT_CRITICAL_NESTING_STACK */

#else /* NUT_CRITICAL_NESTING */

#define NutEnterCritical() \
{                                 \
	__asm__ __volatile__ (        \
	"ssrf\t%0"      "\n\t"        \
	"nop"		    "\n\t"        \
	"nop"	        "\n\t"        \
	:: "i" (AVR32_SR_GM_OFFSET)   \
	: "memory");				  \
}

#define NutExitCritical()         \
{                                 \
	__asm__ __volatile__ (        \
	"csrf\t%0"      "\n\t"        \
    :: "i" (AVR32_SR_GM_OFFSET)   \
	: "memory");				  \
}

#endif /* NUT_CRITICAL_NESTING */

#define NutJumpOutCritical()    NutExitCritical()

#endif
