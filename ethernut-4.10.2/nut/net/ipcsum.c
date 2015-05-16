/*
 * Copyright (C) 2001-2005 by egnite Software GmbH. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY EGNITE SOFTWARE GMBH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL EGNITE
 * SOFTWARE GMBH OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * For additional information see http://www.ethernut.de/
 *
 * -
 * Portions Copyright (c) 1983, 1993 by
 *  The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * -
 * Portions Copyright (c) 1993 by Digital Equipment Corporation.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies, and that
 * the name of Digital Equipment Corporation not be used in advertising or
 * publicity pertaining to distribution of the document or software without
 * specific, written prior permission.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND DIGITAL EQUIPMENT CORP. DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS.   IN NO EVENT SHALL DIGITAL EQUIPMENT
 * CORPORATION BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

/*
 * $Log$
 * Revision 1.7  2008/08/11 07:00:30  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.6  2008/04/18 13:32:00  haraldkipp
 * Changed size parameter from u_short to int, which is easier to handle
 * for 32-bit targets. You need to recompile your ARM code. No impact on
 * AVR expected
 * I changed u_int to int at some places to avoid some warnings during
 * compilation of Nut/Net.
 * libs.
 *
 * Revision 1.5  2006/10/05 17:25:41  haraldkipp
 * Avoid possible alignment errors. Fixes bug #1567748.
 *
 * Revision 1.4  2005/04/05 17:39:56  haraldkipp
 * Replaced all this awful crap by a simple generic routine.
 *
 * Revision 1.3  2004/04/07 12:13:58  haraldkipp
 * Matthias Ringwald's *nix emulation added
 *
 * Revision 1.2  2004/03/16 16:48:45  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 * Revision 1.1.1.1  2003/05/09 14:41:32  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.15  2003/03/31 14:53:27  harald
 * Prepare release 3.1
 *
 * Revision 1.14  2003/02/04 18:14:57  harald
 * Version 3 released
 *
 * Revision 1.13  2002/10/29 15:30:26  harald
 * Temporarly switched back from asm to C
 *
 * Revision 1.12  2002/09/15 16:48:34  harald
 * *** empty log message ***
 *
 * Revision 1.11  2002/08/11 12:39:34  harald
 * ICC Assembler version by Klaus Ummenhofer
 *
 * Revision 1.8  2002/06/26 17:29:36  harald
 * First pre-release with 2.4 stack
 *
 * Revision 1.10  2002/08/08 17:29:28  harald
 * Imagecraft support by Klaus Ummenhofer
 *
 */

#include <netinet/ipcsum.h>

/*!
 * \addtogroup xgIP
 *
 * \brief Internet Protocol checksum and related support.
 *
 */

/*@{*/


/*!
 * \brief Calculate a partial IP checksum of a buffer.
 *
 * The caller must create the one's complement of the final result.
 *
 * \param ics Initial checksum from previous parts.
 * \param buf Pointer to the buffer.
 * \param len Number of bytes in the buffer.
 *
 * \return Partial checksum in network byte order.
 */
uint16_t NutIpChkSumPartial(uint16_t ics, CONST void *buf, int len)
{
    register uint32_t sum = ics;
    register uint8_t *cp = (uint8_t *) buf;

    /* Sum up 16 bit values. */
    while (len > 1) {
#ifdef __BIG_ENDIAN__
        sum += ((uint16_t)*cp << 8) | *(cp + 1);
#else
        sum += ((uint16_t)*(cp + 1) << 8) | *cp;
#endif
        cp += 2;
        len -= 2;
    }

    /* Add remaining byte on odd lengths. */
    if (len) {
#ifdef __BIG_ENDIAN__
        sum += (uint16_t)*cp << 8;
#else
        sum += *cp;
#endif
    }

    /* Fold upper 16 bits to lower ones. */
    while (sum >> 16) {
        sum = (uint16_t)sum + (sum >> 16);
    }
    return (uint16_t) sum;
}



/*!
 * \brief Calculates an the final IP checksum over a block of data.
 *
 * Unlike the partial checksum in NutIpChkSumPartial(), this function takes
 * the one's complement of the final result, thus making it the full checksum.
 */
uint16_t NutIpChkSum(uint16_t ics, CONST void *buf, int len)
{
    return ~NutIpChkSumPartial(ics, buf, len);
}

/*
 * Details of the pseudo header used as part of the
 * calculation of UDP and TCP header checksums.
 */
struct __attribute__ ((packed)) pseudo_hdr {
    uint32_t ph_src_addr;
    uint32_t ph_dest_addr;
    uint8_t ph_zero;
    uint8_t ph_protocol;
    uint16_t ph_len;
};

/*!
 * \brief Calculates the partial IP pseudo checksum.
 *
 */
uint32_t NutIpPseudoChkSumPartial(uint32_t src_addr, uint32_t dest_addr, uint8_t protocol, int len)
{
    struct pseudo_hdr ph;

    ph.ph_src_addr = src_addr;
    ph.ph_dest_addr = dest_addr;
    ph.ph_zero = 0;
    ph.ph_protocol = protocol;
    ph.ph_len = len;

    return NutIpChkSumPartial(0, &ph, sizeof(struct pseudo_hdr));
}

/*@}*/
