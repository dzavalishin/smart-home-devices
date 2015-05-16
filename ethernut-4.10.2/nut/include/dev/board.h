#ifndef _DEV_BOARD_H_
#define _DEV_BOARD_H_

/*
 * Copyright (C) 2001-2007 by egnite Software GmbH. All rights reserved.
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

/*
 * $Log$
 * Revision 1.20  2009/09/20 13:24:58  ulrichprinz
 * Added limited USART support for DBGU.
 *
 * Revision 1.19  2009/03/08 20:19:34  haraldkipp
 * Fixed missing UART device for AVR.
 *
 * Revision 1.18  2009/03/05 22:16:57  freckle
 * use __NUT_EMULATION instead of __APPLE__, __linux__, or __CYGWIN__
 *
 * Revision 1.17  2009/01/09 17:54:28  haraldkipp
 * Added SPI bus controller for AVR and AT91.
 *
 * Revision 1.16  2008/10/23 08:56:56  haraldkipp
 * Added default MMC interfaces.
 *
 * Revision 1.15  2008/08/27 07:01:10  thornen
 * Added:
 *  - RTL          support for MMnet01..04
 *  - LANC111 support for MMnet101..104
 *  - RTC         support for MMnet02..04 and MMnet102..104
 *
 * Revision 1.14  2008/08/26 17:36:45  haraldkipp
 * Revoked changes 2008/08/26 by thornen.
 *
 * Revision 1.12  2008/08/06 12:51:12  haraldkipp
 * Added support for Ethernut 5 (AT91SAM9XE reference design).
 *
 * Revision 1.11  2008/02/15 17:09:44  haraldkipp
 * Added support for the Elektor Internet Radio.
 *
 * Revision 1.10  2007/10/08 06:37:51  hwmaier
 * Added RTC DS1307 for XNUT-100 and XNUT-105 units
 *
 * Revision 1.9  2007/10/04 20:32:24  olereinhardt
 * Support for SAM7S256 added
 *
 * Revision 1.8  2007/06/03 08:51:30  haraldkipp
 * RTC_CHIP definition now visible for ETHERNUT3 and MMNET02 only.
 *
 * Revision 1.7  2006/10/05 17:18:49  haraldkipp
 * Hardware independant RTC layer added.
 *
 * Revision 1.6  2006/08/31 19:01:08  haraldkipp
 * Using devDebug2 for the DBGU output was a bad idea. Some AT91 chips
 * provide more than two UARTs. We now use devDebug to specify the DBGU
 * device. Baudrate calculations failed on CPUs running on a processor
 * clock, which differs from a futher divided main clock. This had been
 * fixed.
 *
 * Revision 1.5  2006/07/05 07:45:29  haraldkipp
 * Split on-chip interface definitions.
 *
 * Revision 1.4  2006/06/28 17:17:50  haraldkipp
 * Added initial support for Atmel's AT91SAM7X-EK.
 *
 * Revision 1.3  2006/02/23 15:34:00  haraldkipp
 * Support for Philips LPC2xxx Family and LPC-E2294 Board from Olimex added.
 * Many thanks to Michael Fischer for this port.
 *
 * Revision 1.2  2005/11/22 09:19:03  haraldkipp
 * Include condition corrected.
 *
 * Revision 1.1  2005/11/20 14:43:38  haraldkipp
 * First check-in
 *
 */

#include <cfg/arch.h>
#include <cfg/uart.h>

#if defined(ELEKTOR_IR1)
#include <arch/arm/board/elektor_ir1.h>
#elif defined(ETHERNUT1)
#include <arch/avr/board/ethernut1.h>
#elif defined(ETHERNUT2)
#include <arch/avr/board/ethernut2.h>
#elif defined(ETHERNUT3)
#include <arch/arm/board/ethernut3.h>
#elif defined(ETHERNUT5)
#include <arch/arm/board/ethernut5.h>
#endif

/*
 * Debug device.
 */
#include <dev/debug.h>

#ifndef DEV_DEBUG
#if defined(OLIMEX_LPCE2294) || defined(HHOPEN_63F) || defined(EVK1104)
#define DEV_DEBUG       devDebug1
#elif defined(DBGU_BASE)
#define DEV_DEBUG       devDebug
#else
#define DEV_DEBUG       devDebug0
#endif
#endif

#ifndef DEV_DEBUG_NAME
#if defined(GBAXPORT2)
#define DEV_DEBUG_NAME  "con"
#elif defined(OLIMEX_LPCE2294) || defined(HHOPEN_63F) || defined(EVK1104)
#define DEV_DEBUG_NAME  "uart1"
#elif defined(DBGU_BASE)
#define DEV_DEBUG_NAME  "dbgu"
#else
#define DEV_DEBUG_NAME  "uart0"
#endif
#endif

/*
 * UART device.
 */
#if defined(__AVR__) || defined(__NUT_EMULATION__)

#include <dev/usartavr.h>

#ifndef DEV_UART0
#define DEV_UART0       devUsartAvr0
#endif

#ifndef DEV_UART1
#define DEV_UART1       devUsartAvr1
#endif

#define DEV_UART1_NAME  "uart1"

#elif defined(MCU_AT91)

#include <dev/usartat91.h>

#ifndef DEV_UART0
#define DEV_UART0       devUsartAt910
#endif

#ifndef DEV_UART1
#define DEV_UART1       devUsartAt911
#endif

#ifndef DEV_UARTD
#define DEV_UARTD       devDbguAt91
#endif

#elif defined(__AVR32__)

#include <dev/usartavr32.h>

#ifndef DEV_UART0
#define DEV_UART0       devUsartAvr320
#endif

#ifndef DEV_UART1
#define DEV_UART1       devUsartAvr321
#endif

#ifndef DEV_UART2
#define DEV_UART2       devUsartAvr322
#endif

#if defined(EVK1104) && !defined(DEV_UART)
#define DEV_UART        DEV_UART1
#define DEV_UART_NAME   DEV_UART1_NAME
#endif

#elif defined(GBAXPORT2)

#define DEV_UART        DEV_DEBUG
#define DEV_UART_NAME   DEV_DEBUG_NAME

#endif

#ifndef DEV_UART0
#define DEV_UART0       devUart0
#endif
#ifndef DEV_UART0_NAME
#define DEV_UART0_NAME  "uart0"
#endif

#if defined(DEV_UART1) && !defined(DEV_UART1_NAME)
#define DEV_UART1_NAME  "uart1"
#endif

#if defined(DEV_UART2) && !defined(DEV_UART2_NAME)
#define DEV_UART2_NAME  "uart2"
#endif

#if defined(DEV_UARTD) && !defined(DEV_UARTD_NAME)
#define DEV_UARTD_NAME  "uartd"
#endif

#ifndef DEV_UART
#define DEV_UART        DEV_UART0
#endif
#ifndef DEV_UART_NAME
#define DEV_UART_NAME   DEV_UART0_NAME
#endif

/*
 * Console devices.
 */
#ifndef DEV_CONSOLE
#ifdef NUT_DEV_DEBUG_READ
#define DEV_CONSOLE      DEV_DEBUG
#ifndef DEV_CONSOLE_NAME
#define DEV_CONSOLE_NAME DEV_DEBUG_NAME
#endif
#else
#define DEV_CONSOLE      DEV_UART
#ifndef DEV_CONSOLE_NAME
#define DEV_CONSOLE_NAME DEV_UART_NAME
#endif
#endif
#endif

/*
 * Ethernet device.
 */
#if defined(CHARON2) || defined(XNUT_100) || defined(XNUT_105) ||\
	defined(MMNET01) || defined(MMNET02) || defined(MMNET03) || defined(MMNET04) ||\
    defined(ARTHERNET1)
#include <dev/nicrtl.h>
#elif defined(MMNET101) || defined(MMNET102) || defined(MMNET103) || defined(MMNET104)
#include <dev/lanc111.h>
#elif defined(OLIMEX_LPCE2294)
#include <dev/cs8900a.h>
#elif defined(AT91SAM7X_EK) || defined(AT91SAM9260_EK) || defined(MORPHOQ1) || defined(ENET_SAM7X)
#include <dev/at91_emac.h>
#elif defined(EVK1100) || defined(EVK1105)
#include <dev/avr32_macb.h>
#endif

#ifndef DEV_ETHER
#include <dev/null_ether.h>
#endif
#ifndef DEV_ETHER_NAME
#define DEV_ETHER_NAME  "eth0"
#endif

/*
 * SPI bus.
 */
#ifndef DEV_SPIBUS
#if defined(__AVR__)
#include <dev/spibus_avr.h>
#define DEV_SPIBUS0     spiBus0Avr
#elif defined(MCU_AT91) && defined(SPI0_BASE)
#include <dev/spibus_at91.h>
#define DEV_SPIBUS0     spiBus0At91
#elif defined(__AVR32__)
#include <dev/spibus_avr32.h>
#define DEV_SPIBUS0		spiBus0Avr32
#define DEV_SPIBUS1		spiBus1Avr32
#endif
#endif /* DEV_SPIBUS */
#ifndef DEV_SPIBUS
#if defined(DEV_SPIBUS0)
#define DEV_SPIBUS      DEV_SPIBUS0
#elif defined(DEV_SPIBUS1)
#define DEV_SPIBUS      DEV_SPIBUS1
#endif
#endif

/*
 * RTC chip.
 */
#if defined(XNUT_100) || defined(XNUT_105) ||\
	  defined(MMNET102) || defined(MMNET103) || defined(MMNET104) ||\
	  defined(MMNET02)  || defined(MMNET03)  || defined(MMNET04)
#define RTC_CHIP0 rtcDs1307
#include <dev/ds1307rtc.h>
#elif defined(ENET_SAM7X)
#define RTC_CHIP0 rtcPcf8563
#include <dev/pcf8563.h>
#endif

#ifdef RTC_CHIP0
#ifndef RTC_CHIP
#define RTC_CHIP RTC_CHIP0
#endif
#endif

/*
 * MultiMedia Card.
 */
#if defined(AT91SAM7X_EK)
#define DEV_MMCARD0         devAt91SpiMmc0
#include <dev/spimmc_at91.h>
#elif defined(AT91SAM9260_EK)
#define DEV_MMCARD0         devAt91Mci0
#define DEV_MMCARD0_NAME    "MCI0"
#include <dev/at91_mci.h>
#elif defined (ENET_SAM7X)
#include <dev/spi_mmc_gpio.h>
#define DEV_MMCARD0         devSpiMmcGpio
#endif

#if defined(DEV_MMCARD0)
#ifndef DEV_MMCARD
#define DEV_MMCARD          DEV_MMCARD0
#endif
#ifndef DEV_MMCARD0_NAME
#define DEV_MMCARD0_NAME    "MMC0"
#endif
#ifndef DEV_MMCARD_NAME
#define DEV_MMCARD_NAME     DEV_MMCARD0_NAME
#endif
#endif

__BEGIN_DECLS
/* Prototypes */

extern void NutBoardInit(void);
extern void NutIdleInit(void);
extern void NutMainInit(void);

__END_DECLS
/* End of prototypes */

#endif

