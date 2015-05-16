#ifndef _ARCH_ARM_AT91_EEFC_H_
#define	_ARCH_ARM_AT91_EEFC_H_

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
 * \file arch/arm/at91_eefc.h
 * \brief AT91 peripherals.
 *
 * \verbatim
 * $Id$
 * \endverbatim
 */

/*!
 * \addtogroup xgNutArchArmAt91Eefc
 */
/*@{*/

/*! \name EEFC Flash Mode Register */
/*@{*/
#define EEFC_FMR_OFF            0x00000000  /*!< \brief Mode register offset. */
#define EEFC_FMR (EEFC_BASE + EEFC_FMR_OFF) /*!< \brief Mode register address. */
#define EEFC_FWS                0x00000F00  /*!< \brief Flash wait state mask. */
#define EEFC_FWS_LSB                    8   /*!< \brief Flash wait state LSB. */
/*@}*/

/*! \name EEFC Flash Command Register */
/*@{*/
#define EEFC_FCR_OFF            0x00000004  /*!< \brief Command register offset. */
#define EEFC_FCR (EEFC_BASE + EEFC_FCR_OFF) /*!< \brief Command register address. */
#define EEFC_FCMD               0x000000FF  /*!< \brief Flash command mask. */
#define EEFC_FCMD_GETD          0x00000000  /*!< \brief Get flash descriptor command. */
#define EEFC_FCMD_WP            0x00000001  /*!< \brief Write page command. */
#define EEFC_FCMD_WPL           0x00000002  /*!< \brief Write page and lock command. */
#define EEFC_FCMD_EWP           0x00000003  /*!< \brief Erase and write page command. */
#define EEFC_FCMD_EWPL          0x00000004  /*!< \brief Erase and write page with lock command. */
#define EEFC_FCMD_EA            0x00000005  /*!< \brief Erase all command. */
#define EEFC_FCMD_EPL           0x00000006  /*!< \brief Erase plane command. */
#define EEFC_FCMD_EPA           0x00000007  /*!< \brief Erase pages command. */
#define EEFC_FCMD_SLB           0x00000008  /*!< \brief Set lock bit command. */
#define EEFC_FCMD_CLB           0x00000009  /*!< \brief Clear lock bit command. */
#define EEFC_FCMD_GLB           0x0000000A  /*!< \brief Get lock bit command. */
#define EEFC_FCMD_SGPB          0x0000000B  /*!< \brief Set GPNVM bit command. */
#define EEFC_FCMD_CGPB          0x0000000C  /*!< \brief Clear GPNVM bit command. */
#define EEFC_FCMD_GGPB          0x0000000D  /*!< \brief Get GPNVM bit command. */
#define EEFC_FARG               0x00FFFF00  /*!< \brief Flash command argument mask. */
#define EEFC_FARG_LSB                   8   /*!< \brief Flash command argument LSB. */
#define EEFC_FKEY               0x5A000000  /*!< \brief Flash write protection key. */
/*@}*/

/*! \name EEFC Flash Status Register */
/*@{*/
#define EEFC_FSR_OFF            0x00000008  /*!< \brief Status register offset. */
#define EEFC_FSR (EEFC_BASE + EEFC_FSR_OFF) /*!< \brief Status register address. */
#define EEFC_FRDY               0x00000001  /*!< \brief Flash ready. */
#define EEFC_FCMDE              0x00000002  /*!< \brief Flash command error. */
#define EEFC_LOCKE              0x00000004  /*!< \brief Flash lock error. */
/*@}*/

/*! \name EEFC Flash Result Register */
/*@{*/
#define EEFC_FRR_OFF            0x0000000C  /*!< \brief Result register offset. */
#define EEFC_FRR (EEFC_BASE + EEFC_FRR_OFF) /*!< \brief Result register address. */
/*@}*/

/*! \name EEFC Flash Version Register */
/*@{*/
#define EEFC_FVR_OFF            0x00000010  /*!< \brief Version register offset. */
#define EEFC_FVR (EEFC_BASE + EEFC_FVR_OFF) /*!< \brief Version register address. */
/*@}*/

/*@} xgNutArchArmAt91Eefc */

#endif
