/*
 * Copyright (C) 2003-2004 by egnite Software GmbH. All rights reserved.
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
 * Revision 1.5  2009/02/06 15:37:39  haraldkipp
 * Added stack space multiplier and addend. Adjusted stack space.
 *
 * Revision 1.4  2008/08/11 06:59:14  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.3  2006/10/08 16:48:07  haraldkipp
 * Documentation fixed
 *
 * Revision 1.2  2005/08/02 17:46:45  haraldkipp
 * Major API documentation update.
 *
 * Revision 1.1  2005/07/26 18:02:27  haraldkipp
 * Moved from dev.
 *
 * Revision 1.10  2005/06/26 12:40:59  chaac
 * Added support for raw mode to AHDLC driver.
 *
 * Revision 1.9  2005/05/27 14:43:28  chaac
 * Fixed bugs on closing AHDLC sessions. Fixed AHDLC ioctl handling. Not all
 * messages were handled correctly	and fixed possible problem of reading memory
 * from address zero.
 *
 * Revision 1.8  2005/04/05 17:44:56  haraldkipp
 * Made stack space configurable.
 *
 * Revision 1.7  2005/02/10 07:06:17  hwmaier
 * Changes to incorporate support for AT90CAN128 CPU
 *
 * Revision 1.6  2005/01/24 21:11:48  freckle
 * renamed NutEventPostFromIRQ into NutEventPostFromIrq
 *
 * Revision 1.5  2005/01/21 16:49:45  freckle
 * Seperated calls to NutEventPostAsync between Threads and IRQs
 *
 * Revision 1.4  2004/12/16 08:40:35  haraldkipp
 * Late increment fixes ICCAVR bug.
 *
 * Revision 1.3  2004/03/18 14:05:20  haraldkipp
 * Comments updated
 *
 * Revision 1.2  2004/03/16 16:48:27  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 * Revision 1.1  2004/03/08 11:16:54  haraldkipp
 * Asynchronous HDLC driver added
 *
 */

#include <cfg/ahdlc.h>
#include <cfg/arch/avr.h>

#include <sys/atom.h>
#include <sys/heap.h>
#include <sys/event.h>
#include <sys/timer.h>
#include <sys/thread.h>

#include <dev/irqreg.h>
#include <dev/uartavr.h>
#include <dev/ppp.h>
#include <dev/ahdlcavr.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

/*!
 * \addtogroup xgAhdlcAvr
 */
/*@{*/

/*!
* \name UART0 RTS Handshake Control
*
* UART0_RTS_BIT must be defined in arch/avr.h
*/
#ifdef UART0_RTS_BIT

#if (UART0_RTS_AVRPORT == AVRPORTB)
#define UART0_RTS_PORT  PORTB
#define UART0_RTS_DDR   DDRB

#elif (UART0_RTS_AVRPORT == AVRPORTD)
#define UART0_RTS_PORT  PORTD
#define UART0_RTS_DDR   DDRD

#elif (UART0_RTS_AVRPORT == AVRPORTE)
#define UART0_RTS_PORT  PORTE
#define UART0_RTS_DDR   DDRE

#elif (UART0_RTS_AVRPORT == AVRPORTF)
#define UART0_RTS_PORT  PORTF
#define UART0_RTS_DDR   DDRF

#elif (UART0_RTS_AVRPORT == AVRPORTG)
#define UART0_RTS_PORT  PORTG
#define UART0_RTS_DDR   DDRG

#elif (UART0_RTS_AVRPORT == AVRPORTH)
#define UART0_RTS_PORT  PORTH
#define UART0_RTS_DDR   DDRH

#endif

#endif /* UART0_RTS_BIT */

#ifdef UART0_DTR_BIT

#if (UART0_DTR_AVRPORT == AVRPORTB)
#define UART0_DTR_PORT  PORTB
#define UART0_DTR_DDR   DDRB

#elif (UART0_DTR_AVRPORT == AVRPORTD)
#define UART0_DTR_PORT  PORTD
#define UART0_DTR_DDR   DDRD

#elif (UART0_DTR_AVRPORT == AVRPORTE)
#define UART0_DTR_PORT  PORTE
#define UART0_DTR_DDR   DDRE

#elif (UART0_DTR_AVRPORT == AVRPORTF)
#define UART0_DTR_PORT  PORTF
#define UART0_DTR_DDR   DDRF

#elif (UART0_DTR_AVRPORT == AVRPORTG)
#define UART0_DTR_PORT  PORTG
#define UART0_DTR_DDR   DDRG

#elif (UART0_DTR_AVRPORT == AVRPORTH)
#define UART0_DTR_PORT  PORTH
#define UART0_DTR_DDR   DDRH

#endif

#endif /* UART0_DTR_BIT */

/*!
* \name UART1 RTS Handshake Control
*
* UART1_RTS_BIT must be defined in arch/avr.h
*/
#ifdef UART1_RTS_BIT

#if (UART1_RTS_AVRPORT == AVRPORTB)
#define UART1_RTS_PORT   PORTB
#define UART1_RTS_DDR    DDRB

#elif (UART1_RTS_AVRPORT == AVRPORTD)
#define UART1_RTS_PORT   PORTD
#define UART1_RTS_DDR    DDRD

#elif (UART1_RTS_AVRPORT == AVRPORTE)
#define UART1_RTS_PORT   PORTE
#define UART1_RTS_DDR    DDRE

#elif (UART1_RTS_AVRPORT == AVRPORTF)
#define UART1_RTS_PORT   PORTF
#define UART1_RTS_DDR    DDRF

#elif (UART1_RTS_AVRPORT == AVRPORTG)
#define UART1_RTS_PORT   PORTG
#define UART1_RTS_DDR    DDRG

#elif (UART1_RTS_AVRPORT == AVRPORTH)
#define UART1_RTS_PORT   PORTH
#define UART1_RTS_DDR    DDRH

#endif

#endif /* UART1_RTS_BIT */

#ifdef UART1_DTR_BIT

#if (UART1_DTR_AVRPORT == AVRPORTB)
#define UART1_DTR_PORT  PORTB
#define UART1_DTR_DDR   DDRB

#elif (UART1_DTR_AVRPORT == AVRPORTD)
#define UART1_DTR_PORT  PORTD
#define UART1_DTR_DDR   DDRD

#elif (UART1_DTR_AVRPORT == AVRPORTE)
#define UART1_DTR_PORT  PORTE
#define UART1_DTR_DDR   DDRE

#elif (UART1_DTR_AVRPORT == AVRPORTF)
#define UART1_DTR_PORT  PORTF
#define UART1_DTR_DDR   DDRF

#elif (UART1_DTR_AVRPORT == AVRPORTG)
#define UART1_DTR_PORT  PORTG
#define UART1_DTR_DDR   DDRG

#elif (UART1_DTR_AVRPORT == AVRPORTH)
#define UART1_DTR_PORT  PORTH
#define UART1_DTR_DDR   DDRH

#endif

#endif /* UART1_DTR_BIT */

/*
 * FCS lookup table located in program memory space.
 */
static prog_char fcstab[512] = {
    0x00, 0x00, 0x11, 0x89, 0x23, 0x12, 0x32, 0x9b, 0x46, 0x24, 0x57, 0xad, 0x65, 0x36, 0x74, 0xbf,
    0x8c, 0x48, 0x9d, 0xc1, 0xaf, 0x5a, 0xbe, 0xd3, 0xca, 0x6c, 0xdb, 0xe5, 0xe9, 0x7e, 0xf8, 0xf7,
    0x10, 0x81, 0x01, 0x08, 0x33, 0x93, 0x22, 0x1a, 0x56, 0xa5, 0x47, 0x2c, 0x75, 0xb7, 0x64, 0x3e,
    0x9c, 0xc9, 0x8d, 0x40, 0xbf, 0xdb, 0xae, 0x52, 0xda, 0xed, 0xcb, 0x64, 0xf9, 0xff, 0xe8, 0x76,
    0x21, 0x02, 0x30, 0x8b, 0x02, 0x10, 0x13, 0x99, 0x67, 0x26, 0x76, 0xaf, 0x44, 0x34, 0x55, 0xbd,
    0xad, 0x4a, 0xbc, 0xc3, 0x8e, 0x58, 0x9f, 0xd1, 0xeb, 0x6e, 0xfa, 0xe7, 0xc8, 0x7c, 0xd9, 0xf5,
    0x31, 0x83, 0x20, 0x0a, 0x12, 0x91, 0x03, 0x18, 0x77, 0xa7, 0x66, 0x2e, 0x54, 0xb5, 0x45, 0x3c,
    0xbd, 0xcb, 0xac, 0x42, 0x9e, 0xd9, 0x8f, 0x50, 0xfb, 0xef, 0xea, 0x66, 0xd8, 0xfd, 0xc9, 0x74,

    0x42, 0x04, 0x53, 0x8d, 0x61, 0x16, 0x70, 0x9f, 0x04, 0x20, 0x15, 0xa9, 0x27, 0x32, 0x36, 0xbb,
    0xce, 0x4c, 0xdf, 0xc5, 0xed, 0x5e, 0xfc, 0xd7, 0x88, 0x68, 0x99, 0xe1, 0xab, 0x7a, 0xba, 0xf3,
    0x52, 0x85, 0x43, 0x0c, 0x71, 0x97, 0x60, 0x1e, 0x14, 0xa1, 0x05, 0x28, 0x37, 0xb3, 0x26, 0x3a,
    0xde, 0xcd, 0xcf, 0x44, 0xfd, 0xdf, 0xec, 0x56, 0x98, 0xe9, 0x89, 0x60, 0xbb, 0xfb, 0xaa, 0x72,
    0x63, 0x06, 0x72, 0x8f, 0x40, 0x14, 0x51, 0x9d, 0x25, 0x22, 0x34, 0xab, 0x06, 0x30, 0x17, 0xb9,
    0xef, 0x4e, 0xfe, 0xc7, 0xcc, 0x5c, 0xdd, 0xd5, 0xa9, 0x6a, 0xb8, 0xe3, 0x8a, 0x78, 0x9b, 0xf1,
    0x73, 0x87, 0x62, 0x0e, 0x50, 0x95, 0x41, 0x1c, 0x35, 0xa3, 0x24, 0x2a, 0x16, 0xb1, 0x07, 0x38,
    0xff, 0xcf, 0xee, 0x46, 0xdc, 0xdd, 0xcd, 0x54, 0xb9, 0xeb, 0xa8, 0x62, 0x9a, 0xf9, 0x8b, 0x70,

    0x84, 0x08, 0x95, 0x81, 0xa7, 0x1a, 0xb6, 0x93, 0xc2, 0x2c, 0xd3, 0xa5, 0xe1, 0x3e, 0xf0, 0xb7,
    0x08, 0x40, 0x19, 0xc9, 0x2b, 0x52, 0x3a, 0xdb, 0x4e, 0x64, 0x5f, 0xed, 0x6d, 0x76, 0x7c, 0xff,
    0x94, 0x89, 0x85, 0x00, 0xb7, 0x9b, 0xa6, 0x12, 0xd2, 0xad, 0xc3, 0x24, 0xf1, 0xbf, 0xe0, 0x36,
    0x18, 0xc1, 0x09, 0x48, 0x3b, 0xd3, 0x2a, 0x5a, 0x5e, 0xe5, 0x4f, 0x6c, 0x7d, 0xf7, 0x6c, 0x7e,
    0xa5, 0x0a, 0xb4, 0x83, 0x86, 0x18, 0x97, 0x91, 0xe3, 0x2e, 0xf2, 0xa7, 0xc0, 0x3c, 0xd1, 0xb5,
    0x29, 0x42, 0x38, 0xcb, 0x0a, 0x50, 0x1b, 0xd9, 0x6f, 0x66, 0x7e, 0xef, 0x4c, 0x74, 0x5d, 0xfd,
    0xb5, 0x8b, 0xa4, 0x02, 0x96, 0x99, 0x87, 0x10, 0xf3, 0xaf, 0xe2, 0x26, 0xd0, 0xbd, 0xc1, 0x34,
    0x39, 0xc3, 0x28, 0x4a, 0x1a, 0xd1, 0x0b, 0x58, 0x7f, 0xe7, 0x6e, 0x6e, 0x5c, 0xf5, 0x4d, 0x7c,

    0xc6, 0x0c, 0xd7, 0x85, 0xe5, 0x1e, 0xf4, 0x97, 0x80, 0x28, 0x91, 0xa1, 0xa3, 0x3a, 0xb2, 0xb3,
    0x4a, 0x44, 0x5b, 0xcd, 0x69, 0x56, 0x78, 0xdf, 0x0c, 0x60, 0x1d, 0xe9, 0x2f, 0x72, 0x3e, 0xfb,
    0xd6, 0x8d, 0xc7, 0x04, 0xf5, 0x9f, 0xe4, 0x16, 0x90, 0xa9, 0x81, 0x20, 0xb3, 0xbb, 0xa2, 0x32,
    0x5a, 0xc5, 0x4b, 0x4c, 0x79, 0xd7, 0x68, 0x5e, 0x1c, 0xe1, 0x0d, 0x68, 0x3f, 0xf3, 0x2e, 0x7a,
    0xe7, 0x0e, 0xf6, 0x87, 0xc4, 0x1c, 0xd5, 0x95, 0xa1, 0x2a, 0xb0, 0xa3, 0x82, 0x38, 0x93, 0xb1,
    0x6b, 0x46, 0x7a, 0xcf, 0x48, 0x54, 0x59, 0xdd, 0x2d, 0x62, 0x3c, 0xeb, 0x0e, 0x70, 0x1f, 0xf9,
    0xf7, 0x8f, 0xe6, 0x06, 0xd4, 0x9d, 0xc5, 0x14, 0xb1, 0xab, 0xa0, 0x22, 0x92, 0xb9, 0x83, 0x30,
    0x7b, 0xc7, 0x6a, 0x4e, 0x58, 0xd5, 0x49, 0x5c, 0x3d, 0xe3, 0x2c, 0x6a, 0x1e, 0xf1, 0x0f, 0x78
};

/*!
 * Checks the 32-bit ACCM to see if the byte needs un-escaping
 */
#define IN_ACC_MAP(c, m) in_acc_map(c, &(m))
/* Trust me, this is a whole lot smaller when compiled than it looks in C.
 * It is written explicitly so that gcc can make good AVR asm out of it. */
static INLINE uint8_t in_acc_map(u_char ch, void * esc_mask) __attribute__((always_inline));  /* gcc specific attribute */
static INLINE uint8_t in_acc_map(u_char ch, void * esc_mask)
{
    const uint8_t shift_mask = 0x07;
    const uint8_t index_mask = 0x18;
    const uint8_t over_mask = ~(shift_mask|index_mask);

    uint8_t shift, index, emask;
    uint8_t * emskp = esc_mask;

    if (over_mask & ch) {
        return 0;
    }

    shift = shift_mask & ch;

    index = ch >> (uint8_t)3;
	
    /* NOTE:  This assumes that the esc bit field is little endian,
     * which it should have been switched to before being set. */

    emask = emskp[index];
    return emask & ( ((uint8_t)1) << shift );
}

#ifndef NUT_THREAD_AHDLCRXSTACK
#define NUT_THREAD_AHDLCRXSTACK     512
#endif

/*
 * Handle AVR UART0 transmit complete interrupts.
 */
static void Tx0Complete(void *arg)
{
    AHDLCDCB *dcb = arg;

    if (dcb->dcb_tx_idx != dcb->dcb_wr_idx) {
#ifdef UART0_CTS_BIT
        if (bit_is_set(UART0_CTS_PIN, UART0_CTS_BIT)) {
            cbi(UCR, UDRIE);
            return;
        }
#endif
        outb(UDR, dcb->dcb_tx_buf[dcb->dcb_tx_idx]);
        dcb->dcb_tx_idx++;
    } else {
        cbi(UCR, UDRIE);
        NutEventPostFromIrq(&dcb->dcb_tx_rdy);
    }
}

#ifdef UART0_CTS_BIT
/*
 * Handle AVR UART0 transmit handshake interrupts.
 */
static void Cts0Interrupt(void *arg)
{
    sbi(UCR, UDRIE);
}
#endif

#ifdef __AVR_ENHANCED__
/*
 * Handle AVR UART1 transmit complete interrupts.
 */
static void Tx1Complete(void *arg)
{
    register AHDLCDCB *dcb = arg;

    if (dcb->dcb_tx_idx != dcb->dcb_wr_idx) {
#ifdef UART1_CTS_BIT
        if (bit_is_set(UART1_CTS_PIN, UART1_CTS_BIT)) {
            cbi(UCSR1B, UDRIE);
            return;
        }
#endif
        outb(UDR1, dcb->dcb_tx_buf[dcb->dcb_tx_idx]);
        dcb->dcb_tx_idx++;
    } else {
        cbi(UCSR1B, UDRIE);
        NutEventPostFromIrq(&dcb->dcb_tx_rdy);
    }
}

#ifdef UART1_CTS_BIT
/*
 * Handle AVR UART1 transmit handshake interrupts.
 */
static void Cts1Interrupt(void *arg)
{
    sbi(UCSR1B, UDRIE);
}
#endif

#endif                          /* __AVR_ENHANCED__ */

/*
 * Handle AVR UART0 receive complete interrupts.
 */
static void Rx0Complete(void *arg)
{
    AHDLCDCB *dcb = arg;

    dcb->dcb_rx_buf[dcb->dcb_rx_idx] = inb(UDR);
    if (dcb->dcb_rd_idx == dcb->dcb_rx_idx)
        NutEventPostFromIrq(&dcb->dcb_rx_rdy);
    /* Late increment fixes ICCAVR bug on volatile variables. */
    dcb->dcb_rx_idx++;
}

#ifdef __AVR_ENHANCED__
/*
 * Handle AVR UART1 receive complete interrupts.
 */
static void Rx1Complete(void *arg)
{
    AHDLCDCB *dcb = arg;

    dcb->dcb_rx_buf[dcb->dcb_rx_idx] = inb(UDR1);
    if (dcb->dcb_rd_idx == dcb->dcb_rx_idx)
        NutEventPostFromIrq(&dcb->dcb_rx_rdy);
    /* Late increment fixes ICCAVR bug on volatile variables. */
    dcb->dcb_rx_idx++;
}
#endif                          /* __AVR_ENHANCED__ */

/*
 * \return 0 on success, -1 in case of any errors.
 */
static int SendRawByte(AHDLCDCB * dcb, uint8_t ch, uint8_t flush)
{
    /*
     * If transmit buffer is full, wait until interrupt routine
     * signals an empty buffer or until a timeout occurs.
     */
    while ((uint8_t) (dcb->dcb_wr_idx + 1) == dcb->dcb_tx_idx) {
        if (NutEventWait(&dcb->dcb_tx_rdy, dcb->dcb_wtimeout))
            break;
    }

    /*
     * If transmit buffer is still full, we have a write timeout.
     */
    if ((uint8_t) (dcb->dcb_wr_idx + 1) == dcb->dcb_tx_idx) {
        return -1;
    }

    /*
     * Buffer has room for more data. Put the byte in the buffer
     * and increment the write index.
     */
    dcb->dcb_tx_buf[dcb->dcb_wr_idx] = ch;
    dcb->dcb_wr_idx++;

    /*
     * If transmit buffer has become full and the transmitter
     * is not active, then activate it.
     */
    if (flush || (uint8_t) (dcb->dcb_wr_idx + 1) == dcb->dcb_tx_idx) {

        /*
         * TODO: Check handshake.
         */
        NutEnterCritical();
#ifdef __AVR_ENHANCED__
        if (dcb->dcb_base)
            sbi(UCSR1B, UDRIE);
        else
#endif
            sbi(UCR, UDRIE);
        NutExitCritical();
    }
    return 0;
}

/*
 * Characters are properly escaped and checksum is updated.
 *
 * \return 0 on success, -1 in case of any errors.
 */
static int SendHdlcData(AHDLCDCB * dcb, CONST uint8_t * data, uint16_t len, uint16_t * txfcs)
{
    uint16_t tbx;
    register uint16_t fcs;

    if (txfcs)
        fcs = *txfcs;
    else
        fcs = 0;
    while (len) {
        tbx = (uint16_t) ((uint8_t) fcs ^ *data) << 1;
        fcs >>= 8;
        fcs ^= ((uint16_t) PRG_RDB(fcstab + tbx) << 8) | PRG_RDB(fcstab + tbx + 1);
        if (IN_ACC_MAP(*data, dcb->dcb_tx_accm) || *data == AHDLC_FLAG || *data == AHDLC_ESCAPE) {
            if (SendRawByte(dcb, AHDLC_ESCAPE, 0))
                return -1;
            if (SendRawByte(dcb, *data ^ AHDLC_TRANS, 0))
                return -1;
        } else if (SendRawByte(dcb, *data, 0))
            return -1;
        data++;
        len--;
    }
    if (txfcs)
        *txfcs = fcs;

    return 0;
}

/*!
 * \brief Send HDLC frame.
 *
 * \param dev   Identifies the device to use.
 * \param nb    Network buffer structure containing the packet to be sent.
 *              The structure must have been allocated by a previous
 *              call NutNetBufAlloc().
 *
 * \return 0 on success, -1 in case of any errors.
 */
int AhdlcOutput(NUTDEVICE * dev, NETBUF * nb)
{
    uint16_t txfcs;
    AHDLCDCB *dcb = dev->dev_dcb;
    uint16_t sz;

    /*
     * If we are in RAW mode we are not allowed to send AHDLC output.
     * We just emulate packet loss behaviour in here.
     */
    if (dcb->dcb_modeflags & UART_MF_RAWMODE) {
        return 0;
    }

    /*
     * Calculate the number of bytes to be send. Do not
     * send packets larger than transmit mru.
     */
    sz = nb->nb_dl.sz + nb->nb_nw.sz + nb->nb_tp.sz + nb->nb_ap.sz;

    if (sz > dcb->dcb_tx_mru) {
        return -1;
    }

    /*
     * TODO: If transmitter is running, we may omit the flag.
     */
    SendRawByte(dcb, AHDLC_FLAG, 0);

    /* Initialize the checksum and send the NETBUF. */
    txfcs = AHDLC_INITFCS;
    if (SendHdlcData(dcb, nb->nb_dl.vp, nb->nb_dl.sz, &txfcs))
        return -1;
    if (SendHdlcData(dcb, nb->nb_nw.vp, nb->nb_nw.sz, &txfcs))
        return -1;
    if (SendHdlcData(dcb, nb->nb_tp.vp, nb->nb_tp.sz, &txfcs))
        return -1;
    if (SendHdlcData(dcb, nb->nb_ap.vp, nb->nb_ap.sz, &txfcs))
        return -1;

    /* Send the checksum and the final flag. */
    txfcs ^= 0xffff;
    if (SendHdlcData(dcb, (uint8_t *) & txfcs, 2, 0))
        return -1;
    SendRawByte(dcb, AHDLC_FLAG, 1);

    return 0;
}

/*! \fn AhdlcRx(void *arg)
 * \brief Asynchronous HDLC receiver thread.
 *
 *
 * Running at high priority.
 */
THREAD(AhdlcRx, arg)
{
    NUTDEVICE *dev = arg;
    NUTDEVICE *netdev;
    AHDLCDCB *dcb = dev->dev_dcb;
    IFNET *ifn;
    NETBUF *nb;
    uint8_t *rxbuf;
    uint8_t *rxptr;
    uint16_t rxcnt;
    uint8_t ch;
    uint16_t tbx;
    uint8_t inframe;
    uint8_t escaped;
    uint16_t rxfcs;

    NutThreadSetPriority(9);
    for (;;) {
        /*
         * Reset variables to their initial state
         */
        rxptr = 0;
        rxcnt = 0;
        escaped = 0;
        rxfcs = AHDLC_INITFCS;
        inframe = 0;

        for (;;) {
            /*
             * Wait until the network interface has been attached.
             * This will be initiated by the application calling
             * NutNetIfConfig(), which in turn calls a HDLC_SETIFNET
             * ioctl() to store the NUTDEVICE pointer of the network
             * device in dev_icb and trigger an event on dcb_mf_evt.
             */
            while ((netdev = dev->dev_icb) == 0) {
                NutEventWait(&dcb->dcb_mf_evt, 1000);
            }
            ifn = netdev->dev_icb;
            dcb->dcb_rtimeout = 1000;
            inframe = 0;

            /*
             * Allocate the receive buffer, if this fails, we are in a
             * low memory situation. Take a nap and see, if the
             * situation improved.
             */
            if ((rxbuf = NutHeapAlloc(dcb->dcb_rx_mru)) != 0) {
                break;
            }
            NutSleep(1000);
        }

        /*
         * Signal the link driver that we are up.
         */
        ifn->if_send = AhdlcOutput;
        netdev->dev_ioctl(netdev, LCP_LOWERUP, 0);

        for (;;) {
            /*
             * If we are still connected to a network, fetch the next
             * character from the buffer.
             */
            while (dcb->dcb_rd_idx == dcb->dcb_rx_idx) {
                if (dev->dev_icb == 0)
                    break;
                /* TODO: Check for idle timeout. */
                if (NutEventWait(&dcb->dcb_rx_rdy, dcb->dcb_rtimeout)) {
                    continue;
                }
            }

            /*
             * Leave loop if network interface is detached
             */
            if (dev->dev_icb == 0)
                break;

            /*
             * If RAW mode is active, we are not allowing any data encapsulation
             * processing. So we just sleep for a while.
             */
            if (dcb->dcb_modeflags & UART_MF_RAWMODE) {
                /*
                 * It is a must to sleep here, because if we just yield it could create
                 * too much processing in here and stall processing elsewhere. This gives
                 * opportunity to other threads to process incoming data from USART.
                 */
                NutSleep(100);
                continue;
            }

            /*
             * Read next character from input buffer
             */
            ch = dcb->dcb_rx_buf[dcb->dcb_rd_idx++];

            if (inframe) {
                if (ch != AHDLC_FLAG) {
                    if (ch == AHDLC_ESCAPE) {
                        escaped = 1;
                        continue;
                    }
                    if (escaped) {
                        ch ^= AHDLC_TRANS;
                        escaped = 0;
                    }

                    /*
                     * Unless the peer lied to us about the negotiated MRU,
                     * we should never get a frame which is too long. If it
                     * happens, toss it away and grab the next incoming one.
                     */
                    if (rxcnt++ < dcb->dcb_rx_mru) {
                        /* Update calculated checksum and store character in buffer. */
                        tbx = (uint16_t) ((uint8_t) rxfcs ^ ch) << 1;
                        rxfcs >>= 8;
                        rxfcs ^= ((uint16_t) PRG_RDB(fcstab + tbx) << 8) | PRG_RDB(fcstab + tbx + 1);
                        *rxptr++ = ch;
                    } else
                        inframe = 0;
                    continue;
                }

                if (rxcnt > 6 && rxfcs == AHDLC_GOODFCS) {
                    /*
                     * If the frame checksum is valid, create a NETBUF
                     * and pass it to the network specific receive handler.
                     */
                    rxcnt -= 2;
                    if ((nb = NutNetBufAlloc(0, NBAF_DATALINK, rxcnt)) != 0) {
                        memcpy(nb->nb_dl.vp, rxbuf, rxcnt);
                        (*ifn->if_recv) (netdev, nb);
                    }
                }
            }

            /*
             * If frame flag is received, resync frame processing
             */
            if (ch == AHDLC_FLAG) {
                inframe = 1;
                escaped = 0;
                rxptr = rxbuf;
                rxcnt = 0;
                rxfcs = AHDLC_INITFCS;
            }
        }

        /* Signal the link driver that we are down. */
        netdev->dev_ioctl(netdev, LCP_LOWERDOWN, 0);

        /* Disconnected, clean up. */
        if (rxbuf) {
            NutHeapFree(rxbuf);
            rxbuf = 0;
        }
    }
}

/*
 * \param dev Indicates the UART device.
 *
 * \return 0 on success, -1 otherwise.
 */
static int AhdlcAvrGetStatus(NUTDEVICE * dev, uint32_t * status)
{
    AHDLCDCB *dcb = dev->dev_dcb;
    uint8_t us;

    *status = 0;

#ifdef __AVR_ENHANCED__
    if (dev->dev_base) {
#ifdef UART1_CTS_BIT
        if (bit_is_set(UART1_CTS_PIN, UART1_CTS_BIT))
            *status |= UART_CTSDISABLED;
        else
            *status |= UART_CTSENABLED;
#endif
#ifdef UART1_RTS_BIT
        if (bit_is_set(UART1_RTS_PORT, UART1_RTS_BIT))
            *status |= UART_RTSDISABLED;
        else
            *status |= UART_RTSENABLED;
#endif
#ifdef UART1_DTR_BIT
        if (bit_is_set(UART1_DTR_PORT, UART1_DTR_BIT))
            *status |= UART_DTRDISABLED;
        else
            *status |= UART_DTRENABLED;
#endif
        us = inb(UCSR1A);
    } else
#endif                          /* __AVR_ENHANCED__ */
    {
#ifdef UART0_CTS_BIT
        if (bit_is_set(UART0_CTS_PIN, UART0_CTS_BIT))
            *status |= UART_CTSDISABLED;
        else
            *status |= UART_CTSENABLED;
#endif
#ifdef UART0_RTS_BIT
        if (bit_is_set(UART0_RTS_PORT, UART0_RTS_BIT))
            *status |= UART_RTSDISABLED;
        else
            *status |= UART_RTSENABLED;
#endif
#ifdef UART0_DTR_BIT
        if (bit_is_set(UART0_DTR_PORT, UART0_DTR_BIT))
            *status |= UART_DTRDISABLED;
        else
            *status |= UART_DTRENABLED;
#endif
        us = inb(USR);
    }
    if (us & FE)
        *status |= UART_FRAMINGERROR;
    if (us & DOR)
        *status |= UART_OVERRUNERROR;
    if (dcb->dcb_tx_idx == dcb->dcb_wr_idx)
        *status |= UART_TXBUFFEREMPTY;
    if (dcb->dcb_rd_idx == dcb->dcb_rx_idx)
        *status |= UART_RXBUFFEREMPTY;

    return 0;
}

/*
 * \param dev Indicates the UART device.
 *
 * \return 0 on success, -1 otherwise.
 */
static int AhdlcAvrSetStatus(NUTDEVICE * dev, uint32_t status)
{
#ifdef __AVR_ENHANCED__
    if (dev->dev_base) {
#ifdef UART1_RTS_BIT
        if (status & UART_RTSDISABLED)
            sbi(UART1_RTS_PORT, UART1_RTS_BIT);
        else if (status & UART_RTSENABLED)
            cbi(UART1_RTS_PORT, UART1_RTS_BIT);
#endif
#ifdef UART1_DTR_BIT
        if (status & UART_DTRDISABLED)
            sbi(UART1_DTR_PORT, UART1_DTR_BIT);
        else if (status & UART_DTRENABLED)
            cbi(UART1_DTR_PORT, UART1_DTR_BIT);
#endif
    } else
#endif                          /* __AVR_ENHANCED__ */
    {
#ifdef UART0_RTS_BIT
        if (status & UART_RTSDISABLED)
            sbi(UART0_RTS_PORT, UART0_RTS_BIT);
        else if (status & UART_RTSENABLED)
            cbi(UART0_RTS_PORT, UART0_RTS_BIT);
#endif
#ifdef UART0_DTR_BIT
        if (status & UART_DTRDISABLED)
            sbi(UART0_DTR_PORT, UART0_DTR_BIT);
        else if (status & UART_DTRENABLED)
            cbi(UART0_DTR_PORT, UART0_DTR_BIT);
#endif
    }
    return 0;
}

/*
 * Carefully enable UART functions.
 */
static void AhdlcAvrEnable(uint16_t base)
{
    NutEnterCritical();

#ifdef __AVR_ENHANCED__
    if (base) {
#ifdef UART1_CTS_BIT
        sbi(EIMSK, UART1_CTS_BIT);
#endif
        outb(UCSR1B, BV(RXCIE) | BV(RXEN) | BV(TXEN));
    } else
#endif                          /* __AVR_ENHANCED__ */
    {
#ifdef UART0_CTS_BIT
        sbi(EIMSK, UART0_CTS_BIT);
#endif
        outb(UCR, BV(RXCIE) | BV(RXEN) | BV(TXEN));
    }
    NutExitCritical();
}

/*
 * Carefully disable UART functions.
 */
static void AhdlcAvrDisable(uint16_t base)
{
    /*
     * Disable UART interrupts.
     */
    NutEnterCritical();
#ifdef __AVR_ENHANCED__
    if (base) {
#ifdef UART1_CTS_BIT
        cbi(EIMSK, UART1_CTS_BIT);
#endif
        outb(UCSR1B, inb(UCSR1B) & ~(BV(RXCIE) | BV(UDRIE)));
    } else
#endif                          /* __AVR_ENHANCED__ */
    {
#ifdef UART0_CTS_BIT
        cbi(EIMSK, UART0_CTS_BIT);
#endif
        outb(UCR, inb(UCR) & ~(BV(RXCIE) | BV(UDRIE)));
    }
    NutExitCritical();

    /*
     * Allow incoming or outgoing character to finish.
     */
    NutDelay(10);

    /*
     * Now disable UART functions.
     */
#ifdef __AVR_ENHANCED__
    if (base)
        outb(UCSR1B, inb(UCSR1B) & ~(BV(RXEN) | BV(TXEN)));
    else
#endif                          /* __AVR_ENHANCED__ */
        outb(UCR, inb(UCR) & ~(BV(RXEN) | BV(TXEN)));
}

/*!
 * \brief Perform on-chip UART control functions.
 *
 * \param dev  Identifies the device that receives the device-control
 *             function.
 * \param req  Requested control function. May be set to one of the
 *             following constants:
 *             - UART_SETSPEED, conf points to an uint32_t value containing the baudrate.
 *             - UART_GETSPEED, conf points to an uint32_t value receiving the current baudrate.
 *             - UART_SETDATABITS, conf points to an uint32_t value containing the number of data bits, 5, 6, 7 or 8.
 *             - UART_GETDATABITS, conf points to an uint32_t value receiving the number of data bits, 5, 6, 7 or 8.
 *             - UART_SETPARITY, conf points to an uint32_t value containing the parity, 0 (no), 1 (odd) or 2 (even).
 *             - UART_GETPARITY, conf points to an uint32_t value receiving the parity, 0 (no), 1 (odd) or 2 (even).
 *             - UART_SETSTOPBITS, conf points to an uint32_t value containing the number of stop bits 1 or 2.
 *             - UART_GETSTOPBITS, conf points to an uint32_t value receiving the number of stop bits 1 or 2.
 *             - UART_SETSTATUS, conf points to an uint32_t value containing the changes for the control lines.
 *             - UART_GETSTATUS, conf points to an uint32_t value receiving the state of the control lines and the device.
 *             - UART_SETREADTIMEOUT, conf points to an uint32_t value containing the read timeout.
 *             - UART_GETREADTIMEOUT, conf points to an uint32_t value receiving the read timeout.
 *             - UART_SETWRITETIMEOUT, conf points to an uint32_t value containing the write timeout.
 *             - UART_GETWRITETIMEOUT, conf points to an uint32_t value receiving the write timeout.
 *             - UART_SETLOCALECHO, conf points to an uint32_t value containing 0 (off) or 1 (on).
 *             - UART_GETLOCALECHO, conf points to an uint32_t value receiving 0 (off) or 1 (on).
 *             - UART_SETFLOWCONTROL, conf points to an uint32_t value containing combined UART_FCTL_ values.
 *             - UART_GETFLOWCONTROL, conf points to an uint32_t value containing receiving UART_FCTL_ values.
 *             - UART_SETCOOKEDMODE, conf points to an uint32_t value containing 0 (off) or 1 (on).
 *             - UART_GETCOOKEDMODE, conf points to an uint32_t value receiving 0 (off) or 1 (on).
 *             - UART_SETRAWMODE, conf points to an uint32_t value containing 0 (off) or 1 (on).
 *             - UART_GETRAWMODE, conf points to an uint32_t value receiving 0 (off) or 1 (on).
 *             - HDLC_SETIFNET, conf points to a pointer containing the address of the network device's NUTDEVICE structure.
 *             - HDLC_GETIFNET, conf points to a pointer receiving the address of the network device's NUTDEVICE structure.
 *             - HDLC_SETTXACCM, conf points to an uint32_t value containing the ACC Map in host endian.
 *             - HDLC_GETTXACCM, conf points to an uint32_t value receiving the ACC Map in host endian.
 *
 * \param conf Points to a buffer that contains any data required for
 *             the given control function or receives data from that
 *             function.
 * \return 0 on success, -1 otherwise.
 *
 * \warning Timeout values are given in milliseconds and are limited to
 *          the granularity of the system timer.
 *
 * \note For ATmega103, only 8 data bits, 1 stop bit and no parity are allowed.
 *
 */
int AhdlcAvrIOCtl(NUTDEVICE * dev, int req, void *conf)
{
    int rc = 0;
    AHDLCDCB *dcb;
    void **ppv = (void **) conf;
    uint32_t *lvp = (uint32_t *) conf;
    uint8_t bv;
    uint16_t sv;
    uint8_t devnum;

    if (dev == 0)
        dev = &devUart0;

    devnum = dev->dev_base;
    dcb = dev->dev_dcb;

    switch (req) {
    case UART_SETSPEED:
        AhdlcAvrDisable(devnum);
        sv = (uint16_t) ((((2UL * NutGetCpuClock()) / (*lvp * 16UL)) + 1UL) / 2UL) - 1;
#ifdef __AVR_ENHANCED__
        if (devnum) {
            outb(UBRR1L, (uint8_t) sv);
            outb(UBRR1H, (uint8_t) (sv >> 8));
        } else {
            outb(UBRR0L, (uint8_t) sv);
            outb(UBRR0H, (uint8_t) (sv >> 8));
        }
#else
        outb(UBRR, (uint8_t) sv);
#endif
        AhdlcAvrEnable(devnum);
        break;

    case UART_GETSPEED:
#ifdef __AVR_ENHANCED__
        if (devnum)
            sv = (uint16_t) inb(UBRR1H) << 8 | inb(UBRR1L);
        else
            sv = (uint16_t) inb(UBRR0H) << 8 | inb(UBRR0L);
#else
        sv = inb(UBRR);
#endif
        *lvp = NutGetCpuClock() / (16UL * (uint32_t) (sv + 1));
        break;

    case UART_SETDATABITS:
        AhdlcAvrDisable(devnum);
        bv = (uint8_t)(*lvp);
#ifdef __AVR_ENHANCED__
        if (bv >= 5 && bv <= 8) {
            bv = (bv - 5) << 1;
            if (devnum) {
                outb(UCSR1C, (inb(UCSR1C) & 0xF9) | bv);
                outb(UCSR1B, inb(UCSR1B) & 0xFB);
            } else {
                outb(UCSR0C, (inb(UCSR0C) & 0xF9) | bv);
                outb(UCSR0B, inb(UCSR0B) & 0xFB);
            }
        } else
            rc = -1;
#else
        if (bv != 8)
            rc = -1;
#endif
        AhdlcAvrEnable(devnum);
        break;

    case UART_GETDATABITS:
#ifdef __AVR_ENHANCED__
        if (devnum)
            *lvp = ((inb(UCSR1C) & 0x06) >> 1) + 5;
        else
            *lvp = ((inb(UCSR0C) & 0x06) >> 1) + 5;
#else
        *lvp = 8;
#endif
        break;

    case UART_SETPARITY:
        AhdlcAvrDisable(devnum);
        bv = (uint8_t)(*lvp);
#ifdef __AVR_ENHANCED__
        if (bv <= 2) {
            if (bv == 1)
                bv = 3;
            bv <<= 4;
            if (devnum)
                outb(UCSR1C, (inb(UCSR1C) & 0xCF) | bv);
            else
                outb(UCSR0C, (inb(UCSR0C) & 0xCF) | bv);
        } else
            rc = -1;
#endif
        if (bv)
            rc = -1;
        AhdlcAvrEnable(devnum);
        break;

    case UART_GETPARITY:
#ifdef __AVR_ENHANCED__
        if (devnum)
            bv = (inb(UCSR1C) & 0x30) >> 4;
        else
            bv = (inb(UCSR0C) & 0x30) >> 4;
        if (bv == 3)
            bv = 1;
#else
        bv = 0;
#endif
        *lvp = bv;
        break;

    case UART_SETSTOPBITS:
        AhdlcAvrDisable(devnum);
        bv = (uint8_t)(*lvp);
#ifdef __AVR_ENHANCED__
        if (bv == 1 || bv == 2) {
            bv = (bv - 1) << 3;
            if (devnum)
                outb(UCSR1C, (inb(UCSR1C) & 0xF7) | bv);
            else
                outb(UCSR0C, (inb(UCSR0C) & 0xF7) | bv);
        } else
            rc = -1;
#else
        if (bv != 1)
            rc = -1;
#endif
        AhdlcAvrEnable(devnum);
        break;

    case UART_GETSTOPBITS:
#ifdef __AVR_ENHANCED__
        if (devnum)
            *lvp = ((inb(UCSR1C) & 0x08) >> 3) + 1;
        else
            *lvp = ((inb(UCSR0C) & 0x08) >> 3) + 1;
#else
        *lvp = 1;
#endif
        break;

    case UART_GETSTATUS:
        AhdlcAvrGetStatus(dev, lvp);
        break;
    case UART_SETSTATUS:
        AhdlcAvrSetStatus(dev, *lvp);
        break;

    case UART_SETREADTIMEOUT:
        dcb->dcb_rtimeout = *lvp;
        break;
    case UART_GETREADTIMEOUT:
        *lvp = dcb->dcb_rtimeout;
        break;

    case UART_SETWRITETIMEOUT:
        dcb->dcb_wtimeout = *lvp;
        break;
    case UART_GETWRITETIMEOUT:
        *lvp = dcb->dcb_wtimeout;
        break;

    case UART_SETLOCALECHO:
        bv = (uint8_t)(*lvp);
        if (bv)
            dcb->dcb_modeflags |= UART_MF_LOCALECHO;
        else
            dcb->dcb_modeflags &= ~UART_MF_LOCALECHO;
        break;
    case UART_GETLOCALECHO:
        if (dcb->dcb_modeflags & UART_MF_LOCALECHO)
            *lvp = 1;
        else
            *lvp = 0;
        break;

    case UART_SETFLOWCONTROL:
        bv = (uint8_t)(*lvp);
        if (bv)
            dcb->dcb_modeflags |= UART_MF_LOCALECHO;
        else
            dcb->dcb_modeflags &= ~UART_MF_LOCALECHO;
        break;
    case UART_GETFLOWCONTROL:
        break;

    case UART_SETRAWMODE:
        bv = (uint8_t)(*lvp);
        if (bv)
            dcb->dcb_modeflags |= UART_MF_RAWMODE;
        else
            dcb->dcb_modeflags &= ~UART_MF_RAWMODE;
        break;

    case UART_GETRAWMODE:
        if (dcb->dcb_modeflags & UART_MF_RAWMODE)
            *lvp = 1;
        else
            *lvp = 0;
        break;

    case HDLC_SETIFNET:
        if (ppv && (*ppv != 0)) {
            dev->dev_icb = *ppv;
            dev->dev_type = IFTYP_NET;
            NutEventPost(&dcb->dcb_mf_evt);
        } else {
            dev->dev_type = IFTYP_CHAR;

            if (dev->dev_icb != 0)
            {
                dev->dev_icb = 0;

                /*
                 * Signal AHDLC Thread, so it can change it's state instantly
                 */
                NutEventPost(&dcb->dcb_rx_rdy);
            }
        }
        break;
    case HDLC_GETIFNET:
        *ppv = dev->dev_icb;
        break;

    /* AVR host endian is little endian, and the ACCM should have been switched
     * to host endian when the value was read in. */
    case HDLC_SETTXACCM:
        dcb->dcb_tx_accm = (*lvp);
        break;

    case HDLC_GETTXACCM:
        (*lvp) = dcb->dcb_tx_accm;
        break;

    default:
        rc = -1;
        break;
    }
    return rc;
}

/*!
 * \brief Initialize asynchronous HDLC device.
 *
 * This function will be called during device registration. It
 * initializes the hardware, registers all required interrupt
 * handlers and initializes all internal data structures used by
 * this driver.
 *
 * \param dev  Identifies the device to initialize.
 *
 * \return 0 on success, -1 otherwise.
 */
int AhdlcAvrInit(NUTDEVICE * dev)
{
    int rc = 0;
    AHDLCDCB *dcb;
    uint32_t baudrate = 9600;

    /* Disable UART. */
    AhdlcAvrDisable(dev->dev_base);

    /* Initialize driver control block. */
    dcb = dev->dev_dcb;
    memset(dcb, 0, sizeof(AHDLCDCB));
    dcb->dcb_base = dev->dev_base;
    dcb->dcb_rx_buf = NutHeapAlloc(256);
    dcb->dcb_tx_buf = NutHeapAlloc(256);
    dcb->dcb_rx_mru = 1500;
    dcb->dcb_tx_mru = 1500;
    dcb->dcb_tx_accm = 0xFFFFFFFF;

    /*
     * Initialize UART handshake hardware and register interrupt handlers.
     */
    if (dev->dev_base) {
#ifdef __AVR_ENHANCED__

#ifdef UART1_CTS_BIT
        sbi(UART1_CTS_PORT, UART1_CTS_BIT);
        cbi(UART1_CTS_DDR, UART1_CTS_BIT);
        /* Falling edge will generate interrupts. */
#if UART1_CTS_BIT == 4
        sbi(EICR, 1);
#elif UART1_CTS_BIT == 5
        sbi(EICR, 3);
#elif UART1_CTS_BIT == 6
        sbi(EICR, 5);
#elif UART1_CTS_BIT == 7
        sbi(EICR, 7);
#endif
#endif
#ifdef UART1_RTS_BIT
        sbi(UART1_RTS_PORT, UART1_RTS_BIT);
        sbi(UART1_RTS_DDR, UART1_RTS_BIT);
#endif
#ifdef UART1_DTR_BIT
        sbi(UART1_DTR_PORT, UART1_DTR_BIT);
        sbi(UART1_DTR_DDR, UART1_DTR_BIT);
#endif

        if (NutRegisterIrqHandler(&sig_UART1_RECV, Rx1Complete, dcb))
            rc = -1;
        else if (NutRegisterIrqHandler(&sig_UART1_DATA, Tx1Complete, dcb))
#ifdef UART1_CTS_BIT
            rc = -1;
        else if (NutRegisterIrqHandler(&UART1_CTS_SIGNAL, Cts1Interrupt, dev))
#endif
#endif                          /* __AVR_ENHANCED__ */
            rc = -1;

    } else {

#ifdef UART0_CTS_BIT
        sbi(UART0_CTS_PORT, UART0_CTS_BIT);
        cbi(UART0_CTS_DDR, UART0_CTS_BIT);
#if UART0_CTS_BIT == 4
        sbi(EICR, 1);
#elif UART0_CTS_BIT == 5
        sbi(EICR, 3);
#elif UART0_CTS_BIT == 6
        sbi(EICR, 5);
#elif UART0_CTS_BIT == 7
        sbi(EICR, 7);
#endif
#endif
#ifdef UART0_RTS_BIT
        sbi(UART0_RTS_PORT, UART0_RTS_BIT);
        sbi(UART0_RTS_DDR, UART0_RTS_BIT);
#endif
#ifdef UART0_DTR_BIT
        sbi(UART0_DTR_PORT, UART0_DTR_BIT);
        sbi(UART0_DTR_DDR, UART0_DTR_BIT);
#endif

        if (NutRegisterIrqHandler(&sig_UART0_RECV, Rx0Complete, dcb))
            rc = -1;
        else if (NutRegisterIrqHandler(&sig_UART0_DATA, Tx0Complete, dcb))
            rc = -1;
#ifdef UART0_CTS_BIT
        else if (NutRegisterIrqHandler(&UART0_CTS_SIGNAL, Cts0Interrupt, dev))
            rc = -1;
#endif
    }


    /*
     * If we have been successful so far, start the HDLC receiver thread,
     * set the initial baudrate and enable the UART.
     */
    if (rc == 0 && NutThreadCreate("ahdlcrx", AhdlcRx, dev, 
        (NUT_THREAD_AHDLCRXSTACK * NUT_THREAD_STACK_MULT) + NUT_THREAD_STACK_ADD)) {
        AhdlcAvrIOCtl(dev, UART_SETSPEED, &baudrate);

        return 0;
    }

    /* We failed, clean up. */
    if (dcb->dcb_rx_buf)
        NutHeapFree((void *) dcb->dcb_rx_buf);
    if (dcb->dcb_tx_buf)
        NutHeapFree((void *) dcb->dcb_tx_buf);

    return -1;
}

/*!
 * \brief Read from the asynchronous HDLC device.
 *
 * This function is called by the low level input routines of the
 * \ref xrCrtLowio "C runtime library", using the _NUTDEVICE::dev_read
 * entry.
 *
 * The function may block the calling thread until at least one
 * character has been received or a timeout occurs.
 *
 * It is recommended to set a proper read timeout with software handshake.
 * In this case a timeout may occur, if the communication peer lost our
 * last XON character. The application may then use ioctl() to disable the
 * receiver and do the read again. This will send out another XON.
 *
 * \param fp     Pointer to a \ref _NUTFILE structure, obtained by a
 *               previous call to AhdlcOpen().
 * \param buffer Pointer to the buffer that receives the data. If zero,
 *               then all characters in the input buffer will be
 *               removed.
 * \param size   Maximum number of bytes to read.
 *
 * \return The number of bytes read, which may be less than the number
 *         of bytes specified. A return value of -1 indicates an error,
 *         while zero is returned in case of a timeout.
 */
int AhdlcAvrRead(NUTFILE * fp, void *buffer, int size)
{
    int rc = 0;
    AHDLCDCB *dcb = fp->nf_dev->dev_dcb;
    uint8_t *cp = buffer;

    /*
     * Get characters from receive buffer.
     */
    if (buffer) {
        while (rc < size) {
            if (dcb->dcb_rd_idx != dcb->dcb_rx_idx) {
                *cp++ = dcb->dcb_rx_buf[dcb->dcb_rd_idx++];
                rc++;
            } else if (rc || NutEventWait(&dcb->dcb_rx_rdy, dcb->dcb_rtimeout))
                break;
        }
    }

    /*
     * Call without data buffer discards receive buffer.
     */
    else
        dcb->dcb_rd_idx = dcb->dcb_rx_idx;

    return rc;
}

/*!
 * \brief Write to the asynchronous HDLC device.
 *
 * \param dev    Pointer to a previously registered NUTDEVICE structure.
 * \param buffer Pointer the data to write.
 * \param len    Number of data bytes to write.
 * \param pflg   If this flag is set, then the buffer is located in program
 *               space.
 *
 * \return The number of bytes written. In case of a write timeout, this
 *         may be less than the specified length.
 */
int AhdlcAvrPut(NUTDEVICE * dev, CONST void *buffer, int len, int pflg)
{
    int rc = 0;
    AHDLCDCB *dcb = dev->dev_dcb;
    CONST uint8_t *cp = buffer;

    /*
     * Put characters in transmit buffer.
     */
    if (buffer) {
        while (rc < len) {
            if (SendRawByte(dcb, pflg ? PRG_RDB(cp) : *cp, 0))
                break;
            cp++;
            rc++;
        }
    }

    /*
     * Call without data pointer starts transmission.
     */
    else {
        /*
         * TODO: Check handshake.
         */
#ifdef __AVR_ENHANCED__
        if (dev->dev_base)
            sbi(UCSR1B, UDRIE);
        else
#endif
            sbi(UCR, UDRIE);
    }
    return rc;
}

/*!
 * \brief Write to the asynchronous HDLC device.
 *
 * This function is called by the low level output routines of the
 * \ref xrCrtLowio "C runtime library", using the
 * \ref _NUTDEVICE::dev_write entry.
 *
 * The function may block the calling thread.
 *
 * \param fp     Pointer to a _NUTFILE structure, obtained by a previous
 *               call to AhldcOpen().
 * \param buffer Pointer to the data to be written. If zero, then the
 *               output buffer will be flushed.
 * \param len    Number of bytes to write.
 *
 * \return The number of bytes written, which may be less than the number
 *         of bytes specified if a timeout occured. A return value of -1
 *         indicates an error.
 */
int AhdlcAvrWrite(NUTFILE * fp, CONST void *buffer, int len)
{
    return AhdlcAvrPut(fp->nf_dev, buffer, len, 0);
}

/*!
 * \brief Write to the asynchronous HDLC device.
 *
 * Similar to AhdlcWrite() except that the data is located in program
 * memory.
 *
 * This function is called by the low level output routines of the
 * \ref xrCrtLowio "C runtime library", using the _NUTDEVICE::dev_write_P
 * entry.
 *
 * The function may block the calling thread.
 *
 * \param fp     Pointer to a NUTFILE structure, obtained by a previous
 *               call to AhdlcOpen().
 * \param buffer Pointer to the data in program space to be written.
 * \param len    Number of bytes to write.
 *
 * \return The number of bytes written, which may be less than the number
 *         of bytes specified if a timeout occured. A return value of -1
 *         indicates an error.
 */
int AhdlcAvrWrite_P(NUTFILE * fp, PGM_P buffer, int len)
{
    return AhdlcAvrPut(fp->nf_dev, (CONST char *) buffer, len, 1);
}

/*!
 * \brief Open the asynchronous HDLC device.
 *
 * This function is called by the low level open routine of the C runtime
 * library, using the _NUTDEVICE::dev_open entry.
 *
 * \param dev Pointer to the NUTDEVICE structure.
 * \param name Ignored, should point to an empty string.
 * \param mode Operation mode. Any of the following values may be or-ed:
 * - \ref _O_BINARY
 * - \ref _O_RDONLY
 * - \ref _O_WRONLY
 * \param acc Ignored, should be zero.
 *
 * \return Pointer to a NUTFILE structure if successful or NUTFILE_EOF otherwise.
 */
NUTFILE *AhdlcAvrOpen(NUTDEVICE * dev, CONST char *name, int mode, int acc)
{
    NUTFILE *fp;

    if ((fp = NutHeapAlloc(sizeof(NUTFILE))) == 0)
        return NUTFILE_EOF;

    fp->nf_next = 0;
    fp->nf_dev = dev;
    fp->nf_fcb = 0;

    /* Enable handshake outputs. */
#ifdef __AVR_ENHANCED__
    if (dev->dev_base) {
#ifdef UART1_RTS_BIT
        cbi(UART1_RTS_PORT, UART1_RTS_BIT);
#endif
#ifdef UART1_DTR_BIT
        cbi(UART1_DTR_PORT, UART1_DTR_BIT);
#endif
    } else
#endif                          /* __AVR_ENHANCED__ */
    {
#ifdef UART0_RTS_BIT
        cbi(UART0_RTS_PORT, UART0_RTS_BIT);
#endif
#ifdef UART0_DTR_BIT
        cbi(UART0_DTR_PORT, UART0_DTR_BIT);
#endif
    }
    return fp;
}

/*!
 * \brief Close the asynchronous HDLC device.
 *
 * This function is called by the low level close routine of the C runtime
 * library, using the _NUTDEVICE::dev_close entry.
 *
 * \param fp Pointer to a _NUTFILE structure, obtained by a previous call
 *           to UsartOpen().
 *
 * \return 0 on success or -1 otherwise.
 *
 * \todo We may support shared open and use dev_irq as an open counter.
 */
int AhdlcAvrClose(NUTFILE * fp)
{
    if (fp && fp != NUTFILE_EOF) {
        /* Disable handshake outputs. */
#ifdef __AVR_ENHANCED__
        if (fp->nf_dev->dev_base) {
#ifdef UART1_RTS_BIT
            sbi(UART1_RTS_PORT, UART1_RTS_BIT);
#endif
#ifdef UART1_DTR_BIT
            sbi(UART1_DTR_PORT, UART1_DTR_BIT);
#endif
        } else
#endif                          /* __AVR_ENHANCED__ */
        {
#ifdef UART0_RTS_BIT
            sbi(UART0_RTS_PORT, UART0_RTS_BIT);
#endif
#ifdef UART0_DTR_BIT
            sbi(UART0_DTR_PORT, UART0_DTR_BIT);
#endif
        }
        NutHeapFree(fp);
        return 0;
    }
    return -1;
}

/*@}*/
