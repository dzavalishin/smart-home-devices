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

-- Operating system functions
--
-- $Log$
-- Revision 1.22  2009/02/13 14:43:53  haraldkipp
-- Heap debug options and split memory flag added.
-- Banked memory options moved to the right module.
-- NUTPANIC added.
--
-- Revision 1.21  2009/02/06 15:52:14  haraldkipp
-- Removed stack size defaults.
--
-- Revision 1.20  2009/01/19 18:55:49  haraldkipp
-- Default fatal error handler added.
--
-- Revision 1.19  2009/01/16 17:03:02  haraldkipp
-- Configurable host name length. The *nix conditional is
-- no longer required as this will be handled in the nvmem
-- routines. NutLoadConfig will now set the virgin host
-- name, if no valid configuration is available. Cookie
-- and virgin host name are configurable too, but disabled
-- in the Configurator until we fixed the string value
-- problem. You may use UserConf.mk instead.
--
-- Revision 1.18  2008/07/08 08:25:04  haraldkipp
-- NutDelay is no more architecture specific.
-- Number of loops per millisecond is configurable or will be automatically
-- determined.
-- A new function NutMicroDelay provides shorter delays.
--
-- Revision 1.17  2008/07/07 11:04:27  haraldkipp
-- Configurable ways of handling critical sections for ARM targets.
--
-- Revision 1.16  2008/04/21 22:29:01  olereinhardt
-- Added configuration options for condition variables
--
-- Revision 1.15  2007/04/12 09:08:57  haraldkipp
-- Segmented buffer routines ported to ARM.
--
-- Revision 1.14  2006/02/08 15:20:22  haraldkipp
-- ATmega2561 Support
--
-- Revision 1.13  2006/01/23 17:31:07  haraldkipp
-- Dependency added to system configuration.
--
-- Revision 1.12  2005/10/24 09:58:21  haraldkipp
-- Generalized description for NUT_CPU_FREQ.
--
-- Revision 1.11  2005/10/04 05:44:29  hwmaier
-- Added support for separating stack and conventional heap as required by AT09CAN128 MCUs
--
-- Revision 1.10  2005/07/26 16:13:24  haraldkipp
-- Target dependent modules moved to arch.
--
-- Revision 1.9  2005/02/16 20:02:07  haraldkipp
-- Typo of NUTDEBUG corrected.
-- Philipp Blum's tracer added.
--
-- Revision 1.8  2004/11/24 14:48:34  haraldkipp
-- crt/crt.nut
--
-- Revision 1.7  2004/11/08 18:58:58  haraldkipp
-- Configurable stack sizes
--
-- Revision 1.6  2004/09/25 15:42:09  drsung
-- Removed configuration for separate interrupt stack,
-- it's now in conf/dev/dev.nut
--
-- Revision 1.5  2004/09/01 14:07:15  haraldkipp
-- Cleaned up memory configuration
--
-- Revision 1.4  2004/08/18 16:05:38  haraldkipp
-- Use consistent directory structure
--
-- Revision 1.3  2004/08/18 13:46:10  haraldkipp
-- Fine with avr-gcc
--
-- Revision 1.2  2004/08/03 15:09:31  haraldkipp
-- Another change of everything
--
-- Revision 1.1  2004/06/07 16:38:43  haraldkipp
-- First release
--
--

nutos =
{
    --
    -- Version Information
    --
    {
        name = "nutos_version",
        brief = function() return "Version " .. GetNutOsVersion(); end,
        description = "The version info was read from os/version.c in the current source tree.",
        sources = { "version.c" },
        options =
        {
            {
                macro = "NUT_VERSION_EXT",
                brief = "Extended",
                description = "User provided extension to the hard coded version information.",
                requires = { "NOT_AVAILABLE" },
                file = "include/cfg/os.h"
            }
        }
    },

    --
    -- Initialization
    --
    {
        name = "nutos_init",
        brief = "System Initialization",
        description = "This module is automatically called after booting the system. "..
                      "It will initialize memory and timer hardware and start the "..
                      "Nut/OS idle thread, which in turn starts the application's "..
                      "main routine in a separate thread.",
        sources = { "nutinit.c" },
        targets = { "nutinit.o" },
    },

    --
    -- Memory management
    --
    {
        name = "nutos_heap",
        brief = "Memory management",
        provides = { "NUT_HEAPMEM" },
        sources = { "heap.c" },
        options =
        {
            {
                macro = "NUTMEM_GUARD",
                brief = "Guard Bytes",
                description = "If enabled, guard bytes will be added to both ends when "..
                              "new memory is allocated. When memory is released later, "..
                              "these guard bytes will be checked in order to detect "..
                              "memory overflows.",
                flavor = "boolean",
                file = "include/cfg/memory.h"
            },
            {
                macro = "NUTDEBUG_HEAP",
                brief = "Heap Debugging",
                description = "If enabled, additional code will be added to the heap "..
                              "management to track memory allocation. This helps to "..
                              "detect memory leaks. Furthermore, problems are reported "..
                              "by calling NUTPANIC with a source code reference.",
                flavor = "boolean",
                file = "include/cfg/memory.h"
            },
            {
                macro = "NUTMEM_SIZE",
                brief = "Memory Size",
                description = "Number of bytes available in fast data memory. On "..
                              "most platforms this value specifies the total number "..
                              "of bytes available in RAM.\n\n"..
                              "On Harvard architectures this value specifies the size "..
                              "of the data memory. It will be occupied by global "..
                              "variables and static data. Any remaining space will "..
                              "be added to the Nut/OS heap during system initialization.\n"..
                              "When running on an AVR MCU, set this to size of the "..
                              "on-chip SRAM, e.g. 4096 for the ATmega128 and 8192 for the ATmega2561.",
                default = "4096",
                file = "include/cfg/memory.h"
            },
            {
                macro = "NUTMEM_START",
                brief = "Memory Start",
                description = "First address of fast data memory.",
                file = "include/cfg/memory.h"
            },
            {
                macro = "NUTMEM_RESERVED",
                brief = "Reserved Memory Size",
                description = "Number of bytes reserved for special purposes.\n"..
                              "Right now this is used with the AVR platform only. "..
                              "The specified number of bytes may be used by a "..
                              "device driver when the external memory interface "..
                              " is disabled.",
                requires = { "HW_MCU_AVR" },
                flavor = "booldata",
                file = "include/cfg/memory.h",
                makedefs = { "NUTMEM_RESERVED" }
            },
            {
                macro = "NUTXMEM_SIZE",
                brief = "Extended Memory Size",
                description = "Number of bytes available in external data memory.\n\n"..
                              "The result of enabling this item is platform specific."..
                              "With AVR systems it will enable the external memory "..
                              "interface of the CPU, even if the value is set to zero.",
                provides = { "NUTXDATAMEM_SIZE" },
                flavor = "booldata",
                file = "include/cfg/memory.h"
            },
            {
                macro = "NUTXMEM_START",
                brief = "Extended Memory Start",
                description = "First address of external data memory.",
                requires = { "NUTXDATAMEM_SIZE" },
                file = "include/cfg/memory.h"
            },
            {
                macro = "NUTMEM_SPLIT_FAST",
                brief = "Split Memory",
                description = "If enabled and if two memory regions are available on the "..
                              "target board, then each region is managed separately.",
                flavor = "boolean",
                file = "include/cfg/memory.h"
            },
            {
                macro = "NUTMEM_STACKHEAP",
                brief = "Separate heap for stack",
                description = "This option enables use of a separate heap for stack.\n\n"..
                              "When a thread is created with this option enabled, it's stack is "..
                              "allocated on a special \"thread stack heap\" which is kept in "..
                              "internal memory before the data segments instead of using the \"standard "..
                              "heap\" which is typically located in external memory after the data segments. \n"..
                              "\n"..
                              "Using this option is a must for silicon revisions C of the AT90CAN128 MCU \n"..
                              "as the device misfunctions when code stack is in XRAM.  Refer to \n"..
                              "AT90CAN128 Datasheet Rev. 4250F–CAN–04/05 - Errata Rev C \n"..
                              "\n"..
                              "Use this option is conjunction with DATA_SEG for AT90CAN128 MCUs!",
                requires = { "HW_MCU_AVR" },
                flavor = "booldata",
                file = "include/cfg/memory.h"
            },
            {
                macro = "DATA_SEG",
                brief = "Start of data segment",
                description = "This option relocates the data segment to a different address during the linking phase.\n\n"..
                              "Leave this option empty to use the architecture's default setting.\n\n"..
                              "Using this option is a must for silicon revisions C of the AT90CAN128 MCU \n"..
                              "as the device misfunctions when code stack is in XRAM.  Refer to \n"..
                              "AT90CAN128 Datasheet Rev. 4250F–CAN–04/05 - Errata Rev C \n"..
                              "\n"..
                              "Use this option is conjunction with NUTMEM_STACKHEAP for AT90CAN128 MCUs!",
                requires = { "HW_MCU_AVR" },
                flavor = "booldata",
                makedefs = { "DATA_SEG" }
            }
        }
    },
    {
        name = "nutos_bankmem",
        brief = "Segmented Buffer",
        description = "The segmented buffer API can either handle a continuos "..
                      "memory space automatically allocated from heap or it can "..
                      "use banked memory hardware.",
        -- requires = { "HW_MCU_AVR" },
        provides = { "NUT_SEGBUF" },
        sources = { "bankmem.c" },
        options =
        {
            {
                macro = "NUTBANK_COUNT",
                brief = "Memory Banks",
                description = "Number of memory banks.\n\n"..
                              "Specially on 8-bit systems the address space is typically "..
                              "very limited. To overcome this, some hardware implementations "..
                              "like the Ethernut 2 reference design provide memory banking. "..
                              "Right now this is supported for the AVR platform only.",
                requires = { "HW_MCU_AVR" },
                provides = { "NUTBANK_COUNT" },
                flavor = "booldata",
                file = "include/cfg/memory.h"
            },
            {
                macro = "NUTBANK_START",
                brief = "Banked Memory Start",
                description = "First address of the banked memory area.",
                requires = { "NUTBANK_COUNT" },
                file = "include/cfg/memory.h"
            },
            {
                macro = "NUTBANK_SIZE",
                brief = "Banked Memory Size",
                description = "Size of the banked memory area.",
                requires = { "NUTBANK_COUNT" },
                file = "include/cfg/memory.h"
            },
            {
                macro = "NUTBANK_SR",
                brief = "Bank Select Register",
                description = "Address of the bank select register.",
                requires = { "NUTBANK_COUNT" },
                file = "include/cfg/memory.h"
            }
        }
    },

    --
    -- Thread management
    --
    {
        name = "nutos_thread",
        brief = "Multithreading",
        requires = { "NUT_CONTEXT_SWITCH" },
        provides = { "NUT_THREAD" },
        sources = { "thread.c" },
        options =
        {
            {
                macro = "NUT_THREAD_IDLESTACK",
                brief = "Idle Thread Stack Size",
                description = "Number of bytes to be allocated for the stack of the idle thread.",
                flavor = "booldata",
                file = "include/cfg/os.h"
            },
            {
                macro = "NUT_THREAD_MAINSTACK",
                brief = "Main Thread Stack Size",
                description = "Number of bytes to be allocated for the stack of the main thread.",
                flavor = "booldata",
                file = "include/cfg/os.h"
            },
            {
                macro = "NUT_CRITICAL_NESTING",
                brief = "Critical Section Nesting",
                description = "The kernel avoids nesting of critical sections, but applications "..
                              "may want to use this feature. When enabled, a global counter keeps "..
                              "track of the nesting level. Disadvantages are increased code "..
                              "size and a significantly increased interrupt latency time.\n\n"..
                              "This option is currently available for ARM targets only. "..
                              "On the AVR, nesting of critical sections is available by default.",
                flavor = "boolean",
                requires = { "HW_MCU_ARM" },
                provides = { "NUT_CRITNESTING" },
                file = "include/cfg/os.h"
            },
            {
                macro = "NUT_CRITICAL_NESTING_STACK",
                brief = "Critical Section Nesting Uses Stack",
                description = "Using the stack for critical section nesting results in better "..
                              "interrupt responsiveness. However, most compilers are not able "..
                              "to deal with stack modifications. Use this option with great care.\n\n"..
                              "This option is ignored, if critical section nesting is disabled.",
                flavor = "boolean",
                requires = { "HW_MCU_ARM", "NUT_CRITNESTING" },
                file = "include/cfg/os.h"
            },
        }
    },

    --
    -- Timer management
    --
    {
        name = "nutos_timer",
        brief = "Timer management",
        requires = { "NUT_EVENT", "NUT_OSTIMER_DEV" },
        provides = { "NUT_TIMER" },
        sources = { "timer.c" },
        options =
        {
            {
                macro = "NUT_CPU_FREQ",
                brief = "Fixed MCU clock",
                description = "Frequency of the MCU clock. On some boards the system is able "..
                              "to automatically determine this value during initialization "..
                              "by using a reference clock. In this case the option may be disabled.",
                flavor = "booldata",
                file = "include/cfg/os.h"
            },
            {
                macro = "NUT_DELAYLOOPS",
                brief = "Loops per Millisecond",
                description = "Number of loops to execute per millisecond in NutMicroDelay.\n\n"..
                              "This value depends not only on the target hardware, but also on "..
                              "compiler options. If not specified, the system will try to "..
                              "determine a rough approximation.",
                flavor = "booldata",
                file = "include/cfg/os.h"
            }
        }
    },

    --
    -- Event management
    --
    {
        name = "nutos_event",
        brief = "Event handling",
        description = "Events provide the core thread synchronization.",
        provides = { "NUT_EVENT" },
        sources = { "event.c" }
    },


    {
        name = "nutos_devreg",
        brief = "Device registration",
        description = "Applications need to register devices before using "..
                      "them. This serves two purposes. First it will create "..
                      "a link to the device driver code. Second it will "..
                      "initialize the device hardware.",
        provides = { "NUT_DEVREG" },
        sources = { "devreg.c" }
    },
    {
        name = "nutos_confos",
        brief = "Configuration",
        description = "Initial system settings are stored in non volatile memory."..
                      "The current version uses 3 bytes for validity check, 15 bytes "..
                      "for the host name and one last byte, which is always zero.\n\n"..
                      "The length of the host name is configurable.",
        provides = { "NUT_OSCONFIG" },
        sources = { "confos.c" },
        options =
        {
            {
                macro = "CONFOS_EE_OFFSET",
                brief = "Location",
                description = "This is the non-volatile memory address offset, where Nut/OS "..
                              "expects its configuration.\n\n"..
                              "Note, that changing this value will invalidate previously "..
                              "stored setting after upgrading to this new version. You must "..
                              "also make sure, that this memory area will not conflict with "..
                              "others, specifically the network configuration.\n\n"..
                              "This item is disabled if the system doesn't offer any "..
                              "non-volatile memory. Check the non-volatile memory"..
                              "module in the device driver section.",
                requires = { "DEV_NVMEM" },
                default = "0",
                type = "integer",
                file = "include/cfg/eeprom.h"
            },
            {
                macro = "CONFOS_EE_MAGIC",
                brief = "Magic Cookie",
                description = "Together with the length of the configuration structure "..
                              "this is used to determine that we got a valid configuration "..
                              "structure in non-volatile memory.\n",
                default = "\"OS\"",
                file = "include/cfg/eeprom.h"
            },
            {
                macro = "MAX_HOSTNAME_LEN",
                brief = "Max. Host Name Length",
                description = "The name of the local host can't grow beyond this size. "..
                              "This is just the basic name without domain information.\n\n"..
                              "Make sure that the virgin host name will fit. Further, "..
                              "keep in mind that changing this size will have an impact "..
                              "at least on the auto discovery feature.",
                default = "15",
                type = "integer",
                file = "include/cfg/eeprom.h"
            },
            {
                macro = "CONFOS_VIRGIN_HOSTNAME",
                brief = "Virgin Host Name",
                description = "This name will be used for hosts without valid configuration.\n",
                default = "\"ethernut\"",
                file = "include/cfg/eeprom.h"
            }
        }
    },

    --
    -- Additional functions, not required by the kernel.
    --
    {
        name = "nutos_semaphore",
        brief = "Semaphores",
        description = "Semaphores are not required by the kernel, but "..
                      "may be useful for applications.",
        requires = { "NUT_EVENT" },
        provides = { "NUT_SEMAPHORE" },
        sources = { "semaphore.c" }
    },
    {
        name = "nutos_mutex",
        brief = "Mutual exclusion semaphores",
        description = "Mutex semaphores are not required by the kernel, but "..
                      "may be useful for applications.",
        requires = { "NUT_EVENT" },
        provides = { "NUT_MUTEX" },
        sources = { "mutex.c" }
    },
    {
        name = "nutos_msg",
        brief = "Message queues",
        description = "Message queues are not required by the kernel, but "..
                      "may be useful for applications.",
        requires = { "NUT_EVENT" },
        provides = { "NUT_MQUEUE" },
        sources = { "msg.c" }
    },
    {
        name = "nutos_condition",
        brief = "Condition variables",
        description = "Condition variabled are not required by the kernel, but "..
                      "may be useful for applications.",
        requires = { "NUT_EVENT", "NUT_MUTEX"},
        provides = { "NUT_CONDITION_VARIABLES" },
        sources = { "condition.c" }
    },


    --
    -- Debugging
    --
    {
        name = "nutos_osdebug",
        brief = "OS Debug",
        requires = { "NUT_EVENT", "CRT_STREAM_WRITE" },
        provides = { "NUT_OSDEBUG" },
        sources = { "osdebug.c" },
        options =
        {
            {
                macro = "NUTDEBUG",
                brief = "OS Debug",
                description = "Used for kernel debugging.\n"..
                              "Enabling this functions will add a lot of "..
                              "extra code and require more RAM. In addition "..
                              "the application must register an output device "..
                              "early and redirect stdout to it.",
                flavor = "boolean",
                file = "include/cfg/os.h"
            },
            {
            	macro = "NUTDEBUG_CHECK_STACK",
            	brief = "Thread Stack Checking",
            	description = "Used for thread stack checking.\n"..
            				  "Enabling this will fill any threads stack area "..
            				  "with a pattern at the point of creating the thread.\n"..
            				  "By using NutThreadStackAvailable() one can now check "..
            				  "for the maximum ammount of stack ever used by this "..
            				  "thread.\n\n"..
            				  "This functionality can be enabled without OS Debug to "..
            				  "find out maximum stack usage in your final appliation.",
                flavor = "boolean",
                file = "include/cfg/os.h"
            }
        }
    },
    {
        name = "nutos_ostracer",
        brief = "OS Tracer",
        requires = { "HW_MCU_AVR", "TOOL_GCC" },
        provides = { "NUT_OSTRACER" },
        sources = { "tracer.c" },
        options =
        {
            {
                macro = "NUTTRACER",
                brief = "OS Tracer",
                description = "Used for kernel debugging.\n",
                flavor = "boolean",
                file = "include/cfg/os.h"
            },
            {
                macro = "NUTTRACER_CRITICAL",
                brief = "OS Critical Tracer",
                description = "Used for kernel debugging.\n",
                flavor = "boolean",
                file = "include/cfg/os.h"
            }
        }
    },
    {
        name = "nutos_fatal",
        brief = "Fatal Error Handler",
        description = "This default handler may be overridden by "..
                      "NUTFATAL and NUTPANIC routines provided by the application.",
        sources = { "fatal.c", "panic.c" }
    }
}

