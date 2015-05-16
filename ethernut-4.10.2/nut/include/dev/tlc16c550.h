#ifndef _DEV_TLC16C550_H_
#define _DEV_TLC16C550_H_

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
 */

/*
 * $Log$
 * Revision 1.5  2009/01/17 11:26:47  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.4  2008/08/11 06:59:59  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.3  2007/05/24 07:29:10  haraldkipp
 * Update provided by Przemyslaw Rudy.
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

/*
	Chip connections:
	Ax - to avr address lines
	CSx - to avr/PGA (chip select)
    Dn - to avr data lines
    INTN - to vcc (for 16c554 forces activation of interrupt lines)
    INTx - to avr interrupt lines (can be OR-ed and share common avr interrupt line)
	OIR - to avr /rd
	OIW - to avr /wr
    RESET - high is active
 */

#include <sys/device.h>
#include <dev/ace.h>

/*!
 * \file dev/tlc16c550.h
 * \brief ACE ACE definitions.
 */

/*!
 * \addtogroup xgAceDriver
 */
/*@{*/

#define ACE_MF_RTSSENSE		0x00000001UL    /*!< DCE input, low on port bit is +12V, which means ON. */
#define ACE_MF_CTSCONTROL	0x00000002UL    /*!< DCE output. */
#define ACE_MF_DTRSENSE		0x00000004UL    /*!< DCE input. */
#define ACE_MF_DSRCONTROL	0x00000008UL    /*!< DCE output. */
#define ACE_MF_DCDCONTROL	0x00000010UL    /*!< DCE output. */

#define ACE_MF_RTSCONTROL	0x00000020UL    /*!< DTE output. */
#define ACE_MF_CTSSENSE		0x00000040UL    /*!< DTE input. */
#define ACE_MF_DTRCONTROL	0x00000080UL    /*!< DTE output. */
#define ACE_MF_DSRSENSE		0x00000100UL    /*!< DTE input. */
#define ACE_MF_DCDSENSE		0x00000200UL    /*!< DTE input. */

#define ACE_MF_SENSEMASK	0x0345  /*!< Handshake sense mask. */
#define ACE_MF_CONTROLMASK	0x00BC  /*!< Handshake control mask. */

#define ACE_MF_XONXOFF		0x00000400UL    /*!< Software handshake. */

#define ACE_MF_HALFDUPLEX   0x00000800UL    /*!< Half duplex bit control */

#define ACE_MF_LOCALECHO	0x00010000UL    /*!< Should be used in stream, not device. */
#define ACE_MF_COOKEDMODE	0x00020000UL    /*!< Should be used in stream, not device. */

#define ACE_MF_NOBUFFER		0x00100000UL    /*!< No buffering. */
#define ACE_MF_LINEBUFFER	0x00200000UL    /*!< Line buffered. */
#define ACE_MF_BUFFERMASK	0x00300000UL    /*!< Masks buffering mode flags. */


#define ACE_SF_RTSOFF		0x00000001UL    /*!< Set RTS line is off. */
#define ACE_SF_CTSOFF	    0x00000002UL        /*!< Set CTS line is off. */
#define ACE_SF_DTROFF	    0x00000004UL        /*!< Set DTR line is off. */
#define ACE_SF_DSROFF	    0x00000008UL        /*!< Set DSR line is off. */
#define ACE_SF_DCDOFF	    0x00000010UL        /*!< Set DCD line is off. */

#define ACE_SF_TXDISABLED	0x00000040UL    /*!< Transmitter disabled. */
#define ACE_SF_RXDISABLED	0x00000080UL    /*!< Receiver disabled. */

#define ACE_HS_DCERTSCTS	0x00000003UL    /*!< RTS/CTS DCE handshake. */
#define ACE_HS_DCEFULL	    0x0000001FUL        /*!< Full DCE handshake. */

#define ACE_HS_DTERTSCTS	0x00000060UL    /*!< RTS/CTS DTE handshake. */
#define ACE_HS_DTEFULL	    0x000003E0UL        /*!< Full DTE handshake. */

#define ACE_HS_XONXOFF	    0x00000400UL        /*!< Software handshake. */

#ifndef ACE_CLOCK
    #define ACE_CLOCK           14745600UL     /* in Hz - common for all devices (should be ok) */
#endif

#define ACE_FIFO_SIZE	16 /* hardware fifo size */

/* define ACE_HDX_LINE to DTR or RTS to use HDX functionality */
#ifdef ACE_HDX_LINE
#undef ACE_HDX_LINE
#endif
#ifdef ACE_HDX_USE_RTS
	#define ACE_HDX_LINE MCR_RTS_MSK
#endif
#ifdef ACE_HDX_USE_DTR
	#define ACE_HDX_LINE MCR_DTR_MSK
#endif	
#ifdef ACE_HDX_LINE
    #ifdef ACE_HDX_LINE_FLIP
        #define ACE_HDX_RECEIVE(base) *(uint8_t *) ((base) + ACE_MCR_OFS) &= ~ACE_HDX_LINE
        #define ACE_HDX_TRANSMIT(base) *(uint8_t *) ((base) + ACE_MCR_OFS) |= ACE_HDX_LINE
		#define ACE_HDX_IS_TRANSMIT(base) (*(uint8_t *) ((base) + ACE_MCR_OFS) & ACE_HDX_LINE)
    #else
        #define ACE_HDX_RECEIVE(base) *(uint8_t *) ((base) + ACE_MCR_OFS) |= ACE_HDX_LINE
        #define ACE_HDX_TRANSMIT(base) *(uint8_t *) ((base) + ACE_MCR_OFS) &= ~ACE_HDX_LINE
		#define ACE_HDX_IS_TRANSMIT(base) (!(*(uint8_t *) ((base) + ACE_MCR_OFS) & ACE_HDX_LINE))
    #endif
#endif

/*!
 * ACE device control block type.
 */
typedef struct _ACEDCB ACEDCB;

/*!
 * \struct _ACEDCB tlc16c550.h dev/tlc16c550.h
 * \brief ACE device control block structure.
 */
struct _ACEDCB {
    /*! \brief pointer to the next device, used if more devices share the same interrupt */
    NUTDEVICE *dev_next;

    /*! \brief Read timeout.
     */
    uint32_t dcb_rtimeout;

    /*! \brief Write timeout.
     */
    uint32_t dcb_wtimeout;

    /*! \brief Queue of threads waiting for output buffer empty.
     *
     * Threads are added to this queue when calling ACEAvrFlush(). 
     */
    HANDLE dcb_tx_rdy;

    /*! \brief Queue of threads waiting for a character in the input buffer.
     *
     * Threads are added to this queue when calling ACEAvrInput(). 
     */
    HANDLE dcb_rx_rdy;

    /*! \brief Mode flags.
     */
    uint32_t dcb_modeflags;

    /*! \brief Level of the fifo trigger, 0 if no fifo.
     */
    uint8_t dcb_rfifo;

    /*! \brief Free space in the output fifo since the last write operation.
     */
    uint8_t dcb_wfifo;
#ifdef ACE_HDX_LINE
    /*! \brief One byte time delay after which HDX pin will be off, in OCR register format.
     */	 
	unsigned int hdxByteTime;
	
    /*! \brief OCR register value at which HDX pin will be off, 0 if not used.
     */	 
	unsigned int hdxOcrTime;
#endif
};

/*@}*/

#endif
