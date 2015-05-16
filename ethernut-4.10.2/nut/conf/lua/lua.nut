--
-- Copyright (C) 2008 by egnite GmbH. 
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

-- Lua Script Interpreter
--
-- $Id: lua.nut 2887 2010-02-06 11:12:11Z haraldkipp $
--

nutlua =
{
    {
        name = "nutlua_std_library",
        brief = "Standard Libraries",
        description = "Standard libraries are automatically loaded. "..
                      "Not all targets offer enough memory to load "..
                      "all libraries.",
        requires = { "TOOL_GCC" },
        sources = { 
            "lapi.c",
            "ldo.c",
            "ldump.c",
            "ldebug.c",
            "lfunc.c",
            "lgc.c",
            "lmem.c",
            "lobject.c",
            "lopcodes.c",
            "lstate.c",
            "lstring.c",
            "ltable.c",
            "ltm.c",
            "lundump.c",
            "lvm.c",
            "lzio.c",
            "lauxlib.c",
            "linit.c",
            "loadlib.c",
            "lbaselib.c",
            "liolib.c",
            "loslib.c",
            "ldblib.c",
            "lmathlib.c",
            "ltablib.c",
            "lstrlib.c",
            "lcode.c",
            "llex.c",
            "lparser.c",
            "lrotable.c"
        },
        options =
        {
            {
                macro = "NUTLUA_OPTIMIZE_MEMORY",
                brief = "Memory Optimization",
                description = "Minimizes RAM footprint at the expense of speed.\n\n"..
                              "0 - no optimizations\n"..
                              "1 - optimize while maitaining full compatibility with the test suite\n"..
                              "2 - aggresive optimizations (breaks compatibility with some tests)",
                type = "enumerated",
                default = "0",
                choices = { "0", "1", "2" },
                file = "include/cfg/lua.h"
            },
            {
                macro = "NUTLUA_FLOATING_POINT",
                brief = "Floating Point Numbers",
                description = "Floating point numbers are used when enabled.",
                requires = { "FLOATING_POINT_IO" },
                flavor = "boolean",
                file = "include/cfg/lua.h"
            },
            {
                macro = "NUTLUA_DEBUGLIB_IS_STANDARD",
                brief = "Debug Library",
                description = "Lua debug library is standard when enabled.",
                flavor = "boolean",
                file = "include/cfg/lua.h"
            },
            {
                macro = "NUTLUA_IOLIB_IS_STANDARD",
                brief = "I/O Library",
                description = "Lua I/O library is standard when enabled.",
                flavor = "boolean",
                file = "include/cfg/lua.h"
            },
            {
                macro = "NUTLUA_IOLIB_TCP",
                brief = "TCP I/O",
                description = "Lua I/O library with TCP/IP support.",
                flavor = "boolean",
                file = "include/cfg/lua.h"
            },
            {
                macro = "NUTLUA_LOADLIB_IS_STANDARD",
                brief = "Package Load Library",
                description = "Lua package load library is standard when enabled.",
                flavor = "boolean",
                file = "include/cfg/lua.h"
            },
            {
                macro = "NUTLUA_MATHLIB_IS_STANDARD",
                brief = "Math Library",
                description = "Lua math library is standard when enabled.",
                flavor = "boolean",
                file = "include/cfg/lua.h"
            },
            {
                macro = "NUTLUA_OSLIB_IS_STANDARD",
                brief = "OS Library",
                description = "Lua OS library is standard when enabled.",
                flavor = "boolean",
                file = "include/cfg/lua.h"
            },
            {
                macro = "NUTLUA_STRLIB_IS_STANDARD",
                brief = "String Library",
                description = "Lua string library is standard when enabled.",
                flavor = "boolean",
                file = "include/cfg/lua.h"
            },
            {
                macro = "NUTLUA_TABLIB_IS_STANDARD",
                brief = "Table Library",
                description = "Lua table library is standard when enabled.",
                flavor = "boolean",
                file = "include/cfg/lua.h"
            },
            {
                macro = "NUTLUA_PARSER_EXCLUDED",
                brief = "Exclude Parser",
                description = "Lua parser is excluded when enabled.",
                flavor = "boolean",
                file = "include/cfg/lua.h"
            },
            {
                macro = "NUTLUA_DUMP_EXCLUDED",
                brief = "Exclude Dump",
                description = "Lua binary code dump is excluded when enabled.",
                flavor = "boolean",
                file = "include/cfg/lua.h"
            }
        }
    }
}
