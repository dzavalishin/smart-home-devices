#ifndef _CFG_MCAN_H_
#define _CFG_MCAN_H_

/*
 * Copyright (C) 2004 by Ole Reinhardt<ole.reinhardt@kernelconcepts.de>
 * Kernel concepts (http://www.kernelconcepts.de) All rights reserved.
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
 * Revision 1.4  2006/10/08 16:48:09  haraldkipp
 * Documentation fixed
 *
 * Revision 1.3  2005/08/02 17:46:48  haraldkipp
 * Major API documentation update.
 *
 * Revision 1.2  2005/05/27 14:08:01  olereinhardt
 * Changed specs to new m-can board design (LCD controller)
 *
 * Revision 1.1  2004/06/07 15:14:25  olereinhardt
 * Initial checkin
 *
 */

/*!
 * \addtogroup xgMCanCfg
 */
/*@{*/

/*!
 * \file cfg/m-can.h
 * \brief MCAN Board Configuration
 */

/*
 * \brief Defines for CAN controller
 */

#define SJA_SIGNAL     sig_INTERRUPT7
#define SJA_EICR       EICRB
#define SJA_SIGNAL_BIT 7

/*
 * \brief LCD Display definitions
 */
 
//#define KS0073_CONTROLLER 

#define LCD_4x20


#undef  LCD_DATA_PORT
#undef  LCD_DATA_DDR
#undef  LCD_DATA_BITS
#undef  LCD_DATA_PIN

#define LCD_DATA_PORT   PORTB   /*!< Port output register of \ref LCD_DATA_BITS. */
#define LCD_DATA_DDR    DDRB    /*!< Data direction register of \ref LCD_DATA_BITS. */
#define LCD_DATA_BITS   0xFF    /*!< \brief LCD data lines, either upper or lower 4 bits. */
#define LCD_DATA_PIN    PINB    /*!< Port input register of \ref LCD_DATA_BITS. */

#undef  LCD_ENABLE_PORT
#undef  LCD_ENABLE_DDR
#undef  LCD_ENABLE_BIT

#define LCD_ENABLE_PORT PORTE   /*!< Port output register of \ref LCD_ENABLE_BIT. */
#define LCD_ENABLE_DDR  DDRE    /*!< Data direction register of \ref LCD_ENABLE_BIT. */
#define LCD_ENABLE_BIT  2       /*!< \brief LCD enable output. */

#undef  LCD_RW_PORT
#undef  LCD_RW_DDR
#undef  LCD_RW_BIT

#define LCD_RW_PORT     PORTE   /*!< Port output register of \ref LCD_RW_BIT. */
#define LCD_RW_DDR      DDRE    /*!< Data direction register of \ref LCD_RW_BIT. */
#define LCD_RW_BIT      1       /*!< \brief LCD read/write output. */

#undef  LCD_REGSEL_PORT
#undef  LCD_REGSEL_DDR
#undef  LCD_REGSEL_BIT

#define LCD_REGSEL_PORT PORTE   /*!< Port output register of \ref LCD_REGSEL_BIT. */
#define LCD_REGSEL_DDR  DDRE    /*!< Data direction register of \ref LCD_REGSEL_BIT. */
#define LCD_REGSEL_BIT  0       /*!< \brief LCD register select output. */

/*@}*/

#endif
