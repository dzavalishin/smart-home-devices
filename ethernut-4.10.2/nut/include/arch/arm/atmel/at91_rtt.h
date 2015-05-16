#ifndef _ARCH_ARM_AT91_RTT_H_
#define _ARCH_ARM_AT91_RTT_H_

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
 * \file arch/arm/at91_rtt.h
 * \brief AT91 peripherals.
 *
 * \verbatim
 * $Id$
 * \endverbatim
 */

/*!
 * \addtogroup xgNutArchArmAt91Rtt
 */
/*@{*/

/*! \name Real-Time Timer Mode Register */
/*@{*/
#define RTT_MR_OFF              0x00000000  /*!< \brief Mode register offset. */
#define RTT_MR      (RTT_BASE + RTT_MR_OFF) /*!< \brief Mode register address. */
#define RTT_RTPRES              0x0000FFFF  /*!< \brief Timer prescaler value mask. */
#define RTT_RTPRES_LSB                  0   /*!< \brief Timer prescaler value mask. */
#define RTT_ALMIEN              0x00010000  /*!< \brief Alarm interrupt enable. */
#define RTT_RTTINCIEN           0x00020000  /*!< \brief Timer increment interrupt enable. */
#define RTT_RTTRST              0x00040000  /*!< \brief Timer restart. */
/*@}*/

/*! \name Real-Time Timer Alarm Register */
/*@{*/
#define RTT_AR_OFF              0x00000004  /*!< \brief Alarm register offset. */
#define RTT_AR      (RTT_BASE + RTT_AR_OFF) /*!< \brief Alarm register address. */
/*@}*/

/*! \name Real-Time Timer Value Register */
/*@{*/
#define RTT_VR_OFF              0x00000004  /*!< \brief Value register offset. */
#define RTT_VR      (RTT_BASE + RTT_VR_OFF) /*!< \brief Value register address. */

/*! \name Real-Time Timer Status Register */
/*@{*/
#define RTT_SR_OFF              0x00000008  /*!< \brief Status register offset. */
#define RTT_SR      (RTT_BASE + RTT_SR_OFF) /*!< \brief Status register address. */
#define RTT_ALMS                0x00000001  /*!< \brief Alarm status. */
#define RTT_RTTINC              0x00000002  /*!< \brief Timer increment. */
/*@}*/


/*@} xgNutArchArmAt91Rtt */

#endif /* _ARCH_ARM_AT91_RTT_H_ */

