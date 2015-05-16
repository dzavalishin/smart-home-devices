; to be included by crt*.s files
;
; enable external ACCESS
;
	XMCRA	= 0x74

	ldi		R16,0x80		; extern RAM enable
	ldi		R17,FRAM
	sbrs		R17,0
	ori		R16,0x04		; wait state
	sts		XMCRA,R16
