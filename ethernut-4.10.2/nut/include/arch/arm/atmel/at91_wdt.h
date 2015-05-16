#ifndef _ARCH_ARM_AT91_WDT_H_
#define _ARCH_ARM_AT91_WDT_H_

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
 * \file arch/arm/at91_wdt.h
 * \brief AT91 peripherals.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.3  2006/08/31 19:13:44  haraldkipp
 * Added LSB definitions for counter and delta values.
 *
 * Revision 1.2  2006/07/18 14:06:18  haraldkipp
 * Changed coding style to follow existing headers.
 * Removed old lines, which had been derived from at91_wd.h.
 * Corrected register addresses, thanks to Jix.
 * Register names changed, now following the datasheet.
 *
 * Revision 1.1  2006/07/05 07:45:28  haraldkipp
 * Split on-chip interface definitions.
 *
 *
 * \endverbatim
 */

/*!
 * \addtogroup xgNutArchArmAt91Wdt
 */
/*@{*/

/*! \name Watch Dog Control Register */
/*@{*/
#define WDT_CR_OFF          0x00000000  /*!< \brief Watchdog control register offset. */
#define WDT_CR  (WDT_BASE + WDT_CR_OFF) /*!< \brief Watchdog control register address. */
#define WDT_WDRSTT          0x00000001  /*!< \brief Watchdog restart. */
#define WDT_KEY             0xA5000000  /*!< \brief Watchdog password. */
/*@}*/

/*! \name Watch Dog Mode Register */
/*@{*/
#define WDT_MR_OFF          0x00000004  /*!< \brief Mode register offset. */
#define WDT_MR  (WDT_BASE + WDT_MR_OFF) /*!< \brief Mode register address. */
#define WDT_WDV             0x00000FFF  /*!< \brief Counter value mask. */
#define WDT_WDV_LSB                 0   /*!< \brief Counter value LSB. */
#define WDT_WDFIEN          0x00001000  /*!< \brief Fault interrupt enable. */
#define WDT_WDRSTEN         0x00002000  /*!< \brief Reset enable. */
#define WDT_WDRPROC         0x00004000  /*!< \brief Eset processor enable. */
#define WDT_WDDIS           0x00008000  /*!< \brief Watchdog disable. */
#define WDT_WDD             0x0FFF0000  /*!< \brief Delta value mask. */
#define WDT_WDD_LSB                 16  /*!< \brief Delta value LSB. */
#define WDT_WDDBGHLT        0x10000000  /*!< \brief Watchdog debug halt. */
#define WDT_WDIDLEHLT       0x20000000  /*!< \brief Watchdog idle halt. */
/*@}*/

/*! \name Watch Dog Status Register */
/*@{*/
#define WDT_SR_OFF          0x00000008  /*!< \brief Status register offset. */
#define WDT_SR  (WDT_BASE + WDT_SR_OFF) /*!< \brief Status register address. */
#define WDT_WDUNF           0x00000001  /*!< \brief Watchdog underflow. */
#define WDT_WDERR           0x00000002  /*!< \brief Watchdog error. */
/*@}*/

/*@} xgNutArchArmAt91Wdt */

#endif                          /* _ARCH_ARM_AT91_WDT_H_ */
