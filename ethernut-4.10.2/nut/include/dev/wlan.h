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
#ifndef __WLAN_H__
#define __WLAN_H__


#include <dev/netbuf.h>
#include <net/if_var.h>

/*
 * Available drivers.
*/
extern NUTDEVICE devWlan;

/*-------------------------------------------------------------------------*/
/* global defines                                                          */
/*-------------------------------------------------------------------------*/
#ifndef DEV_ETHER
#define DEV_ETHER   devWlan
#endif

/*
 * Networkname
*/ 
#define WLAN_NETWORK_NAME_MAX_LEN 32

/*
 * WEP
 */
#define WLAN_WEP_MAX_KEY_COUNT    4
#define WLAN_WEP_MAX_KEY_SIZE     16

/*-------------------------------------------------------------------------*/
/* global types                                                            */
/*-------------------------------------------------------------------------*/

/*
 * IOCTL commands
 */ 
typedef enum {
  WLAN_IOCTL_GET_MAC_ADDRESS = 1,
  
  WLAN_IOCTL_SET_CONFIG,
  WLAN_IOCTL_GET_STATUS  

} WLAN_IOCTL;

/*
 * WLAN mode
*/
typedef enum {
  WLAN_MODE_STOP = 0,
  WLAN_MODE_STATION,
  WLAN_MODE_ADHOC
} WLAN_MODE; 

/*
 * Port status
 */
typedef enum {
  WLAN_STATUS_PORT_DISABLE   = 1,
  WLAN_STATUS_PORT_SEARCHING = 2,
  WLAN_STATUS_PORT_CONN_IBSS = 3,
  WLAN_STATUS_PORT_CONN_ESS  = 4,
  WLAN_STATUS_PORT_OOR_ESS   = 5
} WLAN_STATUS_PORT;

/*
 * WLAN_WEP
*/
typedef enum {
  WLAN_USE_NO_WEP     = 0,
  WLAN_USE_64BIT_WEP  = 5,
  WLAN_USE_128BIT_WEP = 13  
} WLAN_WEP;

typedef struct _wlan_wep_key {
  uint8_t  KeyLen;
  uint8_t  Key[WLAN_WEP_MAX_KEY_SIZE];
} WLAN_WEP_KEY;

typedef struct _wlan_config {
  uint16_t       Size;
  WLAN_MODE     Mode;
  char          Networkname[WLAN_NETWORK_NAME_MAX_LEN];
  WLAN_WEP      UseWEP;
  uint8_t        UseWEPTxKey;
  WLAN_WEP_KEY  WEPKey[WLAN_WEP_MAX_KEY_COUNT];
} WLAN_CONFIG;

typedef struct _wlan_status {
  uint8_t            Size;
  WLAN_STATUS_PORT  PortStatus;       // Current port status
  uint8_t            BSSIDAddress[6];  // MAC address of the AP?
  uint8_t            Channel;          // Current Channel
  uint8_t            TxRate;           // Current TxRate
  uint16_t           Quality;          // Quality in dBm
  uint16_t           Signal;           // Signal in dBm negative value
  uint16_t           Noise;            // Noise in dBm negative value
} WLAN_STATUS;

/*-------------------------------------------------------------------------*/
/* global macros                                                           */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* Prototypes                                                              */
/*-------------------------------------------------------------------------*/

/*
 * Driver routines.
*/
extern int WlanInit(NUTDEVICE * dev);
extern int WlanIOCtl(NUTDEVICE * dev, int req, void *conf);
extern int WlanOutput(NUTDEVICE * dev, NETBUF * nb);

#endif /* !__WLAN_H__ */
