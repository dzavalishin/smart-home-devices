/*
 * Copyright (C) 2005-2006 by egnite Software GmbH. All rights reserved.
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
 * Revision 1.5  2009/01/16 19:45:42  haraldkipp
 * All ARM code is now running in system mode.
 *
 * Revision 1.4  2008/07/14 13:06:46  haraldkipp
 * Consistent chip selects and wait states.
 *
 * Revision 1.3  2008/01/16 00:19:41  olereinhardt
 * Changed backslash to slash in include path names
 *
 * Revision 1.2  2006/10/05 17:12:36  haraldkipp
 * Bug #1497065 fixed. Stack sizes increased.
 *
 * Revision 1.1  2006/08/01 07:34:16  haraldkipp
 * New linker script and new startup file support applications running in
 * flash memory.
 *
 */

#include <arch/arm.h>

IRQ_STACK_SIZE = (128*4)
FIQ_STACK_SIZE = (64*4)
ABT_STACK_SIZE = (16*4)
UND_STACK_SIZE = (16*4)

        /* We start in ARM mode. */
        .arm

/*
 * Section 0: Vector table and remapping.
 */
        .section .init0,"ax",%progbits
    /*
     * After reset these vectors are located at the beginning of
     * flash memory, which starts at address 0x00000000. After
     * remap, flash will be relocated at 0x10000000 and these
     * vectors are copied to the RAM base address, which will
     * be remapped to address 0x00000000.
     */
        .global __vectors
__vectors:
        b       _start          /* Reset */
        ldr     pc, [pc, #20]   /* Undefined instruction */
        ldr     pc, [pc, #20]   /* Software interrupt */
        ldr     pc, [pc, #20]   /* Prefetch abort */
        ldr     pc, [pc, #20]   /* Data abort */
        ldr     pc, [pc, #20]   /* Reserved */

        /*
         * On IRQ the PC will be loaded from AIC_IVR, which
         * provides the address previously set in AIC_SVR.
         * The interrupt routine will be called in ARM_MODE_IRQ
         * with IRQ disabled and FIQ unchanged.
         */
        ldr     pc, [pc, #-0xF20]   /* Interrupt request, auto vectoring. */
        ldr     pc, [pc, #-0xF20]   /* Fast interrupt request, auto vectoring. */

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

        /*
         * Exceptions are weakly linked to this endless loop.
         */
        .global __xcpt_dummy
__xcpt_dummy:
        b       __xcpt_dummy

        .ltorg
        .global _start
_start:
        
        /* 
         * Set supervisor mode. 
         */
        mrs     r0, cpsr
        bic     r0, r0, #ARM_MODE_MASK
        orr     r0, r0, #ARM_MODE_SVC
        msr     cpsr, r0

        /* 
         * Remapping memory. 
         */
        adr     r10, _rmap_tab      /* Load remap table address. */
        ldr     r12, _rmap_endp     /* Load jump target. */
        ldmia   r10!, {r0-r9,r11}   /* Load the complete remap table plus EBI address. */
        stmia   r11!, {r0-r9}
        mov     pc, r12             /* Jump to remapped flash and break the pipeline */

_rmap_endp:
        .long   remapmem_end

_rmap_tab:
        /* 
         * Chip select NCS0 enables 16-bit Flash memory at 0x10000000. 
         * We need 4 wait states. 
         */
        .long   0x10000000 | EBI_CSEN | EBI_PAGES_16M | EBI_WSE | EBI_NWS_4 | EBI_DBW_16
        /* 
         * Chip select NCS1 enables 16-bit Ethernet controller at 0x20000000.
         * We need 3 wait states. 
         */
        .long   0x20000000 | EBI_CSEN | EBI_BAT_BYTE_SELECT | EBI_PAGES_1M | EBI_WSE | EBI_NWS_3 | EBI_DBW_16
        /* 
         * Chip select NCS2 selects 8-bit NPL registers at 0x21000000.
         * The CPLD is quite fast and may run at one or zero wait states.
         * This pin is shared with GPIO pin 26.
         */
        .long   0x21000000 | EBI_CSEN | EBI_PAGES_1M | EBI_WSE | EBI_NWS_2 | EBI_DBW_8
        /*
         * Chip select NCS3 is not used.
         * This pin os shared with GPIO pin 27 and available at the expansion port.
         */
        .long   0x30000000
        /* 
         * Chip select CS4 selects the 8-bit NPL expansion bus at 0x22000000.
         * This pin is shared with GPIO pin 31 and address bit 23.
         */
        .long   0x22000000 | EBI_CSEN | EBI_TDF_7 | EBI_PAGES_1M | EBI_WSE | EBI_NWS_8 | EBI_DBW_8
        /*
         * Chip select CS5 is not used.
         * This pin is shared with GPIO pin 30 and address bit 22.
         */
        .long   0x50000000
        /*
         * Chip select CS6 is not available, but used as address bit 21.
         */
        .long   0x60000000
        /*
         * Chip select CS7 is not available, but used as address bit 20.
         */
        .long   0x70000000
        /* 
         * EBI remap command.
         */
        .long   EBI_RCB
        /* 
         * Maximum address space of 4 MBytes per chip select.
         * This occupies A0 to A21.
         */
        .long   EBI_ALE_4M
        /* 
         * EBI base address.
         */
        .long   EBI_BASE                    

remapmem_end:

        /*
         * Move vectors from Flash to RAM.
         */
        ldr     r0, =__vectors
        mov     r1, #0
        ldmia   r0!, {r2-r9}
        stmia   r1!, {r2-r9}
        ldmia   r0!, {r2-r9}
        stmia   r1!, {r2-r9}

        /* Jump to next section. */        
        b       __hw_init
        
        /* Literal pool of section 0. */
        .ltorg
        
/*
 * Section 1: Hardware initialization.
 */
        .section .init1,"ax",%progbits
        .global __hw_init
__hw_init:

        /*
         * Enable all clocks.
         */
        mvn     r0, #0
        ldr     r1, =PS_BASE
        str     r0, [r1, #0x04]

        /*
         * Initialize the interrupt controller.
         */
        add     r0, pc,#-(8+.-__aic_table)
        ldmia   r0, {r1-r4}
        str     r4, [r1, #AIC_SPU]

        mov     r0, #8
L0:
        str     r1, [r1, #AIC_EOICR]
        subs    r0, r0, #1
        bhi     L0

        str     r2, [r1, #AIC_SVR(0)]
        add     r1, r1, #AIC_SVR(0)
        mov     r0, #31
L1:
        str     r3, [r1, r0, LSL #2]
        subs    r0, r0, #1
        bhi     L1

        b       __set_stacks

__aic_table:
        .word   AIC_BASE
        .word   __irq_dummy
        .word   __irq_dummy
        .word   __irq_dummy

        .ltorg

        /* Interrupt dummy. */
        .global __irq_dummy
__irq_dummy:
        b       __irq_dummy

/*
 * Section 2: Set stack pointers.
 */
        .section .init2,"ax",%progbits
        .global __set_stacks
__set_stacks:
        /*
         * Set exception stack pointers and enable interrupts.
         */
        ldr     r0, =0x00030000
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
        .align
__enter_mode:
        msr     CPSR_c, #ARM_MODE_SYS | ARM_CPSR_I_BIT | ARM_CPSR_F_BIT
        b       __crt_init

        .ltorg

/*
 * Section 4: C runtime initialization.
 */
        .section .init4,"ax",%progbits
        .global __crt_init
__crt_init:

        /*
         * Clear bss.
         */
        ldr     r1, =__bss_start
        ldr     r2, =__bss_end
        ldr     r3, =0
_40:
        cmp     r1, r2
        strne   r3, [r1], #+4
        bne     _40

        /*
         * Copy data and RAM functions from Flash to RAM.
         */
        ldr     r0, =_text_end
        ldr     r1, =__data_start
        ldr     r2, =__data_end
        subs    r2, r2, r1
        beq     _42

_41:
        ldr     r3, [r0], #4
        str     r3, [r1], #4
        subs    r2, r2, #4
        bne     _41

        /*
         * Initialize user stack pointer.
         */
_42:         
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
        ldr     r0, =_start
        bx      r0

        /* Literal pool of section 5. */
        .ltorg
