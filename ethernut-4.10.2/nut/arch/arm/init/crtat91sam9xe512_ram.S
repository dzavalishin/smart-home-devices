/*
 * Copyright (C) 2006-2007 by egnite Software GmbH. All rights reserved.
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
 * Revision 1.2  2009/01/16 19:45:42  haraldkipp
 * All ARM code is now running in system mode.
 *
 * Revision 1.1  2008/08/06 12:51:02  haraldkipp
 * Added support for Ethernut 5 (AT91SAM9XE reference design).
 *
 *
 */

#include <cfg/clock.h>
#include <cfg/memory.h>

#include <arch/arm.h>

#ifndef PLL_MUL_VAL
//#define PLL_MUL_VAL 72
#define PLL_MUL_VAL 391 //48=90MHz 97=180MHz
#endif

#ifndef PLL_DIV_VAL
//#define PLL_DIV_VAL 14
#define PLL_DIV_VAL 80 //10
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
        .word   0x00001000      /* Size of the boot image will go here */

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
        .set    __undef, __undef_stop
        .weak   __swi
        .set    __swi, __swi_stop
        .weak   __prefetch_abort
        .set    __prefetch_abort, __prefetch_stop
        .weak   __data_abort
        .set    __data_abort, __data_stop

        .global __undef_stop
__undef_stop:
        b       __undef_stop

        .global __swi_stop
__swi_stop:
        b       __swi_stop

        .global __prefetch_stop
__prefetch_stop:
        b       __prefetch_stop

        .global __data_stop
__data_stop:
        b       __data_stop

        .ltorg
/*
 * Section 1: Hardware initialization.
 */
        .section .init1, "ax", %progbits
        .global	_start
_start:

        /*
         * Disable all interrupts and clear any pending ones. 
         * Useful for debugging w/o target reset.
         */
        ldr     r1, =AIC_BASE
        mvn     r0, #0
        str     r0, [r1, #AIC_IDCR_OFF]
        str     r0, [r1, #AIC_ICCR_OFF]

        /* 
         * Perform 8 "End Of Interrupt" commands to make sure that
         * the AIC will not lock out nIRQ. 
         */
        str     r0, [r1, #AIC_EOICR_OFF]
        str     r0, [r1, #AIC_EOICR_OFF]
        str     r0, [r1, #AIC_EOICR_OFF]
        str     r0, [r1, #AIC_EOICR_OFF]
        str     r0, [r1, #AIC_EOICR_OFF]
        str     r0, [r1, #AIC_EOICR_OFF]
        str     r0, [r1, #AIC_EOICR_OFF]
        str     r0, [r1, #AIC_EOICR_OFF]

        /*
         * The watchdog is enabled after processor reset. Disable it.
         */
        ldr     r1, =WDT_BASE
        ldr     r0, =WDT_WDDIS
        str     r0, [r1, #WDT_MR_OFF]

        /*
         * Enable the main oscillator. Set startup time of 64 * 8 slow 
         * clock cycles and wait until oscillator is stabilized.
         */
        ldr     r1, =PMC_BASE
        mov     r0, #(64 << CKGR_OSCOUNT_LSB)
        orr     r0, r0, #CKGR_MOSCEN
        str     r0, [r1, #CKGR_MOR_OFF]
	.global wait_moscs
wait_moscs:
        ldr     r0, [r1, #CKGR_MCFR_OFF]
        tst     r0, #CKGR_MAINRDY
        beq     wait_moscs

        /*
         * CKGR_MCFR contains the number of main clock cycles within
         * 16 slow clock cycles. Use this value to test if the main 
         * clock is running at higher frequency than the slow clock. 
         * If yes, select the main clock.
         */
@        ldr     r0, [r1, #CKGR_MCFR_OFF]
@        mov     r0, r0, asl #16 /* Main clock is in lower 16 bits. */
@        mov     r0, r0, lsr #16
@        cmp     r0, #16
@        bls     set_pllb
@        mov     r0, #PMC_CSS_MAIN_CLK
@        str     r0, [r1, #PMC_MCKR_OFF]

        /*
         * PLLfreq = crystal / divider * (multiplier + 1)
         * Crystal is 18.432 MHz, divider is 14 and multiplier is 72.
         * The resulting PLL B output frequency is 96.1097142857 MHz.
         * For USB the deviation is less than the required 0.25%.
         * Wait max clock cycles until PLL is locked.
         */
        .global set_pllb
set_pllb:
//        ldr     r0, =(CKGR_USBDIV_1 | CKGR_OUT_0 | CKGR_PLLCOUNT | (72 << CKGR_MUL_LSB) | (14 << CKGR_DIV_LSB)) */
@        ldr     r0, =(_BV(29) | CKGR_OUT_0 | CKGR_PLLCOUNT | (PLL_MUL_VAL << CKGR_MUL_LSB) | (PLL_DIV_VAL << CKGR_DIV_LSB))
@        str     r0, [r1, #CKGR_PLLAR_OFF]
wait_lock:
@        ldr     r0, [r1, #PMC_SR_OFF]
@        and     r0, r0, #PMC_LOCKA
@        cmp     r0, #0
@        beq     wait_lock

        /*
         * Select PLL for the processor clock (PCK) and set the master 
         * clock divider, which results in a master clock frequency
         * of 48.055 MHz).
         */
//        ldr     r0, =(PMC_CSS_PLLB_CLK | PMC_MDIV_2)
@        ldr     r0, =(PMC_CSS_PLLA_CLK | PMC_PRES_CLK | PMC_MDIV_1) //90 MHz
//        ldr     r0, =(PMC_CSS_PLLA_CLK | PMC_PRES_CLK | PMC_MDIV_2) //180MHz
@        str     r0, [r1, #PMC_MCKR_OFF]
        .global wait_mckrdy
wait_mckrdy:
@        ldr     r0, [r1, #PMC_SR_OFF]
@        and     r0, r0, #PMC_MCKRDY
@        cmp     r0, #0
@        beq     wait_mckrdy

        /*
         * Enable external reset key.
         */
        ldr     r0, =(RSTC_KEY | RSTC_URSTEN)
        ldr     r1, =RSTC_MR
        str     r0, [r1, #0]

        /*
         * Remap internal RAM to address 0 and copy vectors to this loacation.
         */
        mov     r0, #(MATRIX_MRCR_RCB0 | MATRIX_MRCR_RCB1)
        ldr     r1, =MATRIX_BASE
        str     r0, [r1, #MATRIX_MRCR_OFF]

        ldr     r0, =__vectors
        mov     r1, #0
        ldmia   r0!, {r2-r9}
        stmia   r1!, {r2-r9}
        ldmia   r0!, {r2-r9}
        stmia   r1!, {r2-r9}
        
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
 * Section 3: Set ARM specific modes.
 */
        .section .init3,"ax",%progbits

        .global __enter_mode
__enter_mode:

        /*
         * Enable the instruction cache.
         */
        mrc     p15, 0, r0, c1, c0, 0
        ldr     r3, =0xC0001000
        orr     r0, r0, r3
        mcr     p15, 0, r0, c1, c0, 0

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

__next_bss:
        cmp     r1, r2
        strne   r3, [r1], #+4
        bne     __next_bss

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
        mov     lr, pc
        bx      r0

        .global __exit_loop
__exit_loop:
        b       __exit_loop

        .ltorg
