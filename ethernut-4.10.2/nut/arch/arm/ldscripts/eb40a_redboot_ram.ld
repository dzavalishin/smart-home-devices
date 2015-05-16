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
 * Revision 1.2  2010/05/13 Andre Riesberg
 * Added .ramfunc segment.
 *
 * Revision 1.1  2007/09/06 19:59:38  olereinhardt
 * First checkin, support for AT91EB40A eval board with redboot bootloader
 * running
 *
 * Revision 1.1  2004/09/19 12:37:42  haraldkipp
 * ARM7 initialization added
 *
 */

ENTRY(_start)
STARTUP(crteb40a_ram.o)
SEARCH_DIR(.)

MEMORY
{
  /* EB40A loads code at 0x100. */
  ram : org = 0x20000, len = 0x20000
}

SECTIONS
{
  . = 0x100;
  .text :
  {
    *(.vectors);
    . = ALIGN(4);
    *(.init0);
    . = ALIGN(4);
    *(.init1);
    . = ALIGN(4);
    *(.init2);
    . = ALIGN(4);
    *(.init3);
    . = ALIGN(4);
    *(.init4);
    . = ALIGN(4);
    *(.init5);
    . = ALIGN(4);
    *(.text);
    . = ALIGN(4);
    *(.text.*);
    . = ALIGN(4);
    *(.rodata*);
    . = ALIGN(4);
    *(.eh_frame*);
    . = ALIGN(4);
    *(.glue_7t);
    . = ALIGN(4);
    *(.glue_7);
    . = ALIGN(4);
    etext = .;
  } > ram

  .data :
  {
    PROVIDE (__data_start = .);
    *(.data)
    . = ALIGN(4);
    *(.ramfunc)
    edata = .;
    _edata = .;
    PROVIDE (__data_end = .);
  } > ram

  .bss :
  {
    PROVIDE (__bss_start = .);
    *(.bss)
    *(COMMON)
    . = ALIGN(4);
    PROVIDE (__bss_end = .);
    . += 0x400;
    PROVIDE (__stack = .);
    PROVIDE (__heap_start = .);
  } > ram

  .stab 0 (NOLOAD) :
  {
    [ .stab ]
  }
  .stabstr 0 (NOLOAD) :
  {
    [ .stabstr ]
  }
}
