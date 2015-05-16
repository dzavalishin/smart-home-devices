/****************************************************************************
*  This file is part of the WLAN-Ethernut device driver.
*
*  Copyright (c) 2004 by Michael Fischer. All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*  
*  1. Redistributions of source code must retain the above copyright 
*     notice, this list of conditions and the following disclaimer.
*  2. Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in the 
*     documentation and/or other materials provided with the distribution.
*  3. Neither the name of the author nor the names of its contributors may 
*     be used to endorse or promote products derived from this software 
*     without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL 
*  THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS 
*  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
*  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
*  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF 
*  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
*  SUCH DAMAGE.
*
****************************************************************************
*  History:
*
*  27.01.04  mifi   First Version
****************************************************************************/
#define __WLAN_C__

#include <compiler.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include <sys/heap.h>
#include <sys/timer.h>

#include <netinet/if_ether.h>
#include <net/ether.h>
#include <net/if_var.h>

#include <dev/wlantypes.h>
#include <dev/wlan.h>
#include <dev/wlandrv.h>

/*==========================================================*/
/*  DEFINE: All Structures and Common Constants             */
/*==========================================================*/
#define ERROR   -1
#define OK      0

/*==========================================================*/
/*  DEFINE: Prototypes                                      */
/*==========================================================*/

/*==========================================================*/
/*  DEFINE: Prototypes                                      */
/*==========================================================*/

/*
 * Network interface information structure.
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
    WlanOutput,                 /*!< \brief Driver output routine, if_send(). */
    NutEtherOutput              /*!< \brief Media output routine, if_output(). */
};

/*==========================================================*/
/*  DEFINE: Definition of all global Data                   */
/*==========================================================*/

/*
 * Device information structure.
 *
 * A pointer to this structure must be passed to NutRegisterDevice() 
 * to bind this Ethernet device driver to the Nut/OS kernel.
 * An application may then call NutNetIfConfig() with the name \em eth0 
 * of this driver to initialize the network interface.
 * 
*/
NUTDEVICE devWlan = {
    0,                          /* Pointer to next device. */
    {'w', 'l', 'a', 'n', '0', 0, 0, 0, 0},      /* Unique device name. */
    IFTYP_NET,                  /* Type of device. */
    0,                          /* Base address. */
    0,                          /* First interrupt number. */
    &ifn_eth0,                  /* Interface control block. */
    0,                          /* Driver control block. */
    WlanInit,                   /* Driver initialization routine. */
    WlanIOCtl,                  /* Driver specific control function. */
    0,                          /* Read from device. */
    0,                          /* Write to device. */
    0,                          /* Write from program space data to device. */
    0,                          /* Open a device or file. */
    0,                          /* Close a device or file. */
    0                           /* Request file size. */
};

/*==========================================================*/
/*  DEFINE: Definition of all local Procedures              */
/*==========================================================*/

/*==========================================================*/
/*  DEFINE: All code exported                               */
/*==========================================================*/
/************************************************************/
/*  WlanInit                                                */
/*                                                          */
/*  Initialize Ethernet hardware.                           */
/*                                                          */
/*  Resets the WLAN Ethernet controller,                    */
/*  initializes all required hardware registers and starts  */
/*  a background thread for incoming Ethernet traffic.      */
/*                                                          */
/*  Applications should do not directly call this function. */
/*  It is automatically executed during device registration */
/*  by NutRegisterDevice().                                 */
/*                                                          */
/*  In    : dev Identifies the device to initialize         */
/*  Out   : none                                            */
/*  Return: 0 on success, -1 in case of any errors          */
/************************************************************/
int WlanInit(NUTDEVICE * dev)
{
    int nError = ERROR;

    /*
     * Check if this is the first call. 
     * Only one init allowed.
     */
    if (dev->dev_dcb == NULL) {
        dev->dev_dcb = NutHeapAlloc(sizeof(WI_SOFTC));
        /*
         * Have we got the memory ?
         */
        if (dev->dev_dcb != NULL) {
            /*
             * Clear NICINFO structure.
             */
            memset(dev->dev_dcb, 0, sizeof(WI_SOFTC));

            /*
             * Check if a card is available
             */
            nError = wlandrv_ProbeDevice();
            if (nError == OK) {
                /*
                 * Check if we can attach the card
                 */
                if (wlandrv_Attach(dev) != OK) {
                    nError = ERROR;
                } else {
                    /*
                     * Now we have all the information,
                     * and can start the device.
                     */
                    wlandrv_Init(dev);
                }               /* endif wi_attach(dev->dev_dcb) != OK */
            }                   /* HardwareReset() */
        }                       /* endif dev->dev_dcb != NULL */
    }                           /* check first call, dev->dev_dcb == NULL */
    return (nError);
}

/************************************************************/
/*  WlanIOCtl                                               */
/*                                                          */
/*  Perform WLAN control functions.                         */
/*                                                          */
/*  reg May be set to one of the following constants:       */
/*                                                          */
/*  In    : dev  Identifies the device to use.              */
/*          reg  Requested control function.                */
/*          conf Points to a buffer that contains any data  */
/*               required for the given control function or */
/*               receives data from that function.          */
/*  Out   : none                                            */
/*  Return: 0 on success, -1 in case of any errors          */
/************************************************************/
int WlanIOCtl(NUTDEVICE * dev, int req, void *conf)
{
    int nError;

    nError = wlandrv_IOCTL(dev, req, conf);

    return (nError);
}

/************************************************************/
/*  WlanOutput                                              */
/*                                                          */
/*  Send Ethernet packet.                                   */
/*                                                          */
/*  In    : dev  Identifies the device to use.              */
/*          nb   Network buffer structure containing the    */
/*               packet to be sent. The structure must have */
/*               been allocated by a previous call          */
/*               NutNetBufAlloc().                          */
/*  Out   : none                                            */
/*  Return: 0 on success, -1 in case of any errors          */
/************************************************************/
int WlanOutput(NUTDEVICE * dev, NETBUF * nb)
{
    int nError = ERROR;

    nError = wlandrv_PutPacket(dev, nb);

    return (nError);
}
