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
*  Copyright (c) 2002
*	 M Warner Losh <imp@freebsd.org>.  All rights reserved.
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
*                   If you like to see the original source. Take a look
*                   in if_wi.c from FreeBSD.
*  01.03.04  mifi   Now we use a timeout in our Thread, and check if we lost
*                   the card. In this case we try to Init it again.
*  02.03.04  mifi   Remove QuickHack for Networkname and WEP.
*  03.03.04  mifi   Use struct to config WLAN (WLAN_IOCTL_SET_CONFIG)
*  04.03.04  mifi   Add function to get WLAN_STATUS (WLAN_IOCTL_GET_STATUS)
*  06.03.04  mifi   Remove all @@MF later for monitor mode.
*                   It does not work with my card, and I think we need a
*                   special firmware for it.
****************************************************************************/
#define __WLANDRV_C__

#include <compiler.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include <sys/timer.h>
#include <sys/thread.h>
#include <sys/event.h>
#include <errno.h>
#include <netinet/if_ether.h>
#include <net/ether.h>
#include <net/if_var.h>
#include <dev/irqreg.h>

#include <dev/wlantypes.h>
#include <dev/pcmcia.h>
#include <dev/wlancfg.h>
#include <dev/wlan.h>
#include <dev/wlandrv.h>

/*==========================================================*/
/*  DEFINE: All Structures and Common Constants             */
/*==========================================================*/
#if (_DEBUG >= 1)
#define Debug(_x)     if (bDebugState ) printf _x
#define Debug2(_x)    if (bDebugState > 1) printf _x
#define panic         printf
#else
#define Debug(_x)
#define Debug2(_x)
#define panic
#endif

#define WI_LOCK(sc)           NutEventWait(&hDeviceSemaphore,0)
#define WI_UNLOCK(sc)         NutEventPost(&hDeviceSemaphore)

#define ieee80211_new_state(_a,_b,_c)  _a->ic_state=_b
#define isset(_a,_b)                  (*_a & (1<<_b))

#define CSR_WRITE_2(_a,_b,_c) pcmcia_WriteReg(_b,_c)
#define CSR_READ_2(_a,_b)     pcmcia_ReadReg(_b)

/*
 * Length information
 */
#define WLAN_MIN_ETHERNET_FRAME_LEN	60
#define WLAN_MAX_ETHERNET_FRAME_LEN	1514
#define WLAN_ETHERNET_HEADER_LEN		14

/*
 * Interrupt define
*/
#define WI_INTRS  (WI_EV_RX | WI_EV_ALLOC | WI_EV_INFO)

struct wi_card_ident {
    u_int16_t card_id;
    char *card_name;
    u_int8_t firm_type;
};

struct ieee80211_frame_addr4 {
    u_int8_t i_fc[2];
    u_int8_t i_dur[2];
    u_int8_t i_addr1[IEEE80211_ADDR_LEN];
    u_int8_t i_addr2[IEEE80211_ADDR_LEN];
    u_int8_t i_addr3[IEEE80211_ADDR_LEN];
    u_int8_t i_seq[2];
    u_int8_t i_addr4[IEEE80211_ADDR_LEN];
};

/*
 * transmit/receive frame structure
*/
struct wi_frame {
    u_int16_t wi_status;        /* 0x00 */
    u_int16_t wi_rx_tstamp1;    /* 0x02 */
    u_int16_t wi_rx_tstamp0;    /* 0x04 */
    u_int8_t wi_rx_silence;     /* 0x06 */
    u_int8_t wi_rx_signal;      /* 0x07 */
    u_int8_t wi_rx_rate;        /* 0x08 */
    u_int8_t wi_rx_flow;        /* 0x09 */
    u_int8_t wi_tx_rtry;        /* 0x0a *//* Prism2 AP Only */
    u_int8_t wi_tx_rate;        /* 0x0b *//* Prism2 AP Only */
    u_int16_t wi_tx_ctl;        /* 0x0c */
    struct ieee80211_frame_addr4 wi_whdr;       /* 0x0e */
    u_int16_t wi_dat_len;       /* 0x2c */
    struct ether_header wi_ehdr;        /* 0x2e */
};

/*
 * Station set identification (SSID). (0xFC02, 0xFC04)
*/
struct wi_ssid {
    u_int16_t wi_len;
    u_int8_t wi_ssid[32];
};

/*
 * Ethernet LLCS + SNAP header
*/
typedef struct {
    u_int16_t DSAPSSAP;
    u_int16_t Control;
    u_int16_t MustZero;
    u_int16_t Type;
} LLCS_SNAP_HEADER;

/*==========================================================*/
/*  DEFINE: Prototypes                                      */
/*==========================================================*/

/*==========================================================*/
/*  DEFINE: Definition of all local Data                    */
/*==========================================================*/
static HANDLE hDeviceSemaphore;
static BYTE bDebugState = 0;

struct wi_card_ident wi_card_ident[] = {
    /*
     * CARD_ID          CARD_NAME      FIRM_TYPE
     */
#if (WLAN_SUPPORT_LUCENT >= 1)
    {WI_NIC_LUCENT_ID, WI_NIC_LUCENT_STR, WI_LUCENT},
    {WI_NIC_SONY_ID, WI_NIC_SONY_STR, WI_LUCENT},
    {WI_NIC_LUCENT_EMB_ID, WI_NIC_LUCENT_EMB_STR, WI_LUCENT},
#endif                          /* (WLAN_SUPPORT_LUCENT >= 1) */

#if (WLAN_SUPPORT_INTERSIL >= 1)
    {WI_NIC_EVB2_ID, WI_NIC_EVB2_STR, WI_INTERSIL},
    {WI_NIC_HWB3763_ID, WI_NIC_HWB3763_STR, WI_INTERSIL},
    {WI_NIC_HWB3163_ID, WI_NIC_HWB3163_STR, WI_INTERSIL},
    {WI_NIC_HWB3163B_ID, WI_NIC_HWB3163B_STR, WI_INTERSIL},
    {WI_NIC_EVB3_ID, WI_NIC_EVB3_STR, WI_INTERSIL},
    {WI_NIC_HWB1153_ID, WI_NIC_HWB1153_STR, WI_INTERSIL},
    {WI_NIC_P2_SST_ID, WI_NIC_P2_SST_STR, WI_INTERSIL},
    {WI_NIC_EVB2_SST_ID, WI_NIC_EVB2_SST_STR, WI_INTERSIL},
    {WI_NIC_3842_EVA_ID, WI_NIC_3842_EVA_STR, WI_INTERSIL},
    {WI_NIC_3842_PCMCIA_AMD_ID, WI_NIC_3842_PCMCIA_STR, WI_INTERSIL},
    {WI_NIC_3842_PCMCIA_SST_ID, WI_NIC_3842_PCMCIA_STR, WI_INTERSIL},
    {WI_NIC_3842_PCMCIA_ATL_ID, WI_NIC_3842_PCMCIA_STR, WI_INTERSIL},
    {WI_NIC_3842_PCMCIA_ATS_ID, WI_NIC_3842_PCMCIA_STR, WI_INTERSIL},
    {WI_NIC_3842_MINI_AMD_ID, WI_NIC_3842_MINI_STR, WI_INTERSIL},
    {WI_NIC_3842_MINI_SST_ID, WI_NIC_3842_MINI_STR, WI_INTERSIL},
    {WI_NIC_3842_MINI_ATL_ID, WI_NIC_3842_MINI_STR, WI_INTERSIL},
    {WI_NIC_3842_MINI_ATS_ID, WI_NIC_3842_MINI_STR, WI_INTERSIL},
    {WI_NIC_3842_PCI_AMD_ID, WI_NIC_3842_PCI_STR, WI_INTERSIL},
    {WI_NIC_3842_PCI_SST_ID, WI_NIC_3842_PCI_STR, WI_INTERSIL},
    {WI_NIC_3842_PCI_ATS_ID, WI_NIC_3842_PCI_STR, WI_INTERSIL},
    {WI_NIC_3842_PCI_ATL_ID, WI_NIC_3842_PCI_STR, WI_INTERSIL},
    {WI_NIC_P3_PCMCIA_AMD_ID, WI_NIC_P3_PCMCIA_STR, WI_INTERSIL},
    {WI_NIC_P3_PCMCIA_SST_ID, WI_NIC_P3_PCMCIA_STR, WI_INTERSIL},
    {WI_NIC_P3_PCMCIA_ATL_ID, WI_NIC_P3_PCMCIA_STR, WI_INTERSIL},
    {WI_NIC_P3_PCMCIA_ATS_ID, WI_NIC_P3_PCMCIA_STR, WI_INTERSIL},
    {WI_NIC_P3_MINI_AMD_ID, WI_NIC_P3_MINI_STR, WI_INTERSIL},
    {WI_NIC_P3_MINI_SST_ID, WI_NIC_P3_MINI_STR, WI_INTERSIL},
    {WI_NIC_P3_MINI_ATL_ID, WI_NIC_P3_MINI_STR, WI_INTERSIL},
    {WI_NIC_P3_MINI_ATS_ID, WI_NIC_P3_MINI_STR, WI_INTERSIL},
#endif                          /* (WLAN_SUPPORT_INTERSIL >= 1) */
    {0, NULL, 0},
};

/*==========================================================*/
/*  DEFINE: Definition of all local Procedures              */
/*==========================================================*/
/************************************************************/
/*  DELAY                                                   */
/*                                                          */
/*  Delay in us.                                            */
/************************************************************/
static void DELAY(u_int32_t delay_us)
{
    delay_us = delay_us / 1000;
    delay_us++;
    NutSleep(delay_us);
}

/************************************************************/
/*  DumpData                                                */
/*                                                          */
/*  Dump the Buffer to the Terminal                         */
/************************************************************/
/* Harald: Not always used, but not sure how. Changed
fron static to global and changed name to avoid conflicts */
void DumpWlanData(BYTE * pBuffer, WORD wBufferSize)
{
    u_int16_t x, MaxX = 16;
    u_int16_t y, MaxY;
    u_int16_t Size;

    if (bDebugState != 0) {
        Size = 0;
        MaxY = (wBufferSize / 16) + 1;
        for (y = 0; y < MaxY; y++) {
            Debug(("%04X: ", (y * 16)));
            for (x = 0; x < MaxX; x++) {
                Debug(("%02X ", pBuffer[Size]));
                Size++;

                if (Size >= wBufferSize) {
                    Debug(("\r\n"));
                    return;
                }
            }
            Debug(("\r\n"));
        }
    }
}

/************************************************************/
/*  wi_cmd                                                  */
/************************************************************/
static int wi_cmd(struct wi_softc *sc, int cmd, int val0, int val1, int val2)
{
    unsigned long i;            /* Harald: Changed from int to long. */
    unsigned int s = 0;			/* Oliver: Changed from signed to unsigned. */
    static volatile int count = 0;

    if (sc->wi_gone) {
        return (ENODEV);
    }

    /* Harald: Removed this, it makes no sense */
    //if (count > 0) {
    //  panic("Hey partner, hold on there!");
    //}
    count++;

    /*
     * wait for the busy bit to clear
     */
    for (i = sc->wi_cmd_count; i > 0; i--) {    /* 500ms */
        if (!(CSR_READ_2(sc, WI_COMMAND) & WI_CMD_BUSY)) {
            break;
        }
        DELAY(1 * 1000);        /* 1ms */
    }
    if (i == 0) {
        Debug(("wi_cmd: busy bit won't clear.\n"));
        sc->wi_gone = 1;
        count--;
        return (ETIMEDOUT);
    }

    CSR_WRITE_2(sc, WI_PARAM0, val0);
    CSR_WRITE_2(sc, WI_PARAM1, val1);
    CSR_WRITE_2(sc, WI_PARAM2, val2);
    CSR_WRITE_2(sc, WI_COMMAND, cmd);

    if (cmd == WI_CMD_INI) {
        /*
         * XXX: should sleep here.
         */
        /* Harald: Added UL specifiers, no idea how Michael's ICC accepted this */
        DELAY(250UL * 1000UL);  /* 250ms delay for init */
    }
    for (i = 0; i < WI_TIMEOUT; i++) {
        /*
         * Wait for 'command complete' bit to be
         * set in the event status register.
         */
        s = CSR_READ_2(sc, WI_EVENT_STAT);
        if (s & WI_EV_CMD) {
            /*
             * Ack the event and read result code.
             */
            s = CSR_READ_2(sc, WI_STATUS);
            CSR_WRITE_2(sc, WI_EVENT_ACK, WI_EV_CMD);
            if (s & WI_STAT_CMD_RESULT) {
                count--;
                return (EIO);
            }
            break;
        }
        DELAY(WI_DELAY);
    }

    count--;
    if (i == WI_TIMEOUT) {
        Debug(("timeout in wi_cmd 0x%04x; event status 0x%04x\n", cmd, s));
        if (s == 0xffff) {
            sc->wi_gone = 1;
        }
        return (ETIMEDOUT);
    }
    return (0);
}

/************************************************************/
/*  wi_stop                                                 */
/************************************************************/
void wi_stop(struct wi_softc *sc, int disable)
{
    struct ieee80211com *ic = &sc->sc_ic;

    DELAY(100000);

    ieee80211_new_state(ic, IEEE80211_S_INIT, -1);
    if (sc->sc_enabled && !sc->wi_gone) {
        CSR_WRITE_2(sc, WI_INT_EN, 0);
        wi_cmd(sc, WI_CMD_DISABLE | sc->sc_portnum, 0, 0, 0);
        if (disable) {
            sc->sc_enabled = 0;
        }
    } else {
        if (sc->wi_gone && disable) {   /* gone --> not enabled */
            sc->sc_enabled = 0;
        }
    }

    sc->sc_tx_timer = 0;
    sc->sc_scan_timer = 0;
}

/************************************************************/
/*  wi_seek_bap                                             */
/************************************************************/
static int wi_seek_bap(struct wi_softc *sc, int id, int off)
{
    long i;                     /* Harald: from int to long  */
    unsigned int status;		/* Oliver: from signed to unsigned */

    CSR_WRITE_2(sc, WI_SEL0, id);
    CSR_WRITE_2(sc, WI_OFF0, off);

    for (i = 0;; i++) {
        status = CSR_READ_2(sc, WI_OFF0);
        if ((status & WI_OFF_BUSY) == 0) {
            break;
        }
        if (i == WI_TIMEOUT) {
            Debug(("timeout in wi_seek to %x/%x\n", id, off));
            sc->sc_bap_off = WI_OFF_ERR;        /* invalidate */
            if (status == 0xffff) {
                sc->wi_gone = 1;
            }
            return ETIMEDOUT;
        }
        DELAY(1);
    }
    if (status & WI_OFF_ERR) {
        Debug(("failed in wi_seek to %x/%x\n", id, off));
        sc->sc_bap_off = WI_OFF_ERR;    /* invalidate */
        return EIO;
    }
    sc->sc_bap_id = id;
    sc->sc_bap_off = off;
    return 0;
}

/************************************************************/
/*  wi_read_bap                                             */
/************************************************************/
static int wi_read_bap(struct wi_softc *sc, int id, int off, void *buf, int buflen)
{
    u_int16_t *ptr;
    int i, error, cnt;

    if (buflen == 0) {
        return 0;
    }
    if (id != sc->sc_bap_id || off != sc->sc_bap_off) {
        if ((error = wi_seek_bap(sc, id, off)) != 0) {
            return error;
        }
    }
    cnt = (buflen + 1) / 2;
    ptr = (u_int16_t *) buf;
    for (i = 0; i < cnt; i++) {
        *ptr++ = CSR_READ_2(sc, WI_DATA0);
    }
    sc->sc_bap_off += cnt * 2;
    return 0;
}

/************************************************************/
/*  wi_write_bap                                            */
/************************************************************/
static int wi_write_bap(struct wi_softc *sc, int id, int off, void *buf, int buflen)
{
    u_int16_t *ptr;
    int i, error, cnt;

    if (buflen == 0) {
        return 0;
    }
#ifdef WI_HERMES_AUTOINC_WAR
  again:
#endif
    if (id != sc->sc_bap_id || off != sc->sc_bap_off) {
        if ((error = wi_seek_bap(sc, id, off)) != 0) {
            return error;
        }
    }
    cnt = (buflen + 1) / 2;
    ptr = (u_int16_t *) buf;
    for (i = 0; i < cnt; i++) {
        CSR_WRITE_2(sc, WI_DATA0, ptr[i]);
    }
    sc->sc_bap_off += cnt * 2;

#ifdef WI_HERMES_AUTOINC_WAR
    /*
     * According to the comments in the HCF Light code, there is a bug
     * in the Hermes (or possibly in certain Hermes firmware revisions)
     * where the chip's internal autoincrement counter gets thrown off
     * during data writes:  the autoincrement is missed, causing one
     * data word to be overwritten and subsequent words to be written to
     * the wrong memory locations. The end result is that we could end
     * up transmitting bogus frames without realizing it. The workaround
     * for this is to write a couple of extra guard words after the end
     * of the transfer, then attempt to read then back. If we fail to
     * locate the guard words where we expect them, we preform the
     * transfer over again.
     */
    if ((sc->sc_flags & WI_FLAGS_BUG_AUTOINC) && (id & 0xf000) == 0) {
        CSR_WRITE_2(sc, WI_DATA0, 0x1234);
        CSR_WRITE_2(sc, WI_DATA0, 0x5678);
        wi_seek_bap(sc, id, sc->sc_bap_off);
        sc->sc_bap_off = WI_OFF_ERR;    /* invalidate */
        if (CSR_READ_2(sc, WI_DATA0) != 0x1234 || CSR_READ_2(sc, WI_DATA0) != 0x5678) {
            Debug(("detect auto increment bug, try again\n"));
            goto again;
        }
    }
#endif
    return 0;
}

/************************************************************/
/*  wi_read_rid                                             */
/************************************************************/
static int wi_read_rid(struct wi_softc *sc, int rid, void *buf, int *buflenp)
{
    int error, len;
    u_int16_t ltbuf[2];

    /*
     * Tell the NIC to enter record read mode.
     */
    error = wi_cmd(sc, WI_CMD_ACCESS | WI_ACCESS_READ, rid, 0, 0);
    if (error) {
        return error;
    }

    error = wi_read_bap(sc, rid, 0, ltbuf, sizeof(ltbuf));
    if (error) {
        return error;
    }

    if (le16toh(ltbuf[1]) != (u_int16_t) rid) {
        Debug(("record read mismatch, rid=%x, got=%x\n", rid, le16toh(ltbuf[1])));
        return EIO;
    }
    len = (le16toh(ltbuf[0]) - 1) * 2;  /* already got rid */
    if (*buflenp < len) {
        Debug(("record buffer is too small, rid=%x, size=%d, len=%d\n", rid, *buflenp, len));
        return ENOSPC;
    }
    *buflenp = len;
    return wi_read_bap(sc, rid, sizeof(ltbuf), buf, len);
}

/************************************************************/
/*  wi_write_rid                                            */
/*                                                          */
/*  Write a block of data to the Resource Identifier.       */
/*                                                          */
/*  Return: OK or Error cause.                              */
/************************************************************/
static int wi_write_rid(struct wi_softc *sc, int rid, void *buf, int buflen)
{
    int error;
    u_int16_t ltbuf[2];

    ltbuf[0] = htole16((buflen + 1) / 2 + 1);   /* includes rid */
    ltbuf[1] = htole16(rid);

    error = wi_write_bap(sc, rid, 0, ltbuf, sizeof(ltbuf));
    if (error) {
        return error;
    }
    error = wi_write_bap(sc, rid, sizeof(ltbuf), buf, buflen);
    if (error) {
        return error;
    }

    return wi_cmd(sc, WI_CMD_ACCESS | WI_ACCESS_WRITE, rid, 0, 0);
}

/************************************************************/
/*  wi_write_val                                            */
/*                                                          */
/*  Write the value to the Resource Identifier.             */
/*  The function will write only a WORD                     */
/*                                                          */
/*  Return: OK or Error cause.                              */
/************************************************************/
static int wi_write_val(struct wi_softc *sc, int rid, u_int16_t val)
{
    val = htole16(val);
    return wi_write_rid(sc, rid, &val, sizeof(val));
}

/************************************************************/
/*  wi_write_txrate                                         */
/************************************************************/
static int wi_write_txrate(struct wi_softc *sc)
{
    struct ieee80211com *ic = &sc->sc_ic;
    int i;
    u_int16_t rate;

    if (ic->ic_fixed_rate < 0) {
        rate = 0;               /* auto */
    } else {
        rate = (ic->ic_sup_rates[IEEE80211_MODE_11B].rs_rates[ic->ic_fixed_rate] & IEEE80211_RATE_VAL) / 2;
    }

    /*
     * rate: 0, 1, 2, 5, 11
     */

    switch (sc->sc_firmware_type) {
    case WI_LUCENT:
        switch (rate) {
        case 0:                /* auto == 11mbps auto */
            rate = 3;
            break;
            /*
             * case 1, 2 map to 1, 2
             */
        case 5:                /* 5.5Mbps -> 4 */
            rate = 4;
            break;
        case 11:               /* 11mbps -> 5 */
            rate = 5;
            break;
        default:
            break;
        }
        break;
    default:
        /*
         * Choose a bit according to this table.
         * *
         * * bit | data rate
         * * ----+-------------------
         * * 0   | 1Mbps
         * * 1   | 2Mbps
         * * 2   | 5.5Mbps
         * * 3   | 11Mbps
         */
        for (i = 8; i > 0; i >>= 1) {
            if ((int) rate >= i) {
                break;
            }
        }
        if (i == 0) {
            rate = 0xf;         /* auto */
        } else {
            rate = i;
        }
        break;
    }
    return wi_write_val(sc, WI_RID_TX_RATE, rate);
}

/************************************************************/
/*  wi_reset                                                */
/************************************************************/
static int wi_reset(struct wi_softc *sc)
{
    //struct ieee80211com *ic = &sc->sc_ic; /* Harald: Not used */
    int i;
    int error = 0;
    int tries;

    /*
     * Symbol firmware cannot be initialized more than once
     */
    if (sc->sc_firmware_type == WI_SYMBOL && sc->sc_reset) {
        return (0);
    }
    if (sc->sc_firmware_type == WI_SYMBOL) {
        tries = 1;
    } else {
        tries = 3;
    }

    for (i = 0; i < tries; i++) {
        if ((error = wi_cmd(sc, WI_CMD_INI, 0, 0, 0)) == 0) {
            break;
        }
        DELAY(WI_DELAY * 1000);
    }
    sc->sc_reset = 1;

    if (i == tries) {
        Debug(("init failed\n"));
        return (error);
    }

    CSR_WRITE_2(sc, WI_INT_EN, 0);
    CSR_WRITE_2(sc, WI_EVENT_ACK, 0xFFFF);

    /*
     * Calibrate timer
     */
    wi_write_val(sc, WI_RID_TICK_TIME, 8);

    return (0);
}

/************************************************************/
/*  WLANInterrupt                                           */
/************************************************************/
static void WLANInterrupt(void *p)
{
    NUTDEVICE *dev = (NUTDEVICE *) p;
    struct wi_softc *sc = (struct wi_softc *) dev->dev_dcb;

    if ((sc->wi_gone == 1) || (sc->sc_enabled == 0)) {
        CSR_WRITE_2(sc, WI_INT_EN, 0);
        CSR_WRITE_2(sc, WI_EVENT_ACK, 0xFFFF);
        return;
    }

    /*
     * Disable interrupts. We will enable the interrupt
     * later in the RxThread.
     */
    CSR_WRITE_2(sc, WI_INT_EN, 0);

    sc->EventStatus = CSR_READ_2(sc, WI_EVENT_STAT);

    NutEventPostFromIrq(&sc->InterruptEvent);
}

/************************************************************/
/*  wi_tx_intr                                              */
/************************************************************/
static void wi_tx_intr(struct wi_softc *sc)
{
    // struct ieee80211com *ic = &sc->sc_ic; /* Harald: Not used. */
    int fid, cur;

    if (sc->wi_gone) {
        return;
    }

    fid = CSR_READ_2(sc, WI_ALLOC_FID);
    CSR_WRITE_2(sc, WI_EVENT_ACK, WI_EV_ALLOC);

    cur = sc->sc_txcur;
    if (sc->sc_txd[cur].d_fid != fid) {
#if (WLAN_ENABLE_TX_FRAME_DUMP >= 1)
        Debug(("bad alloc %x != %x, cur %d nxt %d\n", fid, sc->sc_txd[cur].d_fid, cur, sc->sc_txnext));
#endif
        return;
    }
    sc->sc_tx_timer = 0;
    sc->sc_txd[cur].d_len = 0;
    sc->sc_txcur = cur = (cur + 1) % sc->sc_ntxbuf;
    if (sc->sc_txd[cur].d_len == 0) {
        //ifp->if_flags &= ~IFF_OACTIVE;
    } else {
        if (wi_cmd(sc, WI_CMD_TX | WI_RECLAIM, sc->sc_txd[cur].d_fid, 0, 0)) {
            Debug(("xmit failed\n"));
            sc->sc_txd[cur].d_len = 0;
        } else {
            sc->sc_tx_timer = 5;
            //ifp->if_timer = 1;
        }
    }
}

/************************************************************/
/*  wi_info_intr                                            */
/************************************************************/
static void wi_info_intr(struct wi_softc *sc)
{
    struct ieee80211com *ic = &sc->sc_ic;
    //struct ifnet *ifp = &ic->ic_if;
    //int i, len, off; /* Harald: Not used */
    int fid;
    u_int16_t ltbuf[2];
    u_int16_t stat;
    //u_int32_t *ptr; /* Harald: Not used. */

    fid = CSR_READ_2(sc, WI_INFO_FID);
    wi_read_bap(sc, fid, 0, ltbuf, sizeof(ltbuf));

    switch (le16toh(ltbuf[1])) {

    case WI_INFO_LINK_STAT:
        wi_read_bap(sc, fid, sizeof(ltbuf), &stat, sizeof(stat));
#if (WLAN_ENABLE_LINK_STATUS >= 1)
        Debug(("wi_info_intr: LINK_STAT 0x%x\n", le16toh(stat)));
#endif                          /* (WLAN_ENABLE_LINK_STATUS >= 1) */

        switch (le16toh(stat)) {
        case WI_INFO_LINK_STAT_CONNECTED:
            sc->sc_flags &= ~WI_FLAGS_OUTRANGE;
            if (ic->ic_state == IEEE80211_S_RUN && ic->ic_opmode != IEEE80211_M_IBSS) {
                break;
            }
            /*
             * FALLTHROUGH
             */
        case WI_INFO_LINK_STAT_AP_CHG:
            ieee80211_new_state(ic, IEEE80211_S_RUN, -1);
            break;
        case WI_INFO_LINK_STAT_AP_INR:
            sc->sc_flags &= ~WI_FLAGS_OUTRANGE;
            break;
        case WI_INFO_LINK_STAT_AP_OOR:
            if (sc->sc_firmware_type == WI_SYMBOL && sc->sc_scan_timer > 0) {
                if (wi_cmd(sc, WI_CMD_INQUIRE, WI_INFO_HOST_SCAN_RESULTS, 0, 0)
                    != 0) {
                    sc->sc_scan_timer = 0;
                }
                break;
            }
            if (ic->ic_opmode == IEEE80211_M_STA) {
                sc->sc_flags |= WI_FLAGS_OUTRANGE;
            }
            break;
        case WI_INFO_LINK_STAT_DISCONNECTED:
        case WI_INFO_LINK_STAT_ASSOC_FAILED:
            if (ic->ic_opmode == IEEE80211_M_STA) {
                ieee80211_new_state(ic, IEEE80211_S_INIT, -1);
            }
            break;
        }
        break;

    case WI_INFO_COUNTERS:
#if 0                           //@@MF later
        /*
         * I think we does not need the counter,
         * therefore it can be take longer to implement it.
         */

        /*
         * some card versions have a larger stats structure
         */
        len = min(le16toh(ltbuf[0]) - 1, sizeof(sc->sc_stats) / 4);
        ptr = (u_int32_t *) & sc->sc_stats;
        off = sizeof(ltbuf);
        for (i = 0; i < len; i++, off += 2, ptr++) {
            wi_read_bap(sc, fid, off, &stat, sizeof(stat));
#ifdef WI_HERMES_STATS_WAR
            if (stat & 0xf000) {
                stat = ~stat;
            }
#endif
            *ptr += stat;
        }
        ifp->if_collisions = sc->sc_stats.wi_tx_single_retries + sc->sc_stats.wi_tx_multi_retries + sc->sc_stats.wi_tx_retry_limit;
#endif                          /* later */
        break;

#if 0                           //@@MF later
    case WI_INFO_SCAN_RESULTS:
    case WI_INFO_HOST_SCAN_RESULTS:
        wi_scan_result(sc, fid, le16toh(ltbuf[0]));
        break;
#endif                          /* later */

    default:
        Debug(("wi_info_intr: got fid %x type %x len %d\n", fid, le16toh(ltbuf[1]), le16toh(ltbuf[0])));
        break;
    }
    CSR_WRITE_2(sc, WI_EVENT_ACK, WI_EV_INFO);
}

/************************************************************/
/*  wi_rx_intr                                              */
/************************************************************/
static NETBUF *wi_rx_intr(struct wi_softc *sc)
{
    // struct ieee80211com *ic = &sc->sc_ic; /* Harald: Not used */
    struct wi_frame frmhdr;
    int fid, len, off;
    u_int16_t status;
    int error;

    NETBUF *nb = 0;
    LLCS_SNAP_HEADER *pLLCSSNAPHeader;
    ETHERHDR LABBuffer;
    ETHERHDR MAC802_3;
    u_int8_t *pFrameBuffer = 0; /* Harald: Missing init */
    u_int8_t *pBytePointer;
    int copy_size = 0;          /* Harald: Missing init */
    int size;
    int IPFrame = 0;

    if (sc->wi_gone) {
        Debug(("wi_rx_intr gone\r\n"));
        return (0);
    }

    fid = CSR_READ_2(sc, WI_RX_FID);

    /*
     * First read in the frame header
     */
    if (wi_read_bap(sc, fid, 0, &frmhdr, sizeof(frmhdr))) {
        CSR_WRITE_2(sc, WI_EVENT_ACK, WI_EV_RX);
        Debug(("wi_rx_intr: read fid %x failed\n", fid));
        return 0;
    }

    /*
     * Drop undecryptable or packets with receive errors here
     */
    status = le16toh(frmhdr.wi_status);
    if (status & WI_STAT_ERRSTAT) {
        CSR_WRITE_2(sc, WI_EVENT_ACK, WI_EV_RX);
        Debug(("wi_rx_intr: fid %x error status %x\n", fid, status));
        return 0;
    }

    /*
     * Catch the MAC 802.3 header
     */
    memcpy(MAC802_3.ether_dhost, frmhdr.wi_ehdr.ether_dhost, IEEE80211_ADDR_LEN);
    memcpy(MAC802_3.ether_shost, frmhdr.wi_ehdr.ether_shost, IEEE80211_ADDR_LEN);
    /*
     * The ether_type comes later
     */

    /*
     * Get the frame length, at this point I don't know
     * if it is a RFC1024 or RFC894 frame. But Ethernut need
     * an RFC894 frame! So we will alloc the NetBuffer later.
     */
    len = le16toh(frmhdr.wi_dat_len);
    off = sizeof(frmhdr);

    /*
     * Sometimes the PRISM2.x returns bogusly large frames. Except
     * in monitor mode, just throw them away.
     */
    if (len > WLAN_MAX_ETHERNET_FRAME_LEN) {
        CSR_WRITE_2(sc, WI_EVENT_ACK, WI_EV_RX);
        Debug(("wi_rx_intr: oversized packet\n"));
        return (0);
    }

    /*
     * First read the LookAheadBuffer
     */
    error = wi_read_bap(sc, fid, off, &LABBuffer, sizeof(LABBuffer));
    off += sizeof(LABBuffer);

    if (error == 0) {
        /*
         * Check if this is a 802.2 LLC 802.2 SNAP IP-Header RFC1024
         */
        pLLCSSNAPHeader = (LLCS_SNAP_HEADER *) & LABBuffer;
        if ((pLLCSSNAPHeader->DSAPSSAP == 0xAAAA) && (pLLCSSNAPHeader->Control == 0x0003) && (pLLCSSNAPHeader->MustZero == 0x0000)) {

            if ((pLLCSSNAPHeader->Type == 0x0608) || (pLLCSSNAPHeader->Type == 0x0008)) {

#if (WLAN_ENABLE_RX_FRAME_DUMP >= 1)
                Debug(("RFC1024 IP-Frame\n"));
#endif

                IPFrame = 1;
                MAC802_3.ether_type = pLLCSSNAPHeader->Type;

                /*
                 * Now we can calculate the wFrameLength.
                 * It is a RFC1042 Frame, and we must discard the:
                 * DSAP, SSAP, cntl, and the org. code
                 *  1  +  1  +  1        +  3  = 6
                 */
                len -= 6;

                /*
                 * The Ethernut need the dest and source address,
                 * therefore add 12 again.
                 */
                len += 12;

                /*
                 * The MAC can not handle odd counts, test it.
                 *
                 */
                len += 1;
                len &= ~1;

                /*
                 * Now alloc the NetBuffer for Ethernut.
                 */
                nb = NutNetBufAlloc(0, NBAF_DATALINK, len);
                if (nb != 0) {
                    pFrameBuffer = nb->nb_dl.vp;

                    /*
                     * We must copy wFrameLength out of the MAC
                     */
                    copy_size = len;

                    /*
                     * Copy the MAC 802.3 Header
                     */
                    memcpy(pFrameBuffer, &MAC802_3, sizeof(MAC802_3));
                    pFrameBuffer += sizeof(MAC802_3);
                    copy_size -= sizeof(MAC802_3);      /* This was already in the WLANHeader */

                    /*
                     * Now we have some data in our LABBuffer which we need
                     */
                    pBytePointer = (BYTE *) & LABBuffer;
                    /*
                     * jump over the header
                     */
                    pBytePointer += sizeof(LLCS_SNAP_HEADER);
                    /*
                     * and copy the rest
                     */
                    size = sizeof(LABBuffer) - sizeof(LLCS_SNAP_HEADER);
                    memcpy(pFrameBuffer, pBytePointer, size);
                    /*
                     * Change the pointer for the "global copy"
                     */
                    pFrameBuffer += size;
                    copy_size -= size;  /* This was already in the LookAheadBuffer */
                } else {
                    IPFrame = 0;
                }
            }                   /* only ARP and IP */
        } else {
            /*
             * Check if this is a RFX894 Frame
             */
            if ((LABBuffer.ether_type == 0x0608) || (LABBuffer.ether_type == 0x0008)) {

#if (WLAN_ENABLE_RX_FRAME_DUMP >= 1)
                Debug(("RFC894 IP-Frame\n"));
#endif

                IPFrame = 1;

                /*
                 * The MAC can not handle odd counts, test it.
                 *
                 */
                len += 1;
                len &= ~1;

                /*
                 * The FrameLength is now the correct length for an RFC894 Frame,
                 * Now alloc the NetBuffer for Ethernut.
                 */
                nb = NutNetBufAlloc(0, NBAF_DATALINK, len);
                if (nb != 0) {
                    pFrameBuffer = nb->nb_dl.vp;

                    /*
                     * We must copy wFrameLength out of the MAC
                     */
                    copy_size = len;

                    /*
                     * The LookAheadBuffer is the MAC-Header
                     */
                    memcpy(pFrameBuffer, &LABBuffer, sizeof(LABBuffer));
                    pFrameBuffer += sizeof(LABBuffer);
                    copy_size -= sizeof(LABBuffer);     /* This was our LookAheadBuffer */
                } else {
                    IPFrame = 0;
                }               /* NutNetBufAlloc */
            }                   /* RFC894 Frame */
        }                       /* Error read LookAheadBuffer */

        /*
         * Do the rest here, "global copy"
         */
        if (IPFrame == 1) {
            error = wi_read_bap(sc, fid, off, pFrameBuffer, copy_size);
            if (error != 0) {
                NutNetBufFree(nb);
                nb = NULL;
            } else {
#if (WLAN_ENABLE_RX_FRAME_DUMP >= 1)
                Debug(("RxFrame: %d\n", len));
                DumpWlanData(nb->nb_dl.vp, nb->nb_dl.sz);
#endif
            }
        }
    }

    /*
     * At least, send a ack for the event
     */
    CSR_WRITE_2(sc, WI_EVENT_ACK, WI_EV_RX);

    return (nb);
}

/************************************************************/
/*  RxThread                                                */
/************************************************************/
THREAD(RxThread, arg)
{
    NUTDEVICE *dev;
    IFNET *ifn;
    NETBUF *nb;
    struct wi_softc *sc;
    u_int16_t EventStatus;

    dev = arg;
    ifn = (IFNET *) dev->dev_icb;
    sc = (struct wi_softc *) dev->dev_dcb;

    for (;;) {
        /*
         * Wait for the arrival of new packets or check
         * the receiver every two second.
         */
        NutEventWait(&sc->InterruptEvent, 2000);
        EventStatus = sc->EventStatus;

        /*
         * Clear the sc-EventStatus. If the next time the
         * EventStatus is 0, it was the timeout and not
         * an interrupt.
         */
        sc->EventStatus = 0;

        if (EventStatus != 0) {
            if (EventStatus & WI_EV_RX) {
                WI_LOCK(sc);
                nb = wi_rx_intr(sc);
                WI_UNLOCK(sc);

                if (nb != 0) {
                    (*ifn->if_recv) (dev, nb);
                }
            }

            if (EventStatus & WI_EV_ALLOC) {
                WI_LOCK(sc);
                wi_tx_intr(sc);
                WI_UNLOCK(sc);
            }

            if (EventStatus & WI_EV_INFO) {
                WI_LOCK(sc);
                wi_info_intr(sc);
                WI_UNLOCK(sc);
            }

            if (EventStatus & WI_EV_TX_EXC) {
                Debug(("WI_EV_TX_EXC\n"));
            }


            CSR_WRITE_2(sc, WI_INT_EN, WI_INTRS);
        } else {
            /*
             * It was the timeout, check if the card is alive
             */
            if ((sc->sc_enabled == 1) && (sc->wi_gone == 1)) {
                /*
                 * The card was enabled but we has lost the card, reinit...
                 */
                Debug(("Houston, we have a problem...\n"));
                sc->wi_gone = 0;
                wlandrv_Init(dev);
            }
        }                       /* (EventStatus != 0) */
    }                           /* for */
}

/************************************************************/
/*  wi_alloc_fid                                            */
/************************************************************/
static int wi_alloc_fid(struct wi_softc *sc, int len, int *idp)
{
    unsigned long i;                     /* Harald: Changed from int to long */
    									 /* Oliver: Changed from signed to unsigned */

    if (wi_cmd(sc, WI_CMD_ALLOC_MEM, len, 0, 0)) {
        Debug(("failed to allocate %d bytes on NIC\n", len));
        return ENOMEM;
    }

    for (i = 0; i < WI_TIMEOUT; i++) {
        if (CSR_READ_2(sc, WI_EVENT_STAT) & WI_EV_ALLOC) {
            break;
        }
        if (i == WI_TIMEOUT) {
            Debug(("timeout in alloc\n"));
            return ETIMEDOUT;
        }
        DELAY(1);
    }
    *idp = CSR_READ_2(sc, WI_ALLOC_FID);
    CSR_WRITE_2(sc, WI_EVENT_ACK, WI_EV_ALLOC);
    return 0;
}

/************************************************************/
/*  wi_write_ssid                                           */
/************************************************************/
static int wi_write_ssid(struct wi_softc *sc, int rid, u_int8_t * buf, int buflen)
{
    struct wi_ssid ssid;

    if (buflen > IEEE80211_NWID_LEN) {
        return ENOBUFS;
    }
    memset(&ssid, 0, sizeof(ssid));
    ssid.wi_len = htole16(buflen);
    memcpy(ssid.wi_ssid, buf, buflen);
    return wi_write_rid(sc, rid, &ssid, sizeof(ssid));
}

/************************************************************/
/*  wi_read_nicid                                           */
/************************************************************/
static void wi_read_nicid(struct wi_softc *sc)
{
    struct wi_card_ident *id;
    char *p;
    int len;
    u_int16_t ver[4];

    /*
     * getting chip identity
     */
    memset(ver, 0, sizeof(ver));
    len = sizeof(ver);
    wi_read_rid(sc, WI_RID_CARD_ID, ver, &len);
    Debug(("using "));

    sc->sc_firmware_type = WI_NOTYPE;
    for (id = wi_card_ident; id->card_name != NULL; id++) {
        if (le16toh(ver[0]) == id->card_id) {
            Debug(("%s", id->card_name));
            sc->sc_firmware_type = id->firm_type;
            break;
        }
    }
    if (sc->sc_firmware_type == WI_NOTYPE) {
        if (le16toh(ver[0]) & 0x8000) {
            Debug(("Unknown PRISM2 chip"));
            sc->sc_firmware_type = WI_INTERSIL;
        } else {
            Debug(("Unknown Lucent chip"));
            sc->sc_firmware_type = WI_LUCENT;
        }
    }

    /*
     * get primary firmware version (Only Prism chips)
     */
    if (sc->sc_firmware_type != WI_LUCENT) {
        memset(ver, 0, sizeof(ver));
        len = sizeof(ver);
        wi_read_rid(sc, WI_RID_PRI_IDENTITY, ver, &len);
        sc->sc_pri_firmware_ver = le16toh(ver[2]) * 10000 + le16toh(ver[3]) * 100 + le16toh(ver[1]);
    }

    /*
     * get station firmware version
     */
    memset(ver, 0, sizeof(ver));
    len = sizeof(ver);
    wi_read_rid(sc, WI_RID_STA_IDENTITY, ver, &len);
    sc->sc_sta_firmware_ver = le16toh(ver[2]) * 10000 + le16toh(ver[3]) * 100 + le16toh(ver[1]);
    if (sc->sc_firmware_type == WI_INTERSIL && (sc->sc_sta_firmware_ver == 10102 || sc->sc_sta_firmware_ver == 20102)) {
        char ident[12];
        memset(ident, 0, sizeof(ident));
        len = sizeof(ident);
        /*
         * value should be the format like "V2.00-11"
         */
        if (wi_read_rid(sc, WI_RID_SYMBOL_IDENTITY, ident, &len) == 0 &&
            *(p = (char *) ident) >= 'A' && p[2] == '.' && p[5] == '-' && p[8] == '\0') {
            sc->sc_firmware_type = WI_SYMBOL;
            sc->sc_sta_firmware_ver = (p[1] - '0') * 10000 +
                (p[3] - '0') * 1000 + (p[4] - '0') * 100 + (p[6] - '0') * 10 + (p[7] - '0');
        }
    }
    Debug(("\n"));
    Debug(("%s Firmware: ",
           sc->sc_firmware_type == WI_LUCENT ? "Lucent" : (sc->sc_firmware_type == WI_SYMBOL ? "Symbol" : "Intersil")));
    if (sc->sc_firmware_type != WI_LUCENT) {    /* XXX */
        Debug(("Primary (%ld.%ld.%ld), ",
               sc->sc_pri_firmware_ver / 10000, (sc->sc_pri_firmware_ver % 10000) / 100, sc->sc_pri_firmware_ver % 100));
    }
    Debug(("Station (%ld.%ld.%ld)\n",
           sc->sc_sta_firmware_ver / 10000, (sc->sc_sta_firmware_ver % 10000) / 100, sc->sc_sta_firmware_ver % 100));
}

/************************************************************/
/*  wi_write_wep                                            */
/************************************************************/
static int wi_write_wep(struct wi_softc *sc)
{
    struct ieee80211com *ic = &sc->sc_ic;
    int error = 0;
    int i, keylen;
    u_int16_t val;
    struct wi_key wkey[IEEE80211_WEP_NKID];

    switch (sc->sc_firmware_type) {
#if (WLAN_SUPPORT_LUCENT >= 1)
    case WI_LUCENT:
        val = (ic->ic_flags & IEEE80211_F_WEPON) ? 1 : 0;
        error = wi_write_val(sc, WI_RID_ENCRYPTION, val);
        if (error)
            break;
        error = wi_write_val(sc, WI_RID_TX_CRYPT_KEY, ic->ic_wep_txkey);
        if (error)
            break;
        memset(wkey, 0, sizeof(wkey));
        for (i = 0; i < IEEE80211_WEP_NKID; i++) {
            keylen = ic->ic_nw_keys[i].wk_len;
            wkey[i].wi_keylen = htole16(keylen);
            memcpy(wkey[i].wi_keydat, ic->ic_nw_keys[i].wk_key, keylen);
        }
        error = wi_write_rid(sc, WI_RID_DEFLT_CRYPT_KEYS, wkey, sizeof(wkey));
        break;
#endif                          /* (WLAN_SUPPORT_LUCENT >= 1) */

#if ((WLAN_SUPPORT_INTERSIL >= 1) || (WLAN_SUPPORT_SYMBOL >= 1))
    case WI_INTERSIL:
    case WI_SYMBOL:
        if (ic->ic_flags & IEEE80211_F_WEPON) {
            /*
             * ONLY HWB3163 EVAL-CARD Firmware version
             * less than 0.8 variant2
             *
             *   If promiscuous mode disable, Prism2 chip
             *  does not work with WEP .
             * It is under investigation for details.
             * (ichiro@netbsd.org)
             */
            if (sc->sc_firmware_type == WI_INTERSIL && sc->sc_sta_firmware_ver < 802) {
                /*
                 * firm ver < 0.8 variant 2
                 */
                wi_write_val(sc, WI_RID_PROMISC, 1);
            }
            wi_write_val(sc, WI_RID_CNFAUTHMODE, sc->sc_cnfauthmode);
            val = PRIVACY_INVOKED | EXCLUDE_UNENCRYPTED;
            /*
             * Encryption firmware has a bug for HostAP mode.
             */
            if (sc->sc_firmware_type == WI_INTERSIL && ic->ic_opmode == IEEE80211_M_HOSTAP) {
                val |= HOST_ENCRYPT;
            }
        } else {
            wi_write_val(sc, WI_RID_CNFAUTHMODE, IEEE80211_AUTH_OPEN);
            val = HOST_ENCRYPT | HOST_DECRYPT;
        }
        error = wi_write_val(sc, WI_RID_P2_ENCRYPTION, val);
        if (error) {
            break;
        }
        error = wi_write_val(sc, WI_RID_P2_TX_CRYPT_KEY, ic->ic_wep_txkey);
        if (error) {
            break;
        }
        /*
         * It seems that the firmware accept 104bit key only if
         * all the keys have 104bit length.  We get the length of
         * the transmit key and use it for all other keys.
         * Perhaps we should use software WEP for such situation.
         */
        keylen = ic->ic_nw_keys[ic->ic_wep_txkey].wk_len;
        if (keylen > IEEE80211_WEP_KEYLEN) {
            keylen = 13;        /* 104bit keys */
        } else {
            keylen = IEEE80211_WEP_KEYLEN;
        }
        for (i = 0; i < IEEE80211_WEP_NKID; i++) {
            error = wi_write_rid(sc, WI_RID_P2_CRYPT_KEY0 + i, ic->ic_nw_keys[i].wk_key, keylen);
            if (error) {
                break;
            }
        }
        break;
#endif                          /* ((WLAN_SUPPORT_INTERSIL >= 1) || (WLAN_SUPPORT_SYMBOL >= 1)) */
    }
    return error;
}

/*==========================================================*/
/*  DEFINE: All code exported                               */
/*==========================================================*/
/************************************************************/
/*  wlandrv_ProbeDevice                                     */
/*                                                          */
/*  Make a HW-Reset and test if a card is available.        */
/*                                                          */
/*  Return: ERROR, OK                                       */
/************************************************************/
int wlandrv_ProbeDevice(void)
{
    int error = -1;
    u_int16_t Value;

    /*
     * Set RESET
     */
    RESET_EN_PORT |= RESET_BIT;
    RESET_PORT |= RESET_BIT;

    NutSleep(100);

    /*
     * Remove RESET
     */
    RESET_PORT &= ~RESET_BIT;

    NutSleep(500);

    /*
     * Set the MAC into I/O mode
     */
    pcmcia_WriteMem(WI_COR_OFFSET, WI_COR_VALUE);
    NutSleep(100);

    /*
     * Test if we can found a card
     */
    pcmcia_WriteReg(WI_HFA384X_SWSUPPORT0_OFF, WI_PRISM2STA_MAGIC);
    Value = pcmcia_ReadReg(WI_HFA384X_SWSUPPORT0_OFF);
    if (Value == WI_PRISM2STA_MAGIC) {
        /*
         * We found a card :-)
         */
        error = 0;
    }

    return (error);
}

/************************************************************/
/*  wlandrv_Attach                                          */
/*                                                          */
/*  Catch all information about the card which              */
/*  could be found. And set some default parameter          */
/*                                                          */
/*  Return: ERROR, OK                                       */
/************************************************************/
int wlandrv_Attach(device_t dev)
{
    struct wi_softc *sc = device_get_softc(dev);
    struct ieee80211com *ic = &sc->sc_ic;
    struct ieee80211_rateset *rs;
    int i, nrates, buflen;
    u_int16_t val;
    u_int8_t ratebuf[2 + IEEE80211_RATE_SIZE];
    //u_int8_t empty_macaddr[IEEE80211_ADDR_LEN] = {
    //  0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    //}; /* Harald: This is unused */
    int error;

    bDebugState = 2;

    sc->wi_cmd_count = 500;
    ic->ic_fixed_rate = -1;     /* auto */


    /*
     * Reset the NIC.
     */
    if (wi_reset(sc) != 0) {
        return ENXIO;           /* XXX */
    }

    /*
     * Read the station address.
     * And do it twice. I've seen PRISM-based cards that return
     * an error when trying to read it the first time, which causes
     * the probe to fail.
     */
    buflen = IEEE80211_ADDR_LEN;
    error = wi_read_rid(sc, WI_RID_MAC_NODE, ic->ic_myaddr, &buflen);
    if (error != 0) {
        buflen = IEEE80211_ADDR_LEN;
        error = wi_read_rid(sc, WI_RID_MAC_NODE, ic->ic_myaddr, &buflen);
    }
    if (error != 0) {
        Debug(("mac read failed %d\n", error));
        return (error);
    }
    Debug(("802.11 address: %02X-%02X-%02X-%02X-%02X-%02X\n",
           ic->ic_myaddr[0], ic->ic_myaddr[1], ic->ic_myaddr[2], ic->ic_myaddr[3], ic->ic_myaddr[4], ic->ic_myaddr[5]));

    /*
     * Read NIC identification
     */
    wi_read_nicid(sc);

    ic->ic_opmode = IEEE80211_M_STA;
    ic->ic_caps = IEEE80211_C_PMGT | IEEE80211_C_AHDEMO;
    ic->ic_state = IEEE80211_S_INIT;

    /*
     * Query the card for available channels.
     */
    buflen = sizeof(val);
    if (wi_read_rid(sc, WI_RID_CHANNEL_LIST, &val, &buflen) != 0) {
        val = htole16(0x1fff);  /* assume 1-13 */
    }
    if (val == 0) {
        Debug(("wi_attach: no available channels listed!"));
    }
    ic->ChannelList = (val << 1);

    /*
     * Read the default channel from the NIC. This may vary
     * depending on the country where the NIC was purchased, so
     * we can't hard-code a default and expect it to work for
     * everyone.
     *
     * If no channel is specified, let the 802.11 code select.
     */
    buflen = sizeof(val);
    if (wi_read_rid(sc, WI_RID_OWN_CHNL, &val, &buflen) == 0) {
        val = le16toh(val);
        i = (1 << val);

        if ((val > 16) || ((ic->ChannelList & i) == 0)) {
            Debug(("wi_attach: invalid own channel %d!", val));
        }
        ic->ic_ibss_chan = val;
    } else {
        Debug(("WI_RID_OWN_CHNL failed, using first channel!\n"));
        ic->ic_ibss_chan = 1;
    }

    /*
     * Set flags based on firmware version.
     */
    switch (sc->sc_firmware_type) {
#if (WLAN_SUPPORT_LUCENT >= 1)
    case WI_LUCENT:
        sc->sc_ntxbuf = 1;
        sc->sc_flags |= WI_FLAGS_HAS_SYSSCALE;

#ifdef WI_HERMES_AUTOINC_WAR
        /*
         * XXX: not confirmed, but never seen for recent firmware
         */
        if (sc->sc_sta_firmware_ver < 40000) {
            sc->sc_flags |= WI_FLAGS_BUG_AUTOINC;
        }
#endif
        if (sc->sc_sta_firmware_ver >= 60000) {
            sc->sc_flags |= WI_FLAGS_HAS_MOR;
        }
        if (sc->sc_sta_firmware_ver >= 60006) {
            ic->ic_caps |= IEEE80211_C_IBSS;
            ic->ic_caps |= IEEE80211_C_MONITOR;
        }
        sc->sc_ibss_port = htole16(1);

        sc->sc_min_rssi = WI_LUCENT_MIN_RSSI;
        sc->sc_max_rssi = WI_LUCENT_MAX_RSSI;
        sc->sc_dbm_offset = WI_LUCENT_DBM_OFFSET;
        break;
#endif                          /* (WLAN_SUPPORT_LUCENT >= 1) */

#if (WLAN_SUPPORT_INTERSIL >= 1)
    case WI_INTERSIL:
        sc->sc_ntxbuf = WI_NTXBUF;
        sc->sc_flags |= WI_FLAGS_HAS_FRAGTHR;
        sc->sc_flags |= WI_FLAGS_HAS_ROAMING;
        sc->sc_flags |= WI_FLAGS_HAS_SYSSCALE;
        /*
         * Old firmware are slow, so give peace a chance.
         */
        if (sc->sc_sta_firmware_ver < 10000) {
            sc->wi_cmd_count = 5000;
        }
        if (sc->sc_sta_firmware_ver > 10101) {
            sc->sc_flags |= WI_FLAGS_HAS_DBMADJUST;
        }
        if (sc->sc_sta_firmware_ver >= 800) {
            ic->ic_caps |= IEEE80211_C_IBSS;
            ic->ic_caps |= IEEE80211_C_MONITOR;
        }
        /*
         * version 0.8.3 and newer are the only ones that are known
         * to currently work.  Earlier versions can be made to work,
         * at least according to the Linux driver.
         */
        if (sc->sc_sta_firmware_ver >= 803) {
            ic->ic_caps |= IEEE80211_C_HOSTAP;
        }
        sc->sc_ibss_port = htole16(0);

        sc->sc_min_rssi = WI_PRISM_MIN_RSSI;
        sc->sc_max_rssi = WI_PRISM_MAX_RSSI;
        sc->sc_dbm_offset = WI_PRISM_DBM_OFFSET;
        break;
#endif                          /* (WLAN_SUPPORT_INTERSIL >= 1) */

#if (WLAN_SUPPORT_SYMBOL >= 1)
    case WI_SYMBOL:
        sc->sc_ntxbuf = 1;
        sc->sc_flags |= WI_FLAGS_HAS_DIVERSITY;
        if (sc->sc_sta_firmware_ver >= 25000) {
            ic->ic_caps |= IEEE80211_C_IBSS;
        }
        sc->sc_ibss_port = htole16(4);

        sc->sc_min_rssi = WI_PRISM_MIN_RSSI;
        sc->sc_max_rssi = WI_PRISM_MAX_RSSI;
        sc->sc_dbm_offset = WI_PRISM_DBM_OFFSET;
        break;
#endif                          /* (WLAN_SUPPORT_SYMBOL >= 1) */
    }

    /*
     * Find out if we support WEP on this card.
     */
    buflen = sizeof(val);
    if (wi_read_rid(sc, WI_RID_WEP_AVAIL, &val, &buflen) == 0 && val != htole16(0)) {
        ic->ic_caps |= IEEE80211_C_WEP;
    }

    /*
     * Find supported rates.
     */
    buflen = sizeof(ratebuf);
    rs = &ic->ic_sup_rates[IEEE80211_MODE_11B];
    if (wi_read_rid(sc, WI_RID_DATA_RATES, ratebuf, &buflen) == 0) {
        nrates = le16toh(*(u_int16_t *) ratebuf);
        if (nrates > IEEE80211_RATE_MAXSIZE) {
            nrates = IEEE80211_RATE_MAXSIZE;
        }
        rs->rs_nrates = 0;
        for (i = 0; i < nrates; i++) {
            if (ratebuf[2 + i]) {
                rs->rs_rates[rs->rs_nrates++] = ratebuf[2 + i];
            }
        }
    } else {
        /*
         * XXX fallback on error?
         */
        rs->rs_nrates = 0;
    }

    buflen = sizeof(val);
    if ((sc->sc_flags & WI_FLAGS_HAS_DBMADJUST) && wi_read_rid(sc, WI_RID_DBM_ADJUST, &val, &buflen) == 0) {
        sc->sc_dbm_offset = le16toh(val);
    }

    sc->sc_max_datalen = 2304;
    sc->sc_system_scale = 1;
    sc->sc_cnfauthmode = IEEE80211_AUTH_OPEN;
    sc->sc_roaming_mode = WI_DEFAULT_ROAMING;

    sc->sc_portnum = WI_DEFAULT_PORT;
    sc->sc_authtype = WI_DEFAULT_AUTHTYPE;

    /*
     * Init DeviceSemaphore
     */
    NutEventPost(&hDeviceSemaphore);

    return (0);
}

/************************************************************/
/*  wlandrv_Init                                            */
/************************************************************/
void wlandrv_Init(device_t dev)
{
    struct wi_softc *sc = device_get_softc(dev);
    struct ieee80211com *ic = &sc->sc_ic;
    int i;
    int error = 0, wasenabled;

    WI_LOCK(sc);

    if (sc->wi_gone) {
        WI_UNLOCK(sc);
        return;
    }

    if ((wasenabled = sc->sc_enabled)) {
        wi_stop(sc, 1);
    }
    wi_reset(sc);

    /*
     * common 802.11 configuration
     */
    ic->ic_flags &= ~IEEE80211_F_IBSSON;
    sc->sc_flags &= ~WI_FLAGS_OUTRANGE;
    switch (ic->ic_opmode) {
    case IEEE80211_M_STA:
        wi_write_val(sc, WI_RID_PORTTYPE, WI_PORTTYPE_BSS);
        break;
    case IEEE80211_M_IBSS:
        wi_write_val(sc, WI_RID_PORTTYPE, sc->sc_ibss_port);
        ic->ic_flags |= IEEE80211_F_IBSSON;
        break;
    case IEEE80211_M_AHDEMO:
        wi_write_val(sc, WI_RID_PORTTYPE, WI_PORTTYPE_ADHOC);
        break;
    case IEEE80211_M_HOSTAP:
        /*
         * For PRISM cards, override the empty SSID, because in
         * HostAP mode the controller will lock up otherwise.
         */
        if (sc->sc_firmware_type == WI_INTERSIL && ic->ic_des_esslen == 0) {
            ic->ic_des_essid[0] = ' ';
            ic->ic_des_esslen = 1;
        }
        wi_write_val(sc, WI_RID_PORTTYPE, WI_PORTTYPE_HOSTAP);
        break;
    case IEEE80211_M_MONITOR:
        /* Harald: Should be handled. */
#if 0                           // @@MF this mode is not working with my card
        if (sc->sc_firmware_type == WI_LUCENT) {
            wi_write_val(sc, WI_RID_PORTTYPE, WI_PORTTYPE_ADHOC);
        }
        error = wi_cmd(sc, WI_CMD_DEBUG | (WI_TEST_MONITOR << 8), 0, 0, 0);
#endif
        break;
    }

    /*
     * Intersil interprets this RID as joining ESS even in IBSS mode
     */
    if (sc->sc_firmware_type == WI_LUCENT && (ic->ic_flags & IEEE80211_F_IBSSON) && ic->ic_des_esslen > 0) {
        wi_write_val(sc, WI_RID_CREATE_IBSS, 1);
    } else {
        wi_write_val(sc, WI_RID_CREATE_IBSS, 0);
    }

#if 0                           //@@MF later
    wi_write_val(sc, WI_RID_MAX_SLEEP, ic->ic_lintval);
#endif                          /* later */

    wi_write_ssid(sc, WI_RID_DESIRED_SSID, ic->ic_des_essid, ic->ic_des_esslen);
    wi_write_val(sc, WI_RID_OWN_CHNL, ic->ic_ibss_chan);
    wi_write_ssid(sc, WI_RID_OWN_SSID, ic->ic_des_essid, ic->ic_des_esslen);

    wi_write_rid(sc, WI_RID_MAC_NODE, ic->ic_myaddr, IEEE80211_ADDR_LEN);

    wi_write_val(sc, WI_RID_PM_ENABLED, (ic->ic_flags & IEEE80211_F_PMGTON) ? 1 : 0);

    /*
     * not yet common 802.11 configuration
     */
    wi_write_val(sc, WI_RID_MAX_DATALEN, sc->sc_max_datalen);

#if 0                           //@@MF later
    wi_write_val(sc, WI_RID_RTS_THRESH, ic->ic_rtsthreshold);
    if (sc->sc_flags & WI_FLAGS_HAS_FRAGTHR) {
        wi_write_val(sc, WI_RID_FRAG_THRESH, ic->ic_fragthreshold);
    }
#endif                          /* later */

    /*
     * driver specific 802.11 configuration
     */
    if (sc->sc_flags & WI_FLAGS_HAS_SYSSCALE) {
        wi_write_val(sc, WI_RID_SYSTEM_SCALE, sc->sc_system_scale);
    }
    if (sc->sc_flags & WI_FLAGS_HAS_ROAMING) {
        wi_write_val(sc, WI_RID_ROAMING_MODE, sc->sc_roaming_mode);
    }
    if (sc->sc_flags & WI_FLAGS_HAS_MOR) {
        wi_write_val(sc, WI_RID_MICROWAVE_OVEN, sc->sc_microwave_oven);
    }
    wi_write_txrate(sc);

#if 0                           //@@MF later
    /*
     * What is a nodename ??? :-(
     */
    wi_write_ssid(sc, WI_RID_NODENAME, sc->sc_nodename, sc->sc_nodelen);
#endif                          /* later */

#if 0                           //@@MF later
    /*
     * Does we realy need HOSTAP on Ethernut??
     */
    if (ic->ic_opmode == IEEE80211_M_HOSTAP && sc->sc_firmware_type == WI_INTERSIL) {
        wi_write_val(sc, WI_RID_OWN_BEACON_INT, ic->ic_lintval);
        wi_write_val(sc, WI_RID_BASIC_RATE, 0x03);      /* 1, 2 */
        wi_write_val(sc, WI_RID_SUPPORT_RATE, 0x0f);    /* 1, 2, 5.5, 11 */
        wi_write_val(sc, WI_RID_DTIM_PERIOD, 1);
    }
#endif                          /* later */

    /*
     *  Initialize promisc mode.
     *  Being in the Host-AP mode causes a great
     *  deal of pain if primisc mode is set.
     *  Therefore we avoid confusing the firmware
     *  and always reset promisc mode in Host-AP
     *  mode.  Host-AP sees all the packets anyway.
     */
    if ((ic->ic_opmode != IEEE80211_M_HOSTAP) && (sc->PromiscuousMode != 0)) {
        wi_write_val(sc, WI_RID_PROMISC, 1);
    } else {
        wi_write_val(sc, WI_RID_PROMISC, 0);
    }

    /*
     * Configure WEP.
     */
    if (ic->ic_caps & IEEE80211_C_WEP) {
        wi_write_wep(sc);
    }
#if 0                           //@@MF later
    /*
     * Set multicast filter.
     */
    wi_write_multi(sc);
#endif                          /* later */

    /*
     * Allocate fids for the card
     */
    if (sc->sc_firmware_type != WI_SYMBOL || !wasenabled) {
        sc->sc_buflen = IEEE80211_MAX_LEN + sizeof(struct wi_frame);
        if (sc->sc_firmware_type == WI_SYMBOL) {
            sc->sc_buflen = 1585;       /* XXX */
        }
        for (i = 0; i < sc->sc_ntxbuf; i++) {
            error = wi_alloc_fid(sc, sc->sc_buflen, &sc->sc_txd[i].d_fid);
            if (error) {
                Debug(("tx buffer allocation failed (error %u)\n", error));
                goto out;
            }
            sc->sc_txd[i].d_len = 0;
        }
    }
    sc->sc_txcur = sc->sc_txnext = 0;

    /*
     * Enable desired port
     */
    wi_cmd(sc, WI_CMD_ENABLE | sc->sc_portnum, 0, 0, 0);

    sc->sc_enabled = 1;
    if (ic->ic_opmode == IEEE80211_M_AHDEMO || ic->ic_opmode == IEEE80211_M_MONITOR || ic->ic_opmode == IEEE80211_M_HOSTAP) {
        ieee80211_new_state(ic, IEEE80211_S_RUN, -1);
    }

  /*************************************************************************/
    /*                  Set interrupt and start RxThread                     */
  /*************************************************************************/
    if (sc->InterruptInitDone == 0) {
        sc->InterruptInitDone = 1;
        /*
         * Install WLAN interrupt
         */
        PORTE |= 0x80;          /* Enable PullUp */
        error = NutRegisterIrqHandler(&sig_INTERRUPT7, WLANInterrupt, dev);
        if (error == FALSE) {
            EICR |= 0x80;       // falling edge
            sbi(EIMSK, INT7);

            /*
             * Start the receiver thread.
             */
            NutThreadCreate("rxi7", RxThread, dev, 640);
        }
    }
  /*************************************************************************/


    /*
     * Enable interrupts
     */
    CSR_WRITE_2(sc, WI_INT_EN, WI_INTRS);

    if (!wasenabled && ic->ic_opmode == IEEE80211_M_HOSTAP && sc->sc_firmware_type == WI_INTERSIL) {
        /*
         * XXX: some card need to be re-enabled for hostap
         */
        wi_cmd(sc, WI_CMD_DISABLE | WI_PORT0, 0, 0, 0);
        wi_cmd(sc, WI_CMD_ENABLE | WI_PORT0, 0, 0, 0);
    }
#if 0                           //@@MF later
    /*
     * This is only for IEEE80211_M_STA
     */
    if (ic->ic_opmode == IEEE80211_M_STA && ((ic->ic_flags & IEEE80211_F_DESBSSID) || ic->ic_des_chan != IEEE80211_CHAN_ANYC)) {
        memset(&join, 0, sizeof(join));
        if (ic->ic_flags & IEEE80211_F_DESBSSID)
            IEEE80211_ADDR_COPY(&join.wi_bssid, ic->ic_des_bssid);
        if (ic->ic_des_chan != IEEE80211_CHAN_ANYC)
            join.wi_chan = htole16(ieee80211_chan2ieee(ic, ic->ic_des_chan));
        /*
         * Lucent firmware does not support the JOIN RID.
         */
        if (sc->sc_firmware_type != WI_LUCENT)
            wi_write_rid(sc, WI_RID_JOIN_REQ, &join, sizeof(join));
    }
#endif                          /* later */

    WI_UNLOCK(sc);
    return;
  out:
    if (error) {
        Debug(("interface not running\n"));
        wi_stop(sc, 1);
    }
    WI_UNLOCK(sc);
    Debug(("wi_init: return %d\n", error));
    return;
}

/************************************************************/
/*  wlandrv_PutPacket                                       */
/************************************************************/
int wlandrv_PutPacket(NUTDEVICE * dev, NETBUF * nb)
{
    int error = -1;
    struct wi_softc *sc = (struct wi_softc *) dev->dev_dcb;
    struct wi_frame frmhdr;
    LLCS_SNAP_HEADER LLCSSNAPHeader;
    ETHERHDR *pMAC8023Header;
    int cur, fid, off, len;

    if (sc->wi_gone) {
        Debug(("wlandrv_PutPacket gone\r\n"));
        return (-1);
    }

    /*
     * Calculate the number of bytes to be send. Do not
     * send packets larger than 1518 bytes.
     */
    len = nb->nb_dl.sz + nb->nb_nw.sz + nb->nb_tp.sz + nb->nb_ap.sz;
    if (len > 1518) {
        return (-1);
    } else {
        WI_LOCK(sc);

        /*
         * This is a RFC894 Frame from Ethernut
         */
#if (WLAN_ENABLE_TX_FRAME_DUMP >= 1)
        Debug(("TxFrame: %d\n", len));
        DumpWlanData(nb->nb_dl.vp, len);
#endif

        /*
         * But we must send a RFC1024 Frame.
         * ETHERHDR is not part of an RFC1024 Frame
         */
        len = len - sizeof(ETHERHDR);

        /*
         * Add RFC1024 Header
         */
        len += sizeof(LLCS_SNAP_HEADER);

        /*
         * I do not know what I do here, therefore clear the header
         * and to hope for, the card will do the rest?
         */
        memset(&frmhdr, 0, sizeof(frmhdr));

        /*
         * Copy the Ethernut MAC 802.3 Dest/Source Address into the WLANHeader
         */
        pMAC8023Header = (ETHERHDR *) nb->nb_dl.vp;
        frmhdr.wi_dat_len = len;
        memcpy(frmhdr.wi_ehdr.ether_dhost, pMAC8023Header->ether_dhost, IEEE80211_ADDR_LEN);
        memcpy(frmhdr.wi_ehdr.ether_shost, pMAC8023Header->ether_shost, IEEE80211_ADDR_LEN);
        frmhdr.wi_ehdr.ether_type = SWAP(len);

        /*
         * Prepare the RFC1024 Header
         */
        LLCSSNAPHeader.DSAPSSAP = 0xAAAA;
        LLCSSNAPHeader.Control = 0x0003;
        LLCSSNAPHeader.MustZero = 0x0000;
        LLCSSNAPHeader.Type = pMAC8023Header->ether_type;

        /*
         * Get the TX-BUffer-ID
         */
        cur = sc->sc_txcur;
        fid = sc->sc_txd[cur].d_fid;

        /*
         * And Copy the WLANHeader
         */
        error = wi_write_bap(sc, fid, 0, &frmhdr, sizeof(frmhdr));
        if (error == 0) {
            off = sizeof(frmhdr);

            /*
             * Now copy the RFC1024 Header
             */
            error = wi_write_bap(sc, fid, off, &LLCSSNAPHeader, sizeof(LLCSSNAPHeader));
            if (error == 0) {
                off += sizeof(LLCSSNAPHeader);

                /*
                 * Copy the rest of the Ethernut Data
                 */
                if ((error == 0) && (nb->nb_nw.sz != 0)) {
                    error = wi_write_bap(sc, fid, off, nb->nb_nw.vp, nb->nb_nw.sz);
                    off += nb->nb_nw.sz;
                }
                if ((error == 0) && (nb->nb_tp.sz != 0)) {
                    error = wi_write_bap(sc, fid, off, nb->nb_tp.vp, nb->nb_tp.sz);
                    off += nb->nb_tp.sz;
                }
                if ((error == 0) && (nb->nb_ap.sz != 0)) {
                    error = wi_write_bap(sc, fid, off, nb->nb_ap.vp, nb->nb_ap.sz);
                }
                if (error == 0) {

                    //@@MF sc->sc_txd[cur].d_len = off;

                    error = wi_cmd(sc, WI_CMD_TX | WI_RECLAIM, fid, 0, 0);
                    if (error != 0) {
                        Debug(("xmit failed\n"));
                        sc->sc_txd[cur].d_len = 0;
                    }

                    sc->sc_txnext = cur = (cur + 1) % sc->sc_ntxbuf;
                }
            }
        }

        WI_UNLOCK(sc);
    }

    return (error);
}

/************************************************************/
/*  wlandrv_IOCTL                                           */
/************************************************************/
int wlandrv_IOCTL(NUTDEVICE * dev, int req, void *conf)
{
    int error = 0;
    int len;
    int i;
    u_int16_t DataBuffer[6];
    struct wi_softc *sc = (struct wi_softc *) dev->dev_dcb;
    struct ieee80211com *ic = &sc->sc_ic;
    WLAN_CONFIG *pWLANConfig;
    WLAN_STATUS *pWLANStatus;
    u_int8_t BSSIDAddress[6];

    if ((sc->wi_gone) || (conf == 0)) {
        error = -1;
    } else {

        switch (req) {
      /*****************************************************/
            /*             WLAN_IOCTL_GET_MAC_ADDRESS            */
      /*****************************************************/
        case WLAN_IOCTL_GET_MAC_ADDRESS:{
                memcpy(conf, ic->ic_myaddr, IEEE80211_ADDR_LEN);
                break;
            }                   /* WLAN_IOCTL_GET_MAC_ADDRESS */

      /*****************************************************/
            /*                WLAN_IOCTL_SET_CONFIG              */
      /*****************************************************/
        case WLAN_IOCTL_SET_CONFIG:{
                pWLANConfig = (WLAN_CONFIG *) conf;

                if ((pWLANConfig != NULL) && (pWLANConfig->Size == sizeof(WLAN_CONFIG))) {
                    WI_LOCK(sc);

                    /* Stop WLAN controller */
                    wi_stop(sc, 1);

                    /* Set mode */
                    switch (pWLANConfig->Mode) {
                    case WLAN_MODE_STOP:{
                            WI_UNLOCK(sc);
                            return (0);
                            break;
                        }
                    case WLAN_MODE_STATION:{
                            ic->ic_opmode = IEEE80211_M_STA;
                            break;
                        }
                    case WLAN_MODE_ADHOC:{
                            ic->ic_opmode = IEEE80211_M_IBSS;
                            break;
                        }
                    default:{
                            error = -1;
                            break;
                        }
                    }

                    if (error == 0) {
                        /* Set SSID */
                        len = strlen(pWLANConfig->Networkname);
                        memcpy(ic->ic_des_essid, pWLANConfig->Networkname, len);
                        ic->ic_des_esslen = len;

                        /* Enable WEP */
                        switch (pWLANConfig->UseWEP) {
                        case WLAN_USE_NO_WEP:{
                                ic->ic_flags &= ~IEEE80211_F_WEPON;
                                break;
                            }
                        case WLAN_USE_64BIT_WEP:
                        case WLAN_USE_128BIT_WEP:{
                                ic->ic_flags |= IEEE80211_F_WEPON;
                                break;
                            }
                        default:{
                                error = -1;
                                break;
                            }
                        }

                        /* Set WEP tx key */
                        ic->ic_wep_txkey = pWLANConfig->UseWEPTxKey;

                        /* Set WEP keys */
                        for (i = 0; i < WLAN_WEP_MAX_KEY_COUNT; i++) {
                            ic->ic_nw_keys[i].wk_len = pWLANConfig->WEPKey[i].KeyLen;
                            memcpy(ic->ic_nw_keys[i].wk_key, pWLANConfig->WEPKey[i].Key, WLAN_WEP_MAX_KEY_SIZE);
                        }
                    }

                    WI_UNLOCK(sc);

                    if (error == 0) {
                        wlandrv_Init(dev);
                    }
                } else {
                    error = -1;
                }
                break;
            }                   /* WLAN_IOCTL_SET_CONFIG */

      /*****************************************************/
            /*                WLAN_IOCTL_GET_STATUS              */
      /*****************************************************/
        case WLAN_IOCTL_GET_STATUS:{
                pWLANStatus = (WLAN_STATUS *) conf;

                if ((pWLANStatus != NULL) && (pWLANStatus->Size == sizeof(WLAN_STATUS))) {
                    WI_LOCK(sc);

                    memset(pWLANStatus, 0x00, sizeof(WLAN_STATUS));
                    pWLANStatus->Size = sizeof(WLAN_STATUS);

                    /*
                     * PortStatus
                     */
                    memset(DataBuffer, 0, sizeof(DataBuffer));
                    len = sizeof(DataBuffer);
                    error = wi_read_rid(sc, WI_RID_PORT_STAT, DataBuffer, &len);
                    if (error == 0) {
                        pWLANStatus->PortStatus = (u_int8_t) DataBuffer[0];
                    }

                    /*
                     * Current BSSID
                     */
                    memset(BSSIDAddress, 0, sizeof(BSSIDAddress));
                    len = sizeof(BSSIDAddress);
                    error = wi_read_rid(sc, WI_RID_CURRENT_BSSID, BSSIDAddress, &len);
                    if (error == 0) {
                        memcpy(pWLANStatus->BSSIDAddress, BSSIDAddress, IEEE80211_ADDR_LEN);
                    }

                    /*
                     * Current Channel
                     */
                    memset(DataBuffer, 0, sizeof(DataBuffer));
                    len = sizeof(DataBuffer);
                    error = wi_read_rid(sc, WI_RID_CURRENT_CHAN, DataBuffer, &len);
                    if (error == 0) {
                        pWLANStatus->Channel = (u_int8_t) DataBuffer[0];
                    }

                    /*
                     * Current TxRate
                     */
                    memset(DataBuffer, 0, sizeof(DataBuffer));
                    len = sizeof(DataBuffer);
                    error = wi_read_rid(sc, WI_RID_CUR_TX_RATE, DataBuffer, &len);
                    if (error == 0) {
                        pWLANStatus->TxRate = (u_int8_t) DataBuffer[0];
                    }

                    /*
                     * Getting Quality
                     */
                    memset(DataBuffer, 0, sizeof(DataBuffer));
                    len = sizeof(DataBuffer);
                    error = wi_read_rid(sc, WI_RID_DBM_COMMS_QUAL, DataBuffer, &len);
                    if (error == 0) {
                        pWLANStatus->Quality = DataBuffer[0];
                        pWLANStatus->Signal = DataBuffer[1];
                        pWLANStatus->Noise = DataBuffer[2];
                    }

                    WI_UNLOCK(sc);
                } else {
                    error = -1;
                }

                break;
            }                   /* WLAN_IOCTL_GET_STATUS */


        default:{
                error = -1;
                break;
            }
        }                       /* switch */
    }                           /* sc->wi_gone */


    return (error);
}
