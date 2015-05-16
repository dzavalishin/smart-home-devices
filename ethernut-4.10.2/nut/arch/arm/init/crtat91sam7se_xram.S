/*
 * Copyright (C) 2005-2008 by egnite Software GmbH
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
 *
 * This file contains the runtime initialization of the AT91SAM7SE
 * family for code loaded into external RAM, typically SDRAM. It
 * has been written for GNU binutils and definitely needs to be
 * rewritten for other toolchains.
 *
 * Loading and running code in external RAM may be mainly used for
 * debugging, because the executable can be loaded faster than
 * programming flash memory and the number of breakpoints is not
 * limited by the debug hardware. Another application is to use a boot
 * loader running in internal flash, which loads very large executables,
 * which won't fit in internal flash, from external flash memory, e.g.
 * serial flash or memory card.
 *
 * In any case, the loader, either the debug tool or the boot loader,
 * must properly initialize the hardware upfront. This may include
 * various clocks, SDRAM, watchdog timer, reset controller etc. This
 * initialization code is intentionally kept simple and will not do any
 * hardware initialization that could be done by the loader. As a
 * result, some options provided by the Configurator like watchdog, PLL
 * or SDRAM settings are ignored here, but may be honored by the loader.
 */

#include <cfg/clock.h>
#include <cfg/memory.h>

#include <arch/arm.h>

/*
 * By default, the internal RAM will be mapped to address zero and is
 * used for the exception vectors, RAM functions, exception stacks and
 * the system initialization stack.
 *
 * +-----------+ Top of internal RAM
 * | FIQ stack | (System initialization stack)
 * |           |
 * +-----------+
 * | ABT stack |
 * +-----------+
 * | UND stack |
 * +-----------+
 * | IRQ stack |
 * ~           ~
 * |           |
 * +-----------+ Top of relocation area (__reloc_end)
 * | RAM funct |
 * ~           ~
 * |           |
 * +-----------+
 * | Vectors   |
 * +-----------+ Address 0x00000000 (__reloc_start)
 *
 * System routines as well as application code are executed in system
 * mode. During system initialization, the related stack pointer is set
 * to the top of internal RAM, overlaying the fast interrupt stack, and
 * possibly any following exception stack. This is usually no problem,
 * because no interrupts will be enabled before entering the idle thread.
 * Threads will allocate their own stacks from heap memory, located in
 * external RAM.
 */

/*
 * Exception stack sizes.
 *
 * No explicit size is given for the interrupt stack. As long as there
 * are a few small RAM functions only, almost all internal RAM will be
 * available.
 *
 * The supervisory mode is currently not used in Nut/OS and no stack
 * space is allocated for this mode. System routines as well as
 */
#ifndef FIQ_STACK_SIZE
#define FIQ_STACK_SIZE  (128 * 4)
#endif
#ifndef ABT_STACK_SIZE
#define ABT_STACK_SIZE  (64 * 4)
#endif
#ifndef UND_STACK_SIZE
#define UND_STACK_SIZE  (64 * 4)
#endif
#ifndef SVC_STACK_SIZE
#define SVC_STACK_SIZE  (0 * 4)
#endif


/* =======================================================================
 * Vector table section
 * ======================================================================= */
        .section .vectors, "ax", %progbits
        .code   32

        /*
         * Note, that this section is implemented in relocatable code.
         * It will be loaded into and executed in external RAM and will
         * then be moved to remapped internal RAM.
         */
        ldr     pc, [pc, #24]   /* Reset */
        ldr     pc, [pc, #24]   /* Undefined instruction */
        ldr     pc, [pc, #24]   /* Software interrupt */
        ldr     pc, [pc, #24]   /* Prefetch abort */
        ldr     pc, [pc, #24]   /* Data abort */
        .word   0               /* Reserved */

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

        /*
         * We use weakly defined default handlers for exceptions. They
         * consist of endless loops and can be overridden by the appli-
         * cation. Although a single loop may serve all exceptions, we
         * intentionally use individual loops. This allows us to easily
         * determine the type of exception by halting the trapped CPU
         * and querying the program counter.
         */
        .weak   __undef
        .set    __undef, __undef_dummy
        .global __undef_dummy
__undef_dummy:
        b       __undef_dummy

        .weak   __swi
        .set    __swi, __swi_dummy
        .global __swi_dummy
__swi_dummy:
        b       __swi_dummy

        .weak   __prefetch_abort
        .set    __prefetch_abort, __prefetch_abort_dummy
        .global __prefetch_abort_dummy
__prefetch_abort_dummy:
        b       __prefetch_abort_dummy

        .weak   __data_abort
        .set    __data_abort, __data_abort_dummy
        .global __data_abort_dummy
__data_abort_dummy:
        b       __data_abort_dummy

        .ltorg


/* =======================================================================
 * Initialization section
 * ======================================================================= */
        .section .init0, "ax", %progbits
        .code   32

        /*
         * Weakly define the stack top at the end of internal RAM.
         */
        .weak   __stack
        .set    __stack, 0x00200000 + MCU_IRAM_SIZE

_start:
        /*
         * Disable all interrupts. Useful for debugging w/o target reset.
         */
        ldr     r1, =AIC_BASE
        mvn     r0, #0
        str     r0, [r1, #AIC_EOICR_OFF]
        str     r0, [r1, #AIC_IDCR_OFF]

        /*
         * Set exception stack pointers and enable interrupts.
         */
        ldr     r1, =__stack
        msr     CPSR_c, #ARM_MODE_FIQ | ARM_CPSR_I_BIT | ARM_CPSR_F_BIT
        mov     sp, r1
        sub     r0, r1, #FIQ_STACK_SIZE
        msr     CPSR_c, #ARM_MODE_ABORT | ARM_CPSR_I_BIT | ARM_CPSR_F_BIT
        mov     sp, r0
        sub     r0, r0, #ABT_STACK_SIZE
        msr     CPSR_c, #ARM_MODE_UNDEF | ARM_CPSR_I_BIT | ARM_CPSR_F_BIT
        mov     sp, r0
        sub     r0, r0, #UND_STACK_SIZE
        msr     CPSR_c, #ARM_MODE_SVC | ARM_CPSR_I_BIT | ARM_CPSR_F_BIT
        mov     sp, r0
        sub     r0, r0, #SVC_STACK_SIZE
        msr     CPSR_c, #ARM_MODE_IRQ | ARM_CPSR_I_BIT | ARM_CPSR_F_BIT
        mov     sp, r0

        /*
         * Enter system mode and initialize the system stack pointer.
         * The stack will use the same memory as the exception stacks
         * that had been setup above. It will be used only until the
         * system enters the idle thread.
         */
        msr     CPSR_c, #ARM_MODE_SYS | ARM_CPSR_I_BIT | ARM_CPSR_F_BIT
        mov     sp, r1

        /*
         * Remap internal RAM to address zero.
         */
        mov     r0, #1
        ldr     r1, =MC_BASE
        str     r0, [r1, #MC_RCR_OFF]

        /*
         * The .init section contains code (and possibly data), which
         * has been loaded into external RAM and needs to be moved to
         * internal RAM.
         *
         * The load address __data_end and the target addresses
         * __reloc_start and __reloc_end must be provided by the linker
         * script.
         */
        ldr     r1, =__data_end
        ldr     r2, =__reloc_start
        ldr     r3, =__reloc_end
1:      cmp     r2, r3
        ldrne   r0, [r1], #4
        strne   r0, [r2], #4
        bne     1b

        /*
         * The .bss section keeps all data, that must be initialized to
         * zero. It typically contains uninitialized global and static
         * variables.
         *
         * Note, that __bss_start and __bss_end must be provided by the
         * linker script.
         */
        ldr     r1, =__bss_start
        ldr     r2, =__bss_end
        mov     r3, #0
1:      cmp     r1, r2
        strne   r3, [r1], #4
        bne     1b

        /*
         * Set return address and jump to Nut/OS initialization.
         */
        ldr     lr, =_exit
        ldr     r0, =NutInit
        bx      r0

        /*
         * Usually neither the system initialization nor the application
         * will ever return. If it does, we will keep the CPU in an
         * endless loop.
         */
        .weak   _exit
        .set    _exit, _exit_loop
        .global _exit
_exit_loop:
        b       _exit

        .ltorg
