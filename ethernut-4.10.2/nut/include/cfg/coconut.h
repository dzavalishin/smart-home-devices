#ifndef _CFG_COCONUT_H_
#define _CFG_COCONUT_H_

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
 * Revision 1.4  2005/01/22 19:26:33  haraldkipp
 * Marked deprecated.
 *
 * Revision 1.3  2003/08/05 20:17:46  haraldkipp
 * Typing errors corrected
 *
 * Revision 1.2  2003/05/15 15:47:30  haraldkipp
 * Conflict with NIC interrupt and LEDL removed.
 *
 * Revision 1.1.1.1  2003/05/09 14:41:04  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.2  2003/05/06 18:39:12  harald
 * Cleanup
 *
 */

/*!
 * \file cfg/coconut.h
 * \brief Coconut hardware specification.
 *
 * Including this file is deprecated. Use cfg/arch/avr.h.
 */
/*
 * \addtogroup xgCoconutCfg
 */
/*@{*/

#warning Using this file is deprecated, just include <cfg/arch/avr.h> instead

/*
 * Coconut LEDs.
 */
#define COCO_LED0_PORT  PORTB
#define COCO_LED0_DDR   DDRB
#define COCO_LED0_BIT   4

#define COCO_LED1_PORT  PORTB
#define COCO_LED1_DDR   DDRB
#define COCO_LED1_BIT   6

/*
 * Coconut handshake lines.
 */
#define COCO_HSO0_PORT  PORTE
#define COCO_HSO0_DDR   DDRE
#define COCO_HSO0_BIT   2

#define COCO_HSI0_PORT  PORTE
#define COCO_HSI0_DDR   DDRE
#define COCO_HSI0_BIT   6

#define COCO_HSO1_PORT  PORTD
#define COCO_HSO1_DDR   DDRD
#define COCO_HSO1_BIT   4

#define COCO_HSI1_PORT  PORTE
#define COCO_HSI1_DDR   DDRE
#define COCO_HSI1_BIT   7

#define COCO_HS0_SIGNAL sig_INTERRUPT6
#define COCO_HS1_SIGNAL sig_INTERRUPT7

/*
 * Ethernut reset lines.
 */
#define ENUT_RST0_PORT  PORTB
#define ENUT_RST0_DDR   DDRB
#define ENUT_RST0_BIT   0

#define ENUT_RST1_PORT  PORTB
#define ENUT_RST1_DDR   DDRB
#define ENUT_RST1_BIT   1

#define ENUT_RST2_PORT  PORTB
#define ENUT_RST2_DDR   DDRB
#define ENUT_RST2_BIT   2

/*
 * Ethernut LEDs.
 * LEDA (activity) with 1k resistor to ground.
 * LEDL (link) moved from PE5 to PE4. PE5 is NIC IRQ.
 */
#define ENUT_LED0_PORT  PORTB
#define ENUT_LED0_DDR   DDRB
#define ENUT_LED0_BIT   4

#define ENUT_LED1_PORT  PORTB
#define ENUT_LED1_DDR   DDRB
#define ENUT_LED1_BIT   6

#define ENUT_LEDL_PORT  PORTE
#define ENUT_LEDL_DDR   DDRE
#define ENUT_LEDL_BIT   4

#define ENUT_LEDA_PORT  PORTE
#define ENUT_LEDA_DDR   DDRE
#define ENUT_LEDA_BIT   6

/*
 * Ethernut SPI I/O.
 */
#define ENUT_SOUT_PORT  PORTD
#define ENUT_SOUT_DDR   DDRD
#define ENUT_SOUT_BIT   6

#define ENUT_SIN_PORT   PORTD
#define ENUT_SIN_DDR    DDRD
#define ENUT_SIN_BIT    7

#define ENUT_SCLK_PORT  PORTD
#define ENUT_SCLK_DDR   DDRD
#define ENUT_SCLK_BIT   5

#define ENUT_LDI_PORT   PORTB
#define ENUT_LDI_DDR    DDRB
#define ENUT_LDI_BIT    7

#define ENUT_LDO_PORT   PORTB
#define ENUT_LDO_DDR    DDRB
#define ENUT_LDO_BIT    5

/*
 * Ethernut handshake lines.
 * No interrupt on HSI0.
 */
#define ENUT_HSO0_PORT  PORTE
#define ENUT_HSO0_DDR   DDRE
#define ENUT_HSO0_BIT   2

#define ENUT_HSI0_PORT  PORTE
#define ENUT_HSI0_DDR   DDRE
#define ENUT_HSI0_BIT   3

#define ENUT_HSO1_PORT  PORTD
#define ENUT_HSO1_DDR   DDRD
#define ENUT_HSO1_BIT   4

#define ENUT_HSI1_PORT  PORTE
#define ENUT_HSI1_DDR   DDRE
#define ENUT_HSI1_BIT   7

#define ENUT_HS1_SIGNAL sig_INTERRUPT7

/*@}*/

#endif
