/*!
 * Copyright (C) 2002 by Call Direct Cellular Solutions Pty. Ltd. All rights reserved.
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
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgement:
 *
 *    This product includes software developed by Call Direct Cellular Solutions Pty. Ltd.
 *    and its contributors.
 *
 * THIS SOFTWARE IS PROVIDED BY CALL DIRECT CELLULAR SOLUTIONS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL CALL DIRECT
 * CELLULAR SOLUTIONS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * For additional information see http://www.calldirect.com.au/
 *-
*
 * Portions Copyright (C) 2001 by egnite Software GmbH. All rights reserved.
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
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgement:
 *
 *    This product includes software developed by egnite Software GmbH
 *    and its contributors.
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
 * Revision 1.4  2008/08/28 11:12:15  haraldkipp
 * Added interface flags, which will be required to implement Ethernet ioctl
 * functions.
 *
 * Revision 1.3  2007/05/02 11:22:51  haraldkipp
 * Added multicast table entry.
 *
 * Revision 1.2  2005/08/02 17:46:45  haraldkipp
 * Major API documentation update.
 *
 * Revision 1.1  2005/07/26 18:02:27  haraldkipp
 * Moved from dev.
 *
 * Revision 1.3  2003/08/05 20:05:11  haraldkipp
 * DNS removed from interface
 *
 * Revision 1.2  2003/07/20 20:07:38  haraldkipp
 * Conflicting Ethernet driver routine names solved.
 *
 * Revision 1.1  2003/07/20 16:37:21  haraldkipp
 * CrystalTek 8900A driver added.
 *
 * Revision 0.1  2002/05/02 CDCS MJC
 * Created
 *
 */

#include <netinet/if_ether.h>
#include <net/ether.h>
#include <net/if_var.h>
#include <dev/nicrtl.h>


NICINFO dcb_eth0cs;

extern int CSNicOutput(NUTDEVICE * dev, NETBUF * nb);
extern int CSNicInit(NUTDEVICE * dev);

/*
 * \brief Network interface information structure.
 *
 * Used to call.
 */
IFNET ifn_eth0cs = {
    IFT_ETHER,                  /*!< \brief Interface type. */
    0,                          /*!< \brief Interface flags, if_flags. */
    {0, 0, 0, 0, 0, 0},         /*!< \brief Hardware net address. */
    0,                          /*!< \brief IP address. */
    0,                          /*!< \brief Remote IP address for point to point. */
    0,                          /*!< \brief IP network mask. */
    567,                        /*!< \brief Maximum size of a transmission unit. */
    0,                          /*!< \brief Packet identifier. */
    0,                          /*!< \brief Linked list of arp entries. */
    0,                          /*!< \brief Linked list of multicast address entries, if_mcast. */
    NutEtherInput,              /*!< \brief Routine to pass received data to, if_recv(). */
    CSNicOutput,                /*!< \brief Driver output routine, if_send(). */
    NutEtherOutput              /*!< \brief Media output routine, if_output(). */
};

/*
 * \brief Device information structure.
 *
 * Applications must pass this structure to NutRegisterDevice() 
 * to bind this Ethernet device driver to the Nut/OS kernel.
 * Having done that, the application may call NutNetIfConfig()
 * with the name \em eth0 of this driver to initialize the network
 * interface.
 * 
 */
NUTDEVICE devEth0cs = {
    0,                          /*!< Pointer to next device. */
    {'e', 't', 'h', '0', 'c', 's', 0, 0, 0}
    ,                           /*!< Unique device name. */
    IFTYP_NET,                  /*!< Type of device. */
    0,                          /*!< Base address. */
    0,                          /*!< First interrupt number. */
    &ifn_eth0cs,                /*!< Interface control block. */
    &dcb_eth0cs,                /*!< Driver control block. */
    CSNicInit,                  /*!< Driver initialization routine. */
    0,                          /*!< Driver specific control function. */
    0,                          /*!< Read from device. */
    0,                          /*!< Write to device. */
    0,                          /*!< Write from program space data to device. */
    0,                          /*!< Open a device or file. */
    0,                          /*!< Close a device or file. */
    0                           /*!< Request file size. */
};

