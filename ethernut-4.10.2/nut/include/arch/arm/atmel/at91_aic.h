#ifndef _ARCH_ARM_AT91_AIC_H_
#define _ARCH_ARM_AT91_AIC_H_

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
 * \file arch/arm/at91_aic.h
 * \brief AT91 peripherals.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.2  2006/08/31 19:08:14  haraldkipp
 * Defining register offsets simplifies assembly programming.
 *
 * Revision 1.1  2006/07/05 07:45:25  haraldkipp
 * Split on-chip interface definitions.
 *
 *
 * \endverbatim
 */

/*!
 * \addtogroup xgNutArchArmAt91Aic
 */
/*@{*/

/*! \name Interrupt Source Mode Registers */
/*@{*/
/*! \brief Source mode register array.
 */
#define AIC_SMR(i)  (AIC_BASE + i * 4)

/*! \brief Priority mask. 
 *
 * Priority levels can be between 0 (lowest) and 7 (highest).
 */
#define AIC_PRIOR                       0x00000007

/*! \brief Interrupt source type mask. 
 *
 * Internal interrupts can level sensitive or edge triggered.
 *
 * External interrupts can triggered on positive or negative levels or 
 * on rising or falling edges.
 */
#define AIC_SRCTYPE                     0x00000060

#define AIC_SRCTYPE_INT_LEVEL_SENSITIVE 0x00000000      /*!< \brief Internal level sensitive. */
#define AIC_SRCTYPE_INT_EDGE_TRIGGERED  0x00000020      /*!< \brief Internal edge triggered. */
#define AIC_SRCTYPE_EXT_LOW_LEVEL       0x00000000      /*!< \brief External low level. */
#define AIC_SRCTYPE_EXT_NEGATIVE_EDGE   0x00000020      /*!< \brief External falling edge. */
#define AIC_SRCTYPE_EXT_HIGH_LEVEL      0x00000040      /*!< \brief External high level. */
#define AIC_SRCTYPE_EXT_POSITIVE_EDGE   0x00000060      /*!< \brief External rising edge. */
/*@}*/

/*! \name Interrupt Source Vector Registers */
/*@{*/
/*! \brief Source vector register array. 
 *
 * Stores the addresses of the corresponding interrupt handlers.
 */
#define AIC_SVR(i)  (AIC_BASE + 0x80 + i * 4)
/*@}*/

/*! \name Interrupt Vector Register */
/*@{*/
#define AIC_IVR_OFF                 0x00000100  /*!< \brief IRQ vector register offset. */
#define AIC_IVR     (AIC_BASE + AIC_IVR_OFF)    /*!< \brief IRQ vector register address. */
/*@}*/

/*! \name Fast Interrupt Vector Register */
/*@{*/
#define AIC_FVR_OFF                 0x00000104  /*!< \brief FIQ vector register offset. */
#define AIC_FVR     (AIC_BASE + AIC_FVR_OFF)    /*!< \brief FIQ vector register address. */
/*@}*/

/*! \name Interrupt Status Register */
/*@{*/
#define AIC_ISR_OFF                 0x00000108  /*!< \brief Interrupt status register offset. */
#define AIC_ISR     (AIC_BASE + AIC_ISR_OFF)    /*!< \brief Interrupt status register address. */
#define AIC_IRQID                   0x0000001F  /*!< \brief Current interrupt identifier mask. */
/*@}*/

/*! \name Interrupt Pending Register */
/*@{*/
#define AIC_IPR_OFF                 0x0000010C  /*!< \brief Interrupt pending register offset. */
#define AIC_IPR     (AIC_BASE + AIC_IPR_OFF)    /*!< \brief Interrupt pending register address. */
/*@}*/

/*! \name Interrupt Mask Register */
/*@{*/
#define AIC_IMR_OFF                 0x00000110  /*!< \brief Interrupt mask register offset. */
#define AIC_IMR     (AIC_BASE + AIC_IMR_OFF)    /*!< \brief Interrupt mask register address. */
/*@}*/

/*! \name Interrupt Core Status Register */
/*@{*/
#define AIC_CISR_OFF                0x00000114  /*!< \brief Core interrupt status register offset. */
#define AIC_CISR    (AIC_BASE + AIC_CISR_OFF)   /*!< \brief Core interrupt status register address. */
#define AIC_NFIQ                    0x00000001  /*!< \brief Core FIQ Status */
#define AIC_NIRQ                    0x00000002  /*!< \brief Core IRQ Status */
/*@}*/

/*! \name Interrupt Enable Command Register */
/*@{*/
#define AIC_IECR_OFF                0x00000120  /*!< \brief Interrupt enable command register offset. */
#define AIC_IECR    (AIC_BASE + AIC_IECR_OFF)   /*!< \brief Interrupt enable command register address. */
/*@}*/

/*! \name Interrupt Disable Command Register */
/*@{*/
#define AIC_IDCR_OFF                0x00000124  /*!< \brief Interrupt disable command register offset. */
#define AIC_IDCR    (AIC_BASE + AIC_IDCR_OFF)   /*!< \brief Interrupt disable command register address. */
/*@}*/

/*! \name Interrupt Clear Command Register */
/*@{*/
#define AIC_ICCR_OFF                0x00000128  /*!< \brief Interrupt clear command register offset. */
#define AIC_ICCR    (AIC_BASE + AIC_ICCR_OFF)   /*!< \brief Interrupt clear command register address. */
/*@}*/

/*! \name Interrupt Set Command Register */
/*@{*/
#define AIC_ISCR_OFF                0x0000012C  /*!< \brief Interrupt set command register offset. */
#define AIC_ISCR    (AIC_BASE + AIC_ISCR_OFF)   /*!< \brief Interrupt set command register address. */
/*@}*/

/*! \name End Of Interrupt Command Register */
/*@{*/
#define AIC_EOICR_OFF               0x00000130  /*!< \brief End of interrupt command register offset. */
#define AIC_EOICR   (AIC_BASE + AIC_EOICR_OFF)  /*!< \brief End of interrupt command register address. */
/*@}*/

/*! \name Spurious Interrupt Vector Register */
/*@{*/
#define AIC_SPU_OFF                 0x00000134  /*!< \brief Spurious vector register offset. */
#define AIC_SPU     (AIC_BASE + AIC_SPU_OFF)    /*!< \brief Spurious vector register address. */
/*@}*/

/*! \name Debug Control Register */
/*@{*/
#define AIC_DCR_OFF                 0x0000138   /*!< \brief Debug control register offset. */
#define AIC_DCR     (AIC_BASE + AIC_DCR_OFF)    /*!< \brief Debug control register address. */
/*@}*/

/*! \name Fast Forcing Enable Register */
/*@{*/
#define AIC_FFER_OFF                0x00000140  /*!< \brief Fast forcing enable register offset. */
#define AIC_FFER    (AIC_BASE + AIC_FFER_OFF)   /*!< \brief Fast forcing enable register address. */
/*@}*/

/*! \name Fast Forcing Disable Register */
/*@{*/
#define AIC_FFDR_OFF                0x00000144  /*!< \brief Fast forcing disable register address. */
#define AIC_FFDR    (AIC_BASE + AIC_FFDR_OFF)   /*!< \brief Fast forcing disable register address. */
/*@}*/

/*! \name Fast Forcing Status Register */
/*@{*/
#define AIC_FFSR_OFF                0x00000148  /*!< \brief Fast forcing status register address. */
#define AIC_FFSR    (AIC_BASE + AIC_FFSR_OFF)   /*!< \brief Fast forcing status register address. */
/*@}*/

/*@} xgNutArchArmAt91Aic */

#endif                          /* _ARCH_ARM_AT91_AIC_H_ */
