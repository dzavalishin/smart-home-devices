#ifndef _NETINET_IF_ETHER_H_
#define _NETINET_IF_ETHER_H_

/*
 * Copyright (C) 2008 by egnite GmbH.
 * Copyright (C) 2001-2003 by egnite Software GmbH.
 * Copyright (c) 1983, 1993 by The Regents of the University of California.
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
 */

/*
 * $Id: if_ether.h 2238 2008-08-11 17:38:27Z haraldkipp $
 */

#include <net/if_var.h>
#include <net/if_arp.h>

/*!
 * \file netinet/if_ether.h
 * \brief Ethernet interface definitions.
 */

/*! \brief Length of an Ethernet address. */
#define	ETHER_ADDR_LEN      6

/*! \brief Length of the Ethernet type field. */
#define	ETHER_TYPE_LEN      2

/*! \brief Length of the Ethernet CRC. */
#define	ETHER_CRC_LEN       4

/*! \brief Length of an Ethernet header. */
#define	ETHER_HDR_LEN       (ETHER_ADDR_LEN + ETHER_ADDR_LEN + ETHER_TYPE_LEN)

#ifndef ETHER_MIN_LEN
/*! \brief Minimum frame length, including CRC. */
#define	ETHER_MIN_LEN       64
#endif

#ifndef ETHER_MAX_LEN
/*! \brief Maximum frame length, including CRC. */
#define	ETHER_MAX_LEN       1518
#endif

/*!
 * \struct ether_header if_ether.h netinet/if_ether.h
 * \brief Ethernet protocol header.
 */
/*!
 * \typedef ETHERHDR
 * \brief Ethernet protocol header type.
 */
typedef struct __attribute__((packed)) ether_header {
    /*! \brief Destination MAC address. */
    uint8_t  ether_dhost[ETHER_ADDR_LEN];
    /*! \brief Source MAC address. */
    uint8_t  ether_shost[ETHER_ADDR_LEN];
    /*! \brief Protocol type. */
    uint16_t ether_type;
} ETHERHDR;

/*! \brief Ethernet maximum transfer unit.
 *
 * Must be checked by the hardware driver.
 */
#define ETHERMTU    (ETHER_MAX_LEN - ETHER_HDR_LEN - ETHER_CRC_LEN)

/*! \brief Ethernet minimum transfer unit. 
 *
 * Must be checked by the hardware driver.
 */
#define ETHERMIN    (ETHER_MIN_LEN - ETHER_HDR_LEN - ETHER_CRC_LEN)

#define ETHERTYPE_IP    0x0800  /*!< \brief IP protocol */
#define ETHERTYPE_ARP   0x0806  /*!< \brief Address resolution protocol */

/*! 
 * \brief Determine if a given Ethernet address is zero.
 *
 * \param ea Pointer to a character array containing the address.
 *
 * Return 1 if the address is zero. Otherwise 0 is returned.
 */
#define	ETHER_IS_ZERO(ea) (((ea)[0] | (ea)[1] | (ea)[2] | (ea)[3] | (ea)[4] | (ea)[5]) == 0)

/*! 
 * \brief Determine if a given Ethernet address is a broadcast address.
 *
 * \param ea Pointer to a character array containing the address.
 *
 * Return 1 if the address is a broadcast address. Otherwise 0 is returned.
 */
#define	ETHER_IS_BROADCAST(ea) (((ea)[0] & (ea)[1] & (ea)[2] & (ea)[3] & (ea)[4] & (ea)[5]) == 0xFF)

/*! 
 * \brief Determine if a given Ethernet address is a multicast address.
 *
 * The broadcast address is defined as a special multicast address.
 *
 * \param ea Pointer to a character array containing the address.
 *
 * Return 1 if the address is a multicast address. Otherwise 0 is returned.
 */
#define	ETHER_IS_MULTICAST(ea) ((ea)[0] & 1) 

/*! 
 * \brief Determine if a given Ethernet address is a unicast address.
 *
 * By definition, an address with all zeros is not a valid unicast address.
 *
 * \param ea Pointer to a character array containing the address.
 *
 * Return 1 if the address is a unicast address. Otherwise 0 is returned.
 */
#define	ETHER_IS_UNICAST(ea) (!ETHER_IS_ZERO(ea) && !ETHER_IS_MULTICAST(ea)) 

__BEGIN_DECLS
/* ASCII conversion function prototypes. */
extern uint8_t *ether_aton(CONST char *str);
extern char *ether_ntoa(CONST uint8_t *mac);

__END_DECLS

/*!
 * \struct ether_arp if_ether.h netinet/if_ether.h
 * \brief Ethernet ARP protocol structure.
 *
 * See RFC 826 for protocol description.
 */
/*!
 * \typedef ETHERARP
 * \brief Ethernet ARP protocol type.
 */
typedef struct __attribute__((packed)) ether_arp {
    ARPHDR   ea_hdr;        /*!< \brief Fixed-size header. */
    uint8_t  arp_sha[6];    /*!< \brief Source hardware address. */
    uint32_t arp_spa;       /*!< \brief Source protocol address. */
    uint8_t  arp_tha[6];    /*!< \brief Target hardware address. */
    uint32_t arp_tpa;       /*!< \brief Target protocol address. */
} ETHERARP;

__BEGIN_DECLS
/* ARP function prototypes. */
extern void NutArpInput(NUTDEVICE *dev, NETBUF *nb);
extern NETBUF *NutArpAllocNetBuf(uint16_t type, uint32_t ip, uint8_t *mac);
extern int NutArpOutput(NUTDEVICE *dev, NETBUF *nb);
extern void NutArpCacheUpdate(NUTDEVICE *dev, uint32_t ip, uint8_t *ha);
extern int NutArpCacheQuery(NUTDEVICE *dev, uint32_t ip, uint8_t *mac);

__END_DECLS
/* */

#endif
