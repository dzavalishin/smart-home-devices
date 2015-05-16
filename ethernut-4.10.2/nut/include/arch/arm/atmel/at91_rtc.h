#ifndef _ARCH_ARM_RTC_H_
#define _ARCH_ARM_RTC_H_
/*
 * Copyright (C) 2006-2007 by egnite Software GmbH. All rights reserved.
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
 * \file arch/arm/at91_rtc.h
 * \brief AT91 real time clock.
 *
 * \verbatim
 * $Id:  $
 * \endverbatim
 */

/*!
 * \addtogroup xgNutArchArmAt91Rtc
 */
/*@{*/

#ifdef RTC_BASE

/*! \name RTC Control Register */
/*@{*/
#define RTC_CR                (RTC_BASE + 0x00)      /*!< \brief RTC controller control register address. */
#define RTC_UPDTIM            0x00000001             /*!< \brief Update Request Time Register. */
#define RTC_UPDCAL            0x00000002             /*!< \brief Peripheral reset. */
#define RTC_TIMEVSEL_MIN      0x00000008             /*!< \brief Time Event Selection - Minute change. */
#define RTC_TIMEVSEL_HR       0x00000008             /*!< \brief Time Event Selection - Hour change. */
#define RTC_TIMEVSEL_MIDN     0x00000008             /*!< \brief Time Event Selection - Every day at midnight. */
#define RTC_TIMEVSEL_NOON     0x00000008             /*!< \brief Time Event Selection - Every day at noon. */
#define RTC_CALEVSEL_WEEK     0x00000000             /*!< \brief Calendar Event Selection - Week change (every Monday at time 00:00:00. */
#define RTC_CALEVSEL_MONTH    0x00000001             /*!< \brief Calendar Event Selection - Month change (every 01 of each month at time 00:00:00. */
#define RTC_CALEVSEL_YEAR     0x00000003             /*!< \brief Calendar Event Selection - Year change (every January 1 at time 00:00:00. */
/*@}*/

/*! \name RTC Mode Register */
/*@{*/
#define RTC_MR                (RTC_BASE + 0x04)      /*!< \brief RTC controller mode register address. */
#define RTC_HRMOD_24          0x00000000             /*!< \brief 24-hour Mode. */
#define RTC_HRMOD_12          0x00000001             /*!< \brief 12-hour Mode. */
/*@}*/

/*! \name RTC Time Register */
/*@{*/
#define RTC_TIMR                (RTC_BASE + 0x08)    /*!< \brief RTC controller time register address. */
#define RTC_SEC                 0x0000007F           /*!< \brief Current second. */
#define RTC_SEC_LSB             0                    /*!< \brief Least significant bit of current second. */
#define RTC_MIN                 0x00007F00           /*!< \brief Current minute. */
#define RTC_MIN_LSB             8                    /*!< \brief Least significant bit of current minute. */
#define RTC_HOUR                0x003F0000           /*!< \brief Current hour. */
#define RTC_HOUR_LSB            16                   /*!< \brief Least significant bit of current hour. */
#define RTC_AM                  0x00000000           /*!< \brief Ante Meridiem Post Meridiem Indicator: AM. */
#define RTC_PM                  0x00400000           /*!< \brief Ante Meridiem Post Meridiem Indicator: PM. */
/*@}*/

/*! \name RTC Calendar Register */
/*@{*/
#define RTC_CALR                (RTC_BASE + 0x0C)    /*!< \brief RTC controller calendar register address. */
#define RTC_CENT                0x0000007F           /*!< \brief Current century. */
#define RTC_CENT_LSB            0                    /*!< \brief Least significant bit of current century. */
#define RTC_YEAR                0x0000FF00           /*!< \brief Current year. */
#define RTC_YEAR_LSB            8                    /*!< \brief Least significant bit of current year. */
#define RTC_MONTH               0x001F0000           /*!< \brief Current month. */
#define RTC_MONTH_LSB           16                   /*!< \brief Least significant bit of current month. */
#define RTC_DAY                 0x00E00000           /*!< \brief Current day (1-7). */
#define RTC_DAY_LSB             21                   /*!< \brief Least significant bit of current day. */
#define RTC_DATE                0x3F000000           /*!< \brief Current day in current month (1-31). */
#define RTC_DATE_LSB            24                   /*!< \brief Least significant bit of current day in current month (1-31). */
/*@}*/

/*! \name RTC Time Alarm Register */
/*@{*/
#define RTC_TIMALR              (RTC_BASE + 0x10)    /*!< \brief RTC controller time alarm register address. */
#define RTC_SECEN             0x00000080           /*!< \brief Second alarm enable. */
#define RTC_MINEN             0x00008000           /*!< \brief Minute alarm enable. */
#define RTC_HOUREN            0x00800000           /*!< \brief Hour alarm enable. */
/*@}*/

/*! \name RTC Calendar Alarm Register */
/*@{*/
#define RTC_CALALR              (RTC_BASE + 0x14)    /*!< \brief RTC controller calendar alarm register address. */
#define RTC_DATEEN              0x80000000           /*!< \brief Second alarm enable. */
#define RTC_MTHEN               0x00800000           /*!< \brief Minute alarm enable. */
/*@}*/

/*! \name RTC Status Register */
/*@{*/
#define RTC_SR                  (RTC_BASE + 0x18)    /*!< \brief RTC controller status register address. */
#define RTC_ACKUPD              0x00000001           /*!< \brief Acknowledge for Update. */
#define RTC_ALARM               0x00000002           /*!< \brief Alarm flag. */
#define RTC_SECEV               0x00000004           /*!< \brief Second event. */
#define RTC_TIMEV               0x00000008           /*!< \brief Time event. */
#define RTC_CALEV               0x00000010           /*!< \brief Calendar event. */
/*@}*/

/*! \name RTC Status Clear Command Register */
/*@{*/
#define RTC_SCCR                (RTC_BASE + 0x1C)    /*!< \brief RTC controller status clear register address. */
#define RTC_ACKCLR              0x00000001           /*!< \brief Acknowledge clear. */
#define RTC_ALRCLR              0x00000002           /*!< \brief Alarm clear. */
#define RTC_SECCLR              0x00000004           /*!< \brief Second clear. */
#define RTC_TIMCLR              0x00000008           /*!< \brief Time clear. */
#define RTC_CALCLR              0x00000010           /*!< \brief Calendar clear. */
/*@}*/

/*! \name RTC Interrupt Enable/Disable/Mask Register */
/*@{*/
#define RTC_IER                (RTC_BASE + 0x20)    /*!< \brief RTC controller interrupt enable register address. */
#define RTC_IDR                (RTC_BASE + 0x24)    /*!< \brief RTC controller interrupt disable status register address. */
#define RTC_IMR                (RTC_BASE + 0x28)    /*!< \brief RTC controller interrupt disable status register address. */
#define RTC_ACKINT             0x00000001           /*!< \brief Acknowledge update interrupt enable. */
#define RTC_ALRINT             0x00000002           /*!< \brief Alarm interrupt enable. */
#define RTC_SECINT             0x00000004           /*!< \brief Second interrupt enable. */
#define RTC_TIMINT             0x00000008           /*!< \brief Time interrupt enable. */
#define RTC_CALINT             0x00000010           /*!< \brief Calendar interrupt enable. */
/*@}*/

/*! \name RTC Valid Entry Register */
/*@{*/
#define RTC_VER                (RTC_BASE + 0x2C)    /*!< \brief RTC controller valid entry register address. */
#define RTC_NVTIM              0x00000001           /*!< \brief Non-Valid time. */
#define RTC_NVCAL              0x00000002           /*!< \brief Non-Valid calendar. */
#define RTC_NVTIMALR           0x00000004           /*!< \brief Non-Valid time alarm. */
#define RTC_NVCALALR           0x00000008           /*!< \brief Non-Valid calendar alarm. */
/*@}*/

#endif

/*@} xgNutArchArmAt91Rtc */

#endif                          /* _ARCH_ARM_RTC_H_ */
