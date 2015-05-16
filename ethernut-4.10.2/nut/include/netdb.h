#ifndef _NETDB_H_

/*
 * Copyright (C) 2002 by egnite Software GmbH. All rights reserved.
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
 * Copyright (c) 1980, 1983, 1988, 1993
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
 *
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
 * Revision 1.9  2009/03/05 22:16:57  freckle
 * use __NUT_EMULATION instead of __APPLE__, __linux__, or __CYGWIN__
 *
 * Revision 1.8  2008/08/11 06:59:58  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.7  2008/02/15 17:07:09  haraldkipp
 * Added routine to query DNS IP settings.
 *
 * Revision 1.6  2005/07/26 15:49:59  haraldkipp
 * Cygwin support added.
 *
 * Revision 1.5  2005/04/04 19:33:40  freckle
 * added creation of include/netdb_orig.h, include/sys/socket_orig.h and
 * include/netinet/in_orig.h to allow unix emulation to use tcp/ip sockets
 *
 * Revision 1.4  2004/04/15 19:28:02  drsung
 * New function NutDnsGetMxByDomain to request
 * an MX record from DNS server.
 *
 * Revision 1.3  2004/03/16 16:48:27  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 * Revision 1.2  2003/07/20 18:26:41  haraldkipp
 * Support secondary DNS.
 *
 * Revision 1.1.1.1  2003/05/09 14:41:03  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.5  2003/05/06 18:38:33  harald
 * Cleanup
 *
 * Revision 1.4  2003/02/04 18:00:36  harald
 * Version 3 released
 *
 * Revision 1.3  2002/06/26 17:29:14  harald
 * First pre-release with 2.4 stack
 *
 */

/* use native version on unix emulation */
#ifdef __NUT_EMULATION__
#include <netdb_orig.h>
/* assure _NETDB_H_ is set */
#undef  _NETDB_H_
#define _NETDB_H_
#else  /* unix emulation */

#define _NETDB_H_
#include <sys/types.h>
#include <stdint.h>

#include <compiler.h> // For NUT_DEPRECATED

__BEGIN_DECLS

/*
 * Error return codes.
 */
#define NETDB_INTERNAL  -1      /* see errno */
#define NETDB_SUCCESS   0       /* no problem */
#define HOST_NOT_FOUND  1       /* Authoritative Answer Host not found */
#define TRY_AGAIN       2       /* Non-Authoritative Host not found, or SERVERFAIL */
#define NO_RECOVERY     3       /* Non recoverable errors, FORMERR, REFUSED, NOTIMP */
#define NO_DATA         4       /* Valid name, no data record of requested type */
#define NO_ADDRESS      NO_DATA /* no address, look for MX record */

__END_DECLS

#endif /* unix emulation */

typedef struct {
    uint8_t *doc_hostname;
    uint8_t *doc_domain;
    uint32_t doc_ip1;
    uint32_t doc_ip2;
} DNSCONFIG;

__BEGIN_DECLS

extern void NutDnsConfig2(CONST uint8_t * hostname, CONST uint8_t * domain, uint32_t pdnsip, uint32_t sdnsip);
extern void NutDnsGetConfig2(char ** hostname, char ** domain, uint32_t *pdnsip, uint32_t *sdnsip);
extern void NutDnsConfig(CONST uint8_t *hostname, CONST uint8_t *domain, uint32_t dnsip) NUT_DEPRECATED;
extern uint32_t NutDnsGetHostByName(CONST uint8_t *hostname);
extern uint8_t NutDnsGetHostsByName(CONST uint8_t * hostname, uint32_t * ip_all);
extern uint32_t NutDnsGetMxByDomain(CONST uint8_t * hostname);

__END_DECLS

#endif
