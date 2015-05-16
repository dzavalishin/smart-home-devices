/*
 * Copyright (C) 2006 by egnite Software GmbH. All rights reserved.
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

/*!
 * \file dev/pcf8563.c
 * \brief RTC for Philips PCF8563 clock chip.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.4  2008/08/11 06:59:42  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.3  2006/10/05 17:18:49  haraldkipp
 * Hardware independant RTC layer added.
 *
 * Revision 1.2  2006/06/28 17:23:47  haraldkipp
 * Bugfix. PcfRtcGetClock() returned wrong century.
 *
 * Revision 1.1  2006/04/07 13:54:17  haraldkipp
 * PCF8563 RTC driver added.
 *
 *
 * \endverbatim
 */

#include <cfg/os.h>
#include <dev/twif.h>
#include <sys/event.h>
#include <sys/timer.h>

#include <time.h>
#include <stdlib.h>
#include <string.h>

#include <dev/pcf8563.h>

#ifndef I2C_SLA_RTC
#define I2C_SLA_RTC     0x51
#endif

static uint32_t rtc_status;

/*!
 * \brief Read RTC registers.
 *
 * \param reg  The first register to read.
 * \param buff Pointer to a buffer that receives the register contents.
 * \param cnt  The number of registers to read.
 *
 * \return 0 on success or -1 in case of an error.
 */
int PcfRtcReadRegs(uint8_t reg, uint8_t *buff, size_t cnt)
{
    int rc = -1;

    if (TwMasterTransact(I2C_SLA_RTC, &reg, 1, buff, cnt, NUT_WAIT_INFINITE) == cnt) {
        rc = 0;
    }
    return rc;
}

/*!
 * \brief Write to RTC registers.
 *
 * \param nv   Must be set to 1 when writing to non-volatile registers.
 *             In this case the routine will poll for write cycle
 *             completion before returning to the caller. Set to zero
 *             if writing to volatile registers.
 * \param buff This buffer must contain all bytes to be transfered to
 *             the RTC chip, including the register address.
 * \param cnt  Number of valid bytes in the buffer.
 *
 * \return 0 on success or -1 in case of an error.
 */
int PcfRtcWrite(int nv, CONST uint8_t *buff, size_t cnt)
{
    return TwMasterTransact(I2C_SLA_RTC, buff, cnt, 0, 0, NUT_WAIT_INFINITE);
}

/*!
 * \brief Get date and time from an PCF8563 hardware clock.
 *
 * \param tm Points to a structure that receives the date and time 
 *           information.
 *
 * \return 0 on success or -1 in case of an error.
 */
int PcfRtcGetClock(struct _tm *tm)
{
    int rc;
    uint8_t data[7];

    if ((rc = PcfRtcReadRegs(0x02, data, 7)) == 0) {
        tm->tm_sec = BCD2BIN(data[0] & 0x7F);
        tm->tm_min = BCD2BIN(data[1] & 0x7F);
        tm->tm_hour = BCD2BIN(data[2] & 0x3F);
        tm->tm_mday = BCD2BIN(data[3] & 0x3F);
        tm->tm_mon = BCD2BIN(data[5] & 0x1F) - 1;
        tm->tm_year = BCD2BIN(data[6]);
        if (data[5] & 0x80) {
            tm->tm_year += 100;
        }
        tm->tm_wday = data[4] & 0x07;
    }
    return rc;
}

/*!
 * \brief Set an PCF8563 hardware clock.
 *
 * New time will be taken over at the beginning of the next second.
 *
 * \param tm Points to a structure which contains the date and time
 *           information.
 *
 * \return 0 on success or -1 in case of an error.
 */
int PcfRtcSetClock(CONST struct _tm *tm)
{
    uint8_t data[8];

    memset(data, 0, sizeof(data));
    if (tm) {
        data[0] = 0x02;
        data[1] = BIN2BCD(tm->tm_sec);
        data[2] = BIN2BCD(tm->tm_min);
        data[3] = BIN2BCD(tm->tm_hour);
        data[4] = BIN2BCD(tm->tm_mday);
        data[5] = tm->tm_wday;
        data[6] = BIN2BCD(tm->tm_mon + 1);
        if (tm->tm_year > 99) {
            data[7] = BIN2BCD(tm->tm_year - 100);
            data[6] |= 0x80;
        }
        else {
            data[7] = BIN2BCD(tm->tm_year);
        }
    }
    return PcfRtcWrite(0, data, 8);
}

/*!
 * \brief Get alarm date and time of an PCF8563 hardware clock.
 *
 * Not implemented.
 *
 * \param idx   Zero based index. Two alarms are supported.
 * \param tm    Points to a structure that receives the date and time 
 *              information.
 * \param aflgs Points to an unsigned long that receives the enable flags.
 *
 * \return 0 on success or -1 in case of an error.
 *
 */
int PcfRtcGetAlarm(int idx, struct _tm *tm, int *aflgs)
{
    return -1;
}

/*!
 * \brief Set alarm of an PCF8563 hardware clock.
 *
 * Not implemented.
 *
 * \param idx   Zero based index. Two alarms are supported.
 * \param tm    Points to a structure which contains the date and time
 *              information. May be NULL to clear the alarm.
 * \param aflgs Each bit enables a specific comparision.
 *              - Bit 0: Seconds
 *              - Bit 1: Minutes
 *              - Bit 2: Hours
 *              - Bit 3: Day of month
 *              - Bit 4: Month
 *              - Bit 7: Day of week (Sunday is zero)
 *
 * \return 0 on success or -1 in case of an error.
 */
int PcfRtcSetAlarm(int idx, CONST struct _tm *tm, int aflgs)
{
    return -1;
}

/*!
 * \brief Query RTC status flags.
 *
 * \param sflgs Points to an unsigned long that receives the status flags.
 *              - Bit 0: Power fail.
 *              - Bit 5: Alarm 0 occured (not implemented).
 *              - Bit 6: Alarm 1 occured (not implemented).
 *
 * \return 0 on success or -1 in case of an error.
 */
int PcfRtcGetStatus(uint32_t *sflgs)
{
    int rc;
    uint8_t data;

    if ((rc = PcfRtcReadRegs(0x02, &data, 1)) == 0) {
        if (data & 0x80) {
            rtc_status = RTC_STATUS_PF;
        }
        *sflgs = rtc_status;
    }
    return rc;
}

/*!
 * \brief Clear RTC status flags.
 *
 * \param sflgs Status flags to clear.
 *
 * \return Always 0.
 */
int PcfRtcClearStatus(uint32_t sflgs)
{
    rtc_status &= ~sflgs;

    return 0;
}

/*!
 * \brief Initialize the interface to an Intersil PCF8563 hardware clock.
 *
 * \return 0 on success or -1 in case of an error.
 *
 */
int PcfRtcInit(void)
{
    int rc;
    uint32_t tmp;

    if ((rc = TwInit(0)) == 0) {
        rc = PcfRtcGetStatus(&tmp);
    }
    return rc;
}

NUTRTC rtcPcf8563 = {
    PcfRtcInit,         /*!< Hardware initializatiuon, rtc_init */
    PcfRtcGetClock,     /*!< Read date and time, rtc_gettime */
    PcfRtcSetClock,     /*!< Set date and time, rtc_settime */
    PcfRtcGetAlarm,     /*!< Read alarm date and time, rtc_getalarm */
    PcfRtcSetAlarm,     /*!< Set alarm date and time, rtc_setalarm */
    PcfRtcGetStatus,    /*!< Read status flags, rtc_getstatus */
    PcfRtcClearStatus   /*!< Clear status flags, rtc_clrstatus */
};
