/*
 * Copyright (C) 2004 by Jan Dubiec. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY JAN DUBIEC AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL JAN DUBIEC
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * $Log$
 * Revision 1.1  2004/03/16 16:48:26  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 */

/*
 * Hardware initialisation routine. It is called before .text and .data
 * sections are initialised. Bus controller and other essential hardware
 * setup code should be placed here.
 *
 * The code is partially based on an example distributed with H8 gcc.
 *
 * Author: Jan Dubiec <jdx@slackware.pl>
 *
 */

/* definition of the bus & wait-state controller register addresses */

#include <h83068f.h>

__attribute__ ((section(".init")))
void NutHWInit(void)
{

    /*
     * disable system clock output; switch peripherials (except SCI1,SCI2, DMAC
     * and 16 bit timer) into standby mode
     */
    MSTCR.WORD = 0xf9ad;

    /* Area 4 accessed without wait states */
    BSC.WCR.BIT.W4 = 0;

    /* enable /CS4 signal */
    BSC.CSCR.BYTE = 0x1f;

    /* Enable /WAIT pin */
    BSC.BCR.BIT.WAITE = 1;

    /* set P1 to be address bus */
    P1DDR = 0xff;
    /* set P2 to be address bus */
    P2DDR = 0xff;
    /* set P5 to be address bus */
    P5DDR = 0xff;
    /* P6.2 is output pin (ReaDY LED) */
    P6DDR = 0x82;
    /* set up port 8 */
    P8DDR = 0xee;
    /* set up port 9 */
    P9DDR = 0xc2;
    /* set up port A */
    PADDR = 0x0e;
    /* set up port B */
    PBDDR = 0x63;

    /* disable IRQ0 */
    INTC.IER.BIT.IRQ0E = 0;

    /* disable IMIA0 interrupt */
    ITU.TISRA.BIT.IMIEA0 = 0;
    /* stop timer 0 */
    ITU.TSTR.BIT.STR0 = 0;

    /* Initially the ReaDY LED is off */
    P6DR.BIT.B1 = 1;            /* just for testing; will be removed soon */

}
