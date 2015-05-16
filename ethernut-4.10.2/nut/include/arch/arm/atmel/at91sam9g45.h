#ifndef _ARCH_ARM_SAM9G45_H_
#define _ARCH_ARM_SAM9G45_H_
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
 * \file arch/arm/at91sam9g45.h
 * \brief AT91SAM9G45 peripherals.
 *
 */

#include <arch/arm/v5te.h>

#if 0
extern int sprintf (char *__restrict __s,
		    __const char *__restrict __format, ...);
void PrintBlockingDebug(char *Str);
#endif

#define FLASH_BASE           0x100000UL
#define RAM_BASE             0x200000UL
#define NAND_FLASH_BASE      0x40000000

// Internal peripherals
#define LCDC_BASE       0x00500000      /*!< \brief USB device port base address. */

#define UDP_BASE        0xFFF78000      /*!< \brief USB device port base address. */
#define TC_BASE         0xFFF7C000      /*!< \brief Timer/counter base address. (TC0-TC2) */
#define MCI_BASE        0xFFF80000      /*!< \brief MMC/SDCard interface base address. (MCI0) */
#define TWI_BASE        0xFFF84000      /*!< \brief Two-wire interface base address. (TWI0-TWI1) */
#define USART0_BASE     0xFFF8C000      /*!< \brief USART 0 base address. */
#define USART1_BASE     0xFFF90000      /*!< \brief USART 1 base address. */
#define USART2_BASE     0xFFF94000      /*!< \brief USART 2 base address. */
#define USART3_BASE     0xFFF98000      /*!< \brief USART 2 base address. */
#define SSC_BASE        0xFFF9C000      /*!< \brief Serial synchronous controller base address. (SSC0-SSC1) */
#define SPI0_BASE       0xFFFA4000      /*!< \brief SPI0 0 base address. */
#define SPI1_BASE       0xFFFA8000      /*!< \brief SPI0 1 base address. */
#define AC97_BASE       0xFFFAC000      /*!< \brief AC97 Codec interface. */
#define TSADCC_BASE     0xFFFB0000      /*!< \brief Touch screen ADC controller. */
#define ISI_BASE        0xFFFB4000      /*!< \brief Image sensor interface base address. */
#define PWMC_BASE       0xFFFB8000      /*!< \brief PWM controller base address. */
#define EMAC_BASE       0xFFFBC000      /*!< \brief EMAC base address. */
#define TRNG_BASE       0xFFFCC000      /*!< \brief True random number generator. */
#define MCI1_BASE       0xFFFD0000      /*!< \brief MMC/SDCard interface base address. (MCI1) */
#define TC345_BASE      0xFFFD4000      /*!< \brief Timer/counter base address. (TC3-TC5) */

// System controller
#define DDRSDRC1_BASE   0xFFFFE400      /*!< \brief DDRSDRC1 base address. */
#define DDRSDRC0_BASE   0xFFFFE600      /*!< \brief DDRSDRC0 base address. */
#define SMC_BASE        0xFFFFE800      /*!< \brief SMC base address. */
#define MATRIX_BASE     0xFFFFEA00      /*!< \brief MATRIX base address. */
#define DMAC_BASE       0xFFFFEC00      /*!< \brief DMA controller base address. */
#define DBGU_BASE       0xFFFFEE00      /*!< \brief DBGU base address. */
#define AIC_BASE        0xFFFFF000      /*!< \brief AIC base address. */
#define PIOA_BASE       0xFFFFF200      /*!< \brief PIO A base address. */
#define PIOB_BASE       0xFFFFF400      /*!< \brief PIO B base address. */
#define PIOC_BASE       0xFFFFF600      /*!< \brief PIO C base address. */
#define PIOD_BASE       0xFFFFF800      /*!< \brief PIO D base address. */
#define PIOE_BASE       0xFFFFFA00      /*!< \brief PIO E base address. */
#define PMC_BASE        0xFFFFFC00      /*!< \brief PMC base address. */
#define RSTC_BASE       0xFFFFFD00      /*!< \brief Resect controller register base address. */
#define SHDWC_BASE      0xFFFFFD10      /*!< \brief Shutdown controller. */
#define RTT_BASE        0xFFFFFD20      /*!< \brief Realtime timer base address. */
#define PIT_BASE        0xFFFFFD30      /*!< \brief Periodic interval timer base address. */
#define WDT_BASE        0xFFFFFD40      /*!< \brief Watch Dog register base address. */
#define SCKCR_BASE      0xFFFFFD50      /*!< \brief Slow clock control register. */
#define GPBR_BASE       0xFFFFFD60      /*!< \brief General purpose backup registers. */
#define RTC_BASE        0xFFFFFDB0      /*!< \brief RTC. */

// Undocumented Base adress iin datasheet! HECC? -> Atmel Header-File (HECC ==? ECC)
#define ECC_BASE        0xFFFFE200      /*!< \brief ECC base address. */

// Peripheral DMA Controller (PDC) User Interface
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

// Transfer Control Register - Bits
#define PDC_RXTEN       0x00000001      /*!< \brief Receiver transfer enable. */
#define PDC_RXTDIS      0x00000002      /*!< \brief Receiver transfer disable. */
#define PDC_TXTEN       0x00000100      /*!< \brief Transmitter transfer enable. */
#define PDC_TXTDIS      0x00000200      /*!< \brief Transmitter transfer disable. */

// Check...
#define DBGU_HAS_PDC
#define SPI_HAS_PDC
#define SSC_HAS_PDC
#define USART_HAS_PDC
#define USART_HAS_MODE
#define MCI_HAS_PDC
#define PMC_HAS_PLLB
#define PMC_HAS_MDIV
#define EBI_HAS_CSA

// Check....
#define PIO_HAS_MULTIDRIVER
#define PIO_HAS_PULLUP
#define PIO_HAS_PERIPHERALSELECT
#define PIO_HAS_OUTPUTWRITEENABLE

#include <arch/arm/atmel/at91_rtc.h>
#include <arch/arm/atmel/at91_ecc.h>
#include <arch/arm/atmel/at91_ebi.h>
#include <arch/arm/atmel/at91_tc.h>
#include <arch/arm/atmel/at91_us.h>
#include <arch/arm/atmel/at91_dbgu.h>
#include <arch/arm/atmel/at91_emac.h>
#include <arch/arm/atmel/at91_spi.h>
#include <arch/arm/atmel/at91_aic.h>
#include <arch/arm/atmel/at91_pio.h>
#include <arch/arm/atmel/at91_pmc.h>
#include <arch/arm/atmel/at91_rstc.h>
#include <arch/arm/atmel/at91_wdt.h>
#include <arch/arm/atmel/at91_ssc.h>
#include <arch/arm/atmel/at91_twi.h>
#include <arch/arm/atmel/at91_smc.h>
#include <arch/arm/atmel/at91_mci.h>
#include <arch/arm/atmel/at91_matrix.h>
#include <arch/arm/atmel/at91_ccfg.h>
#include <arch/arm/atmel/at91_sdramc.h>
#include <arch/arm/atmel/at91_adc.h>
#include <arch/arm/atmel/at91_pit.h>
#include <arch/arm/atmel/at91_lcdc.h>
#include <arch/arm/atmel/at91_tsadcc.h>

/*! \addtogroup xgNutArchArmAt91Sam9g45 */
/*@{*/

/*! \name Peripheral Identifiers and Interrupts */
/*@{*/
#define FIQ_ID      0           /*!< \brief Fast interrupt. */
#define SYSC_ID     1           /*!< \brief System interrupt. */
#define PIOA_ID     2           /*!< \brief Parallel I/O controller A. */
#define PIOB_ID     3           /*!< \brief Parallel I/O controller B. */
#define PIOC_ID     4           /*!< \brief Parallel I/O controller C. */
#define PIODE_ID    5           /*!< \brief Parallel I/O controller C. */
#define RNG_ID      6           /*!< \brief Analog to digital converter. */
#define US0_ID      7           /*!< \brief USART 0. */
#define US1_ID      8           /*!< \brief USART 1. */
#define US2_ID      9           /*!< \brief USART 2. */
#define US3_ID      10          /*!< \brief USART 3. */
#define MCI0_ID     11          /*!< \brief MMC interface. */
#define TWI0_ID     12          /*!< \brief Two wire interface. */
#define TWI1_ID     13          /*!< \brief Two wire interface. */
#define SPI0_ID     14          /*!< \brief Serial peripheral 0. */
#define SPI1_ID     15          /*!< \brief Serial peripheral 1. */
#define SSC0_ID     16          /*!< \brief Serial peripheral interface. */
#define SSC1_ID     17          /*!< \brief Serial peripheral interface. */
#define TC0_ID      18          /*!< \brief Timer/counter 0. */
#define TC1_ID      18          /*!< \brief Timer/counter 1. */
#define TC2_ID      18          /*!< \brief Timer/counter 2. */
#define TC3_ID      18          /*!< \brief Timer/counter 3. */
#define TC4_ID      18          /*!< \brief Timer/counter 4. */
#define TC5_ID      18          /*!< \brief Timer/counter 5. */
#define PWMC_ID     19          /*!< \brief Pulse Width Modulation Controller. */
#define TSADCC_ID   20          /*!< \brief Touch Screen ADC Controller. */
#define DMA_ID      21          /*!< \brief DMA Controller. */
#define UHP_ID      22          /*!< \brief USB host port. */
#define LCDC_ID     23          /*!< \brief LCD controller. */
#define AC97_ID     24          /*!< \brief AC97 controller. */
#define EMAC_ID     25          /*!< \brief Ethernet MAC. */
#define ISI_ID      26          /*!< \brief Image sensor interface. */
#define UDP_ID      27          /*!< \brief USB device port. */
#define MCI1_ID     29          /*!< \brief MMC interface. */
#define IRQ0_ID     31          /*!< \brief External interrupt 0. */

#define TWI_ID      TWI0_ID     /*!< \brief Two wire interface. */
#define MCI_ID      MCI0_ID     /*!< \brief MMC interface. */
#define SSC_ID      SSC0_ID     /*!< \brief Serial peripheral interface. */

/*@}*/

/*! \name USART Peripheral Multiplexing */
/*@{*/
#define PB16_SCK0_B         16  /*!< \brief Channel 0 serial clock pin. */
#define PB19_TXD0_A         19  /*!< \brief Channel 0 transmit data pin. */
#define PB18_RXD0_A         18  /*!< \brief Channel 0 receive data pin. */
#define PB15_CTS0_B         15  /*!< \brief Channel 0 clear to send pin. */
#define PB17_RTS0_B         17  /*!< \brief Channel 0 request to send pin. */

#define PD29_SCK1_B         29  /*!< \brief Channel 1 serial clock pin. */
#define PB4_TXD1_A          4   /*!< \brief Channel 1 transmit data pin. */
#define PB5_RXD1_A          5   /*!< \brief Channel 1 receive data pin. */
#define PD17_CTS1_A         17  /*!< \brief Channel 1 clear to send pin. */
#define PD16_RTS1_A         16  /*!< \brief Channel 1 request to send pin. */

#define PD30_SCK2_B         30  /*!< \brief Channel 2 serial clock pin. */
#define PB6_TXD2_A          6   /*!< \brief Channel 2 transmit data pin. */
#define PB7_RXD2_A          7   /*!< \brief Channel 2 receive data pin. */
#define PC11_CTS2_B         11  /*!< \brief Channel 2 clear to send pin. */
#define PC9_RTS2_B          9   /*!< \brief Channel 2 request to send pin. */

#define PA22_SCK3_B         22  /*!< \brief Channel 3 serial clock pin. */
#define PB8_TXD3_A          8   /*!< \brief Channel 3 transmit data pin. */
#define PB9_RXD3_A          9   /*!< \brief Channel 3 receive data pin. */
#define PA24_CTS3_B         24  /*!< \brief Channel 3 clear to send pin. */
#define PA23_RTS3_B         23  /*!< \brief Channel 3 request to send pin. */
/*@}*/

/*! \name SPI Peripheral Multiplexing */
/*@{*/
#define PB0_SPI0_MISO_A     0   /*!< \brief Channel 0 master input slave output pin. */
#define PB1_SPI0_MOSI_A     1   /*!< \brief Channel 0 master output slave input pin. */
#define PB2_SPI0_SPCK_A     2   /*!< \brief Channel 0 serial clock pin. */
#define PB3_SPI0_NPCS0_A    3   /*!< \brief Channel 0 chip select 0 pin. */
#define PB18_SPI0_NPCS1_B   18  /*!< \brief Channel 0 chip select 1 pin. */
#define PD24_SPI0_NPCS1_A   24  /*!< \brief Channel 0 chip select 1 pin. */
#define PB19_SPI0_NPCS2_B   19  /*!< \brief Channel 0 chip select 2 pin. */
#define PD25_SPI0_NPCS2_A   25  /*!< \brief Channel 0 chip select 1 pin. */
#define PD27_SPI0_NPCS3_B   27  /*!< \brief Channel 0 chip select 3 pin. */

#define SPI0_PINS           _BV(PB0_SPI0_MISO_A) | _BV(PB1_SPI0_MOSI_A) | _BV(PB2_SPI0_SPCK_A)
#define SPI0_PIO_BASE       PIOA_BASE
#define SPI0_PSR_OFF        PIO_ASR_OFF

#define SPI0_CS0_PIN        _BV(PB3_SPI0_NPCS0_A)
#define SPI0_CS0_PIO_BASE   PIOB_BASE
#define SPI0_CS0_PSR_OFF    PIO_ASR_OFF

#define SPI0_CS1_PIN        _BV(PB18_SPI0_NPCS1_B)
#define SPI0_CS1_PIO_BASE   PIOB_BASE
#define SPI0_CS1_PSR_OFF    PIO_BSR_OFF

#define PB14_SPI1_MISO_A     14  /*!< \brief Channel 1 master input slave output pin. */
#define PB15_SPI1_MOSI_A     15  /*!< \brief Channel 1 master output slave input pin. */
#define PB16_SPI1_SPCK_A     16  /*!< \brief Channel 1 serial clock pin. */
#define PB17_SPI1_NPCS0_A    17  /*!< \brief Channel 1 chip select 0 pin. */
#define PD28_SPI1_NPCS1_B    28  /*!< \brief Channel 1 chip select 1 pin. */
#define PD18_SPI1_NPCS2_A    18  /*!< \brief Channel 1 chip select 2 pin. */
#define PD19_SPI1_NPCS3_A    19  /*!< \brief Channel 1 chip select 3 pin. */

#define SPI1_PINS           _BV(PB14_SPI1_MISO_A) | _BV(PB15_SPI1_MOSI_A) | _BV(PB16_SPI1_SPCK_A)
#define SPI1_PIO_BASE       PIOB_BASE
#define SPI1_PSR_OFF        PIO_ASR_OFF

#define SPI1_CS0_PIN        _BV(PB17_SPI1_NPCS0_A)
#define SPI1_CS0_PIO_BASE   PIOB_BASE
#define SPI1_CS0_PSR_OFF    PIO_ASR_OFF

#ifndef SPI1_CS3_PIN
#define SPI1_CS3_PIN        _BV(PD19_SPI1_NPCS3_A)
#define SPI1_CS3_PIO_BASE   PIOD_BASE
#define SPI1_CS3_PSR_OFF    PIO_ASR_OFF
#endif

/*@}*/

/*! \name Image Sensor Interface Peripheral Multiplexing */
/*@{*/
#define PB20_ISI_D0_A       20  /*!< \brief Image sensor data bit 0 pin. */
#define PB21_ISI_D1_A       21  /*!< \brief Image sensor data bit 1 pin. */
#define PB22_ISI_D2_A       22  /*!< \brief Image sensor data bit 2 pin. */
#define PB23_ISI_D3_A       23  /*!< \brief Image sensor data bit 3 pin. */
#define PB24_ISI_D4_A       24  /*!< \brief Image sensor data bit 4 pin. */
#define PB25_ISI_D5_A       25  /*!< \brief Image sensor data bit 5 pin. */
#define PB26_ISI_D6_A       26  /*!< \brief Image sensor data bit 6 pin. */
#define PB27_ISI_D7_A       27  /*!< \brief Image sensor data bit 7 pin. */
#define PB10_ISI_D8_B       10  /*!< \brief Image sensor data bit 8 pin. */
#define PB11_ISI_D9_B       11  /*!< \brief Image sensor data bit 9 pin. */
#define PB12_ISI_D10_B      12  /*!< \brief Image sensor data bit 10 pin. */
#define PB13_ISI_D11_B      13  /*!< \brief Image sensor data bit 11 pin. */
#define PB28_ISI_PCK_A      28  /*!< \brief Image sensor data clock pin. */
#define PB29_ISI_VSYNC_A    29  /*!< \brief Image sensor vertical sync pin. */
#define PB30_ISI_HSYNC_A    30  /*!< \brief Image sensor horizontal sync pin. */
#define PB31_ISI_MCK_A      31  /*!< \brief Image sensor reference clock pin. */
/*@}*/

/*! \name EMAC Interface Peripheral Multiplexing */
/*@{*/
#define PA6_ETX2_B          6   /*!< \brief Transmit data bit 2 pin. */
#define PA7_ETX3_B          7   /*!< \brief Transmit data bit 3 pin. */
#define PA10_ETX0_A         10  /*!< \brief Transmit data bit 0 pin. */
#define PA11_ETX1_A         11  /*!< \brief Transmit data bit 1 pin. */
#define PA12_ERX0_A         12  /*!< \brief Receive data bit 0 pin. */
#define PA13_ERX1_A         13  /*!< \brief Receive data bit 1 pin. */
#define PA14_ETXEN_A        14  /*!< \brief Transmit enable pin. */
#define PA15_ERXDV_A        15  /*!< \brief Data valid pin. */
#define PA16_ERXER_A        16  /*!< \brief Receive error pin. */
#define PA17_ETXCK_A        17  /*!< \brief Transmit clock pin. */
#define PA18_EMDC_A         18  /*!< \brief Management data clock pin. */
#define PA19_EMDIO_A        19  /*!< \brief Management data I/O pin. */
#define PA27_ETXER_B        27  /*!< \brief Transmit error pin. */
#define PA8_ERX2_B          8   /*!< \brief Receive data bit 2 pin. */
#define PA9_ERX3_B          9   /*!< \brief Receive data bit 3 pin. */
#define PA28_ERXCK_B        28  /*!< \brief Receive clock pin. */
#define PA29_ECRS_B         29  /*!< \brief Carrier sense pin. */
#define PA30_ECOL_B         30  /*!< \brief Collision detect pin. */
/*@}*/

/*! \name AT91SAM9G45-EKES-Specific LAN defintitions */
/*@{*/
#define PHY_MODE_RMII
#define EMAC_PIO_ASR				PIO_ASR_OFF
#define PHY_MII_PINS_A			(PA10_ETX0_A | PA11_ETX1_A | PA12_ERX0_A | PA13_ERX1_A | PA14_ETXEN_A | PA15_ERXDV_A | PA16_ERXER_A | \
                             PA17_ETXCK_A | PA18_EMDC_A | PA19_EMDIO_A)
#define EMAC_PIO_BSR				PIO_BSR_OFF
#define PHY_MII_PINS_B			(PA6_ETX2_B | PA7_ETX3_B | PA27_ETXER_B | PA8_ERX2_B | PA9_ERX3_B | PA28_ERXCK_B | PA29_ECRS_B | PA30_ECOL_B)
#define EMAC_PIO_PDR				PIOA_PDR
/*@}*/

/*! \name ADC Interface Peripheral Multiplexing */
/*@{*/
#define PD28_ADTRG_A        28  /*!< \brief ADC trigger pin. */
/*@}*/

/*! \name Debug Unit Peripheral Multiplexing */
/*@{*/
#define PB12_DRXD_A         12  /*!< \brief Debug unit receive data pin. */
#define PB13_DTXD_A         13  /*!< \brief Debug unit transmit data pin. */
/*@}*/

/*! \name Synchronous Serial Controller Peripheral Multiplexing */
/*@{*/
#define PD2_TD0_A           2  /*!< \brief Transmit data pin. */
#define PD3_RD0_A           3  /*!< \brief Receive data pin. */
#define PD0_TK0_A           0  /*!< \brief Transmit clock pin. */
#define PD4_RK0_A           4  /*!< \brief Receive clock pin. */
#define PD1_TF0_A           1  /*!< \brief Transmit frame sync. pin. */
#define PD5_RF0_A           5  /*!< \brief Receive frame sync. pin. */
/*@}*/

/*! \name Two Wire Interface Peripheral Multiplexing */
/*@{*/
#define PA20_TWD0_A          20  /*!< \brief Two wire serial data pin. */
#define PA21_TWCK0_A         21  /*!< \brief Two wire serial clock pin. */

#define PB10_TWD1_A          10  /*!< \brief Two wire serial data pin. */
#define PB11_TWCK1_A         11  /*!< \brief Two wire serial clock pin. */
/*@}*/

/*! \name Timer/Counter Peripheral Multiplexing */
/*@{*/
#define PD23_TCLK0_A         23   /*!< \brief Timer/counter 0 external clock input. */
#define PD20_TIOA0_A         20   /*!< \brief Timer/counter 0 I/O line A. */
#define PD30_TIOB0_A         30   /*!< \brief Timer/counter 0 I/O line B. */

#define PD29_TCLK1_A         29   /*!< \brief Timer/counter 1 external clock input. */
#define PD21_TIOA1_A         21   /*!< \brief Timer/counter 1 I/O line A. */
#define PD31_TIOB1_A         31   /*!< \brief Timer/counter 1 I/O line B. */

#define PC10_TCLK2_B         10   /*!< \brief Timer/counter 2 external clock input. */
#define PD22_TIOA2_A         22   /*!< \brief Timer/counter 2 I/O line A. */
#define PA26_TIOB2_B         26   /*!< \brief Timer/counter 2 I/O line B. */

#define PA0_TCLK3_B          0    /*!< \brief Timer/counter 3 external clock input. */
#define PA1_TIOA3_B          1    /*!< \brief Timer/counter 3 I/O line A. */
#define PA2_TIOB3_B          2    /*!< \brief Timer/counter 3 I/O line B. */

#define PA3_TCLK4_B          3    /*!< \brief Timer/counter 4 external clock input. */
#define PA4_TIOA4_B          4    /*!< \brief Timer/counter 4 I/O line A. */
#define PA5_TIOB4_B          5    /*!< \brief Timer/counter 4 I/O line B. */

#define PD9_TCLK5_B          9    /*!< \brief Timer/counter 5 external clock input. */
#define PD7_TIOA5_B          7    /*!< \brief Timer/counter 5 I/O line A. */
#define PD8_TIOB5_B          8    /*!< \brief Timer/counter 5 I/O line B. */
/*@}*/

/*! \name Clocks, Oscillators and PLLs Peripheral Multiplexing */
/*@{*/
#define PD26_PCK0_A         26  /*!< \brief Programmable clock 0 output pin. */
#define PE0_PCK0_B          0   /*!< \brief Programmable clock 0 output pin. */
#define PD27_PCK1_A         27  /*!< \brief Programmable clock 1 output pin. */
#define PE31_PCK1_B         31  /*!< \brief Programmable clock 1 output pin. */
/*@}*/

/*! \name CompactFlash Peripheral Multiplexing */
/*@{*/
#define PC12_A25_CFRNW_A    12   /*!< \brief Read not write pin. */
#define PC10_NCS4_CFCS0_A   10   /*!< \brief Chip select line 0 pin. */
#define PC11_NCS5_CFCS1_A   11   /*!< \brief Chip select line 1 pin. */
#define PC8_CFCE1_A         8    /*!< \brief Chip enable line 1 pin. */
#define PC9_CFCE2_A         9    /*!< \brief Chip enable line 2 pin. */
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
#define PC6_A23_A           6   /*!< \brief Address bus bit 23 pin. */
#define PC7_A24_A           7   /*!< \brief Address bus bit 24 pin. */
#define PC13_NCS2_A         13  /*!< \brief Negated chip select 2 pin. */
#define PC14_NCS3_NANDCS_A  14  /*!< \brief Negated chip select 3 pin. */
#define PC15_NWAIT_A        15  /*!< \brief External wait signal pin. */
/*@}*/

/*! \name Advanced Interrupt Controller Peripheral Multiplexing */
/*@{*/
#define PD19_FIQ_B          19  /*!< \brief Fast interrupt input pin. */
#define PC18_IRQ_B          18  /*!< \brief External interrupt input pin. */
/*@}*/

/*! \name LCD Port definition */
/*@{*/
#define LCDC_PIO_BASE       PIOE_BASE
#define LCDC_PINS_A         0x6FEFFFDE
#define LCDC_PINS_B         0x10100000
#define LCDC_PINS           (LCDC_PINS_A | LCDC_PINS_B)
#define LCDC_PIO_ASR        PIOE_ASR
#define LCDC_PIO_BSR        PIOE_BSR
#define LCDC_PIO_PDR        PIOE_PDR

/*@}*/

/*@} xgNutArchArmAt91Sam9g45 */

#endif                          /* _ARCH_ARM_SAM9G45_H_ */
