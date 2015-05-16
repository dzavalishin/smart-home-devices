/*
 * Copyright (C) 2004 by egnite Software GmbH. All rights reserved.
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
 * Revision 1.3  2005/07/26 16:19:40  haraldkipp
 * Run Nut/OS in user mode.
 *
 * Revision 1.2  2004/11/08 18:10:45  haraldkipp
 * Interrupt handling removed.
 *
 * Revision 1.1  2004/10/17 15:14:21  haraldkipp
 * Added Nintendo Game Boy Advance with Xport 2.0 from Charmed Labs.
 *
 */

        .section .text,"ax"

        .global _reset
_reset:

/*
 * Nintendo ROM Header
 */
rom_header:
        b       rom_header_end

        /* 
         * 156 bytes compressed bitmap of Nintendo logo. The BIOS
         * will compare this with an internal copy and lock the
         * GBA if they differ.
         */
        .byte   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0              /* 0x04..0x0F */
        .byte   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  /* 0x10..0x1F */
        .byte   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  /* 0x20..0x2F */
        .byte   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  /* 0x30..0x3F */
        .byte   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  /* 0x40..0x4F */
        .byte   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  /* 0x50..0x5F */
        .byte   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  /* 0x60..0x6F */
        .byte   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  /* 0x70..0x7F */
        .byte   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  /* 0x80..0x8F */
        .byte   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0              /* 0x90..0x9B */
        .byte   0 /* If bits 2 and 7 are set, enables BIOS FIQ/Undef handler. */
        .byte   0 /* Bits 0 and 1 are used for the cartridge key. */  
        .byte   0
        .byte   0

        /* Game title, 12 uppercase characters. */
        .byte   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0

        /* 4 characters game code. */
        .byte   0, 0, 0, 0x00

        /* 2 characters maker code, "01" for Nintendo. */
        .byte   0x30, 0x31

        /* Fixed value, must be 0x96. */
        .byte   0x96

        /* Main unit code, 0 for GBA. */
        .byte   0x00

        /* Device type, used by the debugging handler. */
        .byte   0x00

        /* 7 bytes reserved area. */
        .byte   0, 0, 0, 0, 0, 0, 0

        /* Software version, usually zero. */
        .byte   0x00

        /* Header checksum. */
        .byte   0xF0

        /* 2 byte checksum. */
        .byte   0, 0


/*
 * Multiboot Header
 */
rom_header_end:
        b       _start

        /* 
         * Boot method, set by the BIOS:
         * - 1 = joybus mode
         * - 2 = normal mode
         * - 3 = multiboot mode
         */
        .byte   0

        /* 
         * Slave ID, set to 1..3 by the BIOS in multiboot mode. 
         * In normal mode this is set to 1.
         */
        .byte   0

        /* Reserved area of 26 bytes. */
        .byte   0
        .byte   0
        .word   0
        .word   0
        .word   0
        .word   0
        .word   0
        .word   0

        .extern     main

        .arm
        .align

        .global     _start
_start:

        /* Switch to IRQ mode and set IRQ stack pointer. */
        mov     r0, #0x12
        msr     cpsr, r0
        ldr     sp, =__sp_irq

        /* Switch to user mode and set user stack pointer. */
        mov     r0, #0x10
        msr     cpsr, r0
        ldr     sp, =__sp_usr

        /* Enter Thumb mode */
        adr    r0, _enter_thumb + 1
        bx     r0
        .thumb
_enter_thumb:

        /* Initialize data segment. */
        ldr     r1, =__rom_data_start
        ldr     r2, =__ram_data_start
        ldr     r4, =__ram_data_end
        sub     r3, r4, r2
        beq     enter_main

copy_data:
        ldmia   r1!, {r0}
        stmia   r2!, {r0}
        sub     r3, #4
        bne     copy_data

enter_main:
        ldr     r0, =NutInit
        bx      r0

exit_loop:
        b       exit_loop

        .end
