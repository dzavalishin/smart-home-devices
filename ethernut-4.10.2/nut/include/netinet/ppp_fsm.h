#ifndef _NETINET_PPP_FSM_H_
#define _NETINET_PPP_FSM_H_

/*
 * Copyright (C) 2001-2004 by egnite Software GmbH. All rights reserved.
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
 * Portions are
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
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgement:
 *
 *    This product includes software developed by Call Direct Cellular Solutions Pty. Ltd
 *    and its contributors.
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
 * Revision 1.4  2008/08/11 07:00:23  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.3  2008/04/01 10:14:04  haraldkipp
 * Bug #1902993 fixed. XCPOPT must be declared packed to work on 32-bit
 * targets.
 *
 * Revision 1.2  2004/03/08 11:19:55  haraldkipp
 * HDLC functions moved to async HDLC driver.
 *
 * Revision 1.1.1.1  2003/05/09 14:41:16  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.2  2003/05/06 18:46:46  harald
 * Cleanup
 *
 * Revision 1.1  2003/03/31 14:57:20  harald
 * *** empty log message ***
 *
 */

#include <stdint.h>

/*!
 * \file netinet/ppp_fsm.h
 * \brief PPP state machine definitions.
 */

/*
 * Link states.
 */
#define PPPS_INITIAL         0  /*!< \brief Down, hasn't been opened */
#define PPPS_STARTING        1  /*!< \brief Down, been opened */
#define PPPS_CLOSED          2  /*!< \brief Up, hasn't been opened */
#define PPPS_STOPPED         3  /*!< \brief Open, waiting for down event */
#define PPPS_CLOSING         4  /*!< \brief Terminating the connection, not open */
#define PPPS_STOPPING        5  /*!< \brief Peer is going down */
#define PPPS_REQSENT         6  /*!< \brief We've sent a Config Request */
#define PPPS_ACKRCVD         7  /*!< \brief We've received a Config Ack */
#define PPPS_ACKSENT         8  /*!< \brief We've sent a Config Ack */
#define PPPS_OPENED          9  /*!< \brief Connection available */

#define XCP_CONFREQ          1	/*!< \brief LCP/IPCP Configure-Request Code */
#define XCP_CONFACK          2	/*!< \brief LCP/IPCP Configure-Ack Code */
#define XCP_CONFNAK          3	/*!< \brief LCP/IPCP Configure-Nack Code */
#define XCP_CONFREJ          4	/*!< \brief LCP/IPCP Configure-Reject Code */
#define XCP_TERMREQ          5	/*!< \brief LCP/IPCP Terminate-Request Code */
#define XCP_TERMACK          6	/*!< \brief LCP/IPCP Terminate-Ack Code */
#define XCP_CODEREJ          7	/*!< \brief LCP/IPCP Code-Reject Code */
#define LCP_PROTREJ          8	/*!< \brief LCP Protocol-Reject Code */
#define LCP_ERQ              9	/*!< \brief LCP Echo-Request Code */
#define LCP_ERP             10	/*!< \brief LCP Echo-Response Code */
#define LCP_DRQ             11	/*!< \brief LCP Discard-Request Code */

/*
 * PAP client states.
 */
#define PAPCS_INITIAL        0  /*!< \brief Connection down */
#define PAPCS_CLOSED         1  /*!< \brief Connection up, haven't requested auth */
#define PAPCS_PENDING        2  /*!< \brief Connection down, have requested auth */
#define PAPCS_AUTHREQ        3  /*!< \brief We've sent an Authenticate-Request */
#define PAPCS_OPEN           4  /*!< \brief We've received an Ack */
#define PAPCS_BADAUTH        5  /*!< \brief We've received a Nak */

typedef struct __attribute__ ((packed)) {
    uint8_t xcpo_type;
    uint8_t xcpo_len;
    union {
        uint16_t us;
        uint32_t ul;
        uint8_t uc[1];
    } xcpo_;
} XCPOPT;


__BEGIN_DECLS

extern void LcpOpen(NUTDEVICE * dev);
extern void LcpClose(NUTDEVICE *dev);
extern void LcpLowerUp(NUTDEVICE *dev);
extern void LcpLowerDown(NUTDEVICE *dev);

extern void PapLowerUp(NUTDEVICE *dev);
extern void PapLowerDown(NUTDEVICE *dev);

extern void IpcpOpen(NUTDEVICE * dev);
extern void IpcpClose(NUTDEVICE * dev);
extern void IpcpLowerUp(NUTDEVICE *dev);
extern void IpcpLowerDown(NUTDEVICE *dev);

extern void PppOpen(NUTDEVICE *dev);
extern void PppClose(NUTDEVICE *dev);

extern int NutPppInitStateMachine(NUTDEVICE * dev);

__END_DECLS

#endif

