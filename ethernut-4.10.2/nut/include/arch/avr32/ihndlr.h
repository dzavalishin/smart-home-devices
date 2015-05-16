#ifndef _ARCH_AVR32_INTERRUPT_HANDLER_H_
#define _ARCH_AVR32_INTERRUPT_HANDLER_H_

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
* $Log: ihndlr.h,v $
*
*/

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Nut/OS API.  It exists for the convenience
// of Interrupt Handling.  This header file may change from version
// to version without notice, or even be removed.
//
// We mean it.
//
//

//! Pointer to interrupt handler.
#if __GNUC__
typedef void (*__int_handler)(void);
#elif __ICCAVR32__
typedef void (__interrupt *__int_handler)(void);
#endif

extern void init_interrupts(void);

extern void register_interrupt(__int_handler handler, unsigned int irq, unsigned int int_lev);

/*
	WHEN CHANGING PLEASE NOTICE:
	Errata 41.5.5.5 reads:
	"Need two NOPs instruction after instructions masking interrupts
	The instructions following in the pipeline the instruction masking the interrupt through SR
	may behave abnormally.
	Fix/Workaround
	Place two NOPs instructions after each SSRF or MTSR instruction setting IxM or GM in SR."
*/
#define IRQ_ENTRY()																			\
{																							\
	/* Prevent preempted interrupts disabling global interrupts */							\
	__asm__ __volatile__ ("ssrf\t%0; nop; nop" :: "i" (AVR32_SR_GM_OFFSET) : "memory");		\
																							\
	/* Save R0..R7. Other registers are saved by the CPU if __AVR32_UC__ */					\
	/* Or by exception.S if __AVR32_AP7000__ */												\
	__asm__ __volatile__ ("pushm r0-r7");													\
}


#define IRQ_EXIT()																			\
{																							\
	/* Restore R0..R7 */																	\
	__asm__ __volatile__ ("popm r0-r7");													\
																							\
	/* Prevent preempted interrupts disabling global interrupts */							\
	__asm__ __volatile__ ("csrf\t%0" :: "i" (AVR32_SR_GM_OFFSET) : "memory");				\
}

#endif // _ARCH_AVR32_INTERRUPT_HANDLER_H_
