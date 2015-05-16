; to be included by crt*.s files
;
; enable external ACCESS
;
	MCUCR	= 0x35

	ldi		R16,0x80		; extern RAM enable
	ldi		R17,FRAM
	sbrs	R17,0
	ori		R16,0x40		; wait state
	out		MCUCR,R16
