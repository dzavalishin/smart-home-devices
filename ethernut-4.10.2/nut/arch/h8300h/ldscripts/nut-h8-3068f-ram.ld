/* Generic linker script for H8/3068F. Uses only internal Flash and RAM.     */
/* Program code (.text section) is located in the internal RAM. It is        */
/* startup routine (NutEntry) task to copy the code from Flash into RAM.     */
/* Author: Jan Dubiec <jdx@slackware.pl>                                     */

OUTPUT_ARCH(h8300h)
ENTRY(_NutEntry)
MEMORY
{
     rom (rx)  : ORIGIN = 0x000000, LENGTH = 384k
     ram (rwx) : ORIGIN = 0xffbf20, LENGTH = 16k
}

SECTIONS
{
    /* 256 byte interrupt vector area */
    .vects  0x000000 :
    {
	_vects = .;
	*(.vects);
    } > rom

    /* startup code area */
    .init 0x000100 :
    {
	*(.init)
    } > rom

    .fini :
    {
	*(.fini)
    } > rom

    .got :
    {
	*(.got)
	*(.got.plt)
    } > rom

    .rodata :
    {
	*(.rodata)
	*(.rodata.*)
	_erodata = .;
    } > rom

    .eh_frame_hdr :
    {
	*(.eh_frame_hdr)
    } > ram

    .eh_frame :
    {
	*(.eh_frame)
    } > rom

    .jcr :
    {
	*(.jcr)
    } > rom

    .gcc_exc :
    {
	*(.gcc_exc)
    } > rom

    .tors :
    {
	__CTOR_LIST__ = .;
	LONG((__CTOR_END__ - __CTOR_LIST__) / 4 - 2)
	___ctors = . ;
	*(.ctors)
	___ctors_end = . ;
	LONG(0)
	__CTOR_END__ = .;
	__DTOR_LIST__ = .;
	LONG((__DTOR_END__ - __DTOR_LIST__) / 4 - 2)
	___dtors = . ;
	*(.dtors)
	___dtors_end = . ;
	LONG(0)
	__DTOR_END__ = .;
	. = ALIGN(2);
	_mdata = .;
	PROVIDE(___mdata_end = .);
    } > rom

    /* program code area */
    .text : AT (_mdata)
    {
	PROVIDE(___text_start = .);
	*(.text)
	. = ALIGN(2);
	_etext = .;
	PROVIDE(___text_end = .);
    } > ram

    /* ROM image of the initialized data area */
    .data : {
	PROVIDE(___data_start = .);
	_data = .;
	*(.data)
	_edata = .;
	PROVIDE(___data_end = .);
    } > ram

    /* uninitialized data area */
    .bss :
    {
	PROVIDE(___bss_start = .);
	_bss = .;
	*(.bss)
	*(COMMON)
	 . = ALIGN(0x02);
	 _ebss = .;
	 _end = .;
	PROVIDE(___bss_end = .);
    } > ram

    /* set stack pointer to the end of internal RAM */
    PROVIDE(___stack = 0xffff20);

    .stab . (NOLOAD) :
    {
	[ .stab ]
    }

    .stabstr . (NOLOAD) :
    {
	[ .stabstr ]
    }

    /DISCARD/ :
    {
	*(.comment)
    }
}
