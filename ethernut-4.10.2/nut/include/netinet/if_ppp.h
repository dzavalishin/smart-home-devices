#ifndef _NETINET_IF_PPP_H_
#define _NETINET_IF_PPP_H_

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
 * Copyright (C) 2002 by Call Direct Cellular Solutions Pty. Ltd. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY CALL DIRECT CELLULAR SOLUTIONS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL CALL DIRECT
 * CELLULAR SOLUTIONS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * For additional information see http://www.calldirect.com.au/
 * -
 *
 * Portions are 
 * Copyright (c) 1989 by Carnegie Mellon University.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by Carnegie Mellon University.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

/*
 * $Log$
 * Revision 1.4  2008/08/11 07:00:22  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.3  2004/01/30 11:38:25  haraldkipp
 * Handle magic number rejects
 *
 * Revision 1.2  2003/08/14 15:05:18  haraldkipp
 * Caller will do ID increment
 *
 * Revision 1.1.1.1  2003/05/09 14:41:14  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.2  2003/05/06 18:46:33  harald
 * Cleanup
 *
 * Revision 1.1  2003/03/31 14:57:20  harald
 * *** empty log message ***
 *
 */

#include <sys/types.h>
#include <net/if_var.h>

/*!
 * \file netinet/if_ppp.h
 * \brief PPP interface definitions.
 *
 */

#define MAX_NAK_LOOPS   10
#define MIN_LCPMRU     128

#define PPP_MRU     1500


/*
 * Protocol field values.
 */
#define PPP_IP          0x0021  /* Internet Protocol */
#define PPP_AT          0x0029  /* AppleTalk Protocol */
#define PPP_IPX         0x002b  /* IPX protocol */
#define PPP_VJC_COMP    0x002d  /* VJ compressed TCP */
#define PPP_VJC_UNCOMP  0x002f  /* VJ uncompressed TCP */
#define PPP_IPV6        0x0057  /* Internet Protocol Version 6 */
#define PPP_COMP        0x00fd  /* compressed packet */
#define PPP_IPCP        0x8021  /* IP Control Protocol */
#define PPP_ATCP        0x8029  /* AppleTalk Control Protocol */
#define PPP_IPXCP       0x802b  /* IPX Control Protocol */
#define PPP_IPV6CP      0x8057  /* IPv6 Control Protocol */
#define PPP_CCP         0x80fd  /* Compression Control Protocol */
#define PPP_LCP         0xc021  /* Link Control Protocol */
#define PPP_PAP         0xc023  /* Password Authentication Protocol */
#define PPP_LQR         0xc025  /* Link Quality Report protocol */
#define PPP_CHAP        0xc223  /* Cryptographic Handshake Auth. Protocol */
#define PPP_CBCP        0xc029  /* Callback Control Protocol */


/*
 * LCP options.
 */
#define LCP_MRU           1     /*!< \brief Maximum Receive Unit */
#define LCP_ASYNCMAP      2     /*!< \brief Async Control Character Map */
#define LCP_AUTHTYPE      3     /*!< \brief Authentication Type */
#define LCP_QUALITY       4     /*!< \brief Quality Protocol */
#define LCP_MAGICNUMBER   5     /*!< \brief Magic Number */
#define LCP_PCOMPRESSION  7     /*!< \brief Protocol Field Compression */
#define LCP_ACCOMPRESSION 8     /*!< \brief Address/Control Field Compression */

/*
 * IPCP Options.
 */
#define IPCP_ADDRS        1       /* IP Addresses */
#define IPCP_COMPRESSTYPE 2       /* Compression Type */
#define IPCP_ADDR         3       /* IP Address */

#define IPCP_MS_DNS1      129     /* Primary DNS value */
#define IPCP_MS_WINS1     130     /* Primary WINS value */
#define IPCP_MS_DNS2      131     /* Secondary DNS value */
#define IPCP_MS_WINS2     132     /* Secondary WINS value */

/*!
 * \typedef PPPHDR
 * \brief PPP header type.
 */
typedef struct ppp_header {
    uint8_t address;		/*!< \brief HDLC Address. */
    uint8_t control;		/*!< \brief HDLC Control. */
    uint16_t prot_type;		/*!< \brief Protocol type. */
} PPPHDR;

typedef struct xcphdr {
    /*!< \brief Code. 
     */
    uint8_t  xch_code;

    /*!< \brief Identifier 
     */
    uint8_t  xch_id;

    /*!< \brief Length 
     */
    uint16_t xch_len;
} XCPHDR;


extern void NutLcpInput(NUTDEVICE * dev, NETBUF * nb);
extern int NutLcpOutput(NUTDEVICE * dev, uint8_t code, uint8_t id, NETBUF * nb);
extern void LcpTxConfReq(NUTDEVICE *dev, uint8_t id, uint8_t rejected);
extern void LcpTxProtRej(NUTDEVICE *dev, uint16_t protocol, NETBUF *nb);

extern void NutPapInput(NUTDEVICE * dev, NETBUF * nb);
extern int NutPapOutput(NUTDEVICE * dev, uint8_t code, uint8_t id, NETBUF * nb);
extern void PapTxAuthReq(NUTDEVICE *dev, uint8_t id);

extern void NutIpcpInput(NUTDEVICE * dev, NETBUF * nb);
extern int NutIpcpOutput(NUTDEVICE * dev, uint8_t code, uint8_t id, NETBUF * nb);
extern void IpcpTxConfReq(NUTDEVICE *dev, uint8_t id);

#endif

