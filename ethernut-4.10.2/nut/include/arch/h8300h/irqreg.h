#ifndef _DEV_IRQREG_H8_H_
#define _DEV_IRQREG_H8_H_

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
 * Revision 1.1  2005/07/26 18:35:09  haraldkipp
 * First check in
 *
 * Revision 1.1  2004/03/16 16:48:28  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 */

enum {
    IRQ_NMI,
    IRQ_INT0,
    IRQ_INT1,
    IRQ_INT2,
    IRQ_INT3,
    IRQ_INT4,
    IRQ_INT5,
    IRQ_WOVI,
    IRQ_CMI,
    IRQ_ADI,
    IRQ_IMIA0,
    IRQ_IMIB0,
    IRQ_OVI0,
    IRQ_IMIA1,
    IRQ_IMIB1,
    IRQ_OVI1,
    IRQ_IMIA2,
    IRQ_IMIB2,
    IRQ_OVI2,
    IRQ_CMIA0,
    IRQ_CMIB0,
    IRQ_CMIA0_B1,
    IRQ_TOVI0_1,
    IRQ_CMIA2,
    IRQ_CMIB2,
    IRQ_CMIA2_B3,
    IRQ_TOVI2_3,
    IRQ_DEND0A,
    IRQ_DEND0B,
    IRQ_DEND1A,
    IRQ_DEND1B,
    IRQ_ERI0,
    IRQ_RXI0,
    IRQ_TXI0,
    IRQ_TEI0,
    IRQ_ERI1,
    IRQ_RXI1,
    IRQ_TXI1,
    IRQ_TEI1,
    IRQ_ERI2,
    IRQ_RXI2,
    IRQ_TXI2,
    IRQ_TEI2,
    IRQ_MAX
};

extern IRQ_HANDLER sig_NMI;
extern IRQ_HANDLER sig_INT0;
extern IRQ_HANDLER sig_INT1;
extern IRQ_HANDLER sig_INT2;
extern IRQ_HANDLER sig_INT3;
extern IRQ_HANDLER sig_INT4;
extern IRQ_HANDLER sig_INT5;
extern IRQ_HANDLER sig_WOVI;
extern IRQ_HANDLER sig_CMI;
extern IRQ_HANDLER sig_ADI;
extern IRQ_HANDLER sig_IMIA0;
extern IRQ_HANDLER sig_IMIB0;
extern IRQ_HANDLER sig_OVI0;
extern IRQ_HANDLER sig_IMIA1;
extern IRQ_HANDLER sig_IMIB1;
extern IRQ_HANDLER sig_OVI1;
extern IRQ_HANDLER sig_IMIA2;
extern IRQ_HANDLER sig_IMIB2;
extern IRQ_HANDLER sig_OVI2;
extern IRQ_HANDLER sig_CMIA0;
extern IRQ_HANDLER sig_IMIB0;
extern IRQ_HANDLER sig_CMIA0_B1;
extern IRQ_HANDLER sig_TOVI0_1;
extern IRQ_HANDLER sig_CMIA2;
extern IRQ_HANDLER sig_IMIB2;
extern IRQ_HANDLER sig_CMIA2_B3;
extern IRQ_HANDLER sig_TOVI2_3;
extern IRQ_HANDLER sig_DEND0A;
extern IRQ_HANDLER sig_DEND0B;
extern IRQ_HANDLER sig_DEND1A;
extern IRQ_HANDLER sig_DEND1B;
extern IRQ_HANDLER sig_ERI0;
extern IRQ_HANDLER sig_RXI0;
extern IRQ_HANDLER sig_TXI0;
extern IRQ_HANDLER sig_TEI0;
extern IRQ_HANDLER sig_ERI1;
extern IRQ_HANDLER sig_RXI1;
extern IRQ_HANDLER sig_TXI1;
extern IRQ_HANDLER sig_TEI1;
extern IRQ_HANDLER sig_ERI2;
extern IRQ_HANDLER sig_RXI2;
extern IRQ_HANDLER sig_TXI2;
extern IRQ_HANDLER sig_TEI2;

#endif
