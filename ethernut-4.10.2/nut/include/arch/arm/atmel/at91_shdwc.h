#ifndef _ARCH_ARM_AT91_SHDWC_H_
#define _ARCH_ARM_AT91_SHDWC_H_

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
 * \file arch/arm/at91_shdwc.h
 * \brief AT91 peripherals.
 *
 * \verbatim
 * $Id$
 * \endverbatim
 */

/*!
 * \addtogroup xgNutArchArmAt91Shdwc
 */
/*@{*/

/*! \name Shutdown Control Register */
/*@{*/
#define SHDWC_CR_OFF                0x00000000  /*!< \brief Control register offset. */
#define SHDWC_CR    (SHDWC_BASE + SHDWC_CR_OFF) /*!< \brief Control register address. */
#define SHDWC_SHDW                  0x00000001  /*!< \brief Shutdown command. */
#define SHDWC_KEY                   0xA5000000  /*!< \brief Shutdown password. */
/*@}*/

/*! \name Shutdown Mode Register */
/*@{*/
#define SHDWC_MR_OFF                0x00000004  /*!< \brief Mode register offset. */
#define SHDWC_MR    (SHDWC_BASE + SHDWC_MR_OFF) /*!< \brief Mode register address. */
#define SHDWC_WKMODE0               0x00000003  /*!< \brief Wake-up 0 mode mask. */
#define SHDWC_WKMODE0_NONE          0x00000000  /*!< \brief No detection on the wake-up input. */
#define SHDWC_WKMODE0_HIGH          0x00000001  /*!< \brief Low to high level. */
#define SHDWC_WKMODE0_LOW           0x00000002  /*!< \brief High to low level. */
#define SHDWC_WKMODE0_ANYLEVEL      0x00000003  /*!< \brief Both levels change. */
#define SHDWC_CPTWK0                0x000000F0  /*!< \brief Counter on wake-up 0 mask. */
#define SHDWC_CPTWK0_LSB                    4   /*!< \brief Counter on wake-up 0 LSB. */
#define SHDWC_WKMODE1               0x00000300  /*!< \brief Wake-up 1 mode mask. */
#define SHDWC_WKMODE1_NONE          0x00000000  /*!< \brief No detection on the wake-up input. */
#define SHDWC_WKMODE1_HIGH          0x00000100  /*!< \brief Low to high level. */
#define SHDWC_WKMODE1_LOW           0x00000200  /*!< \brief High to low level. */
#define SHDWC_WKMODE1_ANYLEVEL      0x00000300  /*!< \brief Both levels change. */
#define SHDWC_CPTWK1                0x0000F000  /*!< \brief Counter on wake-up 1 mask. */
#define SHDWC_CPTWK1_LSB                    12  /*!< \brief Counter on wake-up 1 LSB. */
#define SHDWC_RTTWKEN               0x00010000  /*!< \brief Real-time timer wake-up enable. */
#define SHDWC_RTCWKEN               0x00020000  /*!< \brief Real-time clock wake-up enable. */
/*@}*/

/*! \name Shutdown Status Register */
/*@{*/
#define SHDWC_SR_OFF                0x00000008  /*!< \brief Status register offset. */
#define SHDWC_SR    (SHDWC_BASE + SHDWC_SR_OFF) /*!< \brief Status register address. */
#define SHDWC_WAKEUP0               0x00000001  /*!< \brief Wake-up 0 status. */
#define SHDWC_WAKEUP1               0x00000002  /*!< \brief Wake-up 1 status. */
#define SHDWC_FWKUP                 0x00000004  /*!< \brief Force wake-up status. */
#define SHDWC_RTTWK                 0x00010000  /*!< \brief Real-time timer wake-up. */
#define SHDWC_RTCWK                 0x00020000  /*!< \brief Real-time clock wake-up. */
/*@}*/

/*@} xgNutArchArmAt91Shdwc */

#endif /* _ARCH_ARM_AT91_SHDWC_H_ */

