#ifndef _DEV_SPI_7SEG_H_
#define _DEV_SPI_7SEG_H_
/*
 * Copyright (C) 2009 by Ulrich Prinz (uprinz2@netscape.net)
 * Copyright (C) 2009 by Rittal GmbH & Co. KG. All rights reserved.
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
 */

/*!
 * \file include/dev/spi_7seg.h
 * \brief ntrx
 *
 * \verbatim
 * $Id: spi_ntrx.h$
 * \endverbatim
 */

#include <sys/device.h>
#include <dev/spibus.h>

extern NUTSPINODE nodeSpi7SEG;
extern NUTDEVICE devSpi7SEG;

#ifndef ESC_CHAR
#define ESC_CHAR        "\x1B"
#endif

/* 7 Segment Display commands for use in fprintf */
#define CLR_7SEG    ESC_CHAR "c"    /*< \brief Clear display and reset write position. */
#define BLS_7SEG    ESC_CHAR "b"    /*< \brief Set display blinking 0.5Hz. */
#define BLF_7SEG    ESC_CHAR "f"    /*< \brief Set display blinking 2Hz. */
#define NOR_7SEG    ESC_CHAR "n"    /*< \brief Set display to normal mode. */
#define HOME_7SEG   ESC_CHAR "h"    /*< \brief Set write position home. */
#define TEST_7SEG   ESC_CHAR "t"    /*< \brief Enable test mode of display (all segments on). */
#define INT_7SEG    ESC_CHAR "i"    /*< \brief Set intensity of display ( "0".."f"). */

/* 7 Segment Display dot extra functions */
#define DOT_7SEG_CLR  0     /*< \brief Clear the decimal dot of a digit. */
#define DOT_7SEG_SET  1     /*< \brief Set the decimal dot of a digit. */
#define DOT_7SEG_FLIP 2     /*< \brief Invert the decimal dot of a digit. */

__BEGIN_DECLS
/* Prototypes */
void Spi7segDot(NUTDEVICE * dev, uint8_t pos, uint8_t act);
extern int Spi7segCommand(NUTDEVICE * dev, uint8_t addr, CONST void *txbuf, void *rxbuf, int xlen);
/* Prototypes */
__BEGIN_DECLS

#endif
