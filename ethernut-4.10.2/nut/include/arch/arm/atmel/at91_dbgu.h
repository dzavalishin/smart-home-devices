#ifndef _ARCH_ARM_AT91_DBGU_H_
#define _ARCH_ARM_AT91_DBGU_H_

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
 * \file arch/arm/at91_dbgu.h
 * \brief AT91 peripherals.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.2  2006/08/05 11:54:45  haraldkipp
 * PDC registers added.
 *
 * Revision 1.1  2006/07/05 07:45:25  haraldkipp
 * Split on-chip interface definitions.
 *
 *
 * \endverbatim
 */

/*!
 * \addtogroup xgNutArchArmAt91Dbgu
 */
/*@{*/

#define DBGU_CR     (DBGU_BASE + US_CR_OFF)     /*!< \brief DBGU control register address. */
#define DBGU_MR     (DBGU_BASE + US_MR_OFF)     /*!< \brief DBGU mode register address. */
#define DBGU_IER    (DBGU_BASE + US_IER_OFF)    /*!< \brief DBGU interrupt enable register address. */
#define DBGU_IDR    (DBGU_BASE + US_IDR_OFF)    /*!< \brief DBGU interrupt disable register address. */
#define DBGU_IMR    (DBGU_BASE + US_IMR_OFF)    /*!< \brief DBGU interrupt mask register address. */
#define DBGU_SR     (DBGU_BASE + US_CSR_OFF)    /*!< \brief DBGU status register address. */
#define DBGU_RHR    (DBGU_BASE + US_RHR_OFF)    /*!< \brief DBGU receiver holding register address. */
#define DBGU_THR    (DBGU_BASE + US_THR_OFF)    /*!< \brief DBGU transmitter holding register address. */
#define DBGU_BRGR   (DBGU_BASE + US_BRGR_OFF)   /*!< \brief DBGU baud rate register address. */

#define DBGU_CIDR_OFF           0x00000040      /*!< \brief DBGU chip ID register offset. */
#define DBGU_CIDR   (DBGU_BASE + DBGU_CIDR_OFF) /*!< \brief DBGU chip ID register. */

#define DBGU_EXID_OFF           0x00000044      /*!< \brief DBGU chip ID extension register offset. */
#define DBGU_EXID   (DBGU_BASE + DBGU_EXID_OFF) /*!< \brief DBGU chip ID extension register. */

#define DBGU_FNR_OFF            0x00000048      /*!< \brief DBGU force NTRST register offset. */
#define DBGU_FNR    (DBGU_BASE + DBGU_FNR_OFF)  /*!< \brief DBGU force NTRST register. */

#if defined(DBGU_HAS_PDC)
#define DBGU_RPR    (DBGU_BASE + PERIPH_RPR_OFF)    /*!< \brief PDC receive pointer register. */
#define DBGU_RCR    (DBGU_BASE + PERIPH_RCR_OFF)    /*!< \brief PDC receive counter register. */
#define DBGU_TPR    (DBGU_BASE + PERIPH_TPR_OFF)    /*!< \brief PDC transmit pointer register. */
#define DBGU_TCR    (DBGU_BASE + PERIPH_TCR_OFF)    /*!< \brief PDC transmit counter register. */
#define DBGU_RNPR   (DBGU_BASE + PERIPH_RNPR_OFF)   /*!< \brief PDC receive next pointer register. */
#define DBGU_RNCR   (DBGU_BASE + PERIPH_RNCR_OFF)   /*!< \brief PDC receive next counter register. */
#define DBGU_TNPR   (DBGU_BASE + PERIPH_TNPR_OFF)   /*!< \brief PDC transmit next pointer register. */
#define DBGU_TNCR   (DBGU_BASE + PERIPH_TNCR_OFF)   /*!< \brief PDC transmit next counter register. */
#define DBGU_PTCR   (DBGU_BASE + PERIPH_PTCR_OFF)   /*!< \brief PDC transfer control register. */
#define DBGU_PTSR   (DBGU_BASE + PERIPH_PTSR_OFF)   /*!< \brief PDC transfer status register. */
#endif

/*@} xgNutArchArmAt91Dbgu */

#endif                          /* _ARCH_ARM_AT91_DBGU_H_ */
