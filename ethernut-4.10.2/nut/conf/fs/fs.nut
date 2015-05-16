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
-- Revision 1.8  2009/01/09 17:54:42  haraldkipp
-- Added raw file system driver.
--
-- Revision 1.7  2006/08/01 07:42:56  haraldkipp
-- New functions extract last component and parent directory from pathnames.
--
-- Revision 1.6  2006/02/23 15:43:08  haraldkipp
-- PHAT file system now supports configurable number of sector buffers.
--
-- Revision 1.5  2006/01/05 16:49:48  haraldkipp
-- PHAT file system driver added.
--
-- Revision 1.4  2005/04/05 17:56:16  haraldkipp
-- UROM file system is not platform dependant.
--
-- Revision 1.3  2005/02/05 20:40:44  haraldkipp
-- Peanut added.
--
-- Revision 1.2  2004/08/18 13:46:09  haraldkipp
-- Fine with avr-gcc
--
-- Revision 1.1  2004/06/07 16:35:53  haraldkipp
-- First release
--
--

nutfs =
{
    {
        name = "nutfs_pathops",
        brief = "Path Operations",
        requires = { "NUT_FS_WRITE", "NUT_FS_DIR" },
        provides = { "NUT_FS_PATHOPS" },
        description = "Standard file system functions:\n"..
                      "mkdir()\nstat()\nfstat()\nunlink()\nrmdir()\nlseek()\n",
        sources = { "pathops.c" },
    },
    {
        name = "nutfs_pathnames",
        brief = "Pathname Operations",
        description = "Standard pathname functions:\n"..
                      "basename()\ndirname()\n",
        sources = { "basename.c", "dirname.c" },
    },
    {
        name = "nutfs_dirent",
        brief = "Directory Read",
        requires = { "NUT_FS_DIR" },
        provides = { "NUT_FS_DIRSTREAM" },
        description = "Standard functions for reading directories:\n"..
                      "opendir()\nclosedir()\nreaddir()\n",
        sources = { "dirent.c" },
    },
    {
        name = "nutfs_fat",
        brief = "FAT32",
        description = "Read only.",
        requires = { "NUT_EVENT", "HW_MCU_AVR" },
        provides = { "NUT_FS", "NUT_FS_READ" },
        sources = { "fat.c" }
    },
    {
        name = "nutfs_uromfs",
        brief = "UROM",
        provides = { "NUT_FS", "NUT_FS_READ" },
        description = "Read only.",
        sources = { "uromfs.c" },
        makedefs = { "CRUROM=crurom" }
    },
    {
        name = "nutfs_uflashfs",
        brief = "UFLASH",
        requires = { "SERIALFLASH_INTERFACE" },
        provides = { "NUT_FS", "NUT_FS_READ", "NUT_FS_WRITE", "NUT_FS_DIR" },
        sources = { "uflashfs.c" },
        options =
        {
            {
                macro = "UFLASH_MAX_BLOCKS",
                brief = "Maximum Blocks",
                description = "Maximum number of blocks supported.\n\n"..
                              "UFLASH uses a table in RAM to translate logical to physical "..
                              "block numbers. This option can be used to limit the number "..
                              "of blocks in order to reduce RAM usage.",
                type = "integer",
                default = "8192",
                file = "include/cfg/fs.h"
            },
            {
                macro = "UFLASH_BLOCK_UNITS",
                brief = "Units per Block",
                description = "Number of flash units used for one block.\n\n"..
                              "A UFLASH file system block must have at least 2 units.\n\n"..
                              "Serial flash memory is divided in various ways. For the UFLASH "..
                              "file system a unit is the smallest part, that can be written and "..
                              "erased. Typically a unit matches a flash memory page.",
                type = "integer",
                default = "4",
                file = "include/cfg/fs.h"
            },
            {
                macro = "UFLASH_ENTRIES",
                brief = "Maximum Number of Entries",
                description = "Specifies the number of files that can exist on a volume.",
                type = "integer",
                default = "128",
                file = "include/cfg/fs.h"
            },
            {
                macro = "UFLASH_MAX_PATH",
                brief = "Maximum Entry Name Size",
                description = "If enabled, the given number of bytes are reserved for "..
                              "the name of each entry. Otherwise variable length names "..
                              "are used.\n\n",
                type = "integer",
                flavor = "booldata",
                file = "include/cfg/fs.h"
            },
            {
                macro = "UFLASH_USE_TIMESTAMP",
                brief = "Timestamps",
                description = "If enabled, the last modification date and time is maintained. "..
                              "For reasonable results, this requires that the system time had been set. ",
                flavor = "boolean",
                file = "include/cfg/fs.h"
            },
            {
                macro = "UFLASH_USAGE_CACHE",
                brief = "Usage Cache Size",
                type = "integer",
                default = "32",
                file = "include/cfg/fs.h"
            },
        },
    },
    {
        name = "nutfs_phatfs",
        brief = "PHAT",
        requires = { "DEV_BLOCK" },
        provides = { "NUT_FS", "NUT_FS_READ", "NUT_FS_WRITE", "NUT_FS_DIR" },
        description = "FAT compatible file system.",
        sources = { 
                "phatfs.c", 
                "phatvol.c", 
                "phatdir.c", 
                "phatio.c", 
                "phat12.c", 
                "phat16.c", 
                "phat32.c", 
                "phatutil.c",
                "phatdbg.c"
        },
        options =
        {
            {
                macro = "PHAT_SECTOR_BUFFERS",
                brief = "Sector Buffers",
                description = "Number of sector buffers.\n\n"..
                              "These buffers will be allocated from heap memory. Thus, you "..
                              "need to make sure that enough heap memory is available.\n"..
                              "If this option is disabled, the file system will use "..
                              "a single sector buffer, which is typically provided by the "..
                              "block device itself.",
                type = "integer",
                flavor = "booldata",
                file = "include/cfg/fs.h"
            },
        },
    },
    {
        name = "nutfs_pnutfs",
        brief = "PNUT",
        -- requires = { "HW_MCU_AVR", "NUT_SEGBUF" },
        provides = { "NUT_FS", "NUT_FS_READ", "NUT_FS_WRITE", "NUT_FS_DIR" },
        description = "RAM file system for banked memory (Ethernut 2).",
        sources = { "pnutfs.c" },
    },
    {
        name = "nutfs_rawfs",
        brief = "Raw",
        requires = { "DEV_BLOCKIO" },
        provides = { "NUT_FS", "NUT_FS_READ", "NUT_FS_WRITE" },
        description = "This file system provides a single file entry only, "..
                      "which is mapped to the entire volume. This implies, "..
                      "that there is no file name and that the size of the "..
                      "file is fixed to the size of the device's volume.\n\n"..
                      "This early release had been tested with the AT45D DataFlash.",
        sources = { "rawfs.c" }
    }
}
