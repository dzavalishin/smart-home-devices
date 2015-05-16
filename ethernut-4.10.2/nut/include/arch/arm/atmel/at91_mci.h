#ifndef _ARCH_ARM_AT91_MCI_H_
#define _ARCH_ARM_AT91_MCI_H_

/*
 * Copyright (C) 2006 by egnite Software GmbH. All rights reserved.
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
 * \file arch/arm/at91_mci.h
 * \brief AT91 peripherals.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.3  2008/10/03 11:29:51  haraldkipp
 * MCI_OPCMD should have been MCI_OPDCMD. The wrong one will be kept for
 * backward compatibility.
 *
 * Revision 1.2  2006/09/05 12:32:13  haraldkipp
 * Timeout multiplier settings renamed to follow Atmel's convention.
 * 4-bit bus SDC setting corrected.
 * Several comments added or corrected.
 *
 * Revision 1.1  2006/08/31 19:10:37  haraldkipp
 * New peripheral register definitions for the AT91SAM9260.
 *
 *
 * \endverbatim
 */

/*!
 * \addtogroup xgNutArchArmAt91Mci
 */
/*@{*/

/*! \name MMC Control Register */
/*@{*/
#define MCI_CR_OFF              0x00000000      /*!< \brief Control register offset. */
#define MCI_CR      (MCI_BASE + MCI_CR_OFF)     /*!< \brief Control register address. */
#define MCI_MCIEN               0x00000001      /*!< \brief Interface enable. */
#define MCI_MCIDIS              0x00000002      /*!< \brief Interface disable. */
#define MCI_PWSEN               0x00000004      /*!< \brief Power save mode enable. */
#define MCI_PWSDIS              0x00000008      /*!< \brief Power save mode disable. */
#define MCI_SWRST               0x00000080      /*!< \brief Software reset. */
/*@}*/

/*! \name MMC Mode Register */
/*@{*/
#define MCI_MR_OFF              0x00000004      /*!< \brief Mode register offset. */
#define MCI_MR      (MCI_BASE + MCI_MR_OFF)     /*!< \brief Mode register address. */
#define MCI_CLKDIV              0x000000FF      /*!< \brief Clock divider mask. */
#define MCI_CLKDIV_LSB                  0       /*!< \brief Clock divider LSB. */
#define MCI_PWSDIV              0x00000700      /*!< \brief Power saving divider mask. */
#define MCI_PWSDIV_LSB                  8       /*!< \brief Power saving divider LSB. */
#define MCI_RDPROOF             0x00000800      /*!< \brief Enable read proof. */
#define MCI_WRPROOF             0x00001000      /*!< \brief Enable write proof. */
#define MCI_PDCFBYTE            0x00002000      /*!< \brief Force PDC byte transfer. */
#define MCI_PDCPADV             0x00004000      /*!< \brief PDC padding value. */
#define MCI_PDCMODE             0x00008000      /*!< \brief PDC-oriented mode. */
#define MCI_BLKLEN              0xFFFF0000      /*!< \brief Data block length mask. */
#define MCI_BLKLEN_LSB                  16      /*!< \brief Data block length LSB. */
/*@}*/

/*! \name MMC Data Timeout Register */
/*@{*/
#define MCI_DTOR_OFF            0x00000008      /*!< \brief Data timeout register offset. */
#define MCI_DTOR    (MCI_BASE + MCI_DTOR_OFF)   /*!< \brief Data timeout register address. */
#define MCI_DTOCYC              0x0000000F      /*!< \brief Data timeout cycle number mask. */
#define MCI_DTOCYC_LSB                  0       /*!< \brief Data timeout cycle number LSB. */
#define MCI_DTOMUL              0x00000070      /*!< \brief Data timeout multiplier mask. */
#define MCI_DTOMUL_1            0x00000000      /*!< \brief Data timeout multiplier 1. */
#define MCI_DTOMUL_16           0x00000010      /*!< \brief Data timeout multiplier 16. */
#define MCI_DTOMUL_128          0x00000020      /*!< \brief Data timeout multiplier 128. */
#define MCI_DTOMUL_256          0x00000030      /*!< \brief Data timeout multiplier 256. */
#define MCI_DTOMUL_1K           0x00000040      /*!< \brief Data timeout multiplier 1024. */
#define MCI_DTOMUL_4K           0x00000050      /*!< \brief Data timeout multiplier 4096. */
#define MCI_DTOMUL_64K          0x00000060      /*!< \brief Data timeout multiplier 65536. */
#define MCI_DTOMUL_1M           0x00000070      /*!< \brief Data timeout multiplier 1048576. */
/*@}*/

/*! \name MMC SDCard/SDIO Register */
/*@{*/
#define MCI_SDCR_OFF            0x0000000C      /*!< \brief SDC/SDIO register offset. */
#define MCI_SDCR    (MCI_BASE + MCI_SDCR_OFF)   /*!< \brief SDC/SDIO register address. */
#define MCI_SDCSEL              0x00000003      /*!< \brief SDC/SDIO slot mask. */
#define MCI_SDCSEL_SLOTA        0x00000000      /*!< \brief Slot A selected. */
#define MCI_SDCSEL_SLOTB        0x00000001      /*!< \brief Slot B selected. */
#define MCI_SDCBUS              0x00000080      /*!< \brief SDC/SDIO 4-bit bus. */
/*@}*/

/*! \name MMC Argument Register */
/*@{*/
#define MCI_ARGR_OFF            0x00000010      /*!< \brief Argument register offset. */
#define MCI_ARGR    (MCI_BASE + MCI_ARGR_OFF)   /*!< \brief Argument register address. */
/*@}*/

/*! \name MMC Command Register */
/*@{*/
#define MCI_CMDR_OFF            0x00000014      /*!< \brief Command register offset. */
#define MCI_CMDR    (MCI_BASE + MCI_CMDR_OFF)   /*!< \brief Command register address. */
#define MCI_CMDNB               0x0000003F      /*!< \brief Command number mask. */
#define MCI_CMDNB_LSB                   0       /*!< \brief Command number LSB. */
#define MCI_RSPTYP              0x000000C0      /*!< \brief Response type mask. */
#define MCI_RSPTYP_NONE         0x00000000      /*!< \brief No response. */
#define MCI_RSPTYP_48           0x00000040      /*!< \brief 48-bit response. */
#define MCI_RSPTYP_136          0x00000080      /*!< \brief 136-bit response. */
#define MCI_SPCMD               0x00000700      /*!< \brief Special command mask. */
#define MCI_SPCMD_NONE          0x00000000      /*!< \brief Not a special command. */
#define MCI_SPCMD_INIT          0x00000100      /*!< \brief Initialization command. */
#define MCI_SPCMD_SYNC          0x00000200      /*!< \brief Synchronized command. */
#define MCI_SPCMD_ICMD          0x00000400      /*!< \brief Interrupt command. */
#define MCI_SPCMD_IRSP          0x00000500      /*!< \brief Interrupt response. */
#define MCI_OPDCMD              0x00000800      /*!< \brief Open drain command. */
#define MCI_OPCMD               MCI_OPDCMD      /*!< \brief Open drain command. Deprecated spelling. */
#define MCI_MAXLAT              0x00001000      /*!< \brief Maximum latency for command to response. */
#define MCI_TRCMD               0x00030000      /*!< \brief Transfer command mask. */
#define MCI_TRCMD_NONE          0x00000000      /*!< \brief No data transfer. */
#define MCI_TRCMD_START         0x00010000      /*!< \brief Start data transfer. */
#define MCI_TRCMD_STOP          0x00020000      /*!< \brief Stop data transfer. */
#define MCI_TRDIR               0x00040000      /*!< \brief Read transfer. */
#define MCI_TRTYP               0x00380000      /*!< \brief Transfer type mask. */
#define MCI_TRTYP_MMC_SBLK      0x00000000      /*!< \brief MMC/SDC single block transfer. */
#define MCI_TRTYP_MMC_MBLK      0x00080000      /*!< \brief MMC/SDC multiple block transfer. */
#define MCI_TRTYP_MMC_STREAM    0x00100000      /*!< \brief MMC stream transfer. */
#define MCI_TRTYP_SDIO_BYTE     0x00200000      /*!< \brief SDIO byte transfer. */
#define MCI_TRTYP_SDIO_BLK      0x00280000      /*!< \brief SDIO block transfer. */
#define MCI_IOSPCMD             0x03000000      /*!< \brief Specila SDIO command mask. */
#define MCI_IOSPCMD_NONE        0x00000000      /*!< \brief Not a special SDIO command. */
#define MCI_IOSPCMD_SUSPEND     0x01000000      /*!< \brief SDIO suspend command. */
#define MCI_IOSPCMD_RESUME      0x02000000      /*!< \brief SDIO resume command. */
/*@}*/

/*! \name MMC Block Register */
/*@{*/
#define MCI_BLKR_OFF            0x00000018      /*!< \brief Block register offset. */
#define MCI_BLKR    (MCI_BASE + MCI_BLKR_OFF)   /*!< \brief Block register address. */
#define MCI_BCNT                0x0000FFFF      /*!< \brief MMC/SDIO block count or SDIO byte count mask. */
#define MCI_BCNT_LSB                    0       /*!< \brief MMC/SDIO block count or SDIO byte count LSB. */
/*@}*/

/*! \name MMC Response Register */
/*@{*/
#define MCI_RSPR_OFF            0x00000020      /*!< \brief Response register offset. */
#define MCI_RSPR    (MCI_BASE + MCI_RSPR_OFF)   /*!< \brief Response register address. */
/*@}*/

/*! \name MMC Receive Data Register */
/*@{*/
#define MCI_RDR_OFF             0x00000030      /*!< \brief Receive data register offset. */
#define MCI_RDR     (MCI_BASE + MCI_RDR_OFF)    /*!< \brief Receive data register address. */
/*@}*/

/*! \name MMC Transmit Data Register */
/*@{*/
#define MCI_TDR_OFF             0x00000034      /*!< \brief Transmit data register offset. */
#define MCI_TDR     (MCI_BASE + MCI_TDR_OFF)    /*!< \brief Transmit data register address. */
/*@}*/

/*! \name MMC Interrupt and Status Registers */
/*@{*/
#define MCI_SR_OFF              0x00000040      /*!< \brief Status register offset. */
#define MCI_SR      (MCI_BASE + MCI_SR_OFF)     /*!< \brief Status register address. */

#define MCI_IER_OFF             0x00000044      /*!< \brief Enable register offset. */
#define MCI_IER     (MCI_BASE + MCI_IER_OFF)    /*!< \brief Enable register address. */

#define MCI_IDR_OFF             0x00000048      /*!< \brief Disable register offset. */
#define MCI_IDR     (MCI_BASE + MCI_IDR_OFF)    /*!< \brief Disable register address. */

#define MCI_IMR_OFF             0x0000004C      /*!< \brief Mask register offset. */
#define MCI_IMR     (MCI_BASE + MCI_IMR_OFF)    /*!< \brief Mask register address. */

#define MCI_CMDRDY              0x00000001      /*!< \brief Command ready. */
#define MCI_RXRDY               0x00000002      /*!< \brief Receiver ready. */
#define MCI_TXRDY               0x00000004      /*!< \brief Transmit ready. */
#define MCI_BLKE                0x00000008      /*!< \brief Data block ended. */
#define MCI_DTIP                0x00000010      /*!< \brief Data transfer in progress. */
#define MCI_NOTBUSY             0x00000020      /*!< \brief MCI not busy. */
#define MCI_ENDRX               0x00000040      /*!< \brief End of receive buffer. */
#define MCI_ENDTX               0x00000080      /*!< \brief End of transmit buffer. */
#define MCI_SDIOIRQA            0x00000100      /*!< \brief Undocumented. */
#define MCI_SDIOIRQB            0x00000200      /*!< \brief Undocumented. */
#define MCI_RXBUFF              0x00004000      /*!< \brief Receive buffer full. */
#define MCI_TXBUFE              0x00008000      /*!< \brief Transmit buffer empty. */
#define MCI_RINDE               0x00010000      /*!< \brief Response index error. */
#define MCI_RDIRE               0x00020000      /*!< \brief Response direction error. */
#define MCI_RCRCE               0x00040000      /*!< \brief Response CRC error. */
#define MCI_RENDE               0x00080000      /*!< \brief Response end bit error. */
#define MCI_RTOE                0x00100000      /*!< \brief Response timeout error. */
#define MCI_DCRCE               0x00200000      /*!< \brief Data CRC error. */
#define MCI_DTOE                0x00400000      /*!< \brief Date timeout error. */
#define MCI_OVRE                0x40000000      /*!< \brief Overrun error. */
#define MCI_UNRE                0x80000000      /*!< \brief Underrun error. */

/*@}*/

#if defined(MCI_HAS_PDC)

/*! \name SSC Receive Pointer Register */
/*@{*/
#define MCI_RPR    (MCI_BASE + PERIPH_RPR_OFF)  /*!< \brief PDC receive pointer register address. */
/*@}*/

/*! \name SSC Receive Counter Register */
/*@{*/
#define MCI_RCR    (MCI_BASE + PERIPH_RCR_OFF)  /*!< \brief PDC receive counter register address. */
/*@}*/

/*! \name SSC Transmit Pointer Register */
/*@{*/
#define MCI_TPR    (MCI_BASE + PERIPH_TPR_OFF)  /*!< \brief PDC transmit pointer register address. */
/*@}*/

/*! \name SSC Transmit Counter Register */
/*@{*/
#define MCI_TCR    (MCI_BASE + PERIPH_TCR_OFF)  /*!< \brief PDC transmit counter register address. */
/*@}*/

/*! \name SSC Receive Next Pointer Register */
/*@{*/
#define MCI_RNPR   (MCI_BASE + PERIPH_RNPR_OFF) /*!< \brief PDC receive next pointer register address. */
/*@}*/

/*! \name SSC Receive Next Counter Register */
/*@{*/
#define MCI_RNCR   (MCI_BASE + PERIPH_RNCR_OFF) /*!< \brief PDC receive next counter register address. */
/*@}*/

/*! \name SSC Transmit Next Pointer Register */
/*@{*/
#define MCI_TNPR   (MCI_BASE + PERIPH_TNPR_OFF) /*!< \brief PDC transmit next pointer register address. */
/*@}*/

/*! \name SSC Transmit Next Counter Register */
/*@{*/
#define MCI_TNCR   (MCI_BASE + PERIPH_TNCR_OFF) /*!< \brief PDC transmit next counter register address. */
/*@}*/

/*! \name SSC Transfer Control Register */
/*@{*/
#define MCI_PTCR   (MCI_BASE + PERIPH_PTCR_OFF) /*!< \brief PDC transfer control register address. */
/*@}*/

/*! \name SSC Transfer Status Register */
/*@{*/
#define MCI_PTSR   (MCI_BASE + PERIPH_PTSR_OFF) /*!< \brief PDC transfer status register address. */
/*@}*/

#endif

/*@} xgNutArchArmAt91Mci */

#endif                          /* _ARCH_ARM_AT91_MCI_H_ */
