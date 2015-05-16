#ifndef ARCH_ARM_PTRACE_H
#define ARCH_ARM_PTRACE_H
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
 * \file include/arch/arm/ptrace.h
 *
 * \verbatim
 * $Id: ptrace.h 2400 2009-01-16 19:44:05Z haraldkipp $
 * \endverbatim
 */

/*
 * Names are identical, so that this 'plays nicely with' linux
 * documentation and other associated code.
 */



#define PTRACE_R0_idx        0
#define PTRACE_R1_idx        1
#define PTRACE_R2_idx        2
#define PTRACE_R3_idx        3
#define PTRACE_R4_idx        4
#define PTRACE_R5_idx        5
#define PTRACE_R6_idx        6
#define PTRACE_R7_idx        7
#define PTRACE_R8_idx        8
#define PTRACE_R9_idx        9
#define PTRACE_R10_idx       10
#define PTRACE_R11_idx       11
#define PTRACE_R12_idx       12
#define PTRACE_R13_idx       13
#define PTRACE_SP_idx        PTRACE_R13_idx
#define PTRACE_R14_idx       14
#define PTRACE_LR_idx        PTRACE_R14_idx
#define PTRACE_R15_idx       15
#define PTRACE_PC_idx        PTRACE_R15_idx
#define PTRACE_CPSR_idx      16
#define PTRACE_R0_retval_idx 17
#define PTRACE_FRAMETYPE_idx 18
#define PTRACE_SWICODE_idx   19
#define PTRACE_FRAME_size    20 /* in bytes */

#define ARM_CPSR_V_BIT   (1<<28)
#define ARM_CPSR_C_BIT   (1<<29)
#define ARM_CPSR_Z_BIT   (1<<30)
#define ARM_CPSR_N_BIT   (1<<31)
#define ARM_USR_MODE     (0x10) /**< no priv mode */
#define ARM_FIQ_MODE     (0x11) /**< fiq occured */
#define ARM_IRQ_MODE     (0x12) /**< irq occured */
#define ARM_SVC_MODE     (0x13) /**< swi occured */
#define ARM_ABT_MODE     (0x17) /**< pfa or udf  */
#define ARM_UND_MODE     (0x1b) /**< bad opcode  */
#define ARM_SYS_MODE     (0x1f) /**< normal supervisor */
#ifndef ARM_MODE_MASK
#define ARM_MODE_MASK    (0x1f)
#endif



#define PTRACE_FRAME_isirq(X)   ( (X)->uregs[ PTRACE_FRAMETYPE ] < 32 )
#define PTRACE_FRAMETYPE_reset   32 /* in vector order */
#define PTRACE_FRAMETYPE_udf     33
#define PTRACE_FRAMETYPE_swi     34
#define PTRACE_FRAMETYPE_pfa     35
#define PTRACE_FRAMETYPE_da      36
#define PTRACE_FRAMETYPE_notused 37
/* #define PTRACE_FRAMETYPE_irq is above, ie: 0..32 for each irq vector */
#define PTRACE_FRAMETYPE_fiq     38
#define PTRACE_FRAMETYPE_thread  39 /* thread saved state */

#if __ASSEMBLER__
/* nothing here */
#else 

#if __cplusplus
extern "C" {
#endif


struct pt_regs {
  long uregs[ 20 ];
};

void ARM_UDF_Handler( struct pt_regs  *p );
void ARM_SWI_Handler( struct pt_regs  *p );
void ARM_PFA_Handler( struct pt_regs  *p );
void ARM_DA_Handler ( struct pt_regs  *p );
void ARM_IRQ_Handler( struct pt_regs  *p );
void ARM_FIQ_Handler( struct pt_regs  *p );
void ARM_NOTUSED_Handler( struct pt_regs  *p );
void ARM_COMMON_Handler_crash( struct pt_regs  *p, const char *name );

#if __cplusplus
}
#endif

#endif /* __ASSEMBLER__ */

#endif /* CPU_ARM_PTRACE_H */
