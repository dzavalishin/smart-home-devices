#ifndef _DEV_TERM_H_
#define _DEV_TERM_H_

/*
 * Copyright (C) 2003-2005 by egnite Software GmbH. All rights reserved.
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
 */

#include <sys/device.h>

/*!
 * \file dev/term.h
 * \brief Terminal device definitions.
 */

/*!
 * \addtogroup xgTerminal
 */
/*@{*/

/*
 * I/O control commands.
 */
#define LCD_CMDBYTE         0x0401
#define LCD_CMDWORD16       0x0402
#define LCD_CMDWORD32       0x0403
#define LCD_DATABYTE        0x0405
#define LCD_DATAWORD16      0x0406
#define LCD_DATAWORD32      0x0407
#define LCD_SETCOOKEDMODE   0x0413  /*!< \brief Set raw mode. */ 
#define LCD_GETCOOKEDMODE   0x0414  /*!< \brief Query raw mode. */ 
#define LCD_SET_AUTOLF      0x0415
#define LCD_GET_AUTOLF      0x0416

#ifndef TIOCGWINSZ
#define TIOCGWINSZ          0x0501  /*!< Gets the window size */
#endif
#ifndef TIOCSWINSZ
#define TIOCSWINSZ          0x0502  /*!< Sets the window size */
#endif

#define LCD_MF_CURSORON     0x00000001UL    /*!< \brief Cursor on flag. */
#define LCD_MF_COOKEDMODE   0x00020000UL    /*!< \brief Control character interpretation on flag. */
#define LCD_MF_AUTOLF       0x00040000UL    /*!< \brief Control automatic break into next line on line end */
#define LCD_MF_AUTOSCROLL   0x00080000UL    /*!< \brief Control automatic scrolling at end of display */
#define LCD_MF_INVERTED     0x10000000UL    /*!< \breif display inverted characters on grafic displays */

#ifndef ESC_CHAR
#define ESC_CHAR        "\x1B"
#endif

#define ESC_POS         ESC_CHAR "Y"
#define ESC_UP          ESC_CHAR "A"
#define ESC_DOWN        ESC_CHAR "B"
#define ESC_RIGHT       ESC_CHAR "C"
#define ESC_LEFT        ESC_CHAR "D"
#define ESC_CLRHOME     ESC_CHAR "E"

#define ESC_HOME        ESC_CHAR "H"
#define ESC_CLREND      ESC_CHAR "J"
#define ESC_CLREOL      ESC_CHAR "K"
#define ESC_CLRSTART    ESC_CHAR "d"
#define ESC_CLRSOL      ESC_CHAR "o"

#define ESC_INSCHAR     ESC_CHAR "@"
#define ESC_INSLINE     ESC_CHAR "L"
#define ESC_DELCHAR     ESC_CHAR "P"
#define ESC_DELLINE     ESC_CHAR "M"
#define ESC_RLF         ESC_CHAR "I"

#define ESC_CURSORON    ESC_CHAR "e"
#define ESC_CURSOROFF   ESC_CHAR "f"
#define ESC_INVERTON    ESC_CHAR "i"
#define ESC_INVERTOFF   ESC_CHAR "n"
#define ESC_SPECIALSET  ESC_CHAR "F"
#define ESC_DEFAULTSET  ESC_CHAR "G"
#define ESC_ACTIVE      ESC_CHAR "R"
#define ESC_SLEEP       ESC_CHAR "S"

/*! \brief Deprecated, use ESC_CLRHOME.
 *
 * Although wrong, we keep it for now in order not to break existing code.
 */
#define ESC_CLR         ESC_CLRHOME

/*
 * winsize structure
 */
typedef struct _WINSIZE WINSIZE;

struct _WINSIZE {
    uint16_t ws_row;
    uint16_t ws_col;
    uint16_t ws_xpixel;
    uint16_t ws_ypixel;
};

/*!
 * Terminal device control block type.
 */
typedef struct _TERMDCB TERMDCB;

/*!
 * \struct _TERMDCB term.h dev/term.h
 * \brief Terminal device control block structure.
 */
struct _TERMDCB {

    /*! \brief Initialize display subsystem.
     */
    int (*dss_init)(NUTDEVICE *);

    /*! \brief Write display character.
     */
    void (*dss_write)(uint8_t);

    /*! \brief Write display command.
     */
    void (*dss_command)(uint8_t, uint8_t);

    /*! \brief Clear display.
     */
    void (*dss_clear)(void);

    /*! \brief Set display cursor.
     */
    void (*dss_set_cursor)(uint8_t);

    /*! \brief Set display cursor home.
     */
    void (*dss_cursor_home)(void);

    /*! \brief Move display cursor left.
     */
    void (*dss_cursor_left)(void);

    /*! \brief Move display cursor right.
     */
    void (*dss_cursor_right)(void);

    /*! \brief Switch cursor on/off.
     */
    void (*dss_cursor_mode)(uint8_t);

    /*! \brief Mode flags.
     */
    uint32_t dcb_modeflags;

    /*! \brief Control sequence.
     */
    uint8_t dcb_ctlseq;

    /*! \brief Number of rows.
     * Specifies the display height.
     */
    uint8_t dcb_nrows;

    /*! \brief Total number of columns per row.
     * Used to calculate display memory addresses.
     */
    uint8_t dcb_ncols;

    /*! \brief Number of visible columns.
     * Specifies the display width being updated.
     */
    uint8_t dcb_vcols;

    /*! \brief Cursor row.
     */
    uint8_t dcb_row;

    /*! \brief Cursor column.
     */
    uint8_t dcb_col;

    /*! \brief Display shadow memory.
     */
    uint8_t *dcb_smem;

    /*! \brief Display shadow memory.
     */
    uint8_t *dcb_sptr;
};

/*@}*/

extern int TermInit(NUTDEVICE * dev);
extern int TermIOCtl(NUTDEVICE * dev, int req, void *conf);
extern int TermWrite(NUTFILE * fp, CONST void *buffer, int len);
#ifdef __HARVARD_ARCH__
extern int TermWrite_P(NUTFILE * fp, PGM_P buffer, int len);
#endif
extern NUTFILE *TermOpen(NUTDEVICE * dev, CONST char *name, int mode, int acc);
extern int TermClose(NUTFILE * fp);

#endif
