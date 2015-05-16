#ifndef _ARCH_ARM_AT91_WD_H_
#define _ARCH_ARM_AT91_WD_H_

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
 * \file arch/arm/at91_wd.h
 * \brief AT91 peripherals.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.1  2006/07/05 07:45:28  haraldkipp
 * Split on-chip interface definitions.
 *
 *
 * \endverbatim
 */

/*!
 * \addtogroup xgNutArchArmAt91Wd
 */
/*@{*/

/*! \name Watch Dog Overflow Mode Register */
/*@{*/
#define WD_OMR          (WD_BASE + 0x00)        /*!< \brief Overflow mode register address. */
#define WD_WDEN                 0x00000001      /*!< \brief Watch Dog enable. */
#define WD_RSTEN                0x00000002      /*!< \brief Internal reset enable. */
#define WD_IRQEN                0x00000004      /*!< \brief Interrupt enable. */
#define WD_EXTEN                0x00000008      /*!< \brief External signal enable. */
#define WD_OKEY                 0x00002340      /*!< \brief Overflow mode register access key. */
/*@}*/

/*! \name Watch Dog Clock Register */
/*@{*/
#define WD_CMR          (WD_BASE + 0x04)        /*!< \brief Clock mode register address. */
#define WD_WDCLKS               0x00000003      /*!< \brief Clock selection mask. */
#define WD_WDCLKS_MCK8          0x00000000      /*!< \brief Selects MCK/8. */
#define WD_WDCLKS_MCK32         0x00000001      /*!< \brief Selects MCK/32. */
#define WD_WDCLKS_MCK128        0x00000002      /*!< \brief Selects MCK/128. */
#define WD_WDCLKS_MCK1024       0x00000003      /*!< \brief Selects MCK/1024. */
#define WD_HPCV                 0x0000003C      /*!< \brief High preload counter value. */
#define WD_CKEY                 (0x06E<<7)      /*!< \brief Clock register access key. */
/*@}*/

/*! \name Watch Dog Control Register */
/*@{*/
#define WD_CR           (WD_BASE + 0x08)        /*!< \brief Control register address. */
#define WD_RSTKEY               0x0000C071      /*!< \brief Watch Dog restart key. */
/*@}*/

/*! \name Watch Dog Status Register */
/*@{*/
#define WD_SR           (WD_BASE + 0x0C)        /*!< \brief Status register address. */
#define WD_WDOVF                0x00000001      /*!< \brief Watch Dog overflow status. */
/*@}*/

/*@} xgNutArchArmAt91Wd */

#endif                          /* _ARCH_ARM_AT91_WD_H_ */

