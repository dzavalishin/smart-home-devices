/*
 * Copyright (C) 2008 by egnite Software GmbH
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


STARTUP(crtat91sam7se512_xram.o)
SEARCH_DIR(.)

MEMORY
{
  iram  : org = 0x00000000, len = 32k
  sdram : org = 0x20000000, len = 256M
}

REGION_ALIAS("REGION_TEXT", sdram);
REGION_ALIAS("REGION_RODATA", sdram);
REGION_ALIAS("REGION_DATA", sdram);
REGION_ALIAS("REGION_BSS", sdram);
REGION_ALIAS("REGION_RELOC", iram);

SECTIONS
{
  .text :
  {
    *(.init0);
    . = ALIGN(4);
    *(.text);
    . = ALIGN(4);
    *(.text.*);
    . = ALIGN(4);
    *(.eh_frame*);
    . = ALIGN(4);
    *(.glue_7t);
    . = ALIGN(4);
    *(.glue_7);
  } > REGION_TEXT

  . = ALIGN(4);

  .rodata :
  {
    . = ALIGN(4);
    *(.rodata*);
  } > REGION_RODATA

  . = ALIGN(4);

  .data :
  {
    *(.data)
    . = ALIGN(4);
    PROVIDE (__data_end = .);
  } > REGION_DATA

  .init : AT (__data_end)
  {
    PROVIDE (__reloc_start = .);
    *(.vectors);
    . = ALIGN(4);
    *(.ramfunc)
    . = ALIGN(4);
    PROVIDE (__reloc_end = .);
  } > REGION_RELOC

  .bss :
  {
    . = ALIGN(4);
    PROVIDE (__bss_start = .);
    *(.bss)
    *(COMMON)
    . = ALIGN(4);
    PROVIDE (__bss_end = .);
    . += 0x400;
    PROVIDE (__exp_stack = .);
    . += 0x400;
    PROVIDE (__stack = .);
    PROVIDE (__heap_start = .);
  } > REGION_BSS

  .stab 0 (NOLOAD) :
  {
    [ .stab ]
  }
  .stabstr 0 (NOLOAD) :
  {
    [ .stabstr ]
  }
}
