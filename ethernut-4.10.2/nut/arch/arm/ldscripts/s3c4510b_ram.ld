OUTPUT_ARCH(arm)
ENTRY(TheEntry)
MEMORY
{
     ram (rwx)  : ORIGIN = 0x00000000, LENGTH = 0x01000000
     /* ram (rwx) : ORIGIN = 0xffbf20, LENGTH = 16k */
}

SECTIONS
{
    /* interrupt vector area */
    .vects  0x00000000 :
    {
	_vects = .;
	*(.vects);
    } > ram

    /* startup code area, bois will jump to 0x8000 after boot */
    /* only one instruction in this section */
    .init 0x00008000 :
    {
	    *(.init)
    } > ram
    
    /* 
        ram vectors
    .ramvects 0x00008010 :
    {
        *(.ramvects)
    } > ram 
    
    */
	
    /* program code area	. = ALIGN(4); */
    .text 0x00008100 :
    {
    	_text = .;
    	*(.text)
    	*(.text.*)
    	*(.rodata)
    	. = ALIGN(0x100);
    	_etext = .;
    } > ram
    
    
    /* rom image of the initialized data area */
    .data : 
    {
    	_data_start = .;
    	*(.data)
        . = ALIGN(0x100);
    	_edata = .;
    } > ram

	 
    /* uninitialized data area */
    .bss :
    {
    	_bss_start = .;
    	*(.bss)
    	*(COMMON) 
    	 . = ALIGN(0x100);	
    	 __bss_end = .;
    } > ram
    
    
    /* set stack pointer */
    .stack (0x01000000 ) : 
    {
		__stack = .;
	}
		
    /* 
     * PROVIDE(__stack = 0x01000000);
     */
}
