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

-- H8/300H Architecture
--
-- $Log$
-- Revision 1.1  2005/07/26 15:41:06  haraldkipp
-- All target dependent code is has been moved to a new library named
-- libnutarch. Each platform got its own script.
--
--

nutarch_h8300h =
{
    --
    -- System Timer Hardware
    --
    {
        name = "nutarch_h8300h_ostimer",
        brief = "System Timer",
        requires = { "HW_MCU_H8300" },
        provides = { "NUT_OSTIMER_DEV" },
        sources = { "h8300h/dev/ostimer.c" },
    },

    --
    -- Interrupt handling.
    --
    {
        name = "nutarch_h8300h_irq",
        brief = "Interrupt Handler",
        requires = { "HW_MCU_H8300" },
        sources = { "h8300h/dev/ihndlr.c", "h8300h/dev/s3c4510b_irqreg.c" },
    },
    
    --
    -- Device Drivers
    --
    {
        name = "nutarch_h8300h_rtl8019as",
        brief = "RTL8019AS Driver",
        requires = { "HW_MCU_H8300", "NUT_TIMER" },
        provides = { "NET_PHY" },
        sources = { "h8300h/dev/nicrtl.c" },
    },
    {
        name = "nutarch_uart_s3c4510b_dbg",
        brief = "S3C4510B Debug Output (S3C45)",
        requires = { "HW_UART_S3C45" },
        sources = { "h8300h/dev/uart_s3c4510b_dbg.c" }
    },
    {
        name = "nutarch_s3c4510b_console",
        brief = "UART driver (S3C45)",
        requires = { "HW_UART_S3C45" },
        provides = { "DEV_FILE", "DEV_READ", "DEV_WRITE" },
        sources = { "h8300h/dev/s3c4510b_console.c" }
    },
    {
        name = "nutarch_scih8dbg",
        brief = "SCI Debug Output (H8300)",
        requires = { "HW_MCU_H8300" },
        sources = { "h8300h/dev/scih8dbg.c" }
    },
    {
        name = "nutarch_scih8",
        brief = "UART driver (H8300)",
        requires = { "HW_UART_H8300" },
        provides = { "DEV_FILE", "DEV_READ", "DEV_WRITE" },
        sources = { "h8300h/dev/scih8.c", "h8300h/dev/scih8devs.c" }
    },
    {
        name = "nutarch_mweeprom",
        brief = "EEPROM Support (H8300)",
        requires = { "HW_MCU_H8300" },
        sources = { "h8300h/dev/mweeprom.c" }
    },    
}



