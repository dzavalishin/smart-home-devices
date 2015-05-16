        .nolist
        .psize      0
/*
 * Copyright (C) 2005 by egnite Software GmbH
 * Copyright (C) 2011 by egnite GmbH
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
 * $Id$
 */

#include <arch/arm.h>

        .list

/*
 * This file contains the runtime initialization for AT91X40 based systems.
 * It replaces the older crtat91_ram.S, which is now deprecated.
 *
 * The related linker file is at91x40_ram.ld.
 *
 * All code and all data is located in internal RAM and may be used with
 * a boot loader or debugger, where the remapping must take place. It is
 * assumed, that this code is loaded at address 0x00000000.
 *
 * Tested on AT91R40008 only.
 */

/*
 * Stack sizes.
 *
 * Interrupt nesting is currently not supported. Therefore dedicated
 * stacks are used for all exceptions.
 */
#ifndef INI_STACK_SIZE
#define INI_STACK_SIZE      (128 * 4)
#endif

#ifndef IRQ_STACK_SIZE
#define IRQ_STACK_SIZE      (128 * 4)
#endif

#ifndef FIQ_STACK_SIZE
#define FIQ_STACK_SIZE      (64 * 4)
#endif

#ifndef EXC_STACK_SIZE
#define EXC_STACK_SIZE      (32 * 4)
#endif

#define TOTAL_STACK_SIZE    (INI_STACK_SIZE + IRQ_STACK_SIZE + FIQ_STACK_SIZE + EXC_STACK_SIZE)

        /* We start in ARM mode. */
        .arm

/*
 * Vector Section: Must be located at address 0.
 */
        .section .vectors,"ax",%progbits

        .global __vectors
__vectors:
        /* Reset entry. */
        b       _start
        /* Undefined instruction exception entry. */
        ldr     pc, [pc, #(5 * 4)]
        /* Software interrupt entry. */
        ldr     pc, [pc, #(5 * 4)]
        /* Prefetch abort exception entry. */
        ldr     pc, [pc, #(5 * 4)]
        /* Data abort exception entry. */
        ldr     pc, [pc, #(5 * 4)]
        /*
         * Reserved entry, used by some boot loaders to store the
         * size of the binary image.
         */
        .word   0

        /*
         * On IRQ/FIQ the PC will be loaded from AIC_IVR, which
         * provides the address previously set in AIC_SVR().
         * The interrupt routine will be called in ARM_MODE_IRQ
         * with IRQ disabled and FIQ unchanged.
         */
        ldr     pc, [pc, #-0xF20]
        ldr     pc, [pc, #-0xF20]

        /*
         * Exceptions are weakly linked to endless loops.
         *
         * This may look a bit confusing. The following words contain
         * the jump addresses that are loaded at each entry, see above.
         * They contain weakly defined addresses pointing to executable
         * code. Each of them may be overridden by the operating system
         * or the application, if a function with the same name exist.
         * By default they simply point to endless loops.
         *
         * We intentionally use a dedicated loop with a global label
         * for each exception. If an unhandled exception occurs, we
         * can use a simple debugger (like OpenOCD) to retrieve the
         * current program counter and check the linker map to determine
         * the type of the exception.
         */
        .word   __undef
        .word   __swi
        .word   __prefetch_abort
        .word   __data_abort

        .weak   __undef, __swi, __prefetch_abort, __data_abort
        .set    __undef, __undef_stop
        .set    __swi, __swi_stop
        .set    __prefetch_abort, __prefetch_abort_stop
        .set    __data_abort, __data_abort_stop

/*
 * Init Section 0: Exception Dummies.
 */
        .section .init0,"ax",%progbits

__undef_stop:
        b       __undef_stop
__swi_stop:
        b       __swi_stop
__prefetch_abort_stop:
        b       __prefetch_abort_stop
__data_abort_stop:
        b       __data_abort_stop

        /* Make sure that the literal pool is empty before the section ends. */
        .ltorg

/*
 * User Init Section 0
 *
 * Additional code may be added here by placing functions in
 * section .init0.user.
 */

/*
 * Init Section 1: Empty, remapping is done in the loader.
 *
 * Remapping is done in the loader.
 */
        .section .init1,"ax",%progbits

/*
 * User Init Section 1
 *
 * Additional code may be added here by placing functions in
 * section .init1.user. With code running in RAM this is
 * actually the same as .init0.user.
 */
        .global _start
_start:

/*
 * Init Section 2: Set stack pointers and initialize C variables.
 */
        .section .init2,"ax",%progbits
        /*
         * Set stack pointers with disabled interrupts.
         */
        ldr     r0, =__stack

        msr     CPSR_c, #ARM_MODE_SVC | ARM_CPSR_I_BIT | ARM_CPSR_F_BIT
        mov     sp, r0
        sub     r0, r0, #INI_STACK_SIZE

        msr     CPSR_c, #ARM_MODE_FIQ | ARM_CPSR_I_BIT | ARM_CPSR_F_BIT
        mov     sp, r0
        sub     r0, r0, #FIQ_STACK_SIZE

        msr     CPSR_c, #ARM_MODE_IRQ | ARM_CPSR_I_BIT | ARM_CPSR_F_BIT
        mov     sp, r0
        sub     r0, r0, #IRQ_STACK_SIZE

        msr     CPSR_c, #ARM_MODE_ABORT | ARM_CPSR_I_BIT | ARM_CPSR_F_BIT
        mov     sp, r0

        msr     CPSR_c, #ARM_MODE_UNDEF | ARM_CPSR_I_BIT | ARM_CPSR_F_BIT
        mov     sp, r0

        /*
         * Nut/OS system and application are running in system mode.
         * Note, that we re-use the supervisory stack.
         */
        msr     CPSR_c, #ARM_MODE_SYS | ARM_CPSR_I_BIT | ARM_CPSR_F_BIT
        ldr     r0, =__stack
        mov     sp, r0

        /*
         * Clear bss.
         */
        ldr     r1, =__bss_start
        ldr     r2, =__bss_end
        ldr     r3, =0
1:      cmp     r1, r2
        strne   r3, [r1], #+4
        bne     1b

        /* Jump over the literal pool. */
        b       _init_hw
        /* Make sure that the literal pool is empty before the section ends. */
        .ltorg

/*
 * User Init Section 2
 *
 * Additional code may be added here by placing functions in
 * section .init2.user.
 *
 * This section may be used for early hardware initialization
 * routines written in C.
 */

_init_hw:

/*
 * Init Section 3: Hardware initialization.
 */
        .section .init3,"ax",%progbits

        /*
         * Enable all clocks.
         */
        mvn     r0, #0
        ldr     r1, =PS_BASE
        str     r0, [r1, #(PS_PCER - PS_BASE)]

        /*
         * Initialize the interrupt controller.
         */
        add     r0, pc,#-(8 + . - __aic_table)
        ldmia   r0, {r1-r4}
        str     r4, [r1, #AIC_SPU]

        mov     r0, #8
1:      str     r1, [r1, #AIC_EOICR]
        subs    r0, r0, #1
        bhi     1b

        str     r2, [r1, #AIC_SVR(0)]
        add     r1, r1, #AIC_SVR(0)
        mov     r0, #31
1:      str     r3, [r1, r0, LSL #2]
        subs    r0, r0, #1
        bhi     1b

        b       _enter_rtos

__aic_table:
        .word   AIC_BASE
        .word   _irq_dummy
        .word   _irq_dummy
        .word   _irq_dummy

        /* Interrupt dummy. */
        .global _irq_dummy
_irq_dummy:
        b       _irq_dummy

        /* Make sure that the literal pool is empty before the section ends. */
        .ltorg


/*
 * User Init Section 3
 *
 * Additional code may be added here by placing functions in
 * section .init3.user.
 *
 * This section may be used for additional hardware initialization
 * routines written in C. Native interrupt routines may be used.
 */
_enter_rtos:

/*
 * Init Section 4: Enter system.
 */
        .section .init4,"ax",%progbits

        /* Jump to Nut/OS initialization. */
        ldr     r0, =NutInit
        mov     lr, pc
        bx      r0


/*
 * Exit Section 0: Endless loop.
 */
        .section .exit0,"ax",%progbits
1:      b       1b

/*
 * Stack Section.
 */
	.section .stack, "w", %nobits
        .space  TOTAL_STACK_SIZE
