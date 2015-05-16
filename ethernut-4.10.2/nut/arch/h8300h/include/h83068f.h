#ifndef __INCLUDE_H83068F_H__
#define __INCLUDE_H83068F_H__

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
 * This file is based on 3068s.h which can be freely downloaded from
 * http://www.renesas.com/eng/products/mpumcu/tool/crosstool/iodef/index.html.
 * 
 * I have added lacking definition of ADRCR register in structure st_bsc
 * (bus controler), chanded "int" to "short" in some places in order to make
 * structs/unions be compatible with gcc's "-mint32" option and added appropriate
 * ifdefs in order to avoid including the contents of this file more than once.
 *
 * Jan Dubiec <jdx@slackware.pl>
 *
 */

/*
 * $Log$
 * Revision 1.2  2004/03/19 18:21:12  jdubiec
 * Added comment about the origin of this file.
 *
 * Revision 1.1  2004/03/16 16:48:26  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 */

/************************************************************************/
/*      H8/3068 Series Include File                        Ver 2.0      */
/************************************************************************/

union un_p1dr {                 /* union P1DR   */
    unsigned char BYTE;         /*  Byte Access */
    struct {                    /*  Bit  Access */
        unsigned char B7:1;     /*    Bit 7     */
        unsigned char B6:1;     /*    Bit 6     */
        unsigned char B5:1;     /*    Bit 5     */
        unsigned char B4:1;     /*    Bit 4     */
        unsigned char B3:1;     /*    Bit 3     */
        unsigned char B2:1;     /*    Bit 2     */
        unsigned char B1:1;     /*    Bit 1     */
        unsigned char B0:1;     /*    Bit 0     */
    } BIT;                      /*              */
};                              /*              */
union un_p2pcr {                /* union P2PCR  */
    unsigned char BYTE;         /*  Byte Access */
    struct {                    /*  Bit  Access */
        unsigned char B7:1;     /*    Bit 7     */
        unsigned char B6:1;     /*    Bit 6     */
        unsigned char B5:1;     /*    Bit 5     */
        unsigned char B4:1;     /*    Bit 4     */
        unsigned char B3:1;     /*    Bit 3     */
        unsigned char B2:1;     /*    Bit 2     */
        unsigned char B1:1;     /*    Bit 1     */
        unsigned char B0:1;     /*    Bit 0     */
    } BIT;                      /*              */
};                              /*              */
union un_p2dr {                 /* union P2DR   */
    unsigned char BYTE;         /*  Byte Access */
    struct {                    /*  Bit  Access */
        unsigned char B7:1;     /*    Bit 7     */
        unsigned char B6:1;     /*    Bit 6     */
        unsigned char B5:1;     /*    Bit 5     */
        unsigned char B4:1;     /*    Bit 4     */
        unsigned char B3:1;     /*    Bit 3     */
        unsigned char B2:1;     /*    Bit 2     */
        unsigned char B1:1;     /*    Bit 1     */
        unsigned char B0:1;     /*    Bit 0     */
    } BIT;                      /*              */
};                              /*              */
union un_p3dr {                 /* union P3DR   */
    unsigned char BYTE;         /*  Byte Access */
    struct {                    /*  Bit  Access */
        unsigned char B7:1;     /*    Bit 7     */
        unsigned char B6:1;     /*    Bit 6     */
        unsigned char B5:1;     /*    Bit 5     */
        unsigned char B4:1;     /*    Bit 4     */
        unsigned char B3:1;     /*    Bit 3     */
        unsigned char B2:1;     /*    Bit 2     */
        unsigned char B1:1;     /*    Bit 1     */
        unsigned char B0:1;     /*    Bit 0     */
    } BIT;                      /*              */
};                              /*              */
union un_p4pcr {                /* union P4PCR  */
    unsigned char BYTE;         /*  Byte Access */
    struct {                    /*  Bit  Access */
        unsigned char B7:1;     /*    Bit 7     */
        unsigned char B6:1;     /*    Bit 6     */
        unsigned char B5:1;     /*    Bit 5     */
        unsigned char B4:1;     /*    Bit 4     */
        unsigned char B3:1;     /*    Bit 3     */
        unsigned char B2:1;     /*    Bit 2     */
        unsigned char B1:1;     /*    Bit 1     */
        unsigned char B0:1;     /*    Bit 0     */
    } BIT;                      /*              */
};                              /*              */
union un_p4dr {                 /* union P4DR   */
    unsigned char BYTE;         /*  Byte Access */
    struct {                    /*  Bit  Access */
        unsigned char B7:1;     /*    Bit 7     */
        unsigned char B6:1;     /*    Bit 6     */
        unsigned char B5:1;     /*    Bit 5     */
        unsigned char B4:1;     /*    Bit 4     */
        unsigned char B3:1;     /*    Bit 3     */
        unsigned char B2:1;     /*    Bit 2     */
        unsigned char B1:1;     /*    Bit 1     */
        unsigned char B0:1;     /*    Bit 0     */
    } BIT;                      /*              */
};                              /*              */
union un_p5pcr {                /* union P5PCR  */
    unsigned char BYTE;         /*  Byte Access */
    struct {                    /*  Bit  Access */
        unsigned char:4;        /*              */
        unsigned char B3:1;     /*    Bit 3     */
        unsigned char B2:1;     /*    Bit 2     */
        unsigned char B1:1;     /*    Bit 1     */
        unsigned char B0:1;     /*    Bit 0     */
    } BIT;                      /*              */
};                              /*              */
union un_p5dr {                 /* union P5DR   */
    unsigned char BYTE;         /*  Byte Access */
    struct {                    /*  Bit  Access */
        unsigned char:4;        /*              */
        unsigned char B3:1;     /*    Bit 3     */
        unsigned char B2:1;     /*    Bit 2     */
        unsigned char B1:1;     /*    Bit 1     */
        unsigned char B0:1;     /*    Bit 0     */
    } BIT;                      /*              */
};                              /*              */
union un_p6dr {                 /* union P6DR   */
    unsigned char BYTE;         /*  Byte Access */
    struct {                    /*  Bit  Access */
        unsigned char B7:1;     /*    Bit 7     */
        unsigned char B6:1;     /*    Bit 6     */
        unsigned char B5:1;     /*    Bit 5     */
        unsigned char B4:1;     /*    Bit 4     */
        unsigned char B3:1;     /*    Bit 3     */
        unsigned char B2:1;     /*    Bit 2     */
        unsigned char B1:1;     /*    Bit 1     */
        unsigned char B0:1;     /*    Bit 0     */
    } BIT;                      /*              */
};                              /*              */
union un_p7dr {                 /* union P7DR   */
    unsigned char BYTE;         /*  Byte Access */
    struct {                    /*  Bit  Access */
        unsigned char B7:1;     /*    Bit 7     */
        unsigned char B6:1;     /*    Bit 6     */
        unsigned char B5:1;     /*    Bit 5     */
        unsigned char B4:1;     /*    Bit 4     */
        unsigned char B3:1;     /*    Bit 3     */
        unsigned char B2:1;     /*    Bit 2     */
        unsigned char B1:1;     /*    Bit 1     */
        unsigned char B0:1;     /*    Bit 0     */
    } BIT;                      /*              */
};                              /*              */
union un_p8dr {                 /* union P8DR   */
    unsigned char BYTE;         /*  Byte Access */
    struct {                    /*  Bit  Access */
        unsigned char:3;        /*              */
        unsigned char B4:1;     /*    Bit 4     */
        unsigned char B3:1;     /*    Bit 3     */
        unsigned char B2:1;     /*    Bit 2     */
        unsigned char B1:1;     /*    Bit 1     */
        unsigned char B0:1;     /*    Bit 0     */
    } BIT;                      /*              */
};                              /*              */
union un_p9dr {                 /* union P9DR   */
    unsigned char BYTE;         /*  Byte Access */
    struct {                    /*  Bit  Access */
        unsigned char:2;        /*              */
        unsigned char B5:1;     /*    Bit 5     */
        unsigned char B4:1;     /*    Bit 4     */
        unsigned char B3:1;     /*    Bit 3     */
        unsigned char B2:1;     /*    Bit 2     */
        unsigned char B1:1;     /*    Bit 1     */
        unsigned char B0:1;     /*    Bit 0     */
    } BIT;                      /*              */
};                              /*              */
union un_padr {                 /* union PADR   */
    unsigned char BYTE;         /*  Byte Access */
    struct {                    /*  Bit  Access */
        unsigned char B7:1;     /*    Bit 7     */
        unsigned char B6:1;     /*    Bit 6     */
        unsigned char B5:1;     /*    Bit 5     */
        unsigned char B4:1;     /*    Bit 4     */
        unsigned char B3:1;     /*    Bit 3     */
        unsigned char B2:1;     /*    Bit 2     */
        unsigned char B1:1;     /*    Bit 1     */
        unsigned char B0:1;     /*    Bit 0     */
    } BIT;                      /*              */
};                              /*              */
union un_pbdr {                 /* union PBDR   */
    unsigned char BYTE;         /*  Byte Access */
    struct {                    /*  Bit  Access */
        unsigned char B7:1;     /*    Bit 7     */
        unsigned char B6:1;     /*    Bit 6     */
        unsigned char B5:1;     /*    Bit 5     */
        unsigned char B4:1;     /*    Bit 4     */
        unsigned char B3:1;     /*    Bit 3     */
        unsigned char B2:1;     /*    Bit 2     */
        unsigned char B1:1;     /*    Bit 1     */
        unsigned char B0:1;     /*    Bit 0     */
    } BIT;                      /*              */
};                              /*              */
union un_mdcr {                 /* union MDCR   */
    unsigned char BYTE;         /*  Byte Access */
    struct {                    /*  Bit  Access */
        unsigned char:5;        /*              */
        unsigned char MDS:3;    /*    MDS       */
    } BIT;                      /*              */
};                              /*              */
union un_syscr {                /* union SYSCR  */
    unsigned char BYTE;         /*  Byte Access */
    struct {                    /*  Bit  Access */
        unsigned char SSBY:1;   /*    SSBY      */
        unsigned char STS:3;    /*    STS       */
        unsigned char UE:1;     /*    UE        */
        unsigned char NMIEG:1;  /*    NMIEG     */
        unsigned char SSOE:1;   /*    SSOE      */
        unsigned char RAME:1;   /*    RAME      */
    } BIT;                      /*              */
};                              /*              */
union un_divcr {                /* union DIVCR  */
    unsigned char BYTE;         /*  Byte Access */
    struct {                    /*  Bit  Access */
        unsigned char:6;        /*              */
        unsigned char DIV:2;    /*    DIV       */
    } BIT;                      /*              */
};                              /*              */
union un_mstcr {                /* union MSTCR  */
    unsigned short WORD;        /*  Word Access */
    struct {                    /*  Byte Access */
        unsigned char H;        /*    High      */
        unsigned char L;        /*    Low       */
    } BYTE;                     /*              */
    struct {                    /*  Bit  Access */
        unsigned char PSTOP:1;  /*    PSTOP     */
        unsigned char:4;        /*              */
        unsigned char _SCI2:1;  /*    MSTPH2    */
        unsigned char _SCI1:1;  /*    MSTPH1    */
        unsigned char _SCI0:1;  /*    MSTPH0    */
        unsigned char _DMAC:1;  /*    MSTPL7    */
        unsigned char:1;        /*              */
        unsigned char _DRAM:1;  /*    MSTPL5    */
        unsigned char _ITU:1;   /*    MSTPL4    */
        unsigned char _TMR01:1; /*    MSTPL3    */
        unsigned char _TMR23:1; /*    MSTPL2    */
        unsigned char:1;        /*              */
        unsigned char _AD:1;    /*    MSTPL0    */
    } BIT;                      /*              */
};                              /*              */
struct st_bsc {                 /* struct BSC   */
    char wk1;                   /*              */
    union {                     /* BRCR         */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char A23E:1;       /*    A23E      */
            unsigned char A22E:1;       /*    A22E      */
            unsigned char A21E:1;       /*    A21E      */
            unsigned char A20E:1;       /*    A20E      */
            unsigned char:3;    /*              */
            unsigned char BRLE:1;       /*    BRLE      */
        } BIT;                  /*              */
    } BRCR;                     /*              */
    char wk2[10];               /*              */
    union {                     /* ADRCR        */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char:7;    /*              */
            unsigned char ADRCTL:1;     /*    ADRCTL    */
        } BIT;                  /*              */
    } ADRCR;                    /*              */
    union {                     /* CSCR         */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char CS7E:1;       /*    CS7E      */
            unsigned char CS6E:1;       /*    CS6E      */
            unsigned char CS5E:1;       /*    CS5E      */
            unsigned char CS4E:1;       /*    CS4E      */
        } BIT;                  /*              */
    } CSCR;                     /*              */
    union {                     /* ABWCR        */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char ABW7:1;       /*    ABW7      */
            unsigned char ABW6:1;       /*    ABW6      */
            unsigned char ABW5:1;       /*    ABW5      */
            unsigned char ABW4:1;       /*    ABW4      */
            unsigned char ABW3:1;       /*    ABW3      */
            unsigned char ABW2:1;       /*    ABW2      */
            unsigned char ABW1:1;       /*    ABW1      */
            unsigned char ABW0:1;       /*    ABW0      */
        } BIT;                  /*              */
    } ABWCR;                    /*              */
    union {                     /* ASTCR        */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char AST7:1;       /*    AST7      */
            unsigned char AST6:1;       /*    AST6      */
            unsigned char AST5:1;       /*    AST5      */
            unsigned char AST4:1;       /*    AST4      */
            unsigned char AST3:1;       /*    AST3      */
            unsigned char AST2:1;       /*    AST2      */
            unsigned char AST1:1;       /*    AST1      */
            unsigned char AST0:1;       /*    AST0      */
        } BIT;                  /*              */
    } ASTCR;                    /*              */
    union {                     /* WCR          */
        unsigned short WORD;    /*  Word Access */
        struct {                /*  Byte Access */
            unsigned char H;    /*    WCRH      */
            unsigned char L;    /*    WCRL      */
        } BYTE;                 /*              */
        struct {                /*  Bit  Access */
            unsigned char W7:2; /*    W7        */
            unsigned char W6:2; /*    W6        */
            unsigned char W5:2; /*    W5        */
            unsigned char W4:2; /*    W4        */
            unsigned char W3:2; /*    W3        */
            unsigned char W2:2; /*    W2        */
            unsigned char W1:2; /*    W1        */
            unsigned char W0:2; /*    W0        */
        } BIT;                  /*              */
    } WCR;                      /*              */
    union {                     /* BCR          */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char ICIS1:1;      /*    ICIS1     */
            unsigned char ICIS0:1;      /*    ICIS0     */
            unsigned char BROME:1;      /*    BROME     */
            unsigned char BRSTS1:1;     /*    BRSTS1    */
            unsigned char BRSTS0:1;     /*    BRSTS0    */
            unsigned char:1;    /*              */
            unsigned char RDEA:1;       /*    RDEA      */
            unsigned char WAITE:1;      /*    WAITE     */
        } BIT;                  /*              */
    } BCR;                      /*              */
    char wk3;                   /*              */
    union {                     /* DRCRA        */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char DRAS:3;       /*    DRAS      */
            unsigned char:1;    /*              */
            unsigned char BE:1; /*    BE        */
            unsigned char RDM:1;        /*    RDM       */
            unsigned char SRFMD:1;      /*    SRFMD     */
            unsigned char RFSHE:1;      /*    RFSHE     */
        } BIT;                  /*              */
    } DRCRA;                    /*              */
    union {                     /* DRCRB        */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char MXC:2;        /*    MXC       */
            unsigned char CSEL:1;       /*    CSEL      */
            unsigned char RCYCE:1;      /*    RCYCE     */
            unsigned char:1;    /*              */
            unsigned char _TPC:1;       /*    TPC       */
            unsigned char RCW:1;        /*    RCW       */
            unsigned char RLW:1;        /*    RLW       */
        } BIT;                  /*              */
    } DRCRB;                    /*              */
    union {                     /* RTMCSR       */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char CMF:1;        /*    CMF       */
            unsigned char CMIE:1;       /*    CMIE      */
            unsigned char CKS:3;        /*    CKS       */
        } BIT;                  /*              */
    } RTMCSR;                   /*              */
    unsigned char RTCNT;        /* RTCNT        */
    unsigned char RTCOR;        /* RTCOR        */
};                              /*              */
struct st_intc {                /* struct INTC  */
    union {                     /* ISCR         */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char:2;    /*              */
            unsigned char IRQ5SC:1;     /*    IRQ5SC    */
            unsigned char IRQ4SC:1;     /*    IRQ4SC    */
            unsigned char IRQ3SC:1;     /*    IRQ3SC    */
            unsigned char IRQ2SC:1;     /*    IRQ2SC    */
            unsigned char IRQ1SC:1;     /*    IRQ1SC    */
            unsigned char IRQ0SC:1;     /*    IRQ0SC    */
        } BIT;                  /*              */
    } ISCR;                     /*              */
    union {                     /* IER          */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char:2;    /*              */
            unsigned char IRQ5E:1;      /*    IRQ5E     */
            unsigned char IRQ4E:1;      /*    IRQ4E     */
            unsigned char IRQ3E:1;      /*    IRQ3E     */
            unsigned char IRQ2E:1;      /*    IRQ2E     */
            unsigned char IRQ1E:1;      /*    IRQ1E     */
            unsigned char IRQ0E:1;      /*    IRQ0E     */
        } BIT;                  /*              */
    } IER;                      /*              */
    union {                     /* ISR          */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char:2;    /*              */
            unsigned char IRQ5F:1;      /*    IRQ5F     */
            unsigned char IRQ4F:1;      /*    IRQ4F     */
            unsigned char IRQ3F:1;      /*    IRQ3F     */
            unsigned char IRQ2F:1;      /*    IRQ2F     */
            unsigned char IRQ1F:1;      /*    IRQ1F     */
            unsigned char IRQ0F:1;      /*    IRQ0F     */
        } BIT;                  /*              */
    } ISR;                      /*              */
    char wk;                    /*              */
    union {                     /* IPRA         */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char _IRQ0:1;      /*    IRQ0      */
            unsigned char _IRQ1:1;      /*    IRQ1      */
            unsigned char _IRQ23:1;     /*    IRQ2,IRQ3 */
            unsigned char _IRQ45:1;     /*    IRQ4,IRQ5 */
            unsigned char _WDT:1;       /* WDT,DRAM,A/D */
            unsigned char _ITU0:1;      /*    ITU0      */
            unsigned char _ITU1:1;      /*    ITU1      */
            unsigned char _ITU2:1;      /*    ITU2      */
        } BIT;                  /*              */
    } IPRA;                     /*              */
    union {                     /* IPRB         */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char _TMR01:1;     /*    TMR0,1    */
            unsigned char _TMR23:1;     /*    TMR2,3    */
            unsigned char _DMAC:1;      /*    DMAC      */
            unsigned char:1;    /*              */
            unsigned char _SCI0:1;      /*    SCI0      */
            unsigned char _SCI1:1;      /*    SCI1      */
            unsigned char _SCI2:1;      /*    SCI2      */
        } BIT;                  /*              */
    } IPRB;                     /*              */
};                              /*              */
#if __CPU__==2                  /* Normal Mode  */
struct st_sam {                 /* struct DMAC  */
    char wk[2];                 /*              */
    void *MAR;                  /* MAR          */
    unsigned short ETCR;        /* ETCR         */
    unsigned char IOAR;         /* IOAR         */
    union {                     /* DTCR         */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char DTE:1;        /*    DTE       */
            unsigned char DTSZ:1;       /*    DTSZ      */
            unsigned char DTID:1;       /*    DTID      */
            unsigned char RPE:1;        /*    RPE       */
            unsigned char DTIE:1;       /*    DTIE      */
            unsigned char DTS:3;        /*    DTS       */
        } BIT;                  /*              */
    } DTCR;                     /*              */
};                              /*              */
struct st_fam {                 /* struct DMAC  */
    char wk1[2];                /*              */
    void *MARA;                 /* MARA         */
    unsigned short ETCRA;       /* ETCRA        */
    char wk2;                   /*              */
    union {                     /* DTCRA        */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char DTE:1;        /*    DTE       */
            unsigned char DTSZ:1;       /*    DTSZ      */
            unsigned char SAID:1;       /*    SAID      */
            unsigned char SAIDE:1;      /*    SAIDE     */
            unsigned char DTIE:1;       /*    DTIE      */
            unsigned char DTS:3;        /*    DTS       */
        } BIT;                  /*              */
    } DTCRA;                    /*              */
    char wk3[2];                /*              */
    void *MARB;                 /* MARB         */
    unsigned short ETCRB;       /* ETCRB        */
    char wk4;                   /*              */
    union {                     /* DTCRB        */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char DTME:1;       /*    DTME      */
            unsigned char:1;    /*              */
            unsigned char DAID:1;       /*    DAID      */
            unsigned char DAIDE:1;      /*    DAIDE     */
            unsigned char TMS:1;        /*    TMS       */
            unsigned char DTS:3;        /*    DTS       */
        } BIT;                  /*              */
    } DTCRB;                    /*              */
};                              /*              */
#else                           /* Advanced Mode */
struct st_sam {                 /* struct DMAC  */
    void *MAR;                  /* MAR          */
    unsigned short ETCR;        /* ETCR         */
    unsigned char IOAR;         /* IOAR         */
    union {                     /* DTCR         */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char DTE:1;        /*    DTE       */
            unsigned char DTSZ:1;       /*    DTSZ      */
            unsigned char DTID:1;       /*    DTID      */
            unsigned char RPE:1;        /*    RPE       */
            unsigned char DTIE:1;       /*    DTIE      */
            unsigned char DTS:3;        /*    DTS       */
        } BIT;                  /*              */
    } DTCR;                     /*              */
};                              /*              */
struct st_fam {                 /* struct DMAC  */
    void *MARA;                 /* MARA         */
    unsigned short ETCRA;       /* ETCRA        */
    char wk1;                   /*              */
    union {                     /* DTCRA        */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char DTE:1;        /*    DTE       */
            unsigned char DTSZ:1;       /*    DTSZ      */
            unsigned char SAID:1;       /*    SAID      */
            unsigned char SAIDE:1;      /*    SAIDE     */
            unsigned char DTIE:1;       /*    DTIE      */
            unsigned char DTS:3;        /*    DTS       */
        } BIT;                  /*              */
    } DTCRA;                    /*              */
    void *MARB;                 /* MARB         */
    unsigned short ETCRB;       /* ETCRB        */
    char wk2;                   /*              */
    union {                     /* DTCRB        */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char DTME:1;       /*    DTME      */
            unsigned char:1;    /*              */
            unsigned char DAID:1;       /*    DAID      */
            unsigned char DAIDE:1;      /*    DAIDE     */
            unsigned char TMS:1;        /*    TMS       */
            unsigned char DTS:3;        /*    DTS       */
        } BIT;                  /*              */
    } DTCRB;                    /*              */
};                              /*              */
#endif                          /*              */
struct st_flash {               /* struct FLASH */
    union {                     /* FLMCR1       */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char FWE:1;        /*    FWE       */
            unsigned char SWE:1;        /*    SWE       */
            unsigned char ESU:1;        /*    ESU       */
            unsigned char PSU:1;        /*    PSU       */
            unsigned char EV:1; /*    EV        */
            unsigned char PV:1; /*    PV        */
            unsigned char E:1;  /*    E         */
            unsigned char P:1;  /*    P         */
        } BIT;                  /*              */
    } FLMCR1;                   /*              */
    union {                     /* FLMCR2       */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char FLER:1;       /*    FLER      */
        } BIT;                  /*              */
    } FLMCR2;                   /*              */
    union {                     /* EBR1         */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char EB7:1;        /*    EB7       */
            unsigned char EB6:1;        /*    EB6       */
            unsigned char EB5:1;        /*    EB5       */
            unsigned char EB4:1;        /*    EB4       */
            unsigned char EB3:1;        /*    EB3       */
            unsigned char EB2:1;        /*    EB2       */
            unsigned char EB1:1;        /*    EB1       */
            unsigned char EB0:1;        /*    EB0       */
        } BIT;                  /*              */
    } EBR1;                     /*              */
    union {                     /* EBR2         */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char:2;    /*              */
            unsigned char EB13:1;       /*    EB13      */
            unsigned char EB12:1;       /*    EB12      */
            unsigned char EB11:1;       /*    EB11      */
            unsigned char EB10:1;       /*    EB10      */
            unsigned char EB9:1;        /*    EB9       */
            unsigned char EB8:1;        /*    EB8       */
        } BIT;                  /*              */
    } EBR2;                     /*              */
    char wk[67];                /*              */
    union {                     /* RAMCR        */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char:4;    /*              */
            unsigned char RAMS:1;       /*    RAMS      */
            unsigned char RAM:3;        /*    RAM       */
        } BIT;                  /*              */
    } RAMCR;                    /*              */
};                              /*              */
struct st_itu {                 /* struct ITU   */
    union {                     /* TSTR         */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char:5;    /*              */
            unsigned char STR2:1;       /*    STR2      */
            unsigned char STR1:1;       /*    STR1      */
            unsigned char STR0:1;       /*    STR0      */
        } BIT;                  /*              */
    } TSTR;                     /*              */
    union {                     /* TSNC         */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char:5;    /*              */
            unsigned char SYNC2:1;      /*    SYNC2     */
            unsigned char SYNC1:1;      /*    SYNC1     */
            unsigned char SYNC0:1;      /*    SYNC0     */
        } BIT;                  /*              */
    } TSNC;                     /*              */
    union {                     /* TMDR         */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char:1;    /*              */
            unsigned char MDF:1;        /*    MDF       */
            unsigned char FDIR:1;       /*    FDIR      */
            unsigned char:2;    /*              */
            unsigned char PWM2:1;       /*    PWM2      */
            unsigned char PWM1:1;       /*    PWM1      */
            unsigned char PWM0:1;       /*    PWM0      */
        } BIT;                  /*              */
    } TMDR;                     /*              */
    union {                     /* TOLR         */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char:2;    /*              */
            unsigned char TOB2:1;       /*    TOB2      */
            unsigned char TOA2:1;       /*    TOA2      */
            unsigned char TOB1:1;       /*    TOB1      */
            unsigned char TOA1:1;       /*    TOA1      */
            unsigned char TOB0:1;       /*    TOB0      */
            unsigned char TOA0:1;       /*    TOA0      */
        } BIT;                  /*              */
    } TOLR;                     /*              */
    union {                     /* TISRA        */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char:1;    /*              */
            unsigned char IMIEA2:1;     /*    IMIEA2    */
            unsigned char IMIEA1:1;     /*    IMIEA2    */
            unsigned char IMIEA0:1;     /*    IMIEA2    */
            unsigned char:1;    /*              */
            unsigned char IMFA2:1;      /*    IMFA2     */
            unsigned char IMFA1:1;      /*    IMFA1     */
            unsigned char IMFA0:1;      /*    IMFA0     */
        } BIT;                  /*              */
    } TISRA;                    /*              */
    union {                     /* TISRB        */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char:1;    /*              */
            unsigned char IMIEB2:1;     /*    IMIEB2    */
            unsigned char IMIEB1:1;     /*    IMIEB2    */
            unsigned char IMIEB0:1;     /*    IMIEB2    */
            unsigned char:1;    /*              */
            unsigned char IMFB2:1;      /*    IMFB2     */
            unsigned char IMFB1:1;      /*    IMFB1     */
            unsigned char IMFB0:1;      /*    IMFB0     */
        } BIT;                  /*              */
    } TISRB;                    /*              */
    union {                     /* TISRC        */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char:1;    /*              */
            unsigned char OVIE2:1;      /*    OVIE2     */
            unsigned char OVIE1:1;      /*    OVIE2     */
            unsigned char OVIE0:1;      /*    OVIE2     */
            unsigned char:1;    /*              */
            unsigned char OVF2:1;       /*    OVF2      */
            unsigned char OVF1:1;       /*    OVF1      */
            unsigned char OVF0:1;       /*    OVF0      */
        } BIT;                  /*              */
    } TISRC;                    /*              */
};                              /*              */
struct st_itu0 {                /* struct ITU0  */
    union {                     /* TCR          */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char:1;    /*              */
            unsigned char CCLR:2;       /*    CCLR      */
            unsigned char CKEG:2;       /*    CKEG      */
            unsigned char TPSC:3;       /*    TPSC      */
        } BIT;                  /*              */
    } TCR;                      /*              */
    union {                     /* TIOR         */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char:1;    /*              */
            unsigned char IOB:3;        /*    IOB       */
            unsigned char:1;    /*              */
            unsigned char IOA:3;        /*    IOA       */
        } BIT;                  /*              */
    } TIOR;                     /*              */
    unsigned short TCNT;        /* TCNT         */
    unsigned short GRA;         /* GRA          */
    unsigned short GRB;         /* GRB          */
};                              /*              */
union un_wdt {                  /* union WDT    */
    struct {                    /* Read  Access */
        union {                 /* TCSR         */
            unsigned char BYTE; /*  Byte Access */
            struct {            /*  Bit  Access */
                unsigned char OVF:1;    /*    OVF       */
                unsigned char WTIT:1;   /*    WT/IT     */
                unsigned char TME:1;    /*    TME       */
                unsigned char:2;        /*              */
                unsigned char CKS:3;    /*    CKS       */
            } BIT;              /*              */
        } TCSR;                 /*              */
        unsigned char TCNT;     /* TCNT         */
        char wk;                /*              */
        union {                 /* RSTCSR       */
            unsigned char BYTE; /*  Byte Access */
            struct {            /*              */
                unsigned char WRST:1;   /*    WSRT      */
            } BIT;              /*              */
        } RSTCSR;               /*              */
    } READ;                     /*              */
    struct {                    /* Write Access */
        unsigned short TCSR;    /* TCSR/TCNT    */
        unsigned short RSTCSR;  /* RSTCSR       */
    } WRITE;                    /*              */
};                              /*              */
struct st_tmr01 {               /* struct TMR01 */
    union {                     /* TCR0         */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char CMIEB:1;      /*    CMIEB     */
            unsigned char CMIEA:1;      /*    CMIEA     */
            unsigned char OVIE:1;       /*    OVIE      */
            unsigned char CCLR:2;       /*    CCLR      */
            unsigned char CKS:3;        /*    CKS       */
        } BIT;                  /*              */
    } TCR0;                     /*              */
    union {                     /* TCR1         */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char CMIEB:1;      /*    CMIEB     */
            unsigned char CMIEA:1;      /*    CMIEA     */
            unsigned char OVIE:1;       /*    OVIE      */
            unsigned char CCLR:2;       /*    CCLR      */
            unsigned char CKS:3;        /*    CKS       */
        } BIT;                  /*              */
    } TCR1;                     /*              */
    union {                     /* TCSR0        */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char CMFB:1;       /*    CMFB      */
            unsigned char CMFA:1;       /*    CMFA      */
            unsigned char OVF:1;        /*    OVF       */
            unsigned char ADTE:1;       /*    ADTE      */
            unsigned char OS:4; /*    OS        */
        } BIT;                  /*              */
    } TCSR0;                    /*              */
    union {                     /* TCSR1        */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char CMFB:1;       /*    CMFB      */
            unsigned char CMFA:1;       /*    CMFA      */
            unsigned char OVF:1;        /*    OVF       */
            unsigned char ICE:1;        /*    ICE       */
            unsigned char OS:4; /*    OS        */
        } BIT;                  /*              */
    } TCSR1;                    /*              */
    unsigned short TCORA;       /* TCORA        */
    unsigned short TCORB;       /* TCORB        */
    unsigned short TCNT;        /* TCNT         */
};                              /*              */
struct st_tmr23 {               /* struct TMR23 */
    union {                     /* TCR2         */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char CMIEB:1;      /*    CMIEB     */
            unsigned char CMIEA:1;      /*    CMIEA     */
            unsigned char OVIE:1;       /*    OVIE      */
            unsigned char CCLR:2;       /*    CCLR      */
            unsigned char CKS:3;        /*    CKS       */
        } BIT;                  /*              */
    } TCR2;                     /*              */
    union {                     /* TCR3         */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char CMIEB:1;      /*    CMIEB     */
            unsigned char CMIEA:1;      /*    CMIEA     */
            unsigned char OVIE:1;       /*    OVIE      */
            unsigned char CCLR:2;       /*    CCLR      */
            unsigned char CKS:3;        /*    CKS       */
        } BIT;                  /*              */
    } TCR3;                     /*              */
    union {                     /* TCSR2        */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char CMFB:1;       /*    CMFB      */
            unsigned char CMFA:1;       /*    CMFA      */
            unsigned char OVF:1;        /*    OVF       */
            unsigned char:1;    /*              */
            unsigned char OS:4; /*    OS        */
        } BIT;                  /*              */
    } TCSR2;                    /*              */
    union {                     /* TCSR3        */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char CMFB:1;       /*    CMFB      */
            unsigned char CMFA:1;       /*    CMFA      */
            unsigned char OVF:1;        /*    OVF       */
            unsigned char ICE:1;        /*    ICE       */
            unsigned char OS:4; /*    OS        */
        } BIT;                  /*              */
    } TCSR3;                    /*              */
    unsigned short TCORA;       /* TCORA        */
    unsigned short TCORB;       /* TCORB        */
    unsigned short TCNT;        /* TCNT         */
};                              /*              */
struct st_tmr0 {                /* struct TMR0  */
    union {                     /* TCR          */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char CMIEB:1;      /*    CMIEB     */
            unsigned char CMIEA:1;      /*    CMIEA     */
            unsigned char OVIE:1;       /*    OVIE      */
            unsigned char CCLR:2;       /*    CCLR      */
            unsigned char CKS:3;        /*    CKS       */
        } BIT;                  /*              */
    } TCR;                      /*              */
    char wk1;                   /*              */
    union {                     /* TCSR         */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char CMFB:1;       /*    CMFB      */
            unsigned char CMFA:1;       /*    CMFA      */
            unsigned char OVF:1;        /*    OVF       */
            unsigned char ADTE:1;       /*    ADTE      */
            unsigned char OS:4; /*    OS        */
        } BIT;                  /*              */
    } TCSR;                     /*              */
    char wk2;                   /*              */
    unsigned char TCORA;        /* TCORA        */
    char wk3;                   /*              */
    unsigned char TCORB;        /* TCORB        */
    char wk4;                   /*              */
    unsigned char TCNT;         /* TCNT         */
};                              /*              */
struct st_tmr1 {                /* struct TMR1  */
    union {                     /* TCR          */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char CMIEB:1;      /*    CMIEB     */
            unsigned char CMIEA:1;      /*    CMIEA     */
            unsigned char OVIE:1;       /*    OVIE      */
            unsigned char CCLR:2;       /*    CCLR      */
            unsigned char CKS:3;        /*    CKS       */
        } BIT;                  /*              */
    } TCR;                      /*              */
    char wk1;                   /*              */
    union {                     /* TCSR         */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char CMFB:1;       /*    CMFB      */
            unsigned char CMFA:1;       /*    CMFA      */
            unsigned char OVF:1;        /*    OVF       */
            unsigned char ICE:1;        /*    ICE       */
            unsigned char OS:4; /*    OS        */
        } BIT;                  /*              */
    } TCSR;                     /*              */
    char wk2;                   /*              */
    unsigned char TCORA;        /* TCORA        */
    char wk3;                   /*              */
    unsigned char TCORB;        /* TCORB        */
    char wk4;                   /*              */
    unsigned char TCNT;         /* TCNT         */
};                              /*              */
struct st_tmr2 {                /* struct TMR2  */
    union {                     /* TCR          */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char CMIEB:1;      /*    CMIEB     */
            unsigned char CMIEA:1;      /*    CMIEA     */
            unsigned char OVIE:1;       /*    OVIE      */
            unsigned char CCLR:2;       /*    CCLR      */
            unsigned char CKS:3;        /*    CKS       */
        } BIT;                  /*              */
    } TCR;                      /*              */
    char wk1;                   /*              */
    union {                     /* TCSR         */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char CMFB:1;       /*    CMFB      */
            unsigned char CMFA:1;       /*    CMFA      */
            unsigned char OVF:1;        /*    OVF       */
            unsigned char:1;    /*              */
            unsigned char OS:4; /*    OS        */
        } BIT;                  /*              */
    } TCSR;                     /*              */
    char wk2;                   /*              */
    unsigned char TCORA;        /* TCORA        */
    char wk3;                   /*              */
    unsigned char TCORB;        /* TCORB        */
    char wk4;                   /*              */
    unsigned char TCNT;         /* TCNT         */
};                              /*              */
union un_dastcr {               /* DASTCR       */
    unsigned char BYTE;         /*  Byte Access */
    struct {                    /*  Bit  Access */
        unsigned char:7;        /*              */
        unsigned char DASTE:1;  /*    DASTE     */
    } BIT;                      /*              */
};                              /*              */
struct st_da {                  /* struct D/A   */
    unsigned char DADR0;        /* DADR0        */
    unsigned char DADR1;        /* DADR1        */
    union {                     /* DACR         */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char DAOE1:1;      /*    DAOE1     */
            unsigned char DAOE0:1;      /*    DAOE0     */
            unsigned char DAE:1;        /*    DAE       */
        } BIT;                  /*              */
    } DACR;                     /*              */
};                              /*              */
struct st_tpc {                 /* struct TPC   */
    union {                     /* TPMR         */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char:4;    /*              */
            unsigned char G3NOV:1;      /*    G3NOV     */
            unsigned char G2NOV:1;      /*    G2NOV     */
            unsigned char G1NOV:1;      /*    G1NOV     */
            unsigned char G0NOV:1;      /*    G0NOV     */
        } BIT;                  /*              */
    } TPMR;                     /*              */
    union {                     /* TPCR         */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char G3CMS:2;      /*    G3CMS     */
            unsigned char G2CMS:2;      /*    G2CMS     */
            unsigned char G1CMS:2;      /*    G1CMS     */
            unsigned char G0CMS:2;      /*    G0CMS     */
        } BIT;                  /*              */
    } TPCR;                     /*              */
    union {                     /* NDERB        */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char B15:1;        /*    NDER15    */
            unsigned char B14:1;        /*    NDER14    */
            unsigned char B13:1;        /*    NDER13    */
            unsigned char B12:1;        /*    NDER12    */
            unsigned char B11:1;        /*    NDER11    */
            unsigned char B10:1;        /*    NDER10    */
            unsigned char B9:1; /*    NDER9     */
            unsigned char B8:1; /*    NDER8     */
        } BIT;                  /*              */
    } NDERB;                    /*              */
    union {                     /* NDERA        */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char B7:1; /*    NDER7     */
            unsigned char B6:1; /*    NDER6     */
            unsigned char B5:1; /*    NDER5     */
            unsigned char B4:1; /*    NDER4     */
            unsigned char B3:1; /*    NDER3     */
            unsigned char B2:1; /*    NDER2     */
            unsigned char B1:1; /*    NDER1     */
            unsigned char B0:1; /*    NDER0     */
        } BIT;                  /*              */
    } NDERA;                    /*              */
    union {                     /* NDRB (H'A4)  */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char B15:1;        /*    NDR15     */
            unsigned char B14:1;        /*    NDR14     */
            unsigned char B13:1;        /*    NDR13     */
            unsigned char B12:1;        /*    NDR12     */
            unsigned char B11:1;        /*    NDR11     */
            unsigned char B10:1;        /*    NDR10     */
            unsigned char B9:1; /*    NDR9      */
            unsigned char B8:1; /*    NDR8      */
        } BIT;                  /*              */
    } NDRB1;                    /*              */
    union {                     /* NDRA (H'A5)  */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char B7:1; /*    NDR7      */
            unsigned char B6:1; /*    NDR6      */
            unsigned char B5:1; /*    NDR5      */
            unsigned char B4:1; /*    NDR4      */
            unsigned char B3:1; /*    NDR3      */
            unsigned char B2:1; /*    NDR2      */
            unsigned char B1:1; /*    NDR1      */
            unsigned char B0:1; /*    NDR0      */
        } BIT;                  /*              */
    } NDRA1;                    /*              */
    union {                     /* NDRB (H'A6)  */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char:4;    /*              */
            unsigned char B11:1;        /*    NDR11     */
            unsigned char B10:1;        /*    NDR10     */
            unsigned char B9:1; /*    NDR9      */
            unsigned char B8:1; /*    NDR8      */
        } BIT;                  /*              */
    } NDRB2;                    /*              */
    union {                     /* NDRA (H'A7)  */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char:4;    /*              */
            unsigned char B3:1; /*    NDR3      */
            unsigned char B2:1; /*    NDR2      */
            unsigned char B1:1; /*    NDR1      */
            unsigned char B0:1; /*    NDR0      */
        } BIT;                  /*              */
    } NDRA2;                    /*              */
};                              /*              */
struct st_sci {                 /* struct SCI   */
    union {                     /* SMR          */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char CA:1; /*    C/A       */
            unsigned char CHR:1;        /*    CHR       */
            unsigned char PE:1; /*    PE        */
            unsigned char OE:1; /*    O/E       */
            unsigned char STOP:1;       /*    STOP      */
            unsigned char MP:1; /*    MP        */
            unsigned char CKS:2;        /*    CKS       */
        } BIT;                  /*              */
    } SMR;                      /*              */
    unsigned char BRR;          /* BRR          */
    union {                     /* SCR          */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char TIE:1;        /*    TIE       */
            unsigned char RIE:1;        /*    RIE       */
            unsigned char TE:1; /*    TE        */
            unsigned char RE:1; /*    RE        */
            unsigned char MPIE:1;       /*    MPIE      */
            unsigned char TEIE:1;       /*    TEIE      */
            unsigned char CKE:2;        /*    CKE       */
        } BIT;                  /*              */
    } SCR;                      /*              */
    unsigned char TDR;          /* TDR          */
    union {                     /* SSR          */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char TDRE:1;       /*    TDRE      */
            unsigned char RDRF:1;       /*    RDRF      */
            unsigned char ORER:1;       /*    ORER      */
            unsigned char FER:1;        /*    FER       */
            unsigned char PER:1;        /*    PER       */
            unsigned char TEND:1;       /*    TEND      */
            unsigned char MPB:1;        /*    MPB       */
            unsigned char MPBT:1;       /*    MPBT      */
        } BIT;                  /*              */
    } SSR;                      /*              */
    unsigned char RDR;          /* RDR          */
    union {                     /* SCMR         */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char:4;    /*              */
            unsigned char SDIR:1;       /*    SDIR      */
            unsigned char SINV:1;       /*    SINV      */
            unsigned char:1;    /*              */
            unsigned char SMIF:1;       /*    SMIF      */
        } BIT;                  /*              */
    } SCMR;                     /*              */
};                              /*              */
struct st_smci {                /* struct SMCI  */
    union {                     /* SMR          */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char GM:1; /*    GM        */
            unsigned char CHR:1;        /*    CHR       */
            unsigned char PE:1; /*    PE        */
            unsigned char OE:1; /*    O/E       */
            unsigned char STOP:1;       /*    STOP      */
            unsigned char MP:1; /*    MP        */
            unsigned char CKS:2;        /*    CKS       */
        } BIT;                  /*              */
    } SMR;                      /*              */
    unsigned char BRR;          /* BRR          */
    union {                     /* SCR          */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char TIE:1;        /*    TIE       */
            unsigned char RIE:1;        /*    RIE       */
            unsigned char TE:1; /*    TE        */
            unsigned char RE:1; /*    RE        */
            unsigned char MPIE:1;       /*    MPIE      */
            unsigned char TEIE:1;       /*    TEIE      */
            unsigned char CKE:2;        /*    CKE       */
        } BIT;                  /*              */
    } SCR;                      /*              */
    unsigned char TDR;          /* TDR          */
    union {                     /* SSR          */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char TDRE:1;       /*    TDRE      */
            unsigned char RDRF:1;       /*    RDRF      */
            unsigned char ORER:1;       /*    ORER      */
            unsigned char ERS:1;        /*    ERS       */
            unsigned char PER:1;        /*    PER       */
            unsigned char TEND:1;       /*    TEND      */
            unsigned char MPB:1;        /*    MPB       */
            unsigned char MPBT:1;       /*    MPBT      */
        } BIT;                  /*              */
    } SSR;                      /*              */
    unsigned char RDR;          /* RDR          */
    union {                     /* SCMR         */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char:4;    /*              */
            unsigned char SDIR:1;       /*    SDIR      */
            unsigned char SINV:1;       /*    SINV      */
            unsigned char:1;    /*              */
            unsigned char SMIF:1;       /*    SMIF      */
        } BIT;                  /*              */
    } SCMR;                     /*              */
};                              /*              */
struct st_ad {                  /* struct A/D   */
    unsigned short ADDRA;       /* ADDRA        */
    unsigned short ADDRB;       /* ADDRB        */
    unsigned short ADDRC;       /* ADDRC        */
    unsigned short ADDRD;       /* ADDRD        */
    union {                     /* ADCSR        */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char ADF:1;        /*    ADF       */
            unsigned char ADIE:1;       /*    ADIE      */
            unsigned char ADST:1;       /*    ADST      */
            unsigned char SCAN:1;       /*    SCAN      */
            unsigned char CKS:1;        /*    CKS       */
            unsigned char CH:3; /*    CH        */
        } BIT;                  /*              */
    } ADCSR;                    /*              */
    union {                     /* ADCR         */
        unsigned char BYTE;     /*  Byte Access */
        struct {                /*  Bit  Access */
            unsigned char TRGE:1;       /*    TRGE      */
        } BIT;                  /*              */
    } ADCR;                     /*              */
};                              /*              */
#define P1DDR   (*(volatile unsigned char   *)0xFEE000) /* P1DDR Address */
#define P2DDR   (*(volatile unsigned char   *)0xFEE001) /* P2DDR Address */
#define P3DDR   (*(volatile unsigned char   *)0xFEE002) /* P3DDR Address */
#define P4DDR   (*(volatile unsigned char   *)0xFEE003) /* P4DDR Address */
#define P5DDR   (*(volatile unsigned char   *)0xFEE004) /* P5DDR Address */
#define P6DDR   (*(volatile unsigned char   *)0xFEE005) /* P6DDR Address */
#define P8DDR   (*(volatile unsigned char   *)0xFEE007) /* P8DDR Address */
#define P9DDR   (*(volatile unsigned char   *)0xFEE008) /* P9DDR Address */
#define PADDR   (*(volatile unsigned char   *)0xFEE009) /* PADDR Address */
#define PBDDR   (*(volatile unsigned char   *)0xFEE00A) /* PBDDR Address */
#define P2PCR   (*(volatile union  un_p2pcr *)0xFEE03C) /* P2PCR Address */
#define P4PCR   (*(volatile union  un_p4pcr *)0xFEE03E) /* P4PCR Address */
#define P5PCR   (*(volatile union  un_p5pcr *)0xFEE03F) /* P5PCR Address */
#define P1DR    (*(volatile union  un_p1dr  *)0xFFFFD0) /* P1DR  Address */
#define P2DR    (*(volatile union  un_p2dr  *)0xFFFFD1) /* P2DR  Address */
#define P3DR    (*(volatile union  un_p3dr  *)0xFFFFD2) /* P3DR  Address */
#define P4DR    (*(volatile union  un_p4dr  *)0xFFFFD3) /* P4DR  Address */
#define P5DR    (*(volatile union  un_p5dr  *)0xFFFFD4) /* P5DR  Address */
#define P6DR    (*(volatile union  un_p6dr  *)0xFFFFD5) /* P6DR  Address */
#define P7DR    (*(volatile union  un_p7dr  *)0xFFFFD6) /* P7DR  Address */
#define P8DR    (*(volatile union  un_p8dr  *)0xFFFFD7) /* P8DR  Address */
#define P9DR    (*(volatile union  un_p9dr  *)0xFFFFD8) /* P9DR  Address */
#define PADR    (*(volatile union  un_padr  *)0xFFFFD9) /* PADR  Address */
#define PBDR    (*(volatile union  un_pbdr  *)0xFFFFDA) /* PBDR  Address */
#define MDCR    (*(volatile union  un_mdcr  *)0xFEE011) /* MDCR  Address */
#define SYSCR   (*(volatile union  un_syscr *)0xFEE012) /* SYSCR Address */
#define DIVCR   (*(volatile union  un_divcr *)0xFEE01B) /* DIVCR Address */
#define MSTCR   (*(volatile union  un_mstcr *)0xFEE01C) /* MSTCR Address */
#define BSC     (*(volatile struct st_bsc   *)0xFEE012) /* BSC   Address */
#define INTC    (*(volatile struct st_intc  *)0xFEE014) /* INTC  Address */
#define DMAC0A  (*(volatile struct st_sam   *)0xFFFF20) /* DMAC 0A Addr */
#define DMAC0B  (*(volatile struct st_sam   *)0xFFFF28) /* DMAC 0B Addr */
#define DMAC1A  (*(volatile struct st_sam   *)0xFFFF30) /* DMAC 1A Addr */
#define DMAC1B  (*(volatile struct st_sam   *)0xFFFF38) /* DMAC 1B Addr */
#define DMAC0   (*(volatile struct st_fam   *)0xFFFF20) /* DMAC 0  Addr */
#define DMAC1   (*(volatile struct st_fam   *)0xFFFF30) /* DMAC 1  Addr */
#define FLASH   (*(volatile struct st_flash *)0xFFFF40) /* FLASH Address */
#define ITU     (*(volatile struct st_itu   *)0xFFFF60) /* ITU   Address */
#define ITU0    (*(volatile struct st_itu0  *)0xFFFF68) /* ITU0  Address */
#define ITU1    (*(volatile struct st_itu0  *)0xFFFF70) /* ITU1  Address */
#define ITU2    (*(volatile struct st_itu0  *)0xFFFF78) /* ITU2  Address */
#define WDT     (*(volatile union  un_wdt   *)0xFFFF8C) /* WDT   Address */
#define TMR01   (*(volatile struct st_tmr01 *)0xFFFF80) /* TMR01 Address */
#define TMR23   (*(volatile struct st_tmr23 *)0xFFFF90) /* TMR23 Address */
#define TMR0    (*(volatile struct st_tmr0  *)0xFFFF80) /* TMR0  Address */
#define TMR1    (*(volatile struct st_tmr1  *)0xFFFF81) /* TMR1  Address */
#define TMR2    (*(volatile struct st_tmr2  *)0xFFFF90) /* TMR2  Address */
#define TMR3    (*(volatile struct st_tmr1  *)0xFFFF91) /* TMR3  Address */
#define DASTCR  (*(volatile union  un_dastcr*)0xFEE01A) /* DASTCRAddress */
#define DA      (*(volatile struct st_da    *)0xFFFF9C) /* D/A   Address */
#define TPC     (*(volatile struct st_tpc   *)0xFFFFA0) /* TPC   Address */
#define SCI0    (*(volatile struct st_sci   *)0xFFFFB0) /* SCI0  Address */
#define SCI1    (*(volatile struct st_sci   *)0xFFFFB8) /* SCI1  Address */
#define SCI2    (*(volatile struct st_sci   *)0xFFFFC0) /* SCI2  Address */
#define SMCI0   (*(volatile struct st_smci  *)0xFFFFB0) /* SMCI0 Address */
#define SMCI1   (*(volatile struct st_smci  *)0xFFFFB8) /* SMCI1 Address */
#define SMCI2   (*(volatile struct st_smci  *)0xFFFFC0) /* SMCI2 Address */
#define AD      (*(volatile struct st_ad    *)0xFFFFE0) /* A/D   Address */

#endif                          /* #ifndef __INCLUDE_H83068F_H__ */
