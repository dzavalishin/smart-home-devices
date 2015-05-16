--
-- Copyright (C) 2001-2010 by egnite Software GmbH
--
-- All rights reserved.
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
-- THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
-- ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
-- LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
-- FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
-- COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
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


-- AVR32 Architecture
--
-- $Log: avr32.nut,v $

nutarch_avr32 =
{
  
    --
    -- Runtime Initialization
    --
    {
        name = "nutarch_avr32_cstartup",
        brief = "AVR32-GCC Startup",
        sources = { "avr32/init/crtavr32.S" },
        targets = { "avr32/init/crtavr32.o" },
        requires = { "TOOL_CC_AVR32", "TOOL_GCC" },
    },
    
    --
    -- Power Manager
    --
    {
        name = "nutarch_avr32_pm",
        brief = "Power Manager",
        requires = { "HW_MCU_AVR32" },
        sources = { "avr32/dev/pm.c" },
        options =
        {
            {
                macro = "OSC0_VAL",
                brief = "Osc0 Frequency",
                description = "The value of the Oscilator 0 (crystal) in Hz connected to the CPU.\n\n"..
                              "Default values are:\n"..
                              "EVK1100: 12000000\n"..
                              "EVK1101: 12000000\n"..
                              "EVK1104: 12000000\n"..
                              "EVK1105: 12000000\n",
                flavor = "booldata",
                file = "include/cfg/clock.h"
            },
            {
                macro = "PLL_MUL_VAL",
                brief = "PLL Multiply Factor",
                description = "Make sure to read the datasheet before modifying this value.\n\n"..
                              "These bitfields determine the ratio of the PLL output frequency "..
                              "(voltage controlled oscillator frequency fVCO) to the source "..
                              "oscillator frequency:\n\n"..
                              "fVCO = (PLLMUL+1)/(PLLDIV) x fOSC if PLLDIV > 0.\n".. 
                              "fVCO = 2*(PLLMUL+1) x fOSC if PLLDIV = 0\n"..
                              "Note that the MUL field cannot be equal to 0 or 1, or the behavior of the PLL will be undefined.\n"..
                              "Possible values of PLLMUL are 2...15\n",
                requires = { "HW_PLL_AVR32" },
                default = "9",
                flavor = "booldata",
                file = "include/cfg/clock.h"
            },
            {
                macro = "PLL_DIV_VAL",
                brief = "PLL Division Factor",
                description = "Make sure to read the datasheet before modifying this value.\n\n"..
                              "These bitfields determine the ratio of the PLL output frequency "..
                              "(voltage controlled oscillator frequency fVCO) to the source "..
                              "oscillator frequency:\n\n"..
                              "fVCO = (PLLMUL+1)/(PLLDIV) x fOSC if PLLDIV > 0.\n".. 
                              "fVCO = 2*(PLLMUL+1) x fOSC if PLLDIV = 0\n"..
                              "Note that the MUL field cannot be equal to 0 or 1, or the behavior of the PLL will be undefined.\n"..
                              "Possible values of PLLDIV are 0...15\n",
                requires = { "HW_PLL_AVR32" },
                default = "1",
                flavor = "booldata",
                file = "include/cfg/clock.h"
            },
            {
                macro = "PLL_FREQ_VAL",
                brief = "Select the VCO frequency range",
                description = "Make sure to read the datasheet before modifying this value.\n\n"..
                              "Select the VCO frequency range:\n\n"..
                              "0: 160MHz < fvco < 240MHz\n"..
                              "1: 80MHz < fvco < 180MHz",
                requires = { "HW_PLL_AVR32" },
                default = "1",
                flavor = "booldata",
                file = "include/cfg/clock.h"
            },
            {
                macro = "PLL_DIV2_VAL",
                brief = "Enable the extra output divider",
                description = "Make sure to read the datasheet before modifying this value.\n\n"..
                              "Enable the extra output divider:\n\n"..
                              "0: fPLL = fvco\n"..
                              "1: fPLL = fvco / 2",
                requires = { "HW_PLL_AVR32" },
                default = "1",
                flavor = "booldata",
                file = "include/cfg/clock.h"
            },
            {
                macro = "PLL_WBWD_VAL",
                brief = "Disable the Wide-Bandwidth mode",
                description = "Make sure to read the datasheet before modifying this value.\n\n"..
                              "Disable the Wide-Bandwidth mode (Wide-Bandwidth mode allows a faster startup time and out-of-lock time):\n\n"..
                              "0: Wide Bandwidth Mode enabled\n"..
                              "1: Wide Bandwidth Mode disabled",
                requires = { "HW_PLL_AVR32" },
                default = "0",
                flavor = "booldata",
                file = "include/cfg/clock.h"
            },
            {
                macro = "PLL_PBADIV_VAL",
                brief = "PBA Division and Clock Select",
                description = "Make sure to read the datasheet before modifying this value.\n\n"..
                              "PBA Division and Clock Select:\n\n"..
                              "PBADIV = 0: PBA clock equals main clock\n"..
                              "PBADIV = 1: PBA clock equals main clock divided by 2^(PBASEL+1)",
                requires = { "HW_PLL_AVR32" },
                default = "0",
                flavor = "booldata",
                file = "include/cfg/clock.h"
            },
            {
                macro = "PLL_PBASEL_VAL",
                brief = "PBA Division and Clock Select",
                description = "Make sure to read the datasheet before modifying this value.\n\n"..
                              "PBA Division and Clock Select:\n\n"..
                              "PBADIV = 0: PBA clock equals main clock\n"..
                              "PBADIV = 1: PBA clock equals main clock divided by 2^(PBASEL+1)",
                requires = { "HW_PLL_AVR32" },
                default = "0",
                flavor = "booldata",
                file = "include/cfg/clock.h"
            },
            {
                macro = "PLL_PBBDIV_VAL",
                brief = "PBB Division and Clock Select",
                description = "Make sure to read the datasheet before modifying this value.\n\n"..
                              "PBB Division and Clock Select:\n\n"..
                              "PBBDIV = 0: PBB clock equals main clock\n"..
                              "PBBDIV = 1: PBB clock equals main clock divided by 2^(PBBSEL+1)",
                requires = { "HW_PLL_AVR32" },
                default = "0",
                flavor = "booldata",
                file = "include/cfg/clock.h"
            },
            {
                macro = "PLL_PBBSEL_VAL",
                brief = "PBB Division and Clock Select",
                description = "Make sure to read the datasheet before modifying this value.\n\n"..
                              "PBB Division and Clock Select:\n\n"..
                              "PBBDIV = 0: PBB clock equals main clock\n"..
                              "PBBDIV = 1: PBB clock equals main clock divided by 2^(PBBSEL+1)",
                requires = { "HW_PLL_AVR32" },
                default = "0",
                flavor = "booldata",
                file = "include/cfg/clock.h"
            },
            {
                macro = "PLL_HSBDIV_VAL",
                brief = "HSB Division and Clock Select",
                description = "Make sure to read the datasheet before modifying this value.\n\n"..
                              "For the AT32UC3B, HSBDIV always equals CPUDIV, and HSBSEL always "..
                              "equals CPUSEL, as the HSB clock is always equal to the CPU clock.\n\n"..
                              "HSB Division and Clock Select:\n\n"..
                              "HSBDIV = 0: CPU clock equals main clock\n"..
                              "HSBDIV = 1: CPU clock equals main clock divided by 2^(HSBSEL+1)",
                requires = { "HW_PLL_AVR32" },
                default = "0",
                flavor = "booldata",
                file = "include/cfg/clock.h"
            },
            {
                macro = "PLL_HSBSEL_VAL",
                brief = "HSB Division and Clock Select",
                description = "Make sure to read the datasheet before modifying this value.\n\n"..
                              "For the AT32UC3B, HSBDIV always equals CPUDIV, and HSBSEL always "..
                              "equals CPUSEL, as the HSB clock is always equal to the CPU clock.\n\n"..
                              "HSB Division and Clock Select:\n\n"..
                              "HSBDIV = 0: CPU clock equals main clock\n"..
                              "HSBDIV = 1: CPU clock equals main clock divided by 2^(HSBSEL+1)",
                requires = { "HW_PLL_AVR32" },
                default = "0",
                flavor = "booldata",
                file = "include/cfg/clock.h"
            },
        },
    },
    
    --
    -- SDRAM Controler
    --
    {
        name = "nutarch_avr32_sdramc",
        brief = "SDRAM Controler",
        requires = { "HW_MCU_AVR32", "HW_SDRAMC" },
        options =
        {
            {
                macro = "NUTMEM_SDRAM_BASE",
                brief = "SDRAM Base Address",
                description = "Base SDRAM Address where SDRAM memory starts."..
                              "Disable this if your board doesn't provide external SDRAM",
                requires = { "HW_SDRAMC" },
                provides = { "HW_SDRAM" },
                default = "0xD0000000",
                flavor = "booldata",
                file = "include/cfg/memory.h"
            },
            {
                macro = "NUTMEM_SDRAM_COLBITS",
                brief = "SDRAM Column Address Bits",
                requires = { "HW_SDRAM" },
                default = "9",
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
                macro = "NUTMEM_SDRAM_BANKS",
                brief = "SDRAM Banks",
                requires = { "HW_SDRAM" },
                default = "2",
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
                macro = "NUTMEM_SDRAM_DBW",
                brief = "SDRAM Data Bus Width",
                description = "EBI Data bus width connected to SDRAM."..
                              "This is either 16 or 32",
                requires = { "HW_SDRAM" },
                default = "16",
                file = "include/cfg/memory.h"
            },
            {
                macro = "NUTMEM_SDRAM_TWR",
                brief = "SDRAM Write Recovery Cycles",
                requires = { "HW_SDRAM" },
                default = "14",
                file = "include/cfg/memory.h"
            },
            {
                macro = "NUTMEM_SDRAM_TRC",
                brief = "SDRAM Row Cycle Delay",
                requires = { "HW_SDRAM" },
                default = "60",
                file = "include/cfg/memory.h"
            },
            {
                macro = "NUTMEM_SDRAM_TRP",
                brief = "SDRAM Row Precharge Delay",
                requires = { "HW_SDRAM" },
                default = "15",
                file = "include/cfg/memory.h"
            },
            {
                macro = "NUTMEM_SDRAM_TRCD",
                brief = "SDRAM Row to Column Delay",
                requires = { "HW_SDRAM" },
                default = "15",
                file = "include/cfg/memory.h"
            },
            {
                macro = "NUTMEM_SDRAM_TRAS",
                brief = "SDRAM Active to Precharge Delay",
                requires = { "HW_SDRAM" },
                default = "37",
                file = "include/cfg/memory.h"
            },
            {
                macro = "NUTMEM_SDRAM_TXSR",
                brief = "SDRAM Self Refresh to Active Delay",
                requires = { "HW_SDRAM" },
                default = "67",
                file = "include/cfg/memory.h"
            },
            {
                macro = "NUTMEM_SDRAM_TR",
                brief = "SDRAM Max Refresh Delay",
                description = "Maximum amount of time in which the SDRAMC must issue a refresh command",
                requires = { "HW_SDRAM" },
                default = "7812",
                file = "include/cfg/memory.h"
            },
            {
                macro = "NUTMEM_SDRAM_TMRD",
                brief = "SDRAM Mode Register to Active or Refresh Delay",
                requires = { "HW_SDRAM" },
                default = "2",
                file = "include/cfg/memory.h"
            },
        },
    },

    --
    -- Board Initialization
    --
    {
        name = "nutarch_avr32_bs",
        brief = "Board Support",
        sources = 
            function() 
                return { "avr32/board/"..string.lower(c_macro_edit("PLATFORM"))..".c" }; 
            end,
        requires = { "HW_BOARD_SUPPORT" },
    },

    --
    -- Context Switching
    --
    {
        name = "nutarch_avr32_context",
        brief = "Context Switching",
        provides = { "NUT_CONTEXT_SWITCH" },
        requires = { "HW_MCU_AVR32", "TOOL_GCC" },
        sources = { "avr32/os/context.c" },
    },
    
    --
    -- System Timer Hardware
    --
    {
        name = "nutarch_ostimer_avr32",
        brief = "System Timer",
        requires = { "HW_TIMER_AVR32" },
        provides = { "NUT_OSTIMER_DEV" },
        sources = { "avr32/dev/ostimer.c" },
        options =
        {
            {
                macro = "AVR32_PLL_MAINCK",
                brief = "Main Clock",
                description = "Frequency of the external crystal. If this option is "..
                              "enabled and NUT_CPU_FREQ is not enabled, then the "..
                              "specified value will be used together with "..
                              "the PLL register settings to determine the CPU master clock. "..
                              "Otherwise you must specify NUT_CPU_FREQ.",
                requires = { "HW_PLL_AT91" },
                flavor = "booldata",
                file = "include/cfg/clock.h"
            },
        },
    },
    {
        name = "nutarch_wdt_avr32",
        brief = "Watchdog Timer",
        requires = { "HW_WDOG_AVR32" },
        sources = { "avr32/dev/wdt.c" },
    },

    --
    -- Interrupt handling.
    --
    {
        name = "nutarch_avr32_irq",
        brief = "Interrupt Handler",
        requires = { "HW_MCU_AVR32" },
        provides = { "DEV_IRQ_AVR32" },
        sources =
        {
            "avr32/dev/ih_uart0.c",
            "avr32/dev/ih_uart1.c",
            "avr32/dev/ih_uart2.c",
			"avr32/dev/ih_rtc.c",
            "avr32/dev/ih_macb.c",
            "avr32/dev/ih_irq0.c",
            "avr32/dev/ih_irq1.c",
            "avr32/dev/ih_irq2.c",
            "avr32/dev/ih_irq3.c",
            "avr32/dev/ih_irq4.c",
            "avr32/dev/ih_irq5.c",
            "avr32/dev/ih_irq6.c",
            "avr32/dev/ih_irq7.c",
            "avr32/dev/ih_nmi.c",
            "avr32/dev/ihndlr.c",
        },
    },

    --
    -- Device Drivers
    --
    {
        name = "nutarch_avr32_debug",
        brief = "UART Debug Output",
        description = "This simple UART output driver uses polling instead of "..
                      "interrupts and can be used within interrupt routines. It "..
                      "is mainly used for debugging and tracing.\n\n"..
                      "Call NutRegisterDevice(&devDebug0, 0, 0) for U(S)ART0 and "..
                      "NutRegisterDevice(&devDebug1, 0, 0) for U(S)ART1. "..
                      "Then you can use any of the stdio functions to open "..
                      "device uart0 or uart1 resp, up to the number of U(S)ARTs"..
                      "available",
        requires = { "HW_UART_AVR32" },
        provides = { "DEV_UART", "DEV_FILE", "DEV_WRITE" },
        sources = 
		{ 
			"avr32/dev/debug.c",
        },
    },
    {
        name = "nutarch_avr32_usart0",
        brief = "USART0 Driver",
        description = "Hardware specific USART driver. Implements hardware "..
                      "functions for the generic driver framework.",
        requires = { "HW_UART_AVR32", "DEV_IRQ_AVR32", "NUT_EVENT", "CRT_HEAPMEM" },
        provides = { "DEV_UART_SPECIFIC" },
        sources = { "avr32/dev/usart0.c" },
        options =
        {
			{
				macro = "UART0_ALT_PINSET",
				brief = "USART0 Alternative Pinset",
                type = "enumerated",
                choices = function() return GetAlternativePinsets() end,
                file = "include/cfg/uart.h"
				
			},
            {
                macro = "UART0_RXTX_ONLY",
                brief = "Receive/Transmit Only",
                description = "When selected, the driver will not support any handshake signals.",
                flavor = "boolean",
                exclusivity = { "UART0_RXTX_ONLY", "UART0_HARDWARE_HANDSHAKE", "UART0_MODEM_CONTROL" },
                file = "include/cfg/uart.h"
            },
            {
                macro = "UART0_HARDWARE_HANDSHAKE",
                brief = "Hardware Handshake",
                description = "When selected, the driver will support RTS/CTS hardware handshake. "..
                              "Make sure, that the related peripheral pins are available.",
                flavor = "boolean",
                exclusivity = { "UART0_RXTX_ONLY", "UART0_HARDWARE_HANDSHAKE", "UART0_MODEM_CONTROL" },
                requires = { "HW_UART0_RTSCTS" },
                file = "include/cfg/uart.h"
            },
            {
                macro = "UART0_MODEM_CONTROL",
                brief = "Full Modem Control",
                description = "When selected, the driver will support full modem control. "..
                              "Make sure, that all related peripheral pins are available.",
                flavor = "boolean",
                exclusivity = { "UART0_RXTX_ONLY", "UART0_HARDWARE_HANDSHAKE", "UART0_MODEM_CONTROL" },
                requires = { "HW_UART0_MODEM" },
                file = "include/cfg/uart.h"
            },
            {
                macro = "AVR32_UART0_RS485",
                brief = "USE HW RS485 on UART0",
				description = "If enabled, UART0 driver will enable hw RS485 on SAM7x."..
                      "The UART0 RTS pin is used for RS485 direction switching.",
                provides = { "AVR32_UART0_RS485" },
                flavor = "booldata",
            },

		},
    },
    {
        name = "nutarch_avr32_usart1",
        brief = "USART1 Driver",
        description = "Hardware specific USART driver. Implements hardware "..
                      "functions for the generic driver framework.",
        requires = { "HW_UART_AVR32", "DEV_IRQ_AVR32", "NUT_EVENT", "CRT_HEAPMEM" },
        provides = { "DEV_UART_SPECIFIC" },
        sources = { "avr32/dev/usart1.c" },
        options =
        {
			{
				macro = "UART1_ALT_PINSET",
				brief = "USART1 Alternative Pinset",
                type = "enumerated",
                choices = function() return GetAlternativePinsets() end,
                file = "include/cfg/uart.h"
				
			},
            {
                macro = "UART1_RXTX_ONLY",
                brief = "Receive/Transmit Only",
                description = "When selected, the driver will not support any handshake signals.",
                flavor = "boolean",
                exclusivity = { "UART1_RXTX_ONLY", "UART1_HARDWARE_HANDSHAKE", "UART1_MODEM_CONTROL" },
                file = "include/cfg/uart.h"
            },
            {
                macro = "UART1_HARDWARE_HANDSHAKE",
                brief = "Hardware Handshake",
                description = "When selected, the driver will support RTS/CTS hardware handshake. "..
                              "Make sure, that the related peripheral pins are available.",
                flavor = "boolean",
                exclusivity = { "UART1_RXTX_ONLY", "UART1_HARDWARE_HANDSHAKE", "UART1_MODEM_CONTROL" },
                requires = { "HW_UART1_RTSCTS" },
                file = "include/cfg/uart.h"
            },
            {
                macro = "UART1_MODEM_CONTROL",
                brief = "Full Modem Control",
                description = "When selected, the driver will support full modem control. "..
                              "Make sure, that all related peripheral pins are available.",
                flavor = "boolean",
                exclusivity = { "UART1_RXTX_ONLY", "UART1_HARDWARE_HANDSHAKE", "UART1_MODEM_CONTROL" },
                requires = { "HW_UART1_MODEM" },
                file = "include/cfg/uart.h"
            },
            {
                macro = "AVR32_UART1_RS485",
                brief = "USE HW RS485 on UART1",
				description = "If enabled, UART1 driver will enable hw RS485 on SAM7x."..
                      "The UART1 RTS pin is used for RS485 direction switching.",
                provides = { "AVR32_UART1_RS485" },
                flavor = "booldata",
            },

		},
    },
    {
        name = "nutarch_avr32_usart2",
        brief = "USART2 Driver",
        description = "Hardware specific USART driver. Implements hardware "..
                      "functions for the generic driver framework.",
        requires = { "HW_UART_AVR32", "HW_UART2_AVR32", "DEV_IRQ_AVR32", "NUT_EVENT", "CRT_HEAPMEM" },
        provides = { "DEV_UART_SPECIFIC" },
        sources = { "avr32/dev/usart2.c" },
        options =
        {
			{
				macro = "UART2_ALT_PINSET",
				brief = "USART2 Alternative Pinset",
                type = "enumerated",
                choices = function() return GetAlternativePinsets() end,
                file = "include/cfg/uart.h"
				
			},
            {
                macro = "UART2_RXTX_ONLY",
                brief = "Receive/Transmit Only",
                description = "When selected, the driver will not support any handshake signals.",
                flavor = "boolean",
                exclusivity = { "UART2_RXTX_ONLY", "UART2_HARDWARE_HANDSHAKE", "UART2_MODEM_CONTROL" },
                file = "include/cfg/uart.h"
            },
            {
                macro = "UART2_HARDWARE_HANDSHAKE",
                brief = "Hardware Handshake",
                description = "When selected, the driver will support RTS/CTS hardware handshake. "..
                              "Make sure, that the related peripheral pins are available.",
                flavor = "boolean",
                exclusivity = { "UART2_RXTX_ONLY", "UART2_HARDWARE_HANDSHAKE", "UART2_MODEM_CONTROL" },
                requires = { "HW_UART2_RTSCTS" },
                file = "include/cfg/uart.h"
            },
            {
                macro = "UART2_MODEM_CONTROL",
                brief = "Full Modem Control",
                description = "When selected, the driver will support full modem control. "..
                              "Make sure, that all related peripheral pins are available.",
                flavor = "boolean",
                exclusivity = { "UART2_RXTX_ONLY", "UART2_HARDWARE_HANDSHAKE", "UART2_MODEM_CONTROL" },
                requires = { "HW_UART2_MODEM" },
                file = "include/cfg/uart.h"
            },
            {
                macro = "AVR32_UART2_RS485",
                brief = "USE HW RS485 on UART1",
				description = "If enabled, UART1 driver will enable hw RS485 on SAM7x."..
                      "The UART1 RTS pin is used for RS485 direction switching.",
                provides = { "AVR32_UART2_RS485" },
                flavor = "booldata",
            },

		},
    },
    {
        name = "nutarch_avr32_macb",
        brief = "MACB Driver",
        description = "LAN driver for AVR32.",
        requires = { "HW_MACB_AVR32", "NUT_EVENT", "NUT_TIMER" },
        provides = { "NET_PHY" },
        sources = { "avr32/dev/macb.c" },
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
                file = "include/cfg/arch/avr32pio.h"
            },
            {
                macro = "PHY_PWRDN_NEGPOL",
                brief = "PHY Power Down Polarity",
                description = "Select this, if the PHY is powered down by a low signal.",
                requires = { "PHY_PWRDN_CONTROL" },
                flavor = "boolean",
                file = "include/cfg/arch/avr32pio.h"
            },
            {
                macro = "NUT_THREAD_NICRXSTACK",
                brief = "Receiver Thread Stack",
                description = "Number of bytes to be allocated for the stack of the NIC receive thread.",
                default = "768",
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
        }
    },

    --
    -- Special Functions
    --
    {
        name = "nutarch_avr32_gpio",
        brief = "GPIO Controller",
        description = "Generic port I/O API.",
        requires = { "HW_MCU_AVR32" },
        sources = { 
          "avr32/dev/gpio_nutos.c",
          "avr32/dev/gpio.c",
        },
    },
    {
        name = "nutarch_avr32_rstc",
        brief = "Reset Controller",
        description = "reset controller support.",
        requires = { "HW_MCU_AVR32" },
        provides = { "DEV_MCU_RESET" },
        sources = { "avr32/dev/reset.c" },
    },
    {
        name = "nutarch_avr32_spibus0",
        brief = "SPI0 Bus Controller",
        description = "Hardware specific SPI driver. Implements hardware "..
                      "functions for the SPI0 bus.",
        requires = { "HW_SPI_AVR32_0" },
        provides = { "SPIBUS_CONTROLLER" },
        sources = 
        { 
            "avr32/dev/spibus.c",
            "avr32/dev/spibus0.c",
            "avr32/dev/ih_spi0.c",
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
                              "Under development.",
                requires = { "NOT_AVAILABLE" },
                flavor = "boolean",
                file = "include/cfg/spi.h"
            },
            {
                macro = "SPI0_CS0_PIO_ID",
                brief = "CS0 Port ID (First Controller)",
                description = "ID of the port used for SPI bus 0 chip select 0.",
                type = "enumerated",
                choices = function() return GetAvr32PioIds() end,
                file = "include/cfg/spi.h"
            },
            {
                macro = "SPI0_CS0_PIO_BIT",
                brief = "CS0 Port Bit (First Controller)",
                description = "Port bit used for SPI bus 0 chip select 0.",
                type = "enumerated",
                choices = mcu_32bit_choice,
                flavor = "integer",
                file = "include/cfg/spi.h"
            },
            {
                macro = "SPI0_CS1_PIO_ID",
                brief = "CS1 Port ID (First Controller)",
                description = "ID of the port used for SPI bus 0 chip select 1.",
                type = "enumerated",
                choices = function() return GetAvr32PioIds() end,
                file = "include/cfg/spi.h"
            },
            {
                macro = "SPI0_CS1_PIO_BIT",
                brief = "CS1 Port Bit (First Controller)",
                description = "Port bit used for SPI bus 0 chip select 1.",
                type = "enumerated",
                choices = mcu_32bit_choice,
                flavor = "integer",
                file = "include/cfg/spi.h"
            },
            {
                macro = "SPI0_CS2_PIO_ID",
                brief = "CS2 Port ID (First Controller)",
                description = "ID of the port used for SPI bus 0 chip select 2.",
                type = "enumerated",
                choices = function() return GetAvr32PioIds() end,
                file = "include/cfg/spi.h"
            },
            {
                macro = "SPI0_CS2_PIO_BIT",
                brief = "CS2 Port Bit (First Controller)",
                description = "Port bit used for SPI bus 0 chip select 2.",
                type = "enumerated",
                choices = mcu_32bit_choice,
                flavor = "integer",
                file = "include/cfg/spi.h"
            },
            {
                macro = "SPI0_CS3_PIO_ID",
                brief = "CS3 Port ID (First Controller)",
                description = "ID of the port used for SPI bus 0 chip select 3.",
                type = "enumerated",
                choices = function() return GetAvr32PioIds() end,
                file = "include/cfg/spi.h"
            },
            {
                macro = "SPI0_CS3_PIO_BIT",
                brief = "CS3 Port Bit (First Controller)",
                description = "Port bit used for SPI bus 0 chip select 3.",
                type = "enumerated",
                choices = mcu_32bit_choice,
                flavor = "integer",
                file = "include/cfg/spi.h"
            },
        },
    },
    {
        name = "nutarch_avr32_spibus1",
        brief = "SPI1 Bus Controller",
        description = "Hardware specific SPI driver. Implements hardware "..
                      "functions for the SPI1 bus.",
        requires = { "HW_SPI_AVR32_1" },
        provides = { "SPIBUS_CONTROLLER" },
        sources = 
        { 
            "avr32/dev/spibus1.c",
            "avr32/dev/ih_spi1.c",
        },
        options =
        {
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
                              "Under development.",
                requires = { "NOT_AVAILABLE" },
                flavor = "boolean",
                file = "include/cfg/spi.h"
            },
            {
                macro = "SPI1_CS0_PIO_ID",
                brief = "CS0 Port ID (Second Controller)",
                description = "ID of the port used for SPI bus 1 chip select 0.",
                type = "enumerated",
                choices = function() return GetAvr32PioIds() end,
                file = "include/cfg/spi.h"
            },
            {
                macro = "SPI1_CS0_PIO_BIT",
                brief = "CS0 Port Bit (Second Controller)",
                description = "Port bit used for SPI bus 1 chip select 0.",
                type = "enumerated",
                choices = mcu_32bit_choice,
                flavor = "integer",
                file = "include/cfg/spi.h"
            },
            {
                macro = "SPI1_CS1_PIO_ID",
                brief = "CS1 Port ID (Second Controller)",
                description = "ID of the port used for SPI bus 1 chip select 1.",
                type = "enumerated",
                choices = function() return GetAvr32PioIds() end,
                file = "include/cfg/spi.h"
            },
            {
                macro = "SPI1_CS1_PIO_BIT",
                brief = "CS1 Port Bit (Second Controller)",
                description = "Port bit used for SPI bus 1 chip select 1.",
                type = "enumerated",
                choices = mcu_32bit_choice,
                flavor = "integer",
                file = "include/cfg/spi.h"
            },
            {
                macro = "SPI1_CS2_PIO_ID",
                brief = "CS2 Port ID (Second Controller)",
                description = "ID of the port used for SPI bus 1 chip select 2.",
                type = "enumerated",
                choices = function() return GetAvr32PioIds() end,
                file = "include/cfg/spi.h"
            },
            {
                macro = "SPI1_CS2_PIO_BIT",
                brief = "CS2 Port Bit (Second Controller)",
                description = "Port bit used for SPI bus 1 chip select 2.",
                type = "enumerated",
                choices = mcu_32bit_choice,
                flavor = "integer",
                file = "include/cfg/spi.h"
            },
            {
                macro = "SPI1_CS3_PIO_ID",
                brief = "CS3 Port ID (Second Controller)",
                description = "ID of the port used for SPI bus 1 chip select 3.",
                type = "enumerated",
                choices = function() return GetAvr32PioIds() end,
                file = "include/cfg/spi.h"
            },
            {
                macro = "SPI1_CS3_PIO_BIT",
                brief = "CS3 Port Bit (Second Controller)",
                description = "Port bit used for SPI bus 1 chip select 3.",
                type = "enumerated",
                choices = mcu_32bit_choice,
                flavor = "integer",
                file = "include/cfg/spi.h"
            },
        },
    },
    {
        name = "nutarch_avr32_flashc",
        brief = "Flash Controller",
        description = "Routines for reading and writing embedded flash memory.",
        requires = { "HW_EFC_AVR32" },
        sources = { "avr32/dev/flashc.c" },
    },

}

