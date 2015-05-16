#ifndef _ARCH_ARM_AT91_ECC_H_
#define	_ARCH_ARM_AT91_ECC_H_

/*
 * Copyright (C) 2010 by egnite GmbH
 *
 * All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
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
 * \file arch/arm/at91_ecc.h
 * \brief AT91 peripherals.
 *
 * \verbatim
 * $Id$
 * \endverbatim
 */

/*!
 * \addtogroup xgNutArchArmAt91Ecc
 */
/*@{*/

/*! \name ECC Control Register */
/*@{*/
#define ECC_CR_OFF              0x00000000  /*!< \brief Control register offset. */
#define ECC_CR      (ECC_BASE + ECC_CR_OFF) /*!< \brief Control register address. */
#define ECC_RST                 0x00000001  /*!< \brief Reset ECC parity registers. */
#define ECC_SRST                0x00000002  /*!< \brief Software reset. */
/*@}*/

/*! \name ECC Mode Register */
/*@{*/
#define ECC_MR_OFF              0x00000004  /*!< \brief Mode register offset. */
#define ECC_MR      (ECC_BASE + ECC_MR_OFF) /*!< \brief Mode register address. */
#define ECC_PAGESIZE            0x00000003  /*!< \brief NAND flash page size mask. */
#define ECC_PAGESIZE_528        0x00000000  /*!< \brief NAND flash page size is 528 words. */
#define ECC_PAGESIZE_1056       0x00000001  /*!< \brief NAND flash page size is 1056 words. */
#define ECC_PAGESIZE_2112       0x00000002  /*!< \brief NAND flash page size is 2112 words. */
#define ECC_PAGESIZE_4224       0x00000003  /*!< \brief NAND flash page size is 4224 words. */
#define ECC_TYPECORREC          0x00000030  /*!< \brief Type of correction mask. */
#define ECC_TYPECORREC_0        0x00000000  /*!< \brief 1 bit correction. */
#define ECC_TYPECORREC_1        0x00000010  /*!< \brief 1 bit correction for 256 data bytes. */
#define ECC_TYPECORREC_2        0x00000020  /*!< \brief 1 bit correction for 512 data bytes. */
/*@}*/

/*! \name ECC Status Registers */
/*@{*/
#define ECC_SR1_OFF             0x00000008  /*!< \brief Status register 1 offset. */
#define ECC_SR1    (ECC_BASE + ECC_SR1_OFF) /*!< \brief Status register 1 address. */
#define ECC_SR2_OFF             0x00000014  /*!< \brief Status register 2 offset. */
#define ECC_SR2    (ECC_BASE + ECC_SR2_OFF) /*!< \brief Status register 2 address. */
#define ECC_RECERR(i)       (1 << (i * 4))  /*!< \brief Recoverable error. */
#define ECC_ECCERR(i)       (2 << (i * 4))  /*!< \brief Single bit error. */
#define ECC_MULERR(i)       (4 << (i * 4))  /*!< \brief Multiple errors. */
/*@}*/

/*! \name ECC Parity Registers */
/*@{*/
#define ECC_PR0_OFF             0x0000000C  /*!< \brief Parity register 0 offset. */
#define ECC_PR0    (ECC_BASE + ECC_PR0_OFF) /*!< \brief Parity register 0 address. */
#define ECC_PR1_OFF             0x00000010  /*!< \brief Parity register 1 offset. */
#define ECC_PR1    (ECC_BASE + ECC_PR1_OFF) /*!< \brief Parity register 1 address. */
#define ECC_PR(i)  (ECC_BASE + 0x18 + (((i) - 2) * 4)) /*!< \brief Parity registers 2 to 15 address. */
#define ECC_BITADDR0            0x0000000F  /*!< \brief Corrupted bit offset mask. */
#define ECC_BITADDR0_LSB                0   /*!< \brief Corrupted bit offset LSB. */
#define ECC_BITADDR             0x00000007  /*!< \brief Corrupted bit offset mask, 8-bit word. */
#define ECC_BITADDR_LSB                 0   /*!< \brief Corrupted bit offset LSB, 8-bit word. */
#define ECC_WORDADDR0           0x0000FFF0  /*!< \brief Corrupted bit word offset mask, type 1. */
#define ECC_WORDADDR0_LSB               4   /*!< \brief Corrupted bit word offset LSB, type 1. */
#define ECC_WORDADDR            0x0000FFF8  /*!< \brief Corrupted bit word offset mask. */
#define ECC_WORDADDR_LSB                3   /*!< \brief Corrupted bit word offset LSB. */
#define ECC_NPARITY1            0x0000FFFF  /*!< \brief Parity 1 mask. */
#define ECC_NPARITY1_LSB                0   /*!< \brief Parity 1 LSB. */
#define ECC_NPARITY             0x00FFF000  /*!< \brief Parity mask. */
#define ECC_NPARITY_LSB                12   /*!< \brief Parity LSB. */
/*@}*/

/*@}*/
/*! \name ECC Version Register */
/*@{*/
#define ECC_VR_OFF              0x000000FC // (ECC_VR)  ECC Version register
#define ECC_VR          (ECC_BASE + ECC_VR_OFF) // (ECC) ECC version register

/*@} xgNutArchArmAt91Ecc */

#endif
