/*
 * Copyright (C) 2001-2007 by egnite Software GmbH. All rights reserved.
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
 * Revision 1.12  2009/01/17 11:26:37  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.11  2008/08/11 06:59:09  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.10  2008/08/06 12:51:00  haraldkipp
 * Added support for Ethernut 5 (AT91SAM9XE reference design).
 *
 * Revision 1.9  2008/02/15 16:58:41  haraldkipp
 * Spport for AT91SAM7SE512 added.
 *
 * Revision 1.8  2007/10/04 19:57:54  olereinhardt
 * Support for SAM7S256 added
 *
 * Revision 1.7  2007/02/15 16:05:29  haraldkipp
 * Port usage is now configurable. Data bits no longer need four consecutive
 * port bits. Added delays in read for better reliability with some slow
 * displays.
 *
 * Revision 1.6  2006/10/05 17:11:16  haraldkipp
 * Fixes bug #1567813. Should now work after power on and after reset without
 * power loss. Many thanks to Klaus-Dieter Sohn.
 *
 * Revision 1.5  2006/08/31 19:02:25  haraldkipp
 * Added support for AT91SAM9260.
 * Some displays fail after reset. An additional nibble sent
 * during 4-bit initialization seems to fix this. However,
 * a user reported that his 3.3V driven LCD now fails during
 * power on.
 *
 * Revision 1.4  2006/07/15 11:15:31  haraldkipp
 * Initialization flag removed. It is not required because the driver doesn't
 * poll the busy flag during initialization.
 * Bug fixed, which let the driver fail to properly initialize displays with
 * two lines.
 *
 * Revision 1.3  2006/06/28 17:23:19  haraldkipp
 * Significantly extend delay time to allow running slow 3.3V LCDs with fast
 * CPUs. Not a nice fix, but it works.
 *
 * Revision 1.2  2006/05/15 11:44:06  haraldkipp
 * Added delays for more reliable initialization.
 *
 * Revision 1.1  2006/04/07 13:50:15  haraldkipp
 * ARM driver for HD44780 LCD controller added.
 *
 */

#include <cfg/arch.h>
#include <cfg/arch/armpio.h>
#include <cfg/lcd.h>

#include <stdlib.h>
#include <string.h>
//#include <stdio.h>

#include <sys/nutconfig.h>
#include <dev/hd44780.h>
#include <dev/term.h>
#include <sys/timer.h>

#ifdef LCD_IF_4BIT
#ifdef LCD_DATA_LSB
#define LCD_DATA    (0xF << LCD_DATA_LSB)
#else   /* LCD_DATA_LSB */
#define LCD_D0      _BV(LCD_DATA_BIT0)
#define LCD_D1      _BV(LCD_DATA_BIT1)
#define LCD_D2      _BV(LCD_DATA_BIT2)
#define LCD_D3      _BV(LCD_DATA_BIT3)
#define LCD_DATA    (LCD_D0 | LCD_D1 | LCD_D2 | LCD_D3)
#endif  /* LCD_DATA_LSB */
#else   /* LCD_IF_4BIT */
#define LCD_DATA     (0xFF << LCD_DATA_LSB)
#endif  /* LCD_IF_4BIT */

/*!
 * \addtogroup xgDisplay
 */
/*@{*/

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

#ifdef LCD_PW_EH
void LcdNanoDelay( uint_fast16_t n)
{
    while (n--) {
        _NOP();
    }
}
#else
#define LcdNanoDelay( a)
#endif


static void INLINE LcdSetBits(unsigned int mask)
{
    outr(LCD_DATA_BASE+PIO_SODR_OFF, mask);
    outr(LCD_DATA_BASE+PIO_OER_OFF, mask);
}

static void INLINE LcdClrBits(unsigned int mask)
{
    outr(LCD_DATA_BASE+PIO_CODR_OFF, mask);
    outr(LCD_DATA_BASE+PIO_OER_OFF, mask);
}

#ifdef LCD_RW_BIT
static unsigned int LcdReadNibble(void)
{
    unsigned int rc;

    LCD_EN_SET();
    LcdDelay(LCD_SHORT_DELAY);
    rc = inr(LCD_DATA_PIO_ID+PIO_PDSR_OFF) & LCD_DATA;
    LCD_EN_CLR();
    LcdDelay(LCD_SHORT_DELAY);

#ifdef LCD_DATA_LSB
    rc >>= LCD_DATA_LSB
#else
    {
        unsigned int val = 0;

        if (rc & LCD_D0) {
            val |= 0x01;
        }
        if (rc & LCD_D1) {
            val |= 0x02;
        }
        if (rc & LCD_D2) {
            val |= 0x04;
        }
        if (rc & LCD_D3) {
            val |= 0x08;
        }
        rc = val;
    }
#endif
    return rc;
}

/*!
 * \brief Read byte from LCD controller.
 */
static unsigned int LcdReadByte(void)
{
    outr(LCD_DATA_PIO_ID+PIO_ODR_OFF, LCD_DATA);
    LcdDelay(LCD_SHORT_DELAY);
    LCD_RW_SET();
    LcdDelay(LCD_SHORT_DELAY);
    return (LcdReadNibble() << 4) | LcdReadNibble();
}

/*!
 * \brief Read status byte from LCD controller.
 */
static unsigned int LcdReadStatus(void)
{
    /* RS low selects status register. */
    LCD_RS_CLR();
    return LcdReadByte();
}

#endif                          /* HD44_RW_BIT */

#if 0
/* This function is a bit critical as some chipsets are known to
 * release the redy bit some time early. So after rady goes low,
 * another fixed delay has to be added before soing the next access.
 */
static void LcdWaitReady(unsigned int delay)
{
    while (delay--) {
#if defined(HD44_RW_BIT)
        if ((LcdReadStatus() & _BV(LCD_BUSY)) == 0) {
            break;
        }
#endif
        NutMicroDelay(1);
    }
}
#endif

/*!
 * \brief Send half byte to LCD controller.
 *
 * \param nib The four least significant bits are sent.
 */
static void LcdWriteNibble(unsigned int nib)
{
#ifdef LCD_DATA_LSB
    nib <<= LCD_DATA_LSB;
#else
    {
        unsigned int val = 0;
        if (nib & 0x01) {
            val |= LCD_D0;
        }
        if (nib & 0x02) {
            val |= LCD_D1;
        }
        if (nib & 0x04) {
            val |= LCD_D2;
        }
        if (nib & 0x08) {
            val |= LCD_D3;
        }
        nib = val;
    }
#endif
    LcdSetBits(nib & LCD_DATA);
    LcdClrBits(~nib & LCD_DATA);

    /* Create Enable Pulse:
     * For HD44780 Displays we need:
     * Vcc = 5.0V -> PWeh >= 230ns
     * Vcc = 3.3V -> PWeh >= 500ns
     */
    LCD_EN_SET();
    LcdNanoDelay(LCD_PW_EH);
    LCD_EN_CLR();
}

/*!
 * \brief Send byte to LCD controller.
 *
 * \param data Byte to send.
 */
static void LcdWriteByte(unsigned int data)
{
    /* If configured set RW low */
#ifdef LCD_RW_BIT
    LCD_RW_CLR();
#endif

    /* If using 4-bit access, write two nibbles now */
#ifdef LCD_IF_4BIT
    LcdWriteNibble(data >> 4);
    LcdNanoDelay(LCD_PW_EH);
    LcdWriteNibble(data);
#else
    /* else write one byte */
    data <<= LCD_DATA_LSB;
    LcdSetBits(data & LCD_DATA);
    LcdClrBits(~data & LCD_DATA);
#endif

    /* If configured, let the task sleep before next character */
#if defined(LCD_SLEEP_DLY)
    NutSleep(1);
#else
    /* or add a fixed delay and immediately process next char */
    NutMicroDelay(LCD_E2E_DLY);
#endif
}

/*!
 * \brief Send command byte to LCD controller.
 *
 * \param cmd Byte to send.
 */
static void LcdWriteCmd(uint8_t cmd)
{
    /* RS low selects instruction register. */
    LCD_RS_CLR();
    LcdWriteByte(cmd);
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
    LCD_RS_SET();
    LcdWriteByte(data);
}

static void LcdSetCursor(uint8_t pos)
{
    uint8_t offset[] = {
#ifdef LCD_KS0073
        0x00, 0x20, 0x40, 0x60
#elif (LCD_COLS >= 20)
        0x00, 0x40, 0x14, 0x54
#else
        0x00, 0x40, 0x10, 0x50
#endif
    };

    pos = offset[(pos / LCD_COLS) % LCD_ROWS] + pos % LCD_COLS;
    LcdWriteCmd(1 << LCD_DDRAM | pos);
}

static void LcdCursorHome(void)
{
    LcdWriteCmd(1 << LCD_HOME);
    NutSleep(2);
}

static void LcdCursorLeft(void)
{
    LcdWriteCmd(1 << LCD_MOVE);
}

static void LcdCursorRight(void)
{
    LcdWriteCmd(1 << LCD_MOVE | 1 << LCD_MOVE_RIGHT);
}

static void LcdClear(void)
{
    LcdWriteCmd(_BV(LCD_CLR));
    NutSleep(2);
}

static void LcdCursorMode(uint8_t on)
{
    LcdWriteCmd(1 << LCD_ON_CTRL | on ? 1 << LCD_ON_CURSOR : 0x00);
}

static int LcdInit(NUTDEVICE * dev)
{
#if defined(PMC_PCER)
    outr(PMC_PCER, _BV(LCD_RS_PIO_ID) | _BV(LCD_EN_PIO_ID));
#endif

    /* Initialize GPIO lines. */
#ifdef LCD_RW_BIT
    outr(PMC_PCER, _BV(LCD_RW_PIO_ID));
    LCD_RW_CLR();
#endif

#ifdef LCD_EN2_BIT
    outr(PMC_PCER, _BV(LCD_EN2_PIO_ID));
    LCD_EN2_CLR();
#endif

#ifdef LCD_RST_BIT
    outr(PMC_PCER, _BV(LCD_RST_PIO_ID));
    LCD_RST_CLR();
#endif

    LCD_RS_CLR();
    LCD_RW_CLR();
    LcdClrBits(LCD_DATA);
    NutMicroDelay(30);
    LCD_EN_CLR();
    NutMicroDelay(30);

    /* Initial delay. Actually only required after power on. */
    NutSleep(16);

    /* This initialization will make sure, that the LCD is switched
     * to 8-bit mode, no matter which mode we start from or we finally
     * need.
     */
    LcdWriteNibble((_BV(LCD_FUNCTION) | _BV(LCD_FUNCTION_8BIT)) >> 4);
    NutSleep(15);
    LcdWriteNibble((_BV(LCD_FUNCTION) | _BV(LCD_FUNCTION_8BIT)) >> 4);
    NutSleep(4);
    LcdWriteNibble((_BV(LCD_FUNCTION) | _BV(LCD_FUNCTION_8BIT)) >> 4);
    NutSleep(2);

#ifdef LCD_IF_4BIT
    /* We now switch to 4-bit mode */
    LcdWriteNibble(_BV(LCD_FUNCTION) >> 4);
    NutSleep(2);

    // TODO: Add support for large font in single line displays
    /* Set number of lines and font. Can't be changed later. */
#if (LCD_ROWS == 2) || (LCD_ROWS==4)
    LcdWriteNibble((_BV(LCD_FUNCTION) | _BV(LCD_FUNCTION_2LINES)) >> 4);
    LcdWriteNibble(_BV(LCD_FUNCTION) | _BV(LCD_FUNCTION_2LINES));
#else
    LcdWriteNibble(_BV(LCD_FUNCTION) >> 4);
    LcdWriteNibble(_BV(LCD_FUNCTION) );
#endif
#else /* LCD_IF_4BIT */
    LcdWriteCmd(_BV(LCD_FUNCTION) | _BV(LCD_FUNCTION_8BIT));
#endif /* LCD_IF_4BIT */
    NutSleep(2);

    /* Switch display and cursor off. */
    LcdWriteNibble(_BV(LCD_ON_CTRL) >> 4);
    LcdWriteNibble(_BV(LCD_ON_CTRL));
    NutSleep(2);

    /* Clear display. */
    LcdClear();

    /* Set entry mode. */
    LcdWriteCmd(_BV(LCD_ENTRY_MODE) | _BV(LCD_ENTRY_INC));
    /* Switch display on. */
    LcdWriteCmd(_BV(LCD_ON_CTRL) | _BV(LCD_ON_DISPLAY));
    /* Move cursor home. */
    LcdCursorHome();
    /* Set data address to zero. */
    LcdWriteCmd(_BV(LCD_DDRAM));

    return 0;
}

/*!
 * \brief Terminal device control block structure.
 */
TERMDCB dcb_term = {
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
NUTDEVICE devLcd = {
    0,                          /*!< Pointer to next device. */
    {'c', 'h', 'a', 'r', 'l', 'c', 'd', 0, 0},  /*!< Unique device name. */
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
