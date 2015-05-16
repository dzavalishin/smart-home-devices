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
 * \file arch/arm/board/eir1.h
 * \brief Elektor Internet Radio board specific settings.
 *
 * \verbatim
 * $Id$
 * \endverbatim
 */

#ifndef _DEV_BOARD_H_
#error "Do not include this file directly. Use dev/board.h instead!"
#endif

/*
 * Ethernet devices
 */
#ifndef NIC_BASE_ADDR
#define NIC_BASE_ADDR   0x30000000
#endif

#ifndef NIC_SIGNAL_IRQ
#define NIC_SIGNAL_IRQ  INT0
#endif

#ifndef NIC_SIGNAL_PDR
#define NIC_SIGNAL_PDR  PIOB_PDR
#endif

#ifndef NIC_SIGNAL_XSR
#define NIC_SIGNAL_XSR  PIOB_ASR
#endif

#ifndef NIC_SIGNAL_BIT
#define NIC_SIGNAL_BIT  PB20_IRQ0_A
#endif

#include <dev/dm9000.h>

/*
 * RTC chips
 */
#include <dev/pcf8563.h>

#ifndef RTC_CHIP0
#define RTC_CHIP0 rtcPcf8563
#endif

/*
 * MultiMedia Card devices
 */
#include <dev/sbi_mmc.h>

#ifndef DEV_MMCARD0
#define DEV_MMCARD0         devSbi0MmCard0
#endif
