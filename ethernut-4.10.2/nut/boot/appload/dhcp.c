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
 * Revision 1.2  2005/11/03 15:07:42  haraldkipp
 * New GCC versions warn about lvalue casts.
 * Some globals replaced by CONF structures.
 *
 * Revision 1.1  2003/11/03 16:19:38  haraldkipp
 * First release
 *
 */

#include "debug.h"
#include "dhcp.h"
#include "appload.h"

BOOTPHDR test;

/*!
 * \addtogroup xgDhcp
 */
/*@{*/


/*!
 * \brief Retrieve the specified DCHP option.
 *
 * \param opt  Option to look for.
 * \param ptr  Pointer to the buffer that receives the option value.
 * \param size Size of the buffer.
 *
 * \return Size of the retrieved option value or zero,
 *         if the specified option couldn't be found.
 */
u_char DhcpGetOption(u_char opt, void *ptr, u_char size)
{
    register u_char *cp;
    u_char i;

    cp = rframe.u.bootp.bp_options;
    for (;;) {
        if (*cp == DHCPOPT_PAD) {
            cp++;
            continue;
        }
        if (*cp == DHCPOPT_END)
            break;
        if (*cp == opt) {
            if (*(cp + 1) < size)
                size = *(cp + 1);
            for (i = 0; i < size; i++)
                *(((u_char *) ptr) + i) = *(cp + i + 2);
            return *(cp + 1);
        }
        cp++;
        cp += *cp;
        cp++;
    }
    return 0;
}

/*!
 * \brief Add specified DHCP option.
 *
 * \param dst  Pointer to the buffer that receives the option.
 * \param opt  Type of option to set.
 * \param src  Pointer to the buffer that contains the option value.
 * \param size Size of the option value.
 *
 * \return Pointer to the option buffer to receive the next option.
 */
u_char *DhcpSetOption(u_char * dst, u_char opt, u_char *src, u_char size)
{
    *dst++ = opt;
    *dst++ = size;
    while (size--)
        *dst++ = *((u_char *) src)++;
    *dst = DHCPOPT_END;

    return dst;
}

/*!
 * \brief Send DHCP datagram and wait for a specified answer.
 *
 * \param slen Length of message to send.
 * \param xtype Expected response type.
 *
 * \return The number of data bytes received, 0 on timeout or -1 in 
 *         case of a failure.
 */
int DhcpTransact(u_short slen, u_char xtype)
{
    u_char type;
    u_char retry;
    int rlen = 0;

    for (retry = 0; retry < 3; ) {
        /* 
         * Send a message, if nothing has been received yet.
         */
        if (rlen == 0) {
            if (UdpOutput(INADDR_BROADCAST, DHCP_SERVERPORT, DHCP_CLIENTPORT, slen) < 0) {
                /* Transmit failure, must be a NIC problem. Give up. */
                return -1;
            }
        }

        /* 
         * Do a retry on timouts or failures. A receive failures may be 
         * caused by a hardware failure or a bad frame.
         */
        if ((rlen = UdpInput(DHCP_CLIENTPORT, 5000)) <= 0) {
            retry++;
            continue;
        }

        /*
         * Check if the response contains the expected ID and
         * message type.
         */
        if (rframe.u.bootp.bp_xid == sframe.u.bootp.bp_xid && 
            DhcpGetOption(DHCPOPT_MSGTYPE, &type, 1) == 1 && 
            type == xtype) {
            DEBUG("[DHCP]");
            break;
        }
    }
    return rlen;
}

/*!
 * \brief Query any DHCP server on the local net.
 *
 * On success, this routine will fill some global
 * variables:
 *
 * - my_ip 
 * - server_ip
 * - bootfile
 * - my_netmask
 * 
 * \return 0 on success, -1 otherwise.
 */
int DhcpQuery(void)
{
    BOOTPHDR *bp;
    u_short slen;
    u_char i;
    u_long sid;
    register u_char *cp;

    /*
     * Nothing to do if we got a fixed IP address.
     */
    if (confnet.cdn_cip_addr) {
        confnet.cdn_ip_addr = confnet.cdn_cip_addr;
        return 0;
    }
    confnet.cdn_ip_addr = 0;

    /*
     * Setup bootp message.
     */
    bp = &sframe.u.bootp;
    bp->bp_op = 1;
    bp->bp_xid = *((u_long *)&confnet.cdn_mac[2]);
    bp->bp_htype = 1;
    bp->bp_hlen = sizeof(confnet.cdn_mac);
    memcpy_(bp->bp_chaddr, confnet.cdn_mac, 6);

    /*
     * Add DHCP option for discover message.
     */
    bp->bp_cookie = 0x63538263;
    i = DHCP_DISCOVER;
    DhcpSetOption(bp->bp_options, DHCPOPT_MSGTYPE, &i, 1);

    /*
     * Send DHCP discover and wait for any response.
     */
    slen = sizeof(BOOTPHDR) - sizeof(sframe.u.bootp.bp_options) + 4;
    if (DhcpTransact(slen, DHCP_OFFER) <= 0) {
        return -1;
    }

    /*
     * Get the server ID option.
     */
    DhcpGetOption(DHCPOPT_SID, &sid, 4);

    /*
     * Reuse the bootp structure and add DHCP options for request message.
     */
    DEBUGULONG(rframe.u.bootp.bp_yiaddr);
    i = DHCP_REQUEST;
    cp = DhcpSetOption(bp->bp_options, DHCPOPT_MSGTYPE, &i, 1);
    cp = DhcpSetOption(cp, DHCPOPT_REQUESTIP, (u_char *)&rframe.u.bootp.bp_yiaddr, 4);
    DhcpSetOption(cp, DHCPOPT_SID, (u_char *)&sid, 4);

    /*
     * Send DHCP request and wait for ACK.
     */
    slen = sizeof(BOOTPHDR) - sizeof(sframe.u.bootp.bp_options) + 16;
    if (DhcpTransact(slen, DHCP_ACK) <= 0) {
        return -1;
    }

    /*
     * Retrieve local IP, bootp server IP, bootfile name and netmask.
     */
    confnet.cdn_ip_addr = rframe.u.bootp.bp_yiaddr;
    confboot.cb_tftp_ip = rframe.u.bootp.bp_siaddr;
    for (cp = rframe.u.bootp.bp_file, i = 0; *cp && i < sizeof(confboot.cb_image) - 1; cp++, i++) {
        confboot.cb_image[i] = *cp;
    }
    confboot.cb_image[i] = 0;
    DhcpGetOption(DHCPOPT_NETMASK, &confnet.cdn_ip_mask, 4);

#if 0
    /*
     * I'd say that tftpd32 is buggy, because it sends siaddr
     * set to zero. This hack will fix it.
     */
    if (confboot.cb_tftp_ip == 0)
        confboot.cb_tftp_ip = rframe.ip_hdr.ip_src;
#endif

    return 0;
}

/*@}*/
