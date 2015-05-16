#ifndef _DEV_UARTSPI_H_
#define _DEV_UARTSPI_H_

/*
 * Copyright (C) 2001-2003 by egnite Software GmbH. All rights reserved.
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
 * \file dev/uartspi.h
 * \brief SPI UART definitions.
 *
 * Removed.
 */

#ifdef __cplusplus
extern "C" {
#endif

/*
 * \addtogroup xgUartSpi
 */
/*@{*/

/*
 * UART device control block type.
 */
typedef struct _UARTSDCB UARTSDCB;

/*
 * \struct _UARTSDCB uartspi.h dev/uartspi.h
 * \brief UART device control block structure.
 */
struct _UARTSDCB {
    /* \brief Baudrate selector.
     *
     * See devices datasheet for further details.
     */
    uint8_t dcb_baudSelect;

    /* \brief Mode.
     *  - xxxx xxx0  8 bit
     *  - xxxx xxx1  7 bit
     *  - xxxx x0xx  No parity bit
     *  - xxxx x10x  Even parity
     *  - xxxx x11x  Odd parity
     */
    uint8_t dcb_mode;

    /* \brief Read timeout.
     */
    uint32_t dcb_rtimeout;

    /* \brief Write timeout.
     */
    uint32_t dcb_wtimeout;

    /* \brief Configuration flag.
     * Set if device has been configured.
     */
    volatile uint8_t dcb_configured;

    /* \brief Configuration flag.
     * Set if device has been configured.
     */
    uint32_t dcb_crystal;

    /* \brief Device firmware version.
     */
    uint32_t dcb_version;

    /* \brief Queue of threads waiting for output buffer empty.
     *
     * Threads are added to this queue when calling UartSpiFlush(). 
     */
    HANDLE dcb_tx_rdy;

    /* \brief Queue of threads waiting for a character in the input buffer.
     *
     * Threads are added to this queue when calling UartSpiInput(). 
     */
    HANDLE dcb_rx_rdy;
};

/*@}*/

/*
 * Available drivers.
 */
extern NUTDEVICE devUarts[];

extern int UartSpiInit(NUTDEVICE *dev);
extern int UartSpiIOCtl(NUTDEVICE *dev, int req, void *conf);

#ifdef __cplusplus
}
#endif

#endif
