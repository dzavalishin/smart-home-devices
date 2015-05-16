#ifndef _NETINET_TCP_H_
#define _NETINET_TCP_H_

/*
 * Copyright (C) 2001-2003 by egnite Software GmbH. All rights reserved.
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
 * Portions Copyright (C) 2000 David J. Hudson <dave@humbug.demon.co.uk>
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can redistribute this file and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software Foundation;
 * either version 2 of the License, or (at your discretion) any later version.
 * See the accompanying file "copying-gpl.txt" for more details.
 *
 * As a special exception to the GPL, permission is granted for additional
 * uses of the text contained in this file.  See the accompanying file
 * "copying-liquorice.txt" for details.
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
 */

/*
 * $Log$
 * Revision 1.6  2008/08/11 07:00:23  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.5  2006/03/21 21:22:18  drsung
 * Enhancement made to TCP state machine. Now TCP options
 * are read from peer and at least the maximum segment size is stored.
 *
 * Revision 1.4  2005/08/02 17:46:49  haraldkipp
 * Major API documentation update.
 *
 * Revision 1.3  2005/04/05 17:38:45  haraldkipp
 * ARM7 memory alignment bugs fixed.
 *
 * Revision 1.2  2004/03/16 16:48:28  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 * Revision 1.1.1.1  2003/05/09 14:41:16  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.7  2003/02/04 18:00:46  harald
 * Version 3 released
 *
 * Revision 1.6  2002/09/03 17:49:58  harald
 * *** empty log message ***
 *
 * Revision 1.5  2002/08/16 17:53:36  harald
 * Larger window size for better MP3 stream results
 *
 * Revision 1.4  2002/06/26 17:29:22  harald
 * First pre-release with 2.4 stack
 *
 */

#include <sys/types.h>
#include <stdint.h>

/*!
 * \file netinet/tcp.h
 * \brief TCP protocol definitions.
 */

#ifdef __cplusplus
extern "C" {
#endif

#define TH_FIN  0x01    /*!< \brief Finishing transmission. */
#define TH_SYN  0x02    /*!< \brief Synchronizing sequence numbers. */
#define TH_RST  0x04    /*!< \brief Reset connection. */
#define TH_PUSH 0x08    /*!< \brief Push data to application level. */
#define TH_ACK  0x10    /*!< \brief Acknowledge field is valid. */
#define TH_URG  0x20    /*!< \brief Urgent data present. */

/*! \brief TCP flag mask. */
#define TH_FLAGS (TH_FIN | TH_SYN | TH_RST | TH_ACK | TH_URG)

/*!
 * \struct tcphdr tcp.h netinet/tcp.h
 * \brief TCP protocol header structure.
 */
/*!
 * \typedef TCPHDR
 * \brief TCP protocol header type.
 */
typedef struct __attribute__ ((packed)) tcphdr {
    uint16_t th_sport;       /*!< \brief Source port. */
    uint16_t th_dport;       /*!< \brief Destination port. */
    uint32_t  th_seq;         /*!< \brief Sequence number of first octet in this segment. */
    uint32_t  th_ack;         /*!< \brief Expected sequence number of next octet. */
#ifndef __BIG_ENDIAN__
#ifdef __IMAGECRAFT__
    unsigned th_x2:4,
             th_off:4;
#else /* #ifndef __BIG_ENDIAN__ */
    uint8_t  th_x2:4,        /*!< \brief Unused. */
            th_off:4;       /*!< \brief Data offset. */
#endif
#else /* #ifndef __BIG_ENDIAN__ */
    uint8_t  th_off:4,       /*!< \brief Data offset. */
	    th_x2:4;        /*!< \brief Unused. */
#endif
    uint8_t  th_flags;       /*!< \brief Control flags. */
    uint16_t th_win;         /*!< \brief Number of acceptable octects. */
    uint16_t th_sum;         /*!< \brief 96 byte pseudo header checksum. */
    uint16_t th_urp;         /*!< \brief Urgent data pointer. */
} TCPHDR;

#define TCPOPT_EOL                  0   /*!< \brief End of options. */
#define TCPOPT_NOP                  1   /*!< \brief Nothing. */
#define TCPOPT_MAXSEG               2   /*!< \brief Maximum segment size. */
#define TCPOLEN_MAXSEG              4   /*!< \brief Maximum segment size length. */
#define TCPOPT_WINDOW               3   /*!< \brief Receive window. */
#define TCPOLEN_WINDOW              3   /*!< \brief Receive window length. */

/*!
 * \struct _TCPPSEUDOHDR tcp.h netinet/tcp.h
 * \brief TCP pseudo header structure.
 */
/*!
 * \typedef TCPPSEUDOHDR
 * \brief TCP pseudo header type.
 */
typedef struct _TCPPSEUDOHDR {
    uint32_t  tph_src;    /*!< \brief IP address of sender. */
    uint32_t  tph_dst;    /*!< \brief IP address of target. */
    uint8_t  tph_mbz;    /*!< \brief mbz */
    uint8_t  tph_p;      /*!< \brief p */
    uint16_t tph_len;    /*!< \brief len */
    uint16_t tph_sum;    /*!< \brief Checksum */
} TCPPSEUDOHDR;


#define TCP_MSS             536     /*!< \brief Default maximum segment size.
                                         The maximum size of an IP datagram, that will
                                         not become fragmented, is 576. The maximum
                                         IP datagram for Ethernet is 1500. Reduce this
                                         number by 40, 20 bytes TCP header and 20 bytes
                                         IP header.
                                         \showinitializer 
                                     */
#define TCP_WINSIZE         3216    /*!< \brief Default window size.
                                         It's recommended to set this 6 times the maximum
                                         segment size.
                                         \showinitializer 
                                     */

#define TCP_MAXWIN          65535   /*!< \brief Largest value for (unscaled) window. \showinitializer */
#define TTCP_CLIENT_SND_WND 4096    /*!< \brief Default send window for T/TCP client. \showinitializer */

#define TCP_MAX_WINSHIFT    14      /*!< \brief Maximum window shift. \showinitializer */

#define TCP_MAXHLEN         (0xf<<2)    /*!< \brief Maximum length of header in bytes. */
#define TCP_MAXOLEN         (TCP_MAXHLEN - sizeof(struct tcphdr))   /*!< \brief Maximum space left for options. */

/*
 * User-settable options.
 */
#define TCP_NODELAY 0x01    /*!< \brief Don't delay send to coalesce segments. */
#define TCP_MAXSEG  0x02    /*!< \brief Set maximum segment size. */
#define TCP_NOPUSH  0x04    /*!< \brief Don't push last block of write. */
#define TCP_NOOPT   0x08    /*!< \brief Don't use TCP options. */

#ifdef __cplusplus
}
#endif

#endif
