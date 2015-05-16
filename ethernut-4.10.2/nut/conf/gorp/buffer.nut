--
-- Copyright (C) 2010 by egnite GmbH
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

-- Gorp buffer modules.
--
-- $Id$
--

nutgorp_buffer =
{
    {
        name = "nutgorp_buffer_perci",
        brief = "Persistant Ring Buffer",
        description = "Circular buffer using a file.\n\n"..
                      "PerCi buffers are quite useful for log files. They are "..
                      "created with a fixed size and will never grow beyond "..
                      "their initial size. Instead, the oldest data will be "..
                      "overwritten when new data is added to a completely filled "..
                      "file.",
        requires = { "NUT_FS_READ", "NUT_FS_WRITE" },
        sources = { 
            "buffer/perci.c"
        },
        options =
        {
            {
                macro = "PERCI_MAX_RECORDS",
                brief = "Max. Number of Records",
                description = "Maximum number of records.\n\n"..
                              "This is a system wide upper limit. The file's actual number "..
                              "of records will be specified during its initialization",
                type = "integer",
                default = 256,
                file = "include/cfg/perci.h"
            },
            {
                macro = "PERCI_RECSIZE",
                brief = "Record Size",
                description = "Number of bytes per record.\n\n"..
                              "This is a system wide value. If changed, existing files "..
                              "must be re-initialized and their content is lost.",
                type = "integer",
                default = 256,
                file = "include/cfg/perci.h"
            }
        }
    }
}
