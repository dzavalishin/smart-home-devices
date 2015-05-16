#ifndef _DEV_HD44780_BUS_H_
#define _DEV_HD44780_BUS_H_
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
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgement:
 *
 *    This product includes software developed by egnite Software GmbH
 *    and its contributors.
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
 */
 
/*
 * $Log$
 * Revision 1.4  2007/08/29 07:43:53  haraldkipp
 * Documentation updated and corrected.
 *
 * Revision 1.3  2004/05/27 15:03:54  olereinhardt
 * Changed copyright notice
 *
 * Revision 1.2  2004/05/25 17:33:32  drsung
 * Added 'log' keyword for CVS.
 *
 */

#include <sys/device.h>


/*!
 * \addtogroup xgDevDisplay
 */
/*@{*/

// HD44780 Commandset
#define LCD_CLR             0      // DB0: clear display
#define LCD_HOME            1      // DB1: return to home position
#define LCD_ENTRY_MODE      2      // DB2: set entry mode
#define LCD_ENTRY_INC       1      //   DB1: increment
#define LCD_ENTRY_SHIFT     0      //   DB2: shift
#define LCD_ON_CTRL         3      // DB3: turn lcd/cursor on
#define LCD_ON_DISPLAY      2      //   DB2: turn display on
#define LCD_ON_CURSOR       1      //   DB1: turn cursor on
#define LCD_ON_BLINK        0      //   DB0: blinking cursor
#define LCD_MOVE            4      // DB4: move cursor/display
#define LCD_MOVE_DISP       3      //   DB3: move display (0-> move cursor)
#define LCD_MOVE_RIGHT      2      //   DB2: move right (0-> left)
#define LCD_FUNCTION        5      // DB5: function set
#define LCD_FUNCTION_8BIT   4      //   DB4: set 8BIT mode (0->4BIT mode)
#define LCD_FUNCTION_2LINES 3      //   DB3: two lines (0->one line)
#define LCD_FUNCTION_RE     2      //   DB2: KS0073 Controller: Extended Register
#define LCD_FUNCTION_10DOTS 2      //   DB2: 5x10 font (0->5x7 font)
#define LCD_FUNCTION_DS     1      //   DB1: DisplayShift / DotScroll
#define LCD_FUNCTION_REV    0      //   DB0: Reverse Display
#define LCD_EXT             3      // DB3: Extended Register Set
#define LCD_EXT_FONT        2      //   DB2: Fontwidth: 5 / 6 Pixel
#define LCD_EXT_INVCURS     1      //   DB1: Normal / Inverted Cursor
#define LCD_EXT_4LINES      0      //   DB0: 1/2 Lines (normal) or 4Lines
#define LCD_CGRAM           6      // DB6: set CG RAM address
#define LCD_DDRAM           7      // DB7: set DD RAM address
// reading:
#define LCD_BUSY            7      // DB7: LCD is busy

#define LCD_CTRL_ADDR	(lcd_base + 0x0000)
#define LCD_DATA_ADDR	(lcd_base + 0x0001)
#define LCD_READ_OFFSET 0x0002


/*@}*/

extern NUTDEVICE devLcdBus;

#endif
