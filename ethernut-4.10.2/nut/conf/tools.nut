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

-- Tools
--
-- $Log$
-- Revision 1.30  2010/12/15 18:41:29  ve2yag
-- Added bootloader support script for AT91SAM7S family.
--
-- Revision 1.29  2009/03/07 00:04:29  olereinhardt
-- Added at91_bootloader_bootcrom ldscript
--
-- Revision 1.28  2008/09/23 07:33:53  haraldkipp
-- Added support for remaining SAM7 familiy members.
--
-- Revision 1.27  2008/09/18 09:48:06  haraldkipp
-- The old Marv_XXX do no longer work with ICCAVR 7.18B.
--
-- Revision 1.26  2008/08/11 11:51:20  thiagocorrea
-- Preliminary Atmega2560 compile options, but not yet supported.
-- It builds, but doesn't seam to run properly at this time.
--
-- Revision 1.25  2008/08/06 12:51:07  haraldkipp
-- Added support for Ethernut 5 (AT91SAM9XE reference design).
--
-- Revision 1.24  2008/07/14 13:04:28  haraldkipp
-- New Ethernut 3 link option with code in RAM and constant data in ROM.
--
-- Revision 1.23  2008/07/09 09:15:56  haraldkipp
-- EIR code running in RAM, contributed by Michael Fischer.
--
-- Revision 1.22  2008/06/28 07:49:33  haraldkipp
-- Added floating point support for stdio running on ARM.
--
-- Revision 1.21  2008/04/01 10:12:33  haraldkipp
-- Beautified target list.
--
-- Revision 1.20  2008/02/15 17:04:48  haraldkipp
-- Spport for AT91SAM7SE512 added.
--
-- Revision 1.19  2008/01/31 09:14:09  haraldkipp
-- Added ability to upload AT91 flash image via HTTP. Many thanks to
-- Matthias Wilde.
--
-- Revision 1.18  2007/10/04 20:15:26  olereinhardt
-- Support for SAM7S256 added
--
-- Revision 1.17  2007/09/11 13:39:45  haraldkipp
-- Configurable startup file for ICCAVR.
--
-- Revision 1.16  2007/04/12 09:20:00  haraldkipp
-- ATmega2561 no longer bound to ICCAVR.
--
-- Revision 1.15  2006/10/05 17:14:45  haraldkipp
-- Added exclusivity attribute.
--
-- Revision 1.14  2006/09/05 12:31:34  haraldkipp
-- Added missing linker script for SAM9260 applications running in external
-- RAM.
--
-- Revision 1.13  2006/08/01 07:34:16  haraldkipp
-- New linker script and new startup file support applications running in
-- flash memory.
--
-- Revision 1.12  2006/07/10 14:27:31  haraldkipp
-- Added ARM C++ support.
--
-- Revision 1.11  2006/07/10 08:48:03  haraldkipp
-- Distinguish between enhanced and extended AVR.
--
-- Revision 1.10  2006/03/02 19:54:48  haraldkipp
-- First attempt to compile with ICCARM. All compile errors fixed, but not
-- a finished port yet. Many things are missing.
--
-- Revision 1.9  2006/02/08 15:20:21  haraldkipp
-- ATmega2561 Support
--
-- Revision 1.8  2005/10/24 10:06:21  haraldkipp
-- New linker scripts added for AT91 apps running in RAM and ROM.
--
-- Revision 1.7  2005/06/05 16:57:00  haraldkipp
-- ICC target corrected
--
-- Revision 1.6  2005/04/05 18:04:17  haraldkipp
-- Support for ARM7 Wolf Board added.
--
-- Revision 1.5  2005/01/22 19:19:31  haraldkipp
-- Added C++ support contributed by Oliver Schulz (MPI).
--
-- Revision 1.4  2004/10/03 18:37:39  haraldkipp
-- GBA support
--
-- Revision 1.3  2004/09/07 19:12:57  haraldkipp
-- Linker script support added
--
-- Revision 1.2  2004/08/18 16:05:13  haraldkipp
-- Use consistent directory structure
--
-- Revision 1.1  2004/08/18 14:02:00  haraldkipp
-- First check-in
--
--
--

toolchain_names = {"ARM_GCC", "ARM_GCC_NOLIBC", "AVR_GCC", "AVR32_GCC", "LINUX_GCC", "ICCAVR", "ICCARM"}
gcc_output_format = {"ARMELF", "ARMEABI"}
nuttools =
{
    options =
    {
        {
            brief = "GCC for ARM",
            description = "GNU Compiler Collection for ARM including libc.",
            provides = { "TOOL_CC_ARM", "TOOL_GCC", "TOOL_CXX", "TOOL_ARMLIB" },
            macro = "ARM_GCC",
            flavor = "boolean",
            exclusivity = toolchain_names,
            file = "include/cfg/arch.h"
        },
        {
            brief = "GCC for ARM (no libc)",
            description = "GNU Compiler Collection for ARM excluding libc."..
                          "Nut/OS provides all required C standard functions.",
            provides = { "TOOL_CC_ARM", "TOOL_GCC", "TOOL_CXX", "TOOL_NOLIBC" },
            macro = "ARM_GCC_NOLIBC",
            flavor = "boolean",
            exclusivity = toolchain_names,
            file = "include/cfg/arch.h",
            makedefs = { "ADDLIBS = -lnutc" }
        },
        {
            brief = "GCC for AVR",
            description = "GNU Compiler Collection for ARM including avr-libc.",
            --  "TOOL_CXX" disabled due to problems with avr-libc eeprom.h.
            provides = { "TOOL_CC_AVR", "TOOL_GCC" },
            macro = "AVR_GCC",
            flavor = "boolean",
            exclusivity = toolchain_names,
            file = "include/cfg/arch.h",
            makedefs = { "MCU_ATMEGA2560=atmega2560", "MCU_ATMEGA2561=atmega2561", "MCU_ATMEGA128=atmega128", "MCU_ATMEGA103=atmega103" }
        },
        {
            brief = "GCC for AVR32",
            description = "GNU Compiler Collection for AVR32 including libc.",
            provides = { "TOOL_CC_AVR32", "TOOL_GCC", "TOOL_CXX" },
            macro = "AVR32_GCC",
            flavor = "boolean",
            exclusivity = toolchain_names,
            file = "include/cfg/arch.h",
        },
        {
            brief = "GCC for Linux",
            description = "Linux emulation.",
            provides = { "TOOL_CC_LINUX", "TOOL_GCC" },
            macro = "LINUX_GCC",
            flavor = "boolean",
            exclusivity = toolchain_names,
            file = "include/cfg/arch.h"
        },
        {
            brief = "ImageCraft for AVR",
            description = "www.imagecraft.com",
            provides = { "TOOL_CC_AVR", "TOOL_ICC" },
            macro = "ICCAVR",
            flavor = "boolean",
            exclusivity = toolchain_names,
            file = "include/cfg/arch.h",
            makedefs = { "MCU_ATMEGA2560=Extended", "MCU_ATMEGA2561=Extended", "MCU_ATMEGA128=Enhanced", "MCU_ATMEGA103=LongJump" }
        },
        {
            brief = "ImageCraft for ARM",
            description = "Not yet supported",
            provides = { "TOOL_CC_ARM", "TOOL_ICC" },
            macro = "ICCARM",
            flavor = "boolean",
            exclusivity = toolchain_names,
            file = "include/cfg/arch.h",
        }
    },
    {
        name = "nuttools_gccopt",
        brief = "GCC Settings",
        requires = { "TOOL_GCC" },
        options =
        {
            {
                macro = "LDSCRIPT",
                brief = "Linker Script",
                description = function() return GetLDScriptDescription(); end,
                requires = { "TOOL_GCC" },
                flavor = "booldata",
                type = "enumerated",
                choices = function() return GetLDScripts(); end,
                makedefs = function() return { "LDNAME", "LDSCRIPT=" .. GetLDScriptsPath() }; end,
            },
            {
                brief = "arm-elf",
                description = "Old binary format",
                requires = { "TOOL_CC_ARM" },
                macro = "ARMELF",
                flavor = "boolean",
                exclusivity = gcc_output_format,
                makedefs = { "TRGT = arm-elf-" }
           },
            {
                brief = "arm-none-eabi",
                description = "New binary format",
                requires = { "TOOL_CC_ARM" },
                macro = "ARMEABI",
                flavor = "boolean",
                exclusivity = gcc_output_format,
                makedefs = { "TRGT = arm-none-eabi-" }
            }
        }
    },
    {
        name = "nuttools_iccopt",
        brief = "ImageCraft AVR Settings",
        requires = { "TOOL_CC_AVR", "TOOL_ICC" },
        options =
        {
            {
                macro = "ICCAVR_STARTUP",
                brief = "Startup File",
                description = "Select one of the following:\n\n"..
                              "crtnut, if globals and static variables use less than 4kB.\n"..
                              "crtenut, same as above but including EEPROM emulation.\n"..
                              "crtnutram, if globals and static variables use more than 4kB.\n"..
                              "crtenutram, same as above but including EEPROM emulation.\n"..
                              "crtnutm256, for the ATmega2560 and ATmega2561.\n\n"..
                              "Ethernut 1 up to 1.3 Rev-F and all Ethernut 2 boards as well as most "..
                              "compatible boards require crtnutram.\n\n"..
                              "Ethernut 1.3 Rev-G and Rev-H boards need crtenutram. This startup "..
                              "should also work for all other boards with ATmega103/128 CPUs, but "..
                              "requires slightly more code than crtnutram.\n\n"..
                              "Use crtnutm256 for Ethernut and compatible boards with ATmega256 CPU.\n",
                type = "enumerated",
                choices = iccavr_startup_choice,
                makedefs = { "CRTNAME" }
            }
        }
    }

    --
    -- Intentionally no programmer or urom creator specified.
    -- This will be part of the application wizard.
    --
}

avr32_ld_description = {
                        uc3a0512_rom        = "AVR32UC3A0512, code running in FLASH",
                        uc3a0512_rom_extram = "AVR32UC3A0512, code running in FLASH, data in external SDRAM",
                        uc3a3256_rom        = "AT32UC3A3256, code running in FLASH",
                        uc3a3256_rom_extram = "AT32UC3A3256, code running in FLASH, data in external SDRAM",
                        uc3a0256_rom        = "AT32UC3A0256, code running in FLASH",
                        uc3b164_rom         = "AT32UC3B164, code running in FLASH",
}


avr32_ld_choice = { 
					" ", 
					"uc3a0512_rom",
					"uc3a0512_rom_extram",
					"uc3a3256_rom",
					"uc3a3256_rom_extram",
					"uc3a0256_rom",
					"uc3b164_rom"
}

arm_ld_description = {
    at91_boot                = "AT91R40008, code in ROM, copied to and running in RAM",
    at91_bootcrom            = "AT91R40008, code in ROM copied to RAM, but constant data remains in ROM.",
    at91_bootloader_bootcrom = "AT91R40008, code copied to RAM, but consts remain in ROM. Started by bootloader at address 0x10000",
    at91_ram                 = "AT91R40008, code loaded in RAM (deprecated, use at91x40_ram)",
    at91_rom                 = "AT91R40008, code running in FLASH (deprecated, use at91x40_rom)",
    at91_httprom             = "AT91R40008, code running in FLASH. Use this with boothttp.",
    at91sam7x128_rom         = "AT91SAM7X128, code running in FLASH",
    at91sam7x256_rom         = "AT91SAM7X256, code running in FLASH",
    at91sam7x256_bootrom     = "AT91SAM7X256, code running in FLASH, bootloader in FLASH (code entry at offset 0xC000)",
    at91sam7x512_rom         = "AT91SAM7X512, code running in FLASH",
    at91sam7x512_bootrom     = "AT91SAM7X512, code running in FLASH, bootloader in FLASH (code entry at offset 0xC000)",
    at91sam7s16_rom          = "AT91SAM7S16, code running in FLASH",
    at91sam7s32_rom          = "AT91SAM7S32, code running in FLASH",
    at91sam7s64_rom          = "AT91SAM7S64, code running in FLASH",
    at91sam7s64_bootrom      = "AT91SAM7S64, code running in FLASH, bootloader in FLASH",
    at91sam7s128_rom         = "AT91SAM7S128, code running in FLASH",
    at91sam7s256_rom         = "AT91SAM7S256, code running in FLASH",
    at91sam7s256_bootrom     = "AT91SAM7S256, code running in FLASH, bootloader in FLASH",
    at91sam7s512_rom         = "AT91SAM7S512, code running in FLASH",
    at91sam7se32_rom         = "AT91SAM7SE32, code running in FLASH, data in SDRAM",
    at91sam7se32_xram        = "AT91SAM7SE32, code loaded into external RAM",
    at91sam7se256_rom        = "AT91SAM7SE256, code running in FLASH, data in SDRAM",
    at91sam7se256_xram       = "AT91SAM7SE256, code loaded into external RAM",
    at91sam7se512_rom        = "AT91SAM7SE512, code running in FLASH, data in SDRAM",
    at91sam7se512_ram        = "AT91SAM7SE512, code in SDRAM (deprecated, use at91sam7se512_xram)",
    at91sam7se512_xram       = "AT91SAM7SE512, code loaded into external RAM",
    at91sam9260_ram          = "AT91SAM9260, code running in external RAM",
    at91sam9G45_ram          = "AT91SAM9G45, code running in external RAM",
    at91sam9xe512_ram        = "AT91SAM9XE512, code running in external RAM",
    at91x40_ram              = "AT91X40, code loaded in RAM by bootloader/debugger",
    at91x40_rom              = "AT91X40, code running in FLASH",
    ["s3c4510b-ram"]         = "Samsung's S3C4510B, code in RAM (unsupported)",
    eb40a_ram                = "Atmel's AT91EB40A, code in RAM at 0x100",
    gba_xport2               = "Nintendo's Gameboy Advance"
}

arm_ld_choice = {
                " ",
                "at91_boot",
                "at91_bootcrom",
                "at91_bootloader_bootcrom",
                "at91_ram",
                "at91_rom",
                "at91_httprom",
                "at91sam7s16_rom",
                "at91sam7s32_rom",
                "at91sam7s64_rom",
                "at91sam7s64_bootrom",
                "at91sam7s128_rom",
                "at91sam7s256_rom",
                "at91sam7s256_bootrom",
                "at91sam7s512_rom",
                "at91sam7se32_rom",
                "at91sam7se32_xram",
                "at91sam7se256_rom",
                "at91sam7se256_xram",
                "at91sam7se512_rom",
                "at91sam7se512_ram",
                "at91sam7se512_xram",
                "at91sam7x128_rom",
                "at91sam7x256_rom",
                "at91sam7x256_bootrom",
                "at91sam7x512_rom",
                "at91sam7x512_bootrom",
                "at91sam9260_ram",
                "at91sam9G45_ram", 
                "at91sam9xe512_ram",
                "at91x40_ram",
                "at91x40_rom",
                "s3c4510b-ram",
                "eb40a_ram",
                "eb40a_redboot_ram",
                "gbaxport2"
}

--
-- Retrieve platform specific ldscript path.
--
function GetLDScriptsPath()
    local basepath

    basepath = "$(top_srcdir)/arch/"
    if c_is_provided("TOOL_CC_AVR32") then
        return basepath .. "avr32/ldscripts/$(LDNAME).ld"
    end
    if c_is_provided("TOOL_CC_ARM") then
        return basepath .. "arm/ldscripts/$(LDNAME).ld"
    end
    return "Unknown Platform - Check GetLDScriptsPath in tools.nut"
end

--
-- Return the list of ldscripts
--
function GetLDScripts()
	if c_is_provided("TOOL_CC_AVR32") then
		return avr32_ld_choice
	end
	if c_is_provided("TOOL_CC_ARM") then
		return arm_ld_choice
	end
end

--
-- Return the ldscript description
--
function GetLDScriptDescription()
	if c_is_provided("TOOL_CC_AVR32") then
		return FormatLDScriptDescription(avr32_ld_description)
	end
	if c_is_provided("TOOL_CC_ARM") then
		return FormatLDScriptDescription(arm_ld_description)
	end
end

--
-- Returns pairs sorted by keys in alphabetic order
--
function pairsByKeys (t, f)
  local a = {}
  -- build temporary table of the keys
  for n in pairs (t) do
    table.insert (a, n)
  end
  table.sort (a, f)  -- sort using supplied function, if any
  local i = 0        -- iterator variable
  return function () -- iterator function
    i = i + 1
    return a[i], t[a[i]]
  end  -- iterator function
end -- pairsByKeys

--
-- Return the formated ldscript description
-- Note: It looks like it's impossible to properly
-- format based on tabs on both nutconf and qnutconf using
-- non-fixed length fonts. When we move definitively to qnutconf
-- this can be made to output html, then we will get it right.
--
function FormatLDScriptDescription( t )
	local result = ""
	local maxKeyLen = 0;
	for k,v in pairs(t) do
		if maxKeyLen < string.len(k) then
			maxKeyLen = string.len(k)
		end
	end

	maxTabs = math.ceil( maxKeyLen / 6 + 1 );
	for k,v in pairsByKeys(t) do
		tabs = maxTabs - math.ceil( string.len(k) / 6 );
		result = result .. k
		for i = 1, tabs do
			result = result .. "\t"
		end
		result = result .. v .. "\n"
	end
	return result
end

