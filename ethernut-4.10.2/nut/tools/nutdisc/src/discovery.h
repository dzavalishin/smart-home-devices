#ifndef _DISCOVERY_H_
#define _DISCOVERY_H_

/* ----------------------------------------------------------------------------
 * Copyright (C) 2009-2011 by egnite GmbH
 * Copyright (C) 2005-2006 by egnite Software GmbH
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * ----------------------------------------------------------------------------
 */

/*
 * $Id: discovery.h 3841 2012-02-10 18:19:35Z haraldkipp $
 */

#define DISCOVERY_VERSION_1_0   0x10
#define DISCOVERY_VERSION_1_1   0x11

#define DIST_REQUEST    0
#define DIST_ANNOUNCE   1
#define DIST_APPLY      2

#define DISCOVERY_PORT  9806

#pragma pack(1)
typedef struct {
    unsigned long dist_xid;            /*!< \brief Exchange identifier. */
    unsigned char dist_type;           /*!< \brief Message type, 0=request, 1=announce, 2=apply. */
    unsigned char dist_ver;            /*!< \brief Telegram version. */
    unsigned char dist_mac[6];         /*!< \brief Ethernet MAC address. */
    unsigned long dist_ip_addr;        /*!< \brief Last used IP address. */
    unsigned long dist_ip_mask;        /*!< \brief IP netmask. */
    unsigned long dist_gateway;        /*!< \brief Default route. */
    unsigned long dist_cip_addr;       /*!< \brief Configured IP address. */
    unsigned char dist_appendix[100];  /*!< \brief Additional settings, starting with host name. */
} DISCOVERY_TELE;
#pragma pack()

#endif
