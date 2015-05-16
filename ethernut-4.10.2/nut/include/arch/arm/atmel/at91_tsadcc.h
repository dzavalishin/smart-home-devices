#ifndef _ARCH_ARM_TSADCC_H_
#define _ARCH_ARM_TSADCC_H_
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
 * \file arch/arm/at91_tsadcc.h
 * \brief AT91 touch screen adc controller.
 *
 * \verbatim
 * $Id:  $
 * \endverbatim
 */

/*!
 * \addtogroup xgNutArchArmAt91Tsadcc
 */
/*@{*/

#ifdef TSADCC_BASE

/*! \name TSADCC Controller Register */
/*@{*/
#define TSADCC_CR_OFF             0x00000000  						/*!< \brief Control Register. */
#define TSADCC_MR_OFF             0x00000004  						/*!< \brief Mode Register. */
#define TSADCC_TRGR_OFF           0x00000008  						/*!< \brief Trigger Register. */
#define TSADCC_TSR_OFF            0x0000000C  						/*!< \brief Touch Screen Register. */
#define TSADCC_CHER_OFF           0x00000010  						/*!< \brief Channel Enable Register. */
#define TSADCC_CHDR_OFF           0x00000014  						/*!< \brief Channel Disable Register. */
#define TSADCC_CHSR_OFF           0x00000018  						/*!< \brief Channel Status Register. */
#define TSADCC_SR_OFF             0x0000001C  						/*!< \brief Status Register. */
#define TSADCC_LCDR_OFF           0x00000020  						/*!< \brief Last Converted Data Register. */
#define TSADCC_IER_OFF            0x00000024  						/*!< \brief Interrupt Enable Register. */
#define TSADCC_IDR_OFF            0x00000028  						/*!< \brief Interrupt Disable Register. */
#define TSADCC_IMR_OFF            0x0000002C  						/*!< \brief Interrupt Mask Register. */
#define TSADCC_CDR0_OFF           0x00000030  						/*!< \brief Channel Data Register 0. */
#define TSADCC_CDR1_OFF           0x00000034  						/*!< \brief Channel Data Register 1. */
#define TSADCC_CDR2_OFF           0x00000038  						/*!< \brief Channel Data Register 2. */
#define TSADCC_CDR3_OFF           0x0000003C  						/*!< \brief Channel Data Register 3. */
#define TSADCC_CDR4_OFF           0x00000040  						/*!< \brief Channel Data Register 4. */
#define TSADCC_CDR5_OFF           0x00000044  						/*!< \brief Channel Data Register 5. */
#define TSADCC_CDR6_OFF           0x00000048  						/*!< \brief Channel Data Register 6. */
#define TSADCC_CDR7_OFF           0x0000004C  						/*!< \brief Channel Data Register 7. */
#define TSADCC_XPDR_OFF           0x00000050  						/*!< \brief X Position Data Register. */
#define TSADCC_Z1DR_OFF           0x00000054  						/*!< \brief Z1 Data Register. */
#define TSADCC_Z2DR_OFF           0x00000058  						/*!< \brief Z2 Data Register. */
#define TSADCC_MSCR_OFF           0x00000060  						/*!< \brief Manual Switch Command Register. */
#define TSADCC_WPMR_OFF           0x000000E4  						/*!< \brief Write Protection Mode Register. */
#define TSADCC_WPSR_OFF           0x000000E8  						/*!< \brief Write Protection Status Register. */

#define TSADCC_CR          (TSADCC_BASE + TSADCC_CR_OFF) 				/*!< \brief Control Register. */
#define TSADCC_MR          (TSADCC_BASE + TSADCC_MR_OFF)        /*!< \brief Mode Register. */
#define TSADCC_TRGR        (TSADCC_BASE + TSADCC_TRGR_OFF)      /*!< \brief Trigger Register. */
#define TSADCC_TSR         (TSADCC_BASE + TSADCC_TSR_OFF)       /*!< \brief Touch Screen Register. */
#define TSADCC_CHER        (TSADCC_BASE + TSADCC_CHER_OFF)      /*!< \brief Channel Enable Register. */
#define TSADCC_CHDR        (TSADCC_BASE + TSADCC_CHDR_OFF)      /*!< \brief Channel Disable Register. */
#define TSADCC_CHSR        (TSADCC_BASE + TSADCC_CHSR_OFF)      /*!< \brief Channel Status Register. */
#define TSADCC_SR          (TSADCC_BASE + TSADCC_SR_OFF)        /*!< \brief Status Register. */
#define TSADCC_LCDR        (TSADCC_BASE + TSADCC_LCDR_OFF)      /*!< \brief Last Converted Data Register. */
#define TSADCC_IER         (TSADCC_BASE + TSADCC_IER_OFF)       /*!< \brief Interrupt Enable Register. */
#define TSADCC_IDR         (TSADCC_BASE + TSADCC_IDR_OFF)       /*!< \brief Interrupt Disable Register. */
#define TSADCC_IMR         (TSADCC_BASE + TSADCC_IMR_OFF)       /*!< \brief Interrupt Mask Register. */
#define TSADCC_CDR0        (TSADCC_BASE + TSADCC_CDR0_OFF)      /*!< \brief Channel Data Register 0. */
#define TSADCC_CDR1        (TSADCC_BASE + TSADCC_CDR1_OFF)      /*!< \brief Channel Data Register 1. */
#define TSADCC_CDR2        (TSADCC_BASE + TSADCC_CDR2_OFF)      /*!< \brief Channel Data Register 2. */
#define TSADCC_CDR3        (TSADCC_BASE + TSADCC_CDR3_OFF)      /*!< \brief Channel Data Register 3. */
#define TSADCC_CDR4        (TSADCC_BASE + TSADCC_CDR4_OFF)      /*!< \brief Channel Data Register 4. */
#define TSADCC_CDR5        (TSADCC_BASE + TSADCC_CDR5_OFF)      /*!< \brief Channel Data Register 5. */
#define TSADCC_CDR6        (TSADCC_BASE + TSADCC_CDR6_OFF)      /*!< \brief Channel Data Register 6. */
#define TSADCC_CDR7        (TSADCC_BASE + TSADCC_CDR7_OFF)      /*!< \brief Channel Data Register 7. */
#define TSADCC_XPDR        (TSADCC_BASE + TSADCC_XPDR_OFF)      /*!< \brief X Position Data Register. */
#define TSADCC_Z1DR        (TSADCC_BASE + TSADCC_Z1DR_OFF)      /*!< \brief Z1 Data Register. */
#define TSADCC_Z2DR        (TSADCC_BASE + TSADCC_Z2DR_OFF)      /*!< \brief Z2 Data Register. */
#define TSADCC_MSCR        (TSADCC_BASE + TSADCC_MSCR_OFF)      /*!< \brief Manual Switch Command Register. */
#define TSADCC_WPMR        (TSADCC_BASE + TSADCC_WPMR_OFF)      /*!< \brief Write Protection Mode Register. */
#define TSADCC_WPSR        (TSADCC_BASE + TSADCC_WPSR_OFF)      /*!< \brief Write Protection Status Register. */
/*@}*/

#endif

/*@} xgNutArchArmAt91Tsadcc */

#endif                          /* _ARCH_ARM_TSADCC_H_ */
