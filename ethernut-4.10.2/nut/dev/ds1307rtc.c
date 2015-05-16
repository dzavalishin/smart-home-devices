/*
 * Copyright (C) 2006 by egnite Software GmbH and Christian Welzel.
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

/*!
 * \file dev/ds1307rtc.c
 * \brief RTC and RAM routines for the Dallas DS1307 clock chip.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.6  2008/08/11 06:59:42  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.5  2007/09/11 17:32:30  haraldkipp
 * Last minute fix (bug #1786271)
 *
 * Revision 1.4  2006/10/08 16:39:33  haraldkipp
 * Corrected range check in DS1307RamRead(). Thanks to Helmut Holbach.
 *
 * Revision 1.3  2006/10/05 17:18:49  haraldkipp
 * Hardware independant RTC layer added.
 *
 * Revision 1.2  2006/08/25 13:42:55  olereinhardt
 * avr-gcc 3.4 does not understand binary representation of numbers,
 * changed to hex
 *
 * Revision 1.1  2006/06/30 22:05:51  christianwelzel
 * Initial check in.
 *
 *
 * \endverbatim
 */

#include <cfg/os.h>
#include <dev/twif.h>
#include <sys/event.h>
#include <sys/timer.h>

#include <stdlib.h>
#include <string.h>

#include <dev/ds1307rtc.h>

#ifndef I2C_SLA_RTC
#define I2C_SLA_RTC     0x68    // 7 bit address!
#endif


/*!
 * \brief Read RTC registers.
 *
 * \param reg  The first register to read.
 * \param buff Pointer to a buffer that receives the register contents.
 * \param cnt  The number of registers to read.
 *
 * \return 0 on success or -1 in case of an error.
 */
int DS1307RtcReadRegs(uint8_t reg, uint8_t *buff, size_t cnt)
{
    int rc = -1;
    uint8_t wbuf[1];

    wbuf[0] = reg;
    if (TwMasterTransact(I2C_SLA_RTC, wbuf, 1, buff, cnt, NUT_WAIT_INFINITE) == cnt) {
        rc = 0;
    }
    return rc;
}

/*!
 * \brief Write to RTC registers.
 *
 * \param buff This buffer must contain all bytes to be transfered to
 *             the RTC chip, including the register address.
 * \param cnt  Number of valid bytes in the buffer.
 *
 * \return 0 on success or -1 in case of an error.
 */
int DS1307RtcWrite(CONST uint8_t *buff, size_t cnt)
{
    int rc;

    rc = TwMasterTransact(I2C_SLA_RTC, buff, cnt, 0, 0, NUT_WAIT_INFINITE);
    return rc;
}

/*!
 * \brief Get date and time from an DS1307 hardware clock.
 *
 * \param tm Points to a structure that receives the date and time
 *           information.
 *
 * \return 0 on success or -1 in case of an error.
 */
int DS1307RtcGetClock(struct _tm *tm)
{
    int rc;
    uint8_t data[7];

    if ((rc = DS1307RtcReadRegs(0x00, data, 7)) == 0) {
        tm->tm_sec  = BCD2BIN(data[0]);
        tm->tm_min  = BCD2BIN(data[1]);
        tm->tm_hour = BCD2BIN(data[2] & 0x3F);
        tm->tm_mday = BCD2BIN(data[4]);
        tm->tm_mon  = BCD2BIN(data[5]) - 1;
        tm->tm_year = BCD2BIN(data[6]) + 100;
        tm->tm_wday = data[3] - 1;
    }
    return rc;
}

/*!
 * \brief Set the DS1307 hardware clock.
 *
 * \param tm Points to a structure which contains the date and time
 *           information.
 *
 * \return 0 on success or -1 in case of an error.
 */
int DS1307RtcSetClock(CONST struct _tm *tm)
{
    uint8_t data[8];

    memset(data, 0, sizeof(data));
    if (tm) {
        data[0] = 0x00;     // adress 0
        data[1] = BIN2BCD(tm->tm_sec);
        data[2] = BIN2BCD(tm->tm_min);
        data[3] = BIN2BCD(tm->tm_hour) & 0x3f; // 24 hour mode
        data[5] = BIN2BCD(tm->tm_mday);
        data[6] = BIN2BCD(tm->tm_mon + 1);
        data[7] = BIN2BCD(tm->tm_year - 100);
        data[4] = tm->tm_wday + 1;
    }
    return DS1307RtcWrite(data, 8);
}

/*!
 * \brief Read contents from DS1307 RAM.
 *
 * \param addr  Start location.
 * \param buff  Points to a buffer that receives the contents.
 * \param cnt   Number of bytes to read.
 *
 * \return 0 on success or -1 in case of an error.
 */
int DS1307RamRead(uint8_t addr, uint8_t *buff, size_t cnt)
{
    int rc = -1;

    // Range check
    if ((addr>=0x08) && (addr<=0x3F)) {
        rc = DS1307RtcReadRegs(addr, buff, cnt);
    }

    return rc;
}

/*!
 * \brief Store buffer contents in DS1307 RAM.
 *
 * \param addr  Storage start location.
 * \param buff  Points to a buffer that contains the bytes to store.
 * \param len   Number of valid bytes in the buffer.
 *
 * \return 0 on success or -1 in case of an error.
 */
int DS1307RamWrite(uint8_t addr, CONST void *buff, size_t len)
{
    int rc = 0;
    uint8_t *wbuf;
    CONST uint8_t *wp = buff;

    /* Allocate and set a TWI write buffer. */
    if ((wbuf = malloc(len + 1)) == 0) {
        rc = -1;
    } else {
        wbuf[0] = addr;
        memcpy(wbuf + 1, wp, len);

        /* Send the write buffer. */
        rc = TwMasterTransact(I2C_SLA_RTC, wbuf, len + 1, 0, 0, NUT_WAIT_INFINITE);

        /* Release the buffer. */
        free(wbuf);
    }

    return rc;
}

/*!
 * \brief Initialize the interface to an Dallas DS1307 hardware clock.
 *
 * \return 0 on success or -1 in case of an error.
 *
 */
int DS1307Init(void)
{
    int rc;
    uint8_t data;
    uint8_t buff[2];

    if ((rc = TwInit(0)) == 0 ) {
        // Enable Oszillator
        if ((rc = DS1307RtcReadRegs(0x00, &data, 1)) == 0) {
            buff[0] = 0x00;
            buff[1] = data & 0x7F; //0b01111111;
            rc = DS1307RtcWrite(buff, 2);
        }

        // Enable 24h Mode
        if ((rc = DS1307RtcReadRegs(0x02, &data, 1)) == 0) {
            buff[0] = 0x02;
            buff[1] = data & 0xBF; //0b10111111;
            rc = DS1307RtcWrite(buff, 2);
        }
    }
    return rc;
}

NUTRTC rtcDs1307 = {
    DS1307Init,         /*!< Hardware initializatiuon, rtc_init */
    DS1307RtcGetClock,  /*!< Read date and time, rtc_gettime */
    DS1307RtcSetClock,  /*!< Set date and time, rtc_settime */
    NULL,               /*!< Read alarm date and time, rtc_getalarm */
    NULL,               /*!< Set alarm date and time, rtc_setalarm */
    NULL,               /*!< Read status flags, rtc_getstatus */
    NULL                /*!< Clear status flags, rtc_clrstatus */
};
