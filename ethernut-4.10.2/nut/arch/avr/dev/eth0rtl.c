/*
 * Copyright (C) 2001-2003 by egnite Software GmbH. All rights reserved.
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
 */

/*
 * $Log$
 * Revision 1.3  2008/08/28 11:12:15  haraldkipp
 * Added interface flags, which will be required to implement Ethernet ioctl
 * functions.
 *
 * Revision 1.2  2007/05/02 11:22:51  haraldkipp
 * Added multicast table entry.
 *
 * Revision 1.1  2005/07/26 18:02:27  haraldkipp
 * Moved from dev.
 *
 * Revision 1.3  2003/11/03 17:06:52  haraldkipp
 * Allow linking with lanc111
 *
 * Revision 1.2  2003/08/05 20:05:11  haraldkipp
 * DNS removed from interface
 *
 * Revision 1.1.1.1  2003/05/09 14:40:37  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.11  2003/03/31 14:53:06  harald
 * Prepare release 3.1
 *
 * Revision 1.10  2003/02/04 17:50:54  harald
 * Version 3 released
 *
 * Revision 1.9  2002/10/29 12:46:00  harald
 * PPP support added
 *
 * Revision 1.8  2002/06/26 17:29:08  harald
 * First pre-release with 2.4 stack
 *
 */

#include <netinet/if_ether.h>
#include <net/ether.h>
#include <net/if_var.h>
#include <dev/nicrtl.h>

/*!
 * \addtogroup xgEth0Dev
 */
/*@{*/

static NICINFO dcb_eth0;

/*!
 * \brief Network interface information structure.
 *
 * Used to call.
 */
static IFNET ifn_eth0 = {
    IFT_ETHER,                  /*!< \brief Interface type. */
    0,                          /*!< \brief Interface flags, if_flags. */
    {0, 0, 0, 0, 0, 0},         /*!< \brief Hardware net address. */
    0,                          /*!< \brief IP address. */
    0,                          /*!< \brief Remote IP address for point to point. */
    0,                          /*!< \brief IP network mask. */
    ETHERMTU,                   /*!< \brief Maximum size of a transmission unit. */
    0,                          /*!< \brief Packet identifier. */
    0,                          /*!< \brief Linked list of arp entries. */
    0,                          /*!< \brief Linked list of multicast address entries, if_mcast. */
    NutEtherInput,              /*!< \brief Routine to pass received data to, if_recv(). */
    NicOutput,                  /*!< \brief Driver output routine, if_send(). */
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
NUTDEVICE devEth0 = {
    0,                          /* Pointer to next device. */
    {'e', 't', 'h', '0', 0, 0, 0, 0, 0},        /* Unique device name. */
    IFTYP_NET,	    /* Type of device. */
    0,		    /* Base address. */
    0,		    /* First interrupt number. */
    &ifn_eth0,	    /* Interface control block. */
    &dcb_eth0,	    /* Driver control block. */
    NicInit,	    /* Driver initialization routine. */
    0,		    /* Driver specific control function. */
    0,		    /* Read from device. */
    0,		    /* Write to device. */ 
    0,		    /* Write from program space data to device. */
    0,		    /* Open a device or file. */
    0,		    /* Close a device or file. */
    0		    /* Request file size. */
};

/*@}*/
