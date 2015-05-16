#ifndef _NET_IF_VAR_H_
#define _NET_IF_VAR_H_

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
 */

/*
 * $Log$
 * Revision 1.8  2008/08/28 11:12:15  haraldkipp
 * Added interface flags, which will be required to implement Ethernet ioctl
 * functions.
 *
 * Revision 1.7  2008/08/11 07:00:20  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.6  2007/05/02 11:22:51  haraldkipp
 * Added multicast table entry.
 *
 * Revision 1.5  2005/02/04 15:08:37  haraldkipp
 * ARP redesign requires removal flag.
 *
 * Revision 1.4  2004/03/18 11:17:16  haraldkipp
 * Comments updated
 *
 * Revision 1.3  2004/02/02 18:53:49  drsung
 * gateway ip address was not set, if static network configuration from EEPROM is used.
 *
 * Revision 1.2  2003/08/05 20:19:13  haraldkipp
 * DNS removed from interface
 *
 * Revision 1.1.1.1  2003/05/09 14:41:12  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.10  2003/03/31 14:33:26  harald
 * Moved some declarations to ppp device
 *
 * Revision 1.9  2003/02/04 18:00:42  harald
 * Version 3 released
 *
 * Revision 1.8  2002/10/29 15:40:49  harald
 * *** empty log message ***
 *
 * Revision 1.7  2002/06/26 17:29:18  harald
 * First pre-release with 2.4 stack
 *
 */

#include <sys/types.h>
#include <stdint.h>

#include <sys/device.h>
#include <dev/netbuf.h>
#include <net/if_types.h>

/*!
 * \file net/if_var.h
 * \brief Network interface structure.
 *
 * Each network device in Nut/Net has an associated
 * network interface structure.
 */

/*!
 * \addtogroup xgARP
 */
/*@{*/

#define ATF_REM     0x01        /*!< \brief Entry marked for removal. */
#define ATF_COM     0x02        /*!< \brief Completed entry. */
#define ATF_PERM    0x04        /*!< \brief Permanent entry. */

/*!
 * \brief ARP entry type.
 */
typedef struct _ARPENTRY ARPENTRY;


/*!
 * \struct _ARPENTRY if_var.h net/if_var.h
 * \brief ARP entry structure.
 *
 * Each network interface maintains its own ARP table.
 */
struct _ARPENTRY {
    ARPENTRY *ae_next;          /*!< \brief Linked list chain. */
    uint32_t ae_ip;               /*!< \brief IP address. */
    uint8_t ae_ha[6];            /*!< \brief Hardware address. */
    uint8_t ae_flags;            /*!< \brief Status flags, permanent and completed. */
    uint8_t ae_outdated;         /*!< \brief Minutes since last use. */
    HANDLE ae_tq;               /*!< \brief Threads waiting for entry to be completed. */
};

/*@}*/

/*!
 * \addtogroup xgIP
 */
/*@{*/

/*!
 * \brief Multicast address entry type.
 */
typedef struct _MCASTENTRY MCASTENTRY;

/*!
 * \struct _MCASTENTRY if_var.h net/if_var.h
 * \brief Multicast entry structure.
 */
struct _MCASTENTRY {
    MCASTENTRY *mca_next;
    uint8_t mca_ha[6];
    uint32_t mca_ip;
};

#define	SIOCSIFMTU      0x1001  /*!< \brief Set maximum transfer unit. */
#define	SIOCGIFMTU      0x1002  /*!< \brief Get maximum transfer unit. */
#define	SIOCSIFFLAGS    0x1003  /*!< \brief Set interface flags. */
#define	SIOCGIFFLAGS    0x1004  /*!< \brief Get interface flags. */
#define	SIOCSIFADDR     0x1005  /*!< \brief Set interface address. */
#define	SIOCGIFADDR     0x1006  /*!< \brief Get interface address. */
#define	SIOCADDMULTI    0x1007  /*!< \brief Add multicast address.*/
#define	SIOCDELMULTI    0x1008  /*!< \brief Delete multicast address. */

/*!
 * \brief Network interface type.
 */
typedef struct ifnet IFNET;

/*!
 * \struct ifnet if_var.h net/if_var.h
 * \brief Network interface structure.
 *
 * Contains information about the network interface.
 */
struct ifnet {
    uint8_t if_type;        /*!< \brief Interface type.
                            *  Either IFT_ETHER or IFT_PPP.
                            */
    uint32_t if_flags;      /*!< \brief Interface flags. */
    uint8_t if_mac[6];      /*!< \brief Hardware net address. */
    uint32_t if_local_ip;   /*!< \brief IP address. */
    uint32_t if_remote_ip;  /*!< \brief Remote IP address for point to point. */
    uint32_t if_mask;       /*!< \brief IP network mask. */
    uint16_t if_mtu;        /*!< \brief Maximum size of a transmission unit. */
    uint16_t if_pkt_id;     /*!< \brief Packet identifier. */
    ARPENTRY *arpTable;     /*!< \brief Linked list of arp entries. */
    MCASTENTRY *if_mcast;   /*!< \brief Linked list of multicast address entries. */
    void (*if_recv) (NUTDEVICE *, NETBUF *);	/*!< \brief Receive routine. */
    int (*if_send) (NUTDEVICE *, NETBUF *);		/*!< \brief Send routine. */
    int (*if_output) (NUTDEVICE *, uint16_t, uint8_t *, NETBUF *);  /*!< \brief Media output routine. */
    int (*if_ioctl) (NUTDEVICE *, int, void *); /*!< \brief Interface specific control function. */
};

/*@}*/

__BEGIN_DECLS

extern int NutNetIfConfig2(CONST char *name, void *mac_dev, uint32_t ip_addr,
                          uint32_t ip_mask, uint32_t gateway);
extern int NutNetIfConfig(CONST char *name, void *mac_dev, uint32_t ip_addr,
                          uint32_t ip_mask);
extern int NutNetIfSetup(NUTDEVICE * dev, uint32_t ip_addr, uint32_t ip_mask,
                         uint32_t gateway);
extern int NutNetIfAddMcastAddr(CONST char *name, uint32_t ip_addr);
extern int NutNetIfDelMcastAddr(CONST char *name, uint32_t ip_addr);


extern int NutNetLoadConfig(CONST char *name);
extern int NutNetSaveConfig(void);

__END_DECLS

#endif
