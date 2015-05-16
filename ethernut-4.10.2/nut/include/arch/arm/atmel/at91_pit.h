#ifndef _ARCH_ARM_AT91_PIT_H_
#define _ARCH_ARM_AT91_PIT_H_

/*
 * Copyright (C) 2007 by egnite Software GmbH. All rights reserved.
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
 * \file arch/arm/at91_pit.h
 * \brief AT91 periodic interval timer.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.1  2007/02/15 16:14:39  haraldkipp
 * Periodic interrupt timer can be used as a system clock.
 *
 *
 * \endverbatim
 */

/*!
 * \addtogroup xgNutArchArmAt91Pit
 */
/*@{*/

/*! \name Periodic Inverval Timer Mode Register */
/*@{*/
#define PIT_MR_OFF              0x00000000  /*!< \brief Mode register offset. */
#define PIT_MR      (PIT_BASE + PIT_MR_OFF) /*!< \brief Mode register address. */
#define PIT_PIV                 0x000FFFFF  /*!< \brief Periodic interval value mask. */
#define PIT_PIV_LSB                     0   /*!< \brief Periodic interval value LSB. */
#define PIT_PITEN               0x01000000  /*!< \brief Periodic interval timer enable. */
#define PIT_PITIEN              0x02000000  /*!< \brief Periodic interval timer interrupt enable. */
/*@}*/

/*! \name Periodic Inverval Timer Status Register */
/*@{*/
#define PIT_SR_OFF              0x00000004  /*!< \brief Status register offset. */
#define PIT_SR      (PIT_BASE + PIT_SR_OFF) /*!< \brief Status register address. */
#define PIT_PITS                0x00000001  /*!< \brief Timer has reached PIT_PIV. */
/*@}*/

/*! \name Periodic Inverval Timer Value and Image Registers */
/*@{*/
#define PIT_PIVR_OFF            0x00000008  /*!< \brief Value register offset. */
#define PIT_PIVR  (PIT_BASE + PIT_PIVR_OFF) /*!< \brief Value register address. */
#define PIT_PIIR_OFF            0x0000000C  /*!< \brief Image register offset. */
#define PIT_PIIR  (PIT_BASE + PIT_PIIR_OFF) /*!< \brief Image register address. */
#define PIT_CPIV                0x000FFFFF  /*!< \brief Current periodic interval value mask. */
#define PIT_CPIV_LSB                    0   /*!< \brief Current periodic interval value LSB. */
#define PIT_PICNT               0xFFF00000  /*!< \brief Periodic interval counter mask. */
#define PIT_PICNT_LSB                  20   /*!< \brief Periodic interval counter LSB. */
/*@}*/

/*@} xgNutArchArmAt91Pit */

#endif  /* _ARCH_ARM_AT91_PIT_H_ */
