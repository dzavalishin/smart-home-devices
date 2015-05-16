#ifndef _DEV_PPP_H_
#define _DEV_PPP_H_

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
 * -
 *
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
 */

/*
 * $Log$
 * Revision 1.6  2008/08/11 06:59:59  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.5  2005/08/02 17:46:48  haraldkipp
 * Major API documentation update.
 *
 * Revision 1.4  2004/03/14 10:14:42  haraldkipp
 * Unused prototypes removed
 *
 * Revision 1.3  2003/08/14 15:05:55  haraldkipp
 * Distinguish negotiated and final magic
 *
 * Revision 1.2  2003/07/24 16:13:58  haraldkipp
 * Never request a rejected DNS
 *
 * Revision 1.1.1.1  2003/05/09 14:41:08  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.2  2003/05/06 18:43:30  harald
 * Cleanup
 *
 * Revision 1.1  2003/03/31 14:53:23  harald
 * Prepare release 3.1
 *
 */

#include <sys/device.h>
#include <dev/netbuf.h>

/*!
 * \file dev/ppp.h
 * \brief PPP device definitions.
 */

/*
 * Reject flags. Not completed, more will follow.
 */
#define REJ_IPCP_DNS1       0x00000100
#define REJ_IPCP_DNS2       0x00000200

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \addtogroup xgPPP
 */
/*@{*/

#define PPP_PFC     0x01
#define PPP_ACFC    0x02

#define LCP_DEFOPT_ASYNCMAP 0x000A0000UL

/*!
 * \brief PPP interface type.
 */
typedef struct _PPPDCB PPPDCB;

/*!
 * \struct _PPPDCB ppp.h dev/ppp.h
 * \brief PPP interface structure.
 *
 * Contains information about the PPP device.
 */
struct _PPPDCB {
    /*! \brief PPP State event queue.
     * The state machine posts an event to this queue when the
     * network interface becomes ready.
     */
    HANDLE dcb_state_chg;	

    /*! \brief Current state of the link layer.
     */
    uint8_t dcb_lcp_state;
    
    /*! \brief ID of our last request.
     */
    uint8_t dcb_reqid;

    /*! \brief ID of our last reject.
     */
    uint8_t dcb_rejid;

    /*! \brief Flag to remember ack received.
     */
    uint8_t dcb_acked;

    /*! \brief Maximum local receive unit.
     */
    uint16_t dcb_loc_mru;

    /*! \brief Maximum remote receive unit.
     */
    uint16_t dcb_rem_mru;

    /*! \brief Asynch control character map.
     * TODO: Pass to ahdlc driver.
     */
    uint32_t dcb_accm;

    /*! \brief Compression flags.
     * TODO: Pass to ahdlc driver. dcb_useACFC dcb_usePFC
     */
    uint8_t dcb_compr;
    
    /*! \brief Authentication type.
     */
    uint16_t dcb_auth;

    /*! \brief Magic number to negotiate.
     */
    uint32_t dcb_neg_magic;

    /*! \brief Local magic number.
     * Used to detect loopback conditions.
     */
    uint32_t dcb_loc_magic;

    /*! \brief Remote magic number.
     * Used to detect loopback conditions.
     */
    uint32_t dcb_rem_magic;

    /*! \brief User authentication.
     */
    uint8_t *dcb_user;

    /*! \brief Password authentication.
     */
    uint8_t *dcb_pass;

    /*! \brief Local IP address.
     */
    uint32_t dcb_local_ip;

    /*! \brief Remote IP address.
     */
    uint32_t dcb_remote_ip;

    /*! \brief Negotiated IP mask.
     */
    uint32_t dcb_ip_mask;

    /*! \brief Negotiated primary DNS.
     */
    uint32_t dcb_ip_dns1;

    /*! \brief Negotiated secondary DNS.
     */
    uint32_t dcb_ip_dns2;

    /*! \brief Rejected items.
     */
    uint32_t dcb_rejects;

    /*! \brief LCP/IPCP retransmit counter.
     */
    uint8_t dcb_retries;

    /*! \brief LCP NAK counter, avoids endless loops. 
     */
    uint8_t dcb_lcp_naks;

    /*! \brief Identifier of our last LCP request.
     */
    uint8_t dcb_lcp_reqid;

    /*! \brief Current state of the network layer.
     */
    uint8_t dcb_ipcp_state;
    
    /*! \brief LCP NAK counter, avoids endless loops. 
     */
    uint8_t dcb_ipcp_naks;

    /*! \brief Current authentication state.
     */
    uint8_t dcb_auth_state;

    /*! \brief File descriptor of physical device.
     */
    int dcb_fd;
};

/*
 * Available devices.
 */
extern NUTDEVICE devPpp;

#define LCP_OPEN        1
#define LCP_CLOSE       2
#define LCP_LOWERUP     3
#define LCP_LOWERDOWN   4

/*@}*/

#ifdef __cplusplus
}
#endif

#endif
