/*
 * Copyright (C) 2004 by Ole Reinhardt <ole.reinhardt@kernelconcepts.de>,
 *                       Kernelconcepts http://www.kernelconcepts.de
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

/*!
 * \file include/dev/sja1000.h
 * \brief Header for SJA1000 CAN-Bus controller driver
 *
 *
 * The SJA1000 controller is connected to the memory bus. It's base
 * address and interrupt is set by NutRegisterDevice.
 *
 * Have a look to our m-can board if you have questions.
 */

/*!
 * \addtogroup xgCanSJA1000
 */
/*@{*/

#ifndef _SJA1000_H_
#define _SJA1000_H_

#include <stdint.h>

//Register and bit definitions for the SJA1000

// address and bit definitions for the Mode & Control Register
#define SJA1000_MODECTRL (*(volatile uint8_t*) (sja_base+0))
#define RM_RR_Bit   0x01        // reset mode (request) bit

#define LOM_Bit     0x02        // listen only mode bit
#define STM_Bit     0x04        // self test mode bit
#define AFM_Bit     0x08        // acceptance filter mode bit
#define SM_Bit      0x10        // enter sleep mode bit

// address and bit definitions for the Interrupt Enable & Control Register
#define SJA1000_IEN (*(volatile uint8_t*) (sja_base+4))      // PeliCAN mode
#define RIE_Bit     0x01        // receive interrupt enable bit
#define TIE_Bit     0x02        // transmit interrupt enable bit
#define EIE_Bit     0x04        // error warning interrupt enable bit
#define DOIE_Bit    0x08        // data overrun interrupt enable bit
#define WUIE_Bit    0x10        // wake-up interrupt enable bit
#define EPIE_Bit    0x20        // error passive interrupt enable bit
#define ALIE_Bit    0x40        // arbitration lost interr. enable bit
#define BEIE_Bit    0x80        // bus error interrupt enable bit

// address and bit definitions for the Command Register
#define SJA1000_CMD (*(volatile uint8_t*) (sja_base+1))
#define TR_Bit      0x01        // transmission request bit
#define AT_Bit      0x02        // abort transmission bit
#define RRB_Bit     0x04        // release receive buffer bit
#define CDO_Bit     0x08        // clear data overrun bit
#define SRR_Bit     0x10        // self reception request bit

// address and bit definitions for the Status Register
#define SJA1000_STATUS (*(volatile uint8_t*) (sja_base+2))
#define RBS_Bit     0x01        // receive buffer status bit
#define DOS_Bit     0x02        // data overrun status bit
#define TBS_Bit     0x04        // transmit buffer status bit
#define TCS_Bit     0x08        // transmission complete status bit
#define RS_Bit      0x10        // receive status bit
#define TS_Bit      0x20        // transmit status bit
#define ES_Bit      0x40        // error status bit
#define BS_Bit      0x80        // bus status bit

// address and bit definitions for the Interrupt Register
#define SJA1000_INT (*(volatile uint8_t*) (sja_base+3))
#define RI_Bit      0x01        // receive interrupt bit
#define TI_Bit      0x02        // transmit interrupt bit
#define EI_Bit      0x04        // error warning interrupt bit
#define DOI_Bit     0x08        // data overrun interrupt bit
#define WUI_Bit     0x10        // wake-up interrupt bit

#define EPI_Bit     0x20        // error passive interrupt bit
#define ALI_Bit     0x40        // arbitration lost interrupt bit
#define BEI_Bit     0x80        // bus error interrupt bit

// address and bit definitions for the Bus Timing Registers
#define SJA1000_BT0 (*(volatile uint8_t*) (sja_base+6))
#define SJA1000_BT1 (*(volatile uint8_t*) (sja_base+7))
#define SAM_Bit     0x80        // sample mode bit
                                //   1 == the bus is sampled 3 times
                                //   0 == the bus is sampled once */

// address and bit definitions for the Output Control Register
#define SJA1000_OUTCTRL (*(volatile uint8_t*) (sja_base+8))
// OCMODE1, OCMODE0
#define BiPhaseMode 0x00        // bi-phase output mode
#define NormalMode  0x02        // normal output mode
#define ClkOutMode  0x03        // clock output mode
// output pin configuration for TX1
#define OCPOL1_Bit  0x20        // output polarity control bit
#define Tx1Float    0x00        // configured as float
#define Tx1PullDn   0x40        // configured as pull-down
#define Tx1PullUp   0x80        // configured as pull-up
#define Tx1PshPull  0xC0        // configured as push/pull
// output configuration for TX0
#define OCPOLO_Bit  0x04        // output polarity control bit
#define Tx0Float    0x00        // configured as float
#define Tx0PullDn   0x08        // configured as pull-down
#define Tx0PullUp   0x10        // configured as pull-up
#define Tx0PshPull  0x18        // configured as push/pull

// address definitions of Acceptance Code & Mask Registers
#define SJA1000_AC0 (*(volatile uint8_t*) (sja_base+16))
#define SJA1000_AC1 (*(volatile uint8_t*) (sja_base+17))
#define SJA1000_AC2 (*(volatile uint8_t*) (sja_base+18))
#define SJA1000_AC3 (*(volatile uint8_t*) (sja_base+19))
#define SJA1000_AM0 (*(volatile uint8_t*) (sja_base+20))
#define SJA1000_AM1 (*(volatile uint8_t*) (sja_base+21))
#define SJA1000_AM2 (*(volatile uint8_t*) (sja_base+22))
#define SJA1000_AM3 (*(volatile uint8_t*) (sja_base+23))

// address definitions of the Rx-Buffer
#define SJA1000_RxFrameInfo (*(volatile uint8_t*) (sja_base+16))
#define SJA1000_Rx1   (*(volatile uint8_t*) (sja_base+17))
#define SJA1000_Rx2   (*(volatile uint8_t*) (sja_base+18))
#define SJA1000_Rx3   (*(volatile uint8_t*) (sja_base+19))
#define SJA1000_Rx4   (*(volatile uint8_t*) (sja_base+20))
#define SJA1000_Rx5   (*(volatile uint8_t*) (sja_base+21))
#define SJA1000_Rx6   (*(volatile uint8_t*) (sja_base+22))
#define SJA1000_Rx7   (*(volatile uint8_t*) (sja_base+23))
#define SJA1000_Rx8   (*(volatile uint8_t*) (sja_base+24))
#define SJA1000_Rx9   (*(volatile uint8_t*) (sja_base+25))
#define SJA1000_Rx10  (*(volatile uint8_t*) (sja_base+26))
#define SJA1000_Rx11  (*(volatile uint8_t*) (sja_base+27))
#define SJA1000_Rx12  (*(volatile uint8_t*) (sja_base+28))

// address definitions of the Tx-Buffer
/* write only addresses */
#define TestReg (*(volatile uint8_t*) (sja_base+9))

#define SJA1000_TxFrameInfo (*(volatile uint8_t*) (sja_base+16))
#define SJA1000_Tx1   (*(volatile uint8_t*) (sja_base+17))
#define SJA1000_Tx2   (*(volatile uint8_t*) (sja_base+18))
#define SJA1000_Tx3   (*(volatile uint8_t*) (sja_base+19))
#define SJA1000_Tx4   (*(volatile uint8_t*) (sja_base+20))
#define SJA1000_Tx5   (*(volatile uint8_t*) (sja_base+21))
#define SJA1000_Tx6   (*(volatile uint8_t*) (sja_base+22))
#define SJA1000_Tx7   (*(volatile uint8_t*) (sja_base+23))
#define SJA1000_Tx8   (*(volatile uint8_t*) (sja_base+24))
#define SJA1000_Tx9   (*(volatile uint8_t*) (sja_base+25))
#define SJA1000_Tx10  (*(volatile uint8_t*) (sja_base+26))
#define SJA1000_Tx11  (*(volatile uint8_t*) (sja_base+27))
#define SJA1000_Tx12  (*(volatile uint8_t*) (sja_base+28))

/* read only addresses */
#define SJA1000_TxFrameInfoRd (*(volatile uint8_t*) (sja_base+96))
#define SJA1000_TxRd1  (*(volatile uint8_t*) (sja_base+97))
#define SJA1000_TxRd2  (*(volatile uint8_t*) (sja_base+98))
#define SJA1000_TxRd3  (*(volatile uint8_t*) (sja_base+99))
#define SJA1000_TxRd4  (*(volatile uint8_t*) (sja_base+100))
#define SJA1000_TxRd5  (*(volatile uint8_t*) (sja_base+101))
#define SJA1000_TxRd6  (*(volatile uint8_t*) (sja_base+102))
#define SJA1000_TxRd7  (*(volatile uint8_t*) (sja_base+103))
#define SJA1000_TxRd8  (*(volatile uint8_t*) (sja_base+104))
#define SJA1000_TxRd9  (*(volatile uint8_t*) (sja_base+105))
#define SJA1000_TxRd10 (*(volatile uint8_t*) (sja_base+106))
#define SJA1000_TxRd11 (*(volatile uint8_t*) (sja_base+107))
#define SJA1000_TxRd12 (*(volatile uint8_t*) (sja_base+108))

// address definitions of Other Registers
#define SJA1000_ArbLostCap    (*(volatile uint8_t*) (sja_base+11))
#define SJA1000_ErrCodeCap    (*(volatile uint8_t*) (sja_base+12))
#define SJA1000_ErrWarnLimit  (*(volatile uint8_t*) (sja_base+13))
#define SJA1000_RxErrCount    (*(volatile uint8_t*) (sja_base+14))
#define SJA1000_TxErrCount    (*(volatile uint8_t*) (sja_base+15))
#define SJA1000_RxMsgCount    (*(volatile uint8_t*) (sja_base+29))
#define SJA1000_RxBufStartAdr (*(volatile uint8_t*) (sja_base+30))

// address and bit definitions for the Clock Divider Register
#define SJA1000_CLK_DIV (*(volatile uint8_t*) (sja_base+31))
#define DivBy1       0x07       // CLKOUT = oscillator frequency
#define DivBy2       0x00       // CLKOUT = 1/2 oscillator frequency
#define ClkOff_Bit   0x08       // clock off bit, control of the CLK OUT pin
#define RXINTEN_Bit  0x20       // pin TX1 used for receive interrupt
#define CBP_Bit      0x40       // CAN comparator bypass control bit
#define CANMode_Bit  0x80       // CAN mode definition bit
#define ClkOutMode   0x03       // clock output mode

// output pin configuration for TX1
#define OCPOL1_Bit   0x20       // output polarity control bit
#define Tx1Float     0x00       // configured as float
#define Tx1PullDn    0x40       // configured as pull-down
#define Tx1PullUp    0x80       // configured as pull-up
#define Tx1PshPull   0xC0       // configured as push/pull

// output pin configuration for TX0
#define OCPOL0_Bit   0x04       // output polarity control bit
#define Tx0Float     0x00       // configured as float
#define Tx0PullDn    0x08       // configured as pull-down

// Some sample konstants

/* bus timing values for
     - bit-rate : 1 MBit/s
     - oscillator frequency : 16 MHz, 0,1%
     - maximum tolerated propagation delay : 623 ns
     - minimum requested propagation delay : 23 ns
*/

#define Presc_MB_16  0x00       // baud rate prescaler : 1
#define SJW_MB_16    0x00       // SJW : 1
#define TSEG1_MB_16  0x04       // TSEG1 : 5
#define TSEG2_MB_16  0x10       // TSEG2 : 2


// Error codes

#define errCAN_INVALID_BAUD   7
#define CAN_OK                0
#define CAN_ERR               1
#define CAN_INIT_SPEED_ERR    1
#define CAN_NOT_INITIATED     1
#define CAN_NOT_STARTED       1
#define CAN_TXBUF_FULL        2
#define CAN_RXBUF_EMPTY       2

//  Flag bits

#define CAN_LOST_FRAME        0x01
#define CAN_BUS_OFF           0x02

//  Frame Flag bits

#define CAN_29                0x80
#define CAN_RTR               0x40

#define ClrIntEnSJA 	      0x00

extern NUTDEVICE devSJA1000;

#endif
/*@}*/
