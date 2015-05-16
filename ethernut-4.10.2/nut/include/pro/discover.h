#ifndef _PRO_DISCOVER_H
#define _PRO_DISCOVER_H

/*
 * Copyright (C) 2001-2006 by egnite Software GmbH. All rights reserved.
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

/*!
 * \file pro/discover.h
 * \brief Responds to UDP discovery broadcasts.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.4  2009/01/17 11:26:51  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.3  2009/01/16 17:03:50  haraldkipp
 * Configurable discovery protocol version and port plus
 * configurable service thread stack size. The new version 1.1
 * allows host names up to 98 characters. Added some code
 * to make sure, that nothing is overwritten with version 1.0
 * protocol and too long host names. Protocol version 1.0
 * is still the default.
 *
 * Revision 1.2  2008/08/11 07:00:23  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.1  2006/09/07 09:06:17  haraldkipp
 * Discovery service added.
 *
 *
 * \endverbatim
 */


#include <sys/types.h>
#include <cfg/discover.h>
#include <stdint.h>

#ifndef DISCOVERY_VERSION
#define DISCOVERY_VERSION   0x10
#endif

#define DIST_REQUEST    0
#define DIST_ANNOUNCE   1
#define DIST_APPLY      2

#define DISF_INITAL_ANN     0x0001

typedef struct __attribute__ ((packed)) _DISCOVERY_TELE {
    uint32_t dist_xid;            /*!< \brief Exchange identifier. */
    uint8_t dist_type;           /*!< \brief Message type, 0=request, 1=announce, 2=apply. */
    uint8_t dist_ver;            /*!< \brief Telegram version. */
    uint8_t dist_mac[6];         /*!< \brief Ethernet MAC address. */
    uint32_t dist_ip_addr;        /*!< \brief Last used IP address. */
    uint32_t dist_ip_mask;        /*!< \brief IP netmask. */
    uint32_t dist_gateway;        /*!< \brief Default route. */
    uint32_t dist_cip_addr;       /*!< \brief Configured IP address. */
#if DISCOVERY_VERSION <= 0x10
    uint8_t dist_hostname[8];    /*!< \brief Host name of the system. */
    uint8_t dist_custom[92];     /*!< \brief Bootfile to request. */
#else
    uint8_t dist_appendix[100];  /*! \brief Host name with lenght byte in front. */
#endif
} DISCOVERY_TELE;

typedef int (*NutDiscoveryCallback) (uint32_t, uint16_t, DISCOVERY_TELE *, int);

extern int NutRegisterDiscovery(uint32_t ipmask, uint16_t port, unsigned int flags);
extern NutDiscoveryCallback NutRegisterDiscoveryCallback(NutDiscoveryCallback func);

extern int NutDiscoveryAnnTele(DISCOVERY_TELE *dist);
extern int NutDiscoveryAppConf(DISCOVERY_TELE *dist);

#endif
