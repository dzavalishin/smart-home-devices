--
-- Copyright (C) 2004-2007 by egnite Software GmbH. All rights reserved.
--
-- Redistribution and use in source and binary forms, with or without
-- modification, are permitted provided that the following conditions
-- are met:
--
-- 1. Redistributions of source code must retain the above copyright
--    notice, this list of conditions and the following disclaimer.
-- 2. Redistributions in binary form must reproduce the above copyright
--    notice, this list of conditions and the following disclaimer in the
--    documentation and/or other materials provided with the distribution.
-- 3. Neither the name of the copyright holders nor the names of
--    contributors may be used to endorse or promote products derived
--    from this software without specific prior written permission.
--
-- THIS SOFTWARE IS PROVIDED BY EGNITE SOFTWARE GMBH AND CONTRIBUTORS
-- ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
-- LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
-- FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL EGNITE
-- SOFTWARE GMBH OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
-- INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
-- BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
-- OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
-- AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
-- OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
-- THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
-- SUCH DAMAGE.
--
-- For additional information see http://www.ethernut.de/
--

-- ARM Architecture
--
-- $Log$
-- Revision 1.41  2010/12/15 21:46:28  ve2yag
-- Add PWM interrupt support for AT91SAM7S and SE.
--
-- Revision 1.40  2009/09/20 13:24:58  ulrichprinz
-- Added limited USART support for DBGU on SAM.
--
-- Revision 1.39  2009/02/17 09:32:25  haraldkipp
-- A lot of clean-up had been done with SAM9260 initialization.
-- Clock configurations should now work as expected. Note, that
-- the CPU is running now at 198.6560 MHz with peripheral clock
-- at 99.3280 MHz.
--
-- Revision 1.38  2009/02/06 15:57:05  haraldkipp
-- Removed stack size defaults.
--
-- Revision 1.37  2009/01/18 16:46:18  haraldkipp
-- Properly distinguish between PIO IDs and port numbers.
-- Call internal Lua functions when needed only.
--
-- Revision 1.36  2009/01/16 19:44:05  haraldkipp
-- ARM crash dump added.
--
-- Revision 1.35  2009/01/09 17:54:21  haraldkipp
-- Added SPI bus controller for AVR and AT91.
--
-- Revision 1.34  2008/12/15 19:18:49  haraldkipp
-- Enable DataFlash support for EIR board.
--
-- Revision 1.33  2008/10/23 08:51:22  haraldkipp
-- Added MCU family macros and AT91 UART hardware handshake.
--
-- Revision 1.32  2008/10/05 16:51:46  haraldkipp
-- Added suport for the TLV320 audio DAC.
--
-- Revision 1.31  2008/10/03 11:31:27  haraldkipp
-- Added TWI support for the AT91SAM9260.
--
-- Revision 1.30  2008/09/23 07:25:47  haraldkipp
-- Made AT91 SDRAM settings configurable.
-- Simplified AT91 TWI configuration.
-- Added AT91 reset controller module.
--
-- Revision 1.29  2008/09/02 14:29:37  haraldkipp
-- Added a new MCU family macro to avoid these lengthy preprocessor
-- conditionals.
--
-- Revision 1.28  2008/08/06 12:51:08  haraldkipp
-- Added support for Ethernut 5 (AT91SAM9XE reference design).
--
-- Revision 1.27  2008/07/29 07:31:40  haraldkipp
-- Re-enabled NUT_CONFIG_AT91EFC for the SAM7SE.
--
-- Revision 1.26  2008/06/06 10:28:22  haraldkipp
-- ST7036 LCD controller settings moved from source to configuration files.
--
-- Revision 1.25  2008/04/18 13:24:57  haraldkipp
-- Added Szemzo Andras' RS485 patch.
--
-- Revision 1.24  2008/02/15 17:05:53  haraldkipp
-- AT91SAM7SE512 support added. Global MCU list simplifies exclusivity
-- attribute. HW_EBI_AT91 added for MCUs with external bus interface.
--
-- Revision 1.23  2008/02/09 02:11:45  olereinhardt
-- 2008-02-08  Ole Reinhardt <ole.reinhardt@embedded-it.de>
--      * conf/arch/arm.nut: Added ih_at91spi for sam7x, Support for
--           at91sam7s added
--
-- Revision 1.22  2008/01/31 09:22:32  haraldkipp
-- Added first version of platform independent GPIO routines. Consider the
-- AVR version untested.
--
-- Revision 1.21  2007/12/09 21:50:38  olereinhardt
-- Added config options for at91 adc driver
--
-- Revision 1.20  2007/10/12 17:32:16  haraldkipp
-- AT91 ADC code not yet available.
--
-- Revision 1.19  2007/10/04 21:06:27  olereinhardt
-- Support for SAM7S256 added
--
-- Revision 1.18  2007/09/06 19:43:19  olereinhardt
-- Added support for AT91 TWI driver
--
-- Revision 1.17  2007/02/15 16:04:34  haraldkipp
-- Configurable AT91 EMAC buffer usage and link timeout.
-- Port usage for HD44780 is now configurable. Data bits no longer need four
-- consecutive port bits.
-- Added support for ST7036 based displays. Tested with EA DOG-M LCDs.
-- Support for AT91 system controller interrupts added.
-- Support for AT91 periodic interval timer added.
-- Configurable SAM7 and SAM9 runtime initialization.
--
-- Revision 1.16  2006/10/08 16:41:34  haraldkipp
-- PHY address and power down bit are now configurable.
--
-- Revision 1.15  2006/09/29 12:34:59  haraldkipp
-- Basic AT91 SPI support added.
--
-- Revision 1.14  2006/09/08 16:47:24  haraldkipp
-- For some reason the SSC driver for SAM7X had not been included.
--
-- Revision 1.13  2006/09/07 09:03:43  haraldkipp
-- Corrected descriptions. SAM7X and SAM9260 are both using the same
-- Ethernet driver source. at91sam7x_emac.c will become obsolete.
-- SSC driver code added to SAM7X build.
--
-- Revision 1.12  2006/09/05 12:29:59  haraldkipp
-- SPI and MCI support added for SAM9260.
--
-- Revision 1.11  2006/08/31 19:04:08  haraldkipp
-- Added support for the AT91SAM9260 and Atmel's AT91SAM9260 Evaluation Kit.
--
-- Revision 1.10  2006/07/26 11:19:06  haraldkipp
-- Defining AT91_PLL_MAINCK will automatically determine SAM7X clock by
-- reading PLL settings.
-- Added MMC/SD-Card support for AT91SAM7X Evaluation Kit.
--
-- Revision 1.9  2006/07/05 08:02:17  haraldkipp
-- SAM7X interrupt and EMAC support added.
--
-- Revision 1.8  2006/06/28 17:22:34  haraldkipp
-- Make it compile for AT91SAM7X256.
--
-- Revision 1.7  2006/05/25 09:13:22  haraldkipp
-- Platform independent watchdog API added.
--
-- Revision 1.6  2006/04/07 12:24:12  haraldkipp
-- ARM driver for HD44780 LCD controller added.
--
-- Revision 1.5  2006/03/02 19:56:10  haraldkipp
-- First attempt to compile with ICCARM. All compile errors fixed, but not
-- a finished port yet. Many things are missing.
-- Added MCU specific hardware initialization routine. For the AT91 the
-- spurious interrupt handler has been added, which fixes SF 1440948.
--
-- Revision 1.4  2006/02/23 15:41:40  haraldkipp
-- Added support for AT91 watchdog timer.
--
-- Revision 1.3  2005/11/20 14:40:28  haraldkipp
-- Added interrupt driven UART driver for AT91.
--
-- Revision 1.2  2005/10/24 09:52:32  haraldkipp
-- New AT91 interrupt handler routines.
-- New DM9000E Ethernet driver for Ethernut 3 board.
--
-- Revision 1.1  2005/07/26 15:41:06  haraldkipp
-- All target dependent code is has been moved to a new library named
-- libnutarch. Each platform got its own script.
--
--

nutarch_arm =
{
    --
    -- MCU Family
    --
    {
        name = "nutarch_arm_family",
        brief = "MCU Family",
        options =
        {
            {
                macro = "MCU_AT91",
                brief = "Atmel AT91",
                type = "integer",
                default = 1,
                requires = { "HW_MCU_AT91" },
                file = "include/cfg/arch.h"
            }
        }
    },
    {
        name = "nutarch_at91_family",
        brief = "Atmel AT91 Family",
        requires = { "HW_MCU_AT91" },
        options =
        {
            {
                macro = "MCU_AT91SAM7S",
                brief = "Atmel AT91SAM7S",
                type = "integer",
                default = 1,
                requires = { "HW_MCU_AT91SAM7S" },
                file = "include/cfg/arch.h"
            },
            {
                macro = "MCU_AT91SAM7SE",
                brief = "Atmel AT91SAM7SE",
                type = "integer",
                default = 1,
                requires = { "HW_MCU_AT91SAM7SE" },
                file = "include/cfg/arch.h"
            },
            {
                macro = "MCU_AT91SAM7X",
                brief = "Atmel AT91SAM7X",
                type = "integer",
                default = 1,
                requires = { "HW_MCU_AT91SAM7X" },
                file = "include/cfg/arch.h"
            },
            {
                macro = "MCU_AT91SAM9XE",
                brief = "Atmel AT91SAM9XE",
                type = "integer",
                default = 1,
                requires = { "HW_MCU_AT91SAM9XE" },
                file = "include/cfg/arch.h"
            },
            {
                macro = "MCU_AT91SAM9G45",
                brief = "Atmel AT91SAM9G45",
                type = "integer",
                default = 1,
                requires = { "HW_MCU_AT91SAM9G45" },
                file = "include/cfg/arch.h"
            }
        }
    },

    --
    -- Runtime Initialization
    --
    {
        name = "nutarch_arm_cstartup",
        brief = "ARM-GCC Startup",
        sources = { "arm/init/crt$(LDNAME).S" },
        targets = { "arm/init/crt$(LDNAME).o" },
        -- ICCARM: FIXME!
        requires = { "TOOL_CC_ARM", "TOOL_GCC" },
        options =
        {
            {
                macro = "IRQ_STACK_SIZE",
                brief = "IRQ Stack Size",
                description = "Number of bytes reserved for interrupt stack\n"..
                              "Default is 512 (128 words).",
                flavor = "booldata",
                file = "include/cfg/memory.h"
            },
            {
                macro = "FIQ_STACK_SIZE",
                brief = "FIQ Stack Size",
                description = "Number of bytes reserved for fast interrupt stack\n"..
                              "Default is 256 (64 words).",
                flavor = "booldata",
                file = "include/cfg/memory.h"
            },
            {
                macro = "ABT_STACK_SIZE",
                brief = "ABT Stack Size",
                description = "Number of bytes reserved for abort exception stack\n"..
                              "Default is 128 (32 words).",
                flavor = "booldata",
                file = "include/cfg/memory.h"
            },
            {
                macro = "UND_STACK_SIZE",
                brief = "UND Stack Size",
                description = "Number of bytes reserved for undefined exception stack\n"..
                              "Default is 128 (32 words).",
                flavor = "booldata",
                file = "include/cfg/memory.h"
            },
            {
                macro = "PLL_DIV_VAL",
                brief = "PLL Divider",
                description = "Make sure to read the datasheet before modifying this value.\n\n"..
                              "The main clock (crystal) will be divided by this value "..
                              "and multiplied by the PLL multiplier value plus 1 to "..
                              "generate the PLL output frequency.\n\n"..
                              "Default values are\n"..
                              "SAM7X: 14\n"..
                              "SAM7S: 14\n"..
                              "SAM7SE: 14\n"..
                              "SAM9260: 9\n",
                requires = { "HW_PLL_AT91" },
                flavor = "booldata",
                file = "include/cfg/clock.h"
            },
            {
                macro = "PLL_MUL_VAL",
                brief = "PLL Multiplier",
                description = "Make sure to read the datasheet before modifying this value.\n\n"..
                              "The main clock (crystal) will be divided by the PLL "..
                              "divider value and multiplied by the specified multiplier "..
                              " value plus 1 to generate the PLL output frequency.\n\n"..
                              "Default values are\n"..
                              "SAM7X: 72\n"..
                              "SAM7S: 72\n"..
                              "SAM7SE: 72\n"..
                              "SAM9260: 96\n",
                requires = { "HW_PLL_AT91" },
                flavor = "booldata",
                file = "include/cfg/clock.h"
            },
            {
                macro = "MASTER_CLOCK_PRES",
                brief = "Master Clock Prescaler",
                description = "The selected clock will be divided by this value to generate "..
                              "the master clock. Possible values are 1, 2 (default), 4, 8, 16, 32 or 64.",
                flavor = "booldata",
                file = "include/cfg/clock.h"
            },
            {
                macro = "NUTMEM_SDRAM_BASE",
                brief = "SDRAM Base Address",
                requires = { "HW_SDRAMC" },
                provides = { "HW_SDRAM" },
                default = "0x20000000",
                flavor = "booldata",
                file = "include/cfg/memory.h"
            },
            {
                macro = "NUTMEM_SDRAM_BANKS",
                brief = "SDRAM Banks",
                requires = { "HW_SDRAM" },
                default = "4",
                file = "include/cfg/memory.h"
            },
            {
                macro = "NUTMEM_SDRAM_COLBITS",
                brief = "SDRAM Column Address Bits",
                requires = { "HW_SDRAM" },
                default = "10",
                file = "include/cfg/memory.h"
            },
            {
                macro = "NUTMEM_SDRAM_ROWBITS",
                brief = "SDRAM Row Address Bits",
                requires = { "HW_SDRAM" },
                default = "13",
                file = "include/cfg/memory.h"
            },
            {
                macro = "NUTMEM_SDRAM_CASLAT",
                brief = "SDRAM CAS Latency",
                requires = { "HW_SDRAM" },
                default = "2",
                file = "include/cfg/memory.h"
            },
            {
                macro = "NUTMEM_SDRAM_TWR",
                brief = "SDRAM Write Recovery Cycles",
                requires = { "HW_SDRAM" },
                default = "2",
                file = "include/cfg/memory.h"
            },
            {
                macro = "NUTMEM_SDRAM_TRC",
                brief = "SDRAM Row Cycle Delay",
                requires = { "HW_SDRAM" },
                default = "4",
                file = "include/cfg/memory.h"
            },
            {
                macro = "NUTMEM_SDRAM_TRP",
                brief = "SDRAM Row Precharge Delay",
                requires = { "HW_SDRAM" },
                default = "4",
                file = "include/cfg/memory.h"
            },
            {
                macro = "NUTMEM_SDRAM_TRCD",
                brief = "SDRAM Row to Column Delay",
                requires = { "HW_SDRAM" },
                default = "2",
                file = "include/cfg/memory.h"
            },
            {
                macro = "NUTMEM_SDRAM_TRAS",
                brief = "SDRAM Active to Precharge Delay",
                requires = { "HW_SDRAM" },
                default = "3",
                file = "include/cfg/memory.h"
            },
            {
                macro = "NUTMEM_SDRAM_TXSR",
                brief = "SDRAM Self Refresh to Active Delay",
                requires = { "HW_SDRAM" },
                default = "4",
                file = "include/cfg/memory.h"
            },
        },
    },

    --
    -- Board Initialization
    --
    {
        name = "nutarch_arm_bs",
        brief = "Board Support",
        sources =
            function()
                return { "arm/board/"..string.lower(c_macro_edit("PLATFORM"))..".c" };
            end,
        requires = { "HW_BOARD_SUPPORT" },
    },

    --
    -- Context Switching
    --
    {
        name = "nutarch_arm_context",
        brief = "Context Switching",
        provides = { "NUT_CONTEXT_SWITCH" },
        requires = { "HW_MCU_ARM", "TOOL_GCC" },
        sources = { "arm/os/context.c" },
    },

    --
    -- System Timer Hardware
    --
    {
        name = "nutarch_ostimer_at91",
        brief = "System Timer (AT91)",
        requires = { "HW_TIMER_AT91" },
        provides = { "NUT_OSTIMER_DEV" },
        sources = { "arm/dev/atmel/ostimer_at91.c" },
        options =
        {
            {
                macro = "AT91_PLL_MAINCK",
                brief = "AT91 Main Clock",
                description = "Frequency of the external crystal. If this option is "..
                              "enabled and NUT_CPU_FREQ is not enabled, then the "..
                              "specified value will be used together with "..
                              "the PLL register settings to determine the CPU master clock. "..
                              "Otherwise you must specify NUT_CPU_FREQ.",
                requires = { "HW_PLL_AT91" },
                flavor = "booldata",
                file = "include/cfg/clock.h"
            },
            {
                macro = "NUT_TICK_AT91PIT",
                brief = "Use PIT",
                description = "If selected, the AT91 periodic interval timer is used "..
                              "to generate system ticks. Otherwise timer 0 is used. ",
                requires = { "HW_PIT_AT91" },
                flavor = "booldata",
                file = "include/cfg/clock.h"
            },
        },
    },
    {
        name = "nutarch_ostimer_gba",
        brief = "System Timer (GBA)",
        requires = { "HW_TIMER_GBA" },
        provides = { "NUT_OSTIMER_DEV" },
        sources = { "arm/dev/gba/ostimer_gba.c" },
    },
    {
        name = "nutarch_wdt_at91",
        brief = "Watchdog Timer (AT91)",
        requires = { "HW_WDOG_AT91" },
        sources = { "arm/dev/atmel/wdt_at91.c" },
    },

    --
    -- Interrupt handling.
    --
    {
        name = "nutarch_arm_irqgba",
        brief = "Interrupt Handler (GBA)",
        requires = { "HW_MCU_GBA" },
        provides = { "DEV_IRQ_GBA" },
        sources = { "arm/dev/gba/ihndlr_gba.c" },
    },
    {
        name = "nutarch_arm_irqat91x40",
        brief = "Interrupt Handler (AT91R40XX)",
        requires = { "HW_MCU_AT91R40008" },
        provides = { "DEV_IRQ_AT91" },
        sources =
        {
            "arm/dev/atmel/ih_at91fiq.c",
            "arm/dev/atmel/ih_at91irq0.c",
            "arm/dev/atmel/ih_at91irq1.c",
            "arm/dev/atmel/ih_at91irq2.c",
            "arm/dev/atmel/ih_at91pio.c",
            "arm/dev/atmel/ih_at91swirq.c",
            "arm/dev/atmel/ih_at91tc0.c",
            "arm/dev/atmel/ih_at91tc1.c",
            "arm/dev/atmel/ih_at91tc2.c",
            "arm/dev/atmel/ih_at91uart0.c",
            "arm/dev/atmel/ih_at91uart1.c",
            "arm/dev/atmel/ih_at91wdi.c"
        },
    },
    {
        name = "nutarch_arm_irqat91sam7x",
        brief = "Interrupt Handler (SAM7X)",
        requires = { "HW_MCU_AT91SAM7X" },
        provides = { "DEV_IRQ_AT91" },
        sources =
        {
            "arm/dev/atmel/ih_at91fiq.c",
            "arm/dev/atmel/ih_at91sys.c",
            "arm/dev/atmel/ih_at91irq0.c",
            "arm/dev/atmel/ih_at91irq1.c",
            "arm/dev/atmel/ih_at91irq2.c",
            "arm/dev/atmel/ih_at91pioa.c",
            "arm/dev/atmel/ih_at91piob.c",
            "arm/dev/atmel/ih_at91pioc.c",
            "arm/dev/atmel/ih_at91spi0.c",
            "arm/dev/atmel/ih_at91spi1.c",
            "arm/dev/atmel/ih_at91ssc.c",
            "arm/dev/atmel/ih_at91swirq.c",
            "arm/dev/atmel/ih_at91tc0.c",
            "arm/dev/atmel/ih_at91tc1.c",
            "arm/dev/atmel/ih_at91tc2.c",
            "arm/dev/atmel/ih_at91adc.c",
            "arm/dev/atmel/ih_at91twi.c",
            "arm/dev/atmel/ih_at91uart0.c",
            "arm/dev/atmel/ih_at91uart1.c",
            "arm/dev/atmel/ih_at91emac.c",
            "arm/dev/atmel/ih_at91wdi.c",
        },
    },
    {
        name = "nutarch_arm_irqat91sam7s",
        brief = "Interrupt Handler (SAM7S)",
        requires = { "HW_MCU_AT91SAM7S" },
        provides = { "DEV_IRQ_AT91" },
        sources =
        {
            "arm/dev/atmel/ih_at91fiq.c",
            "arm/dev/atmel/ih_at91sys.c",
            "arm/dev/atmel/ih_at91irq0.c",
            "arm/dev/atmel/ih_at91irq1.c",
            "arm/dev/atmel/ih_at91irq2.c",
            "arm/dev/atmel/ih_at91pio.c",
            "arm/dev/atmel/ih_at91pioa.c",
            "arm/dev/atmel/ih_at91spi0.c",
            "arm/dev/atmel/ih_at91ssc.c",
            "arm/dev/atmel/ih_at91swirq.c",
            "arm/dev/atmel/ih_at91tc0.c",
            "arm/dev/atmel/ih_at91tc1.c",
            "arm/dev/atmel/ih_at91tc2.c",
            "arm/dev/atmel/ih_at91pwmc.c",
            "arm/dev/atmel/ih_at91adc.c",
            "arm/dev/atmel/ih_at91twi.c",
            "arm/dev/atmel/ih_at91uart0.c",
            "arm/dev/atmel/ih_at91uart1.c",
            "arm/dev/atmel/ih_at91wdi.c"
        },
    },
    {
        name = "nutarch_arm_irqat91sam7se",
        brief = "Interrupt Handler (SAM7SE)",
        requires = { "HW_MCU_AT91SAM7SE" },
        provides = { "DEV_IRQ_AT91" },
        sources =
        {
            "arm/dev/atmel/ih_at91fiq.c",
            "arm/dev/atmel/ih_at91sys.c",
            "arm/dev/atmel/ih_at91irq0.c",
            "arm/dev/atmel/ih_at91irq1.c",
            "arm/dev/atmel/ih_at91irq2.c",
            "arm/dev/atmel/ih_at91pio.c",
            "arm/dev/atmel/ih_at91pioa.c",
            "arm/dev/atmel/ih_at91piob.c",
            "arm/dev/atmel/ih_at91pioc.c",
            "arm/dev/atmel/ih_at91spi0.c",
            "arm/dev/atmel/ih_at91ssc.c",
            "arm/dev/atmel/ih_at91swirq.c",
            "arm/dev/atmel/ih_at91tc0.c",
            "arm/dev/atmel/ih_at91tc1.c",
            "arm/dev/atmel/ih_at91tc2.c",
            "arm/dev/atmel/ih_at91pwmc.c",
            "arm/dev/atmel/ih_at91adc.c",
            "arm/dev/atmel/ih_at91twi.c",
            "arm/dev/atmel/ih_at91uart0.c",
            "arm/dev/atmel/ih_at91uart1.c",
            "arm/dev/atmel/ih_at91wdi.c"
        },
    },
    {
        name = "nutarch_arm_irqat91sam926x",
        brief = "Interrupt Handler (SAM926X)",
        requires = { "HW_MCU_AT91SAM9260" },
        provides = { "DEV_IRQ_AT91" },
        sources =
        {
            "arm/dev/atmel/ih_at91emac.c",
            "arm/dev/atmel/ih_at91fiq.c",
            "arm/dev/atmel/ih_at91irq0.c",
            "arm/dev/atmel/ih_at91irq1.c",
            "arm/dev/atmel/ih_at91irq2.c",
            "arm/dev/atmel/ih_at91pioa.c",
            "arm/dev/atmel/ih_at91piob.c",
            "arm/dev/atmel/ih_at91pioc.c",
            "arm/dev/atmel/ih_at91spi0.c",
            "arm/dev/atmel/ih_at91spi1.c",
            "arm/dev/atmel/ih_at91ssc.c",
            "arm/dev/atmel/ih_at91swirq.c",
            "arm/dev/atmel/ih_at91tc0.c",
            "arm/dev/atmel/ih_at91tc1.c",
            "arm/dev/atmel/ih_at91tc2.c",
            "arm/dev/atmel/ih_at91twi.c",
            "arm/dev/atmel/ih_at91uart0.c",
            "arm/dev/atmel/ih_at91uart1.c",
        },
    },
    {
        name = "nutarch_arm_irqat91sam9g45",
        brief = "Interrupt Handler (SAM9G45)",
        requires = { "HW_MCU_AT91SAM9G45" },
        provides = { "DEV_IRQ_AT91" },
        sources =
        {
            "arm/dev/atmel/ih_at91sys.c",
            "arm/dev/atmel/ih_at91emac.c",
            "arm/dev/atmel/ih_at91fiq.c",
            "arm/dev/atmel/ih_at91irq0.c",
            "arm/dev/atmel/ih_at91pioa.c",
            "arm/dev/atmel/ih_at91piob.c",
            "arm/dev/atmel/ih_at91pioc.c",
            "arm/dev/atmel/ih_at91spi0.c",
            "arm/dev/atmel/ih_at91spi1.c",
            "arm/dev/atmel/ih_at91ssc.c",
            "arm/dev/atmel/ih_at91swirq.c",
            "arm/dev/atmel/ih_at91tc0.c",
            "arm/dev/atmel/ih_at91tc1.c",
            "arm/dev/atmel/ih_at91tc2.c",
            "arm/dev/atmel/ih_at91twi.c",
            "arm/dev/atmel/ih_at91uart0.c",
            "arm/dev/atmel/ih_at91uart1.c",
        },
    },
    {
        name = "nutarch_arm_irqat91sam9xe",
        brief = "Interrupt Handler (SAM9XE)",
        requires = { "HW_MCU_AT91SAM9XE" },
        provides = { "DEV_IRQ_AT91" },
        sources =
        {
            "arm/dev/atmel/ih_at91sys.c",
            "arm/dev/atmel/ih_at91emac.c",
            "arm/dev/atmel/ih_at91fiq.c",
            "arm/dev/atmel/ih_at91irq0.c",
            "arm/dev/atmel/ih_at91irq1.c",
            "arm/dev/atmel/ih_at91irq2.c",
            "arm/dev/atmel/ih_at91pioa.c",
            "arm/dev/atmel/ih_at91piob.c",
            "arm/dev/atmel/ih_at91pioc.c",
            "arm/dev/atmel/ih_at91spi0.c",
            "arm/dev/atmel/ih_at91spi1.c",
            "arm/dev/atmel/ih_at91ssc.c",
            "arm/dev/atmel/ih_at91swirq.c",
            "arm/dev/atmel/ih_at91tc0.c",
            "arm/dev/atmel/ih_at91tc1.c",
            "arm/dev/atmel/ih_at91tc2.c",
            "arm/dev/atmel/ih_at91twi.c",
            "arm/dev/atmel/ih_at91uart0.c",
            "arm/dev/atmel/ih_at91uart1.c",
        },
    },
    {
        name = "nutarch_arm_except_ent",
        brief = "Debug Exception Entries",
        sources =
        {
            "arm/debug/arm-da.S",
            "arm/debug/arm-pfa.S",
            "arm/debug/arm-swi.S",
            "arm/debug/arm-udf.S"
        },
        targets =
        {
            "arm/debug/arm-da.o",
            "arm/debug/arm-pfa.o",
            "arm/debug/arm-swi.o",
            "arm/debug/arm-udf.o"
        },
    },
    {
        name = "nutarch_arm_except_hdl",
        brief = "Debug Exception Handler",
        sources =
        {
            "arm/debug/common_xxx_handler.c",
            "arm/debug/default_da_handler.c",
            "arm/debug/default_pfa_handler.c",
            "arm/debug/default_swi_handler.c",
            "arm/debug/default_udf_handler.c",
            "arm/debug/reg-dump.c",
            "arm/debug/stackdump.c"
        },
    },

    --
    -- Device Drivers
    --
    {
        name = "nutarch_arm_debug",
        brief = "UART Debug Output (AT91)",
        requires = { "HW_UART_AT91" },
        provides = { "DEV_UART", "DEV_FILE", "DEV_WRITE" },
        sources = { "arm/dev/atmel/debug_at91.c",
                    "arm/dev/atmel/at91_dbg0.c",
                    "arm/dev/atmel/at91_dbg1.c",
                    "arm/dev/atmel/at91_dbgu.c" }
    },
    {
        name = "nutarch_arm_usart0",
        brief = "USART0 Driver",
        description = "Hardware specific USART driver. Implements hardware "..
                      "functions for the generic driver framework.",
        requires = { "HW_UART_AT91", "DEV_IRQ_AT91", "NUT_EVENT", "CRT_HEAPMEM" },
        provides = {"DEV_UART_SPECIFIC",
                    "DEV_UART0_GPIO_RTS",
                    "DEV_UART0_GPIO_CTS",
                    "DEV_UART0_GPIO_HDX" },
        sources = { "arm/dev/atmel/usart0at91.c" },
--        options =
--        {
--            {
--                macro = "UART0_RXTX_ONLY",
--                brief = "Receive/Transmit Only",
--                description = "When selected, the driver will not support any handshake signals.",
--                flavor = "boolean",
--                exclusivity = { "UART0_RXTX_ONLY", "UART0_HARDWARE_HANDSHAKE", "UART0_MODEM_CONTROL" },
--                file = "include/cfg/uart.h"
--            },
--            {
--                macro = "UART0_HARDWARE_HANDSHAKE",
--                brief = "Hardware Handshake",
--                description = "When selected, the driver will support RTS/CTS hardware handshake. "..
--                              "Make sure, that the related peripheral pins are available.",
--                flavor = "boolean",
--                exclusivity = { "UART0_RXTX_ONLY", "UART0_HARDWARE_HANDSHAKE", "UART0_MODEM_CONTROL" },
--                requires = { "HW_UART0_RTSCTS" },
--                file = "include/cfg/uart.h"
--            },
--            {
--                macro = "UART0_MODEM_CONTROL",
--                brief = "Full Modem Control",
--                description = "When selected, the driver will support full modem control. "..
--                              "Make sure, that all related peripheral pins are available.",
--                flavor = "boolean",
--                exclusivity = { "UART0_RXTX_ONLY", "UART0_HARDWARE_HANDSHAKE", "UART0_MODEM_CONTROL" },
--                requires = { "HW_UART0_MODEM" },
--                file = "include/cfg/uart.h"
--            },
--        },
    },
    {
        name = "nutarch_arm_usart1",
        brief = "USART1 Driver",
        description = "Hardware specific USART driver. Implements hardware "..
                      "functions for the generic driver framework.",
        requires = { "HW_UART_AT91", "DEV_IRQ_AT91", "NUT_EVENT", "CRT_HEAPMEM" },
        provides = {"DEV_UART_SPECIFIC",
                    "DEV_UART1_GPIO_RTS",
                    "DEV_UART1_GPIO_CTS",
                    "DEV_UART1_GPIO_HDX" },
        sources = { "arm/dev/atmel/usart1at91.c" },
--        options =
--        {
--            {
--                macro = "UART1_RXTX_ONLY",
--                brief = "Receive/Transmit Only",
--                description = "When selected, the driver will not support any handshake signals.",
--                flavor = "boolean",
--                exclusivity = { "UART1_RXTX_ONLY", "UART1_HARDWARE_HANDSHAKE", "UART1_MODEM_CONTROL" },
--                file = "include/cfg/uart.h"
--            },
--            {
--                macro = "UART1_HARDWARE_HANDSHAKE",
--                brief = "Hardware Handshake",
--                description = "When selected, the driver will support RTS/CTS hardware handshake. "..
--                              "Make sure, that the related peripheral pins are available.",
--                flavor = "boolean",
--                exclusivity = { "UART1_RXTX_ONLY", "UART1_HARDWARE_HANDSHAKE", "UART1_MODEM_CONTROL" },
--                requires = { "HW_UART1_RTSCTS" },
--                file = "include/cfg/uart.h"
--            },
--            {
--                macro = "UART1_MODEM_CONTROL",
--                brief = "Full Modem Control",
--                description = "When selected, the driver will support full modem control. "..
--                              "Make sure, that all related peripheral pins are available.",
--                flavor = "boolean",
--                exclusivity = { "UART1_RXTX_ONLY", "UART1_HARDWARE_HANDSHAKE", "UART1_MODEM_CONTROL" },
--                requires = { "HW_UART1_MODEM" },
--                file = "include/cfg/uart.h"
--            },
--        },
    },
    {
        name = "nutarch_arm_usartd",
        brief = "DBGU USART Driver",
        description = "Hardware specific USART driver. Implements hardware "..
                      "functions for the generic driver framework.",
        requires = { "HW_DBGU_AT91", "DEV_IRQ_AT91", "NUT_EVENT", "CRT_HEAPMEM" },
        provides = {"DEV_UART_SPECIFIC",
                    "DEV_UARTD_GPIO_RTS",
                    "DEV_UARTD_GPIO_CTS",
                    "DEV_UARTD_GPIO_HDX" },
        sources = { "arm/dev/atmel/usartDat91.c" },
--        options =
--        {
--            {
--                macro = "UARTD_RXTX_ONLY",
--                brief = "Receive/Transmit Only",
--                description = "When selected, the driver will not support any handshake signals.",
--                flavor = "boolean",
--                file = "include/cfg/uart.h"
--            },
--        },
    },
    {
        name = "nutarch_arm_ahdlc",
        brief = "AHDLC Protocol",
        description = "HDLC driver, required for PPP. This is an EXPERIMENTAL driver!",
        requires = { "HW_UART_AT91", "HW_PDC_AT91", "NUT_EVENT" },
        provides = { "PROTO_HDLC" },
        sources = { "arm/dev/atmel/at91_ahdlc.c" },
        options =
        {
            {
                macro = "NUT_THREAD_AHDLCRXSTACK",
                brief = "Receiver Thread Stack",
                description = "Number of bytes to be allocated for the stack of the AHDLC receive thread.",
                default = "1024",
                type = "integer",
                file = "include/cfg/ahdlc.h"
            },
            {
                macro = "NUT_AHDLC_RECV_DMA_SIZE",
                brief = "AT91 RXDMA Size",
                description = "Number of bytes to be allocated for the USART receive DMA buffer.",
                default = "64",
                type = "integer",
                file = "include/cfg/ahdlc.h"
            }
        }
    },
    {
    	name = "nutarch_at91_chlcd",
    	brief = "Character LCD Driver (AT91)",
    	description = "Parallel or serial connected displays like\n"..
    	              "HD44780, KS0066, KS0073 and others.\n",
        requires = { "HW_MCU_AT91" },
        provides = { "DEV_FILE", "DEV_WRITE" },
        sources = { "arm/dev/atmel/charlcd_at91.c" },
    	options =
    	{
    		--
    		--  Define selection of supported driver chips
    		--
    		{
	    		macro = "LCD_HD44780",
		        brief = "HD44780 Driver",
		        description = "Tested on the EIR 1.0 with 2x16 and 4x20 character LCD.",
		        exclusivity = { "LCD_HD44780", "LCD_KS0066", "LCD_KS0073", "LCD_ST7036" },
                flavor = "booldata",
                provides = { "LCD_GPIO" },
                file = "include/cfg/lcd.h",
		    },
    		{
	    		macro = "LCD_KS0066",
		        brief = "KS0066 Driver",
		        description = "Currently not tested.",
		        exclusivity = { "LCD_HD44780", "LCD_KS0066", "LCD_KS0073", "LCD_ST7036" },
                flavor = "booldata",
                provides = { "LCD_GPIO" },
                file = "include/cfg/lcd.h",
		    },
    		{
	    		macro = "LCD_KS0073",
		        brief = "KS0073 Driver",
		        description = "Currently not tested.",
		        exclusivity = { "LCD_HD44780", "LCD_KS0066", "LCD_KS0073", "LCD_ST7036" },
                flavor = "booldata",
                provides = { "LCD_GPIO" },
                file = "include/cfg/lcd.h",
		    },
    		{
	    		macro = "LCD_ST7036",
		        brief = "ST7036 Driver",
		        description = "Serial connected display via SPI.\nCurrently not tested.",
		        exclusivity = { "LCD_HD44780", "LCD_KS0066", "LCD_KS0073", "LCD_ST7036" },
                flavor = "booldata",
                provides = { "LCD_SPI" },
                file = "include/cfg/lcd.h",
		    },
    		--
    		--  Support for timing related parameters
    		--
            {
                macro = "LCD_ROWS",
                brief = "Rows",
                description = "The number of available display rows, either 1, 2 or 4.",
                default = "2",
                flavor = "integer",
                file = "include/cfg/lcd.h"
            },
            {
                macro = "LCD_COLS",
                brief = "Columns",
                description = "The number of available display colums, either 8, 16, 20 or 40.",
                default = "16",
                flavor = "integer",
                file = "include/cfg/lcd.h"
            },
            {
                macro = "LCD_PW_EH",
                brief = "Enable Pulse Stretch",
                description = "Set a value here if the system is to fast to guarantee a minimum enable high time:\n"..
                              "For HD44780 with Vcc=5.0V this is 230ns\n"..
                              "For HD44780 with Vcc=3.3V this is 500ns\n\n"..
                              "This value uses a NOP-Loop to stretch the enable Pulse and is directly dependand on the "..
                              "systems CPU speed. It also may differ for other chips.\n\n"..
                              "For a 5V driven display connected to EIR 1.0, this value can be left empty while it must "..
                              "be set to about 5 for a 3.3V driven HD44780 LCD.",
                requires = { "LCD_GPIO" },
                flavor = "integer",
                file = "include/cfg/lcd.h"
            },
            {
                macro = "LCD_E2E_DLY",
                brief = "Enable to Enable Delay",
                description = "Time for Enable to Enable delay in µs. This is the timespan between two\n"..
                              "consecutive accesses of the 4-bit or 8-bit bus.\n For SPI-bus driven chips, this "..
                              "is the /CS to /CS delay. This function uses NutMicroDelay()",
                default = "80",
                flavor = "integer",
                file = "include/cfg/lcd.h"
            },
            {
                macro = "LCD_SLEEP_DLY",
                brief = "Sleep after Data",
                description = "Enable this option on a timing critical system.\n"..
                              "Instead of cosecutive accessing the display this option will enable a NutSleep(1)\n"..
                              "After each data or command written out to the LCD.",
                flavor = "boolean",
                file = "include/cfg/lcd.h"
            },
    		--
    		--  Selection of parallel interface parameters
    		--
            {
                macro = "LCD_IF_8BIT",
                brief = "8-Bit Mode",
                description = "Select parallel bus width is 8 bit.\n"..
                			  "Splitting single bus lines accross ports is not "..
                			  "supported for data bit lines.\n"..
                			  "In 8 bit mode all data lines have to be aligned "..
                			  "in one row.\n"..
                			  "This option is actually not supported in this driver.",
                requires = { "LCD_GPIO" },
                flavor = "booldata",
                exclusivity = { "LCD_IF_8BIT", "LCD_IF_4BIT" },
                provides = { "LCD_IF_8BIT" },
                file = "include/cfg/lcd.h"
            },
            {
                macro = "LCD_IF_4BIT",
                brief = "Use 4-Bit Mode",
                description = "Select parallel bus width is 4 bit."..
                			  "Splitting single bus lines accross ports is not"..
                			  "supported for data bit lines.",
                requires = { "LCD_GPIO" },
                flavor = "booldata",
                exclusivity = { "LCD_IF_8BIT", "LCD_IF_4BIT" },
                provides = { "LCD_IF_4BIT" },
                file = "include/cfg/lcd.h"
            },
    		--
    		--  Selection of parallel interface parameters
    		--
            {
                macro = "LCD_DATA_PIO_ID",
                brief = "Port of LCD data pins",
                requires = { "LCD_GPIO" },
                description = "Port of the below defined data pins. Valid for "..
                              "both, single bit definitions or LSB.",
                type = "enumerated",
                choices = at91_pio_id_choice,
                file = "include/cfg/arch/armpio.h"
            },
            {
                macro = "LCD_DATA_LSB",
                brief = "Least Significant Data Bit",
                requires = { "LCD_GPIO" },
                description = "Bit number of the least significant data bit. The remaining "..
                              "data bits must be connected to the following port bits.",
                type = "enumerated",
                flavor = "booldata",
                choices = mcu_32bit_choice,
                file = "include/cfg/arch/armpio.h"
            },
            {
                macro = "LCD_DATA_BIT0",
                brief = "Data Bit 0",
                requires = { "LCD_GPIO", "LCD_IF_4BIT" },
                description = "Port bit connected to LCD data bit 0.\n"..
                              "Not used if LCD_DATA_LSB is defined.\n",
                type = "enumerated",
                choices = mcu_32bit_choice,
                file = "include/cfg/arch/armpio.h"
            },
            {
                macro = "LCD_DATA_BIT1",
                brief = "Data Bit 1",
                requires = { "LCD_GPIO", "LCD_IF_4BIT" },
                type = "enumerated",
                requires = { "LCD_IF_4BIT" },
                choices = mcu_32bit_choice,
                file = "include/cfg/arch/armpio.h"
            },
            {
                macro = "LCD_DATA_BIT2",
                brief = "Data Bit 2",
                requires = { "LCD_GPIO", "LCD_IF_4BIT" },
                type = "enumerated",
                choices = mcu_32bit_choice,
                file = "include/cfg/arch/armpio.h"
            },
            {
                macro = "LCD_DATA_BIT3",
                brief = "Data Bit 3",
                requires = { "LCD_GPIO", "LCD_IF_4BIT" },
                type = "enumerated",
                choices = mcu_32bit_choice,
                file = "include/cfg/arch/armpio.h"
            },
            --
            --  Selection of serial interface parameters
            --
            {
                macro = "LCD_CLK_PIO_ID",
                brief = "SPI Clock Port",
                description = "Port ID of the SPI clock line.\n",
                requires = { "LCD_SPI" },
                type = "enumerated",
                choices = at91_pio_id_choice,
                file = "include/cfg/arch/armpio.h"
            },
            {
                macro = "LCD_CLK_BIT",
                brief = "SPI Clock Bit",
                description = "Port bit of the SPI clock line. ",
                requires = { "LCD_SPI" },
                type = "enumerated",
                choices = mcu_32bit_choice,
                file = "include/cfg/arch/armpio.h"
            },
            {
                macro = "LCD_MOSI_PIO_ID",
                brief = "SPI Clock Port",
                description = "Port ID of the SPI MOSI line.\n",
                requires = { "LCD_SPI" },
                type = "enumerated",
                choices = at91_pio_id_choice,
                file = "include/cfg/arch/armpio.h"
            },
            {
                macro = "LCD_MOSI_BIT",
                brief = "SPI MOSI Bit",
                description = "Port bit of the SPI clock line. ",
                requires = { "LCD_SPI" },
                type = "enumerated",
                choices = mcu_32bit_choice,
                file = "include/cfg/arch/armpio.h"
            },
            
            --
            --  Selection of display control lines
            --
            {
                macro = "LCD_EN_PIO_ID",
                brief = "LCD Enable Port",
                description = "Port ID of the Enable line.\n"..
                              "For parallel chips this is the active high enable signal.\n"..
                              "For serial chips this is the active low chip select line.",
                type = "enumerated",
                choices = at91_pio_id_choice,
                file = "include/cfg/arch/armpio.h"
            },
            {
                macro = "LCD_EN_BIT",
                brief = "Enable Bit",
                description = "Port bit of the LCD enable line. "..
                              "This line must be exclusively reserved."..
                              "For parallel chips this is the active high enable signal.\n"..
                              "For serial chips this is the active low chip select line.",
                default = "4",
                type = "enumerated",
                choices = mcu_32bit_choice,
                file = "include/cfg/arch/armpio.h"
            },
            {
                macro = "LCD_RS_PIO_ID",
                brief = "LCD Register Select Port",
                description = "Port ID of the Register Select line.",
                type = "enumerated",
                choices = at91_pio_id_choice,
                flavor = "integer",
                file = "include/cfg/arch/armpio.h"
            },
            {
                macro = "LCD_RS_BIT",
                brief = "Register Select Bit",
                description = "Port bit of the LCD register select line. "..
                              "May be shared.",
                default = "7",
                type = "enumerated",
                choices = mcu_32bit_choice,
                file = "include/cfg/arch/armpio.h"
            },
    		--
    		--  Selection of optional display control lines
    		--
            {
                macro = "LCD_RW_PIO_ID",
                brief = "LCD Read/Write Select Port",
                description = "Optional port base of the Read/Write Select line."..
                              "Driver supports display confiurations with R/W tied to 0.\n"..
                              "For this, leave this entry blank.",
                type = "enumerated",
                choices = at91_pio_id_choice,
                flavor = "integer",
                file = "include/cfg/arch/armpio.h"
            },
            {
                macro = "LCD_RW_BIT",
                brief = "Read/Write Bit",
                description = "Optional port ID of the Read/Write Select line."..
                              "Driver supports display confiurations with R/W tied to 0.\n"..
                              "For this, leave this entry blank.",
                type = "enumerated",
                choices = mcu_32bit_choice,
                file = "include/cfg/arch/armpio.h"
            },
            {
                macro = "LCD_EN2_PIO_ID",
                brief = "LCD Enable Port 2",
                requires = { "LCD_GPIO" },
                description = "Optional port base of the 2nd Enable line. "..
                              "This line must be exclusively reserved.\n"..
                              "This is only used on large 4x40 character displays.",
                type = "enumerated",
                choices = at91_pio_id_choice,
                file = "include/cfg/arch/armpio.h"
            },
            {
                macro = "LCD_EN2_BIT",
                brief = "LCD Enable-2 Bit",
                requires = { "LCD_GPIO" },
                description = "Optional port bit of the 2nd LCD enable line. "..
                              "This is only used on large 4x40 character displays.\n"..
                              "This line must be exclusively reserved.",
                type = "enumerated",
                choices = mcu_32bit_choice,
                file = "include/cfg/arch/armpio.h"
            },
            {
                macro = "LCD_RST_PIO_ID",
                brief = "LCD Reset Port",
                description = "Optional port base of the LCD Reset line. "..
                              "This line must be exclusively reserved.\n"..
                              "Some LCD Drivers support an optional reset line.\n"..
                              "This is currently not supported by this driver!",
                type = "enumerated",
                choices = at91_pio_id_choice,
                file = "include/cfg/arch/armpio.h"
            },
            {
                macro = "LCD_RST_BIT",
                brief = "LCD Reset Bit",
                description = "Optional port bit of the 2nd LCD enable line. "..
                              "This line must be exclusively reserved.\n"..
                              "Some LCD Drivers support an optional reset line.\n"..
                              "This is currently not supported by this driver!",
                type = "enumerated",
                choices = mcu_32bit_choice,
                file = "include/cfg/arch/armpio.h"
            },
	    },
	},
    {
        name = "nutarch_gba_debug",
        brief = "LCD Debug Output (GBA)",
        requires = { "HW_LCD_GBA" },
        provides = { "DEV_UART", "DEV_FILE", "DEV_WRITE" },
        sources = { "arm/dev/gba/debug_gba.c" }
    },
    {
        name = "nutarch_arm_ax88796",
        brief = "AX88796 Driver (AT91)",
        description = "LAN driver for Asix 88796. AT91 only.",
        requires = { "HW_MCU_AT91R40008", "NUT_EVENT", "NUT_TIMER" },
        provides = { "NET_PHY" },
        sources = { "arm/dev/ax88796.c" },
    },
    {
        name = "nutarch_arm_dm9000e",
        brief = "DM9000E Driver (AT91)",
        description = "LAN driver for Davicom DM9000E. AT91 only.",
        requires = { "HW_EBI_AT91", "NUT_EVENT", "NUT_TIMER" },
        provides = { "NET_PHY" },
        sources = { "arm/dev/dm9000e.c" },
        options =
        {
            {
                macro = "NIC_BASE_ADDR",
                brief = "Controller Base Address",
                description = "The driver supports memory mapped controllers only, using "..
                              "the specified based address.\n\n"..
                              "The Ethernut 3 reference design uses 0x20000000.\n"..
                              "The ELEKTOR Internet Radio uses 0x30000000.\n",
                file = "include/cfg/memory.h"
            },
            {
                macro = "NIC_SIGNAL_IRQ",
                brief = "Ethernet Interrupt",
                description = "Ethernet controller interrupt.",
                type = "enumerated",
                choices = avr_irq_choice,
                file = "include/cfg/arch/armpio.h"
            }
        }
    },
    {
        name = "nutarch_arm_at91_emac",
        brief = "AT91 EMAC Driver",
        description = "LAN driver for AT91SAM7X and AT91SAM9260 and AT91SAM9G45.",
        requires = { "HW_EMAC_AT91", "NUT_EVENT", "NUT_TIMER" },
        provides = { "NET_PHY" },
        sources = { "arm/dev/atmel/at91_emac.c" },
        options =
        {
            {
                macro = "PHY_PWRDN_BIT",
                brief = "PHY Power Down Bit",
                description = "For the SAM7X default is 18.\n\n",
                provides = { "PHY_PWRDN_CONTROL" },
                flavor = "booldata",
                type = "enumerated",
                choices = mcu_32bit_choice,
                file = "include/cfg/arch/armpio.h"
            },
            {
                macro = "PHY_PWRDN_NEGPOL",
                brief = "PHY Power Down Polarity",
                description = "Select this, if the PHY is powered down by a low signal.",
                requires = { "PHY_PWRDN_CONTROL" },
                flavor = "boolean",
                file = "include/cfg/arch/armpio.h"
            },
            {
                macro = "NUT_THREAD_NICRXSTACK",
                brief = "Receiver Thread Stack",
                description = "Number of bytes to be allocated for the stack of the NIC receive thread.",
                flavor = "booldata",
                type = "integer",
                file = "include/cfg/dev.h"
            },
            {
                macro = "EMAC_RX_BUFFERS",
                brief = "Receive Buffers",
                description = "Number of 128 byte receive buffers.\n"..
                              "Increase to handle high traffic situations.\n"..
                              "Decrease to handle low memory situations.\n"..
                              "Default is 32.\n",
                flavor = "booldata",
                type = "integer",
                file = "include/cfg/dev.h"
            },
            {
                macro = "EMAC_TX_BUFSIZ",
                brief = "Transmit Buffer Size",
                description = "The driver will allocate two transmit buffers.\n"..
                              "Can be decreased in low memory situations. Be aware, "..
                              "that this may break your network application. Do not "..
                              "change this without exactly knowing the consequences.\n"..
                              "Default is 1536.\n",
                flavor = "booldata",
                type = "integer",
                file = "include/cfg/dev.h"
            },
            {
                macro = "EMAC_LINK_LOOPS",
                brief = "Link Polling Loops",
                description = "This simple implementation runs a dumb polling loop "..
                              "while waiting for the Ethernet link status.\n"..
                              "If you experience link problems, increasing this value "..
                              "may help.\n"..
                              "Default is 10000.\n",
                flavor = "booldata",
                type = "integer",
                file = "include/cfg/dev.h"
            },
        }
    },
    {
        name = "nutarch_arm_twi_at91",
        brief = "AT91 TWI",
        description = "AT91 hardware TWI support.",
        requires = { "HW_TWI_AT91" },
        provides = { "DEV_TWI" },
        sources = { "arm/dev/atmel/at91_twi.c" },
    },
    {
        name = "nutarch_arm_adc_at91",
        brief = "AT91 ADC",
        description = "ADC interface for AT91 (currently SAM7 only).",
        requires = { "HW_MCU_AT91SAM7X" },
        provides = { "DEV_ADC" },
        sources = { "arm/dev/atmel/at91_adc.c" },
    },
    {
        name = "nutarch_arm_spimmc_at91",
        brief = "AT91 SPI MMC Access",
        description = "Low level MMC interface for AT91.",
        requires = { "HW_SPI_AT91" },
        provides = { "DEV_MMCLL" },
        sources = { "arm/dev/atmel/spimmc_at91.c" },
    },
    {
        name = "nutarch_arm_mci_at91",
        brief = "AT91 MCI Device",
        description = "MCI based block device driver.",
        requires = { "HW_MCI_AT91" },
        provides = { "DEV_BLOCK" },
        sources = { "arm/dev/atmel/at91_mci.c" },
        options =
        {
            {
                macro = "MCI0_PIN_SHARING",
                brief = "Share Pins",
                description = "If enabled, the controller will release the peripheral pins when the MCI "..
		              "is not used and the pins may be used for other purposes.",
                flavor = "boolean",
                file = "include/cfg/arch/armpio.h"
            },
	},
    },

    --
    -- Special Functions
    --
    {
        name = "nutarch__arm_init",
        brief = "AT91 Initialization",
        description = "Contains spurious interrupt handler.",
        requires = { "HW_MCU_AT91" },
        sources = { "arm/dev/atmel/at91init.c" },
    },
    {
        name = "nutarch_arm_rstc",
        brief = "AT91 Reset Controller",
        description = "AT91 reset controller support.",
        requires = { "HW_MCU_AT91" },
        provides = { "DEV_MCU_RESET" },
        sources = { "arm/dev/atmel/at91_reset.c" },
    },
    {
        name = "nutarch__arm_gpio_at91",
        brief = "AT91 GPIO",
        description = "Generic port I/O API.",
        requires = { "HW_MCU_AT91" },
        sources = {
          "arm/dev/atmel/gpio_at91.c" ,
          "arm/dev/atmel/gpioa_at91.c",
          "arm/dev/atmel/gpiob_at91.c",
          "arm/dev/atmel/gpioc_at91.c"
        },
    },
    {
        name = "nutarch_arm_spibus_at91",
        brief = "AT91 SPI Bus Controller",
        description = "Supports up to two bus controllers.\n\n"..
                      "This early release had been tested on the AT91SAM7SE only."..
                      "Due to the SDRAM issue on this die, the PDC mode had been "..
                      "left unfinished.",
        requires = { "HW_SPI_AT91" },
        provides = { "SPIBUS_CONTROLLER" },
        sources =
        {
            "arm/dev/atmel/spibus_at91.c",
            "arm/dev/atmel/spibus0at91.c",
            "arm/dev/atmel/spibus1at91.c"
        },
        options =
        {
            {
                macro = "SPIBUS0_POLLING_MODE",
                brief = "Polling Mode (First Controller)",
                description = "If enabled, the controller will use polling mode. By default "..
                              "interrupt mode is used.",
                flavor = "boolean",
                file = "include/cfg/spi.h"
            },
            {
                macro = "SPIBUS0_DOUBLE_BUFFER",
                brief = "PDC Mode (First Controller)",
                description = "If enabled, the controller will use PDC mode.\n\n"..
                              "Under development. Works fine on SAM7X",
		provides = { "SPIBUS0_DOUBLE_BUFFER" },
                flavor = "boolean",
                file = "include/cfg/spi.h"
            },
            {
                macro = "SPIBUS0_DOUBLE_BUFFER_HEURISTIC",
                brief = "Heuristicaly use polling mode for short transfers instead of PDC",
                description = "If enabled, the controller will use the polling mode instead of PDC mode for short "..
                              "transfers (currently less that 4 byte), as setup of PDC might result in larger overhead. ".. 
                              "Depends on the selected SPI clock\n\n",
		requires = { "SPIBUS0_DOUBLE_BUFFER" },
                flavor = "boolean",
                file = "include/cfg/spi.h"
            },	
            {
                macro = "SPIBUS0_PIN_SHARING",
                brief = "Share Pins (First Controller)",
                description = "If enabled, the controller will release the peripheral pins when releasing "..
		              "the bus. This way the pins may be used for other purposes when SPI is inactive.",
                flavor = "boolean",
                file = "include/cfg/arch/armpio.h"
            },
            {
                macro = "SPI0_CS0_PIO_ID",
                brief = "CS0 Port ID (First Controller)",
                description = "ID of the port used for SPI bus 0 chip select 0.",
                type = "enumerated",
                choices = function() return GetAt91PioIds() end,
                file = "include/cfg/arch/armpio.h"
            },
            {
                macro = "SPI0_CS0_PIO_BIT",
                brief = "CS0 Port Bit (First Controller)",
                description = "Port bit used for SPI bus 0 chip select 0.",
                type = "enumerated",
                choices = mcu_32bit_choice,
                flavor = "integer",
                file = "include/cfg/arch/armpio.h"
            },
            {
                macro = "SPI0_CS1_PIO_ID",
                brief = "CS1 Port ID (First Controller)",
                description = "ID of the port used for SPI bus 0 chip select 1.",
                type = "enumerated",
                choices = function() return GetAt91PioIds() end,
                file = "include/cfg/arch/armpio.h"
            },
            {
                macro = "SPI0_CS1_PIO_BIT",
                brief = "CS1 Port Bit (First Controller)",
                description = "Port bit used for SPI bus 0 chip select 1.",
                type = "enumerated",
                choices = mcu_32bit_choice,
                flavor = "integer",
                file = "include/cfg/arch/armpio.h"
            },
            {
                macro = "SPI0_CS2_PIO_ID",
                brief = "CS2 Port ID (First Controller)",
                description = "ID of the port used for SPI bus 0 chip select 2.",
                type = "enumerated",
                choices = function() return GetAt91PioIds() end,
                file = "include/cfg/arch/armpio.h"
            },
            {
                macro = "SPI0_CS2_PIO_BIT",
                brief = "CS2 Port Bit (First Controller)",
                description = "Port bit used for SPI bus 0 chip select 2.",
                type = "enumerated",
                choices = mcu_32bit_choice,
                flavor = "integer",
                file = "include/cfg/arch/armpio.h"
            },
            {
                macro = "SPI0_CS3_PIO_ID",
                brief = "CS3 Port ID (First Controller)",
                description = "ID of the port used for SPI bus 0 chip select 3.",
                type = "enumerated",
                choices = function() return GetAt91PioIds() end,
                file = "include/cfg/arch/armpio.h"
            },
            {
                macro = "SPI0_CS3_PIO_BIT",
                brief = "CS3 Port Bit (First Controller)",
                description = "Port bit used for SPI bus 0 chip select 3.",
                type = "enumerated",
                choices = mcu_32bit_choice,
                flavor = "integer",
                file = "include/cfg/arch/armpio.h"
            },
            {
                macro = "SPIBUS1_POLLING_MODE",
                brief = "Polling Mode (Second Controller)",
                description = "If enabled, the controller will use polling mode. By default "..
                              "interrupt mode is used.",
                flavor = "boolean",
                file = "include/cfg/spi.h"
            },
            {
                macro = "SPIBUS1_DOUBLE_BUFFER",
                brief = "PDC Mode (Second Controller)",
                description = "If enabled, the controller will use PDC mode.\n\n"..
                              "Under development. Works fine on SAM7X",
		provides = { "SPIBUS1_DOUBLE_BUFFER" },
                flavor = "boolean",
                file = "include/cfg/spi.h"
            },
            {
                macro = "SPIBUS1_DOUBLE_BUFFER_HEURISTIC",
                brief = "Heuristicaly use polling mode for short transfers instead of PDC",
                description = "If enabled, the controller will use the polling mode instead of PDC mode for short "..
                              "transfers (currently less that 4 byte), as setup of PDC might result in larger overhead. ".. 
                              "Depends on the selected SPI clock\n\n",
		requires = { "SPIBUS1_DOUBLE_BUFFER" },
                flavor = "boolean",
                file = "include/cfg/spi.h"
            },	
            {
                macro = "SPI1_CS0_PIO_ID",
                brief = "CS0 Port ID (Second Controller)",
                description = "ID of the port used for SPI bus 1 chip select 0.",
                type = "enumerated",
                choices = function() return GetAt91PioIds() end,
                file = "include/cfg/arch/armpio.h"
            },
            {
                macro = "SPI1_CS0_PIO_BIT",
                brief = "CS0 Port Bit (Second Controller)",
                description = "Port bit used for SPI bus 1 chip select 0.",
                type = "enumerated",
                choices = mcu_32bit_choice,
                flavor = "integer",
                file = "include/cfg/arch/armpio.h"
            },
            {
                macro = "SPI1_CS1_PIO_ID",
                brief = "CS1 Port ID (Second Controller)",
                description = "ID of the port used for SPI bus 1 chip select 1.",
                type = "enumerated",
                choices = function() return GetAt91PioIds() end,
                file = "include/cfg/arch/armpio.h"
            },
            {
                macro = "SPI1_CS1_PIO_BIT",
                brief = "CS1 Port Bit (Second Controller)",
                description = "Port bit used for SPI bus 1 chip select 1.",
                type = "enumerated",
                choices = mcu_32bit_choice,
                flavor = "integer",
                file = "include/cfg/arch/armpio.h"
            },
            {
                macro = "SPI1_CS2_PIO_ID",
                brief = "CS2 Port ID (Second Controller)",
                description = "ID of the port used for SPI bus 1 chip select 2.",
                type = "enumerated",
                choices = function() return GetAt91PioIds() end,
                file = "include/cfg/arch/armpio.h"
            },
            {
                macro = "SPI1_CS2_PIO_BIT",
                brief = "CS2 Port Bit (Second Controller)",
                description = "Port bit used for SPI bus 1 chip select 2.",
                type = "enumerated",
                choices = mcu_32bit_choice,
                flavor = "integer",
                file = "include/cfg/arch/armpio.h"
            },
            {
                macro = "SPI1_CS3_PIO_ID",
                brief = "CS3 Port ID (Second Controller)",
                description = "ID of the port used for SPI bus 1 chip select 3.",
                type = "enumerated",
                choices = function() return GetAt91PioIds() end,
                file = "include/cfg/arch/armpio.h"
            },
            {
                macro = "SPI1_CS3_PIO_BIT",
                brief = "CS3 Port Bit (Second Controller)",
                description = "Port bit used for SPI bus 1 chip select 3.",
                type = "enumerated",
                choices = mcu_32bit_choice,
                flavor = "integer",
                file = "include/cfg/arch/armpio.h"
            },
        },
    },
    {
        name = "nutarch__arm_at91spi",
        brief = "AT91 SPI Support",
        description = "Preliminary SPI routines.",
        requires = { "HW_SPI_AT91", "HW_PDC_AT91" },
        sources = { "arm/dev/atmel/at91_spi.c" },
    },
    {
        name = "nutarch__arm_at91efc",
        brief = "AT91 Embedded Flash Controller",
        description = "Routines for reading and writing embedded flash memory.",
        requires = { "HW_EFC_AT91" },
        sources = { "arm/dev/atmel/at91_efc.c" },
    },
    {
        name = "nutarch_arm_tlv320",
        brief = "TLV320 DAC",
        requires = { "HW_EXT_CALYPSO" },
        provides = { "HW_AUDIO_DAC" },
        sources = { "arm/dev/tlv320dac.c" },
    },
}

