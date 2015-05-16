#ifndef _CFG_MEDIANUT_H_
#define _CFG_MEDIANUT_H_

/*
 * Copyright (C) 2001-2003 by egnite Software GmbH. All rights reserved.
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
 * Revision 1.2  2003/07/13 19:42:28  haraldkipp
 * Infrared remote control added.
 *
 * Revision 1.1.1.1  2003/05/09 14:41:04  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.4  2003/05/06 18:39:54  harald
 * Cleanup
 *
 * Revision 1.3  2003/04/07 20:34:06  harald
 * Oops. Medianut's LCD ports had been accidently removed.
 *
 * Revision 1.2  2003/04/07 20:10:37  harald
 * Redesigned by Pavel Chromy
 *
 */

/*!
 * \file cfg/medianut.h
 * \brief Medianut hardware specification.
 */

/*!
 * \addtogroup xgMedianutCfg
 */
/*@{*/

#define VS_SCK_PORT     PORTB	/*!< Port register of \ref VS_SCK_BIT. */
#define VS_SCK_DDR      DDRB	/*!< Data direction register of \ref VS_SCK_BIT. */
#define VS_SCK_BIT      0	/*!< \brief VS1001 serial control interface clock input bit. 
				 * The first rising clock edge after XCS has gone 
				 * low marks the first bit to be written to the decoder.
				 */

#define VS_SS_PORT      PORTB	/*!< Port output register of \ref VS_SS_BIT. */
#define VS_SS_DDR       DDRB	/*!< Data direction register of \ref VS_SS_BIT. */
#define VS_SS_BIT       1	/*!< \brief VS1001 serial data interface clock input bit. */

#define VS_SI_PORT      PORTB	/*!< Port output register of \ref VS_SI_BIT. */
#define VS_SI_DDR       DDRB	/*!< Data direction register of \ref VS_SI_BIT. */
#define VS_SI_BIT       2	/*!< \brief VS1001 serial control interface data input. 
				 * The decoder samples this input on the 
				 * rising edge of SCK if XCS is low.
				 */

#define VS_SO_PIN       PINB	/*!< Port input register of \ref VS_SO_BIT. */
#define VS_SO_DDR       DDRB	/*!< Data direction register of \ref VS_SO_BIT. */
#define VS_SO_BIT       3	/*!< \brief VS1001 serial control interface data output. 
				 * If data is transfered from the decoder, bits
				 * are shifted out on the falling SCK edge. 
				 * If data is transfered to the decoder,
				 * SO is at a high impedance state.
				 */

#define VS_XCS_PORT     PORTB	/*!< Port output register of \ref VS_XCS_BIT. */
#define VS_XCS_DDR      DDRB	/*!< Data direction register of \ref VS_XCS_BIT. */
#define VS_XCS_BIT      4	/*!< \brief VS1001 active low chip select input. 
				 * A high level forces the serial interface 
				 * into standby mode, ending the current 
				 * operation. A high level also forces serial 
				 * output (SO) to high impedance state.
				 */

#define VS_BSYNC_PORT   PORTB	/*!< Port output register of \ref VS_BSYNC_BIT. */
#define VS_BSYNC_DDR    DDRB	/*!< Data direction register of \ref VS_BSYNC_BIT. */
#define VS_BSYNC_BIT    5	/*!< \brief VS1001 serial data interface bit sync. 
				 * The first DCLK sampling edge, during 
				 * which BSYNC is high, marks the first 
				 * bit of a data byte.
				 */

#define VS_RESET_PORT   PORTB	/*!< Port output register of \ref VS_RESET_BIT. */
#define VS_RESET_DDR    DDRB	/*!< Data direction register of \ref VS_RESET_BIT. */
#define VS_RESET_BIT    7	/*!< \brief VS1001 hardware reset input. */

#define VS_DREQ_PORT    PORTE	/*!< Port output register of \ref VS_DREQ_BIT. */
#define VS_DREQ_PIN     PINE	/*!< Port input register of \ref VS_DREQ_BIT. */
#define VS_DREQ_DDR     DDRE	/*!< Data direction register of \ref VS_DREQ_BIT. */
#define VS_DREQ_BIT     6	/*!< \brief VS1001 data request output. */



#define LCD_DATA_PORT   PORTD   /*!< Port output register of \ref LCD_DATA_BITS. */
#define LCD_DATA_DDR    DDRD    /*!< Data direction register of \ref LCD_DATA_BITS. */
#define LCD_DATA_BITS   0xF0    /*!< \brief LCD data lines, either upper or lower 4 bits. */

#define LCD_ENABLE_PORT PORTE   /*!< Port output register of \ref LCD_ENABLE_BIT. */
#define LCD_ENABLE_DDR  DDRE    /*!< Data direction register of \ref LCD_ENABLE_BIT. */
#define LCD_ENABLE_BIT  3       /*!< \brief LCD enable output. */

#define LCD_REGSEL_PORT PORTE   /*!< Port output register of \ref LCD_REGSEL_BIT. */
#define LCD_REGSEL_DDR  DDRE    /*!< Data direction register of \ref LCD_REGSEL_BIT. */
#define LCD_REGSEL_BIT  2       /*!< \brief LCD register select output. */

#define LCD_LIGHT_PORT  PORTB   /*!< Port output register of \ref LCD_LIGHT_BIT. */
#define LCD_LIGHT_DDR   DDRB    /*!< Data direction register of \ref LCD_LIGHT_BIT. */
#define LCD_LIGHT_BIT   6       /*!< \brief LCD output to switch backlight. */

#define IR_SIGNAL_PORT  PORTE   /*!< Port output register of \ref IR_SIGNAL_BIT. */
#define IR_SIGNAL_PIN   PINE    /*!< Port input register of \ref IR_SIGNAL_BIT. */
#define IR_SIGNAL_DDR   DDRE    /*!< Data direction register of \ref IR_SIGNAL_BIT. */
#define IR_SIGNAL_BIT   4       /*!< \brief Infrared decoder signal bit. */

/*@}*/


#endif

