#ifndef _ARCH_ARM_AT91X40_H_
#define _ARCH_ARM_AT91X40_H_

/*
 * Copyright (C) 2005 by egnite Software GmbH. All rights reserved.
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
 * \file arch/arm/at91x40.h
 * \brief AT91 peripherals.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.5  2008/07/31 09:43:25  haraldkipp
 * Initializing peripheral control registers in a more general way.
 * Fixes bug #2032960.
 *
 * Revision 1.4  2006/10/08 16:48:09  haraldkipp
 * Documentation fixed
 *
 * Revision 1.3  2006/08/05 11:56:29  haraldkipp
 * Old SAM7X leftovers finally removed.
 * PDC register configuration added.
 *
 * Revision 1.2  2006/08/01 07:35:59  haraldkipp
 * Exclude function prototypes when included by assembler.
 *
 * Revision 1.1  2006/07/05 07:45:28  haraldkipp
 * Split on-chip interface definitions.
 *
 * Revision 1.7  2006/06/28 17:22:34  haraldkipp
 * Make it compile for AT91SAM7X256.
 *
 * Revision 1.6  2006/05/25 09:09:57  haraldkipp
 * API documentation updated and corrected.
 *
 * Revision 1.5  2006/04/07 12:57:00  haraldkipp
 * Fast interrupt doesn't require to store R8-R12.
 *
 * Revision 1.4  2006/03/02 20:02:56  haraldkipp
 * Added ICCARM interrupt entry code. Probably not working, because I
 * excluded an immediate load.
 *
 * Revision 1.3  2006/01/05 16:52:49  haraldkipp
 * Baudrate calculation is now based on NutGetCpuClock().
 * The AT91_US_BAUD macro had been marked deprecated.
 *
 * Revision 1.2  2005/11/20 14:44:14  haraldkipp
 * Register offsets added.
 *
 * Revision 1.1  2005/10/24 10:31:13  haraldkipp
 * Moved from parent directory.
 *
 *
 * \endverbatim
 */

/*! \addtogroup xgNutArchArmAt91x40 */
/*@{*/

/*! \name Peripheral Identifiers and Interrupts */
/*@{*/
#define FIQ_ID      0           /*!< \brief Fast interrupt ID. */
#define SWIRQ_ID    1           /*!< \brief Software interrupt ID. */
#define US0_ID      2           /*!< \brief USART 0 ID. */
#define US1_ID      3           /*!< \brief USART 1 ID. */
#define TC0_ID      4           /*!< \brief Timer 0 ID. */
#define TC1_ID      5           /*!< \brief Timer 1 ID. */
#define TC2_ID      6           /*!< \brief Timer 2 ID. */
#define WDI_ID      7           /*!< \brief Watchdog interrupt ID. */
#define PIO_ID      8           /*!< \brief Parallel I/O controller ID. */
#define IRQ0_ID     16          /*!< \brief External interrupt 0 ID. */
#define IRQ1_ID     17          /*!< \brief External interrupt 1 ID. */
#define IRQ2_ID     18          /*!< \brief External interrupt 2 ID. */
/*@}*/

#define EBI_BASE        0xFFE00000      /*!< \brief EBI base address. */
#define SF_BASE         0xFFF00000      /*!< \brief Special function register base address. */
#define USART1_BASE     0xFFFCC000      /*!< \brief USART 1 base address. */
#define USART0_BASE     0xFFFD0000      /*!< \brief USART 0 base address. */
#define TC_BASE         0xFFFE0000      /*!< \brief TC base address. */
#define PIO_BASE        0xFFFF0000      /*!< \brief PIO base address. */
#define PS_BASE         0xFFFF4000      /*!< \brief PS base address. */
#define WD_BASE         0xFFFF8000      /*!< \brief Watch Dog register base address. */
#define AIC_BASE        0xFFFFF000      /*!< AIC base address. */

#define PERIPH_RPR_OFF  0x00000030      /*!< \brief Receive pointer register offset. */
#define PERIPH_RCR_OFF  0x00000034      /*!< \brief Receive counter register offset. */
#define PERIPH_TPR_OFF  0x00000038      /*!< \brief Transmit pointer register offset. */
#define PERIPH_TCR_OFF  0x0000003C      /*!< \brief Transmit counter register offset. */

#define USART_HAS_PDC

#include <arch/arm/atmel/at91_ebi.h>
#include <arch/arm/atmel/at91_sf.h>
#include <arch/arm/atmel/at91_us.h>
#include <arch/arm/atmel/at91_tc.h>
#include <arch/arm/atmel/at91_pio.h>
#include <arch/arm/atmel/at91_ps.h>
#include <arch/arm/atmel/at91_wd.h>
#include <arch/arm/atmel/at91_aic.h>

/*! \name USART Peripheral Multiplexing */
/*@{*/
#define P15_RXD0            15
#define P14_TXD0            14
#define P13_SCK0            13

#define P22_RXD1            22
#define P21_TXD1            21
#define P20_SCK1            20
/*@}*/

/*@} xgNutArchArmAt91 */

#ifndef __ASSEMBLER__
extern void McuInit(void);
#endif

#endif                          /* _ARCH_ARM_AT91X40_H_ */
