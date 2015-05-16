#ifndef _SYS_CONFNET_H_
#define _SYS_CONFNET_H_

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
 * \file sys/confnet.h
 * \brief Header file for network configuration.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.6  2008/10/05 16:47:04  haraldkipp
 * Removed attribute 'packed' from typedef.
 *
 * Revision 1.5  2008/08/11 07:00:25  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.4  2006/05/25 09:17:28  haraldkipp
 * Allow configuration of location used in non-volatile memory.
 * API documentation updated and corrected.
 *
 * Revision 1.3  2006/03/16 15:25:34  haraldkipp
 * Changed human readable strings from uint8_t to char to stop GCC 4 from
 * nagging about signedness.
 *
 * Revision 1.2  2006/01/23 17:34:29  haraldkipp
 * Configuration structures must be packed.
 *
 * Revision 1.1.1.1  2003/05/09 14:41:18  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.4  2003/02/04 18:00:52  harald
 * Version 3 released
 *
 * Revision 1.3  2002/06/26 17:29:28  harald
 * First pre-release with 2.4 stack
 *
 * \endverbatim
 */

#include <sys/types.h>
#include <cfg/eeprom.h>

#include <stdint.h>

/*!
 * \addtogroup xgConfNet
 */
/*@{*/

/*!
 * \brief Non-volatile memory location.
 *
 * Offset into non-volatile memory, where Nut/Net stores the network
 * configuration. The default may be overridden by the Configurator.
 */
#ifndef CONFNET_EE_OFFSET
#define CONFNET_EE_OFFSET   64
#endif

#ifndef CONFNET_MAX_IF
#define CONFNET_MAX_IF      1
#endif

/*!
 * \brief Network configuration type.
 */
typedef struct _CONFNET CONFNET;

/*!
 * \struct _CONFNET confnet.h sys/confnet.h
 * \brief Network configuration structure.
 *
 * Applications may directly access the global variable \ref confnet to
 * read or modify the current network configuration.
 */
struct __attribute__ ((packed)) _CONFNET {
    /*! \brief Size of this structure.
     *
     * Used by Nut/Net to verify, that the structure contents is valid
     * after reading it from non-volatile memory.
     */
    uint8_t cd_size;

    /*! \brief Magic cookie.
     *
     * Contains the device name of the network interface.
     */
    char cd_name[9];

    /*! \brief Ethernet MAC address.
     *
     * Unique Ethernet address of the network interface.
     */
    uint8_t cdn_mac[6];

    /*! \brief Last used IP address. 
     *
     * Each time Nut/Net receives an IP address during boot, it
     * will store the address in here.
     *
     * If no fixed IP address has been configured (cdn_cip_addr
     * contains 0.0.0.0) and if no DHCP server is available, then
     * Nut/Net will use this one, if it is not 0.0.0.0.
     */
    uint32_t cdn_ip_addr;

    /*! \brief IP netmask.
     *
     * The netmask is used to determine which machines are
     * available in the local network.
     */
    uint32_t cdn_ip_mask;

    /*! \brief Default route. 
     *
     * Nut/Net will redirect IP packets to this node, if the
     * target IP is not located in the local network.
     */
    uint32_t cdn_gateway;

    /*! \brief Configured IP address. 
     *
     * If this address is set to 0.0.0.0, Nut/Net will try
     * to obtain one from the DHCP server.
     */
    uint32_t cdn_cip_addr;
};

extern CONFNET confnet;

/*@}*/

#endif
