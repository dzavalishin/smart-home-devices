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
 */

/*
 * $Log$
 * Revision 1.1  2004/10/17 15:14:23  haraldkipp
 * Added Nintendo Game Boy Advance with Xport 2.0 from Charmed Labs.
 *
 */
 
ENTRY(_start)
STARTUP(crtgbaxport2.o)
OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)

__sp_usr = 0x3008000 - 0x100;
__sp_irq = 0x3008000 - 0x60;


MEMORY {
    /* 16kB System ROM */
    sysrom : ORIGIN = 0x00000000, LENGTH = 0x4000
    /* 256kB External Work RAM */
    ram : ORIGIN = 0x02000000, LENGTH = 0x40000
    /* 32kB Internal Work RAM */
    iwram : ORIGIN = 0x03000000, LENGTH = 0x8000
    /* 256kB XPORT Flash ROM */
    rom : ORIGIN = 0x08000000, LENGTH = 0x400000
}

SECTIONS
{
    .text 0x8000000 :
    {
        _stext = ABSOLUTE(.);
        PROVIDE (__stext = ABSOLUTE(.));
        *(.text*) *(.gnu.warning) *(.gnu.linkonce*) *(.init) *(.glue_7) *(.glue_7t)
    } > rom
    _etext = .; PROVIDE (__etext = .);

    .fini ALIGN(4) :
    {
        . = . ;
        *(.fini)
    } >  rom

    .rodata ALIGN(4) :
    {
        . = . ;
        *(.rodata*)
    } > rom

    .rodata1 ALIGN(4) :
    {
        . = . ;
        *(.rodata1)
    } >  rom

    .fixup ALIGN(4) :
    {
        . = . ;
        *(.fixup)
    } >  rom

    .eh_frame ALIGN(4):
     {
         KEEP (*(.eh_frame))
     } > rom

    .gcc_except_table   ALIGN (0x4)  :
    {
        . = . ;
        *(.gcc_except_table)
    } > rom

    .data 0x2000000 : AT ((LOADADDR ( .gcc_except_table ) + SIZEOF ( .gcc_except_table ) + 4  - 1) & ~ (4  - 1))
    {
        __ram_data_start = ABSOLUTE (.);
        *(.data*) *(.data1)
        _GOT1_START_ = ABSOLUTE (.);
        *(.got1)
        _GOT1_END_ = ABSOLUTE (.);
        _GOT2_START_ = ABSOLUTE (.);
        *(.got2)
        _GOT2_END_ = ABSOLUTE (.);
        . = ALIGN (4);
        __CTOR_LIST__ = ABSOLUTE (.);
        KEEP (*(SORT (.ctors*)))
        __CTOR_END__ = ABSOLUTE (.);
        __DTOR_LIST__ = ABSOLUTE (.);
        KEEP (*(SORT (.dtors*)))
        __DTOR_END__ = ABSOLUTE (.);
        *(.dynamic) *(.sdata*) *(.sbss*)
        . = ALIGN (4);
        *(.2ram.*)
    } > ram

    __rom_data_start = LOADADDR (.data);
    __ram_data_end = .;
    PROVIDE (__ram_data_end = .);
    _edata = .; PROVIDE (edata = .);
    PROVIDE (__rom_data_end = LOADADDR (.data) + SIZEOF(.data));

    .bss ALIGN(4) :
    {
        __bss_start = ABSOLUTE (.);
        *(.scommon) *(.dynbss) *(.bss*) *(COMMON)
        __bss_end = ABSOLUTE (.);
        PROVIDE (__heap_start = .);
    } > ram

    __heap1 = ALIGN(8);
    . = ALIGN(4);
    _end = .;
    PROVIDE (end = .);
}
