; init.s
;
; to be included by the crt*.s files
;
	; initialize stacks
	;
	; set the hardware stack to ram_end, and the software stack some
	; bytes below that
	ldi R28,<ram_end
	ldi R29,>ram_end
	out $3D,R28
	out $3E,R29
	subi R28,<hwstk_size
	sbci R29,>hwstk_size

	ldi R16,0xAA	; sentenial
	std	Y+0,R16		; put sentenial at bottom of HW stack

	clr R0
	ldi R30,<__bss_start
	ldi R31,>__bss_start
	ldi R17,>__bss_end

; this loop zeros out all the data in the bss area
;
init_loop:
	cpi R30,<__bss_end
	cpc R31,R17
	breq init_done
	st Z+,R0
	rjmp init_loop
init_done:

	std Z+0,R16		; put sentenial at bottom of SW stack

; copy data from idata to data
; idata contains the initialized values of the global variables

	ldi R30,<__idata_start
	ldi R31,>__idata_start
	ldi R26,<__data_start
	ldi R27,>__data_start
	ldi R17,>__idata_end

  .if USE_ELPM
	; set RAMPZ to 1
	ldi R16,1
	out 0x3B,R16
  .endif
copy_loop:
	cpi R30,<__idata_end
	cpc R31,R17
	breq copy_done
  .if USE_ELPM
	elpm	; load (RAMPZ:Z)
  .else
	lpm 	; load (Z) byte into R0
  .endif
	adiw R30,1
	st X+,R0
	rjmp copy_loop
copy_done:
