#ifndef _DEV_UART_H
#define _DEV_UART_H

/*
 * Copyright (C) 2001-2004 by egnite Software GmbH. All rights reserved.
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
 * Revision 1.5  2008/08/11 06:59:59  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.4  2005/06/26 12:40:59  chaac
 * Added support for raw mode to AHDLC driver.
 *
 * Revision 1.3  2004/11/12 11:14:32  freckle
 * added UART_GETBLOCKREAD & UART_SETBLOCKREAD defines
 *
 * Revision 1.2  2004/05/24 20:19:08  drsung
 * Added function UartAvrSize to return number of chars in input buffer.
 *
 * Revision 1.1  2004/03/16 16:48:28  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 * Revision 1.2  2003/12/15 19:29:18  haraldkipp
 * USART driver support added
 *
 * Revision 1.1.1.1  2003/05/09 14:41:23  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.11  2003/05/06 17:58:28  harald
 * Handshakes added
 *
 * Revision 1.10  2003/03/31 14:53:25  harald
 * Prepare release 3.1
 *
 * Revision 1.9  2003/02/04 18:00:54  harald
 * Version 3 released
 *
 * Revision 1.8  2003/01/14 16:35:24  harald
 * Definitions moved
 *
 * Revision 1.7  2002/11/02 15:17:01  harald
 * Library dependencies moved to compiler.h
 *
 * Revision 1.6  2002/06/26 17:29:30  harald
 * First pre-release with 2.4 stack
 *
 */

#include <sys/device.h>

/*!
 * \file dev/uart.h
 * \brief UART I/O function prototypes.
 */

/*!
 * \addtogroup xgUARTIOCTL
 *
 * \brief UART _ioctl() commands.
 *
 * These commands are used to control and retrieve hardware specific
 * configurations. The definitions are kept independent from the 
 * underlying hardware, but not all commands may be fully implemented 
 * in each UART driver.
 *
 * The _ioctl() function expects three parameters:
 * - A device descriptor.
 * - A command code, any of the UART_... commands listed below.
 * - A pointer to a configuration parameter, in most cases an unsigned long.
 */
/*@{*/

/*! \brief UART _ioctl() command code to set the line speed.
 *
 * The configuration parameter specifies the input and output bit rate 
 * per second.
 */
#define UART_SETSPEED           0x0101

/*! \brief UART _ioctl() command code to query the line speed.
 *
 * The configuration parameter is set to the input and output bit rate 
 * per second.
 */
#define UART_GETSPEED           0x0102

/*! \brief UART _ioctl() command code to set the number of data bits.
 *
 * The configuration parameter specifies the number of data bits, 5, 6, 
 * 7, 8 or 9.
 */
#define UART_SETDATABITS        0x0103

/*! \brief UART _ioctl() command code to query the number of data bits.
 *
 * The configuration parameter is set to the number of data bits, 5, 6, 
 * 7, 8 or 9.
 */
#define UART_GETDATABITS        0x0104

/*! \brief UART _ioctl() command code to set the parity mode.
 *
 * The configuration parameter specifies the type of the parity bit, 
 * 0 (none), 1 (odd) or 2 (even).
 */
#define UART_SETPARITY          0x0105

/*! \brief UART _ioctl() command code to query the parity mode.
 *
 * The configuration parameter is set to the type of the parity bit, 
 * 0 (none), 1 (odd) or 2 (even).
 */
#define UART_GETPARITY          0x0106

/*! \brief UART _ioctl() command code to set the number of stop bits.
 *
 * The configuration parameter specifies the number of stop bits, 1 or 2.
 */
#define UART_SETSTOPBITS        0x0107

/*! \brief UART _ioctl() command code to query the number of stop bits.
 *
 * The configuration parameter is set to the number of stop bits, 1 or 2.
 */
#define UART_GETSTOPBITS        0x0108

/*! \brief UART _ioctl() command code to set the status.
 *
 * The configuration parameter specifies the status to set.
 */
#define UART_SETSTATUS          0x0109

/*! \brief UART _ioctl() command code to query the status.
 *
 * The configuration parameter is set to the current status.
 */
#define UART_GETSTATUS          0x010a

/*! \brief UART _ioctl() command code to set the read timeout.
 *
 * The configuration parameter specifies the read timeout in 
 * milliseconds.
 */
#define UART_SETREADTIMEOUT     0x010b

/*! \brief UART _ioctl() command code to query the read timeout.
 *
 * The configuration parameter is set to the read timeout in 
 * milliseconds.
 */
#define UART_GETREADTIMEOUT     0x010c

/*! \brief UART _ioctl() command code to set the write timeout.
 *
 * The configuration parameter specifies the write timeout in 
 * milliseconds.
 */
#define UART_SETWRITETIMEOUT    0x010d

/*! \brief UART _ioctl() command code to query the write timeout.
 *
 * The configuration parameter is set to the write timeout in 
 * milliseconds.
 */
#define UART_GETWRITETIMEOUT    0x010e

/*! \brief UART _ioctl() command code to set the local echo mode.
 *
 * The configuration parameter specifies the local echo mode, 
 * 0 (off) or 1 (on).
 */
#define UART_SETLOCALECHO       0x010f

/*! \brief UART _ioctl() command code to query the local echo mode.
 *
 * The configuration parameter is set to the local echo mode, 
 * 0 (off) or 1 (on).
 */
#define UART_GETLOCALECHO       0x0110

/*! \brief UART _ioctl() command code to set the flow control mode.
 *
 * The configuration parameter specifies the flow control mode.
 */
#define UART_SETFLOWCONTROL     0x0111

/*! \brief UART _ioctl() command code to query the flow control mode.
 *
 * The configuration parameter is set to the flow control mode.
 */
#define UART_GETFLOWCONTROL     0x0112

/*! \brief UART _ioctl() command code to set the cooking mode.
 *
 * The configuration parameter specifies the character cooking mode, 
 * 0 (raw) or 1 (EOL translation).
 */
#define UART_SETCOOKEDMODE      0x0113

/*! \brief UART _ioctl() command code to query the cooking mode.
 *
 * The configuration parameter is set to the character cooking mode, 
 * 0 (raw) or 1 (EOL translation).
 */
#define UART_GETCOOKEDMODE      0x0114

/*! \brief UART _ioctl() command code to set the buffering mode.
 *
 * The configuration parameter specifies the buffering mode.
 */
#define UART_SETBUFFERMODE      0x0115

/*! \brief UART _ioctl() command code to query the buffering mode.
 *
 * The configuration parameter is set to the buffering mode.
 */
#define UART_GETBUFFERMODE      0x0116

/*! \brief UART _ioctl() command code to set the network interface mode.
 *
 * The configuration parameter specifies the network interface mode.
 */
#define HDLC_SETIFNET           0x0117

/*! \brief UART _ioctl() command code to query the network interface mode.
 *
 * The configuration parameter is set to the network interface mode.
 */
#define HDLC_GETIFNET           0x0118

/*! \brief UART _ioctl() command code to set the clock mode.
 *
 * The configuration parameter specifies the clock mode.
 */
#define UART_SETCLOCKMODE       0x0119

/*! \brief UART _ioctl() command code to query the clock mode.
 *
 * The configuration parameter is set to the clock mode.
 */
#define UART_GETCLOCKMODE       0x011a

/*! \brief UART _ioctl() command code to set the transmit buffer size.
 *
 * The configuration parameter specifies the number of bytes.
 */
#define UART_SETTXBUFSIZ        0x011b

/*! \brief UART _ioctl() command code to query the transmit buffer size.
 *
 * The configuration parameter specifies the number of bytes.
 */
#define UART_GETTXBUFSIZ        0x011c

/*! \brief UART _ioctl() command code to set the receive buffer size.
 *
 * The configuration parameter specifies the number of bytes.
 */
#define UART_SETRXBUFSIZ        0x011d

/*! \brief UART _ioctl() command code to query the receive buffer size.
 *
 * The configuration parameter specifies the number of bytes.
 */
#define UART_GETRXBUFSIZ        0x011e

/*! \brief UART _ioctl() command code to set the transmit buffer low watermark.
 *
 * The configuration parameter specifies the number of bytes.
 */
#define UART_SETTXBUFLWMARK     0x0120

/*! \brief UART _ioctl() command code to query the transmit buffer low watermark.
 *
 * The configuration parameter specifies the number of bytes.
 */
#define UART_GETTXBUFLWMARK     0x0121

/*! \brief UART _ioctl() command code to set the transmit buffer high watermark.
 *
 * The configuration parameter specifies the number of bytes.
 */
#define UART_SETTXBUFHWMARK     0x0122

/*! \brief UART _ioctl() command code to query the transmit buffer high watermark.
 *
 * The configuration parameter specifies the number of bytes.
 */
#define UART_GETTXBUFHWMARK     0x0123

/*! \brief UART _ioctl() command code to set the receive buffer low watermark.
 *
 * The configuration parameter specifies the number of bytes.
 */
#define UART_SETRXBUFLWMARK     0x0124

/*! \brief UART _ioctl() command code to query the receive buffer low watermark.
 *
 * The configuration parameter specifies the number of bytes.
 */
#define UART_GETRXBUFLWMARK     0x0125

/*! \brief UART _ioctl() command code to set the receive buffer high watermark.
 *
 * The configuration parameter specifies the number of bytes.
 */
#define UART_SETRXBUFHWMARK     0x0126

/*! \brief UART _ioctl() command code to query the receive buffer high watermark.
 *
 * The configuration parameter specifies the number of bytes.
 */
#define UART_GETRXBUFHWMARK     0x0127

/*! \brief UART _ioctl() command code to set the block read mode
*
* The configuration parameter specifies the block read mode.
*/
#define UART_SETBLOCKREAD       0x0128

/*! \brief UART _ioctl() command code to query the receive buffer high watermark.
*
* The configuration parameter specifies the block read mode.
*/
#define UART_GETBLOCKREAD       0x0129

/*! \brief UART _ioctl() command code to set physical device to the raw mode.
 *
 * The configuration parameter specifies the raw mode for device. In raw mode
 * data encapsulation is not allowed to be done. This allows other processing to
 * be done on physical device.
 */
#define UART_SETRAWMODE         0x012a

/*! \brief UART _ioctl() command code to query the raw mode.
 *
 * The configuration parameter specified the raw mode.
 */
#define UART_GETRAWMODE         0x012b

/*! \brief AHDLC _ioctl() command code to set the ACCM (Control Character Mask).
 *
 * During the LCP stage of PPP negotiation both peers inform each other which
 * of the control characters (0-31) will not require escaping when being
 * transmitted.  This allows the PPP layer to tell the HDLC layer about this
 * so that data may be transmitted quicker (no escapes).
 */
#define HDLC_SETTXACCM          0x012c

/*! \brief AHDLC _ioctl() command code to get the ACCM (Control Character Mask).
 *
 * Just in case someone ever wants to see what it currently is.
 */
#define HDLC_GETTXACCM          0x012d

/*! \brief UART _ioctl() command code to set physical device to half duplex mode.
 *
 * The configuration parameter specifies the halfduplex mode for device. In raw mode
 */
#define UART_SETHDPXMODE        0x012c

/*! \brief UART _ioctl() command code to query the halfduplex mode.
 *
 */
#define UART_GETHDPXMODE        0x012d

/*!
 * \addtogroup xgUARTStatus
 * \brief UART device status flags,
 *
 * A combination of these status flags is used by the _ioctl() commands
 * \ref UART_SETSTATUS and \ref UART_GETSTATUS. 
 */
/*@{*/

/*! \brief Framing error.
 *
 * \ref UART_SETSTATUS will clear this error.
 */
#define UART_FRAMINGERROR   0x00000001UL

/*! \brief Overrun error. 
 *
 * \ref UART_SETSTATUS will clear this error.
 */
#define UART_OVERRUNERROR   0x00000002UL

/*! \brief Parity error. 
 *
 * \ref UART_SETSTATUS will clear this error.
 */
#define UART_PARITYERROR    0x00000004UL

/*! \brief UART errors.
 *
 * \ref UART_SETSTATUS will clear all errors.
 */
#define UART_ERRORS         (UART_FRAMINGERROR | UART_OVERRUNERROR | UART_PARITYERROR)

/*! \brief Receiver buffer empty. 
 */
#define UART_RXBUFFEREMPTY  0x00000040UL

/*! \brief Transmitter buffer empty.
 *
 * \ref UART_SETSTATUS will immediately clear the buffer. It will not 
 * wait until the remaining characters have been transmitted.
 */
#define UART_TXBUFFEREMPTY  0x00000080UL

/*! \brief RTS handshake output enabled. 
 */
#define UART_RTSENABLED     0x00000100UL

/*! \brief RTS handshake output disabled. 
 */
#define UART_RTSDISABLED    0x00000200UL

/*! \brief CTS handshake input enabled. 
 */
#define UART_CTSENABLED     0x00000400UL

/*! \brief CTS handshake input disabled. 
 */
#define UART_CTSDISABLED    0x00000800UL

/*! \brief DTR handshake output enabled. 
 */
#define UART_DTRENABLED     0x00001000UL

/*! \brief DTR handshake output disabled. 
 */
#define UART_DTRDISABLED    0x00002000UL

/*! \brief Receiver enabled. 
 */
#define UART_RXENABLED      0x00010000UL

/*! \brief Receiver enabled. 
 */
#define UART_RXDISABLED     0x00020000UL

/*! \brief Transmitter enabled. 
 */
#define UART_TXENABLED      0x00040000UL

/*! \brief Transmitter enabled. 
 */
#define UART_TXDISABLED     0x00080000UL

/*! \brief Receive address frames only.
 *
 * Used in multidrop communication. May only work if 9 databits have 
 * been configured.
 */
#define UART_RXADDRFRAME    0x00100000UL

/*! \brief Receive all frames.
 *
 * Used in multidrop communication.
 */
#define UART_RXNORMFRAME    0x00200000UL

/*! \brief Transmit as address frame.
 *
 * Used in multidrop communication. May only work if 9 databits have 
 * been configured.
 */
#define UART_TXADDRFRAME    0x00400000UL

/*! \brief Transmit as normal frame.
 *
 * Used in multidrop communication.
 */
#define UART_TXNORMFRAME    0x00800000UL


/*@}*/

/*!
 * \addtogroup xgUARTHS
 * \brief UART handshake modes.
 *
 * Any of these values may be used by the _ioctl() commands
 * \ref UART_SETFLOWCONTROL and \ref UART_GETFLOWCONTROL.
 */
/*@{*/

/*! \brief RTS / CTS hardware handshake.
 *
 * Nut/OS uses DTE definitions, where RTS is output and CTS is input.
 */
#define UART_HS_RTSCTS      0x0003

/*! \brief Full modem hardware handshake.
 *
 * Not supported yet by the standard drivers.
 */
#define UART_HS_MODEM       0x001F

/*! \brief XON / XOFF software handshake.
 *
 * It is recommended to set a proper read timeout with software handshake.
 * In this case a timeout may occur, if the communication peer lost our 
 * last XON character. The application may then use ioctl() to disable the 
 * receiver and do the read again. This will send out another XON.
 */
#define UART_HS_SOFT        0x0020

/*! \brief Half duplex mode.
 */
#define UART_HS_HALFDUPLEX  0x0400

/*@}*/


/*!
 * \addtogroup xgUARTClock
 * \brief UART device clock modes.
 *
 * Any of these values may be used by the _ioctl() commands
 * \ref UART_SETCLOCKMODE and \ref UART_GETCLOCKMODE. Most drivers
 * require to set the bit rate after modifying the clock mode. In order
 * to avoid unknown clock output frequencies in master mode, set the
 * clock mode to \ref UART_SYNCSLAVE first, than use \ref UART_SETSPEED 
 * to select the bit rate and finally switch to \ref UART_SYNCMASTER or 
 * \ref UART_NSYNCMASTER.
 */
/*@{*/

#define UART_SYNC           0x01
#define UART_MASTER         0x02
#define UART_NCLOCK         0x04
#define UART_HIGHSPEED      0x20

/*! \brief Normal asynchronous mode.
 */
#define UART_ASYNC          0x00

/*! \brief Synchronous slave mode.
 *
 * Transmit data changes on rising edge and receive data is sampled on 
 * the falling edge of the clock input.
 */
#define UART_SYNCSLAVE     UART_SYNC

/*! \brief Synchronous master mode.
 *
 * Transmit data changes on rising edge and receive data is sampled on 
 * the falling edge of the clock output.
 */
#define UART_SYNCMASTER    (UART_SYNC | UART_MASTER)

/*! \brief Synchronous slave mode, clock negated.
 *
 * Similar to \ref UART_SYNCSLAVE, but transmit data changes on falling 
 * edge and receive data is sampled on the rising edge of the clock input.
 */
#define UART_NSYNCSLAVE    (UART_SYNC | UART_NCLOCK)

/*! \brief Synchronous master mode, clock negated
 *
 * Similar to \ref UART_SYNCMASTER, but transmit data changes on falling 
 * edge and receive data is sampled on the rising edge of the clock output.
 */
#define UART_NSYNCMASTER   (UART_SYNC | UART_NCLOCK | UART_MASTER)

/*! \brief Asynchronous high speed mode.
 *
 * More deviation sensitive than normal mode, but supports higher speed.
 */
#define UART_ASYNC_HS      UART_HIGHSPEED

/*@}*/

/*@}*/

#endif
