
; Copyright (C) 2002-2004 by egnite Software GmbH. All rights reserved.
;
; Redistribution and use in source and binary forms, with or without
; modification, are permitted provided that the following conditions
; are met:
;
; 1. Redistributions of source code must retain the above copyright
;    notice, this list of conditions and the following disclaimer.
; 2. Redistributions in binary form must reproduce the above copyright
;    notice, this list of conditions and the following disclaimer in the
;    documentation and/or other materials provided with the distribution.
; 3. All advertising materials mentioning features or use of this
;    software must display the following acknowledgement:
;
;    This product includes software developed by egnite Software GmbH
;    and its contributors.
;
; THIS SOFTWARE IS PROVIDED BY EGNITE SOFTWARE GMBH AND CONTRIBUTORS
; ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
; LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
; FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL EGNITE
; SOFTWARE GMBH OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
; INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
; BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
; OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
; AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
; OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
; THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
; SUCH DAMAGE.
;
; For additional information see http://www.ethernut.de/
;
;
; This file has its orgin in following sources:
;
;  - Jaroslaw Karwik's RS232 boot loader
;  - Atmel's AVR109 application note "Self programming" ( www.atmel.com )
;  - Andy's Hutchinson modifications/optimizations of the original Atmel's code
;  - Bryce Denney's patches  ( bryce@tlw.com)
;

;
; $Log$
; Revision 1.3  2007/11/15 01:36:56  hwmaier
; Uses avr/io.h include file for register definitions so it works for ATmega128, AT90CAN128 and AT90CAN128
;
; Revision 1.2  2007/07/30 09:47:55  olereinhardt
; ATMega2561 port. Makedefs need to be modifies by hand (uncomment LDFLAGS
; line and comment out LDFLAGS for mega128
;
; Revision 1.1  2004/04/15 10:42:39  haraldkipp
; Checked in
;
;

#include <avr/io.h>

        .global SpmBufferFill
        .global SpmCommand

SpmCommand:
        movw    r30, r24        ; First argument is page address
        rjmp	DoSpm

SpmBufferFill:
        movw    r30, r24        ; First argument is page address  
        movw    r0, r22         ; Second argument is data pointer 
        ldi     r22, 1          ; SPMEN bit

DoSpm:
#if defined(__AVR_ATmega2561__)
        out     SPMCSR, r22     ; Store function in SPMCR
#else
        sts     SPMCSR, r22     ; Store function in SPMCR
#endif
        spm                     ; store program memory

WaitSpm:
#if defined(__AVR_ATmega2561__)
        in      r22, SPMCSR     ; Loop until SPMEN in SPMCR is cleared
#else
        lds     r22, SPMCSR     ; Loop until SPMEN in SPMCR is cleared
#endif
        sbrc    r22, 0
        rjmp    WaitSpm

        clr     r1              ; Compiler depends on R1 set to zero.
        ret

