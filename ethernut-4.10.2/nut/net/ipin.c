/*
 * Copyright (C) 2008 by egnite GmbH.
 * Copyright (C) 2001-2007 by egnite Software GmbH.
 * Copyright (c) 1983, 1993 by The Regents of the University of California.
 * Copyright (c) 1993 by Digital Equipment Corporation.
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
 */

/*
 * $Log$
 * Revision 1.15  2008/10/05 16:48:52  haraldkipp
 * Security fix. Check various lengths of incoming packets.
 *
 * Revision 1.14  2008/08/20 06:57:00  haraldkipp
 * Implemented IP demultiplexer.
 *
 * Revision 1.13  2008/08/11 07:00:30  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.12  2008/04/18 13:13:11  haraldkipp
 * Using fast ints.
 *
 * Revision 1.11  2007/07/09 16:20:19  olereinhardt
 * 2007-07-09  Ole Reinhardt <ole.reinhardt@embedded-it.de>
 *         * net/Makefile: Commented in igmp_in.c and igmp_out.c again
 *         * net/ipin.c: igmp support reenabled
 *
 * Revision 1.10  2007/07/09 15:59:00  olereinhardt
 * 2007-07-09  Ole Reinhardt <ole.reinhardt@embedded-it.de>
 * 	* ipin.c: commented out igmp support as long as it does not compile
 *
 * Revision 1.9  2007/05/02 11:18:32  haraldkipp
 * IGMP support added. Incomplete.
 *
 * Revision 1.8  2007/03/23 12:43:50  haraldkipp
 * ARP method wasn't actually disabled by default. Fixed.
 *
 * Revision 1.7  2006/09/05 12:35:39  haraldkipp
 * DHCP servers may probe an IP/MAC relationship by sending an
 * ICMP request. This triggered the Nut/Net ARP method and
 * terminated the DHCP client, leaving the system with default
 * configurations of the network mask (255.255.255.0) and
 * default gateway (none). The rarely used ARP method is now
 * disabled by default.
 *
 * Revision 1.6  2006/07/10 17:46:59  haraldkipp
 * Now really like Jan suggested to fix it.
 *
 * Revision 1.5  2006/07/10 08:49:47  haraldkipp
 * Do not respond to broadcasts with unknown protocols. Many thanks to Jan.
 *
 * Revision 1.4  2005/06/05 16:48:26  haraldkipp
 * Additional parameter enables NutUdpInput() to avoid responding to UDP
 * broadcasts with ICMP unreachable messages. Fixes bug #1215192.
 *
 * Revision 1.3  2004/12/16 18:48:50  haraldkipp
 * Added Damian Slee's IP filter function.
 *
 * Revision 1.2  2004/02/02 18:59:25  drsung
 * Some more ICMP support added.
 *
 * Revision 1.1.1.1  2003/05/09 14:41:32  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.16  2003/05/06 18:14:18  harald
 * Allow incoming DHCP telegrams, even if not broadcasted
 *
 * Revision 1.15  2003/03/31 12:26:05  harald
 * Accept masks with all bits set
 *
 * Revision 1.14  2003/02/04 18:14:57  harald
 * Version 3 released
 *
 * Revision 1.13  2002/06/26 17:29:36  harald
 * First pre-release with 2.4 stack
 *
 */

#include <cfg/ip.h>

#include <net/route.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/icmp.h>
#include <netinet/ip_icmp.h>
#include <netinet/igmp.h>
#include <netinet/udp.h>
#include <sys/socket.h>
#include <arpa/inet.h>

/*!
 * \addtogroup xgIP
 */
/*@{*/

static NutIpFilterFunc NutIpFilter;

/*!
 * \brief Set filter function for incoming IP datagrams.
 *
 * The callbackFunc is called by the IP layer on every incoming IP 
 * datagram. Thus it must not block. The implementer returns 0 for 
 * allow, -1 for deny.
 *
 * It is recommended to set the filer after DHCP has done its thing,
 * just in case your DHCP server is on a different subnet for example.
 *
 * \param callbackFunc Pointer to callback function to filter IP packets.
 *                     Set to 0 to disable the filter again.
 */
void NutIpSetInputFilter(NutIpFilterFunc callbackFunc)
{
    NutIpFilter = callbackFunc;
}

/*!
 * \brief Pointer to an optional demultiplexer.
 *
 * This pointer will be set on the first call to NutRegisterIpHandler().
 */
int (*ip_demux) (NUTDEVICE *, NETBUF *);

/*!
 * \brief Process incoming IP datagrams.
 *
 * Datagrams addressed to other destinations and datagrams 
 * whose version number is not 4 are silently discarded.
 *
 * \note This routine is called by the Ethernet layer on
 *       incoming IP datagrams. Applications typically do
 *       not call this function.
 *
 * \param dev Identifies the device that received this datagram.
 * \param nb  The network buffer received.
 */
void NutIpInput(NUTDEVICE * dev, NETBUF * nb)
{
    IPHDR *ip;
    uint_fast8_t hdrlen;
    uint32_t dst;
    uint_fast8_t bcast;
    IFNET *nif;

    ip = nb->nb_nw.vp;

    /*
     * Silently discard datagrams of different IP version as well as
     * fragmented or filtered datagrams.
     */
    if (ip->ip_v != IPVERSION ||        /* Version check. */
        (ntohs(ip->ip_off) & (IP_MF | IP_OFFMASK)) != 0 ||      /* Fragmentation. */
        (NutIpFilter && NutIpFilter(ip->ip_src))) {     /* Filter. */
        NutNetBufFree(nb);
        return;
    }

    /*
    ** IP header length is given in 32-bit fields. Calculate the size in
    ** bytes and make sure that the header we know will fit in. Check
    ** further, that the header length is not larger than the bytes we
    ** received.
    */
    hdrlen = ip->ip_hl * 4;
    if (hdrlen < sizeof(IPHDR) || hdrlen > nb->nb_nw.sz) {
        NutNetBufFree(nb);
        return;
    }

#if NUT_IP_INCHKSUM
    /* Optional checksum calculation on incoming datagrams. */
#endif

    /*
     * Check for broadcast.
     */
    dst = ip->ip_dst;
    nif = dev->dev_icb;

    if (dst == INADDR_BROADCAST || 
        (nif->if_local_ip && nif->if_mask != INADDR_BROADCAST && (dst | nif->if_mask) == INADDR_BROADCAST)) {
        bcast = 1;
    }

    /*
     * Check for multicast.
     */
    else if (IN_MULTICAST(dst)) {
        MCASTENTRY *mca;

        for (mca = nif->if_mcast; mca; mca = mca->mca_next) {
            if (dst == mca->mca_ip) {
                break;
            }
        }
		if (mca == NULL) {
            NutNetBufFree(nb);
			return;
		}
        bcast = 2;
    }

    /*
     * Packet is unicast.
     */
    else {
        bcast = 0;
        nb->nb_flags |= NBAF_UNICAST;

#ifdef NUTIPCONF_ICMP_ARPMETHOD
        /*
         * Silently discard datagrams for other destinations.
         * However, if we haven't got an IP address yet, we
         * allow ICMP datagrams to support dynamic IP ARP method,
         * if this option had been enabled.
         */
        if (nif->if_local_ip == 0 && ip->ip_p == IPPROTO_ICMP && (dst & 0xff000000) != 0xff000000 && (dst & 0xff000000) != 0) {
            NutNetIfSetup(dev, dst, 0, 0);
        }
#endif
        if (nif->if_local_ip && (dst == 0 || dst != nif->if_local_ip)) {
            NutNetBufFree(nb);
            return;
        }
    }

    /* Check the IP data length. */
    nb->nb_tp.sz = htons(ip->ip_len);
    if (nb->nb_tp.sz < hdrlen || nb->nb_tp.sz > nb->nb_nw.sz) {
        NutNetBufFree(nb);
        return;
    }
    nb->nb_nw.sz = hdrlen;
    nb->nb_tp.sz -= hdrlen;
    if (nb->nb_tp.sz) {
        nb->nb_tp.vp = ((char *) ip) + hdrlen;
    }

    if (ip_demux == NULL || (*ip_demux) (dev, nb)) {
        switch (ip->ip_p) {
        case IPPROTO_ICMP:
            NutIcmpInput(dev, nb);
            break;
        case IPPROTO_IGMP:
            NutIgmpInput(dev, nb);
            break;
        default:
            /* Unkown protocol, send ICMP destination (protocol)
            * unreachable message.
            */
            if (bcast || !NutIcmpResponse(ICMP_UNREACH, ICMP_UNREACH_PROTOCOL, 0, nb))
                NutNetBufFree(nb);
            break;
        }
    }
}

/*@}*/
