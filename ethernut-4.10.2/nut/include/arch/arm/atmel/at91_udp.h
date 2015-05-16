/*
 * Copyright (C) 2007 by Ole Reinhardt, EmbeddedIT. ole.reinhardt@embedded-it.de
 * All rights reserved.
 *
 * This file is based on at91_udp.h 
 * (c) Uwe Bonnes <bon@elektron.ikp.physik.tu-darmstadt.de>
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
 * THIS SOFTWARE IS PROVIDED BY EMBEDDED-IT AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL EMBEDDED-IT
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * For additional information see http://www.ethernut.de/
 */

#ifndef _ARCH_ARM_AT91_UDP_H_
#define _ARCH_ARM_AT91_UDP_H_

/*!
 * \file arch/arm/at91_udp.h
 * \brief AT91 peripherals (USB device port).
 *
 * \verbatim
 * $Id: at91_udp.h 3450 2011-05-31 19:40:25Z mifi $
 * \endverbatim
 */

/*!
 * \addtogroup xgNutArchArmAt91Udp
 */
/*@{*/

/*! \name USB Device Port Frame Number Register */
/*@{*/
#define UDP_NUM_OFF         0x00000000  /*!< \brief Frame number register (offset). */
#define UDP_NUM (UDP_BASE + UDP_NUM_OFF) /*!< \brief Frame number register (address). */
#define UDP_FRM_NUM         0x000007FF  /*!< \brief Current frame number (mask). */
#define UDP_FRM_NUM_LSB             0   /*!< \brief Current frame number (LSB). */
#define UDP_FRM_ERR         _BV(16)     /*!< \brief Frame error. */
#define UDP_FRM_OK          _BV(17)     /*!< \brief Frame OK. */
/*@}*/

/*! \name USB Device Port Global State Register */
/*@{*/
#define UDP_GLB_STAT_OFF    0x00000004  /*!< \brief Global state register (offset). */
#define UDP_GLB_STAT (UDP_BASE + UDP_GLB_STAT_OFF) /*!< \brief Global state register (address). */
#define UDP_FADDEN              _BV(0)  /*!< \brief Device is in address state. */
#define UDP_CONFG               _BV(1)  /*!< \brief Device is in configured state. */
#define UDP_ESR                 _BV(2)  /*!< \brief Enable send resume. */
#define UDP_RSMINPR             _BV(3)  /*!< \brief A resume has been sent to the host. */
#define UDP_RMWUPE              _BV(4)  /*!< \brief Remote wake up enable. */
/*@}*/

/*! \name USB Device Port Function Address Register */
/*@{*/
#define UDP_FADDR_OFF       0x00000008  /*!< \brief Function address register (offset). */
#define UDP_FADDR (UDP_BASE + UDP_FADDR_OFF) /*!< \brief Function address register (address). */
#define UDP_FADD            0x0000007F  /*!< \brief Function address value (mask). */
#define UDP_FADD_LSB                0   /*!< \brief Function address value (LSB). */
#define UDP_FEN                 _BV(8)  /*!< \brief Function endpoint enabled. */
/*@}*/

/*! \name USB Device Port Interrupt Registers */
/*@{*/
#define UDP_IER_OFF         0x00000010  /*!< \brief Interrupt enable register (offset). */
#define UDP_IER (UDP_BASE + UDP_IER_OFF) /*!< \brief Interrupt enable register (address). */
#define UDP_IDR_OFF         0x00000014  /*!< \brief Interrupt disable register (offset). */
#define UDP_IDR (UDP_BASE + UDP_IDR_OFF) /*!< \brief Interrupt disable register (address). */
#define UDP_IMR_OFF         0x00000018  /*!< \brief Interrupt mask register (offset). */
#define UDP_IMR (UDP_BASE + UDP_IMR_OFF) /*!< \brief Interrupt mask register (address). */
#define UDP_ISR_OFF         0x0000001C  /*!< \brief Interrupt status register (offset). */
#define UDP_ISR (UDP_BASE + UDP_ISR_OFF) /*!< \brief Interrupt status register (address). */
#define UDP_ICR_OFF         0x00000020  /*!< \brief Interrupt clear register (offset). */
#define UDP_ICR (UDP_BASE + UDP_ICR_OFF) /*!< \brief Interrupt clear register (address). */
#define UDP_EPINT(n)            _BV(n)  /*!< \brief Endpoint interrupt 0-7. */
#define UDP_RXSUSP              _BV(8)  /*!< \brief USB suspend interrupt. */
#define UDP_RXRSM               _BV(9)  /*!< \brief USB resume interrupt. */
#define UDP_EXTRSM              _BV(10) /*!< \brief USB external resume interrupt. */
#define UDP_SOFINT              _BV(11) /*!< \brief USB start of frame interrupt. */
#define UDP_ENDBUSRES           _BV(12) /*!< \brief USB end of bus reset interrupt. */
#define UDP_WAKEUP              _BV(13) /*!< \brief USB resume interrupt. */
/*@}*/

/*! \name USB Device Port Reset Endpoint Register */
/*@{*/
#define UDP_RST_EP_OFF      0x00000028  /*!< \brief Reset endpoint register (offset). */
#define UDP_RST_EP (UDP_BASE + UDP_RST_EP_OFF) /*!< \brief Reset endpoint register (address). */

#define UDP_EP(n)           _BV(n)      /*!< \brief Reset endpoint 0-7. */
/*@}*/

/*! \name USB Device Port Endpoint Control and Status Registers */
/*@{*/
#define UDP_CSR_OFF         0x00000030  /*!< \brief Endpoint control and status register (offset). */
#define UDP_CSR(n) (UDP_BASE + UDP_CSR_OFF + (n) * 4) /*!< \brief Endpoint control and status registers 0-7 (address). */

#define UDP_TXCOMP              _BV(0)  /*!< \brief Generates an IN packet with data previously written in the DPR. */
#define UDP_RX_DATA_BK0         _BV(1)  /*!< \brief Receive data bank 0. */
#define UDP_RXSETUP             _BV(2)  /*!< \brief Sends Stall to the host (control endpoints). */
#define UDP_STALLSEND_ISOERROR  _BV(3)  /*!< \brief Stall send / isochronous error (isochronous endpoints). */
#define UDP_TXPKTRDY            _BV(4)  /*!< \brief Transmit packet ready. */
#define UDP_FORCESTALL          _BV(5)  /*!< \brief Force Stall (used by control, bulk and isochronous endpoints). */
#define UDP_RX_DATA_BK1         _BV(6)  /*!< \brief Receive data bank 1 (only used by endpoints with ping-pong attributes). */
#define UDP_DIR                 _BV(7)  /*!< \brief Transfer direction. */
#define UDP_EPTYPE          (0x7 << 8)  /*!< \brief Endpoint type mask. */
#define UDP_EPTYPE_CTRL     (0x0 << 8)  /*!< \brief Endpoint type is control. */
#define UDP_EPTYPE_ISO_OUT  (0x1 << 8)  /*!< \brief Endpoint type is isochronous output. */
#define UDP_EPTYPE_BULK_OUT (0x2 << 8)  /*!< \brief Endpoint type is bulk output. */
#define UDP_EPTYPE_INT_OUT  (0x3 << 8)  /*!< \brief Endpoint type is interrupt output. */
#define UDP_EPTYPE_ISO_IN   (0x5 << 8)  /*!< \brief Endpoint type is isochronous input. */
#define UDP_EPTYPE_BULK_IN  (0x6 << 8)  /*!< \brief Endpoint type is bulk input. */
#define UDP_EPTYPE_INT_IN   (0x7 << 8)  /*!< \brief Endpoint type is interrupt input. */
#define UDP_DTGLE               _BV(11) /*!< \brief Data toggle. */
#define UDP_EPEDS               _BV(15) /*!< \brief Endpoint enable disable. */
#define UDP_RXBYTECNT     (0x7FF << 16) /*!< \brief Number of bytes available in the FIFO (mask). */
#define UDP_RXBYTECNT_LSB           16  /*!< \brief Number of bytes available in the FIFO (LSB). */
/*@}*/

/*! \name USB Device Port Endpoint FIFO Data Registers */
/*@{*/
#define UDP_FDR_OFF         0x00000050  /*!< \brief Endpoint FIFO data register (offset). */
#define UDP_FDR(n) (UDP_BASE + UDP_FDR_OFF + (n) * 4) /*!< \brief Endpoint FIFO data register (address). */
#define UDP_FIFO_DATA       0x000000FF  /*!< \brief FIFO data value (mask). */
#define UDP_FIFO_DATA_LSB           0   /*!< \brief FIFO data value (LSB). */
/*@}*/

/*! \name USB Device Port Tranceiver Control Register */
/*@{*/
#define UDP_TXVC_OFF        0x00000074  /*!< \brief Transceiver control register (offset). */
#define UDP_TXVC (UDP_BASE + UDP_TXVC_OFF) /*!< \brief Transceiver control register (address). */
#define UDP_TXVDIS              _BV(8)  /*!< \brief Tranceiver disable. */
#define UDP_PUON                _BV(9)  /*!< \brief Pull-up enable. */
/*@}*/

/*@} xgNutArchArmAt91Udp */

#endif /* _ARCH_ARM_AT91_UDP_H_ */
