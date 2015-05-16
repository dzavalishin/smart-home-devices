#ifndef _ARCH_ARM_AT91_SSC_H_
#define _ARCH_ARM_AT91_SSC_H_

/*
 * Copyright (C) 2006-2007 by egnite Software GmbH. All rights reserved.
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

/*!
 * \file arch/arm/at91_ssc.h
 * \brief AT91 peripherals.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.5  2008/04/18 13:33:14  haraldkipp
 * Corrected definitions of clock selection bits. The new naming is required,
 * but may break existing code.
 *
 * Revision 1.4  2007/02/15 16:20:38  haraldkipp
 * Wrong SSC clock naming broke external clock feeding. Fixed.
 *
 * Revision 1.3  2006/09/29 12:44:17  haraldkipp
 * Just sorted lines a bit.
 *
 * Revision 1.2  2006/08/31 19:12:13  haraldkipp
 * Frame sync definitions added.
 *
 * Revision 1.1  2006/08/05 11:58:54  haraldkipp
 * First release.
 *
 *
 * \endverbatim
 */

/*!
 * \addtogroup xgNutArchArmAt91Ssc
 */
/*@{*/

/*! \name SSC Control Register */
/*@{*/
#define SSC_CR_OFF                  0x00000000  /*!< \brief Control register offset. */
#define SSC_CR      (SSC_BASE + SSC_CR_OFF)     /*!< \brief Control register address. */
#define SSC_RXEN                    0x00000001  /*!< \brief Receive enable. */
#define SSC_RXDIS                   0x00000002  /*!< \brief Receive disable. */
#define SSC_TXEN                    0x00000100  /*!< \brief Transmit enable. */
#define SSC_TXDIS                   0x00000200  /*!< \brief Transmit disable. */
#define SSC_SWRST                   0x00008000  /*!< \brief Software reset. */
/*@}*/

/*! \name SSC Clock Mode Register */
/*@{*/
#define SSC_CMR_OFF                 0x00000004  /*!< \brief Clock mode register offset. */
#define SSC_CMR     (SSC_BASE + SSC_CMR_OFF)    /*!< \brief Clock mode register address. */
#define SSC_DIV_LSB                         0   /*!< \brief Least significant bit of clock divider. */
#define SSC_DIV                     0x00000FFF  /*!< \brief Clock divider. */
/*@}*/

/*! \name SSC Receive/Transmit Clock Mode Register */
/*@{*/
#define SSC_RCMR_OFF                0x00000010  /*!< \brief Receive clock mode register offset. */
#define SSC_RCMR    (SSC_BASE + SSC_RCMR_OFF)   /*!< \brief Receive clock mode register address. */
#define SSC_TCMR_OFF                0x00000018  /*!< \brief Transmit clock mode register offset. */
#define SSC_TCMR    (SSC_BASE + SSC_TCMR_OFF)   /*!< \brief Transmit clock mode register address. */

#define SSC_CKS                     0x00000003  /*!< \brief Receive clock selection. */
#define SSC_CKS_DIV                 0x00000000  /*!< \brief Divided clock. */
#define SSC_CKS_CLK                 0x00000001  /*!< \brief RK/TK clock signal. */
#define SSC_CKS_PIN                 0x00000002  /*!< \brief TK/RK pin. */
#define SSC_CKO                     0x0000001C  /*!< \brief Receive clock output mode selection. */
#define SSC_CKO_NONE                0x00000000  /*!< \brief None. */
#define SSC_CKO_CONT                0x00000004  /*!< \brief Continous receive clock. */
#define SSC_CKO_TRAN                0x00000008  /*!< \brief Receive clock only during data transfers. */
#define SSC_CKI                     0x00000020  /*!< \brief Receive clock inversion. */
#define SSC_CKG                     0x000000C0  /*!< \brief Receive clock gating selection. */
#define SSC_CKG_NONE                0x00000000  /*!< \brief None, continous clock. */
#define SSC_CKG_RFL                 0x00000040  /*!< \brief Continous receive clock. */
#define SSC_CKG_RFH                 0x00000080  /*!< \brief Receive clock only during data transfers. */
#define SSC_START                   0x00000F00  /*!< \brief Receive start selection. */
#define SSC_START_CONT              0x00000000  /*!< \brief Receive start as soon as enabled. */
#define SSC_START_TX                0x00000100  /*!< \brief Receive start on transmit start. */
#define SSC_START_LOW_RF            0x00000200  /*!< \brief Receive start on low level RF. */
#define SSC_START_HIGH_RF           0x00000300  /*!< \brief Receive start on high level RF. */
#define SSC_START_FALL_RF           0x00000400  /*!< \brief Receive start on falling edge RF. */
#define SSC_START_RISE_RF           0x00000500  /*!< \brief Receive start on rising edge RF. */
#define SSC_START_LEVEL_RF          0x00000600  /*!< \brief Receive start on any RF level change. */
#define SSC_START_EDGE_RF           0x00000700  /*!< \brief Receive start on any RF edge. */
#define SSC_START_COMP0             0x00000800  /*!< \brief Receive on compare 0. */
#define SSC_STOP                    0x00001000  /*!< \brief Receive stop selection. */
#define SSC_STTDLY                  0x00FF0000  /*!< \brief Receive start delay. */
#define SSC_STTDLY_LSB                      16  /*!< \brief Least significant bit of receive start delay. */
#define SSC_PERIOD                  0xFF000000  /*!< \brief Receive period divider selection. */
#define SSC_PERIOD_LSB                      24  /*!< \brief Least significant bit of receive period divider selection. */
/*@}*/

/*! \name SSC Receive/Transmit Frame Mode Registers */
/*@{*/
#define SSC_RFMR_OFF                0x00000014  /*!< \brief Receive frame mode register offset. */
#define SSC_RFMR    (SSC_BASE + SSC_RFMR_OFF)   /*!< \brief Receive frame mode register address. */
#define SSC_TFMR_OFF                0x0000001C  /*!< \brief Transmit frame mode register offset. */
#define SSC_TFMR    (SSC_BASE + SSC_TFMR_OFF)   /*!< \brief Transmit frame mode register address. */

#define SSC_DATLEN                  0x0000001F  /*!< \brief Data length. */
#define SSC_DATLEN_LSB                      0   /*!< \brief Least significant bit of data length. */
#define SSC_LOOP                    0x00000020  /*!< \brief Receiver loop mode. */
#define SSC_DATDEF                  0x00000020  /*!< \brief Transmit default value. */
#define SSC_MSBF                    0x00000080  /*!< \brief Most significant bit first. */
#define SSC_DATNB                   0x00000F00  /*!< \brief Data number per frame. */
#define SSC_DATNB_LSB                       8   /*!< \brief Least significant bit of data number per frame. */
#define SSC_FSLEN                   0x000F0000  /*!< \brief Receive frame sync. length. */
#define SSC_FSLEN_LSB                       16  /*!< \brief Least significant bit of receive frame sync. length. */
#define SSC_FSOS                    0x00700000  /*!< \brief Receive frame sync. output selection. */
#define SSC_FSOS_NONE               0x00000000  /*!< \brief No frame sync. Line set to input. */
#define SSC_FSOS_NEGATIVE           0x00100000  /*!< \brief Negative pulse. */
#define SSC_FSOS_POSITIVE           0x00200000  /*!< \brief Positive pulse. */
#define SSC_FSOS_LOW                0x00300000  /*!< \brief Low during transfer. */
#define SSC_FSOS_HIGH               0x00400000  /*!< \brief High during transfer. */
#define SSC_FSOS_TOGGLE             0x00500000  /*!< \brief Toggling at each start. */
#define SSC_FSDEN                   0x00800000  /*!< \brief Frame sync. data enable. */
#define SSC_FSEDGE                  0x01000000  /*!< \brief Frame sync. edge detection. */
/*@}*/

/*! \name SSC Receive Holding Register */
/*@{*/
#define SSC_RHR_OFF                 0x00000020  /*!< \brief Receive holding register offset. */
#define SSC_RHR     (SSC_BASE + SSC_RHR_OFF)    /*!< \brief Receive holding register address. */
/*@}*/

/*! \name SSC Transmit Holding Register */
/*@{*/
#define SSC_THR_OFF                 0x00000024  /*!< \brief Transmit holding register offset. */
#define SSC_THR     (SSC_BASE + SSC_THR_OFF)    /*!< \brief Transmit holding register address. */
/*@}*/

/*! \name SSC Receive Sync. Holding Register */
/*@{*/
#define SSC_RSHR_OFF                0x00000030  /*!< \brief Receive sync. holding register offset. */
#define SSC_RSHR    (SSC_BASE + SSC_RSHR_OFF)   /*!< \brief Receive sync. holding register address. */
/*@}*/

/*! \name SSC Transmit Sync. Holding Register */
/*@{*/
#define SSC_TSHR_OFF                0x00000034  /*!< \brief Transmit sync. holding register offset. */
#define SSC_TSHR    (SSC_BASE + SSC_TSHR_OFF)   /*!< \brief Transmit sync. holding register address. */
/*@}*/

/*! \name SSC Receive Compare 0 Register */
/*@{*/
#define SSC_RC0R_OFF                0x00000038  /*!< \brief Receive compare 0 register offset. */
#define SSC_RC0R    (SSC_BASE + SSC_RC0R_OFF)   /*!< \brief Receive compare 0 register address. */
/*@}*/

/*! \name SSC Receive Compare 1 Register */
/*@{*/
#define SSC_RC1R_OFF                0x0000003C  /*!< \brief Receive compare 1 register offset. */
#define SSC_RC1R    (SSC_BASE + SSC_RC1R_OFF)   /*!< \brief Receive compare 1 register address. */
/*@}*/

/*! \name SSC Status Register */
/*@{*/
#define SSC_SR_OFF                  0x00000040  /*!< \brief Status register offset. */
#define SSC_SR      (SSC_BASE + SSC_SR_OFF)     /*!< \brief Status register address. */
#define SSC_TXRDY                   0x00000001  /*!< \brief Transmit ready. */
#define SSC_TXEMPTY                 0x00000002  /*!< \brief Transmit empty. */
#define SSC_ENDTX                   0x00000004  /*!< \brief End of transmission. */
#define SSC_TXBUFE                  0x00000008  /*!< \brief Transmit buffer empty. */
#define SSC_RXRDY                   0x00000010  /*!< \brief Receive ready. */
#define SSC_OVRUN                   0x00000020  /*!< \brief Receive overrun. */
#define SSC_ENDRX                   0x00000040  /*!< \brief End of receiption. */
#define SSC_RXBUFF                  0x00000080  /*!< \brief Receive buffer full. */
#define SSC_CP0                     0x00000100  /*!< \brief Compare 0. */
#define SSC_CP1                     0x00000200  /*!< \brief Compare 1. */
#define SSC_TXSYN                   0x00000400  /*!< \brief Transmit sync. */
#define SSC_RXSYN                   0x00000800  /*!< \brief Receive sync. */
#define SSC_TXENA                   0x00010000  /*!< \brief Transmit enable. */
#define SSC_RXENA                   0x00020000  /*!< \brief Receive enable. */
/*@}*/

/*! \name SSC Interrupt Enable Register */
/*@{*/
#define SSC_IER_OFF                 0x00000044  /*!< \brief Interrupt enable register offset. */
#define SSC_IER     (SSC_BASE + SSC_IER_OFF)    /*!< \brief Interrupt enable register address. */
/*@}*/

/*! \name SSC Interrupt Disable Register */
/*@{*/
#define SSC_IDR_OFF                 0x00000048  /*!< \brief Interrupt disable register offset. */
#define SSC_IDR     (SSC_BASE + SSC_IDR_OFF)    /*!< \brief Interrupt disable register address. */
/*@}*/

/*! \name SSC Interrupt Mask Register */
/*@{*/
#define SSC_IMR_OFF                 0x0000004C  /*!< \brief Interrupt mask register offset. */
#define SSC_IMR     (SSC_BASE + SSC_IMR_OFF)    /*!< \brief Interrupt mask register address. */
/*@}*/

#if defined(SSC_HAS_PDC)

/*! \name SSC Receive Pointer Register */
/*@{*/
#define SSC_RPR    (SSC_BASE + PERIPH_RPR_OFF)  /*!< \brief PDC receive pointer register address. */
/*@}*/

/*! \name SSC Receive Counter Register */
/*@{*/
#define SSC_RCR    (SSC_BASE + PERIPH_RCR_OFF)  /*!< \brief PDC receive counter register address. */
/*@}*/

/*! \name SSC Transmit Pointer Register */
/*@{*/
#define SSC_TPR    (SSC_BASE + PERIPH_TPR_OFF)  /*!< \brief PDC transmit pointer register address. */
/*@}*/

/*! \name SSC Transmit Counter Register */
/*@{*/
#define SSC_TCR    (SSC_BASE + PERIPH_TCR_OFF)  /*!< \brief PDC transmit counter register address. */
/*@}*/

/*! \name SSC Receive Next Pointer Register */
/*@{*/
#define SSC_RNPR   (SSC_BASE + PERIPH_RNPR_OFF) /*!< \brief PDC receive next pointer register address. */
/*@}*/

/*! \name SSC Receive Next Counter Register */
/*@{*/
#define SSC_RNCR   (SSC_BASE + PERIPH_RNCR_OFF) /*!< \brief PDC receive next counter register address. */
/*@}*/

/*! \name SSC Transmit Next Pointer Register */
/*@{*/
#define SSC_TNPR   (SSC_BASE + PERIPH_TNPR_OFF) /*!< \brief PDC transmit next pointer register address. */
/*@}*/

/*! \name SSC Transmit Next Counter Register */
/*@{*/
#define SSC_TNCR   (SSC_BASE + PERIPH_TNCR_OFF) /*!< \brief PDC transmit next counter register address. */
/*@}*/

/*! \name SSC Transfer Control Register */
/*@{*/
#define SSC_PTCR   (SSC_BASE + PERIPH_PTCR_OFF) /*!< \brief PDC transfer control register address. */
/*@}*/

/*! \name SSC Transfer Status Register */
/*@{*/
#define SSC_PTSR   (SSC_BASE + PERIPH_PTSR_OFF) /*!< \brief PDC transfer status register address. */
/*@}*/

#endif

/*@} xgNutArchArmAt91Ssc */


#endif                          /* _ARCH_ARM_AT91_SSC_H_ */
