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

#ifdef LCD_CLK_BIT
#define LCD_CLK     _BV(LCD_CLK_BIT)
#if LCD_CLK_PIO_ID == PIOA_ID
#define LCD_CLK_SET() { outr(PIOA_PER, LCD_CLK); outr(PIOA_SODR, LCD_CLK); outr(PIOA_OER, LCD_CLK); }
#define LCD_CLK_CLR() { outr(PIOA_PER, LCD_CLK); outr(PIOA_CODR, LCD_CLK); outr(PIOA_OER, LCD_CLK); }
#elif LCD_CLK_PIO_ID == PIOB_ID
#define LCD_CLK_SET() { outr(PIOB_PER, LCD_CLK); outr(PIOB_SODR, LCD_CLK); outr(PIOB_OER, LCD_CLK); }
#define LCD_CLK_CLR() { outr(PIOB_PER, LCD_CLK); outr(PIOB_CODR, LCD_CLK); outr(PIOB_OER, LCD_CLK); }
#elif LCD_CLK_PIO_ID == PIOC_ID
#define LCD_CLK_SET() { outr(PIOC_PER, LCD_CLK); outr(PIOC_SODR, LCD_CLK); outr(PIOC_OER, LCD_CLK); }
#define LCD_CLK_CLR() { outr(PIOC_PER, LCD_CLK); outr(PIOC_CODR, LCD_CLK); outr(PIOC_OER, LCD_CLK); }
#else
#define LCD_CLK_SET() { outr(PIO_PER, LCD_CLK); outr(PIO_SODR, LCD_CLK); outr(PIO_OER, LCD_CLK); }
#define LCD_CLK_CLR() { outr(PIO_PER, LCD_CLK); outr(PIO_CODR, LCD_CLK); outr(PIO_OER, LCD_CLK); }
#endif /* LCD_CLK_PIO_ID */
#else /* LCD_CLK_BIT */
#define LCD_CLK_SET()
#define LCD_CLK_CLR()
#endif /* LCD_CLK_BIT */

#ifdef LCD_MOSI_BIT
#define LCD_MOSI    _BV(LCD_MOSI_BIT)
#if LCD_MOSI_PIO_ID == PIOA_ID
#define LCD_MOSI_SET() { outr(PIOA_PER, LCD_MOSI); outr(PIOA_SODR, LCD_MOSI); outr(PIOA_OER, LCD_MOSI); }
#define LCD_MOSI_CLR() { outr(PIOA_PER, LCD_MOSI); outr(PIOA_CODR, LCD_MOSI); outr(PIOA_OER, LCD_MOSI); }
#elif LCD_MOSI_PIO_ID == PIOB_ID
#define LCD_MOSI_SET() { outr(PIOB_PER, LCD_MOSI); outr(PIOB_SODR, LCD_MOSI); outr(PIOB_OER, LCD_MOSI); }
#define LCD_MOSI_CLR() { outr(PIOB_PER, LCD_MOSI); outr(PIOB_CODR, LCD_MOSI); outr(PIOB_OER, LCD_MOSI); }
#elif LCD_MOSI_PIO_ID == PIOC_ID
#define LCD_MOSI_SET() { outr(PIOC_PER, LCD_MOSI); outr(PIOC_SODR, LCD_MOSI); outr(PIOC_OER, LCD_MOSI); }
#define LCD_MOSI_CLR() { outr(PIOC_PER, LCD_MOSI); outr(PIOC_CODR, LCD_MOSI); outr(PIOC_OER, LCD_MOSI); }
#else
#define LCD_MOSI_SET() { outr(PIO_PER, LCD_MOSI); outr(PIO_SODR, LCD_MOSI); outr(PIO_OER, LCD_MOSI); }
#define LCD_MOSI_CLR() { outr(PIO_PER, LCD_MOSI); outr(PIO_CODR, LCD_MOSI); outr(PIO_OER, LCD_MOSI); }
#endif /* LCD_MOSI_PIO_ID */
#else /* LCD_MOSI_BIT */
#define LCD_MOSI_SET()
#define LCD_MOSI_CLR()
#endif /* LCD_MOSI_BIT */

/*!
 * \addtogroup xgST7036
 */
/*@{*/

#ifndef LCD_SHORT_DELAY
#define LCD_SHORT_DELAY 100
#endif

#ifndef LCD_LONG_DELAY
#define LCD_LONG_DELAY  1000
#endif

/*!
 * \brief Wait until controller will be ready again
 *
 * If LCD_WR_BIT is defined we will wait until the ready bit is set, otherwise
 * We will either busy loop with NutDelay or sleep with NutSleep. The second
 * option will be used if we have defined NUT_CPU_FREQ. In this case we have a higher
 * timer resolution.
 *
 * \param xt Delay time in milliseconds
 */
static void LcdDelay(unsigned int cycles)
{
    while (cycles--) {
        _NOP(); _NOP(); _NOP(); _NOP();
        _NOP(); _NOP(); _NOP(); _NOP();
        _NOP(); _NOP(); _NOP(); _NOP();
        _NOP(); _NOP(); _NOP(); _NOP();
        _NOP(); _NOP(); _NOP(); _NOP();
        _NOP(); _NOP(); _NOP(); _NOP();
        _NOP(); _NOP(); _NOP(); _NOP();
        _NOP(); _NOP(); _NOP(); _NOP();
        _NOP(); _NOP(); _NOP(); _NOP();
        _NOP(); _NOP(); _NOP(); _NOP();
        _NOP(); _NOP(); _NOP(); _NOP();
        _NOP(); _NOP(); _NOP(); _NOP();
    }
}

#if 0
static void INLINE LcdSetBits(unsigned int mask)
{
    outr(LCD_PIO_SOD_REG, mask);
    outr(LCD_PIO_OE_REG, mask);
}

static void INLINE LcdClrBits(unsigned int mask)
{
    outr(LCD_PIO_COD_REG, mask);
    outr(LCD_PIO_OE_REG, mask);
}
#endif

static void LcdWaitReady(unsigned int delay)
{
    while (delay--) {
        _NOP();
    }
}

/*!
 * \brief Send byte to LCD controller.
 *
 * \param data Byte to send.
 */
static void LcdWriteByte(unsigned int data)
{
    uint8_t msk = 0x80;

    while (msk) {
        LCD_CLK_CLR();
        if (data & msk) {
            LCD_MOSI_SET();
        } else {
            LCD_MOSI_CLR();
        }
        LCD_CLK_SET();
        msk >>= 1;
        LcdDelay(1);
    }
    LcdWaitReady(LCD_LONG_DELAY);
}

/*!
 * \brief Send command byte to LCD controller.
 *
 * \param cmd Byte to send.
 */
static void LcdWriteCmd(uint8_t cmd)
{
    /* RS low selects instruction register. */
    LCD_EN_CLR();
    LcdDelay(LCD_SHORT_DELAY);
    LCD_RS_CLR();
    LcdDelay(LCD_SHORT_DELAY);
    LcdWriteByte(cmd);
    LcdDelay(LCD_SHORT_DELAY);
    LCD_RS_SET();
    LcdDelay(LCD_SHORT_DELAY);
    LCD_EN_SET();
    LcdDelay(LCD_SHORT_DELAY);
}

static void LcdWriteInstruction(uint8_t cmd, uint8_t xt)
{
    LcdWriteCmd(cmd);
}

/*!
 * \brief Send data byte to LCD controller.
 *
 * \param data Byte to send.
 */
static void LcdWriteData(uint8_t data)
{
    /* RS high selects data register. */
	LCD_EN_CLR();
    LcdDelay(LCD_SHORT_DELAY);
    LCD_RS_SET();
    LcdDelay(LCD_SHORT_DELAY);
    LcdWriteByte(data);
    LcdDelay(LCD_SHORT_DELAY);
	LCD_EN_SET();
    LcdDelay(LCD_SHORT_DELAY);
}

static void LcdSetCursor(uint8_t pos)
{
    uint8_t offset[] = {
        0x00, 0x40, 0x10, 0x50
    };

    pos = offset[(pos / LCD_COLS) % LCD_ROWS] + pos % LCD_COLS;
    LcdWriteCmd(0x80 | pos);
}

static void LcdCursorHome(void)
{
    LcdWriteCmd(0x02);
    LcdDelay(10 * LCD_LONG_DELAY);
}

static void LcdCursorLeft(void)
{
    LcdWriteCmd(0x10);
}

static void LcdCursorRight(void)
{
    LcdWriteCmd(0x14);
}

static void LcdClear(void)
{
    LcdWriteCmd(0x01);
    LcdDelay(10 * LCD_LONG_DELAY);
}

static void LcdCursorMode(uint8_t on)
{
    if (on) {
        LcdWriteCmd(0x0D);
    } else {
        LcdWriteCmd(0x0C);
    }
    LcdDelay(10 * LCD_LONG_DELAY);
}

static int LcdInit(NUTDEVICE * dev)
{
#if defined(PMC_PCER)
    unsigned int pcer = 0;
#if defined(LCD_EN_PIO_ID)
    pcer = _BV(LCD_EN_PIO_ID);
#endif
#if defined(LCD_RS_PIO_ID)
    pcer |= _BV(LCD_RS_PIO_ID);
#endif
#if defined(LCD_CLK_PIO_ID)
    pcer |= _BV(LCD_CLK_PIO_ID);
#endif
#if defined(LCD_MOSI_PIO_ID)
    pcer |= _BV(LCD_MOSI_PIO_ID);
#endif
    outr(PMC_PCER, pcer);
#endif

    /* Initialize GPIO lines. */
    LCD_EN_SET();
    LCD_RS_SET();
    LCD_CLK_SET();
    LCD_MOSI_SET();

    /* Initial delay. Actually only required after power on. */
    NutSleep(50);

	LCD_RS_CLR();
	LCD_EN_CLR();

	LcdWriteCmd(0x38);    /* Function set. */
	NutSleep(2);
	LcdWriteCmd(0x39);    /* Function set. */
	NutSleep(2);
	LcdWriteCmd(0x14);    /* Bias 1/5, 2 lines. */
	NutSleep(1);
	LcdWriteCmd(0x55);    /* Power/ICON/Contrast control. */
	NutSleep(1);
	LcdWriteCmd(0x6D);    /* Follower control. */
	NutSleep(1);
	LcdWriteCmd(0x78);    /* Booster on, Contrast set. */
	NutSleep(1);
	LcdWriteCmd(0x0F);    /* Display on. */
	NutSleep(1);
	LcdWriteCmd(0x01);    /* Clear display. */
	NutSleep(1);
	LcdWriteCmd(0x06);    /* Entry mode set. */
	NutSleep(1);

	LCD_EN_SET();
	LCD_RS_SET();

    /* Clear display. */
    LcdClear();
    /* Move cursor home. */
    LcdCursorHome();

    return 0;
}

/*!
 * \brief Terminal device control block structure.
 */
static TERMDCB dcb_term = {
    LcdInit,                    /*!< \brief Initialize display subsystem, dss_init. */
    LcdWriteData,               /*!< \brief Write display character, dss_write. */
    LcdWriteInstruction,        /*!< \brief Write display command, dss_command. */
    LcdClear,                   /*!< \brief Clear display, dss_clear. */
    LcdSetCursor,               /*!< \brief Set display cursor, dss_set_cursor. */
    LcdCursorHome,              /*!< \brief Set display cursor home, dss_cursor_home. */
    LcdCursorLeft,              /*!< \brief Move display cursor left, dss_cursor_left. */
    LcdCursorRight,             /*!< \brief Move display cursor right, dss_cursor_right. */
    LcdCursorMode,              /*!< \brief Switch cursor on/off, dss_cursor_mode. */
    0,                          /*!< \brief Mode flags. */
    0,                          /*!< \brief Status flags. */
    LCD_ROWS,                   /*!< \brief Number of rows. */
    LCD_COLS,                   /*!< \brief Number of columns per row. */
    LCD_COLS,                   /*!< \brief Number of visible columns. */
    0,                          /*!< \brief Cursor row. */
    0,                          /*!< \brief Cursor column. */
    0                           /*!< \brief Display shadow memory. */
};

/*!
 * \brief LCD device information structure.
 */
NUTDEVICE devSbiLcd = {
    0,                          /*!< Pointer to next device. */
    {'s', 'b', 'i', 'l', 'c', 'd', 0, 0, 0},  /*!< Unique device name. */
    IFTYP_STREAM,               /*!< Type of device. */
    0,                          /*!< Base address. */
    0,                          /*!< First interrupt number. */
    0,                          /*!< Interface control block. */
    &dcb_term,                  /*!< Driver control block. */
    TermInit,                   /*!< Driver initialization routine. */
    TermIOCtl,                  /*!< Driver specific control function. */
    0,
    TermWrite,
    TermOpen,
    TermClose,
    0
};

/*@}*/
