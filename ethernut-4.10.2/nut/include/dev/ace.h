#ifndef _DEV_ACE_H
#define _DEV_ACE_H

/*
 * Copyright (C) 2001-2003 by Cyber Integration, LLC. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY CYBER INTEGRATION, LLC AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL CYBER
 * INTEGRATION, LLC OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *
 */

/*
 * $Log$
 * Revision 1.4  2008/08/11 06:59:59  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.3  2007/08/29 07:43:53  haraldkipp
 * Documentation updated and corrected.
 *
 * Revision 1.2  2006/05/25 09:09:57  haraldkipp
 * API documentation updated and corrected.
 *
 * Revision 1.1  2005/11/24 11:24:06  haraldkipp
 * Initial check-in.
 * Many thanks to William Basser for this code and also to Przemyslaw Rudy
 * for several enhancements.
 *
 */

#include <sys/device.h>

/*!
 * \file dev/ace.h
 * \brief ACE I/O function prototypes.
 */

/*!
 * \addtogroup xgAceDriver
 *
 * \brief ACE _ioctl() commands.
 *
 * These commands are used to control and retrieve hardware specific
 * configurations. The definitions are kept independent from the 
 * underlying hardware, but not all commands may be fully implemented 
 * in each ACE driver.
 *
 * The _ioctl() function expects three parameters:
 * - A device descriptor.
 * - A command code, any of the ACE_... commands listed below.
 * - A pointer to a configuration parameter, in most cases an unsigned long.
 */
/*@{*/

/*! \brief ACE _ioctl() command code to set the line speed.
 *
 * The configuration parameter specifies the input and output bit rate 
 * per second.
 */
#define ACE_SETSPEED           0x0101

/*! \brief ACE _ioctl() command code to query the line speed.
 *
 * The configuration parameter is set to the input and output bit rate 
 * per second.
 */
#define ACE_GETSPEED           0x0102

/*! \brief ACE _ioctl() command code to set the number of data bits.
 *
 * The configuration parameter specifies the number of data bits, 5, 6, 
 * 7, 8 or 9.
 */
#define ACE_SETDATABITS        0x0103

/*! \brief ACE _ioctl() command code to query the number of data bits.
 *
 * The configuration parameter is set to the number of data bits, 5, 6, 
 * 7, 8 or 9.
 */
#define ACE_GETDATABITS        0x0104

/*! \brief ACE _ioctl() command code to set the parity mode.
 *
 * The configuration parameter specifies the type of the parity bit, 
 * 0 (none), 1 (odd), 2 (even), 3 (mark) or 4 (space).
 */
#define ACE_SETPARITY          0x0105

/*! \brief ACE _ioctl() command code to query the parity mode.
 *
 * The configuration parameter is set to the type of the parity bit, 
 * 0 (none), 1 (odd), 2 (even), 3 (mark) or 4 (space).
 */
#define ACE_GETPARITY          0x0106

/*! \brief ACE _ioctl() command code to set the number of stop bits.
 *
 * The configuration parameter specifies the number of stop bits, 1 or 2.
 */
#define ACE_SETSTOPBITS        0x0107

/*! \brief ACE _ioctl() command code to query the number of stop bits.
 *
 * The configuration parameter is set to the number of stop bits, 1 or 2.
 */
#define ACE_GETSTOPBITS        0x0108

/*! \brief ACE _ioctl() command code to set the status.
 *
 * The configuration parameter specifies the status to set.
 */
#define ACE_SETSTATUS          0x0109

/*! \brief ACE _ioctl() command code to query the status.
 *
 * The configuration parameter is set to the current status.
 */
#define ACE_GETSTATUS          0x010a

/*! \brief ACE _ioctl() command code to set the read timeout.
 *
 * The configuration parameter specifies the read timeout in 
 * milliseconds.
 */
#define ACE_SETREADTIMEOUT     0x010b

/*! \brief ACE _ioctl() command code to query the read timeout.
 *
 * The configuration parameter is set to the read timeout in 
 * milliseconds.
 */
#define ACE_GETREADTIMEOUT     0x010c

/*! \brief ACE _ioctl() command code to set the write timeout.
 *
 * The configuration parameter specifies the write timeout in 
 * milliseconds.
 */
#define ACE_SETWRITETIMEOUT    0x010d

/*! \brief ACE _ioctl() command code to query the write timeout.
 *
 * The configuration parameter is set to the write timeout in 
 * milliseconds.
 */
#define ACE_GETWRITETIMEOUT    0x010e

/*! \brief ACE _ioctl() command code to set the local echo mode.
 *
 * The configuration parameter specifies the local echo mode, 
 * 0 (off) or 1 (on).
 */
#define ACE_SETLOCALECHO       0x010f

/*! \brief ACE _ioctl() command code to query the local echo mode.
 *
 * The configuration parameter is set to the local echo mode, 
 * 0 (off) or 1 (on).
 */
#define ACE_GETLOCALECHO       0x0110

/*! \brief ACE _ioctl() command code to set the flow control mode.
 *
 * The configuration parameter specifies the flow control mode.
 */
#define ACE_SETFLOWCONTROL     0x0111

/*! \brief ACE _ioctl() command code to query the flow control mode.
 *
 * The configuration parameter is set to the flow control mode.
 */
#define ACE_GETFLOWCONTROL     0x0112

/*! \brief ACE _ioctl() command code to set the cooking mode.
 *
 * The configuration parameter specifies the character cooking mode, 
 * 0 (raw) or 1 (EOL translation).
 */
#define ACE_SETCOOKEDMODE      0x0113

/*! \brief ACE _ioctl() command code to query the cooking mode.
 *
 * The configuration parameter is set to the character cooking mode, 
 * 0 (raw) or 1 (EOL translation).
 */
#define ACE_GETCOOKEDMODE      0x0114

/*! \brief ACE _ioctl() command code to set the buffering mode.
 *
 * The configuration parameter specifies the buffering mode.
 */
#define ACE_SETBUFFERMODE      0x0115

/*! \brief ACE _ioctl() command code to query the buffering mode.
 *
 * The configuration parameter is set to the buffering mode.
 */
#define ACE_GETBUFFERMODE      0x0116

/*! \brief ACE _ioctl() command code to set the transmit buffer size.
 *
 * The configuration parameter specifies the number of bytes.
 */
#define ACE_SETTXBUFSIZ        0x011b

/*! \brief ACE _ioctl() command code to query the transmit buffer size.
 *
 * The configuration parameter specifies the number of bytes.
 */
#define ACE_GETTXBUFSIZ        0x011c

/*! \brief ACE _ioctl() command code to set the receive buffer size.
 *
 * The configuration parameter specifies the number of bytes.
 */
#define ACE_SETRXBUFSIZ        0x011d

/*! \brief ACE _ioctl() command code to query the receive buffer size.
 *
 * The configuration parameter specifies the number of bytes.
 */
#define ACE_GETRXBUFSIZ        0x011e

/*! \brief ACE _ioctl() command code to set the transmit buffer low watermark.
 *
 * The configuration parameter specifies the number of bytes.
 */
#define ACE_SETTXBUFLWMARK     0x0120

/*! \brief ACE _ioctl() command code to query the transmit buffer low watermark.
 *
 * The configuration parameter specifies the number of bytes.
 */
#define ACE_GETTXBUFLWMARK     0x0121

/*! \brief ACE _ioctl() command code to set the transmit buffer high watermark.
 *
 * The configuration parameter specifies the number of bytes.
 */
#define ACE_SETTXBUFHWMARK     0x0122

/*! \brief ACE _ioctl() command code to query the transmit buffer high watermark.
 *
 * The configuration parameter specifies the number of bytes.
 */
#define ACE_GETTXBUFHWMARK     0x0123

/*! \brief ACE _ioctl() command code to set the receive buffer low watermark.
 *
 * The configuration parameter specifies the number of bytes.
 */
#define ACE_SETRXBUFLWMARK     0x0124

/*! \brief ACE _ioctl() command code to query the receive buffer low watermark.
 *
 * The configuration parameter specifies the number of bytes.
 */
#define ACE_GETRXBUFLWMARK     0x0125

/*! \brief ACE _ioctl() command code to set the receive buffer high watermark.
 *
 * The configuration parameter specifies the number of bytes.
 */
#define ACE_SETRXBUFHWMARK     0x0126

/*! \brief ACE _ioctl() command code to query the receive buffer high watermark.
 *
 * The configuration parameter specifies the number of bytes.
 */
#define ACE_GETRXBUFHWMARK     0x0127

/*! \brief ACE _ioctl() command code to set the block read mode
*
* The configuration parameter specifies the block read mode
*/
#define ACE_SETBLOCKREAD		0x0128

/*! \brief ACE _ioctl() command code to query the receive buffer high watermark.
*
* The configuration parameter specifies the block read mode
*/
#define ACE_GETBLOCKREAD		0x0129

/*! \brief ACE _ioctl() command code to set the fifo mode and receive fifo trigger level.
*
* The configuration parameter specifies the receive fifo trigger level (x,1,4,8,14), x - disables fifo
*/
#define ACE_SETFIFO		0x012a

/*! \brief ACE _ioctl() command code to query the fifo mode and receive fifo trigger level.
*
* The configuration parameter specifies the receive fifo trigger level (x,1,4,8,14), x - fifo is disabled
*/
#define ACE_GETFIFO		0x012b

/*!
 * \addtogroup xgACEStatus
 * \brief ACE device status flags,
 *
 * A combination of these status flags is used by the _ioctl() commands
 * \ref ACE_SETSTATUS and \ref ACE_GETSTATUS. 
 */
/*@{*/

/*! \brief Framing error.
 *
 * \ref ACE_SETSTATUS will clear this error.
 */
#define ACE_FRAMINGERROR   0x00000001UL

/*! \brief Overrun error. 
 *
 * \ref ACE_SETSTATUS will clear this error.
 */
#define ACE_OVERRUNERROR   0x00000002UL

/*! \brief Parity error. 
 *
 * \ref ACE_SETSTATUS will clear this error.
 */
#define ACE_PARITYERROR    0x00000004UL

/*! \brief ACE errors.
 *
 * \ref ACE_SETSTATUS will clear all errors.
 */
#define ACE_ERRORS         (ACE_FRAMINGERROR | ACE_OVERRUNERROR | ACE_PARITYERROR)

/*! \brief Receiver buffer empty. 
 */
#define ACE_RXBUFFEREMPTY  0x00000040UL

/*! \brief Transmitter buffer empty.
 *
 * \ref ACE_SETSTATUS will immediately clear the buffer. It will not 
 * wait until the remaining characters have been transmitted.
 */
#define ACE_TXBUFFEREMPTY  0x00000080UL

/*! \brief RTS handshake output enabled. 
 */
#define ACE_RTSENABLED     0x00000100UL

/*! \brief RTS handshake output disabled. 
 */
#define ACE_RTSDISABLED    0x00000200UL

/*! \brief CTS handshake input enabled. 
 */
#define ACE_CTSENABLED     0x00000400UL

/*! \brief CTS handshake input disabled. 
 */
#define ACE_CTSDISABLED    0x00000800UL

/*! \brief DTR handshake output enabled. 
 */
#define ACE_DTRENABLED     0x00001000UL

/*! \brief DTR handshake output disabled. 
 */
#define ACE_DTRDISABLED    0x00002000UL

/*! \brief Receiver enabled. 
 */
#define ACE_RXENABLED      0x00010000UL

/*! \brief Receiver enabled. 
 */
#define ACE_RXDISABLED     0x00020000UL

/*! \brief Transmitter enabled. 
 */
#define ACE_TXENABLED      0x00040000UL

/*! \brief Transmitter enabled. 
 */
#define ACE_TXDISABLED     0x00080000UL

/*@}*/

/*!
 * \addtogroup xgACEHS
 * \brief ACE handshake modes.
 *
 * Any of these values may be used by the _ioctl() commands
 * \ref ACE_SETFLOWCONTROL and \ref ACE_GETFLOWCONTROL.
 */
/*@{*/

/*! \brief RTS / CTS hardware handshake.
 *
 * Nut/OS uses DTE definitions, where RTS is output and CTS is input.
 */
#define ACE_HS_RTSCTS      0x0003

/*! \brief Full modem hardware handshake.
 *
 * Not supported yet by the standard drivers.
 */
#define ACE_HS_MODEM       0x001F

/*! \brief XON / XOFF software handshake.
 *
 * It is recommended to set a proper read timeout with software handshake.
 * In this case a timeout may occur, if the communication peer lost our 
 * last XON character. The application may then use ioctl() to disable the 
 * receiver and do the read again. This will send out another XON.
 */
#define ACE_HS_SOFT        0x0020

/*@}*/

/*@}*/

__BEGIN_DECLS
/* */
extern int AceInit(NUTDEVICE * dev);
extern int AceIOCtl(NUTDEVICE * dev, int req, void *conf);
extern int AceInput(NUTDEVICE * dev);
extern int AceOutput(NUTDEVICE * dev);
extern int AceFlush(NUTDEVICE * dev);

extern int AceGetRaw(uint8_t * cp);
extern int AcePutRaw(uint8_t ch);

extern int AceRead(NUTFILE * fp, void *buffer, int size);
extern int AceWrite(NUTFILE * fp, CONST void *buffer, int len);
extern int AceWrite_P(NUTFILE * fp, PGM_P buffer, int len);
extern NUTFILE *AceOpen(NUTDEVICE * dev, CONST char *name, int mode, int acc);
extern long AceSize(NUTFILE * fp);
extern int AceClose(NUTFILE * fp);

__END_DECLS
#endif
