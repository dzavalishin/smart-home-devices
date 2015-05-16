#ifndef _DEV_USARTAT91_H_
#define _DEV_USARTAT91_H_

/*
 * Copyright (C) 2001-2005 by egnite Software GmbH. All rights reserved.
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

/*
 * $Log$
 * Revision 1.3  2009/09/20 13:24:58  ulrichprinz
 * Added limited USART support for DBGU.
 *
 * Revision 1.2  2008/04/18 13:24:58  haraldkipp
 * Added Szemzo Andras' RS485 patch.
 *
 * Revision 1.1  2005/11/20 14:40:28  haraldkipp
 * Added interrupt driven UART driver for AT91.
 *
 */

#include <sys/device.h>
#include <dev/uart.h>
#include <dev/usart.h>

#define UART_MF_RTSSENSE    0x00000001UL    /*!< DCE input, low on port bit is +12V, which means ON. */
#define UART_MF_CTSCONTROL  0x00000002UL    /*!< DCE output. */
#define UART_MF_DTRSENSE    0x00000004UL    /*!< DCE input. */
#define UART_MF_DSRCONTROL  0x00000008UL    /*!< DCE output. */
#define UART_MF_DCDCONTROL  0x00000010UL    /*!< DCE output. */

#define UART_MF_RTSCONTROL  0x00000020UL    /*!< DTE output. */
#define UART_MF_CTSSENSE    0x00000040UL    /*!< DTE input. */
#define UART_MF_DTRCONTROL  0x00000080UL    /*!< DTE output. */
#define UART_MF_DSRSENSE    0x00000100UL    /*!< DTE input. */
#define UART_MF_DCDSENSE    0x00000200UL    /*!< DTE input. */

#define UART_MF_SENSEMASK   0x0345	    	/*!< Handshake sense mask. */
#define UART_MF_CONTROLMASK 0x00BC	    	/*!< Handshake control mask. */

#define UART_MF_XONXOFF     0x00000400UL    /*!< Software handshake. */
#define UART_MF_HDPXMODE    0x00000800UL    /*!< Halfduplex Mode. */

#define UART_MF_LOCALECHO   0x00010000UL    /*!< Should be used in stream, not device. */
#define UART_MF_COOKEDMODE  0x00020000UL    /*!< Should be used in stream, not device. */

#define UART_MF_NOBUFFER    0x00100000UL    /*!< No buffering. */
#define UART_MF_LINEBUFFER  0x00200000UL    /*!< Line buffered. */
#define UART_MF_BUFFERMASK  0x00300000UL    /*!< Masks buffering mode flags. */

#define UART_MF_RAWMODE     0x00400000UL    /*!< Send data as raw, disables data encapsulation for device. */


/*!
 * \file dev/usartat91.h
 * \brief Synchronous/asynchronous serial device definitions.
 */

extern NUTDEVICE devUsartAt910;
extern NUTDEVICE devUsartAt911;
extern NUTDEVICE devDbguAt91;

#endif
