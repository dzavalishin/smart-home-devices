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
*  Portions Copyright: 
*
*  Copyright (c) 2001 Atsushi Onoe
*  Copyright (c) 2002, 2003 Sam Leffler, Errno Consulting
*  Copyright (c) 2002 M Warner Losh <imp@freebsd.org>.  All rights reserved.
*  Copyright (c) 1997, 1998, 1999
*	 Bill Paul <wpaul@ctr.columbia.edu>.  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*  1. Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*  2. Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in the
*     documentation and/or other materials provided with the distribution.
*  3. Neither the name of the author nor the names of any co-contributors
*     may be used to endorse or promote products derived from this software
*     without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY Bill Paul AND CONTRIBUTORS ``AS IS'' AND
*  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
*  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
*  ARE DISCLAIMED.  IN NO EVENT SHALL Bill Paul OR THE VOICES IN HIS HEAD
*  BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
*  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
*  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
*  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
*  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
*  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
*  THE POSSIBILITY OF SUCH DAMAGE.
*
****************************************************************************
*  History:
*
*  28.02.04  mifi   First Version 
*                   This file is a mix of several FreeBSD header files.
*                   I have put all the files together, because it was
*                   difficult to use the original files. I have try this
*                   but I must comment out to much to get it to work.
*                   The other "define" file is wlandef.h, 
*                   here you will the rest of it.
*  03.03.04  mifi   Move debug switch to wlancfg.h
****************************************************************************/
#ifndef __WLANDRV_H__
#define __WLANDRV_H__

#include <compiler.h>
#include <dev/wlantypes.h>
#include <dev/wlan.h>
#include <dev/wlandef.h>

/*-------------------------------------------------------------------------*/
/* global defines and types                                                */
/*-------------------------------------------------------------------------*/

/*
 * Resetport definition
*/
#define RESET_EN_PORT           DDRD
#define RESET_PORT		          PORTD
#define RESET_BIT	 	            0x20

/***************************************************************************/
/*                           if_wivar.h                                    */
/***************************************************************************/

/*
 * Driver Flags
*/
#define	WI_FLAGS_ATTACHED       0x0001
#define	WI_FLAGS_INITIALIZED    0x0002
#define	WI_FLAGS_OUTRANGE       0x0004
#define	WI_FLAGS_HAS_MOR        0x0010
#define	WI_FLAGS_HAS_ROAMING    0x0020
#define	WI_FLAGS_HAS_DIVERSITY  0x0040
#define	WI_FLAGS_HAS_SYSSCALE   0x0080
#define	WI_FLAGS_BUG_AUTOINC    0x0100
#define	WI_FLAGS_HAS_FRAGTHR    0x0200
#define	WI_FLAGS_HAS_DBMADJUST  0x0400

/*
 * TX Buffer count
*/ 
#define	WI_NTXBUF               3

/***************************************************************************/
/*                          ieee80211.h                                    */
/***************************************************************************/

#define IEEE80211_ADDR_LEN      6
#define	IEEE80211_NWID_LEN      32


#define	IEEE80211_WEP_KEYLEN    5 /* 40bit */
#define	IEEE80211_WEP_IVLEN     3	/* 24bit */
#define	IEEE80211_WEP_KIDLEN    1	/* 1 octet */
#define	IEEE80211_WEP_CRCLEN    4	/* CRC-32 */
#define	IEEE80211_WEP_NKID      4	/* number of key ids */

#define	IEEE80211_CRC_LEN       4


/***************************************************************************/
/*                          ieee80211_var.h                                */
/***************************************************************************/

/*
 * ic_flags 
*/
#define	IEEE80211_F_ASCAN       0x00000001  /* STATUS: active scan */
#define	IEEE80211_F_SIBSS       0x00000002  /* STATUS: start IBSS */
#define	IEEE80211_F_WEPON       0x00000100  /* CONF: WEP enabled */
#define	IEEE80211_F_IBSSON      0x00000200  /* CONF: IBSS creation enable */
#define	IEEE80211_F_PMGTON      0x00000400  /* CONF: Power mgmt enable */
#define	IEEE80211_F_DESBSSID    0x00000800  /* CONF: des_bssid is set */
#define	IEEE80211_F_SCANAP      0x00001000  /* CONF: Scanning AP */
#define	IEEE80211_F_ROAMING     0x00002000  /* CONF: roaming enabled */
#define	IEEE80211_F_SWRETRY     0x00004000  /* CONF: sw tx retry enabled */
#define	IEEE80211_F_TXPMGT      0x00018000  /* STATUS: tx power */
#define IEEE80211_F_TXPOW_OFF   0x00000000  /* TX Power: radio disabled */
#define IEEE80211_F_TXPOW_FIXED 0x00008000  /* TX Power: fixed rate */
#define IEEE80211_F_TXPOW_AUTO  0x00010000  /* TX Power: undefined */
#define	IEEE80211_F_SHSLOT      0x00020000  /* CONF: short slot time */
#define	IEEE80211_F_SHPREAMBLE  0x00040000  /* CONF: short preamble */

/*
 * ic_caps
*/
#define	IEEE80211_C_WEP         0x0001  /* CAPABILITY: WEP available  */
#define	IEEE80211_C_IBSS        0x0002  /* CAPABILITY: IBSS available */
#define	IEEE80211_C_PMGT        0x0004  /* CAPABILITY: Power mgmt     */
#define	IEEE80211_C_HOSTAP      0x0008  /* CAPABILITY: HOSTAP avail   */
#define	IEEE80211_C_AHDEMO      0x0010  /* CAPABILITY: Old Adhoc Demo */
#define	IEEE80211_C_SWRETRY     0x0020  /* CAPABILITY: sw tx retry    */
#define	IEEE80211_C_TXPMGT      0x0040  /* CAPABILITY: tx power mgmt  */
#define	IEEE80211_C_SHSLOT      0x0080  /* CAPABILITY: short slottime */
#define	IEEE80211_C_SHPREAMBLE  0x0100  /* CAPABILITY: short preamble */
#define	IEEE80211_C_MONITOR     0x0200  /* CAPABILITY: monitor mode   */
#define	IEEE80211_C_RCVMGT      0x0400  /* CAPABILITY: rcv mgt frames */

enum ieee80211_opmode {
  IEEE80211_M_STA     = 1,      /* infrastructure station */
  IEEE80211_M_IBSS    = 0,      /* IBSS (adhoc) station */
  IEEE80211_M_AHDEMO  = 3,      /* Old lucent compatible adhoc demo */
  IEEE80211_M_HOSTAP  = 6,      /* Software Access Point */
  IEEE80211_M_MONITOR = 8       /* Monitor mode */
};

/***************************************************************************/
/*                          ieee80211_prot.h                               */
/***************************************************************************/

/*
 * 802.11 protocol implementation definitions.
*/
enum ieee80211_state {
	IEEE80211_S_INIT	= 0,	      /* default state */
	IEEE80211_S_SCAN	= 1,	      /* scanning */
	IEEE80211_S_AUTH	= 2,	      /* try to authenticate */
	IEEE80211_S_ASSOC	= 3,	      /* try to assoc */
	IEEE80211_S_RUN		= 4,	      /* associated */
};
#define	IEEE80211_S_MAX		(IEEE80211_S_RUN+1)

/***************************************************************************/
/*                          ieee80211_crypto.h                             */
/***************************************************************************/

/*
 * 802.11 protocol crypto-related definitions.
 */
#define	IEEE80211_KEYBUF_SIZE	16
struct ieee80211_wepkey {
  int wk_len;
  u_int8_t wk_key[IEEE80211_KEYBUF_SIZE];
};

/***************************************************************************/
/*                          ieee80211_node.h                               */
/***************************************************************************/

struct ieee80211_rateset {
  u_int8_t rs_nrates;
  u_int8_t rs_rates[IEEE80211_RATE_MAXSIZE];
};

/***************************************************************************/
/*                          ieee80211_var.h                                */
/***************************************************************************/

struct ieee80211com {
  u_int8_t                  ic_myaddr[IEEE80211_ADDR_LEN];

  struct ieee80211_rateset  ic_sup_rates[IEEE80211_MODE_MAX];
  u_int16_t                 ChannelList;

  u_int32_t                 ic_flags;           /* state flags */
  u_int32_t                 ic_caps;            /* capabilities */

  enum ieee80211_opmode     ic_opmode;          /* operation mode */
  enum ieee80211_state      ic_state;           /* 802.11 state */

  u_int16_t                 ic_ibss_chan;
  int                       ic_fixed_rate;      /* index to ic_sup_rates[] */

  int                       ic_des_esslen;
  u_int8_t                  ic_des_essid[IEEE80211_NWID_LEN];

  struct ieee80211_wepkey   ic_nw_keys[IEEE80211_WEP_NKID];
  int                       ic_wep_txkey;       /* default tx key index */
};

/***************************************************************************/
/*                             if_wivar.h                                  */
/***************************************************************************/

struct wi_softc {
  /*
   * This is our Interrupt stuff
   */
  int                 InterruptInitDone; 
  HANDLE volatile     InterruptEvent;
  u_int16_t           EventStatus;

  /*
   * Now here comes the FreeBSD stuff...
   */
  struct ieee80211com sc_ic;
  
  int                 PromiscuousMode;

  int                 wi_gone;
  int                 sc_enabled;
  int                 sc_reset;
  int                 sc_firmware_type;
  u_int32_t           sc_pri_firmware_ver;  /* Primary firmware */
  u_int32_t           sc_sta_firmware_ver;  /* Station firmware */

  int                 wi_cmd_count;

  int                 sc_flags;
  int                 sc_bap_id;
  int                 sc_bap_off;

  u_int16_t           sc_portnum;

  /*
   * RSSI interpretation 
   */
  u_int16_t           sc_min_rssi;          /* clamp sc_min_rssi < RSSI */
  u_int16_t           sc_max_rssi;          /* clamp RSSI < sc_max_rssi */
  u_int16_t           sc_dbm_offset;        /* dBm ~ RSSI - sc_dbm_offset */

  u_int16_t           sc_max_datalen;
  u_int16_t           sc_system_scale;
  u_int16_t           sc_cnfauthmode;
  u_int16_t           sc_roaming_mode;
  u_int16_t           sc_microwave_oven;
  u_int16_t           sc_authtype;

  int                 sc_buflen;            /* TX buffer size */
  int                 sc_ntxbuf;
  struct {
    int d_fid;
    int d_len;
  } sc_txd[WI_NTXBUF];                      /* TX buffers */
  int                 sc_txnext;            /* index of next TX */
  int                 sc_txcur;             /* index of current TX */
  int                 sc_tx_timer;
  int                 sc_scan_timer;

  u_int16_t           sc_ibss_port;
};

typedef struct wi_softc WI_SOFTC;

/*-------------------------------------------------------------------------*/
/* global macros                                                           */
/*-------------------------------------------------------------------------*/
#define device_t              NUTDEVICE*
#define device_get_softc(_a)  (struct wi_softc*)_a->dev_dcb

/*-------------------------------------------------------------------------*/
/* Prototypes                                                              */
/*-------------------------------------------------------------------------*/
int   wlandrv_ProbeDevice (void);
int   wlandrv_Attach      (device_t dev);
void  wlandrv_Init        (device_t dev);
int   wlandrv_PutPacket   (NUTDEVICE * dev, NETBUF * nb);
int   wlandrv_IOCTL       (NUTDEVICE * dev, int req, void *conf);

#endif /* !__WLANDRV_H__ */
