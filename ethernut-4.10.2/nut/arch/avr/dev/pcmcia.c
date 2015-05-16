/****************************************************************************
*  This file is part of the WLAN-Ethernut driver.
*
*  Copyright (c) 2004 by Michael Fischer. All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*  
*  1. Redistributions of source code must retain the above copyright 
*     notice, this list of conditions and the following disclaimer.
*  2. Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in the 
*     documentation and/or other materials provided with the distribution.
*  3. Neither the name of the author nor the names of its contributors may 
*     be used to endorse or promote products derived from this software 
*     without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL 
*  THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS 
*  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
*  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
*  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF 
*  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
*  SUCH DAMAGE.
*
****************************************************************************
*  History:
*
*  07.02.04  mifi   First Version for the XC9572-PC84 ONLY!
*                   The BASE_ADDRESS is 0x8800!!
****************************************************************************/
#define __PCMCIA_C__

/*
 * The PCMCIA interface works with a Xilinx XC9572-PC84 CPLD.
 * All the lines like CE1, CE2, WE, OE and so on must be control
 * separate by the latch.
 *
*/

#include <compiler.h>

#include <dev/wlantypes.h>
#include <dev/pcmcia.h>

/*==========================================================*/
/*  DEFINE: All Structures and Common Constants             */
/*==========================================================*/
/*
 * Address offset, register index of the CPLD
*/
#define BASE_ADDRESS  0x8800
#define DATA_LOW      0
#define DATA_HIGH     1
#define ADDRESS_LOW   2
#define ADDRESS_HIGH  3
#define CTRL          4

/*
 * CTRL bit definition
*/
#define CTRL_CE1      0x01
#define CTRL_CE2      0x02
#define CTRL_OE       0x04
#define CTRL_WE       0x08
#define CTRL_IORD     0x10
#define CTRL_IOWR     0x20
#define CTRL_REGS     0x40
#define CTRL_DATA_OUT 0x80

#define DATA_LOW_REG      *((volatile BYTE *)BASE_ADDRESS+DATA_LOW)
#define DATA_HIGH_REG     *((volatile BYTE *)BASE_ADDRESS+DATA_HIGH)
#define ADDRESS_LOW_REG   *((volatile BYTE *)BASE_ADDRESS+ADDRESS_LOW)
#define ADDRESS_HIGH_REG  *((volatile BYTE *)BASE_ADDRESS+ADDRESS_HIGH)
#define CTRL_REG          *((volatile BYTE *)BASE_ADDRESS+CTRL)

/*
 * Some delay values
*/
#define DELAY_CE_WRITE()
#define DELAY_WRITE()
#define DELAY_WRITE_CE()

#define DELAY_CE_READ()
#define DELAY_READ()    _NOP();_NOP(); _NOP();_NOP()
#define DELAY_READ_CE()

typedef enum {
    TYPE_IO = 0,
    TYPE_MEM
} MEMORY_TYPE;

/*==========================================================*/
/*  DEFINE: Prototypes                                      */
/*==========================================================*/

/*==========================================================*/
/*  DEFINE: Definition of all local Data                    */
/*==========================================================*/
//static volatile BYTE *pCPLD = (volatile BYTE *) BASE_ADDRESS;
//static BYTE gbCTRLValue = 0x7C;

/*==========================================================*/
/*  DEFINE: Definition of all local Procedures              */
/*==========================================================*/
/************************************************************/
/*  WriteXXX                                                */
/************************************************************/
static void WriteXXX(WORD wAddress, WORD wData, MEMORY_TYPE eIO)
{
    cli();

    /*
     * set ADDRESS
     */
    ADDRESS_LOW_REG = LOBYTE(wAddress);
    ADDRESS_HIGH_REG = HIBYTE(wAddress);

    /*
     * set DATA
     */
    DATA_LOW_REG = LOBYTE(wData);
    DATA_HIGH_REG = HIBYTE(wData);

    /*
     * set CE1, CE2
     */
    CTRL_REG = (CTRL_CE1 | CTRL_CE2 | CTRL_OE | CTRL_WE | CTRL_IORD | CTRL_IOWR | CTRL_DATA_OUT);

    DELAY_CE_WRITE();

    /*
     * set WR/IOWR
     */
    if (eIO == TYPE_MEM) {
        CTRL_REG = (CTRL_CE1 | CTRL_CE2 | CTRL_OE | CTRL_IORD | CTRL_IOWR | CTRL_DATA_OUT);
    } else {
        CTRL_REG = (CTRL_CE1 | CTRL_CE2 | CTRL_OE | CTRL_WE | CTRL_IORD | CTRL_DATA_OUT);
    }

    DELAY_WRITE();

    /*
     * check the WAIT signal
     */

    /*
     * remove WR/IOWR
     */
    CTRL_REG = (CTRL_CE1 | CTRL_CE2 | CTRL_OE | CTRL_WE | CTRL_IORD | CTRL_IOWR | CTRL_DATA_OUT);

    DELAY_WRITE_CE();

    /*
     * remove CE1, CE2
     */
    CTRL_REG = (CTRL_OE | CTRL_WE | CTRL_IORD | CTRL_IOWR);

    sei();
}

/************************************************************/
/*  ReadXXX                                                 */
/************************************************************/
static WORD ReadXXX(WORD wAddress, MEMORY_TYPE eIO)
{
    BYTE bLow;
    BYTE bHigh;
    WORD wData;

    cli();

    /*
     * set ADDRESS
     */
    ADDRESS_LOW_REG = LOBYTE(wAddress);
    ADDRESS_HIGH_REG = HIBYTE(wAddress);

    /*
     * set CE1, CE2
     */
    CTRL_REG = (CTRL_CE1 | CTRL_CE2 | CTRL_OE | CTRL_WE | CTRL_IORD | CTRL_IOWR);

    DELAY_CE_READ();

    /*
     * set OE/IORD
     */
    if (eIO == TYPE_MEM) {
        CTRL_REG = (CTRL_CE1 | CTRL_CE2 | CTRL_WE | CTRL_IORD | CTRL_IOWR);
    } else {
        CTRL_REG = (CTRL_CE1 | CTRL_CE2 | CTRL_OE | CTRL_WE | CTRL_IOWR);
    }

    DELAY_READ();

    /*
     * check the WAIT signal
     */
    bLow = DATA_LOW_REG;
    bHigh = DATA_HIGH_REG;

    /*
     * remove OE/IORD
     */
    CTRL_REG = (CTRL_CE1 | CTRL_CE2 | CTRL_OE | CTRL_WE | CTRL_IORD | CTRL_IOWR | CTRL_DATA_OUT);

    DELAY_READ_CE();

    /*
     * remove CE1, CE2
     */
    CTRL_REG = (CTRL_OE | CTRL_WE | CTRL_IORD | CTRL_IOWR);

    wData = (WORD) bHigh;
    wData = (wData << 8) | (WORD) bLow;

    sei();

    return (wData);
}

/*==========================================================*/
/*  DEFINE: All code exported                               */
/*==========================================================*/
/************************************************************/
/*  pcmcia_WriteMem                                         */
/************************************************************/
void pcmcia_WriteMem(WORD wAddress, WORD wData)
{
    WriteXXX(wAddress, wData, TYPE_MEM);
}

/************************************************************/
/*  pcmcia_WriteReg                                         */
/************************************************************/
void pcmcia_WriteReg(WORD wAddress, WORD wData)
{
    WriteXXX(wAddress, wData, TYPE_IO);
}

/************************************************************/
/*  pcmcia_ReadMem                                          */
/************************************************************/
WORD pcmcia_ReadMem(WORD wAddress)
{
    return (ReadXXX(wAddress, TYPE_MEM));
}

/************************************************************/
/*  pcmcia_ReadReg                                          */
/************************************************************/
WORD pcmcia_ReadReg(WORD wAddress)
{
    return (ReadXXX(wAddress, TYPE_IO));
}
