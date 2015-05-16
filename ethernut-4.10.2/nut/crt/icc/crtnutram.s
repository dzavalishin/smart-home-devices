; make sure to assemble w/ -n flag, e.g.
; iasavr -n crt...

	.include "area.s"
	.text
__start::			; entry point
	.include "extram.s"
	.include "init.s"

; call user main routine
	call _NutInit
_exit::
	rjmp	_exit

; interrupt vectors. The first entry is the reset vector
;
	.area vector(abs)
	.org 0
	jmp __start
