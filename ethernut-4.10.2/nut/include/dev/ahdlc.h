#ifndef _DEV_AHDLC_H_
#define _DEV_AHDLC_H_

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

/*
 * $Log$
 * Revision 1.2  2008/08/11 06:59:59  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.1.1.1  2003/05/09 14:41:05  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.2  2003/05/06 18:40:28  harald
 * Unused items removed
 *
 * Revision 1.1  2003/03/31 14:53:23  harald
 * Prepare release 3.1
 *
 */

#include <dev/netbuf.h>
#include <net/if_var.h>


/*!
 * \file dev/ahdlc.h
 * \brief Asynchronous HDLC device definitions.
 */

/*
 * Significant octet values.
 */
#define AHDLC_ALLSTATIONS 0xff    /*!< \brief All-Stations broadcast address */
#define AHDLC_UI          0x03    /*!< \brief Unnumbered Information */
#define AHDLC_FLAG        0x7e    /*!< \brief Flag Sequence */
#define AHDLC_ESCAPE      0x7d    /*!< \brief Asynchronous Control Escape */
#define AHDLC_TRANS       0x20    /*!< \brief Asynchronous transparency modifier */

/*
 * Values for FCS calculations.
 */
#define AHDLC_INITFCS     0xffff  /*!< \brief Initial FCS value */
#define AHDLC_GOODFCS     0xf0b8  /*!< \brief Good final FCS value */

/*!
 * \struct _AHDLCDCB ahdlc.h dev/ahdlc.h
 * \brief Asynchronous HDLC device information structure.
 *
 * The start of this structure is equal to the UARTDCB structure.
 */
struct _AHDLCDCB {
    
    /*! \brief Mode flags.
     */
    uint32_t dcb_modeflags;

    /*! \brief Status flags.
     */
    uint32_t dcb_statusflags;

    /*! \brief Read timeout.
     */
    uint32_t dcb_rtimeout;

    /*! \brief Write timeout.
     */
    uint32_t dcb_wtimeout;

    /*! \brief Queue of threads waiting for output buffer empty.
     *
     * Threads are added to this queue when the output buffer 
     * is full or when flushing the output buffer.
     */
    HANDLE dcb_tx_rdy;

    /*! \brief Queue of threads waiting for a character in the input buffer.
     *
     * Threads are added to this queue when the output buffer is
     * empty.
     */
    HANDLE dcb_rx_rdy;

    /*! \brief Hardware base address.
     * 
     * This is a copy of the base address in the NUTDEVICE structure
     * and required by the interrupt routine.
     */
    uint8_t dcb_base;

    /*! \brief Input buffer. 
     *
     * This buffer is filled by the the receiver interrupt, so the
     * contents of the buffer is volatile.
     */
    volatile uint8_t *dcb_rx_buf;
    
    /*! \brief Input buffer index for next incoming byte.
     *
     * This volatile index is incremented by the receiver interrupt.
     */
    volatile uint8_t dcb_rx_idx;

    /*! \brief Input buffer index for next byte to read.
     */
    uint8_t dcb_rd_idx;

    /*! \brief Output buffer. 
     */
    uint8_t *dcb_tx_buf;

    /*! \brief Output buffer index for next outgoing byte. 
     *
     * This volatile index is incremented by the transmit interrupt.
     */
    volatile uint8_t dcb_tx_idx;

    /*! \brief Output buffer index for next byte to write. 
     */
    uint8_t dcb_wr_idx;

    /*! \brief HDLC mode change event queue.
     *
     * The frame receiver thread is waiting on this queue until
     * the device is switched to HDLC mode.
     */
    HANDLE dcb_mf_evt;

    /*! \brief 32-bit receive ACCM.
     */
    uint32_t dcb_rx_accm;         

    /*! \brief 256-bit transmit ACCM.
     */
    uint32_t dcb_tx_accm;

    /*! \brief Maximum receive MRU.
     */
    uint16_t dcb_rx_mru;

    /*! \brief Maximum transmit MRU.
     */
    uint16_t dcb_tx_mru;

};

/*!
 * \brief Asynchronous HDLC device information type.
 */
typedef struct _AHDLCDCB AHDLCDCB;


#endif

