#ifndef _ARCH_ARM_AT91SAM9XE_H_
#define _ARCH_ARM_AT91SAM9XE_H_
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
 * \file arch/arm/at91sam9x.h
 * \brief AT91SAM9X peripherals.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.3  2009/02/17 09:33:55  haraldkipp
 * Check for PLL B availability instead of target names.
 *
 * Revision 1.2  2008/10/23 08:50:43  haraldkipp
 * Prepared AT91 UART hardware handshake.
 *
 * Revision 1.1  2008/08/06 12:51:10  haraldkipp
 * Added support for Ethernut 5 (AT91SAM9XE reference design).
 *
 *
 * \endverbatim
 */

#include <arch/arm/v5te.h>

#define FLASH_BASE      0x100000UL
#define RAM_BASE        0x200000UL

#define TC_BASE         0xFFFA0000      /*!< \brief Timer/counter base address. */
#define UDP_BASE        0xFFFA4000      /*!< \brief USB device port base address. */
#define MCI_BASE        0xFFFA8000      /*!< \brief MMC/SDCard interface base address. */
#define TWI_BASE        0xFFFAC000      /*!< \brief Two-wire interface base address. */
#define USART0_BASE     0xFFFB0000      /*!< \brief USART 0 base address. */
#define USART1_BASE     0xFFFB4000      /*!< \brief USART 1 base address. */
#define USART2_BASE     0xFFFB8000      /*!< \brief USART 2 base address. */
#define SSC_BASE        0xFFFBC000      /*!< \brief Serial synchronous controller base address. */
#define ISI_BASE        0xFFFC0000      /*!< \brief Image sensor interface base address. */
#define EMAC_BASE       0xFFFC4000      /*!< \brief EMAC base address. */
#define SPI0_BASE       0xFFFC8000      /*!< \brief SPI0 0 base address. */
#define SPI1_BASE       0xFFFCC000      /*!< \brief SPI0 1 base address. */
#define USART3_BASE     0xFFFD0000      /*!< \brief USART 3 base address. */
#define USART4_BASE     0xFFFD4000      /*!< \brief USART 4 base address. */
#define TWI1_BASE       0xFFFD8000      /*!< \brief TWI 1 base address. */
#define TC345_BASE      0xFFFDC000      /*!< \brief Timer/counter 3, 4 and 5 base address. */
#define ADC_BASE        0xFFFE0000      /*!< \brief ADC base address. */
#define ECC_BASE        0xFFFFE800      /*!< \brief ECC base address. */
#define SDRAMC_BASE     0xFFFFEA00      /*!< \brief SDRAMC base address. */
#define SMC_BASE        0xFFFFEC00      /*!< \brief SMC base address. */
#define MATRIX_BASE     0xFFFFEE00      /*!< \brief MATRIX base address. */
#define CCFG_BASE       0xFFFFEF10      /*!< \brief CCFG base address. */
#define EBI_BASE        0xFFFFEF1C      /*!< \brief External bus interface base address. */
#define AIC_BASE        0xFFFFF000      /*!< \brief AIC base address. */
#define DBGU_BASE       0xFFFFF200      /*!< \brief DBGU base address. */
#define PIOA_BASE       0xFFFFF400      /*!< \brief PIO A base address. */
#define PIOB_BASE       0xFFFFF600      /*!< \brief PIO B base address. */
#define PIOC_BASE       0xFFFFF800      /*!< \brief PIO C base address. */
#define EEFC_BASE       0xFFFFFA00      /*!< \brief EEFC base address. */
#define PMC_BASE        0xFFFFFC00      /*!< \brief PMC base address. */
#define RSTC_BASE       0xFFFFFD00      /*!< \brief Resect controller register base address. */
#define SHDWC_BASE      0xFFFFFD10      /*!< \brief Shutdown controller base address. */
#define RTT_BASE        0xFFFFFD20      /*!< \brief Realtime timer base address. */
#define PIT_BASE        0xFFFFFD30      /*!< \brief Periodic interval timer base address. */
#define WDT_BASE        0xFFFFFD40      /*!< \brief Watch Dog register base address. */
#define GPBR_BASE       0xFFFFFD60      /*!< \brief GPB register base address. */

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

#define DBGU_HAS_PDC
#define SPI_HAS_PDC
#define SSC_HAS_PDC
#define USART_HAS_PDC
#define USART_HAS_MODE
#define MCI_HAS_PDC
#define PMC_HAS_PLLB
#define PMC_HAS_MDIV

#define PIO_HAS_MULTIDRIVER
#define PIO_HAS_PULLUP
#define PIO_HAS_PERIPHERALSELECT
#define PIO_HAS_OUTPUTWRITEENABLE

#include <arch/arm/atmel/at91_tc.h>
#include <arch/arm/atmel/at91_us.h>
#include <arch/arm/atmel/at91_dbgu.h>
#include <arch/arm/atmel/at91_emac.h>
#include <arch/arm/atmel/at91_spi.h>
#include <arch/arm/atmel/at91_aic.h>
#include <arch/arm/atmel/at91_pio.h>
#include <arch/arm/atmel/at91_pmc.h>
#include <arch/arm/atmel/at91_rstc.h>
#include <arch/arm/atmel/at91_shdwc.h>
#include <arch/arm/atmel/at91_wdt.h>
#include <arch/arm/atmel/at91_ssc.h>
#include <arch/arm/atmel/at91_twi.h>
#include <arch/arm/atmel/at91_ebi.h>
#include <arch/arm/atmel/at91_smc.h>
#include <arch/arm/atmel/at91_mci.h>
#include <arch/arm/atmel/at91_smc.h>
#include <arch/arm/atmel/at91_matrix.h>
#include <arch/arm/atmel/at91_ccfg.h>
#include <arch/arm/atmel/at91_sdramc.h>
#include <arch/arm/atmel/at91_pit.h>
#include <arch/arm/atmel/at91_adc.h>
#include <arch/arm/atmel/at91_eefc.h>

/*! \addtogroup xgNutArchArmAt91Sam9x */
/*@{*/

/*! \name Peripheral Identifiers and Interrupts */
/*@{*/
#define FIQ_ID      0           /*!< \brief Fast interrupt. */
#define SYSC_ID     2           /*!< \brief System controller. */
#define PIOA_ID     2           /*!< \brief Parallel I/O controller A. */
#define PIOB_ID     3           /*!< \brief Parallel I/O controller B. */
#define PIOC_ID     4           /*!< \brief Parallel I/O controller C. */
#define ADC_ID      5           /*!< \brief Analog to digital converter. */
#define US0_ID      6           /*!< \brief USART 0. */
#define US1_ID      7           /*!< \brief USART 1. */
#define US2_ID      8           /*!< \brief USART 2. */
#define MCI_ID      9           /*!< \brief MMC interface. */
#define UDP_ID      10          /*!< \brief USB device port. */
#define TWI_ID      11          /*!< \brief Two wire interface 0. */
#define SPI0_ID     12          /*!< \brief Serial peripheral 0. */
#define SPI1_ID     13          /*!< \brief Serial peripheral 1. */
#define SSC_ID      14          /*!< \brief Serial peripheral interface. */
#define TC0_ID      17          /*!< \brief Timer/counter 0. */
#define TC1_ID      18          /*!< \brief Timer/counter 1. */
#define TC2_ID      19          /*!< \brief Timer/counter 2. */
#define UHP_ID      20          /*!< \brief USB host port. */
#define EMAC_ID     21          /*!< \brief Ethernet MAC. */
#define ISI_ID      22          /*!< \brief Image sensor interface. */
#define US3_ID      23          /*!< \brief USART 3. */
#define US4_ID      24          /*!< \brief USART 4. */
#define TWI1_ID     25          /*!< \brief Two wire interface 1. */
#define TC3_ID      26          /*!< \brief Timer/counter 3. */
#define TC4_ID      27          /*!< \brief Timer/counter 4. */
#define TC5_ID      28          /*!< \brief Timer/counter 5. */
#define IRQ0_ID     29          /*!< \brief External interrupt 0. */
#define IRQ1_ID     30          /*!< \brief External interrupt 1. */
#define IRQ2_ID     31          /*!< \brief External interrupt 2. */

/*@}*/

/*! \name USART Peripheral Multiplexing */
/*@{*/
#define PA31_SCK0_A         31  /*!< \brief Channel 0 serial clock pin. */
#define PB4_TXD0_A          4   /*!< \brief Channel 0 transmit data pin. */
#define PB5_RXD0_A          5   /*!< \brief Channel 0 receive data pin. */
#define PB27_CTS0_A         27  /*!< \brief Channel 0 clear to send pin. */
#define PB26_RTS0_A         26  /*!< \brief Channel 0 request to send pin. */
#define PB25_RI0_A          25  /*!< \brief Channel 0 ring indicator pin. */
#define PB22_DSR0_A         22  /*!< \brief Channel 0 data set ready pin. */
#define PB23_DCD0_A         23  /*!< \brief Channel 0 data carrier detect pin. */
#define PB24_DTR0_A         24  /*!< \brief Channel 0 data terminal ready pin. */

#define PA29_SCK1_A         29  /*!< \brief Channel 1 serial clock pin. */
#define PB6_TXD1_A          6   /*!< \brief Channel 1 transmit data pin. */
#define PB7_RXD1_A          7   /*!< \brief Channel 1 receive data pin. */
#define PB29_CTS1_A         29  /*!< \brief Channel 1 clear to send pin. */
#define PB28_RTS1_A         28  /*!< \brief Channel 1 request to send pin. */

#define PA30_SCK2_A         30  /*!< \brief Channel 2 serial clock pin. */
#define PB8_TXD2_A          8   /*!< \brief Channel 2 transmit data pin. */
#define PB9_RXD2_A          9   /*!< \brief Channel 2 receive data pin. */
#define PA5_CTS2_A          5   /*!< \brief Channel 2 clear to send pin. */
#define PA4_RTS2_A          4   /*!< \brief Channel 2 request to send pin. */

#define PC0_SCK3_B          0   /*!< \brief Channel 3 serial clock pin. */
#define PB10_TXD3_A         10  /*!< \brief Channel 3 transmit data pin. */
#define PB11_RXD3_A         11  /*!< \brief Channel 3 receive data pin. */
#define PC10_CTS3_B         10  /*!< \brief Channel 3 clear to send pin. */
#define PC8_RTS3_B          8   /*!< \brief Channel 3 request to send pin. */

#define PA31_TXD4_B         31  /*!< \brief Channel 4 transmit data pin. */
#define PA30_RXD4_B         30  /*!< \brief Channel 4 receive data pin. */
/*@}*/

/*! \name SPI Peripheral Multiplexing */
/*@{*/
#define PA0_SPI0_MISO_A     0   /*!< \brief Channel 0 master input slave output pin. */
#define PA1_SPI0_MOSI_A     1   /*!< \brief Channel 0 master output slave input pin. */
#define PA2_SPI0_SPCK_A     2   /*!< \brief Channel 0 serial clock pin. */
#define PA3_SPI0_NPCS0_A    3   /*!< \brief Channel 0 chip select 0 pin. */
#define PC11_SPI0_NPCS1_B   11  /*!< \brief Channel 0 chip select 1 pin. */
#define PC16_SPI0_NPCS2_B   16  /*!< \brief Channel 0 chip select 2 pin. */
#define PC17_SPI0_NPCS3_B   17  /*!< \brief Channel 0 chip select 3 pin. */

#define SPI0_PINS           _BV(PA0_SPI0_MISO_A) | _BV(PA1_SPI0_MOSI_A) | _BV(PA2_SPI0_SPCK_A)
#define SPI0_PIO_BASE       PIOA_BASE
#define SPI0_PSR_OFF        PIO_ASR_OFF

#define SPI0_CS0_PIN        _BV(PA3_SPI0_NPCS0_A)
#define SPI0_CS0_PIO_BASE   PIOA_BASE
#define SPI0_CS0_PSR_OFF    PIO_ASR_OFF

#define SPI0_CS1_PIN        _BV(PC11_SPI0_NPCS1_B)
#define SPI0_CS1_PIO_BASE   PIOC_BASE
#define SPI0_CS1_PSR_OFF    PIO_BSR_OFF

#define PB0_SPI1_MISO_A     0   /*!< \brief Channel 1 master input slave output pin. */
#define PB1_SPI1_MOSI_A     1   /*!< \brief Channel 1 master output slave input pin. */
#define PB2_SPI1_SPCK_A     2   /*!< \brief Channel 1 serial clock pin. */
#define PB3_SPI1_NPCS0_A    3   /*!< \brief Channel 1 chip select 0 pin. */
#define PC5_SPI1_NPCS1_B    5   /*!< \brief Channel 1 chip select 1 pin. */
#define PC18_SPI1_NPCS1_B   18  /*!< \brief Channel 1 chip select 1 pin. */
#define PC4_SPI1_NPCS2_B    4   /*!< \brief Channel 1 chip select 2 pin. */
#define PC19_SPI1_NPCS2_B   19  /*!< \brief Channel 1 chip select 2 pin. */
#define PC3_SPI1_NPCS3_B    3   /*!< \brief Channel 1 chip select 3 pin. */
#define PC20_SPI1_NPCS3_B   20  /*!< \brief Channel 1 chip select 3 pin. */

#define SPI1_PINS           _BV(PB0_SPI1_MISO_A) | _BV(PB1_SPI1_MOSI_A) | _BV(PB2_SPI1_SPCK_A)
#define SPI1_PIO_BASE       PIOB_BASE
#define SPI1_PSR_OFF        PIO_ASR_OFF

#define SPI1_CS0_PIN        _BV(PB3_SPI1_NPCS0_A)
#define SPI1_CS0_PIO_BASE   PIOB_BASE
#define SPI1_CS0_PSR_OFF    PIO_ASR_OFF

#ifndef SPI1_CS3_PIN
#define SPI1_CS3_PIN        _BV(PC3_SPI1_NPCS3_B)
#define SPI1_CS3_PIO_BASE   PIOC_BASE
#define SPI1_CS3_PSR_OFF    PIO_BSR_OFF
#endif

/*@}*/

/*! \name Image Sensor Interface Peripheral Multiplexing */
/*@{*/
#define PB20_ISI_D0_B       20  /*!< \brief Image sensor data bit 0 pin. */
#define PB21_ISI_D1_B       21  /*!< \brief Image sensor data bit 1 pin. */
#define PB22_ISI_D2_B       22  /*!< \brief Image sensor data bit 2 pin. */
#define PB23_ISI_D3_B       23  /*!< \brief Image sensor data bit 3 pin. */
#define PB24_ISI_D4_B       24  /*!< \brief Image sensor data bit 4 pin. */
#define PB25_ISI_D5_B       25  /*!< \brief Image sensor data bit 5 pin. */
#define PB26_ISI_D6_B       26  /*!< \brief Image sensor data bit 6 pin. */
#define PB27_ISI_D7_B       27  /*!< \brief Image sensor data bit 7 pin. */
#define PB10_ISI_D8_B       10  /*!< \brief Image sensor data bit 8 pin. */
#define PB11_ISI_D9_B       11  /*!< \brief Image sensor data bit 9 pin. */
#define PB12_ISI_D10_B      12  /*!< \brief Image sensor data bit 10 pin. */
#define PB13_ISI_D11_B      13  /*!< \brief Image sensor data bit 11 pin. */
#define PB28_ISI_PCK_B      28  /*!< \brief Image sensor data clock pin. */
#define PB29_ISI_VSYNC_B    29  /*!< \brief Image sensor vertical sync pin. */
#define PB30_ISI_HSYNC_B    30  /*!< \brief Image sensor horizontal sync pin. */
#define PB31_ISI_MCK_B      31  /*!< \brief Image sensor reference clock pin. */
/*@}*/

/*! \name MultiMedia Card and SDCard Interface Peripheral Multiplexing */
/*@{*/
#define PA8_MCCK_A          8   /*!< \brief MultiMedia card clock pin. */
#define PA7_MCCDA_A         7   /*!< \brief MultiMedia card slot A command pin. */
#define PA6_MCDA0_A         6   /*!< \brief MultiMedia card slot A data bit 0 pin. */
#define PA9_MCDA1_A         9   /*!< \brief MultiMedia card slot A data bit 1 pin. */
#define PA10_MCDA2_A        10  /*!< \brief MultiMedia card slot A data bit 2 pin. */
#define PA11_MCDA3_A        11  /*!< \brief MultiMedia card slot A data bit 3 pin. */
#define PA1_MCCDB_B         1   /*!< \brief MultiMedia card slot B command pin. */
#define PA0_MCDB0_B         0   /*!< \brief MultiMedia card slot B data bit 0 pin. */
#define PA5_MCDB1_B         5   /*!< \brief MultiMedia card slot B data bit 1 pin. */
#define PA4_MCDB2_B         4   /*!< \brief MultiMedia card slot B data bit 2 pin. */
#define PA3_MCDB3_B         3   /*!< \brief MultiMedia card slot B data bit 3 pin. */
/*@}*/

/*! \name EMAC Interface Peripheral Multiplexing */
/*@{*/
#define PA10_ETX2_B         10  /*!< \brief Transmit data bit 2 pin. */
#define PA11_ETX3_B         11  /*!< \brief Transmit data bit 3 pin. */
#define PA12_ETX0_A         12  /*!< \brief Transmit data bit 0 pin. */
#define PA13_ETX1_A         13  /*!< \brief Transmit data bit 1 pin. */
#define PA14_ERX0_A         14  /*!< \brief Receive data bit 0 pin. */
#define PA15_ERX1_A         15  /*!< \brief Receive data bit 1 pin. */
#define PA16_ETXEN_A        16  /*!< \brief Transmit enable pin. */
#define PA17_ERXDV_A        17  /*!< \brief Data valid pin. */
#define PA18_ERXER_A        18  /*!< \brief Receive error pin. */
#define PA19_ETXCK_A        19  /*!< \brief Transmit clock pin. */
#define PA20_EMDC_A         20  /*!< \brief Management data clock pin. */
#define PA21_EMDIO_A        21  /*!< \brief Management data I/O pin. */
#define PA22_ETXER_B        22  /*!< \brief Transmit error pin. */
#define PA23_ETX2_B         23  /*!< \brief Transmit data bit 2 pin. */
#define PA24_ETX3_B         24  /*!< \brief Transmit data bit 3 pin. */
#define PA25_ERX2_B         25  /*!< \brief Receive data bit 2 pin. */
#define PA26_ERX3_B         26  /*!< \brief Receive data bit 3 pin. */
#define PA27_ERXCK_B        27  /*!< \brief Receive clock pin. */
#define PA28_ECRS_B         28  /*!< \brief Carrier sense pin. */
#define PA29_ECOL_B         29  /*!< \brief Collision detect pin. */
#define PC21_EF100_B        21  /*!< \brief Force 100Mbit pin. */
/*@}*/

/*! \name ADC Interface Peripheral Multiplexing */
/*@{*/
#define PA22_ADTRG_A        22  /*!< \brief ADC trigger pin. */
/*@}*/

/*! \name Debug Unit Peripheral Multiplexing */
/*@{*/
#define PB14_DRXD_A         14  /*!< \brief Debug unit receive data pin. */
#define PB15_DTXD_A         15  /*!< \brief Debug unit transmit data pin. */
/*@}*/

/*! \name Synchronous Serial Controller Peripheral Multiplexing */
/*@{*/
#define PB18_TD0_A          18  /*!< \brief Transmit data pin. */
#define PB19_RD0_A          19  /*!< \brief Receive data pin. */
#define PB16_TK0_A          16  /*!< \brief Transmit clock pin. */
#define PB20_RK0_A          20  /*!< \brief Receive clock pin. */
#define PB17_TF0_A          17  /*!< \brief Transmit frame sync. pin. */
#define PB21_RF0_A          21  /*!< \brief Receive frame sync. pin. */
/*@}*/

/*! \name Two Wire Interface Peripheral Multiplexing */
/*@{*/
#define PA23_TWD_A          23  /*!< \brief Two wire serial 0 data pin. */
#define PA24_TWCK_A         24  /*!< \brief Two wire serial 0 clock pin. */

#define PB12_TWD1_A         12  /*!< \brief Two wire serial 1 data pin. */
#define PB13_RWCK1_A        13  /*!< \brief Two wire serial 1 clock pin. */
/*@}*/

/*! \name Timer/Counter Peripheral Multiplexing */
/*@{*/
#define PA25_TCLK0_A        25  /*!< \brief Timer/counter 0 external clock input. */
#define PA26_TIOA0_A        26  /*!< \brief Timer/counter 0 I/O line A. */
#define PC9_TIOB0_B         9   /*!< \brief Timer/counter 0 I/O line B. */

#define PB6_TCLK1_B         6   /*!< \brief Timer/counter 1 external clock input. */
#define PA27_TIOA1_A        27  /*!< \brief Timer/counter 1 I/O line A. */
#define PC7_TIOB1_A         7   /*!< \brief Timer/counter 1 I/O line B. */

#define PB7_TCLK2_B         7   /*!< \brief Timer/counter 2 external clock input. */
#define PA28_TIOA2_A        28  /*!< \brief Timer/counter 2 I/O line A. */
#define PC6_TIOB2_A         6   /*!< \brief Timer/counter 2 I/O line B. */

#define PB16_TCLK3_B        16  /*!< \brief Timer/counter 3 external clock input. */
#define PB0_TIOA3_B         0   /*!< \brief Timer/counter 3 I/O line A. */
#define PB1_TIOB3_B         1   /*!< \brief Timer/counter 3 I/O line B. */

#define PB17_TCLK4_B        17  /*!< \brief Timer/counter 4 external clock input. */
#define PB2_TIOA4_B         2   /*!< \brief Timer/counter 4 I/O line A. */
#define PB18_TIOB4_B        18  /*!< \brief Timer/counter 4 I/O line B. */

#define PC22_TCLK5_B        22  /*!< \brief Timer/counter 5 external clock input. */
#define PB3_TIOA5_B         3   /*!< \brief Timer/counter 5 I/O line A. */
#define PB19_TIOB5_B        19  /*!< \brief Timer/counter 5 I/O line B. */
/*@}*/

/*! \name Clocks, Oscillators and PLLs Peripheral Multiplexing */
/*@{*/
#define PB30_PCK0_A         30  /*!< \brief Programmable clock 0 output pin. */
#define PC1_PCK0_B          1   /*!< \brief Programmable clock 0 output pin. */
#define PB31_PCK1_A         31  /*!< \brief Programmable clock 1 output pin. */
#define PC2_PCK1_B          2   /*!< \brief Programmable clock 1 output pin. */
/*@}*/

/*! \name CompactFlash Peripheral Multiplexing */
/*@{*/
#define PC10_A25_CFRNW_A    10  /*!< \brief Read not write pin. */
#define PC8_NCS4_CFCS0_A    8   /*!< \brief Chip select line 0 pin. */
#define PC9_NCS5_CFCS1_A    9   /*!< \brief Chip select line 1 pin. */
#define PC6_CFCE1_B         6   /*!< \brief Chip enable line 1 pin. */
#define PC7_CFCE2_B         7   /*!< \brief Chip enable line 2 pin. */
/*@}*/

/*! \name External Bus Interface Peripheral Multiplexing */
/*@{*/
#define PC16_D16_A          16  /*!< \brief Data bus bit 16 pin. */
#define PC17_D17_A          17  /*!< \brief Data bus bit 17 pin. */
#define PC18_D18_A          18  /*!< \brief Data bus bit 18 pin. */
#define PC19_D19_A          19  /*!< \brief Data bus bit 19 pin. */
#define PC20_D20_A          20  /*!< \brief Data bus bit 20 pin. */
#define PC21_D21_A          21  /*!< \brief Data bus bit 21 pin. */
#define PC22_D22_A          22  /*!< \brief Data bus bit 22 pin. */
#define PC23_D23_A          23  /*!< \brief Data bus bit 23 pin. */
#define PC24_D24_A          24  /*!< \brief Data bus bit 24 pin. */
#define PC25_D25_A          25  /*!< \brief Data bus bit 25 pin. */
#define PC26_D26_A          26  /*!< \brief Data bus bit 26 pin. */
#define PC27_D27_A          27  /*!< \brief Data bus bit 27 pin. */
#define PC28_D28_A          28  /*!< \brief Data bus bit 28 pin. */
#define PC29_D29_A          29  /*!< \brief Data bus bit 29 pin. */
#define PC30_D30_A          30  /*!< \brief Data bus bit 30 pin. */
#define PC31_D31_A          31  /*!< \brief Data bus bit 31 pin. */
#define PC4_A23_A           4   /*!< \brief Address bus bit 23 pin. */
#define PC5_A24_A           5   /*!< \brief Address bus bit 24 pin. */
#define PC11_NCS2_A         11  /*!< \brief Negated chip select 2 pin. */
#define PC14_NCS3_NANDCS_A  14  /*!< \brief Negated chip select 3 pin. */
#define PC13_NCS6_B         13  /*!< \brief Negated chip select 6 pin. */
#define PC12_NCS7_B         12  /*!< \brief Negated chip select 7 pin. */
#define PC15_NWAIT_A        15  /*!< \brief External wait signal pin. */
/*@}*/

/*! \name Advanced Interrupt Controller Peripheral Multiplexing */
/*@{*/
#define PC13_FIQ_A          13  /*!< \brief Fast interrupt input pin. */
#define PC12_IRQ0_A         12  /*!< \brief External interrupt 0 input pin. */
#define PC15_IRQ1_B         15  /*!< \brief External interrupt 1 input pin. */
#define PC14_IRQ2_B         14  /*!< \brief External interrupt 2 input pin. */
/*@}*/

/*@} xgNutArchArmAt91Sam9x */

#endif                          /* _ARCH_ARM_AT91SAM9XE_H_ */
