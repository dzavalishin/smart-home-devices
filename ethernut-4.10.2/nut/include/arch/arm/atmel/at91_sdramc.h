#ifndef _ARCH_ARM_AT91_SDRAMC_H_
#define	_ARCH_ARM_AT91_SDRAMC_H_

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
 * \file arch/arm/at91_sdramc.h
 * \brief AT91 SDRAM controller.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.2  2008/02/15 17:04:59  haraldkipp
 * Spport for AT91SAM7SE512 added.
 *
 * Revision 1.1  2006/08/31 19:10:38  haraldkipp
 * New peripheral register definitions for the AT91SAM9260.
 *
 *
 * \endverbatim
 */

/*!
 * \addtogroup xgNutArchArmAt91Sdramc
 */
/*@{*/

/*! \name SDRAM Controller Mode Register */
/*@{*/
#define SDRAMC_MR_OFF                   0x00000000      /*!< \brief Mode register offset. */
#define SDRAMC_MR   (SDRAMC_BASE + SDRAMC_MR_OFF)       /*!< \brief Mode register address. */
#define SDRAMC_MODE                     0x00000007      /*!< \brief Command mode mask. */
#define SDRAMC_MODE_NORMAL              0x00000000      /*!< \brief Normal mode. */
#define SDRAMC_MODE_NOP                 0x00000001      /*!< \brief Issues a NOP command when accessed. */
#define SDRAMC_MODE_PRCGALL             0x00000002      /*!< \brief Issues an "All Banks Precharge" command when accessed. */
#define SDRAMC_MODE_LMR                 0x00000003      /*!< \brief Issues a "Load Mode Register" command when accessed. */
#define SDRAMC_MODE_RFSH                0x00000004      /*!< \brief Issues a "Auto Refresh" command when accessed. */
#define SDRAMC_MODE_EXT_LMR             0x00000005      /*!< \brief Issues a "Extended Load Mode Register" command when accessed. */
#define SDRAMC_MODE_DEEP                0x00000006      /*!< \brief Enters deep power down mode. */
#if defined(EBI_HAS_CSA)
#define SDRAMC_DBW                      0x00000010      /*!< \brief Data bus width (16bits=1). */
#endif
/*@}*/

/*! \name SDRAM Controller Refresh Timer Register */
/*@{*/
#define SDRAMC_TR_OFF                   0x00000004      /*!< \brief Refresh timer register offset. */
#define SDRAMC_TR   (SDRAMC_BASE + SDRAMC_TR_OFF)       /*!< \brief Refresh timer register address. */
#define SDRAMC_COUNT                    0x00000FFF      /*!< \brief Refresh timer count mask. */
/*@}*/

/*! \name SDRAM Controller Configuration Register */
/*@{*/
#define SDRAMC_CR_OFF                   0x00000008      /*!< \brief Configuration register offset. */
#define SDRAMC_CR   (SDRAMC_BASE + SDRAMC_CR_OFF)       /*!< \brief Configuration register address. */
#define SDRAMC_NC                       0x00000003      /*!< \brief Number of column bits. */
#define SDRAMC_NC_8                     0x00000000      /*!< \brief 8 column bits. */
#define SDRAMC_NC_9                     0x00000001      /*!< \brief 9 column bits. */
#define SDRAMC_NC_10                    0x00000002      /*!< \brief 10 column bits. */
#define SDRAMC_NC_11                    0x00000003      /*!< \brief 11 column bits. */
#define SDRAMC_NR                       0x0000000C      /*!< \brief Number of row bits. */
#define SDRAMC_NR_11                    0x00000000      /*!< \brief 11 row bits. */
#define SDRAMC_NR_12                    0x00000004      /*!< \brief 12 row bits. */
#define SDRAMC_NR_13                    0x00000008      /*!< \brief 13 row bits. */
#define SDRAMC_NB                       0x00000010      /*!< \brief 4 banks. */
#define SDRAMC_CAS                      0x00000060      /*!< \brief CAS latency. */
#define SDRAMC_CAS_1                    0x00000020      /*!< \brief CAS latency of 1 cycle. */
#define SDRAMC_CAS_2                    0x00000040      /*!< \brief CAS latency of 2 cycles. */
#define SDRAMC_CAS_3                    0x00000060      /*!< \brief CAS latency of 3 cycles. */
#if defined(EBI_HAS_CSA)
#define SDRAMC_TWR                      0x00000780      /*!< \brief Write recovery delay. */
#define SDRAMC_TWR_LSB                          7       /*!< \brief Write recovery delay. */
#define SDRAMC_TRC                      0x00007800      /*!< \brief Row cycle delay. */
#define SDRAMC_TRC_LSB                          11      /*!< \brief Row cycle delay. */
#define SDRAMC_TRP                      0x00078000      /*!< \brief Row precharge delay. */
#define SDRAMC_TRP_LSB                          15      /*!< \brief Row precharge delay. */
#define SDRAMC_TRCD                     0x00780000      /*!< \brief Row to column delay. */
#define SDRAMC_TRCD_LSB                         19      /*!< \brief Row to column delay. */
#define SDRAMC_TRAS                     0x07800000      /*!< \brief Active to precharge delay. */
#define SDRAMC_TRAS_LSB                         23      /*!< \brief Active to precharge delay. */
#define SDRAMC_TXSR                     0x78000000      /*!< \brief Exit self refresh to active delay. */
#define SDRAMC_TXSR_LSB                         27      /*!< \brief Exit self refresh to active delay. */
#else
#define SDRAMC_DBW                      0x00000080      /*!< \brief 16-bit data bus. */
#define SDRAMC_TWR                      0x00000F00      /*!< \brief Write recovery delay. */
#define SDRAMC_TWR_LSB                          8       /*!< \brief Write recovery delay. */
#define SDRAMC_TRC                      0x0000F000      /*!< \brief Row cycle delay. */
#define SDRAMC_TRC_LSB                          12      /*!< \brief Row cycle delay. */
#define SDRAMC_TRP                      0x000F0000      /*!< \brief Row precharge delay. */
#define SDRAMC_TRP_LSB                          16      /*!< \brief Row precharge delay. */
#define SDRAMC_TRCD                     0x00F00000      /*!< \brief Row to column delay. */
#define SDRAMC_TRCD_LSB                         20      /*!< \brief Row to column delay. */
#define SDRAMC_TRAS                     0x0F000000      /*!< \brief Active to precharge delay. */
#define SDRAMC_TRAS_LSB                         24      /*!< \brief Active to precharge delay. */
#define SDRAMC_TXSR                     0xF0000000      /*!< \brief Exit self refresh to active delay. */
#define SDRAMC_TXSR_LSB                         28      /*!< \brief Exit self refresh to active delay. */
#endif
/*@}*/

/*! \name SDRAM Controller Low Power Register */
/*@{*/
#define SDRAMC_SRR_OFF                  0x0000000C      /*!< \brief Self refresh register offset. */
#define SDRAMC_SRR  (SDRAMC_BASE + SDRAMC_SRR_OFF)      /*!< \brief Self refresh register address. */
#define SDRAMC_SRCB                     0x00000001      /*!< \brief Self refresh command bit. */
/*@}*/

/*! \name SDRAM Controller Low Power Register */
/*@{*/
#define SDRAMC_LPR_OFF                  0x00000010      /*!< \brief Low power register offset. */
#define SDRAMC_LPR  (SDRAMC_BASE + SDRAMC_LPR_OFF)      /*!< \brief Low power register address. */
#define SDRAMC_LPCB                     0x00000003      /*!< \brief Low power configuration mask. */
#define SDRAMC_LPCB_DISABLE             0x00000000      /*!< \brief Low power feature disabled. */
#define SDRAMC_LPCB_SELF_REFRESH        0x00000001      /*!< \brief Enable self refresh. */
#define SDRAMC_LPCB_POWER_DOWN          0x00000002      /*!< \brief Issues a "Power Down" command when accessed.. */
#define SDRAMC_LPCB_DEEP_POWER_DOWN     0x00000003      /*!< \brief Enters deep power down mode. */
#define SDRAMC_PASR                     0x00000070      /*!< \brief Partial array self-refresh mask. */
#define SDRAMC_PASR_LSB                         4       /*!< \brief Partial array self-refresh LSB. */
#define SDRAMC_TCSR                     0x00000300      /*!< \brief Temperature compensated self-refresh mask. */
#define SDRAMC_TCSR_LSB                         8       /*!< \brief Temperature compensated self-refresh LSB. */
#define SDRAMC_DS                       0x00000C00      /*!< \brief Drive strength mask. */
#define SDRAMC_DS_LSB                           10      /*!< \brief Drive strength LSB. */
#define SDRAMC_TIMEOUT                  0x00003000      /*!< \brief Mask of time to define when low-power mode is enabled. */
#define SDRAMC_TIMEOUT_0                0x00000000      /*!< \brief Activate immediately. */
#define SDRAMC_TIMEOUT_64               0x00001000      /*!< \brief Activate after 64 clock cycles after the end of the last transfer. */
#define SDRAMC_TIMEOUT_128              0x00002000      /*!< \brief Activate after 64 clock cycles after the end of the last transfer. */
/*@}*/

/*! \name SDRAM Controller Interrupt Registers */
/*@{*/
#define SDRAMC_IER_OFF                  0x00000014      /*!< \brief Interrupt enable register offset. */
#define SDRAMC_IER  (SDRAMC_BASE + SDRAMC_IER_OFF)      /*!< \brief Interrupt enable register address. */
#define SDRAMC_IDR_OFF                  0x00000018      /*!< \brief Interrupt disable register offset. */
#define SDRAMC_IDR  (SDRAMC_BASE + SDRAMC_IDR_OFF)      /*!< \brief Interrupt disable register address. */
#define SDRAMC_IMR_OFF                  0x0000001C      /*!< \brief Interrupt mask register offset. */
#define SDRAMC_IMR  (SDRAMC_BASE + SDRAMC_IMR_OFF)      /*!< \brief Interrupt mask register address. */
#define SDRAMC_ISR_OFF                  0x00000020      /*!< \brief Interrupt status register offset. */
#define SDRAMC_ISR  (SDRAMC_BASE + SDRAMC_ISR_OFF)      /*!< \brief Interrupt status register address. */
#define SDRAMC_RES                      0x00000001      /*!< \brief Refresh error status. */
/*@}*/

/*! \name SDRAM Controller Memory Device Register */
/*@{*/
#define SDRAMC_MDR_OFF                  0x00000024      /*!< \brief Memory device register offset. */
#define SDRAMC_MDR  (SDRAMC_BASE + SDRAMC_MDR_OFF)      /*!< \brief Memory device register address. */
#define SDRAMC_MD                       0x00000003      /*!< \brief Memory device type mask. */
#define SDRAMC_MD                       0x00000003      /*!< \brief Memory device type mask. */
#define SDRAMC_MD_SDRAM                 0x00000000      /*!< \brief SDRAM. */
#define SDRAMC_MD_LPSDRAM               0x00000001      /*!< \brief Low power SDRAM. */
/*@}*/

/*@} xgNutArchArmAt91Sdramc */

#endif                          /* _ARCH_ARM_AT91_SDRAMC_H_ */
