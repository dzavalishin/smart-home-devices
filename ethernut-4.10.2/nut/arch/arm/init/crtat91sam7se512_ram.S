/*
 * Copyright (C) 2005-2008 by egnite Software GmbH. All rights reserved.
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
 *
 * NOTE: This is the corrected version for debugging in RAM.
 *       The remapping, SRAM to 0x00000000, must be done by the 
 *       debugger script. Relocating of .data section is removed too.
 *       (20.03.2011, Michael Fischer)
 */

#include <cfg/clock.h>
#include <cfg/memory.h>

#include <arch/arm.h>

#ifndef NUTMEM_FLASH_RWS
/* By default use 1 wait state for flash read access. */
#define NUTMEM_FLASH_RWS    1
#endif

#if NUTMEM_FLASH_RWS == 0
#define MC_FWS_VAL  MC_FWS_1R2W /* 1 cycle for read, 2 for write ops. */
#elif NUTMEM_FLASH_RWS == 1
#define MC_FWS_VAL  MC_FWS_2R3W /* 2 cycles for read, 3 for write ops. */
#elif NUTMEM_FLASH_RWS == 2
#define MC_FWS_VAL  MC_FWS_3R4W /* 3 cycles for read, 4 for write ops. */
#else
#define MC_FWS_VAL  MC_FWS_4R4W /* 4 cycles for read and write ops. */
#endif

#ifndef IRQ_STACK_SIZE
#define IRQ_STACK_SIZE  512
#endif

#ifndef FIQ_STACK_SIZE
#define FIQ_STACK_SIZE  256
#endif

#ifndef ABT_STACK_SIZE
#define ABT_STACK_SIZE  128
#endif

#ifndef UND_STACK_SIZE
#define UND_STACK_SIZE  128
#endif


/*
 * Section 0: Vector table and reset entry.
 */
        .section .init0,"ax",%progbits

        .global __vectors
__vectors:
        ldr     pc, [pc, #24]   /* Reset */
        ldr     pc, [pc, #24]   /* Undefined instruction */
        ldr     pc, [pc, #24]   /* Software interrupt */
        ldr     pc, [pc, #24]   /* Prefetch abort */
        ldr     pc, [pc, #24]   /* Data abort */
        ldr     pc, [pc, #24]   /* Reserved */

        /*
         * On IRQ the PC will be loaded from AIC_IVR, which
         * provides the address previously set in AIC_SVR.
         * The interrupt routine will be called in ARM_MODE_IRQ
         * with IRQ disabled and FIQ unchanged.
         */
        ldr     pc, [pc, #-0xF20]   /* Interrupt request, auto vectoring. */
        ldr     pc, [pc, #-0xF20]   /* Fast interrupt request, auto vectoring. */

        .word   _start
        .word   __undef
        .word   __swi
        .word   __prefetch_abort
        .word   __data_abort

        .weak   __undef
        .set    __undef, __xcpt_dummy
        .weak   __swi
        .set    __swi, __xcpt_dummy
        .weak   __prefetch_abort
        .set    __prefetch_abort, __xcpt_dummy
        .weak   __data_abort
        .set    __data_abort, __xcpt_dummy

        .global __xcpt_dummy
__xcpt_dummy:
        b       __xcpt_dummy

        .ltorg
/*
 * Section 1: Hardware initialization.
 */
        .section .init1, "ax", %progbits
        .globl	_start
_start:

        /*
         * Set cycles for flash access.
         */
        ldr     r1, =MC_BASE
        mov     r0, #MC_FWS_VAL
        str     r0, [r1, #MC_FMR_EFC0_OFF]
        str     r0, [r1, #MC_FMR_EFC1_OFF]

        /*
         * Disable all interrupts. Useful for debugging w/o target reset.
         */
        ldr     r1, =AIC_BASE
        mvn     r0, #0
        str     r0, [r1, #AIC_EOICR_OFF]
        str     r0, [r1, #AIC_IDCR_OFF]

        /*
         * The watchdog is enabled after processor reset. Disable it.
         */
        ldr     r1, =WDT_BASE
        ldr     r0, =WDT_WDDIS
        str     r0, [r1, #WDT_MR_OFF]

        /*
         * Enable external reset key.
         */
        ldr     r0, =(RSTC_KEY | RSTC_URSTEN)
        ldr     r1, =RSTC_MR
        str     r0, [r1, #0]
        
        b       __set_stacks

        .ltorg


/*
 * Section 2: Set stack pointers.
 */
        .section .init2,"ax",%progbits        
        .global __set_stacks
__set_stacks:

        /*
         * Set exception stack pointers and enable interrupts.
         */
        ldr     r0, =__exp_stack
        msr     CPSR_c, #ARM_MODE_FIQ | ARM_CPSR_I_BIT | ARM_CPSR_F_BIT
        mov     r13, r0
        sub     r0, r0, #FIQ_STACK_SIZE
        msr     CPSR_c, #ARM_MODE_IRQ | ARM_CPSR_I_BIT | ARM_CPSR_F_BIT
        mov     r13, r0
        sub     r0, r0, #IRQ_STACK_SIZE
        msr     CPSR_c, #ARM_MODE_ABORT | ARM_CPSR_I_BIT | ARM_CPSR_F_BIT
        mov     r13, r0
        sub     r0, r0, #ABT_STACK_SIZE
        msr     CPSR_c, #ARM_MODE_UNDEF | ARM_CPSR_I_BIT | ARM_CPSR_F_BIT
        mov     r13, r0
        sub     r0, r0, #UND_STACK_SIZE
        msr     CPSR_c, #ARM_MODE_SVC | ARM_CPSR_I_BIT | ARM_CPSR_F_BIT
        mov     r13, r0
        b       __enter_mode
        
        .ltorg

/*
 * Section 3: Enter system mode.
 */
        .section .init3,"ax",%progbits

        .global __enter_mode
__enter_mode:
        msr     CPSR_c, #ARM_MODE_SYS | ARM_CPSR_I_BIT | ARM_CPSR_F_BIT
        b       __clear_bss

        .ltorg

/*
 * Section 4: Clear bss and copy data.
 */
        .section .init4,"ax",%progbits
        .global __clear_bss
__clear_bss:

        ldr     r1, =__bss_start
        ldr     r2, =__bss_end
        ldr     r3, =0

_40:
        cmp     r1, r2
        strne   r3, [r1], #+4
        bne     _40

        /*
         * Initialize user stack pointer.
         */
        ldr     r13, =__stack
        b       __call_rtos

        .ltorg

/*
 * Section 5: Call RTOS
 */
        .section .init5,"ax",%progbits
        .global __call_rtos
__call_rtos:

        /*
         * Jump to Nut/OS initialization.
         */
        ldr     r0, =NutInit
        bx      r0

End:
        b       End

        .ltorg
