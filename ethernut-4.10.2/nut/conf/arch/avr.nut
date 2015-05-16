--
-- Copyright (C) 2004-2005 by egnite Software GmbH. All rights reserved.
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

-- AVR Architecture
--
-- $Log$
-- Revision 1.29  2009/02/06 15:37:40  haraldkipp
-- Added stack space multiplier and addend. Adjusted stack space.
--
-- Revision 1.28  2009/02/04 23:40:52  hwmaier
-- Added support for a receive buffer size configuration entry ATCAN_RX_BUF_SIZE.
--
-- Revision 1.27  2009/01/18 16:46:18  haraldkipp
-- Properly distinguish between PIO IDs and port numbers.
-- Call internal Lua functions when needed only.
--
-- Revision 1.26  2009/01/09 17:54:21  haraldkipp
-- Added SPI bus controller for AVR and AT91.
--
-- Revision 1.25  2008/09/02 14:29:01  haraldkipp
-- Disabled the old vs1001k driver.
--
-- Revision 1.24  2008/08/11 11:51:19  thiagocorrea
-- Preliminary Atmega2560 compile options, but not yet supported.
-- It builds, but doesn't seam to run properly at this time.
--
-- Revision 1.23  2008/07/19 02:54:52  thiagocorrea
-- 	* conf/arch/avr.nut: Fix LCD size configuration for AVR.
-- 	  Previously, LCD Bus config settings would be used instead
-- 	  of the actual LCD Driver config settings.
--
-- Revision 1.22  2008/06/16 13:02:50  haraldkipp
-- Temporarily removed wlandrv.c from build due to GCC 4.3 warnings.
--
-- Revision 1.21  2008/04/29 16:58:21  thiagocorrea
-- Simplified HD44780 code for AVR based on the ARM driver.
--
-- Revision 1.20  2008/04/29 02:28:34  thiagocorrea
-- Add configurable DTR pin to AVR USART driver.
--
-- Revision 1.19  2008/01/31 09:22:32  haraldkipp
-- Added first version of platform independent GPIO routines. Consider the
-- AVR version untested.
--
-- Revision 1.18  2007/10/05 23:47:50  olereinhardt
-- Added a missin colon :-)
--
-- Revision 1.17  2007/10/04 19:32:52  olereinhardt
-- SJA_BASE (base address for sja1000 driver) can now be set in configurator
--
-- Revision 1.16  2007/09/11 13:40:37  haraldkipp
-- Typo corrected.
--
-- Revision 1.15  2007/04/12 09:07:54  haraldkipp
-- Configurable SPI added.
--
-- Revision 1.14  2007/01/18 15:37:54  thiagocorrea
-- Fixed macro for enabling CTS on the second USART port from the nutconf program
--
-- Revision 1.13  2006/10/08 16:40:17  haraldkipp
-- Many thanks to Thiago Correa for adding LCD port configuration.
--
-- Revision 1.12  2006/10/05 17:16:04  haraldkipp
-- Added exclusivity attribute.
--
-- Revision 1.11  2006/05/25 09:27:42  haraldkipp
-- Watchdog API for AVR added.
--
-- Revision 1.10  2006/03/02 23:56:53  hwmaier
-- NUT_THREAD_NICRXSTACK entry added
--
-- Revision 1.9  2006/02/08 15:20:22  haraldkipp
-- ATmega2561 Support
--
-- Revision 1.8  2006/01/23 17:28:30  haraldkipp
-- HW_NVMEM is now provided by specific modules.
--
-- Revision 1.7  2005/11/24 11:26:33  haraldkipp
-- Added TLC16C550 driver.
--
-- Revision 1.6  2005/11/22 16:42:39  haraldkipp
-- ICCAVR fixed. Re-enabled AHDLC and old UART driver.
--
-- Revision 1.5  2005/11/20 14:43:11  haraldkipp
-- AHDLC driver and deprecated AVR UART driver temporarely removed from ICCAVR
-- build. Will look into this later.
--
-- Revision 1.4  2005/10/24 11:14:07  haraldkipp
-- NUT_CPU_FREQ option removed. This is handled in os.nut.
--
-- Revision 1.3  2005/10/04 05:51:49  hwmaier
-- Added CAN driver for AT90CAN128
--
-- Revision 1.2  2005/09/17 09:28:26  drsung
-- Added irqstack.c to sources for avr target.
--
-- Revision 1.1  2005/07/26 15:41:06  haraldkipp
-- All target dependent code is has been moved to a new library named
-- libnutarch. Each platform got its own script.
--
--

nutarch_avr =
{
    --
    -- Runtime Initialization
    --
    {
        name = "nutarch_avr_cstartup_icc",
        brief = "ICCAVR Startup",
        description = "There are five different ICCAVR startup files available.\n\n"..
                      "crtnut.s, if globals and static variables use less than 4kB.\n"..
                      "crtenut.s, same as above but including EEPROM emulation.\n"..
                      "crtnutram.s, if globals and static variables use more than 4kB.\n"..
                      "crtenutram.s, same as above but including EEPROM emulation.\n\n"..
                      "crtnutm256.s, for the ATmega2560 and ATmega2561.\n\n"..
                      "Ethernut 1.3 Rev-G boards require EEPROM emulation.",
        sources = {
            "avr/init/crtnut.s",
            "avr/init/crtnutram.s",
            "avr/init/crtenut.s",
            "avr/init/crtenutram.s",
            "avr/init/crtnutm256.s"
        },
        targets = {
            "avr/init/crtnut.o",
            "avr/init/crtnutram.o",
            "avr/init/crtenut.o",
            "avr/init/crtenutram.o",
            "avr/init/crtnutm256.o"
        },
        requires = { "TOOL_CC_AVR", "TOOL_ICC" },
    },

    --
    -- Board Initialization
    --
    {
        name = "nutarch_avr_bs",
        brief = "Board Support",
        sources = 
            function() 
                return { "avr/board/"..string.lower(c_macro_edit("PLATFORM"))..".c" }; 
            end,
        requires = { "HW_BOARD_SUPPORT" },
    },

    --
    -- Context Switching
    --
    {
        name = "nutarch_avr_context_gcc",
        brief = "Context Switching (AVRGCC)",
        provides = { "NUT_CONTEXT_SWITCH" },
        requires = { "HW_MCU_AVR", "TOOL_GCC" },
        sources = { "avr/os/context_gcc.c" },
    },
    {
        name = "nutarch_avr_context_icc",
        brief = "Context Switching (ICCAVR)",
        provides = { "NUT_CONTEXT_SWITCH" },
        requires = { "HW_MCU_AVR", "TOOL_ICC" },
        sources = { "avr/os/context_icc.c" },
    },

    --
    -- System Timer Hardware
    --
    {
        name = "nutarch_avr_ostimer",
        brief = "System Timer",
        requires = { "HW_MCU_AVR" },
        provides = { "NUT_OSTIMER_DEV" },
        sources = { "avr/dev/ostimer.c" },
    },
    {
        name = "nutarch_avr_wdt",
        brief = "Watchdog Timer (AVR)",
        requires = { "HW_MCU_AVR" },
        sources = { "avr/dev/wdt_avr.c" },
    },

    --
    -- AVR interrupt handling.
    --
    {
        name = "nutarch_avr_irq",
        brief = "Interrupt Handler",
        requires = { "HW_MCU_AVR" },
        provides = { "DEV_IRQ_AVR" },
        sources =
        {
            "avr/dev/ih_adc.c",
            "avr/dev/ih_analog_comp.c",
            "avr/dev/ih_canit.c",
            "avr/dev/ih_ee_ready.c",
            "avr/dev/ih_int0.c",
            "avr/dev/ih_int1.c",
            "avr/dev/ih_int2.c",
            "avr/dev/ih_int3.c",
            "avr/dev/ih_int4.c",
            "avr/dev/ih_int5.c",
            "avr/dev/ih_int6.c",
            "avr/dev/ih_int7.c",
            "avr/dev/ih_ovrit.c",
            "avr/dev/ih_spi_stc.c",
            "avr/dev/ih_spm_ready.c",
            "avr/dev/ih_timer0_comp.c",
            "avr/dev/ih_timer0_ovf.c",
            "avr/dev/ih_timer1_capt.c",
            "avr/dev/ih_timer1_compa.c",
            "avr/dev/ih_timer1_compb.c",
            "avr/dev/ih_timer1_compc.c",
            "avr/dev/ih_timer1_ovf.c",
            "avr/dev/ih_timer2_comp.c",
            "avr/dev/ih_timer2_ovf.c",
            "avr/dev/ih_timer3_capt.c",
            "avr/dev/ih_timer3_compa.c",
            "avr/dev/ih_timer3_compb.c",
            "avr/dev/ih_timer3_compc.c",
            "avr/dev/ih_timer3_ovf.c",
            "avr/dev/ih_twi.c",
            "avr/dev/ih_usart0_rx.c",
            "avr/dev/ih_usart0_tx.c",
            "avr/dev/ih_usart0_udre.c",
            "avr/dev/ih_usart1_rx.c",
            "avr/dev/ih_usart1_tx.c",
            "avr/dev/ih_usart1_udre.c",
            "avr/dev/irqstack.c"
        },
        options =
        {
            {
                macro = "IRQSTACK_SIZE",
                brief = "Separate Stack",
                description = "If this option is enabled, Nut/OS will use a separate "..
                              "stack of the specified size for interrupts.\n"..
                              "If this option is disabled, make sure to reserve "..
                              "additional stack space for each thread.\n\n"..
                              "Available for AVR-GCC only.",
                requires = { "HW_MCU_AVR", "TOOL_GCC" },
                flavor = "booldata",
                file = "include/cfg/dev.h"
            },
        }
    },

    --
    -- Device Drivers
    --
    {
        name = "nutarch_avr_ahdlc",
        brief = "AHDLC Protocol",
        description = "HDLC driver, required for PPP.",
        requires = { "HW_UART_AVR", "NUT_EVENT" },
        provides = { "PROTO_HDLC" },
        sources = { "avr/dev/ahdlc0.c", "avr/dev/ahdlc1.c", "avr/dev/ahdlcavr.c" },
        options =
        {
            {
                macro = "NUT_THREAD_AHDLCRXSTACK",
                brief = "Receiver Thread Stack",
                description = "Number of bytes to be allocated for the stack of the AHDLC receive thread.",
                default = "512",
                type = "integer",
                file = "include/cfg/ahdlc.h"
            }
        }
    },
    {
        name = "nutarch_avr_debug",
        brief = "UART Debug Output",
        description = "This simple UART output driver uses polling instead of "..
                      "interrupts and can be used within interrupt routines. It "..
                      "is mainly used for debugging and tracing.\n\n"..
                      "Call NutRegisterDevice(&devDebug0, 0, 0) for U(S)ART0 and "..
                      "NutRegisterDevice(&devDebug1, 0, 0) for U(S)ART1. "..
                      "Then you can use any of the stdio functions to open "..
                      "device uart0 or uart1 resp.",
        requires = { "HW_UART_AVR" },
        provides = { "DEV_UART", "DEV_FILE", "DEV_WRITE" },
        sources = { "avr/dev/debug0.c", "avr/dev/debug1.c" }
    },
    {
        name = "nutarch_avr_usart0",
        brief = "USART0 Driver",
        description = "Hardware specific USART driver. Implements hardware "..
                      "functions for the generic driver framework.",
        requires = {
                        "HW_MCU_AVR", "DEV_IRQ_AVR", "DEV_UART_GENERIC",
                        "NUT_EVENT", "CRT_HEAPMEM"
        },
        provides = { "DEV_UART_SPECIFIC" },
        sources = { "avr/dev/usart0avr.c" },
--        options =
--        {
--            {
--                macro = "UART0_RTS_BIT",
--                brief = "RTS Bit",
--                description = "Bit number of UART0 RTS handshake output. If enabled, "..
--                              "the driver provides RS 232 input hardware handshake.\n\n"..
--                              "Ethernut 2.x:\n"..
--                              "Short JP1 pins 4 and 6 to select bit 2 on PORTE.",
--                provides = { "UART0_RTS_BIT" },
--                flavor = "booldata",
--                type = "enumerated",
--                choices = avr_bit_choice,
--                file = "include/cfg/arch/avrpio.h"
--            },
--            {
--                macro = "UART0_RTS_AVRPORT",
--                brief = "RTS Port",
--                description = "Port register name of UART0 RTS handshake output.",
--                requires = { "UART0_RTS_BIT" },
--                type = "enumerated",
--                choices = avr_port_choice,
--                file = "include/cfg/arch/avrpio.h"
--            },
--            {
--                macro = "UART0_CTS_IRQ",
--                brief = "CTS Interrupt",
--                description = "Interrupt number of UART0 CTS handshake input. If enabled, "..
--                              "the driver provides RS 232 output hardware handshake.\n\n"..
--                              "Ethernut 2.x:\n"..
--                              "Short JP1 pins 7 and 8 to select INT7.",
--                flavor = "booldata",
--                type = "enumerated",
--                choices = avr_irq_choice,
--                file = "include/cfg/arch/avrpio.h"
--            },
--            {
--                macro = "UART0_DTR_BIT",
--                brief = "DTR Bit",
--                description = "Bit number of UART0 DTR handshake output. If enabled, "..
--                              "the driver provides RS 232 output hardware handshake.\n\n",
--                provides = { "UART0_DTR_BIT" },
--                flavor = "booldata",
--                type = "enumerated",
--                choices = avr_bit_choice,
--                file = "include/cfg/arch/avrpio.h"
--            },
--            {
--                macro = "UART0_DTR_AVRPORT",
--                brief = "DTR Port",
--                description = "Port register name of UART0 DTR handshake output.",
--                requires = { "UART0_DTR_BIT" },
--                type = "enumerated",
--                choices = avr_port_choice,
--                file = "include/cfg/arch/avrpio.h"
--            },
--            {
--                macro = "UART0_HDX_BIT",
--                brief = "Half Duplex Bit",
--                description = "Bit number of UART0 half duplex control output. If enabled, "..
--                              "the driver provides RS 485 half duplex mode.",
--                provides = { "UART0_HDX_BIT" },
--                flavor = "booldata",
--                type = "enumerated",
--                choices = avr_bit_choice,
--                file = "include/cfg/arch/avrpio.h"
--            },
--            {
--                macro = "UART0_HDX_AVRPORT",
--                brief = "Half Duplex Port",
--                description = "Port register name of UART0 half duplex control output.",
--                requires = { "UART0_HDX_BIT" },
--                type = "enumerated",
--                choices = avr_port_choice,
--                file = "include/cfg/arch/avrpio.h"
--            },
--        }
    },
    {
        name = "nutarch_avr_usart1",
        brief = "USART1 Driver",
        description = "Hardware specific USART driver. Implements hardware "..
                      "functions for the generic driver framework.",
        requires = {
                        "HW_MCU_AVR", "DEV_IRQ_AVR", "DEV_UART_GENERIC",
                        "NUT_EVENT", "CRT_HEAPMEM"
        },
        provides = { "DEV_UART_SPECIFIC" },
        sources = { "avr/dev/usart1avr.c" },
--        options =
--        {
--            {
--                macro = "UART1_RTS_BIT",
--                brief = "RTS Bit",
--                description = "Bit number of UART1 RTS handshake output. If enabled, "..
--                              "the driver provides RS 232 input hardware handshake.\n\n"..
--                              "Ethernut 2.x:\n"..
--                              "Short JP1 pins 4 and 6 to select bit 2 on PORTE.",
--                provides = { "UART1_RTS_BIT" },
--                flavor = "booldata",
--                type = "enumerated",
--                choices = avr_bit_choice,
--                file = "include/cfg/arch/avrpio.h"
--            },
--            {
--                macro = "UART1_RTS_AVRPORT",
--                brief = "RTS Port",
--                description = "Port register name of UART1 RTS handshake output.",
--                requires = { "UART1_RTS_BIT" },
--                type = "enumerated",
--                choices = avr_port_choice,
--                file = "include/cfg/arch/avrpio.h"
--            },
--            {
--                macro = "UART1_CTS_IRQ",
--                brief = "CTS Interrupt",
--                description = "Interrupt number of UART1 CTS handshake input. If enabled, "..
--                              "the driver provides RS 232 output hardware handshake.\n\n"..
--                              "Ethernut 2.x:\n"..
--                              "Short JP1 pins 7 and 8 to select INT7.",
--                flavor = "booldata",
--                type = "enumerated",
--                choices = avr_irq_choice,
--                file = "include/cfg/arch/avrpio.h"
--            },
--            {
--                macro = "UART1_DTR_BIT",
--                brief = "DTR Bit",
--                description = "Bit number of UART1 DTR handshake output. If enabled, "..
--                              "the driver provides RS 232 input hardware handshake.\n\n",
--                provides = { "UART1_DTR_BIT" },
--                flavor = "booldata",
--                type = "enumerated",
--                choices = avr_bit_choice,
--                file = "include/cfg/arch/avrpio.h"
--            },
--            {
--                macro = "UART1_DTR_AVRPORT",
--                brief = "DTR Port",
--                description = "Port register name of UART1 DTR handshake output.",
--                requires = { "UART1_DTR_BIT" },
--                type = "enumerated",
--                choices = avr_port_choice,
--                file = "include/cfg/arch/avrpio.h"
--            },
--            {
--                macro = "UART1_HDX_BIT",
--                brief = "Half Duplex Bit",
--                description = "Bit number of UART1 half duplex control output. If enabled, "..
--                              "the driver provides RS 485 half duplex mode.",
--                provides = { "UART1_HDX_BIT" },
--                flavor = "booldata",
--                type = "enumerated",
--                choices = avr_bit_choice,
--                file = "include/cfg/arch/avrpio.h"
--            },
--            {
--                macro = "UART1_HDX_AVRPORT",
--                brief = "Half Duplex Port",
--                description = "Port register name of UART1 half duplex control output.",
--                requires = { "UART1_HDX_BIT" },
--                type = "enumerated",
--                choices = avr_port_choice,
--                file = "include/cfg/arch/avrpio.h"
--            },
--        }
    },
    {
        name = "nutarch_avr_usart2",
        brief = "USART2 Driver",
        description = "Hardware specific USART driver. Implements hardware "..
                      "functions for the generic driver framework.",
        requires = {
                        "HW_MCU_AVR", "DEV_IRQ_AVR", "DEV_UART_GENERIC",
                        "NUT_EVENT", "CRT_HEAPMEM", "HW_AVR_HAVE_UART2"
        },
        provides = { "DEV_UART_SPECIFIC" },
        sources = { "avr/dev/usart2avr.c", "avr/dev/ih_usart2_rx.c", "avr/dev/ih_usart2_tx.c", "avr/dev/ih_usart2_udre.c" },
        options =
        {
            {
                macro = "UART2_RTS_BIT",
                brief = "RTS Bit",
                description = "Bit number of UART1 RTS handshake output. If enabled, "..
                              "the driver provides RS 232 input hardware handshake.\n\n",
                provides = { "UART2_RTS_BIT" },
                flavor = "booldata",
                type = "enumerated",
                choices = avr_bit_choice,
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "UART2_RTS_AVRPORT",
                brief = "RTS Port",
                description = "Port register name of UART2 RTS handshake output.",
                requires = { "UART2_RTS_BIT" },
                type = "enumerated",
                choices = avr_port_choice,
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "UART2_CTS_IRQ",
                brief = "CTS Interrupt",
                description = "Interrupt number of UART2 CTS handshake input. If enabled, "..
                              "the driver provides RS 232 output hardware handshake.\n\n",
                flavor = "booldata",
                type = "enumerated",
                choices = avr_irq_choice,
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "UART2_DTR_BIT",
                brief = "DTR Bit",
                description = "Bit number of UART2 DTR handshake output. If enabled, "..
                              "the driver provides RS 232 input hardware handshake.\n\n",
                provides = { "UART2_DTR_BIT" },
                flavor = "booldata",
                type = "enumerated",
                choices = avr_bit_choice,
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "UART2_DTR_AVRPORT",
                brief = "DTR Port",
                description = "Port register name of UART2 DTR handshake output.",
                requires = { "UART2_DTR_BIT" },
                type = "enumerated",
                choices = avr_port_choice,
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "UART2_HDX_BIT",
                brief = "Half Duplex Bit",
                description = "Bit number of UART2 half duplex control output. If enabled, "..
                              "the driver provides RS 485 half duplex mode.",
                provides = { "UART2_HDX_BIT" },
                flavor = "booldata",
                type = "enumerated",
                choices = avr_bit_choice,
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "UART2_HDX_AVRPORT",
                brief = "Half Duplex Port",
                description = "Port register name of UART2 half duplex control output.",
                requires = { "UART2_HDX_BIT" },
                type = "enumerated",
                choices = avr_port_choice,
                file = "include/cfg/arch/avrpio.h"
            },
        }
    },
    {
        name = "nutarch_avr_usart3",
        brief = "USART3 Driver",
        description = "Hardware specific USART driver. Implements hardware "..
                      "functions for the generic driver framework.",
        requires = {
                        "HW_MCU_AVR", "DEV_IRQ_AVR", "DEV_UART_GENERIC",
                        "NUT_EVENT", "CRT_HEAPMEM", "HW_AVR_HAVE_UART3"
        },
        provides = { "DEV_UART_SPECIFIC" },
        sources = { "avr/dev/usart3avr.c" },
        options =
        {
            {
                macro = "UART3_RTS_BIT",
                brief = "RTS Bit",
                description = "Bit number of UART3 RTS handshake output. If enabled, "..
                              "the driver provides RS 232 input hardware handshake.\n\n",
                provides = { "UART3_RTS_BIT" },
                flavor = "booldata",
                type = "enumerated",
                choices = avr_bit_choice,
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "UART3_RTS_AVRPORT",
                brief = "RTS Port",
                description = "Port register name of UART3 RTS handshake output.",
                requires = { "UART3_RTS_BIT" },
                type = "enumerated",
                choices = avr_port_choice,
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "UART3_CTS_IRQ",
                brief = "CTS Interrupt",
                description = "Interrupt number of UART3 CTS handshake input. If enabled, "..
                              "the driver provides RS 232 output hardware handshake.\n\n",
                flavor = "booldata",
                type = "enumerated",
                choices = avr_irq_choice,
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "UART3_DTR_BIT",
                brief = "DTR Bit",
                description = "Bit number of UART3 DTR handshake output. If enabled, "..
                              "the driver provides RS 232 input hardware handshake.\n\n",
                provides = { "UART3_DTR_BIT" },
                flavor = "booldata",
                type = "enumerated",
                choices = avr_bit_choice,
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "UART3_DTR_AVRPORT",
                brief = "DTR Port",
                description = "Port register name of UART3 DTR handshake output.",
                requires = { "UART3_DTR_BIT" },
                type = "enumerated",
                choices = avr_port_choice,
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "UART3_HDX_BIT",
                brief = "Half Duplex Bit",
                description = "Bit number of UART3 half duplex control output. If enabled, "..
                              "the driver provides RS 485 half duplex mode.",
                provides = { "UART3_HDX_BIT" },
                flavor = "booldata",
                type = "enumerated",
                choices = avr_bit_choice,
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "UART3_HDX_AVRPORT",
                brief = "Half Duplex Port",
                description = "Port register name of UART3 half duplex control output.",
                requires = { "UART3_HDX_BIT" },
                type = "enumerated",
                choices = avr_port_choice,
                file = "include/cfg/arch/avrpio.h"
            },
        }
    },
    {
        name = "nutarch_avr_ace",
        brief = "TL16C550 Driver",
        description = "TL16C550 UART driver for ATmega128.\n"..
                      "Initially contributed by William Basser and later "..
                      "enhanced by Przemyslaw Rudy. Only available for GCC.",
        sources = { "avr/dev/ace.c", "avr/dev/tlc16c550.c" },
        requires = { "HW_MCU_ATMEGA128", "DEV_IRQ_AVR", "NUT_EVENT", "CRT_HEAPMEM", "TOOL_GCC" },
        provides = { "DEV_FILE", "DEV_READ", "DEV_WRITE" },
    },
    {
        name = "nutarch_avr_uart",
        brief = "UART driver",
        description = "Interrupt driven, buffered UART driver.\n"..
                      "Deprecated, use the USART driver.",
        sources = { "avr/dev/uart0.c", "avr/dev/uart1.c", "avr/dev/uartavr.c" },
        requires = { "HW_MCU_AVR", "DEV_IRQ_AVR", "NUT_EVENT", "CRT_HEAPMEM" },
        provides = { "DEV_FILE", "DEV_READ", "DEV_WRITE" },
    },
    {
        name = "nutarch_avr_lanc111",
        brief = "LAN91C111 Driver",
        description = "LAN driver for SMSC LAN91C111. AVR only.",
        requires = { "HW_MCU_AVR", "NUT_EVENT", "NUT_TIMER" },
        provides = { "NET_PHY" },
        sources = { "avr/dev/lanc111.c" },
        options =
        {
            {
                macro = "LANC111_BASE_ADDR",
                brief = "Controller Base Address",
                description = "The driver supports memory mapped controllers only, using "..
                              "the specified based address.\n\n"..
                              "The Ethernut 2 reference design uses 0xC000.",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "LANC111_SIGNAL_IRQ",
                brief = "Ethernet Interrupt",
                description = "Ethernet controller interrupt, typically INT5.",
                default = "INT5",
                type = "enumerated",
                choices = avr_irq_choice,
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "LANC111_RESET_BIT",
                brief = "Ethernet Reset Bit",
                description = "Bit number of the Ethernet controller reset output.\n\n"..
                              "Should be disabled when the LAN91C111 hardware reset "..
                              "is not connected to a port pin.\n\n"..
                              "On Ethernut 2 boards the controller's reset pin "..
                              "is connected to the system reset. However, on early "..
                              "board revisions of Ethernut 2.0 no supervisory chip "..
                              "had been mounted, which causes problems with some "..
                              "power supplies. Mounting R30 or R31 connects the reset "..
                              "to bit 3 or bit 7 of PORTE.",
                provides = { "LANC111_RESET_BIT" },
                flavor = "booldata",
                type = "enumerated",
                choices = avr_bit_choice,
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "LANC111_RESET_AVRPORT",
                brief = "Ethernet Reset Port",
                description = "Port register name of the Ethernet controller reset output.",
                requires = { "LANC111_RESET_BIT" },
                type = "enumerated",
                choices = avr_port_choice,
                file = "include/cfg/arch/avrpio.h"
            },
        }
    },
    {
        name = "nutarch_avr_rtl8019as",
        brief = "RTL8019AS Driver",
        requires = { "HW_MCU_AVR", "NUT_TIMER" },
        provides = { "NET_PHY" },
        sources = { "avr/dev/nicrtl.c", "avr/dev/eth0rtl.c" },
        options =
        {
            {
                macro = "RTL_BASE_ADDR",
                brief = "Controller Base Address",
                description = "The driver supports memory mapped controllers only, using "..
                              "the specified based address.\n\n"..
                              "The Ethernut 1 reference design uses 0x8300.",
                default = "0x8300",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "RTL_SIGNAL_IRQ",
                brief = "Ethernet Interrupt",
                description = "Ethernet controller interrupt, typically INT5.",
                default = "INT5",
                type = "enumerated",
                choices = avr_irq_choice,
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "RTL_IRQ_RISING_EDGE",
                brief = "IRQ Rising Edge Trigger",
                description = "Configures rising edge trigger for RTL interrupt.\n\n"..
                              "Don't enable this for Ethernut boards! "..
                              "Used only on custom hardware designs without "..
                              "inverter gate for the interrupt.\n\n"..
                              "Valid only for ATMega 128 or AT90CAN128 parts, "..
                              "not supported by ATMega 103.",
                flavor = "boolean",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "RTL_RESET_BIT",
                brief = "Ethernet Reset Bit",
                description = "Bit number of the Ethernet controller reset output.\n\n"..
                              "Early Ethernut revisions use bit 4 on PORTE to hard "..
                              "reset the RTL8019AS. Later the controller's reset pin "..
                              "had been connected to the system reset.\n\n"..
                              "Should be disabled when the RTL8019AS hardware reset "..
                              "is not connected to a port pin.",
                provides = { "RTL_RESET_BIT" },
                flavor = "booldata",
                type = "enumerated",
                choices = avr_bit_choice,
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "RTL_RESET_AVRPORT",
                brief = "Ethernet Reset Port",
                description = "Port register name of the Ethernet controller reset output.",
                requires = { "RTL_RESET_BIT" },
                type = "enumerated",
                choices = avr_port_choice,
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "RTL_EESK_BIT",
                brief = "Ethernet EESK Bit",
                description = "Bit number of the Ethernet controller EEPROM clock input.\n\n"..
                              "This function, when enabled, uses three port pins to emulate "..
                              "a serial EEPROM for the RTL8019AS. The clock pin is driven "..
                              "by the Ethernet controller and will be monitored by the "..
                              "driver to shift the correct values out of the EEDO pin.\n\n"..
                              "Ethernut 1.3 Rev-G uses Bit 5 on Port C. On previous board "..
                              "revisions the EEDO pin is connected to high. As a result, "..
                              "the full duplex bit is permanently enabled. But the RTL8019AS "..
                              "does not provide link level handshake, which makes the "..
                              "peer believe, that it's half duplex only and any full duplex "..
                              "traffic from the RTL8019AS is considered a collision. This "..
                              "is not fatal, but noticeably reduces throughput.\n\n"..
                              "Should be disabled when the hardware doesn't support "..
                              "EEPROM Emulation or provides a real EEPROM.",
                provides = { "RTL_EESK_BIT" },
                flavor = "booldata",
                type = "enumerated",
                choices = avr_bit_choice,
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "RTL_EESK_AVRPORT",
                brief = "Ethernet EESK Port",
                description = "Port register name of the Ethernet controller EEPROM clock input.\n\n"..
                              "Ethernut 1.3 Rev-G uses Bit 5 on Port C.",
                requires = { "RTL_EESK_BIT" },
                type = "enumerated",
                choices = avr_port_choice,
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "RTL_EEDO_BIT",
                brief = "Ethernet EEDO Bit",
                description = "Bit number of the Ethernet controller EEPROM data output.\n\n"..
                              "Ethernut 1.3 Rev-G uses Bit 6 on Port C.",
                requires = { "RTL_EESK_BIT" },
                type = "enumerated",
                choices = avr_bit_choice,
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "RTL_EEDO_AVRPORT",
                brief = "Ethernet EEDO Port",
                description = "Port register name of the Ethernet controller EEPROM data output.\n\n"..
                              "Ethernut 1.3 Rev-G uses Bit 6 on Port C.",
                requires = { "RTL_EESK_BIT" },
                type = "enumerated",
                choices = avr_port_choice,
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "RTL_EEMU_BIT",
                brief = "Ethernet EEMU Bit",
                description = "Bit number of the Ethernet controller EEPROM emulator output.\n\n"..
                              "Ethernut 1.3 Rev-G uses Bit 7 on Port C to keep A15 high.\n\n"..
                              "Disable this option if the hardware doesn't use this signal.",
                requires = { "RTL_EESK_BIT" },
                provides = { "RTL_EEMU_BIT" },
                flavor = "booldata",
                type = "enumerated",
                choices = avr_bit_choice,
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "RTL_EEMU_AVRPORT",
                brief = "Ethernet EEMU Port",
                description = "Port register name of the Ethernet controller reset output.\n\n"..
                              "Ethernut 1.3 Rev-G uses Bit 7 on Port C.",
                requires = { "RTL_EEMU_BIT" },
                type = "enumerated",
                choices = avr_port_choice,
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "NUT_THREAD_NICRXSTACK",
                brief = "Receiver Thread Stack",
                description = "Number of bytes to be allocated for the stack of the NIC receive thread.",
                flavor = "booldata",
                type = "integer",
                file = "include/cfg/dev.h"
            }

        }
    },
    {
        name = "nutarch_avr_ide",
        brief = "ATA/IDE driver",
        description = "Mass storage driver.",
        requires = { "HW_MCU_AVR" },
        sources = { "avr/dev/ide.c" }
    },
    {
        name = "nutarch_avr_cs8900",
        brief = "CS8900 Driver",
        requires = { "HW_MCU_AVR", "NUT_TIMER" },
        sources = { "avr/dev/cs8900.c", "avr/dev/eth0cs.c" }
    },
    {
        name = "nutarch_avr_wlan",
        brief = "WLAN Driver",
        description = "Not well maintained. Disabled after compile problems with latest GCC.",
        requires = { "NOT_AVAILABLE", "NUT_EVENT", "HW_MCU_AVR" },
        provides = { "NET_PHY" },
        sources = { "avr/dev/wlan.c", "avr/dev/wlandrv.c" }
    },

    --
    -- Additional Hardware Support
    --
    {
        name = "nutarch_avr_gpio",
        brief = "AVR GPIO",
        requires = { "HW_MCU_AVR" },
        description = "Generic GPIO API.",
        sources = { "avr/dev/gpio_avr.c" }
    },
    {
        name = "nutarch_avr_adc",
        brief = "ADC Driver",
        description = "Driver for the ATmega analog to digital converter.\n\n"..
                      "Only available for AVR-GCC.\n\n"..
                      "Contributed by Ole Reinhardt from www.kernelconcepts.de",
        requires = { "HW_MCU_AVR", "TOOL_GCC" },
        sources = { "avr/dev/adc.c" },
        options =
        {
            {
                macro = "ADC_BUF_SIZE",
                brief = "Buffer Size",
                description = "Defaults to 16",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "ADC_INITIAL_CHANNEL",
                brief = "Initial Channel",
                description = "Set to ADC0..ADC7",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "ADC_INITIAL_REF",
                brief = "Initial Reference",
                description = "Set to AVCC (supply voltage), "..
                              "AREF (external reference) "..
                              "or INTERNAL_256 (2.56V)",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "ADC_INITIAL_MODE",
                brief = "Initial Mode",
                description = "Set to\n\nFREE_RUNNING:\n"..
                              "Free-running mode. Samples continuously taken "..
                              "every 13 cycles of ADC clock after "..
                              "ADC_start_conversion() is called.\n\n"..
                              "SINGLE_CONVERSION:\n"..
                              "Single-conversion mode. One sample taken every "..
                              "time ADC_start_conversion() is called.",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "ADC_INITIAL_PRESCALE",
                brief = "Initial Prescale",
                description = "Set to\n"..
                              "ADC_PRESCALE_DIV2 for CPU clk/2\n"..
                              "ADC_PRESCALE_DIV4 for CPU clk/4\n"..
                              "ADC_PRESCALE_DIV8 for CPU clk/8\n"..
                              "ADC_PRESCALE_DIV16 for CPU clk/16\n"..
                              "ADC_PRESCALE_DIV32 for CPU clk/32\n"..
                              "ADC_PRESCALE_DIV64 for CPU clk/64\n"..
                              "ADC_PRESCALE_DIV128 for CPU clk/128",
                file = "include/cfg/arch/avrpio.h"
            },
        }
    },
    {
        name = "nutarch_avr_ir",
        brief = "IR Driver",
        requires = { "HW_MCU_AVR", "NUT_EVENT" },
        description = "Infrared remote control for Sony protocol.",
        sources = { "avr/dev/ir.c", "avr/dev/irsony.c" }
    },
    {
        name = "nutarch_avr_hd44780",
        brief = "HD44780 Driver",
        requires = { "HW_MCU_AVR" },
        provides = { "DEV_FILE", "DEV_WRITE" },
        sources = { "avr/dev/hd44780.c" },
        options =
        {
            {
                macro = "LCD_ROWS",
                brief = "Rows",
                description = "The number of available display rows, either 1, 2 or 4.",
                default = "2",
                flavor = "integer",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "LCD_COLS",
                brief = "Columns",
                description = "The number of available display colums, either 8, 16, 20 or 40.",
                default = "16",
                flavor = "integer",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "KS0073_CONTROLLER",
                brief = "KS0073 Controller",
                flavor = "boolean",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "LCD_SHORT_DELAY",
                brief = "Short delay",
                description = "Number of milliseconds",
                flavor = "booldata",
                type = "integer",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "LCD_LONG_DELAY",
                brief = "Long delay",
                description = "Number of milliseconds",
                flavor = "booldata",
                type = "integer",
                file = "include/cfg/arch/avrpio.h"
             },
             {
                 macro = "LCD_DATA_AVRPORT",
                 brief = "Data port",
                 description = "Port of the databits to use",
                 type = "enumerated",
                 choices = avr_port_choice,
                 file = "include/cfg/arch/avrpio.h"
             },
             {
                 macro = "LCD_DATA_BITS",
                 brief = "Bits Data Port",
                 description = "Choose between 4 bits mode (upper or lower) and 8 bits mode",
                 type = "enumerated",
                 choices = hd44780_databits_choice,
                 file = "include/cfg/arch/avrpio.h"
             },
             {
                 macro = "LCD_ENABLE_AVRPORT",
                 brief = "Enable Port",
                 description = "Choose port used for the enable line",
                 type = "enumerated",
                 choices = avr_port_choice,
                 file = "include/cfg/arch/avrpio.h"
             },
             {
                 macro = "LCD_ENABLE_BIT",
                 brief = "Enable Bit",
                 description = "Choose bit used for the enable line",
                 type = "enumerated",
                 choices = avr_bit_choice,
                 file = "include/cfg/arch/avrpio.h"
             },
             {
                 macro = "LCD_REGSEL_AVRPORT",
                 brief = "RS Port",
                 description = "Choose port for the Register Select line",
                 type = "enumerated",
                 choices = avr_port_choice,
                 file = "include/cfg/arch/avrpio.h"
             },
             {
                 macro = "LCD_REGSEL_BIT",
                 brief = "RS Bit",
                 description = "Choose bit for the Register Select line",
                 type = "enumerated",
                 choices = avr_bit_choice,
                 file = "include/cfg/arch/avrpio.h"
             },
             {
                 macro = "LCD_RW_AVRPORT",
                 brief = "RW Port",
                 description = "Choose port for the Read Write line",
                 type = "enumerated",
                 choices = avr_port_choice,
                 file = "include/cfg/arch/avrpio.h"
             },
             {
                 macro = "LCD_RW_BIT",
                 brief = "RW Bit",
                 description = "Choose bit for the Read Write line",
                 type = "enumerated",
                 choices = avr_bit_choice,
                 file = "include/cfg/arch/avrpio.h"
            }

        }
    },
    {
        name = "nutarch_arv_hd44780_bus",
        brief = "HD44780 Bus Driver",
        description = "Alternate memory mapped LCD Driver.\n\n"..
                      "This is a terminal device driver for a memory "..
                      "mapped HD44780 compatible LCD. It is connected "..
                      "to the databus / adressbus. A chip select is "..
                      "generated from /RD, /WR, and the address decoder. "..
                      "It is connected to the LCD's enable signal. "..
                      "A0 is connected to the register select pin and A1 "..
                      "to the read/write signal. Therefore you'll read from "..
                      "an address with an offset of two.\n\n"..
                      "Only available for AVR-GCC.\n\n"..
                      "Contributed by Ole Reinhardt from www.kernelconcepts.de",
        requires = { "HW_MCU_AVR", "TOOL_GCC" },
        provides = { "DEV_FILE", "DEV_WRITE" },
        sources = { "avr/dev/hd44780_bus.c" },
        options =
        {
            {
                macro = "LCD_4x20",
                brief = "LCD 4x20",
                flavor = "boolean",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "LCD_4x16",
                brief = "LCD 4x16",
                flavor = "boolean",
                file = "include/cfg/arch/avrpio.h"
            },            {
                macro = "LCD_2x40",
                brief = "LCD 2x40",
                flavor = "boolean",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "LCD_2x20",
                brief = "LCD 2x20",
                flavor = "boolean",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "LCD_2x16",
                brief = "LCD 2x16",
                flavor = "boolean",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "LCD_2x8",
                brief = "LCD 2x8",
                flavor = "boolean",
                file = "include/cfg/arch/avrpio.h"
            },            {
                macro = "LCD_1x20",
                brief = "LCD 1x20",
                flavor = "boolean",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "LCD_1x16",
                brief = "LCD 1x16",
                flavor = "boolean",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "LCD_1x8",
                brief = "LCD 1x8",
                flavor = "boolean",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "KS0073_CONTROLLER",
                brief = "KS0073 Controller",
                flavor = "boolean",
                file = "include/cfg/arch/avrpio.h"
            }
        }
    },
    {
        name = "nutarch_avr_sja1000",
        brief = "SJA1000 CAN Driver",
        description = "Driver for SJA1000 CAN-Bus controller.\n\n"..
                      "The SJA1000 controller is connected to the memory "..
                      "bus. It's base address and interrupt is set by "..
                      "NutRegisterDevice().\n\n"..
                      "Only available for AVR-GCC.\n\n"..
                      "Contributed by Ole Reinhardt from www.embedded-it.de",
        provides = { "DEV_CAN_SPECIFIC"},
        requires = { "HW_MCU_AVR", "TOOL_GCC" },
        sources = { "avr/dev/sja1000.c" },
        options =
        {
            {
                macro = "SJA_BASE",
                brief = "Base address of SJA1000 controller",
                description = "Default is 0x8800.",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "SJA_SIGNAL_BIT",
                brief = "Interrupt Number",
                description = "Default is 7. Must correspond "..
                              "to SJA_SIGNAL.",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "SJA_SIGNAL",
                brief = "Interrupt Signal",
                description = "Default is sig_INTERRUPT7. Must correspond "..
                              "to SJA_SIGNAL_BIT.",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "SJA_EICR",
                brief = "Interrupt Control",
                description = "Default is EICRB. Must correspond "..
                              "to SJA_SIGNAL_BIT.",
                file = "include/cfg/arch/avrpio.h"
            }
        }
    },
    {
        name = "nutarch_avr_atcan",
        brief = "AT CAN Driver",
        description = "Driver for Atmel's internal Full CAN controller "..
                      "found in AT90 parts like the AT90CAN128 MCU.\n\n"..
                      "Requires a fixed MCU clock setting of either 12 or 16 MHz. "..
                      "Check setting in System Timer category.\n"..
                      "Currently only available for AVR-GCC.\n\n"..
                      "Contributed by Henrik Maier from www.proconx.com",
        requires = { "HW_CAN_AVR", "TOOL_GCC"},
        provides = { "DEV_CAN_SPECIFIC"},
        sources = { "avr/dev/atcan.c" },
        options =
        {
            {
                macro = "ATCAN_RX_BUF_SIZE",
                brief = "Receive buffers",
                description = "Number of CAN messages in receive buffer. Default is 64.",
                file = "include/cfg/arch/avrpio.h"
            }
        }
    },
    {
        name = "nutarch_avr_spibus_avr",
        brief = "AVR SPI Bus Controller",
        description = "SPI bus controllers.\n\n",
        provides = { "SPIBUS_CONTROLLER" },
        sources = 
        { 
            "avr/dev/spibus_avr.c",
            "avr/dev/spibus0avr.c"
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
                brief = "Double buffer IRQ (First Controller)",
                description = "If enabled, the controller will use double buffered interrupt mode.\n\n"..
                              "By default a single buffer is used. Ignored when polling mode is enabled.",
                flavor = "boolean",
                file = "include/cfg/spi.h"
            },
            {
                macro = "SPI0_CS0_PIO_ID",
                brief = "CS0 Port (First Controller)",
                description = "ID of the port used for SPI bus 0 chip select 0.",
                requires = { "HW_GPIO" },
                type = "enumerated",
                choices = function() return GetAvrPorts() end,
                flavor = "integer",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "SPI0_CS0_PIO_BIT",
                brief = "CS0 Port Bit (First Controller)",
                description = "Port bit used for SPI bus 0 chip select 0.",
                requires = { "HW_GPIO" },
                type = "enumerated",
                choices = function() return GetGpioBits() end,
                flavor = "integer",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "SPI0_CS1_PIO_ID",
                brief = "CS1 Port (First Controller)",
                description = "ID of the port used for SPI bus 0 chip select 1.",
                requires = { "HW_GPIO" },
                type = "enumerated",
                choices = function() return GetAvrPorts() end,
                flavor = "integer",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "SPI0_CS1_PIO_BIT",
                brief = "CS1 Port Bit (First Controller)",
                description = "Port bit used for SPI bus 0 chip select 1.",
                requires = { "HW_GPIO" },
                type = "enumerated",
                choices = function() return GetGpioBits() end,
                flavor = "integer",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "SPI0_CS2_PIO_ID",
                brief = "CS2 Port (First Controller)",
                description = "ID of the port used for SPI bus 0 chip select 2.",
                requires = { "HW_GPIO" },
                type = "enumerated",
                choices = function() return GetAvrPorts() end,
                flavor = "integer",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "SPI0_CS2_PIO_BIT",
                brief = "CS2 Port Bit (First Controller)",
                description = "Port bit used for SPI bus 0 chip select 2.",
                requires = { "HW_GPIO" },
                type = "enumerated",
                choices = function() return GetGpioBits() end,
                flavor = "integer",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "SPI0_CS3_PIO_ID",
                brief = "CS3 Port (First Controller)",
                description = "ID of the port used for SPI bus 0 chip select 3.",
                requires = { "HW_GPIO" },
                type = "enumerated",
                choices = function() return GetAvrPorts() end,
                flavor = "integer",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "SPI0_CS3_PIO_BIT",
                brief = "CS3 Port Bit (First Controller)",
                description = "Port bit used for SPI bus 0 chip select 3.",
                requires = { "HW_GPIO" },
                type = "enumerated",
                choices = function() return GetGpioBits() end,
                flavor = "integer",
                file = "include/cfg/arch/avrpio.h"
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
                brief = "Double buffer IRQ (Second Controller)",
                description = "If enabled, the controller will use double buffered interrupt mode.\n\n"..
                              "By default a single buffer is used. Ignored when polling mode is enabled.",
                flavor = "boolean",
                file = "include/cfg/spi.h"
            },
            {
                macro = "SPI1_CS0_PIO_ID",
                brief = "CS0 Port (Second Controller)",
                description = "ID of the port used for SPI bus 1 chip select 0.",
                requires = { "HW_GPIO" },
                type = "enumerated",
                choices = function() return GetAvrPorts() end,
                flavor = "integer",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "SPI1_CS0_PIO_BIT",
                brief = "CS0 Port Bit (Second Controller)",
                description = "Port bit used for SPI bus 1 chip select 0.",
                requires = { "HW_GPIO" },
                type = "enumerated",
                choices = function() return GetGpioBits() end,
                flavor = "integer",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "SPI1_CS1_PIO_ID",
                brief = "CS1 Port (Second Controller)",
                description = "ID of the port used for SPI bus 1 chip select 1.",
                requires = { "HW_GPIO" },
                type = "enumerated",
                choices = function() return GetAvrPorts() end,
                flavor = "integer",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "SPI1_CS1_PIO_BIT",
                brief = "CS1 Port Bit (Second Controller)",
                description = "Port bit used for SPI bus 1 chip select 1.",
                requires = { "HW_GPIO" },
                type = "enumerated",
                choices = function() return GetGpioBits() end,
                flavor = "integer",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "SPI1_CS2_PIO_ID",
                brief = "CS2 Port (Second Controller)",
                description = "ID of the port used for SPI bus 1 chip select 2.",
                requires = { "HW_GPIO" },
                type = "enumerated",
                choices = function() return GetAvrPorts() end,
                flavor = "integer",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "SPI1_CS2_PIO_BIT",
                brief = "CS2 Port Bit (Second Controller)",
                description = "Port bit used for SPI bus 1 chip select 2.",
                requires = { "HW_GPIO" },
                type = "enumerated",
                choices = function() return GetGpioBits() end,
                flavor = "integer",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "SPI1_CS3_PIO_ID",
                brief = "CS3 Port (Second Controller)",
                description = "ID of the port used for SPI bus 1 chip select 3.",
                requires = { "HW_GPIO" },
                type = "enumerated",
                choices = function() return GetAvrPorts() end,
                flavor = "integer",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "SPI1_CS3_PIO_BIT",
                brief = "CS3 Port Bit (Second Controller)",
                description = "Port bit used for SPI bus 1 chip select 3.",
                requires = { "HW_GPIO" },
                type = "enumerated",
                choices = function() return GetGpioBits() end,
                flavor = "integer",
                file = "include/cfg/arch/avrpio.h"
            },
        },
    },
    {
        name = "nutdev_sppif0",
        brief = "Polled SPI0 (AVR)",
        description = "Polling hardware SPI driver, AVR and master mode only.",
        requires = { "HW_MCU_AVR" },
        provides = { "DEV_SPI" },
        sources = { "avr/dev/sppif0.c" },
        options =
        {
            {
                macro = "SPPI0_CS0_AVRPORT",
                brief = "CS0 Port",
                description = "ID of the port used for SPI chip select 0.",
                type = "enumerated",
                choices = avr_port_choice,
                flavor = "integer",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "SPPI0_CS0_BIT",
                brief = "CS0 Port Bit",
                description = "Port bit used for SPI chip select 0.",
                type = "enumerated",
                choices = avr_bit_choice,
                flavor = "integer",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "SPPI0_CS1_AVRPORT",
                brief = "CS1 Port",
                description = "ID of the port used for SPI chip select 1.",
                type = "enumerated",
                choices = avr_port_choice,
                flavor = "integer",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "SPPI0_CS1_BIT",
                brief = "CS1 Port Bit",
                description = "Port bit used for SPI chip select 1.",
                type = "enumerated",
                choices = avr_bit_choice,
                flavor = "integer",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "SPPI0_CS2_AVRPORT",
                brief = "CS2 Port",
                description = "ID of the port used for SPI chip select 2.",
                type = "enumerated",
                choices = avr_port_choice,
                flavor = "integer",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "SPPI0_CS2_BIT",
                brief = "CS2 Port Bit",
                description = "Port bit used for SPI chip select 2.",
                type = "enumerated",
                choices = avr_bit_choice,
                flavor = "integer",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "SPPI0_CS3_AVRPORT",
                brief = "CS3 Port",
                description = "ID of the port used for SPI chip select 3.",
                type = "enumerated",
                choices = avr_port_choice,
                flavor = "integer",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "SPPI0_CS3_BIT",
                brief = "CS3 Port Bit",
                description = "Port bit used for SPI chip select 3.",
                type = "enumerated",
                choices = avr_bit_choice,
                flavor = "integer",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "SPPI0_RST0_AVRPORT",
                brief = "RESET0 Port",
                description = "ID of the port used for SPI chip reset 0.",
                type = "enumerated",
                choices = avr_port_choice,
                flavor = "integer",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "SPPI0_RST0_BIT",
                brief = "RESET0 Port Bit",
                description = "Port bit used for SPI chip reset 0.",
                type = "enumerated",
                choices = avr_bit_choice,
                flavor = "integer",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "SPPI0_RST1_AVRPORT",
                brief = "RESET1 Port",
                description = "ID of the port used for SPI chip reset 1.",
                type = "enumerated",
                choices = avr_port_choice,
                flavor = "integer",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "SPPI0_RST1_BIT",
                brief = "RESET1 Port Bit",
                description = "Port bit used for SPI chip reset 1.",
                type = "enumerated",
                choices = avr_bit_choice,
                flavor = "integer",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "SPPI0_RST2_AVRPORT",
                brief = "RESET2 Port",
                description = "ID of the port used for SPI chip reset 2.",
                type = "enumerated",
                choices = avr_port_choice,
                flavor = "integer",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "SPPI0_RST2_BIT",
                brief = "RESET2 Port Bit",
                description = "Port bit used for SPI chip reset 2.",
                type = "enumerated",
                choices = avr_bit_choice,
                flavor = "integer",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "SPPI0_RST3_AVRPORT",
                brief = "RESET3 Port",
                description = "ID of the port used for SPI chip reset 3.",
                type = "enumerated",
                choices = avr_port_choice,
                flavor = "integer",
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "SPPI0_RST3_BIT",
                brief = "RESET3 Port Bit",
                description = "Port bit used for SPI chip reset 3.",
                type = "enumerated",
                choices = avr_bit_choice,
                flavor = "integer",
                file = "include/cfg/arch/avrpio.h"
            },
        },
    },
    
    {
        name = "nutarch_avr_spidigio",
        brief = "Shift Register I/O",
        description = "Digital I/O using shift registers.\n"..
                      "This driver supports upto 32 digital inputs or outputs "..
                      "and occupies 5 port bits only:\n"..
                      "- Shift register data output\n"..
                      "- Shift register data input\n"..
                      "- Shift register clock output\n"..
                      "- Shift register input latch\n"..
                      "- Shift register output latch\n"..
                      "The hardware is very simple, using standard TTL 74165 "..
                      "shift registers for inputs and Allegro's UCN5841A for "..
                      "outputs. All shift are connected in series, with the "..
                      "first output register connected to the MCU data output "..
                      "and the last input register connected to the data input.",
        sources = { "avr/dev/spidigio.c" },
        requires = { "HW_MCU_AVR" },
        options =
        {
            {
                macro = "SPIDIGIO_SOUT_BIT",
                brief = "Shift Output Bit",
                description = "Bit number of the serial data output.\n"..
                              "Coconut uses bit 5 of PORTD.",
                type = "enumerated",
                choices = avr_bit_choice,
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "SPIDIGIO_SOUT_AVRPORT",
                brief = "Shift Output Port",
                description = "Port register name of serial data output.",
                type = "enumerated",
                choices = avr_port_choice,
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "SPIDIGIO_SIN_BIT",
                brief = "Shift Input Bit",
                description = "Bit number of the serial data input.\n"..
                              "Coconut uses bit 6 of PIND.",
                type = "enumerated",
                choices = avr_bit_choice,
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "SPIDIGIO_SIN_AVRPORT",
                brief = "Shift Input Port",
                description = "Port register name of serial data input.",
                type = "enumerated",
                choices = avr_port_choice,
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "SPIDIGIO_SCLK_BIT",
                brief = "Clock Output Bit",
                description = "Bit number of the serial clock output.\n"..
                              "Coconut uses bit 7 of PORTD.",
                type = "enumerated",
                choices = avr_bit_choice,
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "SPIDIGIO_SCLK_AVRPORT",
                brief = "Clock Output Port",
                description = "Port register name of serial clock output.",
                type = "enumerated",
                choices = avr_port_choice,
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "SPIDIGIO_LDI_BIT",
                brief = "Input Latch Bit",
                description = "Bit number of the input latch output.\n"..
                              "Coconut uses bit 7 of PORTB.",
                type = "enumerated",
                choices = avr_bit_choice,
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "SPIDIGIO_LDI_AVRPORT",
                brief = "Input Latch Port",
                description = "Port register name of the input latch output.",
                type = "enumerated",
                choices = avr_port_choice,
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "SPIDIGIO_LDO_BIT",
                brief = "Output Latch Bit",
                description = "Bit number of the output latch signal, which is "..
                              "connected to the strobe input of the UCN5841A\n"..
                              "Coconut uses bit 5 of PORTB.",
                type = "enumerated",
                choices = avr_bit_choice,
                file = "include/cfg/arch/avrpio.h"
            },
            {
                macro = "SPIDIGIO_LDO_AVRPORT",
                brief = "Output Latch Port",
                description = "Port register name of the output latch output.",
                type = "enumerated",
                choices = avr_port_choice,
                file = "include/cfg/arch/avrpio.h"
            },
        }
    },
    {
        name = "nutarch_avr_twif",
        brief = "TWI Driver",
        requires = { "HW_MCU_AVR", "NUT_EVENT" },
        provides = { "DEV_TWI" },
        sources = { "avr/dev/twif.c" }
    },
--    {
--        name = "nutarch_avr_vs1001k",
--        brief = "VS1001 Driver",
--        requires = { "NUT_SEGBUF", "HW_MCU_AVR" },
--        sources = { "avr/dev/vs1001k.c" }
--    },
    {
        name = "nutarch_avr_arthernet_cpld",
        brief = "Arthernet CPLD",
        requires = { "HW_MCU_AVR" },
        options =
        {
            {
                macro = "ARTHERCPLDSTART",
                brief = "CPLD Base Address",
                default = "0x1100",
                file = "include/cfg/memory.h"
            },
            {
                macro = "ARTHERCPLDSPI",
                brief = "CPLD SPI Base Address",
                default = "0x1200",
                file = "include/cfg/memory.h"
            }
        }
    },
    {
        name = "nutarch_avr_pcmcia",
        brief = "PCMCIA Driver",
        requires = { "HW_MCU_AVR" },
        sources = { "avr/dev/pcmcia.c" },
    },
    {
        name = "nutarch_avr_eeprom",
        brief = "EEPROM Access",
        requires = { "HW_MCU_AVR" },
        provides = { "DEV_NVMEM" },
        sources = { "avr/dev/eeprom.c" }
    },
}
