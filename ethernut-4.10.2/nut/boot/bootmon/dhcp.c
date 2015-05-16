/*
 * Copyright (c) 1983, 1993 by The Regents of the University of California
 * Copyright (C) 2001-2007 by egnite Software GmbH
 * Copyright (C) 2010 by egnite GmbH
 *
 * All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
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
 * $Id: dhcp.c 2954 2010-04-03 11:22:40Z haraldkipp $
 */

#include "dialog.h"
#include "dhcp.h"
#include "bootmon.h"

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
unsigned char DhcpGetOption(unsigned char opt, void *ptr, unsigned char size)
{
    register unsigned char *cp;
    unsigned char i;

    cp = rframe.eth.udp.u.bootp.bp_options;
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
                *(((unsigned char *) ptr) + i) = *(cp + i + 2);
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
unsigned char *DhcpSetOption(unsigned char *dst, unsigned char opt, unsigned char *src, unsigned char size)
{
    *dst++ = opt;
    *dst++ = size;
    while (size--)
        *dst++ = *src++;
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
int DhcpTransact(unsigned short slen, unsigned char xtype)
{
    unsigned char type;
    unsigned char retry;
    int rlen = 0;

    for (retry = 0; retry < 3;) {
        /* 
         * Send a message, if nothing has been received yet.
         */
        DEBUG("[RQ DHCP]");
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
        if ((rlen = UdpInput(DHCP_CLIENTPORT, 500)) <= 0) {
            retry++;
            continue;
        }

        /*
         * Check if the response contains the expected ID and
         * message type.
         */
        if (rframe.eth.udp.u.bootp.bp_xid == sframe.eth.udp.u.bootp.bp_xid && DhcpGetOption(DHCPOPT_MSGTYPE, &type, 1) == 1 && type == xtype) {
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
    unsigned short slen;
    unsigned char i;
    unsigned long sid;
    register unsigned char *cp;

    /*
     * Nothing to do if we got a fixed IP address.
     */
    if (confnet.cdn_cip_addr) {
        DEBUG("DHCP skipped\n");
        confnet.cdn_ip_addr = confnet.cdn_cip_addr;
        return 0;
    }
    confnet.cdn_ip_addr = 0;

    /*
     * Setup bootp message.
     */
    bp = &sframe.eth.udp.u.bootp;
    bp->bp_op = 1;
    bp->bp_xid = random_id;
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
    slen = sizeof(BOOTPHDR) - sizeof(sframe.eth.udp.u.bootp.bp_options) + 4;
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
    DEBUGULONG(rframe.eth.udp.u.bootp.bp_yiaddr);
    i = DHCP_REQUEST;
    cp = DhcpSetOption(bp->bp_options, DHCPOPT_MSGTYPE, &i, 1);
    cp = DhcpSetOption(cp, DHCPOPT_REQUESTIP, (unsigned char *) &rframe.eth.udp.u.bootp.bp_yiaddr, 4);
    DhcpSetOption(cp, DHCPOPT_SID, (unsigned char *) &sid, 4);

    /*
     * Send DHCP request and wait for ACK.
     */
    slen = sizeof(BOOTPHDR) - sizeof(sframe.eth.udp.u.bootp.bp_options) + 16;
    if (DhcpTransact(slen, DHCP_ACK) <= 0) {
        return -1;
    }

    /*
     * Retrieve local IP, bootp server IP, bootfile name and netmask.
     */
    confnet.cdn_ip_addr = rframe.eth.udp.u.bootp.bp_yiaddr;
    if (confboot.cb_tftp_ip == 0) {
        confboot.cb_tftp_ip = rframe.eth.udp.u.bootp.bp_siaddr;
    }
    if (confboot.cb_image[0] == 0) {
        for (cp = (unsigned char *)rframe.eth.udp.u.bootp.bp_file, i = 0; *cp && i < sizeof(confboot.cb_image) - 1; cp++, i++) {
            confboot.cb_image[i] = *cp;
        }
        confboot.cb_image[i] = 0;
    }
    DhcpGetOption(DHCPOPT_NETMASK, &confnet.cdn_ip_mask, 4);

    return 0;
}

/*@}*/
