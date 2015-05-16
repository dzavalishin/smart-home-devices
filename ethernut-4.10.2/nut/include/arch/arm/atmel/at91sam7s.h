#ifndef _ARCH_ARM_SAM7S_H_
#define _ARCH_ARM_SAM7S_H_
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
 * Change to sam7s.h
 *     By Justin Bennett, 2007/09/12
 *
 * \file arch/arm/at91sam7s.h
 * \brief AT91 peripherals.
 *
 * \verbatim
 *
 * Revision 1.2  2010/12/15 21:46:28  ve2yag
 * Add PWM interrupt support for AT91SAM7S and SE.
 *
 * Revision 1.1  2007/09/12    jbennett
 * Branched from at91sam7x.h version 1.7
 *
 *
 * \endverbatim
 */

#define FLASH_BASE      0x100000UL
#define RAM_BASE        0x200000UL

#define TC_BASE         0xFFFA0000      /*!< \brief Timer/counter base address. */
#define UDP_BASE        0xFFFB0000      /*!< \brief USB device port base address. */
#define TWI_BASE        0xFFFB8000      /*!< \brief Two-wire interface base address. */
#define USART0_BASE     0xFFFC0000      /*!< \brief USART 0 base address. */
#define USART1_BASE     0xFFFC4000      /*!< \brief USART 1 base address. */
#define PWMC_BASE       0xFFFCC000      /*!< \brief PWM controller base address. */
#define SSC_BASE        0xFFFD4000      /*!< \brief Serial synchronous controller base address. */
#define ADC_BASE        0xFFFD8000      /*!< \brief ADC base address. */
#define SPI0_BASE       0xFFFE0000      /*!< \brief SPI0 base address. */

#define AIC_BASE        0xFFFFF000      /*!< \brief AIC base address. */
#define DBGU_BASE       0xFFFFF200      /*!< \brief DBGU base address. */
#define PIOA_BASE       0xFFFFF400      /*!< \brief PIO A base address. */
#define PMC_BASE        0xFFFFFC00      /*!< \brief PMC base address. */
#define RSTC_BASE       0xFFFFFD00      /*!< \brief Resect controller register base address. */
#define RTT_BASE        0xFFFFFD20      /*!< \brief Realtime timer base address. */
#define PIT_BASE        0xFFFFFD30      /*!< \brief Periodic interval timer base address. */
#define WDT_BASE        0xFFFFFD40      /*!< \brief Watch Dog register base address. */
#define VREG_BASE       0xFFFFFD60      /*!< \brief Voltage regulator mode controller base address. */
#define MC_BASE         0xFFFFFF00      /*!< \brief Memory controller base. */

#define PERIPH_RPR_OFF  0x00000100      /*!< \brief Receive pointer register offset. */
#define PERIPH_RCR_OFF  0x00000104      /*!< \brief Receive counter register offset. */
#define PERIPH_TPR_OFF  0x00000108      /*!< \brief Transmit pointer register offset. */
#define PERIPH_TCR_OFF  0x0000010C      /*!< \brief Transmit counter register offset. */
#define PERIPH_RNPR_OFF 0x00000110      /*!< \brief Receive next pointer register offset. */
#define PERIPH_RNCR_OFF 0x00000114      /*!< \brief Receive next counter register offset. */
#define PERIPH_TNPR_OFF 0x00000118      /*!< \brief Transmit next pointer register offset. */
#define PERIPH_TNCR_OFF 0x0000011C      /*!< \brief Transmit next counter register offset. */
#define PERIPH_PTCR_OFF 0x00000120      /*!< \brief PDC transfer control register offset. */
#define PERIPH_PTSR_OFF 0x00000124      /*!< \brief PDC transfer status register offset. */

#define PDC_RXTEN       0x00000001      /*!< \brief Receiver transfer enable. */
#define PDC_RXTDIS      0x00000002      /*!< \brief Receiver transfer disable. */
#define PDC_TXTEN       0x00000100      /*!< \brief Transmitter transfer enable. */
#define PDC_TXTDIS      0x00000200      /*!< \brief Transmitter transfer disable. */

#define DBGU_HAS_PDC
#define SPI_HAS_PDC
#define SSC_HAS_PDC
#define USART_HAS_PDC
#define USART_HAS_MODE

#define PIO_HAS_MULTIDRIVER
#define PIO_HAS_PULLUP
#define PIO_HAS_PERIPHERALSELECT
#define PIO_HAS_OUTPUTWRITEENABLE

#include <arch/arm/atmel/at91_tc.h>
#include <arch/arm/atmel/at91_pwmc.h>
#include <arch/arm/atmel/at91_us.h>
#include <arch/arm/atmel/at91_dbgu.h>
#include <arch/arm/atmel/at91_spi.h>
#include <arch/arm/atmel/at91_aic.h>
#include <arch/arm/atmel/at91_pio.h>
#include <arch/arm/atmel/at91_pmc.h>
#include <arch/arm/atmel/at91_rstc.h>
#include <arch/arm/atmel/at91_wdt.h>
#include <arch/arm/atmel/at91_pit.h>
#include <arch/arm/atmel/at91_mc.h>
#include <arch/arm/atmel/at91_ssc.h>
#include <arch/arm/atmel/at91_twi.h>
#include <arch/arm/atmel/at91_adc.h>

/*! \addtogroup xgNutArchArmAt91Sam7x */
/*@{*/

/*! \name Peripheral Identifiers and Interrupts */
/*@{*/
#define FIQ_ID      0       /*!< \brief Fast interrupt ID. */
#define SYSC_ID     1       /*!< \brief System interrupt ID. */
#define PIOA_ID     2       /*!< \brief Parallel I/O controller ID. */
/* Reserved         3*/
#define ADC_ID      4       /*!< \brief Analog to digital converter ID. */
#define SPI0_ID     5       /*!< \brief Serial peripheral interface 0 ID. */
#define US0_ID      6       /*!< \brief USART 0 ID. */
#define US1_ID      7       /*!< \brief USART 1 ID. */
#define SSC_ID      8       /*!< \brief Synchronous serial controller ID. */
#define TWI_ID      9       /*!< \brief Two-wire interface ID. */
#define PWMC_ID     10      /*!< \brief PWM controller ID. */
#define UDP_ID      11      /*!< \brief USB device port ID. */
#define TC0_ID      12      /*!< \brief Timer 0 ID. */
#define TC1_ID      13      /*!< \brief Timer 1 ID. */
#define TC2_ID      14      /*!< \brief Timer 2 ID. */
/* Reserved      15-29*/
#define IRQ0_ID     30      /*!< \brief External interrupt 0 ID. */
#define IRQ1_ID     31      /*!< \brief External interrupt 1 ID. */
/*@}*/

/*! \name Historical SPI0 Peripheral Multiplexing Names */
/*@{*/
#define SPI0_NPCS0_PA11A	11	/*!< \brief Port bit number on PIO-A Perpheral A. */
#define SPI0_NPCS1_PA09B	9	/*!< \brief Port bit number on PIO-A Perpheral B. */
#define SPI0_NPCS1_PA31A	31	/*!< \brief Port bit number on PIO-A Perpheral A. */
#define SPI0_NPCS2_PA10B	10	/*!< \brief Port bit number on PIO-A Perpheral B. */
#define SPI0_NPCS2_PA30B	30	/*!< \brief Port bit number on PIO-A Perpheral B. */
#define SPI0_NPCS3_PA03B	3	/*!< \brief Port bit number on PIO-A Perpheral B. */
#define SPI0_NPCS3_PA05B	5	/*!< \brief Port bit number on PIO-A Perpheral B. */
#define SPI0_NPCS3_PA22B	22	/*!< \brief Port bit number on PIO-A Perpheral B. */
#define SPI0_MISO_PA12A    12 /*!< \brief Port bit number on PIO-A Perpheral A. */
#define SPI0_MOSI_PA13A    13 /*!< \brief Port bit number on PIO-A Perpheral A. */
#define SPI0_SPCK_PA14A    14 /*!< \brief Port bit number on PIO-A Perpheral A. */
/*@}*/

/*! \name USART Peripheral Multiplexing */
/*@{*/
#define PA5_RXD0_A          5
#define PA6_TXD0_A          6
#define PA2_SCK0_B          2
#define PA7_RTS0_A          7
#define PA8_CTS0_A          8

#define PA21_RXD1_A         21
#define PA22_TXD1_A         22
#define PA23_SCK1_A         23
#define PA24_RTS1_A         24
#define PA25_CTS1_A         25
#define PB26_DCD1_A         26
#define PB28_DSR1_A         28
#define PB27_DTR1_A         27
#define PB29_RI1_A          29
/*@}*/

/*! \name SPI Peripheral Multiplexing */
/*@{*/
#define PA12_SPI0_MISO_A    12
#define PA13_SPI0_MOSI_A    13
#define PA14_SPI0_SPCK_A    14
#define PA11_SPI0_NPCS0_A   11
#define PA9_SPI0_NPCS1_B    9
#define PA31_SPI0_NPCS1_A   31
#define PA10_SPI0_NPCS2_B   10
#define PB30_SPI0_NPCS2_B   30
#define PA3_SPI0_NPCS3_B    3
#define PA5_SPI0_NPCS3_B    5
#define PA22_SPI0_NPCS3_B   22

#define SPI0_PINS           _BV(PA12_SPI0_MISO_A) | _BV(PA13_SPI0_MOSI_A) | _BV(PA14_SPI0_SPCK_A)
#define SPI0_PIO_BASE       PIOA_BASE
#define SPI0_PSR_OFF        PIO_ASR_OFF

#define SPI0_CS0_PIN        _BV(PA11_SPI0_NPCS0_A)
#define SPI0_CS0_PIO_BASE   PIOA_BASE
#define SPI0_CS0_PSR_OFF    PIO_ASR_OFF

#ifndef SPI0_CS1_PIN
#define SPI0_CS1_PIN        _BV(PA9_SPI0_NPCS1_B)
#define SPI0_CS1_PIO_BASE   PIOA_BASE
#define SPI0_CS1_PSR_OFF    PIO_ASR_OFF
#endif

#ifndef SPI0_CS2_PIN
#define SPI0_CS2_PIN        _BV(PA10_SPI0_NPCS2_B)
#define SPI0_CS2_PIO_BASE   PIOA_BASE
#define SPI0_CS2_PSR_OFF    PIO_ASR_OFF
#endif

#ifndef SPI0_CS3_PIN
#define SPI0_CS3_PIN        _BV(PA3_SPI0_NPCS3_B)
#define SPI0_CS3_PIO_BASE   PIOA_BASE
#define SPI0_CS3_PSR_OFF    PIO_ASR_OFF
#endif

/*@}*/

/*! \name Debug Unit Peripheral Multiplexing */
/*@{*/
#define PA9_DRXD_A          9
#define PA10_DTXD_A         10
/*@}*/

/*! \name Synchronous Serial Controller Peripheral Multiplexing */
/*@{*/
#define PA17_TD_A           17  /*!< \brief Transmit data pin. */
#define PA18_RD_A           18  /*!< \brief Receive data pin. */
#define PA16_TK_A           16  /*!< \brief Transmit clock pin. */
#define PA19_RK_A           19  /*!< \brief Receive clock pin. */
#define PA15_TF_A           15  /*!< \brief Transmit frame sync. pin. */
#define PA20_RF_A           20  /*!< \brief Receive frame sync. pin. */
/*@}*/

/*! \name Two Wire Interface Peripheral Multiplexing */
/*@{*/
#define PA3_TWD_A          3  /*!< \brief Two wire serial data pin. */
#define PA4_TWCK_A         4  /*!< \brief Two wire serial clock pin. */
/*@}*/

/*! \name Timer/Counter Peripheral Multiplexing */
/*@{*/
#define PA0_TIOA0_B        0
#define PA1_TIOB0_B        1
#define PA4_TCLK0_B        4

#define PA15_TIOA1_B        15
#define PA16_TIOB1_B        16
#define PA28_TCLK1_B        28

#define PA26_TIOA2_B        26
#define PA27_TIOB2_B        27
#define PA29_TCLK2_B        29
/*@}*/

/*! \name Clocks, Oscillators and PLLs Peripheral Multiplexing */
/*@{*/
#define PA6_PCK0_B          6
#define PA17_PCK1_B         17
#define PA21_PCK1_B         21
#define PA18_PCK2_B         18
#define PA31_PCK2_B         31
/*@}*/

/*! \name Advanced Interrupt Controller Peripheral Multiplexing */
/*@{*/
#define PA19_FIQ_B          19
#define PA20_IRQ0_B         20
#define PA30_IRQ1_A         30
/*@}*/

/*! \name ADC Interface Peripheral Multiplexing */
/*@{*/
#define PA8_ADTRG_B        8  /*!< \brief ADC trigger pin. */
/*@}*/

/*! \name PWM Peripheral Multiplexing */
/*@{*/
#define PA0_PWM0_A          0
#define PA23_PWM0_B         23
#define PA1_PWM1_A          1
#define PA24_PWM1_B         24
#define PA2_PWM2_A          2
#define PA13_PWM2_B         13
#define PA25_PWM2_B         25
#define PA7_PWM3_B          7
#define PA14_PWM3_B         14
/*@}*/

/*@} xgNutArchArmAt91 */

#endif                          /* _ARCH_ARM_AT91SAM7S_H_ */
