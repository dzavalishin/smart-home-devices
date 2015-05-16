/*
 * Copyright (C) 2006 by egnite Software GmbH
 * Copyright (C) 2008 by egnite GmbH
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
 * \brief Low Level Multimedia Card Access.
 *
 * Low level MMC hardware routines for SPI emulation by software
 * (bit banging).
 *
 * These routines support SPI mode only and are required by the
 * basic MMC driver.
 *
 * \verbatim
 * $Id: sbi0mmc0.c 2349 2008-10-23 08:52:20Z haraldkipp $
 * \endverbatim
 */

#include <cfg/arch.h>
#include <cfg/arch/gpio.h>

#include <sys/event.h>
#include <dev/irqreg.h>
#include <dev/mmcard.h>
#include <dev/sbimmc.h>

#if 0
/* Use for local debugging. */
#define NUTDEBUG
#include <stdio.h>
#endif

/*!
 * \addtogroup xgSbiMmCard
 */
/*@{*/

#if defined(MMC0_CLK_PIO_BIT)
#define MMC_CLK_PIO_BIT     MMC0_CLK_PIO_BIT
#endif
#if defined(MMC0_CLK_PIO_ID)
#define MMC_CLK_PIO_ID      MMC0_CLK_PIO_ID
#endif

#if defined(MMC0_MOSI_PIO_BIT)
#define MMC_MOSI_PIO_BIT    MMC0_MOSI_PIO_BIT
#endif
#if defined(MMC0_MOSI_PIO_ID)
#define MMC_MOSI_PIO_ID     MMC0_MOSI_PIO_ID
#endif

#if defined(MMC0_MISO_PIO_BIT)
#define MMC_MISO_PIO_BIT    MMC0_MISO_PIO_BIT
#endif
#if defined(MMC0_MISO_PIO_ID)
#define MMC_MISO_PIO_ID     MMC0_MISO_PIO_ID
#endif

#if defined(MMC0_CS0_PIO_BIT)
#define MMC_CS_PIO_BIT      MMC0_CS0_PIO_BIT
#endif
#if defined(MMC0_CS0_PIO_ID)
#define MMC_CS_PIO_ID       MMC0_CS0_PIO_ID
#endif

#if defined(MMC0_CD0_PIO_BIT)
#define MMC_CD_PIO_BIT      MMC0_CD0_PIO_BIT
#endif
#if defined(MMC0_CD0_PIO_ID)
#define MMC_CD_PIO_ID       MMC0_CD0_PIO_ID
#endif

#if defined(MMC0_WP0_PIO_BIT)
#define MMC_WP_PIO_BIT      MMC0_WP0_PIO_BIT
#endif
#if defined(MMC0_WP0_PIO_ID)
#define MMC_WP_PIO_ID       MMC0_WP0_PIO_ID
#endif

#define MMC_DEV_NAME    {'M', 'M', 'C', '0', 0, 0, 0, 0, 0}

/*@}*/

#define devSbiMmCard    devSbi0MmCard0
#include "sbi_mmc.c"

