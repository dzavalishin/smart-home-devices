/*
 * Copyright (C) 2007 by egnite Software GmbH. All rights reserved.
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
 * Revision 1.6  2009/01/17 11:26:37  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.5  2008/08/11 06:59:13  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.4  2008/06/06 10:28:21  haraldkipp
 * ST7036 LCD controller settings moved from source to configuration files.
 *
 * Revision 1.3  2008/02/15 16:59:02  haraldkipp
 * Spport for AT91SAM7SE512 added.
 *
 * Revision 1.2  2007/10/04 20:01:50  olereinhardt
 * Support for SAM7S256 added
 *
 * Revision 1.1  2007/02/15 16:09:07  haraldkipp
 * Tested with EA DOG-M LCDs.
 *
 */

#include <cfg/arch.h>
#include <cfg/arch/gpio.h>
#include <cfg/lcd.h>

#include <stdlib.h>
#include <string.h>

#include <sys/nutconfig.h>
#include <dev/st7036.h>
#include <dev/term.h>
#include <sys/timer.h>

#ifndef LCD_ROWS
#define LCD_ROWS    2
#endif

#ifndef LCD_COLS
#define LCD_COLS    16
#endif

#ifndef LCD_E2E_DLY
#define LCD_E2E_DLY 80
#endif

#ifndef LCD_LONG_DELAY
#define LCD_LONG_DELAY  1000
#endif

#if !defined(LCD_IF_8BIT) && !defined(LCD_IF_4BIT)
#define LCD_IF_4BIT
#endif

/*
 * Fallback definition for PIO_IDs and bits
 */
#ifndef LCD_EN_PIO_ID
#if defined(MCU_AT91SAM7X) || defined (MCU_AT91SAM7S256) || defined (MCU_AT91SAM7SE512)
#define LCD_EN_PIO_ID PIOA_ID
#elif defined(MCU_AT91SAM9260) || defined(MCU_AT91SAM9XE512)
#define LCD_CS_PIO_ID PIOB_ID
#else
#define LCD_CS_PIO_ID PIO_ID
#endif
#endif

#ifndef LCD_EN_PIO_ID
#if defined(MCU_AT91SAM7X) || defined (MCU_AT91SAM7S256) || defined (MCU_AT91SAM7SE512)
#define LCD_EN_PIO_ID PIOA_ID
#elif defined(MCU_AT91SAM9260) || defined(MCU_AT91SAM9XE512)
#define LCD_EN_PIO_ID PIOB_ID
#else
#define LCD_EN_PIO_ID PIO_ID
#endif
#endif

#ifndef LCD_RW_PIO_ID
#if defined(MCU_AT91SAM7X) || defined (MCU_AT91SAM7S256) || defined (MCU_AT91SAM7SE512)
#define LCD_RW_PIO_ID PIOA_ID
#elif defined(MCU_AT91SAM9260) || defined(MCU_AT91SAM9XE512)
#define LCD_RW_PIO_ID PIOB_ID
#else
#define LCD_RW_PIO_ID PIO_ID
#endif
#endif

#ifndef LCD_RS_PIO_ID
#if defined(MCU_AT91SAM7X) || defined (MCU_AT91SAM7S256) || defined (MCU_AT91SAM7SE512)
#define LCD_RS_PIO_ID PIOA_ID
#elif defined(MCU_AT91SAM9260) || defined(MCU_AT91SAM9XE512)
#define LCD_RS_PIO_ID PIOB_ID
#else
#define LCD_RS_PIO_ID PIO_ID
#endif
#endif

#ifndef LCD_DATA_PIO_ID
#if defined(MCU_AT91SAM7X) || defined (MCU_AT91SAM7S256) || defined (MCU_AT91SAM7SE512)
#define LCD_DATA_PIO_ID PIOA_ID
#elif defined(MCU_AT91SAM9260) || defined(MCU_AT91SAM9XE512)
#define LCD_DATA_PIO_ID PIOB_ID
#else
#define LCD_DATA_PIO_ID PIO_ID
#endif
#endif

/*
 * Control lines abstraction.
 */

#ifdef LCD_EN_BIT
#define LCD_CS      _BV(LCD_EN_BIT)
#if LCD_EN_PIO_ID == PIOA_ID
#define LCD_EN_SET() { outr(PIOA_PER, LCD_CS); outr(PIOA_SODR, LCD_CS); outr(PIOA_OER, LCD_CS); }
#define LCD_EN_CLR() { outr(PIOA_PER, LCD_CS); outr(PIOA_CODR, LCD_CS); outr(PIOA_OER, LCD_CS); }
#elif LCD_EN_PIO_ID == PIOB_ID
#define LCD_EN_SET() { outr(PIOB_PER, LCD_CS); outr(PIOB_SODR, LCD_CS); outr(PIOB_OER, LCD_CS); }
#define LCD_EN_CLR() { outr(PIOB_PER, LCD_CS); outr(PIOB_CODR, LCD_CS); outr(PIOB_OER, LCD_CS); }
#elif LCD_EN_PIO_ID == PIOC_ID
#define LCD_EN_SET() { outr(PIOC_PER, LCD_CS); outr(PIOC_SODR, LCD_CS); outr(PIOC_OER, LCD_CS); }
#define LCD_CS_CLR() { outr(PIOC_PER, LCD_CS); outr(PIOC_CODR, LCD_CS); outr(PIOC_OER, LCD_CS); }
#else
#define LCD_EN_SET() { outr(PIO_PER, LCD_CS); outr(PIO_SODR, LCD_CS); outr(PIO_OER, LCD_CS); }
#define LCD_EN_CLR() { outr(PIO_PER, LCD_CS); outr(PIO_CODR, LCD_CS); outr(PIO_OER, LCD_CS); }
#endif /* LCD_EN_PIO_ID */
#else /* LCD_EN_BIT */
#define LCD_EN_SET()
#define LCD_EN_CLR()
#endif /* LCD_EN_BIT */

#ifdef LCD_RS_BIT
#define LCD_RS      _BV(LCD_RS_BIT)
#if LCD_RS_PIO_ID == PIOA_ID
#define LCD_RS_SET() { outr(PIOA_PER, LCD_RS); outr(PIOA_SODR, LCD_RS); outr(PIOA_OER, LCD_RS); }
#define LCD_RS_CLR() { outr(PIOA_PER, LCD_RS); outr(PIOA_CODR, LCD_RS); outr(PIOA_OER, LCD_RS); }
#elif LCD_RS_PIO_ID == PIOB_ID
#define LCD_RS_SET() { outr(PIOB_PER, LCD_RS); outr(PIOB_SODR, LCD_RS); outr(PIOB_OER, LCD_RS); }
#define LCD_RS_CLR() { outr(PIOB_PER, LCD_RS); outr(PIOB_CODR, LCD_RS); outr(PIOB_OER, LCD_RS); }
#elif LCD_RS_PIO_ID == PIOC_ID
#define LCD_RS_SET() { outr(PIOC_PER, LCD_RS); outr(PIOC_SODR, LCD_RS); outr(PIOC_OER, LCD_RS); }
#define LCD_RS_CLR() { outr(PIOC_PER, LCD_RS); outr(PIOC_CODR, LCD_RS); outr(PIOC_OER, LCD_RS); }
#else
#define LCD_RS_SET() { outr(PIO_PER, LCD_RS); outr(PIO_SODR, LCD_RS); outr(PIO_OER, LCD_RS); }
#define LCD_RS_CLR() { outr(PIO_PER, LCD_RS); outr(PIO_CODR, LCD_RS); outr(PIO_OER, LCD_RS); }
#endif /* LCD_RS_PIO_ID */
#else /* LCD_RS_BIT */
#define LCD_RS_SET()
#define LCD_RS_CLR()
#endif /* LCD_RS_BIT */

#ifdef LCD_RW_BIT
#define LCD_RW      _BV(LCD_RW_BIT)
#if LCD_RS_PIO_ID == PIOA_ID
#define LCD_RW_SET() { outr(PIOA_PER, LCD_RS); outr(PIOA_SODR, LCD_RS); outr(PIOA_OER, LCD_RS); }
#define LCD_RW_CLR() { outr(PIOA_PER, LCD_RS); outr(PIOA_CODR, LCD_RS); outr(PIOA_OER, LCD_RS); }
#elif LCD_RS_PIO_ID == PIOB_ID
#define LCD_RW_SET() { outr(PIOB_PER, LCD_RS); outr(PIOB_SODR, LCD_RS); outr(PIOB_OER, LCD_RS); }
#define LCD_RW_CLR() { outr(PIOB_PER, LCD_RS); outr(PIOB_CODR, LCD_RS); outr(PIOB_OER, LCD_RS); }
#elif LCD_RS_PIO_ID == PIOC_ID
#define LCD_RW_SET() { outr(PIOC_PER, LCD_RS); outr(PIOC_SODR, LCD_RS); outr(PIOC_OER, LCD_RS); }
#define LCD_RW_CLR() { outr(PIOC_PER, LCD_RS); outr(PIOC_CODR, LCD_RS); outr(PIOC_OER, LCD_RS); }
#else
#define LCD_RW_SET() { outr(PIO_PER, LCD_RS); outr(PIO_SODR, LCD_RS); outr(PIO_OER, LCD_RS); }
#define LCD_RW_CLR() { outr(PIO_PER, LCD_RS); outr(PIO_CODR, LCD_RS); outr(PIO_OER, LCD_RS); }
#endif /* LCD_RW_PIO_ID */
#else /* LCD_RW_BIT */
#define LCD_RW_SET()
#define LCD_RW_CLR()
#endif /* LCD_RW_BIT */

#ifdef LCD_EN2_BIT
#define LCD_EN2     _BV(LCD_EN2_BIT)
#if LCD_EN2_PIO_ID == PIOA_ID
#define LCD_EN2_SET() { outr(PIOA_PER, LCD_EN2); outr(PIOA_SODR, LCD_EN2); outr(PIOA_OER, LCD_EN2); }
#define LCD_EN2_CLR() { outr(PIOA_PER, LCD_EN2); outr(PIOA_CODR, LCD_EN2); outr(PIOA_OER, LCD_EN2); }
#elif LCD_EN2_PIO_ID == PIOB_ID
#define LCD_EN2_SET() { outr(PIOB_PER, LCD_EN2); outr(PIOB_SODR, LCD_EN2); outr(PIOB_OER, LCD_EN2); }
#define LCD_EN2_CLR() { outr(PIOB_PER, LCD_EN2); outr(PIOB_CODR, LCD_EN2); outr(PIOB_OER, LCD_EN2); }
#elif LCD_EN2_PIO_ID == PIOC_ID
#define LCD_EN2_SET() { outr(PIOC_PER, LCD_EN2); outr(PIOC_SODR, LCD_EN2); outr(PIOC_OER, LCD_EN2); }
#define LCD_EN2_CLR() { outr(PIOC_PER, LCD_EN2); outr(PIOC_CODR, LCD_EN2); outr(PIOC_OER, LCD_EN2); }
#else
#define LCD_EN2_SET() { outr(PIO_PER, LCD_EN2); outr(PIO_SODR, LCD_EN2); outr(PIO_OER, LCD_EN2); }
#define LCD_EN2_CLR() { outr(PIO_PER, LCD_EN2); outr(PIO_CODR, LCD_EN2); outr(PIO_OER, LCD_EN2); }
#endif /* LCD_EN2_PIO_ID */
#else /* LCD_EN2_BIT */
#define LCD_EN2_SET()
#define LCD_EN2_CLR()
#endif /* LCD_EN2_BIT */

#ifdef LCD_RST_BIT
#define LCD_RST    _BV(LCD_RST_BIT)
#if LCD_RST_PIO_ID == PIOA_ID
#define LCD_RST_SET() { outr(PIOA_PER, LCD_RST); outr(PIOA_SODR, LCD_RST); outr(PIOA_OER, LCD_RST); }
#define LCD_RST_CLR() { outr(PIOA_PER, LCD_RST); outr(PIOA_CODR, LCD_RST); outr(PIOA_OER, LCD_RST); }
#elif LCD_RST_PIO_ID == PIOB_ID
#define LCD_RST_SET() { outr(PIOB_PER, LCD_RST); outr(PIOB_SODR, LCD_RST); outr(PIOB_OER, LCD_RST); }
#define LCD_RST_CLR() { outr(PIOB_PER, LCD_RST); outr(PIOB_CODR, LCD_RST); outr(PIOB_OER, LCD_RST); }
#elif LCD_RST_PIO_ID == PIOC_ID
#define LCD_RST_SET() { outr(PIOC_PER, LCD_RST); outr(PIOC_SODR, LCD_RST); outr(PIOC_OER, LCD_RST); }
#define LCD_RST_CLR() { outr(PIOC_PER, LCD_RST); outr(PIOC_CODR, LCD_RST); outr(PIOC_OER, LCD_RST); }
#else
#define LCD_RST_SET() { outr(PIO_PER, LCD_RST); outr(PIO_SODR, LCD_RST); outr(PIO_OER, LCD_RST); }
#define LCD_RST_CLR() { outr(PIO_PER, LCD_RST); outr(PIO_CODR, LCD_RST); outr(PIO_OER, LCD_RST); }
#endif /* LCD_RST_PIO_ID */
#else /* LCD_RST_BIT */
#define LCD_RST_SET()
#define LCD_RST_CLR()
#endif /* LCD_RST_BIT */


#if LCD_DATA_PIO_ID == PIOA_ID
#define LCD_DATA_BASE PIOA_BASE
#elif LCD_DATA_PIO_ID == PIOB_ID
#define LCD_DATA_BASE PIOB_BASE
#elif LCD_DATA_PIO_ID == PIOC_ID
#define LCD_DATA_BASE PIOC_BASE
#else /* LCD_DATA_PIO_ID */
#define LCD_DATA_BASE PIO_BASE
#endif

/*!
 * \addtogroup charlcd
 */
/*@{*/

#if defined(LCD_HD44780) || defined(LCD_KS0066) || defined(LCD_KS0073)
#include "hd44780_at91.c"
#elif defined(LCD_ST7036)
#include "st7036_at91.c"
#endif

/*@}*/
