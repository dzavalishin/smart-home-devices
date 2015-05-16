#ifndef _ARCH_ARM_SAM7SE_H_
#define _ARCH_ARM_SAM7SE_H_
/*
 * Copyright (C) 2006-2008 by egnite Software GmbH. All rights reserved.
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
 * \file arch/arm/at91sam7se.h
 * \brief AT91 peripherals.
 *
 * \verbatim
 * $Id: at91sam7se.h 3450 2011-05-31 19:40:25Z mifi $
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
#define PIOB_BASE       0xFFFFF600      /*!< \brief PIO B base address. */
#define PIOC_BASE       0xFFFFF800      /*!< \brief PIO C base address. */
#define PMC_BASE        0xFFFFFC00      /*!< \brief PMC base address. */
#define RSTC_BASE       0xFFFFFD00      /*!< \brief Resect controller register base address. */
#define RTT_BASE        0xFFFFFD20      /*!< \brief Realtime timer base address. */
#define PIT_BASE        0xFFFFFD30      /*!< \brief Periodic interval timer base address. */
#define WDT_BASE        0xFFFFFD40      /*!< \brief Watch Dog register base address. */
#define VREG_BASE       0xFFFFFD60      /*!< \brief Voltage regulator mode controller base address. */
#define MC_BASE         0xFFFFFF00      /*!< \brief Memory controller base address. */
#define EFC0_BASE       0xFFFFFF60      /*!< \brief EFC0 base address. */
#define EFC1_BASE       0xFFFFFF70      /*!< \brief EFC1 base address. */
#define EBI_BASE        0xFFFFFF80      /*!< \brief External bus interface base address. */
#define SMC_BASE        0xFFFFFF90      /*!< \brief SMC base address. */
#define SDRAMC_BASE     0xFFFFFFB0      /*!< \brief SDRAMC base address. */
#define ECC_BASE        0xFFFFFFDC      /*!< \brief ECC base address. */

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

#define EBI_HAS_CSA
#define SMC_HAS_CSR

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
#include <arch/arm/atmel/at91_ebi.h>
#include <arch/arm/atmel/at91_smc.h>
#include <arch/arm/atmel/at91_sdramc.h>
#include <arch/arm/atmel/at91_ssc.h>
#include <arch/arm/atmel/at91_twi.h>
#include <arch/arm/atmel/at91_udp.h>
#include <arch/arm/atmel/at91_adc.h>
#include <arch/arm/atmel/at91_pwmc.h>

/*! \addtogroup xgNutArchArmAt91Sam7se */
/*@{*/

/*! \name Peripheral Identifiers and Interrupts */
/*@{*/
#define FIQ_ID      0       /*!< \brief Fast interrupt ID. */
#define SYSC_ID     1       /*!< \brief System interrupt ID. */
#define PIOA_ID     2       /*!< \brief Parallel I/O controller A ID. */
#define PIOB_ID     3       /*!< \brief Parallel I/O controller B ID. */
#define PIOC_ID     4       /*!< \brief Parallel I/O controller C ID. */
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
#define ADC_ID      15      /*!< \brief Analog to digital converter ID. */
/* Reserved      16-28*/
#define IRQ0_ID     29      /*!< \brief External interrupt 0 ID. */
#define IRQ1_ID     30      /*!< \brief External interrupt 1 ID. */
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
#define PB2_SCK0_A          2
#define PA7_RTS0_A          7
#define PA8_CTS0_A          8

#define PA21_RXD1_A         21
#define PA22_TXD1_A         22
#define PA23_SCK1_A         23
#define PA24_RTS1_A         24
#define PC8_RTS1_B          8
#define PA25_CTS1_A         25
#define PA26_DCD1_A         26
#define PA27_DTR1_A         27
#define PC9_DTR1_B          9
#define PA28_DSR1_A         28
#define PA29_RI1_A          29
/*@}*/

/*! \name SPI Peripheral Multiplexing */
/*@{*/
#define PA12_SPI0_MISO_A    12
#define PA13_SPI0_MOSI_A    13
#define PA14_SPI0_SPCK_A    14
#define PA11_SPI0_NPCS0_A   11
#define PA31_SPI0_NPCS1_A   31
#define PB9_SPI0_NPCS1_A    9
#define PB10_SPI0_NPCS2_A   10
#define PC14_SPI0_NPCS1_B   14
#define PB30_SPI0_NPCS2_A   30
#define PB3_SPI0_NPCS3_A    3
#define PB5_SPI0_NPCS3_A    5
#define PB22_SPI0_NPCS3_A   22

#define SPI0_PINS           _BV(PA12_SPI0_MISO_A) | _BV(PA13_SPI0_MOSI_A) | _BV(PA14_SPI0_SPCK_A)
#define SPI0_PIO_BASE       PIOA_BASE
#define SPI0_PSR_OFF        PIO_ASR_OFF

#define SPI0_CS0_PIN        _BV(PA11_SPI0_NPCS0_A)
#define SPI0_CS0_PIO_BASE   PIOA_BASE
#define SPI0_CS0_PSR_OFF    PIO_ASR_OFF

#ifndef SPI0_CS1_PIN
#define SPI0_CS1_PIN        _BV(PA31_SPI0_NPCS1_A)
#define SPI0_CS1_PIO_BASE   PIOA_BASE
#define SPI0_CS1_PSR_OFF    PIO_ASR_OFF
#endif

/*@}*/

/*! \name External Bus Interface Multiplexing */
/*@{*/
#define PA0_A0_B            0
#define PB0_A0_B            0
#define PA1_A1_B            1
#define PB1_A1_B            1
#define PA2_A2_B            2
#define PB2_A2_B            2
#define PA3_A3_B            3
#define PB3_A3_B            3
#define PA4_A4_B            4
#define PB4_A4_B            4
#define PA5_A5_B            5
#define PB5_A5_B            5
#define PA6_A6_B            6
#define PB6_A6_B            6
#define PA7_A7_B            7
#define PB7_A7_B            7
#define PA8_A8_B            8
#define PB8_A8_B            8
#define PA9_A9_B            9
#define PB9_A9_B            9
#define PA10_A10_B          10
#define PB10_A10_B          10
#define PA11_A11_B          11
#define PB11_A11_B          11
#define PA12_A12_B          12
#define PB12_A12_B          12
#define PA13_A13_B          13
#define PB13_A13_B          13
#define PA14_A14_B          14
#define PB14_A14_B          14
#define PA15_A15_B          15
#define PB15_A15_B          15
#define PA16_A16_B          16
#define PB16_A16_B          16
#define PA17_A17_B          17
#define PB17_A17_B          17
#define PC16_A18_A          16
#define PC17_A19_A          17
#define PC18_A20_A          18
#define PC19_A21_A          19
#define PC20_A22_A          20

#define PC0_D0_A            0
#define PC1_D1_A            1
#define PC2_D2_A            2
#define PC3_D3_A            3
#define PC4_D4_A            4
#define PC5_D5_A            5
#define PC6_D6_A            6
#define PC7_D7_A            7
#define PC8_D8_A            8
#define PC9_D9_A            9
#define PC10_D10_A          10
#define PC11_D11_A          11
#define PC12_D12_A          12
#define PC13_D13_A          13
#define PC14_D14_A          14
#define PC15_D15_A          15
#define PB18_D16_B          18
#define PB19_D17_B          19
#define PB20_D18_B          20
#define PB21_D19_B          21
#define PB22_D20_B          22
#define PB23_D21_B          23
#define PB24_D22_B          24
#define PB25_D23_B          25
#define PB26_D24_B          26
#define PB27_D25_B          27
#define PB28_D26_B          28
#define PB29_D27_B          29
#define PB30_D28_B          30
#define PB31_D29_B          31
#define PA30_D30_B          30
#define PA31_D31_B          31

#define PA19_NCS4_B         19
#define PA20_NCS2_B         20
#define PA21_NCS6_B         21
#define PA22_NCS5_B         22
#define PA26_NCS1_B         26
#define PC15_NCS3_B         15
#define PC20_NCS7_B         20
#define PC23_NCS0_B         23

#define PA18_NBS3_B         18
#define PA23_NWR1_B         23
#define PA24_SDA10_B        24
#define PA25_SDCKE_B        25
#define PA27_SDWE_B         27
#define PA28_CAS_B          28
#define PA29_RAS_B          29
#define PC16_NWAIT_B        16
#define PC17_NANDOE_B       17
#define PC18_NANDWE_B       18
#define PC21_NWR0_B         21
#define PC22_NRD_B          22
#define PC23_CFRNW_A        23
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
#define PB0_TIOA0_A         0
#define PB1_TIOB0_A         1
#define PB4_TCLK0_A         4
#define PB15_TIOA1_A        15
#define PB16_TIOB1_A        16
#define PB28_TCLK1_A        28
#define PB26_TIOA2_A        26
#define PB27_TIOB2_A        27
#define PB29_TCLK2_A        29
/*@}*/

/*! \name Clocks, Oscillators and PLLs Peripheral Multiplexing */
/*@{*/
#define PB6_PCK0_A          6
#define PC10_PCK0_B         10
#define PB17_PCK1_A         17
#define PB21_PCK1_A         21
#define PC11_PCK1_B         11
#define PB18_PCK2_A         18
#define PB31_PCK2_A         31
#define PC12_PCK2_B         12
/*@}*/

/*! \name Advanced Interrupt Controller Peripheral Multiplexing */
/*@{*/
#define PB19_FIQ_A          19
#define PB20_IRQ0_A         20
#define PA30_IRQ1_A         30
/*@}*/

/*! \name ADC Interface Peripheral Multiplexing */
/*@{*/
#define PB8_ADTRG_A        8  /*!< \brief ADC trigger pin. */
/*@}*/

/*! \name PWM Peripheral Multiplexing */
/*@{*/
#define PA0_PWM0_A          0
#define PB11_PWM0_A         11
#define PB23_PWM0_A         23
#define PA1_PWM1_A          1
#define PB12_PWM1_A         12
#define PB24_PWM1_A         24
#define PA2_PWM2_A          2
#define PB13_PWM2_A         13
#define PB25_PWM2_A         25
#define PB7_PWM3_A          7
#define PB14_PWM3_A         14
/*@}*/

/*@} xgNutArchArmAt91se */

#endif /* _ARCH_ARM_AT91SAM7SE_H_ */
