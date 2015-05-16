/*
 * Copyright (C) 2001-2005 by egnite Software GmbH. All rights reserved.
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
 * For additional information see http://www.ethernut.de/
 *
 */

/*
 * $Log$
 * Revision 1.4  2008/08/11 06:59:12  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.3  2005/10/24 17:59:19  haraldkipp
 * Use correct header file, arm, not gba.
 *
 * Revision 1.2  2005/08/02 17:46:45  haraldkipp
 * Major API documentation update.
 *
 * Revision 1.1  2005/07/26 18:02:26  haraldkipp
 * Moved from dev.
 *
 * Revision 1.1  2005/04/05 17:53:24  haraldkipp
 * Initial check in.
 *
 */

#include <arch/arm.h>
#include <dev/irqreg.h>
#include <stdio.h>

/*!
 * \addtogroup xgNutArchArmDevIrqGba
 */
/*@{*/

IRQ_HANDLER sig_VBLANK;
IRQ_HANDLER sig_HBLANK;
IRQ_HANDLER sig_VCOUNT;
IRQ_HANDLER sig_TMR0;
IRQ_HANDLER sig_TMR1;
IRQ_HANDLER sig_TMR2;
IRQ_HANDLER sig_TMR3;
IRQ_HANDLER sig_SIO;
IRQ_HANDLER sig_DMA0;
IRQ_HANDLER sig_DMA1;
IRQ_HANDLER sig_DMA2;
IRQ_HANDLER sig_DMA3;
IRQ_HANDLER sig_KEYPAD;
IRQ_HANDLER sig_GAMEPAK;

void IrqHandler(void)
{
    register uint16_t irqf = inw(REG_IF);

    if(irqf & INT_SIO) {
        CallHandler(&sig_SIO);
    }
#if 0
    if(irqf & INT_VBLANK) {
        CallHandler(&sig_VBLANK);
    }
    if(irqf & INT_HBLANK) {
        CallHandler(&sig_HBLANK);
    }
    if(irqf & INT_VCOUNT) {
        CallHandler(&sig_VCOUNT);
    }
    if(irqf & INT_TMR0) {
        CallHandler(&sig_TMR0);
    }
    if(irqf & INT_TMR1) {
        CallHandler(&sig_TMR1);
    }
    if(irqf & INT_TMR2) {
        CallHandler(&sig_TMR2);
    }
#endif
    if(irqf & INT_TMR3) {
        CallHandler(&sig_TMR3);
    }
#if 0
    if(irqf & INT_DMA0) {
        CallHandler(&sig_DMA0);
    }
    if(irqf & INT_DMA1) {
        CallHandler(&sig_DMA1);
    }
    if(irqf & INT_DMA2) {
        CallHandler(&sig_DMA2);
    }
    if(irqf & INT_DMA3) {
        CallHandler(&sig_DMA3);
    }
    if(irqf & INT_KEYPAD) {
        CallHandler(&sig_KEYPAD);
    }
    if(irqf & INT_GAMEPAK) {
        CallHandler(&sig_GAMEPAK);
    }
#endif
}

void InitIrqHandler(void)
{
    outdw(INT_VECTOR, (uint32_t)IrqHandler);
}

/*@}*/
