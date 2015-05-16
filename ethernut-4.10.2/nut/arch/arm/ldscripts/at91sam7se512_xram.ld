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

/*
 * $Id$
 *
 * This is a GNU ld linker script of the AT91SAM7SE512 for executables,
 * which are loaded and started in external RAM. Parts of the executable
 * need to be located in internal RAM, specifically the exception vectors.
 * On the other hand, some loaders, like Atmel's SAM-BA tool, need pure
 * binary files, which are created during Nut/OS application build by
 * default. Such binaries do not support address gaps, which appears here
 * between internal and external RAM. One solution might be to use
 * binutils objcopy to create two separate binaries. The solution chosen
 * here is to include the internal RAM contents in an external RAM
 * area and let the runtime initialization code move it to the internal
 * RAM. During runtime, this area is re-used by the .bss segment.
 *
 * The current version of this linker script will not work with C++.
 */

/* Specifies the related runtime initialization code. */
STARTUP(crtat91sam7se512_xram.o)
SEARCH_DIR(.)

/*
 * The MEMORY command specifies available memory regions. Note, that
 * we make no assumption about the actual size of the external RAM,
 * but simply occupy the whole address space. To make sure, that no
 * overflow occurs, you need to check the linker map file.
 */
MEMORY
{
  iram  : org = 0x00000000, len = 32k
  xram : org = 0x20000000, len = 256M
}

/*
 * First time we use region aliases in a Nut/OS linker script. They
 * don't really help much in this script, but are included for
 * consistency and may make sense in other scripts.
 */
REGION_ALIAS("REGION_TEXT", xram);
REGION_ALIAS("REGION_RODATA", xram);
REGION_ALIAS("REGION_DATA", xram);
REGION_ALIAS("REGION_BSS", xram);
REGION_ALIAS("REGION_RELOC", iram);

/*
 * The SECTIONS command tells the linker how to map input sections
 * from the compiled object files into output sections of the final
 * executable.
 */
SECTIONS
{
  /* Our .text section contains executable code. */
  .text :
  {
    . = ALIGN(4);
    /* The KEEP command makes sure, that the unreferenced init code will
       not be removed by linker optimization strategies. */
    KEEP(*(.init0));
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

  /* Read-only data is stored in the .rodata section. Not sure, if we
     can include it into the .data section. The debugger may distinguish
     between these types of memory. */
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata*);
  } > REGION_RODATA

  /* Explicitly initialized data goes into the .data section. */
  .data :
  {
    . = ALIGN(4);
    *(.data)
    . = ALIGN(4);
    /* Marks the end of non relocatable code and data. */
    PROVIDE (__data_end = .);
  } > REGION_DATA

  /* This section will be moved to internal RAM. */
  .init : AT (__data_end)
  {
    /* Marks the target start address for the relocation.
       Used by the runtime initialization. */
    PROVIDE (__reloc_start = .);
    KEEP(*(.vectors));
    . = ALIGN(4);
    *(.ramfunc)
    /* Any idea how to add uninitialized RAM buffers? These may be
       required, because some peripherals will fail with DMA (PDC)
       buffers located in external SDRAM. */
    . = ALIGN(4);
    /* Also used by the runtime initialization. */
    PROVIDE (__reloc_end = .);
  } > REGION_RELOC

  /* This section will be filled with zeros. It typically contains not
     explicitly initialized global and static data. Note, that it over-
     laps the relocatable section. Thus, the runtime initialization must
     move the relocatable segment before clearing the .bss section. */
  .bss : AT (__data_end)
  {
    . = ALIGN(4);
    PROVIDE (__bss_start = .);
    *(.bss)
    *(COMMON)
    . = ALIGN(4);
    PROVIDE (__bss_end = .);
    /* Used by the Nut/OS initialization to setup heap memory. */
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
