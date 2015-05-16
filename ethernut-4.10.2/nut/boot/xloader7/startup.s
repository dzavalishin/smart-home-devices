
/* Standard definitions of Mode bits and Interrupt (I & F) flags in PSRs */

    .equ    I_BIT,          0x80      /* when I bit is set, IRQ is disabled */
    .equ    NO_IRQ,         0x80      /* mask to disable IRQ */
    .equ    F_BIT,          0x40      /* when F bit is set, FIQ is disabled */
    .equ    NO_FIQ,         0x40      /* mask to disable FIQ */
    .equ    NO_INT,         (NO_IRQ | NO_FIQ) 		/*mask to disable IRQ and FIQ */

    .equ    USR_MODE,       0x10
    .equ    FIQ_MODE,       0x11
    .equ    IRQ_MODE,       0x12
    .equ    SVC_MODE,       0x13
    .equ    ABT_MODE,       0x17
    .equ    UND_MODE,       0x1B
    .equ    SYS_MODE,       0x1F

/*****************************************************************************
* The starupt code must be linked at the start of ROM, which is NOT
* necessarily address zero.
*/
    .text
    .code 32

    .global _start
    .func   _start

_start:

_reset:
    /* Relocate .fastcode section (copy from ROM to RAM) */
    LDR     r0,=__fastcode_load
    LDR     r1,=__fastcode_start
    LDR     r2,=__fastcode_end
1:
    CMP     r1,r2
    LDMLTIA r0!,{r3}
    STMLTIA r1!,{r3}
    BLT     1b

    /* Clear the .bss section (zero init) */
    LDR     r1,=__bss_start__
    LDR     r2,=__bss_end__
    MOV     r3,#0
1:
    CMP     r1,r2
    STMLTIA r1!,{r3}
    BLT     1b


    /* Initialize stack pointers for all ARM modes */
    MSR     CPSR_c,#(SYS_MODE | I_BIT | F_BIT)
    LDR     sp,=__c_stack_top__                  /* set the C stack pointer */


    /* Enter the C/C++ code */
    LDR     r12,=main
    MOV     lr,pc           /* set the return address */
    BX      r12             /* the target code can be ARM or THUMB */

    .size   _start, . - _start
    .endfunc


    .end
