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
 * Revision 1.7  2009/02/17 09:32:25  haraldkipp
 * A lot of clean-up had been done with SAM9260 initialization.
 * Clock configurations should now work as expected. Note, that
 * the CPU is running now at 198.6560 MHz with peripheral clock
 * at 99.3280 MHz.
 *
 * Revision 1.6  2009/01/16 19:45:42  haraldkipp
 * All ARM code is now running in system mode.
 *
 * Revision 1.5  2008/01/16 00:19:42  olereinhardt
 * Changed backslash to slash in include path names
 *
 * Revision 1.4  2007/02/15 16:18:22  haraldkipp
 * Configurable runtime initialization.
 *
 * Revision 1.3  2006/09/07 09:02:27  haraldkipp
 * Instruction cache is now enabled. Many thanks to Dany Nativel for
 * providing this patch.
 *
 * Revision 1.2  2006/09/05 12:29:17  haraldkipp
 * Dedicated loops for each exception allows to determine its type.
 * Applications fail to run when laoded via SAM-BA fro two reasons.
 * First, internal RAM was not mapped to address 0x00000000.
 * Second, interrupts were disabled.
 *
 * Revision 1.1  2006/08/31 19:19:55  haraldkipp
 * No time to write comments. ;-)
 *
 */

#include <cfg/clock.h>
#include <cfg/memory.h>

#include <arch/arm.h>

#ifndef PLL_MUL_VAL
#define PLL_MUL_VAL 96
#endif

#ifndef PLL_DIV_VAL
#define PLL_DIV_VAL 9
#endif

#if MASTER_CLOCK_PRES == 2
#define AT91MCK_PRES    PMC_PRES_CLK
#elif MASTER_CLOCK_PRES == 4
#define AT91MCK_PRES    PMC_PRES_CLK_4
#elif MASTER_CLOCK_PRES == 8
#define AT91MCK_PRES    PMC_PRES_CLK_8
#elif MASTER_CLOCK_PRES == 16
#define AT91MCK_PRES    PMC_PRES_CLK_16
#elif MASTER_CLOCK_PRES == 32
#define AT91MCK_PRES    PMC_PRES_CLK_32
#elif MASTER_CLOCK_PRES == 64
#define AT91MCK_PRES    PMC_PRES_CLK_64
#else
#define AT91MCK_PRES    PMC_PRES_CLK
#endif

#if PERIPHERAL_CLOCK_DIV == 1
#define AT91MCK_MDIV    PMC_MDIV_1
#elif PERIPHERAL_CLOCK_DIV == 4
#define AT91MCK_MDIV    PMC_MDIV_4
#else
#define AT91MCK_MDIV    PMC_MDIV_2
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

#ifdef NUTMEM_SDRAM_BASE
/* Tested on AT91SAM9260-EK without success. */
#ifndef NUTMEM_SDRAM_COLBITS
#define NUTMEM_SDRAM_COLBITS    9
#endif
#ifndef NUTMEM_SDRAM_ROWBITS
#define NUTMEM_SDRAM_ROWBITS    13
#endif
#ifndef NUTMEM_SDRAM_CASLAT
#define NUTMEM_SDRAM_CASLAT     2
#endif
#ifndef NUTMEM_SDRAM_BANKS
#define NUTMEM_SDRAM_BANKS      4
#endif
#ifndef NUTMEM_SDRAM_TWR
#define NUTMEM_SDRAM_TWR        2
#endif
#ifndef NUTMEM_SDRAM_TRC
#define NUTMEM_SDRAM_TRC        7
#endif
#ifndef NUTMEM_SDRAM_TRP
#define NUTMEM_SDRAM_TRP        2
#endif
#ifndef NUTMEM_SDRAM_TRCD
#define NUTMEM_SDRAM_TRCD       2
#endif
#ifndef NUTMEM_SDRAM_TRAS
#define NUTMEM_SDRAM_TRAS       5
#endif
#ifndef NUTMEM_SDRAM_TXSR
#define NUTMEM_SDRAM_TXSR       8
#endif

#if NUTMEM_SDRAM_BANKS == 2
#define SDRAMC_CFG_NB 0
#else
#define SDRAMC_CFG_NB SDRAMC_NB
#endif

#if NUTMEM_SDRAM_COLBITS == 8
#define SDRAMC_CFG_NC SDRAMC_NC_8
#elif NUTMEM_SDRAM_COLBITS == 9
#define SDRAMC_CFG_NC SDRAMC_NC_9
#elif NUTMEM_SDRAM_COLBITS == 10
#define SDRAMC_CFG_NC SDRAMC_NC_10
#else
#define SDRAMC_CFG_NC SDRAMC_NC_11
#endif

#if NUTMEM_SDRAM_ROWBITS == 11
#define SDRAMC_CFG_NR SDRAMC_NR_11
#elif NUTMEM_SDRAM_ROWBITS == 12
#define SDRAMC_CFG_NR SDRAMC_NR_12
#else
#define SDRAMC_CFG_NR SDRAMC_NR_13
#endif

#if NUTMEM_SDRAM_CASLAT == 1
#define SDRAMC_CFG_CAS SDRAMC_CAS_1
#elif NUTMEM_SDRAM_CASLAT == 2
#define SDRAMC_CFG_CAS SDRAMC_CAS_2
#else
#define SDRAMC_CFG_CAS SDRAMC_CAS_3
#endif

#endif /* NUTMEM_SDRAM_BASE */

.macro DELAY_1MS
        mov     r3, #0x200000
1:
        subs    r3, r3, #1
        bne     1b
.endm

.macro DEBUG_CHAR
		DELAY_1MS
        ldr     r1, =DBGU_BASE
        mov     r0, #88
        str     r0, [r1, #US_THR_OFF]
.endm

.macro STOP_EXEC
1:
        b     	1b
.endm

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
        /* The following vector is reserved. Atmel obviously interpreted 
           this as "reserved for Atmel", because SAM-BA uses it to store 
           the size of the boot image. */
        .word   0

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
		DEBUG_CHAR
        b       __xcpt_dummy

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
 * Debug!?
 */
 		DEBUG_CHAR

        /*
         * The watchdog is enabled after processor reset.
         */
#ifdef USE_WDT
#ifdef NUT_WDT_START
#if NUT_WDT_START
        /* Configure the watchdog. */
        ldr     r1, =WDT_BASE
        ldr     r0, =NUT_WDT_START
        str     r0, [r1, #WDT_MR_OFF]        
#endif /* if NUT_WDT_START */
#else /* ifdef NUT_WDT_START */
        /* Disable the watchdog. */
        ldr     r1, =WDT_BASE
        ldr     r0, =WDT_WDDIS
        str     r0, [r1, #WDT_MR_OFF]
#endif /* ifdef NUT_WDT_START */
#endif /* ifdef USE_WDT */

#ifdef INIT_MAIN_OSCILLATOR_CLOCK
        /* Test if the main oscillator is running. */
        ldr     r1, =PMC_BASE
        ldr     r0, [r1, #PMC_SR_OFF]
        tst     r0, #PMC_MOSCS
        bne     sel_mainck
        
        /*
         * If not running, enable the main oscillator. Set startup time 
         * of 64 * 8 slow clock cycles and wait until oscillator is stable.
         */
        mov     r0, #(64 << CKGR_OSCOUNT_LSB)
        orr     r0, r0, #CKGR_MOSCEN
        str     r0, [r1, #CKGR_MOR_OFF]
1:
        ldr     r0, [r1, #CKGR_MCFR_OFF]
        tst     r0, #CKGR_MAINRDY
        beq     1b

        /*
         * CKGR_MCFR contains the number of main clock cycles within
         * 16 slow clock cycles. Use this value to test if the main 
         * clock is running at higher frequency than the slow clock. 
         * If yes, select the main clock.
         */
sel_mainck:         
        ldr     r0, [r1, #CKGR_MCFR_OFF]
        mov     r0, r0, asl #16 /* Main clock is in lower 16 bits. */
        mov     r0, r0, lsr #16
        cmp     r0, #16
        bls     set_pll
        mov     r0, #PMC_CSS_MAIN_CLK
        str     r0, [r1, #PMC_MCKR_OFF]

        /*
         * Configure PLL A.
         * PLLfreq = crystal / divider * (multiplier + 1)
         * Wait max clock cycles until PLL is locked.
         */
set_pll:
        ldr     r0, =(_BV(29) | CKGR_OUT_2 | (63 << CKGR_PLLCOUNT_LSB) | (PLL_MUL_VAL << CKGR_MUL_LSB) | (PLL_DIV_VAL << CKGR_DIV_LSB))
        str     r0, [r1, #CKGR_PLLAR_OFF]
1:
        ldr     r0, [r1, #PMC_SR_OFF]
        and     r0, r0, #PMC_LOCKA
        cmp     r0, #0
        beq     1b

        /*
         * Select PLL A for the processor clock (PCK) and set the master 
         * clock prescaler and divider.
         */
        ldr     r0, =(PMC_CSS_PLLA_CLK | AT91MCK_PRES | AT91MCK_MDIV)
        str     r0, [r1, #PMC_MCKR_OFF]
1:
        ldr     r0, [r1, #PMC_SR_OFF]
        and     r0, r0, #PMC_MCKRDY
        cmp     r0, #0
        beq     1b

#endif /* INIT_MAIN_OSCILLATOR_CLOCK */

        /* Enable instruction cache. */
        mrc     p15, 0, r0, c1, c0, 0
        orr     r0, r0, #(1 << CP15_CR1_I_BIT)
        mcr     p15, 0, r0, c1, c0, 0

        /* Set the EBI slot cycle limit to 64. */
        ldr     r1, =MATRIX_BASE
        ldr     r0, [r1, #(MATRIX_SCFG_OFF + ( 4 * MATRIX_SLAVE_EBI))]
        and     r0, r0, #0xffffff00
        orr     r0, r0, #64
        str     r0, [r1, #(MATRIX_SCFG_OFF + ( 4 * MATRIX_SLAVE_EBI))]

        /*
         * Enable SDRAM interface, if configured.
         *
         * _N_O_T_E_ : TESTED ON THE AT91SAM9260-EK NAD FOUND NOT WORKING.
         */
#ifdef INIT_SDRAM

#ifdef NUTMEM_SDRAM_BASE

        /* Initialize the matrix */
        ldr     r1, =CCFG_BASE
        ldr     r0, [r1, #CCFG_CSA_OFF]
        orr     r0, r0, #CCFG_CS1A
        str     r0, [r1, #CCFG_CSA_OFF]

        /* Enable upper 16 bit data bus at PIO C. */
        ldr     r1, =PIOC_BASE
        ldr     r0, =(_BV(PC16_D16_A) \
                | _BV(PC17_D17_A) \
                | _BV(PC18_D18_A) \
                | _BV(PC19_D19_A) \
                | _BV(PC20_D20_A) \
                | _BV(PC21_D21_A) \
                | _BV(PC22_D22_A) \
                | _BV(PC23_D23_A) \
                | _BV(PC24_D24_A) \
                | _BV(PC25_D25_A) \
                | _BV(PC26_D26_A) \
                | _BV(PC27_D27_A) \
                | _BV(PC28_D28_A) \
                | _BV(PC29_D29_A) \
                | _BV(PC30_D30_A) \
                | _BV(PC31_D31_A))
        str     r0, [r1, #PIO_ASR_OFF]
        str     r0, [r1, #PIO_PDR_OFF]

        /* SDRAM controller base address kept in r1. */
        ldr     r1, =SDRAMC_BASE

        /* Set SDRAM characteristics in configuration register. */
        ldr     r0, =(SDRAMC_CFG_NC | SDRAMC_CFG_NR \
                | SDRAMC_CFG_NB | SDRAMC_CFG_CAS \
                | (NUTMEM_SDRAM_TWR << SDRAMC_TWR_LSB) \
                | (NUTMEM_SDRAM_TRC << SDRAMC_TRC_LSB) \
                | (NUTMEM_SDRAM_TRP << SDRAMC_TRP_LSB) \
                | (NUTMEM_SDRAM_TRCD << SDRAMC_TRCD_LSB) \
                | (NUTMEM_SDRAM_TRAS << SDRAMC_TRAS_LSB) \
                | (NUTMEM_SDRAM_TXSR << SDRAMC_TXSR_LSB))
        str     r0, [r1, #SDRAMC_CR_OFF]

        /* 200us delay minimum. */
        mov     r3, #0x2000
1:        
        subs    r3, r3, #1
        bne     1b
        /* Leave with r3 = 0! */

        /* SDRAM base address kept in r2. */
        mov     r2, #NUTMEM_SDRAM_BASE
        
        /* Issue SDRAM command: NOP. */
        ldr     r0, =(SDRAMC_MODE_NOP)
        str     r0, [r1, #SDRAMC_MR_OFF]
        str     r3, [r2, #0]

        /* Issue SDRAM command: Precharge all. */
        ldr     r0, =(SDRAMC_MODE_PRCGALL)
        str     r0, [r1, #SDRAMC_MR_OFF]
        str     r3, [r2, #0]

        /* Delay. */
        mov     r3, #0x20000
1:        
        subs    r3, r3, #1
        bne     1b

        /* Issue 8 auto-refresh cycles. */
        ldr     r0, =(SDRAMC_MODE_RFSH)
        str     r0, [r1, #SDRAMC_MR_OFF]
        add     r3, r3, #1
        str     r3, [r2, #4]
        str     r0, [r1, #SDRAMC_MR_OFF]
        add     r3, r3, #1
        str     r3, [r2, #8]
        str     r0, [r1, #SDRAMC_MR_OFF]
        add     r3, r3, #1
        str     r3, [r2, #12]
        str     r0, [r1, #SDRAMC_MR_OFF]
        add     r3, r3, #1
        str     r3, [r2, #16]
        str     r0, [r1, #SDRAMC_MR_OFF]
        add     r3, r3, #1
        str     r3, [r2, #20]
        str     r0, [r1, #SDRAMC_MR_OFF]
        add     r3, r3, #1
        str     r3, [r2, #24]
        str     r0, [r1, #SDRAMC_MR_OFF]
        add     r3, r3, #1
        str     r3, [r2, #28]
        str     r0, [r1, #SDRAMC_MR_OFF]
        add     r3, r3, #1
        str     r3, [r2, #32]

        /* Issue SDRAM command: Set mode register. */
        ldr     r0, =(SDRAMC_MODE_LMR)
        str     r0, [r1, #SDRAMC_MR_OFF]
        ldr     r3, =(0xCAFEDEDE)
        str     r3, [r2, #36]

        /* Set refresh rate count. */
        ldr     r3, =695
        str     r0, [r1, #SDRAMC_TR_OFF]

        /* Issue SDRAM command: Normal mode. */
        ldr     r0, =(SDRAMC_MODE_NORMAL)
        str     r0, [r1, #SDRAMC_MR_OFF]
        mov     r0, #0
        str     r3, [r2, #0]
#endif /* NUTMEM_SDRAM_BASE */
#endif /* ifdef INIT_SDRAM */

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

        /* Enter system mode. */
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

1:
        cmp     r1, r2
        strne   r3, [r1], #+4
        bne     1b

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
 		DEBUG_CHAR
        b       __exit_loop

        .ltorg
