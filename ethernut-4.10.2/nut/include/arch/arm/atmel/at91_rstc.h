#ifndef _ARCH_ARM_AT91_RSTC_H_
#define _ARCH_ARM_AT91_RSTC_H_

/*
 * Copyright (C) 2005-2006 by egnite Software GmbH. All rights reserved.
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
 * \file arch/arm/at91_rstc.h
 * \brief AT91 peripherals.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.2  2006/08/31 19:13:15  haraldkipp
 * Wakeup bit and LSB of external reset length added.
 *
 * Revision 1.1  2006/07/05 07:45:28  haraldkipp
 * Split on-chip interface definitions.
 *
 *
 * \endverbatim
 */

/*!
 * \addtogroup xgNutArchArmAt91Rstc
 */
/*@{*/

/*! \name Reset Controller Control Register */
/*@{*/
#define RSTC_CR         (RSTC_BASE + 0x00)      /*!< \brief Reset controller control register address. */
#define RSTC_PROCRST            0x00000001      /*!< \brief Processor reset. */
#define RSTC_PERRST             0x00000004      /*!< \brief Peripheral reset. */
#define RSTC_EXTRST             0x00000008      /*!< \brief External reset. */
#define RSTC_KEY                0xA5000000      /*!< \brief Password. */
/*@}*/

/*! \name Reset Controller Status Register */
/*@{*/
#define RSTC_SR         (RSTC_BASE + 0x04)      /*!< \brief Reset controller status register address. */
#define RSTC_URSTS              0x00000001      /*!< \brief User reset status. */
#define RSTC_BODSTS             0x00000002      /*!< \brief Brownout detection status. */
#define RSTC_RSTTYP             0x00000700      /*!< \brief Reset type. */
#define RSTC_RSTTYP_POWERUP     0x00000000      /*!< \brief Power-up reset. */
#define RSTC_RSTTYP_WAKEUP      0x00000100      /*!< \brief VDDCORE rising. */
#define RSTC_RSTTYP_WATCHDOG    0x00000200      /*!< \brief Watchdog reset. */
#define RSTC_RSTTYP_SOFTWARE    0x00000300      /*!< \brief Software reset. */
#define RSTC_RSTTYP_USER        0x00000400      /*!< \brief User reset. */
#define RSTC_RSTTYP_BROWNOUT    0x00000500      /*!< \brief Brownout reset. */
#define RSTC_NRSTL              0x00010000      /*!< \brief NRST pin level. */
#define RSTC_SRCMP              0x00020000      /*!< \brief Software reset command in progress. */
/*@}*/

/*! \name Reset Controller Mode Register */
/*@{*/
#define RSTC_MR         (RSTC_BASE + 0x08)      /*!< \brief Reset controller mode register address. */
#define RSTC_URSTEN             0x00000001      /*!< \brief User reset enable. */
#define RSTC_URSTIEN            0x00000010      /*!< \brief User reset interrupt enable. */
#define RSTC_ERSTL              0x00000F00      /*!< \brief External reset length. */
#define RSTC_ERSTL_LSB                  8       /*!< \brief Least significant bit of external reset length. */
#define RSTC_BODIEN             0x00010000      /*!< \brief Brown-out detection interrupt enable. */
#define RSTC_KEY_MSK            0xFF000000      /*!< \brief Password mask. */
/*@}*/

/*@} xgNutArchArmAt91Rstc */

#endif                          /* _ARCH_ARM_AT91_RSTC_H_ */
