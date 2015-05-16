#ifndef _DEV_IRQSTACK_H_
#define _DEV_IRQSTACK_H_

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
 */

/*
 * $Log$
 * Revision 1.6  2005/09/17 09:22:53  drsung
 * More program space saving implementation
 *
 * Revision 1.5  2005/02/05 20:39:43  haraldkipp
 * Force compiler error for leftover debug prints.
 *
 * Revision 1.4  2004/09/22 08:15:56  haraldkipp
 * Speparate IRQ stack configurable
 *
 * Revision 1.3  2004/04/25 17:06:17  drsung
 * Separate IRQ stack now compatible with nested interrupts.
 *
 * Revision 1.2  2004/02/03 11:28:41  drsung
 * Modified to support different target platforms.
 *
 * Revision 1.1  2004/01/30 17:00:46  drsung
 * Separate interrupt stack for avr-gcc only added.
 *
 */

#include <cfg/dev.h>

#ifdef IRQSTACK_SIZE

#include <sys/types.h>

#define _irq_prolog \
    asm volatile ("push r24" "\n\t"				/* save r24 to current stack */ \
                  "push r25" "\n\t"				/* save r25 to current stack */ \
                  "in r24,__SREG__" "\n\t"		/* load SREG into r24 */ \
                  "push r24" "\n\t");			/* and push it to current stack */ 

#define _irq_epilog \
    asm volatile ("pop r24" "\n\t"				/* restore r24 from stack */ \
                  "out __SREG__, r24" "\n\t"	/* write it to SREG */ \
                  "pop r25" "\n\t"				/* load byte from stack to r25 */ \
                  "pop r24" "\n\t");			/* load byte from stack to r24 */

#define NUTSIGNAL(signame,handler)		\
void signame (void) __attribute__ ((naked));	\
void signame (void)	\
{				\
   _irq_prolog \
   asm volatile ("ldi r24, lo8(%0)" "\n\t" \
                 "ldi r25, hi8(%0)" "\n\t" \
                 "jmp _irq_interrupt" "\n\t" \
                 :: "i" (&handler)); \
}

#else                           /* IRQSTACK_SIZE */


#define NUTSIGNAL(signame,handler)	\
SIGNAL(signame)		\
{ CallHandler (&handler);  }

#endif                          /* !IRQSTACK_SIZE */

#endif
