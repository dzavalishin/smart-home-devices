/*
 * Copyright (C) 2005 by egnite Software GmbH. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY EGNITE SOFTWARE GMBH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL EGNITE
 * SOFTWARE GMBH OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/*
 * $Log$
 * Revision 1.1  2005/11/03 15:14:27  haraldkipp
 * First import.
 *
 */

#include "avr/io.h"

        .text

/* 
 * Disable interrupts and initialize the stack pointer.
 */
        .global __boot_entry
__boot_entry:
        cli
;        ldi     r18, lo8(RAMEND)
;        out     _SFR_IO_ADDR(SPL), r18
;        ldi     r18, hi8(RAMEND)
;        out     _SFR_IO_ADDR(SPH), r18

/*
 * Copy initialized data from ROM to RAM.
 */
        .global __boot_idata
__boot_idata:

        ldi     r18, hi8(__data_end)
        ldi     XL, lo8(__data_start)
        ldi     XH, hi8(__data_start)

        ldi     ZL, hh8(__data_load_start)
        out     _SFR_IO_ADDR(RAMPZ), ZL
        ldi     ZL, lo8(__data_load_start)
        ldi     ZH, hi8(__data_load_start)

__boot_idata_next:
        cpi     XL, lo8(__data_end)
        cpc     XH, r18
        breq    __boot_idata_done        
        elpm    r0, Z+
        st      X+, r0
        rjmp    __boot_idata_next
__boot_idata_done:

/*
 * Clear uninitialized global data.
 */
        .global __boot_ibss
__boot_ibss:
        clr     r1
        ldi     YL, hi8(__bss_end)
        ldi     XL, lo8(__bss_start)
        ldi     XH, hi8(__bss_start)

__boot_ibss_next:
        cpi     XL, lo8(__bss_end)
        cpc     XH, YL
        breq    __boot_ibss_done
        st      X+, r1
        rjmp    __boot_ibss_next
__boot_ibss_done:

/* 
 * Call bootloader.
 */
        .global __boot_start
__boot_start:
        call    main

/* 
 * Jump to application.
 */
        .global exit
exit:        
        jmp     0
