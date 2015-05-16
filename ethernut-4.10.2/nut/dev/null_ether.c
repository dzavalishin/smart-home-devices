/*
 * Copyright (C) 2009 by egnite GmbH
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
 * $Id$
 */

#include <cfg/os.h>

#include <net/if_var.h>
#include <net/ether.h>
#include <netinet/if_ether.h>

#include <dev/null_ether.h>

/*!
 * \addtogroup xgNullEther
 */
/*@{*/


/*!
 * \brief Send Ethernet packet.
 *
 * This null devices silently discards all outgoing packets.
 *
 * \param dev Identifies the device to use.
 * \param nb  Network buffer structure containing the packet to be sent.
 *            The structure must have been allocated by a previous
 *            call NutNetBufAlloc().
 *
 * \return Always 0.
 */
int NullEtherOutput(NUTDEVICE * dev, NETBUF * nb)
{
    return 0;
}

/*!
 * \brief Initialize Ethernet hardware.
 *
 * \param dev Identifies the device to initialize.
 *
 * \return Always 0.
 */
int NullEtherInit(NUTDEVICE * dev)
{
    return 0;
}

/*!
 * \brief Network interface information structure.
 *
 * Used to call.
 */
static IFNET ifn_eth0 = {
    IFT_ETHER,                  /*!< \brief Interface type, if_type. */
    0,                          /*!< \brief Interface flags, if_flags. */
    {0, 0, 0, 0, 0, 0},         /*!< \brief Hardware net address, if_mac. */
    0,                          /*!< \brief IP address, if_local_ip. */
    0,                          /*!< \brief Remote IP address for point to point, if_remote_ip. */
    0,                          /*!< \brief IP network mask, if_mask. */
    ETHERMTU,                   /*!< \brief Maximum size of a transmission unit, if_mtu. */
    0,                          /*!< \brief Packet identifier, if_pkt_id. */
    0,                          /*!< \brief Linked list of arp entries, arpTable. */
    0,                          /*!< \brief Linked list of multicast address entries, if_mcast. */
    NutEtherInput,              /*!< \brief Routine to pass received data to, if_recv(). */
    NullEtherOutput,            /*!< \brief Driver output routine, if_send(). */
    NutEtherOutput              /*!< \brief Media output routine, if_output(). */
};

/*!
 * \brief Device information structure.
 *
 * A pointer to this structure must be passed to NutRegisterDevice() 
 * to bind this Ethernet device driver to the Nut/OS kernel.
 * An application may then call NutNetIfConfig() with the name \em eth0 
 * of this driver to initialize the network interface.
 * 
 */
NUTDEVICE devNullEther = {
    0,                          /*!< \brief Pointer to next device. */
    {'e', 't', 'h', '0', 0, 0, 0, 0, 0},        /*!< \brief Unique device name. */
    IFTYP_NET,                  /*!< \brief Type of device. */
    0,                          /*!< \brief Base address. */
    0,                          /*!< \brief First interrupt number. */
    &ifn_eth0,                  /*!< \brief Interface control block. */
    NULL,                       /*!< \brief Driver control block. */
    NullEtherInit,              /*!< \brief Driver initialization routine. */
    0,                          /*!< \brief Driver specific control function. */
    0,                          /*!< \brief Read from device. */
    0,                          /*!< \brief Write to device. */
#ifdef __HARVARD_ARCH__
    0,                          /*!< \brief Write from program space data to device. */
#endif
    0,                          /*!< \brief Open a device or file. */
    0,                          /*!< \brief Close a device or file. */
    0                           /*!< \brief Request file size. */
};

/*@}*/
