/*
 * Copyright (C) 2005-2007 by egnite Software GmbH
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
 *
 */

/*
 * $Id: start_rom.S 2935 2010-04-01 12:14:17Z haraldkipp $
 */

PS_BASE   = 0xFFFF4000      /* Power saving controller. */
AIC_BASE  = 0xFFFFF000      /* Advanced interrupt controller. */
AIC_SVR   = 0x080           /* Source Vector Register */
AIC_EOICR = 0x130           /*  of Interrupt Command Register */
AIC_SPU   = 0x134           /* Spurious Vector Register */

FLASH_BASE = 0x01000000
FLASH_SIZE = 0x00200000

RAM_BASE  = 0x00000000
RAM_LIMIT = 0x00000100
RAM_SIZE  = (256*1024)

IRQ_STACK_SIZE = (3*8*4)     @ 3 words per interrupt priority level
FIQ_STACK_SIZE = (3*4)       @ 3 words
ABT_STACK_SIZE = (1*4)       @ 1 word
UND_STACK_SIZE = (1*4)       @ 1 word

ARM_MODE_USER = 0x10
ARM_MODE_FIQ = 0x11
ARM_MODE_IRQ = 0x12
ARM_MODE_SVC = 0x13
ARM_MODE_ABORT = 0x17
ARM_MODE_UNDEF = 0x1B
ARM_MODE_SYS = 0x1F

EBI_DBW_8 = 0x0002
EBI_DBW_16 = 0x0001

EBI_NWS_1 = 0x0000
EBI_NWS_2 = 0x0004
EBI_NWS_3 = 0x0008
EBI_NWS_4 = 0x000C
EBI_NWS_5 = 0x0010
EBI_NWS_6 = 0x0014
EBI_NWS_7 = 0x0018
EBI_NWS_8 = 0x001C
EBI_WSE = 0x0020

EBI_PAGES_1M = 0x0000
EBI_PAGES_4M = 0x0080
EBI_PAGES_16M = 0x0100
EBI_PAGES_64M = 0x0180

EBI_TDF_0 = 0x0000
EBI_TDF_1 = 0x0200
EBI_TDF_2 = 0x0400
EBI_TDF_3 = 0x0600
EBI_TDF_4 = 0x0800
EBI_TDF_5 = 0x0A00
EBI_TDF_6 = 0x0C00
EBI_TDF_7 = 0x0E00

EBI_BAT_BYTE_WRITE = 0x0000
EBI_BAT_BYTE_SELECT = 0x1000

EBI_CSEN = 0x2000

EBI_ALE_16M = 0x00
EBI_ALE_8M = 0x04
EBI_ALE_4M = 0x05
EBI_ALE_2M = 0x06
EBI_ALE_1M = 0x07

EBI_DRP_STANDARD = 0x00
EBI_DRP_EARLY = 0x10

/*
 * Section 0: Vector table and reset entry.
 */
        .arm
        .section .init0,"ax",%progbits

        .global __vectors
__vectors:
        b       _start                  /* Reset */
        b       __undef                 /* Undefined instruction */
        b       __swi                   /* Software interrupt */
        b       __prefetch_abort        /* Prefetch abort */
        b       __data_abort            /* Data abort */
        b       __xcpt_dummy            /* Reserved */

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
        .long   0x20000000 | EBI_CSEN | EBI_BAT_BYTE_SELECT | EBI_WSE | EBI_NWS_3 | EBI_DBW_16
        /* CPLD registers at 2100 0000 */
        .long   0x21000000 | EBI_CSEN | EBI_WSE | EBI_NWS_2 | EBI_DBW_8
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

        str     r2, [r1, #AIC_SVR]
        add     r1, r1, #AIC_SVR
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
        msr     CPSR_c, #ARM_MODE_FIQ | 0xC0
        mov     r13, r0
        sub     r0, r0, #FIQ_STACK_SIZE
        msr     CPSR_c, #ARM_MODE_IRQ | 0xC0
        mov     r13, r0
        sub     r0, r0, #IRQ_STACK_SIZE
        msr     CPSR_c, #ARM_MODE_ABORT | 0xC0
        mov     r13, r0
        sub     r0, r0, #ABT_STACK_SIZE
        msr     CPSR_c, #ARM_MODE_UNDEF | 0xC0
        mov     r13, r0
        sub     r0, r0, #UND_STACK_SIZE
        msr     CPSR_c, #ARM_MODE_SVC | 0xC0
        mov     r13, r0
        b       __enter_user

        .rept   32
        .long   0
        .endr

        .global __xcp_stack
__xcp_stack:
        
        .ltorg

        .global __enter_user
        .align
__enter_user:
/*
 * Section 3: Enter user mode.
 */
        .section .init3,"ax",%progbits

@        msr     CPSR_c, #ARM_MODE_USER

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

        .global __call_rtos
__call_rtos:
/*
 * Section 5: Call RTOS
 */
        .section .init5,"ax",%progbits

        /*
         * Jump to Nut/OS initialization.
         */
        ldr     r0, =main
        bx      r0

End:
        ldr     r0, =0
        bx      r0

        .ltorg
