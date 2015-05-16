;***************************************************************************
; KPIT Cummins Infosystems Ltd, Pune, India. - 7th July 2003.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE
;
; Adapted to Nut/OS by Jan Dubiec <jdx@slackware.pl>
;
;***************************************************************************
;
; System initialisation routine - entry point for the application.
; The stack pointer is initialised, then the hardware initialisation
; routine called. If needed, the .text section is copied into RAM
; and then the static data areas are initialised, before the main
; function is executed. A simple exit funtion is also supplied.
;
;****************************************************************************

	.h8300h

	.section .init

	.global _NutEntry
	.global _NutExit

	.extern _NutHWInit
	.extern _NutInit

	.extern ___data_start
	.extern ___data_end
	.extern ___bss_start
	.extern ___bss_end
	.extern ___mdata_end
	.extern ___stack

_NutEntry:
	; initialise the stack pointer
	mov.l #___stack, er7
	; disable interrupts
	orc.b #0xc0, ccr
	; call the hardware initialisation routine
	jsr @_NutHWInit

#ifdef NUTDEBUG
	; clear the whole internal RAM
	mov.l #4, er3		; double word chunks 
	mov.l #0, er2
	mov.l #0xffff20, er1	; end of RAM
	mov.l #0xffbf20, er0	; beginning of RAM
	cmp.l er0, er1
	beq start_c2
start_c1:
	mov.l er2, @er0
	add.l er3, er0
	cmp.l er0, er1		; dest == edata?
	bne start_c1
start_c2:
#endif

#ifdef ROMSTART
#ifdef TEXT_IN_RAM /* program code should executed run from RAM*/
	; get the boundaries for the .text and .data section initialisation
	mov.l #___text_start, er0; source start address
#else
	; get the boundaries for the .data section initialisation
	mov.l #___data_start, er0; source start address
#endif /* TEXT_IN_RAM */

	mov.l #___data_end, er1	; source end address
	mov.l #___mdata_end, er2; destination address; see linker script
	cmp.l er0,er1
	beq start_1
start_l:
	mov.b @er2, r3l		; get from src
	mov.b r3l, @er0		; place in dest
	inc.l #1, er2		; inc src
	inc.l #1, er0		; inc dest
	cmp.l er0, er1		; dest == edata?
	bne start_l
start_1:
#endif /* ROMSTART */

	; zero out .bss section
	mov.l #___bss_start, er0
	mov.l #___bss_end, er1
	cmp.l er0, er1
	beq start_3
	sub.b r2l, r2l
start_2:
	mov.b r2l, @er0
	inc.l #1, er0
	cmp.l er0, er1
	bne start_2

start_3:
	; enable interrupts
	andc.b #0x3f, ccr
	; start Nut/OS
	jsr @_NutInit

	; jump to exit
	mov.l er0, er4
	jmp @_NutExit

_NutExit:
	nop
	bra _NutExit
