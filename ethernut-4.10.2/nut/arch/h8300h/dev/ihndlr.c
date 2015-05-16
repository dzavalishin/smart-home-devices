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
 * Revision 1.1  2005/07/26 18:02:40  haraldkipp
 * Moved from dev.
 *
 * Revision 1.1  2004/03/16 16:48:27  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 */

/*! 
 * \fn SIG_NMI(void)
 * \brief Non maskable interrupt entry.
 */
IRQ_HANDLER sig_NMI;
SIGNAL(SIG_NMI)
{
    CallHandler(&sig_NMI);
}

/* \fn SIG_INT0(void)
 * \brief External interrupt 0 entry.
 */
IRQ_HANDLER sig_INT0;
SIGNAL(SIG_INT0)
{
    CallHandler(&sig_INT0);
}

/*! \fn SIG_INT1(void)
 * \brief External interrupt 1 entry.
 */
IRQ_HANDLER sig_INT1;
SIGNAL(SIG_INT1)
{
    CallHandler(&sig_INT1);
}

/*! \fn SIG_INT2(void)
 * \brief External interrupt 2 entry.
 */
IRQ_HANDLER sig_INT2;
SIGNAL(SIG_INT2)
{
    CallHandler(&sig_INT2);
}

/*! \fn SIG_INT3(void)
 * \brief External interrupt 3 entry.
 */
IRQ_HANDLER sig_INT3;
SIGNAL(SIG_INT3)
{
    CallHandler(&sig_INT3);
}

/*! \fn SIG_INT4(void)
 * \brief External interrupt 4 entry.
 */
IRQ_HANDLER sig_INT4;
SIGNAL(SIG_INT4)
{
    CallHandler(&sig_INT4);
}

/*! \fn SIG_INT5(void)
 * \brief External interrupt 5 entry.
 */
IRQ_HANDLER sig_INT5;
SIGNAL(SIG_INT5)
{
    CallHandler(&sig_INT5);
}

/*! \fn SIG_WOVI(void)
 * \brief Watchdog timer overflow interrupt entry.
 */
IRQ_HANDLER sig_WOVI;
SIGNAL(SIG_WOVI)
{
    CallHandler(&sig_WOVI);
}

/*! \fn SIG_CMI(void)
 * \brief Compare match interrupt entry.
 */
IRQ_HANDLER sig_CMI;
SIGNAL(SIG_CMI)
{
    CallHandler(&sig_CMI);
}

/*! \fn SIG_ADI(void) 
 * \brief AD convesion end interrupt entry.
 */
IRQ_HANDLER sig_ADI;
SIGNAL(SIG_ADI)
{
    CallHandler(&sig_ADI);
}

/*! \fn SIG_IMIA0(void)
 * \brief Timer 0 compare match A0 interrupt entry.
 */
IRQ_HANDLER sig_IMIA0;
SIGNAL(SIG_IMIA0)
{
    CallHandler(&sig_IMIA0);
}

/*! \fn SIG_IMIB0(void)
 * \brief Timer 0 compare match B0 interrupt entry.
 */
IRQ_HANDLER sig_IMIB0;
SIGNAL(SIG_IMIB0)
{
    CallHandler(&sig_IMIB0);
}

/*! \fn SIG_OVI0(void)
 * \brief Timer 0 overflow interrupt entry.
 */
IRQ_HANDLER sig_OVI0;
SIGNAL(SIG_OVI0)
{
    CallHandler(&sig_OVI0);
}

/*! \fn SIG_IMIA1(void)
 * \brief Timer 1 compare match A0 interrupt entry.
 */
IRQ_HANDLER sig_IMIA1;
SIGNAL(SIG_IMIA1)
{
    CallHandler(&sig_IMIA1);
}

/*! \fn SIG_IMIB1(void)
 * \brief Timer 1 compare match B1 interrupt entry.
 */
IRQ_HANDLER sig_IMIB1;
SIGNAL(SIG_IMIB1)
{
    CallHandler(&sig_IMIB1);
}

/*! \fn SIG_OVI1(void)
 * \brief Timer 1 overflow interrupt entry.
 */
IRQ_HANDLER sig_OVI1;
SIGNAL(SIG_OVI1)
{
    CallHandler(&sig_OVI1);
}

/*! \fn SIG_IMIA2(void)
 * \brief Timer 2 compare match A2 interrupt entry.
 */
IRQ_HANDLER sig_IMIA2;
SIGNAL(SIG_IMIA2)
{
    CallHandler(&sig_IMIA2);
}

/*! \fn SIG_IMIB2(void)
 * \brief Timer 2 compare match B2 interrupt entry.
 */
IRQ_HANDLER sig_IMIB2;
SIGNAL(SIG_IMIB2)
{
    CallHandler(&sig_IMIB2);
}

/*! \fn SIG_OVI2(void)
 * \brief Timer 2 overflow interrupt entry.
 */
IRQ_HANDLER sig_OVI2;
SIGNAL(SIG_OVI2)
{
    CallHandler(&sig_OVI2);
}

/*! \fn SIG_CMIA0(void)
 * \brief 8 bit timer compare match A0 interrupt entry.
 */
IRQ_HANDLER sig_CMIA0;
SIGNAL(SIG_CMIA0)
{
    CallHandler(&sig_CMIA0);
}

/*! \fn SIG_CMIB0(void)
 * \brief 8 bit timer compare match B0 interrupt entry.
 */
IRQ_HANDLER sig_CMIB0;
SIGNAL(SIG_CMIB0)
{
    CallHandler(&sig_CMIB0);
}

/*! \fn SIG_CMIA0_B1(void)
 * \brief 8 bit timer compare match A1/B1 interrupt entry.
 */
IRQ_HANDLER sig_CMIA0_B1;
SIGNAL(SIG_CMIA0_B1)
{
    CallHandler(&sig_CMIA0_B1);
}

/*! \fn SIG_TOVI0_1(void)
 * \brief 8 bit timer overflow 0/1 interrupt entry.
 */
IRQ_HANDLER sig_TOVI0_1;
SIGNAL(SIG_TOVI0_1)
{
    CallHandler(&sig_TOVI0_1);
}

/*! \fn SIG_CMIA2(void)
 * \brief 8 bit timer compare match A2 interrupt entry.
 */
IRQ_HANDLER sig_CMIA2;
SIGNAL(SIG_CMIA2)
{
    CallHandler(&sig_CMIA2);
}

/*! \fn SIG_CMIB2(void)
 * \brief 8 bit timer compare match B2 interrupt entry.
 */
IRQ_HANDLER sig_CMIB2;
SIGNAL(SIG_CMIB2)
{
    CallHandler(&sig_CMIB2);
}

/*! \fn SIG_CMIA2_B3(void)
 * \brief 8 bit timer compare match A2/B3 interrupt entry.
 */
IRQ_HANDLER sig_CMIA2_B3;
SIGNAL(SIG_CMIA2_B3)
{
    CallHandler(&sig_CMIA2_B3);
}

/*! \fn SIG_TOVI2_3(void)
 * \brief 8 bit timer overflow 2/3 interrupt entry.
 */
IRQ_HANDLER sig_TOVI2_3;
SIGNAL(SIG_TOVI2_3)
{
    CallHandler(&sig_TOVI2_3);
}

/*! \fn SIG_DEND0A(void)
 * \brief DMA channel 0A interrupt entry.
 */
IRQ_HANDLER sig_DEND0A;
SIGNAL(SIG_DEND0A)
{
    CallHandler(&sig_DEND0A);
}

/*! \fn SIG_DEND0B(void)
 * \brief DMA channel 0B interrupt entry.
 */
IRQ_HANDLER sig_DEND0B;
SIGNAL(SIG_DEND0B)
{
    CallHandler(&sig_DEND0B);
}

/*! \fn SIG_DEND1A(void)
 * \brief DMA channel 1A interrupt entry.
 */
IRQ_HANDLER sig_DEND1A;
SIGNAL(SIG_DEND1A)
{
    CallHandler(&sig_DEND1A);
}

/*! \fn SIG_DEND1B(void)
 * \brief DMA channel 1B interrupt entry.
 */
IRQ_HANDLER sig_DEND1B;
SIGNAL(SIG_DEND1B)
{
    CallHandler(&sig_DEND1B);
}

/*! \fn SIG_ERI0(void)
 * \brief SCI0 ERI interrupt entry.
 */
IRQ_HANDLER sig_ERI0;
SIGNAL(SIG_ERI0)
{
    CallHandler(&sig_ERI0);
}

/*! \fn SIG_RXI0(void)
 * \brief SCI0 RXI interrupt entry.
 */
IRQ_HANDLER sig_RXI0;
SIGNAL(SIG_RXI0)
{
    CallHandler(&sig_RXI0);
}

/*! \fn SIG_TXI0(void)
 * \brief SCI0 TXI interrupt entry.
 */
IRQ_HANDLER sig_TXI0;
SIGNAL(SIG_TXI0)
{
    CallHandler(&sig_TXI0);
}

/*! \fn SIG_TEI0(void)
 * \bried SCI0 TEI interrupt entry.
 */
IRQ_HANDLER sig_TEI0;
SIGNAL(SIG_TEI0)
{
    CallHandler(&sig_TEI0);
}

/*! \fn SIG_ERI1(void)
 * \brief SCI1 ERI interrupt entry.
 */
IRQ_HANDLER sig_ERI1;
SIGNAL(SIG_ERI1)
{
    CallHandler(&sig_ERI1);
}

/*! \fn SIG_RXI1(void)
 * \brief SCI1 RXI interrupt entry.
 */
IRQ_HANDLER sig_RXI1;
SIGNAL(SIG_RXI1)
{
    CallHandler(&sig_RXI1);
}

/*! \fn SIG_TXI1(void)
 * \brief SCI1 TXI interrupt entry.
 */
IRQ_HANDLER sig_TXI1;
SIGNAL(SIG_TXI1)
{
    CallHandler(&sig_TXI1);
}

/*! \fn SIG_TEI1(void)
 * \bried SCI1 TEI interrupt entry.
 */
IRQ_HANDLER sig_TEI1;
SIGNAL(SIG_TEI1)
{
    CallHandler(&sig_TEI1);
}

/*! \fn SIG_ERI2(void)
 * \brief SCI2 ERI interrupt entry.
 */
IRQ_HANDLER sig_ERI2;
SIGNAL(SIG_ERI2)
{
    CallHandler(&sig_ERI2);
}

/*! \fn SIG_RXI2(void)
 * \brief SCI2 RXI interrupt entry.
 */
IRQ_HANDLER sig_RXI2;
SIGNAL(SIG_RXI2)
{
    CallHandler(&sig_RXI2);
}

/*! \fn SIG_TXI2(void)
 * \brief SCI2 TXI interrupt entry.
 */
IRQ_HANDLER sig_TXI2;
SIGNAL(SIG_TXI2)
{
    CallHandler(&sig_TXI2);
}

/*! \fn SIG_TEI2(void)
 * \bried SCI2 TEI interrupt entry.
 */
IRQ_HANDLER sig_TEI2;
SIGNAL(SIG_TEI2)
{
    CallHandler(&sig_TEI2);
}

/*!
 * \brief System entry routine.
 *
 * This function initialises .data and .bss segments and pass control
 * to NutMain. It is defined in nutentry.S.
 */
extern void NutEntry(void);

typedef void (*fp) (void);

const fp NutInterruptVector[] __attribute__ ((section(".vects"))) = {
    NutEntry,                   /* Vector 0,  0x0000, Power on          */
        (fp) (0),               /* Vector 1,  0x0004, Reserved          */
        (fp) (0),               /* Vector 2,  0x0008, Reserved          */
        (fp) (0),               /* Vector 3,  0x000c, Reserved          */
        (fp) (0),               /* Vector 4,  0x0010, Reserved          */
        (fp) (0),               /* Vector 5,  0x0014, Reserved          */
        (fp) (0),               /* Vector 6,  0x0018, Reserved          */
        SIG_NMI,                /* Vector 7,  0x001c, NMI               */
        (fp) (0),               /* Vector 8,  0x0020, Reserved          */
        (fp) (0),               /* Vector 9,  0x0024, Reserved          */
        (fp) (0),               /* Vector 10, 0x0028, Reserved          */
        (fp) (0),               /* Vector 11, 0x002c, Reserved          */
        SIG_INT0,               /* Vector 12, 0x0030, IRQ0              */
        SIG_INT1,               /* Vector 13, 0x0034, IRQ1              */
        SIG_INT2,               /* Vector 14, 0x0038, IRQ2              */
        SIG_INT3,               /* Vector 15, 0x003c, IRQ3              */
        SIG_INT4,               /* Vector 16, 0x0040, IRQ4              */
        SIG_INT5,               /* Vector 17, 0x0044, IRQ5              */
        (fp) (0),               /* Vector 18, 0x0048, Reserved          */
        (fp) (0),               /* Vector 19, 0x004c, Reserved          */
        SIG_WOVI,               /* Vector 20, 0x0050, WOVI              */
        SIG_CMI,                /* Vector 21, 0x0054, CMI               */
        (fp) (0),               /* Vector 22, 0x0058, Reserved          */
        SIG_ADI,                /* Vector 23, 0x005c, ADI               */
        SIG_IMIA0,              /* Vector 24, 0x0060, IMIA0             */
        SIG_IMIB0,              /* Vector 25, 0x0064, IMIB0             */
        SIG_OVI0,               /* Vector 26, 0x0068, OVI0              */
        (fp) (0),               /* Vector 27, 0x006c, Reserved          */
        SIG_IMIA1,              /* Vector 28, 0x0070, IMIA1             */
        SIG_IMIB1,              /* Vector 29, 0x0074, IMIB1             */
        SIG_OVI1,               /* Vector 30, 0x0078, OVI1              */
        (fp) (0),               /* Vector 31, 0x007c, Reserved          */
        SIG_IMIA2,              /* Vector 32, 0x0080, IMIA2             */
        SIG_IMIB2,              /* Vector 33, 0x0084, IMIB2             */
        SIG_OVI2,               /* Vector 34, 0x0088, OVI2              */
        (fp) (0),               /* Vector 35, 0x008c, Reserved          */
        SIG_CMIA0,              /* Vector 36, 0x0090, CMIA0             */
        SIG_CMIB0,              /* Vector 37, 0x0094, CMIB0             */
        SIG_CMIA0_B1,           /* Vector 38, 0x0098, CMIA0/CMIB1       */
        SIG_TOVI0_1,            /* Vector 39, 0x009c, TOVI0/TOVI1       */
        SIG_CMIA2,              /* Vector 40, 0x00a0, CMIA2             */
        SIG_CMIB2,              /* Vector 41, 0x00a4, CMIB2             */
        SIG_CMIA2_B3,           /* Vector 42, 0x00a8, CMIA2/CMIB3       */
        SIG_TOVI2_3,            /* Vector 43, 0x00ac, TOVI2/TOVI3       */
        SIG_DEND0A,             /* Vector 44, 0x00b0, DEND0A            */
        SIG_DEND0B,             /* Vector 45, 0x00b4, DEND0B            */
        SIG_DEND1A,             /* Vector 46, 0x00b8, DEND1A            */
        SIG_DEND1B,             /* Vector 47, 0x00bc, DEND1B            */
        (fp) (0),               /* Vector 48, 0x00c0, Reserved          */
        (fp) (0),               /* Vector 49, 0x00c4, Reserved          */
        (fp) (0),               /* Vector 50, 0x00c8, Reserved          */
        (fp) (0),               /* Vector 51, 0x00cc, Reserved          */
        SIG_ERI0,               /* Vector 52, 0x00d0, ERI0              */
        SIG_RXI0,               /* Vector 53, 0x00d4, RXI0              */
        SIG_TXI0,               /* Vector 54, 0x00d8, TXI0              */
        SIG_TEI0,               /* Vector 55, 0x00dc, TEI0              */
        SIG_ERI1,               /* Vector 56, 0x00e0, ERI1              */
        SIG_RXI1,               /* Vector 57, 0x00e4, RXI1              */
        SIG_TXI1,               /* Vector 58, 0x00e8, TXI1              */
        SIG_TEI1,               /* Vector 59, 0x00ec, TEI1              */
        SIG_ERI2,               /* Vector 60, 0x00f0, ERI2              */
        SIG_RXI2,               /* Vector 61, 0x00f4, RXI2              */
        SIG_TXI2,               /* Vector 62, 0x00f8, TXI2              */
        SIG_TEI2                /* Vector 63, 0x00fc, TEI2              */
};

/*@}*/
