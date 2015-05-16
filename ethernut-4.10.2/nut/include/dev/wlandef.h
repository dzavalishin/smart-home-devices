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
*                   Here in this file you will fimd ONLY defines. All
*                   the structs you will find in wlandrv.h.
****************************************************************************/
#ifndef __WLANDEF_H__
#define __WLANDEF_H__

/*-------------------------------------------------------------------------*/
/* global defines                                                          */
/*-------------------------------------------------------------------------*/

/***************************************************************************/
/*                         ieee80211.h                                     */
/***************************************************************************/

enum {
  IEEE80211_AUTH_NONE   = 0,
  IEEE80211_AUTH_OPEN   = 1,
  IEEE80211_AUTH_SHARED = 2,
};

#define	IEEE80211_RATE_BASIC      0x80
#define	IEEE80211_RATE_VAL			  0x7f
    


/*
 * Maximum acceptable MTU is:
 *	IEEE80211_MAX_LEN - WEP overhead - CRC -
 *		QoS overhead - RSN/WPA overhead
 * Min is arbitrarily chosen > IEEE80211_MIN_LEN.  The default
 * mtu is Ethernet-compatible; it's set by ether_ifattach.
 */
#define	IEEE80211_MTU_MAX			    2290
#define	IEEE80211_MTU_MIN			    32
 
#define	IEEE80211_MAX_LEN			(2300 + IEEE80211_CRC_LEN + \
    (IEEE80211_WEP_IVLEN + IEEE80211_WEP_KIDLEN + IEEE80211_WEP_CRCLEN))
    

/***************************************************************************/
/*                      ieee80211_var.h                                    */
/***************************************************************************/

/*
 * 802.11 definitions
 */
#define IEEE80211_CHAN_MAX        32
#define	IEEE80211_CHAN_ANY	      0xffff  /* token for ``any channel'' */
#define	IEEE80211_CHAN_ANYC \
	((struct ieee80211_channel *) IEEE80211_CHAN_ANY)

/* XXX not really a mode; there are really multiple PHY's */
enum ieee80211_phymode {
  IEEE80211_MODE_AUTO  = 0,               /* autoselect */
  IEEE80211_MODE_11A   = 1,               /* 5GHz, OFDM */
  IEEE80211_MODE_11B   = 2,               /* 2GHz, CCK */
  IEEE80211_MODE_11G   = 3,               /* 2GHz, OFDM */
  IEEE80211_MODE_FH    = 4,               /* 2GHz, GFSK */
  IEEE80211_MODE_TURBO = 5,               /* 5GHz, OFDM, 2x clock */
};
#define	IEEE80211_MODE_MAX	(IEEE80211_MODE_TURBO+1)

/* bits 0-3 are for private use by drivers */
/* channel attributes */
#define	IEEE80211_CHAN_TURBO      0x0010  /* Turbo channel */
#define	IEEE80211_CHAN_CCK        0x0020  /* CCK channel */
#define	IEEE80211_CHAN_OFDM       0x0040  /* OFDM channel */
#define	IEEE80211_CHAN_2GHZ       0x0080  /* 2 GHz spectrum channel. */
#define	IEEE80211_CHAN_5GHZ       0x0100  /* 5 GHz spectrum channel */
#define	IEEE80211_CHAN_PASSIVE    0x0200  /* Only passive scan allowed */
#define	IEEE80211_CHAN_DYN        0x0400  /* Dynamic CCK-OFDM channel */
#define	IEEE80211_CHAN_GFSK       0x0800  /* GFSK channel (FHSS PHY) */

/*
 * Useful combinations of channel characteristics.
 */
#define	IEEE80211_CHAN_FHSS \
	(IEEE80211_CHAN_2GHZ | IEEE80211_CHAN_GFSK)
#define	IEEE80211_CHAN_A \
	(IEEE80211_CHAN_5GHZ | IEEE80211_CHAN_OFDM)
#define	IEEE80211_CHAN_B \
	(IEEE80211_CHAN_2GHZ | IEEE80211_CHAN_CCK)
#define	IEEE80211_CHAN_PUREG \
	(IEEE80211_CHAN_2GHZ | IEEE80211_CHAN_OFDM)
#define	IEEE80211_CHAN_G \
	(IEEE80211_CHAN_2GHZ | IEEE80211_CHAN_DYN)
#define	IEEE80211_CHAN_T \
	(IEEE80211_CHAN_5GHZ | IEEE80211_CHAN_OFDM | IEEE80211_CHAN_TURBO)

/***************************************************************************/
/*                      ieee80211_node.h                                   */
/***************************************************************************/

#define	IEEE80211_RATE_SIZE	      8       /* 802.11 standard */
#define	IEEE80211_RATE_MAXSIZE	  15      /* max rates we'll handle */

/***************************************************************************/
/*                          if_wivar.h                                     */
/***************************************************************************/

/*
 * FirmwareType
*/
#define WI_NOTYPE                 0
#define	WI_LUCENT                 1
#define	WI_INTERSIL               2
#define	WI_SYMBOL                 3

/*
 * Encryption controls. We can enable or disable encryption as
 * well as specify up to 4 encryption keys. We can also specify
 * which of the four keys will be used for transmit encryption.
 */
#define WI_RID_ENCRYPTION         0xFC20
#define WI_RID_AUTHTYPE           0xFC21
#define WI_RID_DEFLT_CRYPT_KEYS   0xFCB0
#define WI_RID_TX_CRYPT_KEY       0xFCB1
#define WI_RID_WEP_AVAIL          0xFD4F
#define WI_RID_P2_TX_CRYPT_KEY    0xFC23
#define WI_RID_P2_CRYPT_KEY0      0xFC24
#define WI_RID_P2_CRYPT_KEY1      0xFC25
#define WI_RID_MICROWAVE_OVEN     0xFC25
#define WI_RID_P2_CRYPT_KEY2      0xFC26
#define WI_RID_P2_CRYPT_KEY3      0xFC27
#define WI_RID_P2_ENCRYPTION      0xFC28
#define WI_RID_ROAMING_MODE       0xFC2D

#define	WI_PRISM_MIN_RSSI         0x1b
#define	WI_PRISM_MAX_RSSI         0x9a
#define	WI_PRISM_DBM_OFFSET       100

#define	WI_LUCENT_MIN_RSSI        47
#define	WI_LUCENT_MAX_RSSI        138
#define	WI_LUCENT_DBM_OFFSET      149

/*
 * Various compat hacks/kludges
 */
#define le16toh(x) (x)
#define htole16(x) (x)

/***************************************************************************/
/*                          if_wireg.h                                     */
/***************************************************************************/

#define WI_DELAY	5UL /* Harald: Added UL specifiers. However that worked before */
#define WI_TIMEOUT	(500000UL/WI_DELAY) /* 500 ms */ /* Harald: Same here */

#define WI_PORT0	(0 << 8)
#define WI_PORT1	(1 << 8)
#define WI_PORT2	(2 << 8)
#define WI_PORT3	(3 << 8)
#define WI_PORT4	(4 << 8)
#define WI_PORT5	(5 << 8)

#define WI_HFA384X_SWSUPPORT0_OFF 0x28
#define WI_PRISM2STA_MAGIC        0x4A2D

/* Default port: 0 (only 0 exists on stations) */
#define WI_DEFAULT_PORT	WI_PORT0

#define WI_DEFAULT_ROAMING	1

#define WI_DEFAULT_AUTHTYPE	1

/*
 * Hermes & Prism2 register definitions 
 */

/* Hermes command/status registers. */
#define WI_COMMAND              0x00
#define WI_PARAM0               0x02
#define WI_PARAM1               0x04
#define WI_PARAM2               0x06
#define WI_STATUS               0x08
#define WI_RESP0                0x0A
#define WI_RESP1                0x0C
#define WI_RESP2                0x0E

/* Command register values. */
#define WI_CMD_BUSY             0x8000  /* busy bit */
#define WI_CMD_INI              0x0000  /* initialize */
#define WI_CMD_ENABLE           0x0001  /* enable */
#define WI_CMD_DISABLE          0x0002  /* disable */
#define WI_CMD_DIAG             0x0003
#define WI_CMD_ALLOC_MEM        0x000A  /* allocate NIC memory */
#define WI_CMD_TX               0x000B  /* transmit */
#define WI_CMD_NOTIFY           0x0010
#define WI_CMD_INQUIRE          0x0011
#define WI_CMD_ACCESS           0x0021
#define WI_CMD_ACCESS_WRITE     0x0121
#define WI_CMD_PROGRAM          0x0022
#define WI_CMD_READEE           0x0030	/* symbol only */
#define WI_CMD_READMIF          0x0030	/* prism2 */
#define WI_CMD_WRITEMIF         0x0031	/* prism2 */
#define WI_CMD_DEBUG            0x0038	/* Various test commands */

#define WI_CMD_CODE_MASK        0x003F

/*
 * Various cmd test stuff.
 */
#define WI_TEST_MONITOR         0x0B
#define WI_TEST_STOP            0x0F
#define WI_TEST_CFG_BITS        0x15
#define WI_TEST_CFG_BIT_ALC     0x08

/*
 * Reclaim qualifier bit, applicable to the
 * TX and INQUIRE commands.
 */
#define WI_RECLAIM              0x0100  /* reclaim NIC memory */

/*
 * ACCESS command qualifier bits.
 */
#define WI_ACCESS_READ          0x0000
#define WI_ACCESS_WRITE         0x0100


/* Status register values */
#define WI_STAT_CMD_CODE        0x003F
#define WI_STAT_DIAG_ERR        0x0100
#define WI_STAT_INQ_ERR         0x0500
#define WI_STAT_CMD_RESULT      0x7F00

/* memory handle management registers */
#define WI_INFO_FID             0x10
#define WI_RX_FID               0x20
#define WI_ALLOC_FID            0x22
#define WI_TX_CMP_FID           0x24

/*
 * Buffer Access Path (BAP) registers.
 * These are I/O channels. I believe you can use each one for
 * any desired purpose independently of the other. In general
 * though, we use BAP1 for reading and writing LTV records and
 * reading received data frames, and BAP0 for writing transmit
 * frames. This is a convention though, not a rule.
 */
#define WI_SEL0                 0x18
#define WI_SEL1                 0x1A
#define WI_OFF0                 0x1C
#define WI_OFF1                 0x1E
#define WI_DATA0                0x36
#define WI_DATA1                0x38
#define WI_BAP0                 WI_DATA0
#define WI_BAP1                 WI_DATA1

#define WI_OFF_BUSY             0x8000
#define WI_OFF_ERR              0x4000
#define WI_OFF_DATAOFF          0x0FFF

/* Event registers */
#define WI_EVENT_STAT           0x30	  /* Event status */
#define WI_INT_EN               0x32	  /* Interrupt enable/disable */
#define WI_EVENT_ACK            0x34	  /* Ack event */

/* Events */
#define WI_EV_TICK              0x8000	/* aux timer tick */
#define WI_EV_RES               0x4000  /* controller h/w error (time out) */
#define WI_EV_INFO_DROP         0x2000  /* no RAM to build unsolicited frame */
#define WI_EV_NO_CARD           0x0800  /* card removed (hunh?) */
#define WI_EV_DUIF_RX           0x0400  /* wavelan management packet received */
#define WI_EV_INFO              0x0080  /* async info frame */
#define WI_EV_CMD               0x0010  /* command completed */
#define WI_EV_ALLOC             0x0008  /* async alloc/reclaim completed */
#define WI_EV_TX_EXC            0x0004  /* async xmit completed with failure */
#define WI_EV_TX                0x0002  /* async xmit completed succesfully */
#define WI_EV_RX                0x0001  /* async rx completed */

/*
 * PCMCIA Address definition
*/
#define WI_COR_OFFSET           0x3e0
#define WI_COR_VALUE            0x41

/* 
 * Define card ident 
*/
#define	WI_NIC_LUCENT_ID          0x0001
#define	WI_NIC_LUCENT_STR         "Lucent Technologies, WaveLAN/IEEE"

#define	WI_NIC_SONY_ID            0x0002
#define	WI_NIC_SONY_STR           "Sony WaveLAN/IEEE"

#define	WI_NIC_LUCENT_EMB_ID      0x0005
#define	WI_NIC_LUCENT_EMB_STR	    "Lucent Embedded WaveLAN/IEEE"

#define	WI_NIC_EVB2_ID            0x8000
#define	WI_NIC_EVB2_STR           "RF:PRISM2 MAC:HFA3841"

#define	WI_NIC_HWB3763_ID         0x8001
#define	WI_NIC_HWB3763_STR	      "RF:PRISM2 MAC:HFA3841 CARD:HWB3763 rev.B"

#define	WI_NIC_HWB3163_ID         0x8002
#define	WI_NIC_HWB3163_STR        "RF:PRISM2 MAC:HFA3841 CARD:HWB3163 rev.A"

#define	WI_NIC_HWB3163B_ID        0x8003
#define	WI_NIC_HWB3163B_STR       "RF:PRISM2 MAC:HFA3841 CARD:HWB3163 rev.B"

#define	WI_NIC_EVB3_ID            0x8004
#define	WI_NIC_EVB3_STR           "RF:PRISM2 MAC:HFA3842 CARD:HFA3842 EVAL"

#define	WI_NIC_HWB1153_ID         0x8007
#define	WI_NIC_HWB1153_STR        "RF:PRISM1 MAC:HFA3841 CARD:HWB1153"

#define	WI_NIC_P2_SST_ID          0x8008  /* Prism2 with SST flush */
#define	WI_NIC_P2_SST_STR         "RF:PRISM2 MAC:HFA3841 CARD:HWB3163-SST-flash"

#define	WI_NIC_EVB2_SST_ID        0x8009
#define	WI_NIC_EVB2_SST_STR       "RF:PRISM2 MAC:HFA3841 CARD:HWB3163-SST-flash"

#define	WI_NIC_3842_EVA_ID        0x800A  /* 3842 Evaluation Board */
#define	WI_NIC_3842_EVA_STR       "RF:PRISM2 MAC:HFA3842 CARD:HFA3842 EVAL"

#define	WI_NIC_3842_PCMCIA_AMD_ID	0x800B  /* Prism2.5 PCMCIA */
#define	WI_NIC_3842_PCMCIA_SST_ID	0x800C
#define	WI_NIC_3842_PCMCIA_ATL_ID	0x800D
#define	WI_NIC_3842_PCMCIA_ATS_ID	0x800E
#define	WI_NIC_3842_PCMCIA_STR		"RF:PRISM2.5 MAC:ISL3873"

#define	WI_NIC_3842_MINI_AMD_ID   0x8012  /* Prism2.5 Mini-PCI */
#define	WI_NIC_3842_MINI_SST_ID   0x8013
#define	WI_NIC_3842_MINI_ATL_ID   0x8014
#define	WI_NIC_3842_MINI_ATS_ID   0x8015
#define	WI_NIC_3842_MINI_STR      "RF:PRISM2.5 MAC:ISL3874A(Mini-PCI)"

#define	WI_NIC_3842_PCI_AMD_ID    0x8016  /* Prism2.5 PCI-bridge */
#define	WI_NIC_3842_PCI_SST_ID    0x8017
#define	WI_NIC_3842_PCI_ATL_ID    0x8018
#define	WI_NIC_3842_PCI_ATS_ID    0x8019
#define	WI_NIC_3842_PCI_STR       "RF:PRISM2.5 MAC:ISL3874A(PCI-bridge)"

#define	WI_NIC_P3_PCMCIA_AMD_ID   0x801A  /* Prism3 PCMCIA */
#define	WI_NIC_P3_PCMCIA_SST_ID   0x801B
#define	WI_NIC_P3_PCMCIA_ATL_ID   0x801C
#define	WI_NIC_P3_PCMCIA_ATS_ID   0x801D
#define	WI_NIC_P3_PCMCIA_STR      "RF:PRISM3(PCMCIA)"

#define	WI_NIC_P3_MINI_AMD_ID     0x8021  /* Prism3 Mini-PCI */
#define	WI_NIC_P3_MINI_SST_ID     0x8022
#define	WI_NIC_P3_MINI_ATL_ID     0x8023
#define	WI_NIC_P3_MINI_ATS_ID     0x8024
#define	WI_NIC_P3_MINI_STR        "RF:PRISM3(Mini-PCI)"

/*
 * Connection control characteristics. (0xFC00)
 * 0 == IBSS (802.11 compliant mode) (Only PRISM2)
 * 1 == Basic Service Set (BSS)
 * 2 == Wireless Distribudion System (WDS)
 * 3 == Pseudo IBSS 
 *	(Only PRISM2; not 802.11 compliant mode, testing use only)
 * 6 == HOST AP (Only PRISM2)
 */
#define	WI_PORTTYPE_IBSS    0x0
#define WI_PORTTYPE_BSS     0x1
#define WI_PORTTYPE_WDS     0x2
#define WI_PORTTYPE_ADHOC   0x3
#define	WI_PORTTYPE_HOSTAP  0x6

/*
 * Information frame types.
*/
#define WI_INFO_NOTIFY                  0xF000  /* Handover address */
#define WI_INFO_COUNTERS                0xF100  /* Statistics counters */
#define WI_INFO_SCAN_RESULTS            0xF101  /* Scan results */
#define WI_INFO_HOST_SCAN_RESULTS       0xF104  /* Scan results */
#define WI_INFO_LINK_STAT               0xF200  /* Link status */
#define	WI_INFO_LINK_STAT_CONNECTED     1
#define	WI_INFO_LINK_STAT_DISCONNECTED  2
#define	WI_INFO_LINK_STAT_AP_CHG        3 /* AP Change */
#define	WI_INFO_LINK_STAT_AP_OOR        4 /* AP Out Of Range */
#define	WI_INFO_LINK_STAT_AP_INR        5 /* AP In Range */
#define	WI_INFO_LINK_STAT_ASSOC_FAILED  6
#define WI_INFO_ASSOC_STAT              0xF201  /* Association status */
#define	WI_INFO_AUTH_REQUEST            0xF202  /* Authentication Request (AP) */
#define	WI_INFO_POWERSAVE_COUNT         0xF203  /* PowerSave User Count (AP) */

/* Rx Status Field */
#define WI_STAT_ERRSTAT                 0x0003


/***************************************************************************/
/*                          if_wavelan_ieee.h                              */
/***************************************************************************/

/*
 * Network parameters, static configuration entities.
 */
#define WI_RID_PORTTYPE		        0xFC00  /* Connection control characteristics */
#define WI_RID_MAC_NODE           0xFC01  /* MAC address of this station */
#define WI_RID_DESIRED_SSID       0xFC02  /* Service Set ID for connection */
#define WI_RID_OWN_CHNL           0xFC03  /* Comm channel for BSS creation */
#define WI_RID_OWN_SSID           0xFC04  /* IBSS creation ID */
#define WI_RID_OWN_ATIM_WIN	      0xFC05  /* ATIM window time for IBSS creation */
#define WI_RID_SYSTEM_SCALE	      0xFC06  /* scale that specifies AP density */
#define WI_RID_MAX_DATALEN        0xFC07  /* Max len of MAC frame body data */
#define WI_RID_MAC_WDS            0xFC08  /* MAC addr of corresponding WDS node */
#define WI_RID_PM_ENABLED         0xFC09  /* ESS power management enable */
#define WI_RID_PM_EPS             0xFC0A  /* PM EPS/PS mode */
#define WI_RID_MCAST_RX           0xFC0B  /* ESS PM mcast reception */
#define WI_RID_MAX_SLEEP          0xFC0C  /* max sleep time for ESS PM */
#define WI_RID_HOLDOVER           0xFC0D  /* holdover time for ESS PM */
#define WI_RID_NODENAME           0xFC0E  /* ID name of this node for diag */
#define WI_RID_DTIM_PERIOD        0xFC10  /* beacon interval between DTIMs */
#define WI_RID_WDS_ADDR1          0xFC11  /* port 1 MAC of WDS link node */
#define WI_RID_WDS_ADDR2          0xFC12  /* port 1 MAC of WDS link node */
#define WI_RID_WDS_ADDR3          0xFC13  /* port 1 MAC of WDS link node */
#define WI_RID_WDS_ADDR4          0xFC14  /* port 1 MAC of WDS link node */
#define WI_RID_WDS_ADDR5          0xFC15  /* port 1 MAC of WDS link node */
#define WI_RID_WDS_ADDR6          0xFC16  /* port 1 MAC of WDS link node */
#define WI_RID_MCAST_PM_BUF       0xFC17  /* PM buffering of mcast */
#define WI_RID_ENCRYPTION         0xFC20  /* enable/disable WEP */
#define WI_RID_AUTHTYPE           0xFC21  /* specify authentication type */
#define WI_RID_P2_TX_CRYPT_KEY    0xFC23
#define WI_RID_P2_CRYPT_KEY0      0xFC24
#define WI_RID_P2_CRYPT_KEY1      0xFC25
#define WI_RID_MICROWAVE_OVEN     0xFC25
#define WI_RID_P2_CRYPT_KEY2      0xFC26
#define WI_RID_P2_CRYPT_KEY3      0xFC27
#define WI_RID_P2_ENCRYPTION      0xFC28
#define  PRIVACY_INVOKED          0x01
#define  EXCLUDE_UNENCRYPTED      0x02
#define  HOST_ENCRYPT             0x10
#define	 IV_EVERY_FRAME           0x00    /* IV = Initialization Vector */
#define	 IV_EVERY10_FRAME         0x20    /* every 10 frame IV reuse */
#define	 IV_EVERY50_FRAME         0x40    /* every 50 frame IV reuse */
#define	 IV_EVERY100_FRAME        0x60    /* every 100 frame IV reuse */
#define	 HOST_DECRYPT             0x80
#define WI_RID_WEP_MAPTABLE       0xFC29
#define WI_RID_CNFAUTHMODE        0xFC2A
#define WI_RID_ROAMING_MODE       0xFC2D
#define WI_RID_OWN_BEACON_INT     0xFC33  /* beacon xmit time for BSS creation */
#define WI_RID_CNF_DBM_ADJUST     0xFC46
#define WI_RID_DBM_ADJUST         0xFC46  /* RSSI - WI_RID_DBM_ADJUST ~ dBm */
#define WI_RID_BASIC_RATE         0xFCB3
#define WI_RID_SUPPORT_RATE       0xFCB4

/*
 * Network parameters, dynamic configuration entities
 */
#define WI_RID_MCAST_LIST         0xFC80  /* list of multicast addrs */
#define WI_RID_CREATE_IBSS        0xFC81  /* create IBSS */
#define WI_RID_FRAG_THRESH        0xFC82  /* frag len, unicast msg xmit */
#define WI_RID_RTS_THRESH         0xFC83  /* frame len for RTS/CTS handshake */
#define WI_RID_TX_RATE            0xFC84  /* data rate for message xmit */
                                          /* 0 == Fixed 1mbps */
                                          /* 1 == Fixed 2mbps */
                                          /* 2 == auto fallback */                                  
#define WI_RID_PROMISC            0xFC85  /* enable promisc mode */
#define WI_RID_FRAG_THRESH0       0xFC90
#define WI_RID_FRAG_THRESH1	      0xFC91
#define WI_RID_FRAG_THRESH2	      0xFC92
#define WI_RID_FRAG_THRESH3	      0xFC93
#define WI_RID_FRAG_THRESH4	      0xFC94
#define WI_RID_FRAG_THRESH5	      0xFC95
#define WI_RID_FRAG_THRESH6	      0xFC96
#define WI_RID_RTS_THRESH0        0xFC97
#define WI_RID_RTS_THRESH1        0xFC98
#define WI_RID_RTS_THRESH2        0xFC99
#define WI_RID_RTS_THRESH3        0xFC9A
#define WI_RID_RTS_THRESH4        0xFC9B
#define WI_RID_RTS_THRESH5        0xFC9C
#define WI_RID_RTS_THRESH6        0xFC9D
#define WI_RID_TX_RATE0           0xFC9E
#define WI_RID_TX_RATE1           0xFC9F
#define WI_RID_TX_RATE2           0xFCA0
#define WI_RID_TX_RATE3           0xFCA1
#define WI_RID_TX_RATE4           0xFCA2
#define WI_RID_TX_RATE5           0xFCA3
#define WI_RID_TX_RATE6           0xFCA4
#define WI_RID_DEFLT_CRYPT_KEYS   0xFCB0
#define WI_RID_TX_CRYPT_KEY       0xFCB1
#define WI_RID_TICK_TIME          0xFCE0


struct wi_key {
	u_int16_t		wi_keylen;
	u_int8_t		wi_keydat[14];
};


/*
 * NIC information
*/
#define WI_RID_DNLD_BUF           0xFD01
#define WI_RID_MEMSZ              0xFD02  /* memory size info (XXX Lucent) */
                                          /*  Looks like on lucnet pri firm too */
#define	WI_RID_PRI_IDENTITY       0xFD02  /* primary funcs firmware ident (PRISM2) */
#define WI_RID_PRI_SUP_RANGE      0xFD03  /* primary supplier compatibility */
#define WI_RID_CIF_ACT_RANGE      0xFD04  /* controller sup. compatibility */
#define WI_RID_SERIALNO           0xFD0A  /* card serial number */
#define WI_RID_CARD_ID            0xFD0B  /* card identification */
#define WI_RID_MFI_SUP_RANGE      0xFD0C  /* modem supplier compatibility */
#define WI_RID_CFI_SUP_RANGE      0xFD0D  /* controller sup. compatibility */
#define WI_RID_CHANNEL_LIST       0xFD10  /* allowd comm. frequencies. */
#define WI_RID_REG_DOMAINS        0xFD11  /* list of intendted regulatory doms */
#define WI_RID_TEMP_TYPE          0xFD12  /* hw temp range code */
#define WI_RID_CIS                0xFD13  /* PC card info struct */
#define WI_RID_STA_IDENTITY       0xFD20  /* station funcs firmware ident */
#define WI_RID_STA_SUP_RANGE      0xFD21  /* station supplier compat */
#define WI_RID_MFI_ACT_RANGE      0xFD22
#define WI_RID_SYMBOL_IDENTITY    0xFD24
#define WI_RID_CFI_ACT_RANGE      0xFD33
#define WI_RID_COMMQUAL           0xFD43
#define WI_RID_SCALETHRESH        0xFD46
#define WI_RID_PCF                0xFD87

/*
 * MAC information
*/
#define WI_RID_PORT_STAT          0xFD40  /* actual MAC port con control stat */
#define WI_RID_CURRENT_SSID       0xFD41  /* ID of actually connected SS */
#define WI_RID_CURRENT_BSSID      0xFD42  /* ID of actually connected BSS */
#define WI_RID_COMMS_QUALITY      0xFD43  /* quality of BSS connection */
#define WI_RID_CUR_TX_RATE        0xFD44  /* current TX rate */
#define WI_RID_CUR_BEACON_INT     0xFD45  /* current beacon interval */
#define WI_RID_CUR_SCALE_THRESH   0xFD46  /* actual system scane thresh setting */
#define WI_RID_PROT_RESP_TIME     0xFD47  /* time to wait for resp to req msg */
#define WI_RID_SHORT_RTR_LIM      0xFD48  /* max tx attempts for short frames */
#define WI_RID_LONG_RTS_LIM       0xFD49  /* max tx attempts for long frames */
#define WI_RID_MAX_TX_LIFE        0xFD4A  /* max tx frame handling duration */
#define WI_RID_MAX_RX_LIFE        0xFD4B  /* max rx frame handling duration */
#define WI_RID_CF_POLL            0xFD4C  /* contention free pollable ind */
#define WI_RID_AUTH_ALGS          0xFD4D  /* auth algorithms available */
#define WI_RID_AUTH_TYPE          0xFD4E  /* availanle auth types */
#define WI_RID_WEP_AVAIL          0xFD4F  /* WEP privacy option available */
#define WI_RID_DBM_COMMS_QUAL     0xFD51  /* CommQuality normalized to dBm */
#define WI_RID_CUR_TX_RATE1       0xFD80
#define WI_RID_CUR_TX_RATE2       0xFD81
#define WI_RID_CUR_TX_RATE3       0xFD82
#define WI_RID_CUR_TX_RATE4       0xFD83
#define WI_RID_CUR_TX_RATE5       0xFD84
#define WI_RID_CUR_TX_RATE6       0xFD85
#define WI_RID_OWN_MAC            0xFD86  /* unique local MAC addr */
#define WI_RID_PCI_INFO           0xFD87  /* point coordination func cap */

/*
 * Modem information
 */
#define WI_RID_PHY_TYPE           0xFDC0  /* phys layer type indication */
#define WI_RID_CURRENT_CHAN       0xFDC1  /* current frequency */
#define WI_RID_PWR_STATE          0xFDC2  /* pwr consumption status */
#define WI_RID_CCA_MODE           0xFDC3  /* clear chan assess mode indication */
#define WI_RID_CCA_TIME           0xFDC4  /* clear chan assess time */
#define WI_RID_MAC_PROC_DELAY     0xFDC5  /* MAC processing delay time */
#define WI_RID_DATA_RATES	        0xFDC6  /* supported data rates */

#endif /* !__WLANDEF_H__ */
