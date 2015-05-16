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
 * Revision 1.11  2008/08/28 11:07:45  haraldkipp
 * Make use of ioctl if the driver supports this.
 *
 * Revision 1.10  2008/08/11 07:00:29  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.9  2008/07/14 13:10:31  haraldkipp
 * Added macros to determine Ethernet address types.
 *
 * Revision 1.8  2005/04/30 16:42:42  chaac
 * Fixed bug in handling of NUTDEBUG. Added include for cfg/os.h. If NUTDEBUG
 * is defined in NutConf, it will make effect where it is used.
 *
 * Revision 1.7  2004/03/18 11:08:32  haraldkipp
 * Comments updated
 *
 * Revision 1.6  2004/03/18 10:49:59  haraldkipp
 * Comments updated
 *
 * Revision 1.5  2004/03/16 16:48:45  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 * Revision 1.4  2004/03/08 11:24:19  haraldkipp
 * Ugly PPP activation hack replaced by HDLC ioctl.
 *
 * Revision 1.3  2004/02/25 16:38:20  haraldkipp
 * Do not use zero MAC
 *
 * Revision 1.2  2004/02/02 18:54:22  drsung
 * gateway ip address was not set, if static network configuration from EEPROM is used.
 *
 * Revision 1.1.1.1  2003/05/09 14:41:29  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.24  2003/05/06 18:02:14  harald
 * PPP hack for simple UART support
 *
 * Revision 1.23  2003/04/21 17:02:22  harald
 * Added comments.
 * New PPP state machine implemented.
 * Auto config moved to DHCP.
 *
 * Revision 1.22  2003/03/31 12:03:15  harald
 * Adapt new PPP structure
 *
 * Revision 1.21  2003/02/04 18:14:57  harald
 * Version 3 released
 *
 * Revision 1.20  2003/01/14 16:43:40  harald
 * *** empty log message ***
 *
 * Revision 1.19  2002/11/02 15:18:27  harald
 * Library dependencies removed
 *
 * Revision 1.18  2002/10/31 16:08:09  harald
 * *** empty log message ***
 *
 * Revision 1.17  2002/10/29 15:29:17  harald
 * PPP support
 *
 * Revision 1.16  2002/07/03 16:41:32  harald
 * Bugfix: Failed to store gateway
 *
 * Revision 1.15  2002/06/26 17:29:36  harald
 * First pre-release with 2.4 stack
 *
 */

#include <cfg/os.h>
#include <string.h>
#include <io.h>

#include <dev/ppp.h>

#include <net/if.h>
#include <net/ether.h>
#include <net/route.h>
#include <arpa/inet.h>
#include <netinet/ppp_fsm.h>
#include <netinet/if_ether.h>
#include <netinet/igmp.h>
#include <netinet/in.h>

#include <sys/event.h>
#include <dev/uart.h>
#include <sys/confnet.h>

#include <pro/dhcp.h>

#ifdef NUTDEBUG
#include <sys/osdebug.h>
#include <net/netdebug.h>
#endif

/*!
 * \addtogroup xgIP
 */
/*@{*/

/*!
 * \brief Network interface setup.
 *
 * \param dev     Identifies the network device to setup.
 * \param ip_addr Specified IP address in network byte order.
 * \param ip_mask Specified IP network mask in network byte order.
 * \param gateway Optional default gateway.
 *
 * \return 0 on success, -1 otherwise.
 *
 * \note Typical applications do not use this function, but call 
 *       NutDhcpIfConfig() or NutNetIfConfig().
 */
int NutNetIfSetup(NUTDEVICE * dev, uint32_t ip_addr, uint32_t ip_mask, uint32_t gateway)
{
    IFNET *nif;

    nif = dev->dev_icb;

    /*
     * Use specified or previously used IP address.
     */
    if (ip_addr == 0 && (ip_addr = confnet.cdn_ip_addr) == 0)
        return -1;
    nif->if_local_ip = ip_addr;

    /*
     * Use specified or default mask.
     */
    if (ip_mask == 0)
        ip_mask = inet_addr("255.255.255.0");
    nif->if_mask = ip_mask;

    /*
     * Add routing entries.
     */
    NutIpRouteAdd(ip_addr & ip_mask, ip_mask, 0, dev);
    if (gateway)
        NutIpRouteAdd(0, 0, gateway, dev);

    /*
     * Save configuration in EEPROM.
     */
    memcpy(confnet.cd_name, dev->dev_name, sizeof(confnet.cd_name));
    /* Never save an invalid MAC address. */
    if (ETHER_IS_UNICAST(nif->if_mac)) {
        memcpy(confnet.cdn_mac, nif->if_mac, sizeof(nif->if_mac));
    }
    confnet.cdn_ip_addr = ip_addr;
    confnet.cdn_ip_mask = ip_mask;

    /*
     * Set gateway, if one was provided by the caller. Remove
     * gateway, if it's outside of our network.
     */
    if (gateway || (confnet.cdn_gateway & ip_mask) != (ip_addr & ip_mask))
        confnet.cdn_gateway = gateway;

    return NutNetSaveConfig();
}

/*!
 * \brief Configure a network interface.
 *
 * Devices must have been registered by NutRegisterDevice() before 
 * calling this function.
 *
 * For Ethernet devices applications may alternatively call 
 * NutDhcpIfConfig(), which allows automatic configuration by DHCP or 
 * the so called ARP method.
 *
 * \param name    Name of the device to configure.
 * \param params  Pointer to interface specific parameters. For Ethernet 
 *                interfaces this parameter may be a pointer to a buffer 
 *                containing the 6 byte long MAC address. This will
 *                override the MAC address stored in the non-volatile
 *                configuration memory. If this memory is uninitialized
 *                or not available, the MAC address must be specified. 
 *                For PPP interfaces this parameter is ignored and should
 *                be set to zero.
 * \param ip_addr Specified IP address in network byte order. This must 
 *                be a unique address within the Internet. If you do not 
 *                directly communicate with other Internet hosts, you can 
 *                use a locally assigned address. With PPP interfaces this
 *                may be set to 0.0.0.0, in which case the remote peer
 *                will be queried for an IP address.
 * \param ip_mask Specified IP network mask in network byte order.
 *                Typical Ethernet networks use 255.255.255.0, which 
 *                allows upto 254 hosts. For PPP interfaces 255.255.255.255
 *                is the default.
 *
 * \return 0 on success, -1 otherwise.
 *
 * \note The whole interface configuration has become a mess over
 *       the years and need a major redesign.
 */
int NutNetIfConfig(CONST char *name, void *params, uint32_t ip_addr, uint32_t ip_mask)
{
    return NutNetIfConfig2(name, params, ip_addr, ip_mask, 0);
}

/*!
 * \brief Configure a network interface including the default gateway.
 *
 * Devices must have been registered by NutRegisterDevice() before 
 * calling this function.
 *
 * For Ethernet devices applications may alternatively call 
 * NutDhcpIfConfig(), which allows automatic configuration by DHCP or 
 * the so called ARP method.
 *
 * \param name    Name of the device to configure.
 * \param params  Pointer to interface specific parameters.
 * \param ip_addr Specified IP address in network byte order. This must 
 *                be a unique address within the Internet. If you do not 
 *                directly communicate with other Internet hosts, you can 
 *                use a locally assigned address. With PPP interfaces this
 *                may be set to 0.0.0.0, in which case the remote peer
 *                will be queried for an IP address.
 * \param ip_mask Specified IP network mask in network byte order.
 *                Typical Ethernet networks use 255.255.255.0, which 
 *                allows upto 254 hosts. For PPP interfaces 255.255.255.255
 *                is the default.
 * \param gateway Specified IP address of gateway or next router in LAN.
 *                
 * \return 0 on success, -1 otherwise.
 *
 * \note I do not like this function, because setting a gateway should
 *       be handled by NutIpRouteAdd(). It's not yet deprecated, but I
 *       recommend not to use it in application code.
 */
int NutNetIfConfig2(CONST char *name, void *params, uint32_t ip_addr, uint32_t ip_mask, uint32_t gateway)
{
    NUTDEVICE *dev;
    IFNET *nif;

    /*
     * Check if this is a registered network device.
     */
    if ((dev = NutDeviceLookup(name)) == 0 || dev->dev_type != IFTYP_NET)
        return -1;

    /*
     * Setup Ethernet interfaces.
     */
    nif = dev->dev_icb;
    if (nif->if_type == IFT_ETHER) {
        /* Check if ioctl is supported. */
        if (dev->dev_ioctl) {
            uint32_t flags;

            /* Driver has ioctl, use it. */
            dev->dev_ioctl(dev, SIOCGIFFLAGS, &flags);
            dev->dev_ioctl(dev, SIOCSIFADDR, params);
            flags |= IFF_UP;
            dev->dev_ioctl(dev, SIOCSIFFLAGS, &flags);
        } else {
            /* No ioctl, set MAC address to start driver. */
            memcpy(nif->if_mac, params, sizeof(nif->if_mac));
        }
        return NutNetIfSetup(dev, ip_addr, ip_mask, gateway);
    }

    /*
     * Setup PPP interfaces.
     */
    else if (nif->if_type == IFT_PPP) {
        PPPDCB *dcb = dev->dev_dcb;

        /*
         * Set the interface's IP address, make sure that the state
         * change queue is empty and switch hardware driver into 
         * network mode.
         */
        dcb->dcb_local_ip = ip_addr;
        dcb->dcb_ip_mask = ip_mask ? ip_mask : 0xffffffff;
        NutEventBroadcast(&dcb->dcb_state_chg);
        _ioctl(dcb->dcb_fd, HDLC_SETIFNET, &dev);

        /*
         * Wait for network layer up and configure the interface on
         * success.
         */
        if (NutEventWait(&dcb->dcb_state_chg, 60000) == 0 && dcb->dcb_ipcp_state == PPPS_OPENED) {
            return NutNetIfSetup(dev, dcb->dcb_local_ip, dcb->dcb_ip_mask, dcb->dcb_remote_ip);
        }
    }
    return -1;
}

int NutNetIfAddMcastAddr(CONST char *name, uint32_t ip_addr)
{
    NUTDEVICE *dev;
    IFNET *nif;
    int rc = -1;

    /*
     * Check if this is a registered network device.
     */
    if ((dev = NutDeviceLookup(name)) == 0 || dev->dev_type != IFTYP_NET)
        return -1;

    /*
     * Setup multicast address
     */
    nif = dev->dev_icb;
    if (nif->if_type == IFT_ETHER) {
        /* Check if ioctl is supported. */
        if (dev->dev_ioctl) {
            /* Driver has ioctl, use it. */
            rc = dev->dev_ioctl(dev, SIOCADDMULTI, &ip_addr);
            if ((rc == 0) && (ip_addr != INADDR_ALLHOSTS_GROUP)) {
                NutIgmpJoinGroup(dev, ip_addr);
            }    
        }
    }

    return rc;
}

int NutNetIfDelMcastAddr(CONST char *name, uint32_t ip_addr)
{
    NUTDEVICE *dev;
    IFNET *nif;
    int rc = -1;

    /*
     * Check if this is a registered network device.
     */
    if ((dev = NutDeviceLookup(name)) == 0 || dev->dev_type != IFTYP_NET)
        return -1;

    /*
     * Setup multicast address
     */
    nif = dev->dev_icb;
    if (nif->if_type == IFT_ETHER) {
        /* Check if ioctl is supported. */
        if (dev->dev_ioctl) {
            /* Driver has ioctl, use it. */
            rc = dev->dev_ioctl(dev, SIOCDELMULTI, &ip_addr);
            if ((rc == 0) && (ip_addr != INADDR_ALLHOSTS_GROUP)) {
                NutIgmpLeaveGroup(dev, ip_addr);
            }    
        }
    }

    return rc;
}



/*@}*/
