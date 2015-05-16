/*
 * Copyright 2011 by egnite GmbH
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

/*
 * \file arch/arm/board/ethernut3.h
 * \brief Ethernut 3 board specific settings.
 *
 * \verbatim
 * $Id$
 * \endverbatim
 */

#ifndef _DEV_BOARD_H_
#error "Do not include this file directly. Use dev/board.h instead!"
#endif

/*
 * Determine old board version
 */
#include <cfg/eeprom.h>
#if defined(NUT_CONFIG_X12RTC)
#define ETHERNUT3_0
#endif

/*
 * Console devices
 */
#ifndef DEV_CONSOLE
#ifdef NUT_DEV_DEBUG_READ
#define DEV_CONSOLE DEV_DEBUG
#endif
#endif

/*
 * Ethernet devices
 */
#ifndef NIC_BASE_ADDR
#define NIC_BASE_ADDR   0x20000000
#endif

#ifndef NIC_SIGNAL_IRQ
#define NIC_SIGNAL_IRQ  INT1
#endif

#ifndef NIC_SIGNAL_PDR
#define NIC_SIGNAL_PDR  PIO_PDR
#endif

#ifndef NIC_SIGNAL_BIT
#define NIC_SIGNAL_BIT  10
#endif

#include <dev/dm9000e.h>

/*
 * SPI bus controller devices
 */
#include <dev/spibus_npl.h>

#ifndef DEV_SPIBUS0
#define DEV_SPIBUS0 spiBusNpl
#endif

/*
 * RTC chips
 */
#ifdef ETHERNUT3_0
#include <dev/x12rtc.h>
#else
#include <dev/pcf8563.h>
#endif

#ifndef RTC_CHIP0
#ifdef ETHERNUT3_0
#define RTC_CHIP0 rtcX12x6
#else
#define RTC_CHIP0 rtcPcf8563
#endif
#endif

/*
 * MultiMedia Card devices
 */
#include <dev/nplmmc.h>

#ifndef DEV_MMCARD0
#define DEV_MMCARD0 devNplMmc0
#endif
