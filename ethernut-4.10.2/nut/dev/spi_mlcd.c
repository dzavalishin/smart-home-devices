/*
 * Copyright (C) 2009 by egnite GmbH
 * Copyright (C) 2001-2003 by egnite Software GmbH
 *
 * All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
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
 * $Id: spi_mlcd.c 2651 2009-08-27 21:18:28Z Astralix $
 */

#include <dev/term.h>
#include <sys/timer.h>

#include <stdlib.h>
#include <string.h>

#ifndef LCD_ROWS
#define LCD_ROWS    2
#endif
#ifndef LCD_COLS
#define LCD_COLS    16
#endif


/*!
 * \addtogroup xgDisplay
 */
/*@{*/

static void LcdWriteData(uint8_t ch)
{
}

static void LcdWriteCmd(uint8_t cmd, uint8_t xt)
{
}

static void LcdSetCursor(uint8_t pos)
{
}

static void LcdCursorHome(void)
{
}

static void LcdCursorLeft(void)
{
}

static void LcdCursorRight(void)
{
}

static void LcdClear(void)
{
}

static void LcdCursorMode(uint8_t on)
{
}

static int LcdInit(NUTDEVICE *dev)
{
    return 0;
}

/*!
 * \brief Terminal device control block structure.
 */
static TERMDCB dcb_term = {
    LcdInit,            /*!< \brief Initialize display subsystem, dss_init. */
    LcdWriteData,       /*!< \brief Write display character, dss_write. */
    LcdWriteCmd,        /*!< \brief Write display command, dss_command. */
    LcdClear,           /*!< \brief Clear display, dss_clear. */
    LcdSetCursor,       /*!< \brief Set display cursor, dss_set_cursor. */
    LcdCursorHome,      /*!< \brief Set display cursor home, dss_cursor_home. */
    LcdCursorLeft,      /*!< \brief Move display cursor left, dss_cursor_left. */
    LcdCursorRight,     /*!< \brief Move display cursor right, dss_cursor_right. */
    LcdCursorMode,      /*!< \brief Switch cursor on/off, dss_cursor_mode. */
    0,                  /*!< \brief Mode flags. */
    0,                  /*!< \brief Status flags. */
    LCD_ROWS,           /*!< \brief Number of rows. */
    LCD_COLS,           /*!< \brief Number of columns per row. */
    LCD_COLS,           /*!< \brief Number of visible columns. */
    0,                  /*!< \brief Cursor row. */
    0,                  /*!< \brief Cursor column. */
    0                   /*!< \brief Display shadow memory. */
};

/*!
 * \brief LCD device information structure.
 */
NUTDEVICE devSpiMegaLcd = {
    0,              /*!< Pointer to next device. */
    {'l', 'c', 'd', 0, 0, 0, 0, 0, 0},      /*!< Unique device name. */
    IFTYP_STREAM,   /*!< Type of device. */
    0,              /*!< Base address. */
    0,              /*!< First interrupt number. */
    0,              /*!< Interface control block. */
    &dcb_term,      /*!< Driver control block. */
    TermInit,       /*!< Driver initialization routine. */
    TermIOCtl,      /*!< Driver specific control function. */
    0,
    TermWrite,
#ifdef __HARVARD_ARCH__
    TermWrite_P,
#endif
    TermOpen,
    TermClose,
    0
};

/*@}*/
