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
 * \file dev/hd44780_bus.c
 * \brief Terminal device definitions for memory mapped lcd.
 *
 *
 * This is a terminal device driver for a memory mapped hd44780 compatible
 * lcd. It is connected to the databus / adressbus. A Chipselect is generated
 * from /rd, /wr, and the address decoder. It is connected to the lcds enable
 * signal. A0 is connected to the register select pin and A1 to the read/write
 * signal. Therefore you'll read from an address with an offset of two
 *
 * Have a look to our m-can board if you have questions.
 */

/*
 * $Log$
 * Revision 1.6  2008/08/27 06:35:15  thornen
 * Added support for MMnet03..04 and MMnet102..104
 *
 * Revision 1.5  2008/08/26 17:36:45  haraldkipp
 * Revoked changes 2008/08/26 by thornen.
 *
 * Revision 1.3  2008/08/11 06:59:15  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.2  2005/09/07 16:23:11  christianwelzel
 * Added support for MMnet02 display
 *
 * Revision 1.1  2005/07/26 18:02:27  haraldkipp
 * Moved from dev.
 *
 * Revision 1.8  2005/05/27 14:05:25  olereinhardt
 * Added support for new display sizes configurable by macros
 * LCD_4x20, LCD_4x16, LCD_2x40, LCD_2x20, LCD_2x16, LCD_2x8,
 * LCD_1x20, LCD_1x16, LCD_1x8, KS0073_CONTROLLER (4x20))
 *
 * Revision 1.7  2004/10/14 08:55:38  olereinhardt
 * Added default LCD type to avoid compiling bug if no type is defined
 *
 * Revision 1.6  2004/09/17 14:31:06  olereinhardt
 * Compile only if __GNUC__ defined
 *
 * Revision 1.5  2004/08/26 14:00:04  olereinhardt
 * Fixed cursor positioning for different devices
 *
 * Revision 1.4  2004/05/27 15:03:14  olereinhardt
 * Changed copyright notice
 *
 * Revision 1.3  2004/05/25 17:33:01  drsung
 * Added 'log' keyword for CVS.
 *
 */

/* Not ported. */
#ifdef __GNUC__

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <sys/nutconfig.h>
#include <dev/hd44780_bus.h>
#include <dev/term.h>
#include <sys/timer.h>

static uint16_t lcd_base = 0x0000;

#ifndef LCD_4x20
#ifndef LCD_4x16
#ifndef LCD_2x40
#ifndef LCD_2x20
#ifndef LCD_2x16
#ifndef LCD_2x8
#ifndef LCD_1x20
#ifndef LCD_1x16
#ifndef LCD_1x8
#ifndef KS0073_CONTROLLER
#define LCD_2x16
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif
#endif

#define LCD_DELAY		asm volatile ("nop"); asm volatile ("nop")


/*!
 * \addtogroup xgDisplay
 */
/*@{*/

/*!
 * \brief Waits until Busy bit is reset or timeout
 */


static inline void LcdBusyWait(void)
{
#if !defined(MMNET02)  && !defined(MMNET03)  && !defined(MMNET04) && \
	!defined(MMNET102) && !defined(MMNET103) && !defined(MMNET104)
    // wait until LCD busy bit goes to zero
    // do a read from control register
    while (*(volatile uint8_t *) (LCD_CTRL_ADDR + LCD_READ_OFFSET) & 1 << LCD_BUSY)
        LCD_DELAY;
    LCD_DELAY;
    LCD_DELAY;
    LCD_DELAY;
    LCD_DELAY;
    LCD_DELAY;
    LCD_DELAY;
    LCD_DELAY;
    LCD_DELAY;
    LCD_DELAY;
    LCD_DELAY;
    LCD_DELAY;
    LCD_DELAY;
#else
    /* MMnet02 can not read the control register */
    NutDelay(5);
#endif
}

/*!
 * \brief Write databyte to LCD controller over data bus
 *
 * \param ch Byte to send.
 */

static void LcdWriteData(uint8_t data)
{
    LcdBusyWait();              // wait until LCD not busy
    *(volatile uint8_t *) (LCD_DATA_ADDR) = data;
}

/*!
 * \brief Write command byte to LCD controller.
 */
static void LcdWriteCmd(uint8_t cmd, uint8_t delay)
{
    LcdBusyWait();              // wait until LCD not busy
    *(volatile uint8_t *) (LCD_CTRL_ADDR) = cmd;
}

static void LcdSetCursor(uint8_t pos)
{
    uint8_t x = 0;
    uint8_t y = 0;

#ifdef KS0073_CONTROLLER
    uint8_t  offset[4] = {0x00, 0x20, 0x40, 0x60};
    y = pos / 20;
    x = pos % 20;
    if (y > 3) y = 3;
#endif

#if defined(LCD_2x40) 
    uint8_t  offset  [2] = {0x00, 0x40};
    y = pos / 40;
    x = pos % 40;
    if (y > 1) y = 1;
#endif    
    
#if defined(LCD_4x20) || defined(LCD_2x20)
    uint8_t  offset  [4] = {0x00, 0x40, 0x14, 0x54};
    y = pos / 20;
    x = pos % 20;
    if (y>3) y=3;
#endif    
    
#if defined(LCD_4x16) || defined(LCD_2x16)
    uint8_t  offset  [4] = {0x00, 0x40, 0x10, 0x50};
    y = pos / 16;
    x = pos % 16;
    if (y>3) y=3;
#endif    

#if defined(LCD_2x8)
    uint8_t  offset  [2] = {0x00, 0x40};
    y = pos / 8;
    x = pos % 8;
    if (y>1) y=1;
#endif    

#if defined(LCD_1x8) || defined(LCD_1x16) || defined(LCD_1x20)
    uint8_t  offset  [1] = { 0x00 };
    y = 0;
    x = pos;
#endif 
    
    pos = x + offset[y];
    LcdWriteCmd(1 << LCD_DDRAM | pos, 0);
}

static void LcdCursorHome(void)
{
    LcdWriteCmd(1 << LCD_HOME, 0);
}

static void LcdCursorLeft(void)
{
    LcdWriteCmd(1 << LCD_MOVE, 0);
}

static void LcdCursorRight(void)
{
    LcdWriteCmd(1 << LCD_MOVE | 1 << LCD_MOVE_RIGHT, 0);
}

static void LcdClear(void)
{
    LcdWriteCmd(1 << LCD_CLR, 0);
}

static void LcdCursorMode(uint8_t on)
{
    LcdWriteCmd(1 << LCD_ON_CTRL | on ? 1 << LCD_ON_CURSOR : 0x00, 0);
}

/*!
 * \brief Initialization of the LCD controller
 */

static int LcdInit(NUTDEVICE * dev)
{
    lcd_base = dev->dev_base;
#ifdef  KS0073_CONTROLLER
    // LCD function set with extended Register Set.
    LcdWriteCmd((1 << LCD_FUNCTION) | 1 << LCD_FUNCTION_8BIT | (1 << LCD_FUNCTION_RE), 0);
    NutDelay(50);
    LcdWriteCmd((1 << LCD_FUNCTION) | 1 << LCD_FUNCTION_8BIT | (1 << LCD_FUNCTION_RE), 0);
    NutDelay(50);
    LcdWriteCmd((1 << LCD_FUNCTION) | 1 << LCD_FUNCTION_8BIT | (1 << LCD_FUNCTION_RE), 0);
    NutDelay(50);

    LcdWriteCmd((1 << LCD_EXT) | ((((TERMDCB *) dev->dev_dcb)->dcb_nrows > 2) ? (1 << LCD_EXT_4LINES) : 0), 0);
    LcdWriteCmd((1 << LCD_FUNCTION) | 1 << LCD_FUNCTION_8BIT, 0);

#else

    // LCD function set

    LcdWriteCmd((1 << LCD_FUNCTION) | 1 << LCD_FUNCTION_8BIT | (1 << LCD_FUNCTION_2LINES), 0);
    NutDelay(50);
    LcdWriteCmd((1 << LCD_FUNCTION) | 1 << LCD_FUNCTION_8BIT | (1 << LCD_FUNCTION_2LINES), 0);
    NutDelay(50);
    LcdWriteCmd((1 << LCD_FUNCTION) | 1 << LCD_FUNCTION_8BIT | (1 << LCD_FUNCTION_2LINES), 0);
    NutDelay(50);
#endif
    // clear LCD
    LcdWriteCmd(1 << LCD_CLR, 0);
    // set entry mode
    LcdWriteCmd(1 << LCD_ENTRY_MODE | 1 << LCD_ENTRY_INC, 0);
    // set display to on
    LcdWriteCmd(1 << LCD_ON_CTRL | 1 << LCD_ON_DISPLAY, 0);
    // move cursor to home
    LcdWriteCmd(1 << LCD_HOME, 0);
    // set data address to 0
    LcdWriteCmd(1 << LCD_DDRAM | 0x00, 0);

    return 0;
}

/*!
 * \brief Terminal device control block structure.
 */
TERMDCB dcb_term = {
    LcdInit,                    /*!< \brief Initialize display subsystem, dss_init. */
    LcdWriteData,               /*!< \brief Write display character, dss_write. */
    LcdWriteCmd,                /*!< \brief Write display command, dss_command. */
    LcdClear,                   /*!< \brief Clear display, dss_clear. */
    LcdSetCursor,               /*!< \brief Set display cursor, dss_set_cursor. */
    LcdCursorHome,              /*!< \brief Set display cursor home, dss_cursor_home. */
    LcdCursorLeft,              /*!< \brief Move display cursor left, dss_cursor_left. */
    LcdCursorRight,             /*!< \brief Move display cursor right, dss_cursor_right. */
    LcdCursorMode,              /*!< \brief Switch cursor on/off, dss_cursor_mode. */
    0,                          /*!< \brief Mode flags. */
    0,                          /*!< \brief Status flags. */
#ifdef KS0073_CONTROLLER
    4,                  /*!< \brief Number of rows. */
    20,                 /*!< \brief Number of columns per row. */
    20,                 /*!< \brief Number of visible columns. */
#endif
#ifdef LCD_4x20
    4,                  /*!< \brief Number of rows. */
    20,                 /*!< \brief Number of columns per row. */
    20,                 /*!< \brief Number of visible columns. */
#endif
#ifdef LCD_4x16
    4,                  /*!< \brief Number of rows. */
    16,                 /*!< \brief Number of columns per row. */
    16,                 /*!< \brief Number of visible columns. */
#endif    
#ifdef LCD_2x40    
    2,                  /*!< \brief Number of rows. */
    40,                 /*!< \brief Number of columns per row. */
    40,                 /*!< \brief Number of visible columns. */
#endif
#ifdef LCD_2x20    
    2,                  /*!< \brief Number of rows. */
    20,                 /*!< \brief Number of columns per row. */
    20,                 /*!< \brief Number of visible columns. */
#endif
#ifdef LCD_2x16    
    2,                  /*!< \brief Number of rows. */
    16,                 /*!< \brief Number of columns per row. */
    16,                 /*!< \brief Number of visible columns. */
#endif
#ifdef LCD_2x8    
    2,                  /*!< \brief Number of rows. */
    8,                 /*!< \brief Number of columns per row. */
    8,                 /*!< \brief Number of visible columns. */
#endif
#ifdef LCD_1x20    
    1,                  /*!< \brief Number of rows. */
    20,                 /*!< \brief Number of columns per row. */
    20,                 /*!< \brief Number of visible columns. */
#endif
#ifdef LCD_1x16    
    1,                  /*!< \brief Number of rows. */
    16,                 /*!< \brief Number of columns per row. */
    16,                 /*!< \brief Number of visible columns. */
#endif
#ifdef LCD_1x8    
    1,                  /*!< \brief Number of rows. */
    8,                 /*!< \brief Number of columns per row. */
    8,                 /*!< \brief Number of visible columns. */
#endif
    0,                  /*!< \brief Cursor row. */
    0,                  /*!< \brief Cursor column. */
    0                   /*!< \brief Display shadow memory. */
};

/*!
 * \brief LCD device information structure.
 */
NUTDEVICE devLcdBus = {
    0,                          /*!< Pointer to next device. */
    {'l', 'c', 'd', 'b', 'u', 's', 0, 0, 0},    /*!< Unique device name. */
    IFTYP_STREAM,               /*!< Type of device. */
    0,                          /*!< Base address. */
    0,                          /*!< First interrupt number. */
    0,                          /*!< Interface control block. */
    &dcb_term,                  /*!< Driver control block. */
    TermInit,                   /*!< Driver initialization routine. */
    TermIOCtl,                  /*!< Driver specific control function. */
    0,
    TermWrite,
    TermWrite_P,
    TermOpen,
    TermClose,
    0
};


#else
void keep_icc_happy(void)
{
}

#endif

/*@}*/
