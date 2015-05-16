/*
 * Copyright (C) 2008 by Duane Ellis
 *
 * All rights reserved.
 *
 * The original code had been released as part of the LoastARM Project 
 * under GPL Version 2 and is published here under the following license
 * with kind permission from the author:
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
 * For additional information see http://lostarm.sourceforge.net/
 */

/*!
 * \file include/arch/arm/exception_asm.h
 *
 * \verbatim
 * $Id: exception_asm.h 2400 2009-01-16 19:44:05Z haraldkipp $
 * \endverbatim
 */

 /* -*- mode: asm -*- */
#if  __ASSEMBLER__

.macro	ARM7TDMI_IRQ_BUG
	/** BUG - ARM7TDMI **/
	/* See:	 ATMEL doc DOC1156 */
	/* fix/adjust lr */
	sub	lr,lr,#4
	stmfd	sp!,{lr}
	mrs	lr,spsr

	/* BUG IS - due to pipeline the IRQ bit may be     */
	/* set right (disableing irqs) but it gets lost    */
	/* inside the pipeline, software must fix this     */
	ands	lr,lr,#ARM_CPSR_I_BIT
	ldmnefd	sp!,{pc}^
	ldmfd	sp!,{lr}
	/* undo the add above cause below we will do
	 * the sub again below it would be nicer to
	 * 'clean this up'
	 */
	add	lr,lr,#4
.endm


.macro  exception_enter, whatmode, frametype

	.if	((\whatmode) != ARM_SVC_MODE)
	sub	lr,lr,#4
	.endif
	/* switch to system mode */
	msr	cpsr_c,#(ARM_SYS_MODE | ARM_CPSR_F_BIT | ARM_CPSR_I_BIT)
	/* we are now in system mode */

	/* one fallacy - we always assume
	 * sysmode SP is not messed up
	 */
	
	/* make room for PTRACE frame */
	sub	sp,sp,#(PTRACE_FRAME_size*4)

	/* save the world */
	stmia	sp,{r0-r14}

	/* save the return value here */
	str	r0,[sp,#(PTRACE_R0_retval_idx*4)]
	
	/* Need to go back to old mode and pickup things 
	 * When we get there, we will need the sys stack
	 */
	mov	r0,sp

	/* step over to the dark side */
	msr	cpsr_c,#((\whatmode) | ARM_CPSR_F_BIT | ARM_CPSR_I_BIT)
	/* back in offending mode */

	/* save the exception address */
	str	lr,[r0,#(4*PTRACE_R15_idx)]

	/* And the saved PSR */
	mrs	r1,spsr
	str	r1,[r0,#(4*PTRACE_CPSR_idx)]
	/* and back to supervisor mode */
	msr	cpsr_c,#(ARM_SYS_MODE | ARM_CPSR_F_BIT | ARM_CPSR_I_BIT)
	/* done, w/ IRQ & FIQ disabled */
	/* APP can decide to re-enable if *IT* wants to */
	.if	((\frametype) != 0)
	mov	r1,#(\frametype)
	str	r1,[r0,#(4*PTRACE_FRAMETYPE_idx)]
	.endif
.endm

.macro	exception_exit, whatmode
	/*
	 * SP = the saved state
	 * we always restore via SYSMODE
	 */

	/* pre-position some values */
	ldr	r0,[sp,#(PTRACE_R15_idx*4)]
	ldr	r1,[sp,#(PTRACE_CPSR_idx*4)]

	/* go to the offending mode */
	msr	cpsr_c,#((\whatmode) | ARM_CPSR_F_BIT | ARM_CPSR_I_BIT)
	
	mov	lr,r0
	msr	spsr,r1

	/* back to the main mode */
	msr	cpsr_c,#(ARM_SYS_MODE | ARM_CPSR_F_BIT | ARM_CPSR_I_BIT)
	/* restore the world */
	ldmia	sp,{r0-r14}
	.if	((\whatmode) == ARM_SVC_MODE)
	ldr	r0,[sp,#(PTRACE_R0_retval_idx*4)]
	.endif
	add	sp,sp,#(PTRACE_FRAME_size*4)

	/* back to the offending mode */
	msr	cpsr_c,#((\whatmode) | ARM_CPSR_F_BIT | ARM_CPSR_I_BIT)
	/* and effect the return */
	movs	pc,lr

.endm

#endif
