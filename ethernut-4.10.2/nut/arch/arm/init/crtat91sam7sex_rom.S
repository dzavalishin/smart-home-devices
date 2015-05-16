/*
 * Copyright (C) 2005-2008 by egnite Software GmbH. 
 * Copyright (C) 2008 by egnite GmbH. 
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
 *
 */

/*
 * $Id: crtat91sam7sex_rom.S 3459 2011-06-02 11:30:26Z mifi $
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

#ifndef PLL_MUL_VAL
#define PLL_MUL_VAL 72
#endif

#ifndef PLL_DIV_VAL
#define PLL_DIV_VAL 14
#endif

#if MASTER_CLOCK_PRES == 1
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
#define AT91MCK_PRES    PMC_PRES_CLK_2
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
         * The watchdog is enabled after processor reset.
         */
#ifdef NUT_WDT_START
#if NUT_WDT_START
        /* Configure the watchdog. */
        ldr     r1, =WDT_BASE
        ldr     r0, =NUT_WDT_START
        str     r0, [r1, #WDT_MR_OFF]        
#endif
#else
        /* Disable the watchdog. */
        ldr     r1, =WDT_BASE
        ldr     r0, =WDT_WDDIS
        str     r0, [r1, #WDT_MR_OFF]
#endif

        /*
         * Enable the main oscillator. Set startup time of 6 * 8 slow 
         * clock cycles and wait until oscillator is stabilized.
         */
        ldr     r1, =PMC_BASE
        mov     r0, #(6 << 8)
        orr     r0, r0, #CKGR_MOSCEN
        str     r0, [r1, #CKGR_MOR_OFF]
wait_moscs:
        ldr     r0, [r1, #PMC_SR_OFF]
        tst     r0, #PMC_MOSCS
        beq     wait_moscs

        /*
         * Switch to Slow clock in case PLL was already set up.
         */
        ldr     r0, [r1, #PMC_MCKR_OFF]
        and   r0, r0, #~PMC_CSS
        /* Slow Clock. The next statement really isn't needed. 
        ** It is included for code clarity */
        orr     r0, r0, #PMC_CSS_SLOW_CLK
        str     r0, [r1, #PMC_MCKR_OFF]
wait_slowsel:
        ldr     r0, [r1, #PMC_SR_OFF]
        tst     r0, #PMC_MCKRDY
        beq   wait_slowsel

        /*
         * Set PLL:
         * PLLfreq = crystal / divider * (multiplier + 1)
         * Wait 28 clock cycles until PLL is locked.
         */
        ldr     r0, =((PLL_MUL_VAL << CKGR_MUL_LSB) | (28 << CKGR_PLLCOUNT_LSB) | (PLL_DIV_VAL << CKGR_DIV_LSB))

        str     r0, [r1, #CKGR_PLLR_OFF]
wait_lock:
        ldr     r0, [r1, #PMC_SR_OFF]
        tst     r0, #PMC_LOCK
        beq     wait_lock

        /*
         * Set master clock prescaler.
         */
        mov     r0, #AT91MCK_PRES
        str     r0, [r1, #PMC_MCKR_OFF]
wait_presrdy:
        ldr     r0, [r1, #PMC_SR_OFF]
        tst     r0, #PMC_MCKRDY
        beq     wait_presrdy

        /*
         * Switch to PLL clock.
         */
        ldr     r0, [r1, #PMC_MCKR_OFF]
        orr     r0, r0, #PMC_CSS_PLL_CLK
        str     r0, [r1, #PMC_MCKR_OFF]
wait_pllsel:
        ldr     r0, [r1, #PMC_SR_OFF]
        tst     r0, #PMC_MCKRDY
        beq     wait_pllsel

        /*
         * Enable SDRAM interface, if configured.
         */
#ifdef NUTMEM_SDRAM_BASE
        /* Enable SDRAM control at PIO A. */
        ldr     r1, =PIOA_BASE
        ldr     r0, =(_BV(PA23_NWR1_B) \
                | _BV(PA24_SDA10_B) \
                | _BV(PA25_SDCKE_B) \
                | _BV(PA26_NCS1_B) \
                | _BV(PA27_SDWE_B) \
                | _BV(PA28_CAS_B) \
                | _BV(PA29_RAS_B))
        str     r0, [r1, #PIO_BSR_OFF]
        str     r0, [r1, #PIO_PDR_OFF]
        
        /* Enable address bus (A0, A2-A11, A13-A17) at PIO B. */
        ldr     r1, =PIOB_BASE
        ldr     r0, =(_BV(PB0_A0_B) \
                | _BV(PB2_A2_B) \
                | _BV(PB3_A3_B) \
                | _BV(PB4_A4_B) \
                | _BV(PB5_A5_B) \
                | _BV(PB6_A6_B) \
                | _BV(PB7_A7_B) \
                | _BV(PB8_A8_B) \
                | _BV(PB9_A9_B) \
                | _BV(PB10_A10_B) \
                | _BV(PB11_A11_B) \
                | _BV(PB13_A13_B) \
                | _BV(PB14_A14_B) \
                | _BV(PB15_A15_B) \
                | _BV(PB16_A16_B) \
                | _BV(PB17_A17_B))
        str     r0, [r1, #PIO_BSR_OFF]
        str     r0, [r1, #PIO_PDR_OFF]

        /* Enable 16 bit data bus at PIO C. */
        ldr     r1, =PIOC_BASE
        ldr     r0, =(_BV(PC0_D0_A) \
                | _BV(PC1_D1_A) \
                | _BV(PC2_D2_A) \
                | _BV(PC3_D3_A) \
                | _BV(PC4_D4_A) \
                | _BV(PC5_D5_A) \
                | _BV(PC6_D6_A) \
                | _BV(PC7_D7_A) \
                | _BV(PC8_D8_A) \
                | _BV(PC9_D9_A) \
                | _BV(PC10_D10_A) \
                | _BV(PC11_D11_A) \
                | _BV(PC12_D12_A) \
                | _BV(PC13_D13_A) \
                | _BV(PC14_D14_A) \
                | _BV(PC15_D15_A))
        str     r0, [r1, #PIO_ASR_OFF]
        str     r0, [r1, #PIO_PDR_OFF]

        /* Enable SDRAM chip select. */
        ldr     r1, =EBI_BASE
        ldr     r0, =EBI_CS1A
        str     r0, [r1, #EBI_CSA_OFF]

        /* Load SDRAM controller base address. */
        ldr     r1, =SDRAMC_BASE
        
        /* Set SDRAM characteristics in configuration register. */
        /* Hard coded values for MT48LC32M16A2 with 48MHz CPU. */
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

        /* Load SDRAM base address. */
        mov     r2, #NUTMEM_SDRAM_BASE
        
        /* Issue 16 bit SDRAM command: NOP. */
        ldr     r0, =(SDRAMC_DBW | SDRAMC_MODE_NOP)
        str     r0, [r1, #SDRAMC_MR_OFF]
        mov     r3, #0
        str     r3, [r2, #0]

        /* Issue 16 bit SDRAM command: Precharge all. */
        ldr     r0, =(SDRAMC_DBW | SDRAMC_MODE_PRCGALL)
        str     r0, [r1, #SDRAMC_MR_OFF]
        str     r3, [r2, #0]

        /* Issue 8 auto-refresh cycles. */
        ldr     r0, =(SDRAMC_DBW | SDRAMC_MODE_RFSH)
        str     r0, [r1, #SDRAMC_MR_OFF]
        str     r3, [r2, #0]
        str     r0, [r1, #SDRAMC_MR_OFF]
        str     r3, [r2, #0]
        str     r0, [r1, #SDRAMC_MR_OFF]
        str     r3, [r2, #0]
        str     r0, [r1, #SDRAMC_MR_OFF]
        str     r3, [r2, #0]
        str     r0, [r1, #SDRAMC_MR_OFF]
        str     r3, [r2, #0]
        str     r0, [r1, #SDRAMC_MR_OFF]
        str     r3, [r2, #0]
        str     r0, [r1, #SDRAMC_MR_OFF]
        str     r3, [r2, #0]
        str     r0, [r1, #SDRAMC_MR_OFF]
        str     r3, [r2, #0]

        /* Issue 16 bit SDRAM command: Set mode register. */
        ldr     r0, =(SDRAMC_DBW | SDRAMC_MODE_LMR)
        str     r0, [r1, #SDRAMC_MR_OFF]
        ldr     r3, =(0xCAFEDEDE)
        str     r3, [r2, #20]

        /* Set refresh rate count. */
        mov     r0, #384
        str     r0, [r1, #SDRAMC_TR_OFF]

        /* Issue 16 bit SDRAM command: Normal mode. */
        ldr     r0, =(SDRAMC_DBW | SDRAMC_MODE_NORMAL)
        str     r0, [r1, #SDRAMC_MR_OFF]
        mov     r0, #0
        str     r0, [r2, #0]
#endif /* NUTMEM_SDRAM_BASE */

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
         * Relocate .data section (Copy from ROM to RAM).
         */
        ldr     r1, =__etext
        ldr     r2, =__data_start
        ldr     r3, =__data_end

_41:
        cmp     r2, r3
        ldrlo   r0, [r1], #4
        strlo   r0, [r2], #4
        blo     _41

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
