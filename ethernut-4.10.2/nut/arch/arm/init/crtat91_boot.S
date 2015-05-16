/*
 * Copyright (C) 2005 by egnite Software GmbH. All rights reserved.
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
 * Revision 1.3  2008/03/17 10:14:44  haraldkipp
 * Reduced wait states for Flash access from 8 to 4.
 *
 * Revision 1.2  2006/10/05 17:12:35  haraldkipp
 * Bug #1497065 fixed. Stack sizes increased.
 *
 * Revision 1.1  2005/10/24 11:30:48  haraldkipp
 * Initial check in.
 *
 */

#include <arch/arm.h>

FLASH_BASE = 0x01000000
FLASH_SIZE = 0x00200000

RAM_BASE  = 0x00000000
RAM_LIMIT = 0x00000100
RAM_SIZE  = (256*1024)

IRQ_STACK_SIZE = (128*4)
FIQ_STACK_SIZE = (64*4)
ABT_STACK_SIZE = (16*4)
UND_STACK_SIZE = (16*4)

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

_rom_start:
        .word   0x00000000

_rom_end:
        .word   0x0003FFFC

_ram_start:
        .word   0x00300000

        .ltorg
        .globl	_start
_start:
        /* Set supervisor mode. */
        mrs     r0, cpsr
        bic     r0, r0, #0x1f
        orr     r0, r0, #0x13
        msr     cpsr, r0

        /* Move code from ROM to RAM.
         * r0 = source address
         * r1 = target address
         * r2 = source end address
         */
        ldr     r0, _rom_start
        ldr     r1, _ram_start
        ldr     r2, _rom_end
_rom2ram:
        ldmia   r0!, {r3-r10}
        stmia   r1!, {r3-r10}
        cmp     r0, r2
        ble     _rom2ram

        /* Remapping memory. */
        adr     r10, _rmap_tab

        /* Load the address where to jump */
        ldr     r12, _rmap_endp

        ldmia   r10!, {r0-r9,r11}
        stmia   r11!, {r0-r9}

        /* Jump and break the pipeline */
        mov     pc, r12                 

_rmap_endp:
        .long   remapmem_end

_rmap_tab:
        /* Flash memory at 1000 0000. */
        .long   0x10000000 | EBI_CSEN | EBI_PAGES_16M | EBI_WSE | EBI_NWS_4 | EBI_DBW_16
        /* Ethernet controller at 2000 0000 */
        .long   0x20000000 | EBI_CSEN | EBI_BAT_BYTE_SELECT | EBI_PAGES_1M | EBI_WSE | EBI_NWS_3 | EBI_DBW_16
        /* CPLD registers at 2100 0000 */
        .long   0x21000000 | EBI_CSEN | EBI_PAGES_1M | EBI_WSE | EBI_NWS_2 | EBI_DBW_8
        .long   0x30000000                  /* r3: unused */
        /* CPLD expansion port */
        .long   0x22000000 | EBI_CSEN | EBI_TDF_7 | EBI_WSE | EBI_NWS_8 | EBI_DBW_8
        .long   0x50000000                  /* r5: unused */
        .long   0x60000000                  /* r6: unused */
        .long   0x70000000                  /* r7: unused */

        .long   0x00000001                  /* r8: Remap command */
        .long   EBI_ALE_4M                  /* r9: Memory control. */
        .long   0xFFE00000                  /* r11: EBI base address */

        .ltorg
remapmem_end:

/*
 * Section 1: Hardware initialization.
 */
        .section .init1,"ax",%progbits

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

        .global __set_stacks
__set_stacks:

/*
 * Section 2: Set stack pointers.
 */
        .section .init2,"ax",%progbits
        /*
         * Set exception stack pointers and enable interrupts.
         */
        ldr     r0, =__xcp_stack
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

        .rept   32
        .long   0
        .endr

        .global __xcp_stack
__xcp_stack:
        
        .ltorg

        .global __enter_mode
        .align
__enter_mode:
/*
 * Section 3: Enter system mode.
 */
        .section .init3,"ax",%progbits

        msr     CPSR_c, #ARM_MODE_SYS | ARM_CPSR_I_BIT | ARM_CPSR_F_BIT
        b       __clear_bss

        .ltorg

        .global __clear_bss
__clear_bss:
/*
 * Section 4: Clear bss.
 */
        .section .init4,"ax",%progbits

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

        .global __call_rtos
__call_rtos:
/*
 * Section 5: Call RTOS
 */
        .section .init5,"ax",%progbits

        /*
         * Jump to Nut/OS initialization.
         */
        ldr     r0, =NutInit
        bx      r0

End:
        b       End

        .ltorg
