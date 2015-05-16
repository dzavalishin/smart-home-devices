/*
 * Copyright (C) 2003-2006 by egnite Software GmbH
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

/*!
 * $Id: config.c 2955 2010-04-03 13:54:45Z haraldkipp $
 */


#include <sys/types.h>

#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <net/if_var.h>

#include "config.h"

#include <stdio.h>

/*!
 * \brief List of radio stations.
 */
RADIOSTATION *station;

/*!
 * \brief Radio status and control.
 */
RADIOCONTROL radio;

/*
 * Save a binary into the EEPROM.
 */
static int ConfigSaveBinary(int addr, void *val, size_t len)
{
#if defined(__AVR__)
    size_t i;
    uint8_t *cp = val;

    for (i = 0; i < len; cp++, i++)
        if (eeprom_read_byte((void *) (addr + i)) != *cp)
            eeprom_write_byte((void *) (addr + i), *cp);
#endif /* __AVR__ */

    return len;
}

/*
 * Save a string into the EEPROM.
 */
static int ConfigSaveString(int addr, char * str)
{
    int rc = 0;

#if defined(__AVR__)
    do {
        if (eeprom_read_byte((void *) (addr + rc)) != *str)
            eeprom_write_byte((void *) (addr + rc), *str);
        rc++;
    } while (*str++);
#endif /* __AVR__ */

    return rc;
}

/*
 * Read a string from EEPROM.
 */
static size_t ConfigLoadString(int addr, char * str, size_t size)
{
    size_t rc = 0;

#if defined(__AVR__)
    while (rc < size) {
        *str = eeprom_read_byte((void *) (addr + rc));
        rc++;
        if (*str++ == 0)
            break;
    }
#endif /* __AVR__ */

    return rc;
}

/*
 * Read a binary value from EEPROM.
 */
static int ConfigLoadBinary(int addr, void *val, size_t len)
{
#if defined(__AVR__)
    size_t i;
    uint8_t *cp = val;

    for (i = 0; i < len; cp++, i++)
        *cp = eeprom_read_byte((void *) (addr + i));
#endif /* __AVR__ */

    return len;
}

/*!
 * \brief Calculate total size of configuration data.
 *
 * \return Number of bytes used by configuration data.
 */
size_t ConfigSize(void)
{
    size_t rc = 0;
    uint8_t idx;
    RADIOSTATION *rsp;

    for (idx = 0; idx < MAXNUM_STATIONS; idx++) {
        rsp = &station[idx];
        if (station[idx].rs_port == 0)
            break;
        rc += sizeof(rsp->rs_port);
        rc += sizeof(rsp->rs_ip);
        if (rsp->rs_url)
            rc += strlen(rsp->rs_url);
        rc++;
    }
    rc += sizeof(station[0].rs_port);

    return rc;
}

/*!
 * \brief Configure a station list entry.
 *
 * \param idx Index of the entry.
 * \param url URL of the station.
 *
 * \return 0 on success, -1 otherwise.
 */
int ConfigStation(uint8_t idx, CONST char * url)
{
    uint32_t ip;
    uint16_t port = 80;
    char *buf;
    char *cp;

    if (idx >= MAXNUM_STATIONS) {
        idx = 0;
        while (idx < MAXNUM_STATIONS && station[idx].rs_port)
            idx++;
    }
    if (idx >= MAXNUM_STATIONS)
        return -1;
    else {
        buf = malloc(strlen(url) + 1);

        /* Extract IP address. */
        cp = buf;
        while (*url && *url != '/' && *url != ':')
            *cp++ = *url++;
        *cp = 0;
        if ((int) (ip = inet_addr(buf)) == -1)
            idx = -1;
        else {
            /* Extract URL path. */
            cp = buf;
            if (*url == '/') {
                url++;
                while (*url && *url != ':')
                    *cp++ = *url++;
            }
            *cp = 0;

            /* Extract port. */
            if (*url == ':')
                port = atoi(url + 1);

            if (port) {
                station[idx].rs_ip = ip;
                station[idx].rs_port = port;
                if (*buf) {
                    station[idx].rs_url = malloc(strlen(buf) + 1);
                    strcpy(station[idx].rs_url, buf);
                }
            }
        }

        free(buf);
    }
    return idx;
}

/*
 * Allocate heap memory for configuration structure.
 */
static void ConfigCreate(void)
{
    uint8_t idx;

    if (station == 0)
        station = malloc(MAXNUM_STATIONS * sizeof(RADIOSTATION));
    else {
        /* Release all memory occupied by the current configuration. */
        for (idx = 0; idx < MAXNUM_STATIONS; idx++) {
            if (station[idx].rs_port == 0)
                break;
            if (station[idx].rs_url)
                free(station[idx].rs_url);
        }
    }
    memset(station, 0, MAXNUM_STATIONS * sizeof(RADIOSTATION));
}

/*!
 * \brief Reset configuration.
 */
void ConfigResetFactory(void)
{
    ConfigCreate();

    /* Initial radio control settings. */
    radio.rc_rstation = 2;
    radio.rc_rvolume = 223;

    /* 
     * Add pre-configured radio stations. 
     */

    /* Local server. */
    ConfigStation(MAXNUM_STATIONS, "192.168.192.11:8000");
    /* elDOradio 56 kbit */
    ConfigStation(MAXNUM_STATIONS, "129.217.234.42/128:8000");
    /* Virgin 32 kbit. */
    ConfigStation(MAXNUM_STATIONS, "212.187.204.62:80");
    /* qpop.nl 48 kbit */
    ConfigStation(MAXNUM_STATIONS, "194.109.192.226:8010");
    /* idobi 24 kbit */
    ConfigStation(MAXNUM_STATIONS, "66.28.100.131:8004");
    /* Radiosound-7-24 24 kbit. */
    ConfigStation(MAXNUM_STATIONS, "64.202.98.51:7650");
    /* DH Netradio 56 kbit */
    ConfigStation(MAXNUM_STATIONS, "205.188.234.38:8030");
    /* Cable radio 56 kbit. */
    ConfigStation(MAXNUM_STATIONS, "62.25.96.7:8080");
    /* MIBN1 40 kbit. */
    ConfigStation(MAXNUM_STATIONS, "216.234.109.21:8000");
    /* RFK 56 kbit */
    ConfigStation(MAXNUM_STATIONS, "64.202.98.33:2530");
    /* Braingell 24 kbit. */
    ConfigStation(MAXNUM_STATIONS, "216.237.145.20:8000");
    /* HipHop 48 kbit. */
    ConfigStation(MAXNUM_STATIONS, "64.202.98.33:6150");
    /* Flensburg 56 kbit */
    ConfigStation(MAXNUM_STATIONS, "217.160.210.37:8000");
    /* Boombastic 24 kbit. */
    ConfigStation(MAXNUM_STATIONS, "212.43.230.20:8000");
    /* Russia 24 kbit */
    ConfigStation(MAXNUM_STATIONS, "62.118.255.5:9000");
    /* Frequence3  16 kbit. */
    ConfigStation(MAXNUM_STATIONS, "212.180.2.19:8010");
    /* KCTI Country 16 kbit. */
    ConfigStation(MAXNUM_STATIONS, "63.125.62.117:8000");
    /* Klassik 40 kbit. */
    ConfigStation(MAXNUM_STATIONS, "62.157.113.86:8000");
    /* m2ktalk 8kbit */
    ConfigStation(MAXNUM_STATIONS, "209.17.76.226:8010");
    /* Christian Media 16 kbit. */
    ConfigStation(MAXNUM_STATIONS, "64.202.98.32:6610");
    /* Newsradio 24 kbit. */
    ConfigStation(MAXNUM_STATIONS, "65.172.162.93:9191");

#ifdef ETHERNUT2
    /*
     * These stations require Ethernut 2.
     */
    /* Grapeshot 128 kbit. */
    ConfigStation(MAXNUM_STATIONS, "66.28.45.159:8075");
    /* Radiostorm 128 kbit. */
    ConfigStation(MAXNUM_STATIONS, "64.236.34.141/stream/1014:80");
    /* Digitally imported 128 kbit. */
    ConfigStation(MAXNUM_STATIONS, "205.188.209.193/stream/1003:80");
    /* SmoothJazz 128 kbit */
    ConfigStation(MAXNUM_STATIONS, "64.236.34.141/stream/1005:80");
    /* Tosh Jamaica 128 kbit. */
    ConfigStation(MAXNUM_STATIONS, "38.144.33.148:8022");
    /* weird 160 kbit */
    ConfigStation(MAXNUM_STATIONS, "209.98.88.40:8007");
    /* Stratovarius 192 kbit */
    ConfigStation(MAXNUM_STATIONS, "210.120.247.22:1290");
    /* Virgin 128 kbit. */
    ConfigStation(MAXNUM_STATIONS, "64.236.34.72/stream/1031:80");
    /* Virgin 128 kbit. */
    ConfigStation(MAXNUM_STATIONS, "64.236.34.141/stream/1031:80");
#endif
}

/*!
 * \brief Load configuration from EEPROM.
 *
 * If no configuration is available, some preconfigured stations
 * are loaded.
 *
 * \return 0 on success, -1 if no configuration data had been found.
 */
int ConfigLoad(void)
{
    int rc = -1;
    int addr = CONFAPP_EE_OFFSET;
    char *buf;
    uint8_t idx;
    RADIOSTATION *rsp;

    buf = malloc(MAXLEN_URL + 1);
    addr += ConfigLoadString(addr, buf, sizeof(CONFAPP_EE_NAME));
    if (strcmp(buf, CONFAPP_EE_NAME) == 0) {

        ConfigCreate();
        rc = 0;

        /* 
         * Read radio settings from EEPROM.
         */
        addr += ConfigLoadBinary(addr, &radio.rc_rstation, sizeof(radio.rc_rstation));
        addr += ConfigLoadBinary(addr, &radio.rc_rvolume, sizeof(radio.rc_rvolume));

        /* 
         * Read station configuration from EEPROM.
         */
        for (idx = 0; idx < MAXNUM_STATIONS; idx++) {
            rsp = &station[idx];
            addr += ConfigLoadBinary(addr, &rsp->rs_port, sizeof(rsp->rs_port));
            addr += ConfigLoadBinary(addr, &rsp->rs_ip, sizeof(rsp->rs_ip));
            addr += ConfigLoadString(addr, buf, MAXLEN_URL);
            if (*buf) {
                rsp->rs_url = malloc(strlen(buf) + 1);
                strcpy(rsp->rs_url, buf);
            }
        }
    }
    free(buf);

    return rc;
}

/*!
 * \brief Save radio control settings.
 *
 * Saves currently selected station and volume.
 */
void ConfigSaveControl(void)
{
    int addr = CONFAPP_EE_OFFSET + sizeof(CONFAPP_EE_NAME);

    /* Save radio control. */
    addr += ConfigSaveBinary(addr, &radio.rc_cstation, sizeof(radio.rc_cstation));
    addr += ConfigSaveBinary(addr, &radio.rc_cvolume, sizeof(radio.rc_cvolume));
}

/*!
 * \brief Save configuration in EEPROM.
 */
void ConfigSave(void)
{
    uint8_t idx;
    RADIOSTATION *rsp;
    int addr = CONFAPP_EE_OFFSET;

    NutNetSaveConfig();

    /* Save our name. */
    addr += ConfigSaveString(addr, CONFAPP_EE_NAME);

    /* Save radio control. */
    addr += ConfigSaveBinary(addr, &radio.rc_cstation, sizeof(radio.rc_cstation));
    addr += ConfigSaveBinary(addr, &radio.rc_cvolume, sizeof(radio.rc_cvolume));

    /* Save stations. */
    for (idx = 0; idx < MAXNUM_STATIONS; idx++) {
        rsp = &station[idx];
        addr += ConfigSaveBinary(addr, &rsp->rs_port, sizeof(rsp->rs_port));
        addr += ConfigSaveBinary(addr, &rsp->rs_ip, sizeof(rsp->rs_ip));
        if (rsp->rs_url)
            addr += ConfigSaveString(addr, rsp->rs_url);
        else
            addr += ConfigSaveString(addr, "");
    }
}
