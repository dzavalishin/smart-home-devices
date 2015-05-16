#ifndef _ARCH_ARM_AT91_SMC_H_
#define	_ARCH_ARM_AT91_SMC_H_

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
 * \file arch/arm/at91_smc.h
 * \brief AT91 static memory controller.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.2  2008/02/15 17:00:08  haraldkipp
 * Spport for AT91SAM7SE512 added.
 *
 * Revision 1.1  2006/08/31 19:10:38  haraldkipp
 * New peripheral register definitions for the AT91SAM9260.
 *
 *
 * \endverbatim
 */

/*!
 * \addtogroup xgNutArchArmAt91Smc
 */
/*@{*/

#if defined(SMC_HAS_CSR)

#define SMC_CSR(cs)             (SMC_BASE + cs * 0x04)  /*!< \brief SMC chip select register address. */

#define SMC_NWS                 0x0000007F  /*!< brief Number of wait states mask. */
#define SMC_NWS_LSB                     0   /*!< brief Number of wait states LSB. */
#define SMC_WSEN                0x00000080  /*!< brief Wait state enable. */
#define SMC_TDF                 0x00000F00  /*!< brief Data float time mask. */
#define SMC_TDF_LSB                     8   /*!< brief Data float time LSB. */
#define SMC_BAT                 0x00001000  /*!< brief Byte access type (16 bit device = 1). */
#define SMC_DBW                 0x00006000  /*!< brief Data bus width. */
#define SMC_DBW_16              0x00002000  /*!< brief 16-bit data bus. */
#define SMC_DBW_8               0x00004000  /*!< brief 8-bit data bus. */
#define SMC_DRP                 0x00008000  /*!< brief Data read protocol (early read = 1). */
#define SMC_ACSS                0x00030000  /*!< brief Address to chip select setup mask. */
#define SMC_ACSS_LSB                    16  /*!< brief Address to chip select setup LSB. */
#define SMC_ACSS_STANDARD       0x00000000  /*!< brief Standard, asserted at the beginning of the access and deasserted at the end. */
#define SMC_ACSS_1_CYCLE        0x00010000  /*!< brief One cycle less at the beginning and the end of the access. */
#define SMC_ACSS_2_CYCLES       0x00020000  /*!< brief Two cycles less at the beginning and the end of the access. */
#define SMC_ACSS_3_CYCLES       0x00030000  /*!< brief Three cycles less at the beginning and the end of the access. */
#define SMC_RWSETUP             0x07000000  /*!< brief Read and write signal setup time mask. */
#define SMC_RWSETUP_LSB                 24  /*!< brief Read and write signal setup time LSB. */
#define SMC_RWHOLD              0x70000000  /*!< brief Read and write signal hold time mask. */
#define SMC_RWHOLD_LSB                  28  /*!< brief Read and write signal hold time LSB. */

#else /* SMC_HAS_CSR */

/*! \name SMC Setup Register */
/*@{*/
#define SMC_SETUP(cs)   (SMC_BASE + 0x10 * cs + 0x00)   /*!< \brief SMC setup register address. */
#define SMC_NWE_SETUP                       0x0000003F  /*!< \brief NWE setup length mask. */
#define SMC_NWE_SETUP_LSB                           0   /*!< \brief NWE setup length LSB. */
#define SMC_NCS_WR_SETUP                    0x00003F00  /*!< \brief NCS setup length in write access mask. */
#define SMC_NCS_WR_SETUP_LSB                        8   /*!< \brief NCS setup length in write access LSB. */
#define SMC_NRD_SETUP                       0x003F0000  /*!< \brief NRD setup length mask. */
#define SMC_NRD_SETUP_LSB                           16  /*!< \brief NRD setup length LSB. */
#define SMC_NCS_RD_SETUP                    0x3F000000  /*!< \brief NCS setup length in read access mask. */
#define SMC_NCS_RD_SETUP_LSB                        24  /*!< \brief NCS setup length in read access LSB. */
/*@}*/

/*! \name SMC Pulse Register */
/*@{*/
#define SMC_PULSE(cs)   (SMC_BASE + 0x10 * cs + 0x04)   /*!< \brief SMC pulse register address. */
#define SMC_NWE_PULSE                       0x0000003F  /*!< \brief NWE pulse length mask. */
#define SMC_NWE_PULSE_LSB                           0   /*!< \brief NWE pulse length LSB. */
#define SMC_NCS_WR_PULSE                    0x00003F00  /*!< \brief NCS pulse length in write access mask. */
#define SMC_NCS_WR_PULSE_LSB                        8   /*!< \brief NCS pulse length in write access LSB. */
#define SMC_NRD_PULSE                       0x003F0000  /*!< \brief NRD pulse length mask. */
#define SMC_NRD_PULSE_LSB                           16  /*!< \brief NRD pulse length LSB. */
#define SMC_NCS_RD_PULSE                    0x3F000000  /*!< \brief NCS pulse length in read access mask. */
#define SMC_NCS_RD_PULSE_LSB                        24  /*!< \brief NCS pulse length in read access LSB. */
/*@}*/

/*! \name SMC Cycle Register */
/*@{*/
#define SMC_CYCLE(cs)   (SMC_BASE + 0x10 * cs + 0x08)   /*!< \brief SMC cycle register address. */
#define SMC_NWE_CYCLE                       0x000001FF  /*!< \brief Total write cycle length mask. */
#define SMC_NWE_CYCLE_LSB                           0   /*!< \brief Total write cycle length LSB. */
#define SMC_NRD_CYCLE                       0x01FF0000  /*!< \brief Total read cycle length mask. */
#define SMC_NRD_CYCLE_LSB                           16  /*!< \brief Total read cycle length LSB. */
/*@}*/

/*! \name SMC Mode Register */
/*@{*/
#define SMC_MODE(cs)    (SMC_BASE + 0x10 * cs + 0x0C)   /*!< \brief SMC mode register address. */
#define SMC_READ_MODE                       0x00000001  /*!< \brief Read operation mode. */
#define SMC_WRITE_MODE                      0x00000002  /*!< \brief Write operation mode. */
#define SMC_EXNW_MODE                       0x00000030  /*!< \brief NWAIT mode mask. */
#define SMC_EXNW_MODE_DISABLED              0x00000000  /*!< \brief NWAIT mode mask. */
#define SMC_EXNW_MODE_FROZEN                0x00000020  /*!< \brief NWAIT mode mask. */
#define SMC_EXNW_MODE_READY                 0x00000030  /*!< \brief NWAIT mode mask. */
#define SMC_BAT                             0x00000100  /*!< \brief Byte access mode. */
#define SMC_DBW                             0x00003000  /*!< \brief Data bus width. */
#define SMC_DBW_8                           0x00000000  /*!< \brief 8-bit data bus. */
#define SMC_DBW_16                          0x00001000  /*!< \brief 16-bit data bus. */
#define SMC_DBW_32                          0x00002000  /*!< \brief 32-bit data bus. */
#define SMC_TDF_CYCLES                      0x000F0000  /*!< \brief Data float time mask. */
#define SMC_TDF_CYCLES_LSB                          16  /*!< \brief Data float time LSB. */
#define SMC_TDF_MODE                        0x00100000  /*!< \brief TDF optimization. */
#define SMC_PMEN                            0x01000000  /*!< \brief Page mode enable. */
#define SMC_PS                              0x30000000  /*!< \brief Page size mask. */
#define SMC_PS_4                            0x30000000  /*!< \brief 4-byte page. */
#define SMC_PS_8                            0x30000000  /*!< \brief 8-byte page. */
#define SMC_PS_16                           0x30000000  /*!< \brief 16-byte page. */
#define SMC_PS_32                           0x30000000  /*!< \brief 32-byte page. */
/*@}*/

#endif /* SMC_HAS_CSR */

/*@} xgNutArchArmAt91Smc */


#endif                          /* _ARCH_ARM_AT91_SMC_H_ */
