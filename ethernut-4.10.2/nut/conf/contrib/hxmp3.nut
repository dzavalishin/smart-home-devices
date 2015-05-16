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

-- Helix MP3 Decoder
--
-- $Id: hxmp3.nut 2343 2008-10-05 17:01:15Z haraldkipp $
--

nutcontrib_hxmp3 =
{
    {
        name = "nutcontrib_polyphase_synth",
        brief = "Polyphase Synthesis Filter",
        requires = { "LICENSE_RPSL_RCSL", "HW_MCU_ARM" },
        provides = { "AUDIO_POLYPHASEFILT" },
        sources = { "hxmp3/asmpoly_gcc.S" },
        targets = { "hxmp3/asmpoly_gcc.o" },
    },
    {
        name = "nutcontrib_helix_decoder",
        brief = "Helix MP3 Decoder",
        requires = { "LICENSE_RPSL_RCSL", "AUDIO_POLYPHASEFILT" },
        provides = { "AUDIO_DECODER", "AUDIO_DECODER_HELIX" },
        sources = 
        {
            "hxmp3/bitstream.c",
            "hxmp3/buffers.c",
            "hxmp3/dct32.c",
            "hxmp3/dequant.c",
            "hxmp3/dqchan.c",
            "hxmp3/huffman.c",
            "hxmp3/hufftabs.c",
            "hxmp3/imdct.c",
            "hxmp3/mp3dec.c",
            "hxmp3/mp3tabs.c",
            "hxmp3/scalfact.c",
            "hxmp3/stproc.c",
            "hxmp3/subband.c",
            "hxmp3/trigtabs.c"
        },
    },
    {
        name = "nutcontrib_hermite_resampler",
        brief = "Hermite Resampler",
        requires = { "LICENSE_RPSL_RCSL" },
        provides = { "AUDIO_RESAMPLER" },
        sources = 
        { 
            "hxmp3/hermite.c"
        }
    }
}
