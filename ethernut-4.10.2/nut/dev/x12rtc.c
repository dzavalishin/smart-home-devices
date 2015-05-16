/*
 * Copyright (C) 2005-2007 by egnite Software GmbH. All rights reserved.
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
 * \file dev/x12rtc.c
 * \brief RTC and EEPROM routines for the Intersil X12xx clock chips.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.10  2009/02/13 14:52:05  haraldkipp
 * Include memdebug.h for heap management debugging support.
 *
 * Revision 1.9  2009/01/17 11:26:46  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.8  2008/08/11 06:59:42  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.7  2008/07/09 14:25:06  haraldkipp
 * Made EEPROM_PAGE_SIZE configurable. Does it really make sense?
 *
 * Revision 1.6  2007/06/03 08:50:38  haraldkipp
 * Automatic detection of X1226 or X1286.
 * Fixed wrong century determination for X1226.
 * Debugging output added.
 *
 * Revision 1.5  2007/05/02 11:29:25  haraldkipp
 * Failed to store more than one EEPROM page. Removing NutSleep() from
 * X12WaitReady() fixes this. Why?
 *
 * Revision 1.4  2006/10/05 17:21:53  haraldkipp
 * Hardware specific functions marked deprecated.
 * Hardcoded register addresses and values replaced by macros.
 *
 * Revision 1.3  2006/03/02 19:57:34  haraldkipp
 * ICCARM insists on a (void *) typecast for the second parameter of memcpy().
 *
 * Revision 1.2  2006/01/19 18:41:34  haraldkipp
 * Year translation was completely broken. Fixed.
 *
 * Revision 1.1  2005/10/24 10:21:57  haraldkipp
 * Initial check in.
 *
 *
 * \endverbatim
 */

#define NUT_DEPRECATED

#include <cfg/os.h>
#include <cfg/eeprom.h>

#include <dev/twif.h>
#include <sys/event.h>
#include <sys/timer.h>

#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <memdebug.h>

#include <dev/x12rtc.h>

#if 0
/* Use for local debugging. */
#define X12DEBUG
#endif

#ifdef X12DEBUG
#define NUTDEBUG
#include <stdio.h>
#endif

#ifndef I2C_SLA_RTC
#define I2C_SLA_RTC     0x6F
#endif

#ifndef I2C_SLA_EEPROM
#define I2C_SLA_EEPROM  0x57
#endif

#ifndef EEPROM_PAGE_SIZE
#define EEPROM_PAGE_SIZE    64
#endif

static uint8_t rtc_chip;
static uint32_t rtc_status;

/*!
 * \brief Enable or disable write access.
 *
 * \param on Write access is disabled if this parameter is 0, or
 *           enabled otherwise.
 *
 * \return 0 on success or -1 in case of an error.
 */
static int X12WriteEnable(int on)
{
    int rc;
    uint8_t buf[3];

    buf[0] = 0;
    buf[1] = 0x3F;
    if (on) {
        buf[2] = 0x02;
        if ((rc = TwMasterTransact(I2C_SLA_RTC, buf, 3, 0, 0, NUT_WAIT_INFINITE)) == 0) {
            buf[2] = 0x06;
            rc = TwMasterTransact(I2C_SLA_RTC, buf, 3, 0, 0, NUT_WAIT_INFINITE);
        }
    } else {
        buf[2] = 0x00;
        rc = TwMasterTransact(I2C_SLA_RTC, buf, 3, 0, 0, NUT_WAIT_INFINITE);
    }
    return rc;
}

/*!
 * \brief Wait until non-volatile write cycle finished.
 *
 * \return 0 on success or -1 in case of an error.
 */
static int X12WaitReady(void)
{
    uint8_t poll;
    int cnt = 200; /* Ethernut 3 needs about 50 loops, so this is quite save. */

    /*
     * Poll for write cycle finished. We can't use a sleep here, because our
     * X12xx routines are not re-entrant.
     */
    while (--cnt && TwMasterTransact(I2C_SLA_EEPROM, 0, 0, &poll, 1, NUT_WAIT_INFINITE) == -1);
#ifdef X12DEBUG
    printf("[RtcRdy:%d]", 200 - cnt);
#endif

    return cnt ? 0 : -1;
}

/*!
 * \brief Read RTC registers.
 *
 * \param reg  The first register to read.
 * \param buff Pointer to a buffer that receives the register contents.
 * \param cnt  The number of registers to read.
 *
 * \return 0 on success or -1 in case of an error.
 */
int X12RtcReadRegs(uint8_t reg, uint8_t *buff, size_t cnt)
{
    int rc = -1;
    uint8_t wbuf[2];

    wbuf[0] = 0;
    wbuf[1] = reg;
    if (TwMasterTransact(I2C_SLA_RTC, wbuf, 2, buff, cnt, NUT_WAIT_INFINITE) == cnt) {
#ifdef X12DEBUG
        printf("[Rtc$%02x>", reg);
        while(cnt--) {
            printf(" %02x", *buff++);
        }
        putchar(']');
#endif
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
int X12RtcWrite(int nv, CONST uint8_t *buff, size_t cnt)
{
    int rc;

    if ((rc = X12WriteEnable(1)) == 0) {
        rc = TwMasterTransact(I2C_SLA_RTC, buff, cnt, 0, 0, NUT_WAIT_INFINITE);
        if (rc == 0 && nv) {
            rc = X12WaitReady();
        }
        X12WriteEnable(0);
#ifdef X12DEBUG
        printf("[Rtc$%02X<", *++buff);
        cnt -= 2;
        while(cnt--) {
            printf(" %02x", *++buff);
        }
        putchar(']');
#endif
    }
    return rc;
}

/*!
 * \brief Get date and time from an X12xx hardware clock.
 *
 * \deprecated New applications must use NutRtcGetTime().
 *
 * \param tm Points to a structure that receives the date and time
 *           information.
 *
 * \return 0 on success or -1 in case of an error.
 */
int X12RtcGetClock(struct _tm *tm)
{
    int rc;
    uint8_t data[8];

    if ((rc = X12RtcReadRegs(X12RTC_SC, data, 8)) == 0) {
        tm->tm_sec = BCD2BIN(data[0]);
        tm->tm_min = BCD2BIN(data[1]);
        tm->tm_hour = BCD2BIN(data[2] & 0x3F);
        tm->tm_mday = BCD2BIN(data[3]);
        tm->tm_mon = BCD2BIN(data[4]) - 1;
        if (rtc_chip) {
            tm->tm_year = BCD2BIN(data[5]) + 100;
        }
        else {
            tm->tm_year = BCD2BIN(data[5]);
            if (data[7] == 0x20) {
                tm->tm_year += 100;
            }
        }
        tm->tm_wday = data[6];
    }
    return rc;
}

/*!
 * \brief Set an X12xx hardware clock.
 *
 * \deprecated New applications must use NutRtcSetTime().
 *
 * New time will be taken over at the beginning of the next second.
 *
 * \param tm Points to a structure which contains the date and time
 *           information.
 *
 * \return 0 on success or -1 in case of an error.
 */
int X12RtcSetClock(CONST struct _tm *tm)
{
    uint8_t data[10];

    memset(data, 0, sizeof(data));
    if (tm) {
        data[1] = X12RTC_SC;
        data[2] = BIN2BCD(tm->tm_sec);
        data[3] = BIN2BCD(tm->tm_min);
        data[4] = BIN2BCD(tm->tm_hour) | 0x80;
        data[5] = BIN2BCD(tm->tm_mday);
        data[6] = BIN2BCD(tm->tm_mon + 1);

        if (rtc_chip) {
            /* X1286. */
            data[7] = BIN2BCD(tm->tm_year % 100);
        }
        /* X1226. */
        else if (tm->tm_year > 99) {
            data[7] = BIN2BCD(tm->tm_year - 100);
            data[9] = 0x20;
        }
        else {
            data[7] = BIN2BCD(tm->tm_year);
            data[9] = 0x19;
        }
        data[8] = tm->tm_wday;
    }
    return X12RtcWrite(0, data, 10);
}

/*!
 * \brief Get alarm date and time of an X12xx hardware clock.
 *
 * \deprecated New applications must use NutRtcGetAlarm().
 *
 * \param idx   Zero based index. Two alarms are supported.
 * \param tm    Points to a structure that receives the date and time
 *              information.
 * \param aflgs Points to an unsigned long that receives the enable flags.
 *
 * \return 0 on success or -1 in case of an error.
 *
 */
int X12RtcGetAlarm(int idx, struct _tm *tm, int *aflgs)
{
    int rc;
    uint8_t data[8];

    *aflgs = 0;
    memset(tm, 0, sizeof(struct _tm));
    if ((rc = X12RtcReadRegs(idx * 8, data, 8)) == 0) {
        if (data[0] & X12RTC_SCA_ESC) {
            *aflgs |= RTC_ALARM_SECOND;
            tm->tm_sec = BCD2BIN(data[0] & 0x7F);
        }
        if (data[1] & X12RTC_MNA_EMN) {
            *aflgs |= RTC_ALARM_MINUTE;
            tm->tm_min = BCD2BIN(data[1]);
        }
        if (data[2] & X12RTC_HRA_EHR) {
            *aflgs |= RTC_ALARM_HOUR;
            tm->tm_hour = BCD2BIN(data[2] & ~0x80);
        }
        if (data[3] & X12RTC_DTA_EDT) {
            *aflgs |= RTC_ALARM_MDAY;
            tm->tm_mday = BCD2BIN(data[3]);
        }
        if (data[4] & X12RTC_MOA_EMO) {
            *aflgs |= RTC_ALARM_MONTH;
            tm->tm_mon = BCD2BIN(data[4]) - 1;
        }
        if (data[6] & X12RTC_DWA_EDW) {
            *aflgs |= RTC_ALARM_WDAY;
            tm->tm_wday = BCD2BIN(data[6]);
        }
    }
    return rc;
}

/*!
 * \brief Set alarm of an X12xx hardware clock.
 *
 * \deprecated New applications must use NutRtcSetAlarm().
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
int X12RtcSetAlarm(int idx, CONST struct _tm *tm, int aflgs)
{
    uint8_t data[10];

    memset(data, 0, sizeof(data));
    data[1] = idx * 8;
    if (tm) {
        if (aflgs & RTC_ALARM_SECOND) {
            data[2] = BIN2BCD(tm->tm_sec) | X12RTC_SCA_ESC;
        }
        if (aflgs & RTC_ALARM_MINUTE) {
            data[3] = BIN2BCD(tm->tm_min) | X12RTC_MNA_EMN;
        }
        if (aflgs & RTC_ALARM_HOUR) {
            data[4] = BIN2BCD(tm->tm_hour) | X12RTC_HRA_EHR;
        }
        if (aflgs & RTC_ALARM_MDAY) {
            data[5] = BIN2BCD(tm->tm_mday) | X12RTC_DTA_EDT;
        }
        if (aflgs & RTC_ALARM_MONTH) {
            data[6] = BIN2BCD(tm->tm_mon + 1) | X12RTC_MOA_EMO;
        }
        if (aflgs & RTC_ALARM_WDAY) {
            data[8] = BIN2BCD(tm->tm_wday) | X12RTC_DWA_EDW;
        }
    }
    return X12RtcWrite(1, data, 10);
}

/*!
 * \brief Query RTC status flags.
 *
 * \deprecated New applications must use NutRtcGetStatus().
 *
 * \param sflgs Points to an unsigned long that receives the status flags.
 *              - Bit 0: Power fail.
 *              - Bit 5: Alarm 0 occured.
 *              - Bit 6: Alarm 1 occured.
 *
 * \return 0 on success or -1 in case of an error.
 */
int X12RtcGetStatus(uint32_t *sflgs)
{
    int rc;
    uint8_t data;

    if ((rc = X12RtcReadRegs(X12RTC_SR, &data, 1)) == 0) {
        rtc_status |= data;
        *sflgs = rtc_status;
    }
    return rc;
}

/*!
 * \brief Clear RTC status flags.
 *
 * \deprecated New applications must use NutRtcClearStatus().
 *
 * \param sflgs Status flags to clear.
 *
 * \return Always 0.
 */
int X12RtcClearStatus(uint32_t sflgs)
{
    rtc_status &= ~sflgs;

    return 0;
}

NUTRTC rtcX12x6 = {
    X12Init,            /*!< Hardware initializatiuon, rtc_init */
    X12RtcGetClock,     /*!< Read date and time, rtc_gettime */
    X12RtcSetClock,     /*!< Set date and time, rtc_settime */
    X12RtcGetAlarm,     /*!< Read alarm date and time, rtc_getalarm */
    X12RtcSetAlarm,     /*!< Set alarm date and time, rtc_setalarm */
    X12RtcGetStatus,    /*!< Read status flags, rtc_getstatus */
    X12RtcClearStatus   /*!< Clear status flags, rtc_clrstatus */
};


/*!
 * \brief Read contents from non-volatile EEPROM.
 *
 * \param addr  Start location.
 * \param buff  Points to a buffer that receives the contents.
 * \param len   Number of bytes to read.
 *
 * \return 0 on success or -1 in case of an error.
 */
int X12EepromRead(unsigned int addr, void *buff, size_t len)
{
    int rc = -1;
    uint8_t wbuf[2];

    wbuf[0] = (uint8_t)(addr >> 8);
    wbuf[1] = (uint8_t)addr;
    if (TwMasterTransact(I2C_SLA_EEPROM, wbuf, 2, buff, len, NUT_WAIT_INFINITE) == len) {
        rc = 0;
    }
    return rc;
}

/*!
 * \brief Store buffer contents in non-volatile EEPROM.
 *
 * The EEPROM of the X122x has a capacity of 512 bytes, while the X1286 is
 * able to store 32 kBytes.
 *
 * \param addr  Storage start location.
 * \param buff  Points to a buffer that contains the bytes to store.
 * \param len   Number of valid bytes in the buffer.
 *
 * \return 0 on success or -1 in case of an error.
 */
int X12EepromWrite(unsigned int addr, CONST void *buff, size_t len)
{
    int rc = 0;
    uint8_t *wbuf;
    size_t wlen;
    CONST uint8_t *wp = buff;

    /*
     * Loop for each page to be written to.
     */
    while (len) {
        /* Do not cross page boundaries. */
        wlen = EEPROM_PAGE_SIZE - (addr & (EEPROM_PAGE_SIZE - 1));
        if (wlen > len) {
            wlen = len;
        }

        /* Allocate and set a TWI write buffer. */
        if ((wbuf = malloc(wlen + 2)) == 0) {
            rc = -1;
            break;
        }
        wbuf[0] = (uint8_t)(addr >> 8);
        wbuf[1] = (uint8_t)addr;
        memcpy(wbuf + 2, (void *)wp, wlen);

        /* Enable EEPROM write access and send the write buffer. */
        if ((rc = X12WriteEnable(1)) == 0) {
            rc = TwMasterTransact(I2C_SLA_EEPROM, wbuf, wlen + 2, 0, 0, NUT_WAIT_INFINITE);
        }

        /* Release the buffer and check the result. */
        free(wbuf);
        if (rc) {
            break;
        }
        len -= wlen;
        addr += wlen;
        wp += wlen;

        /* Poll for write cycle finished. */
        if ((rc = X12WaitReady()) != 0) {
            break;
        }
    }
    X12WriteEnable(0);

    return rc;
}

/*!
 * \brief Initialize the interface to an Intersil X12xx hardware clock.
 *
 * \deprecated New applications must use NutRegisterRtc().
 *
 * \return 0 on success or -1 in case of an error.
 *
 */
int X12Init(void)
{
    int rc;
    uint32_t tmp;
    uint8_t data[2];

    /* Initialize I2C bus. */
    if ((rc = TwInit(0)) == 0) {
        /* Query RTC status. */
        if ((rc = X12RtcGetStatus(&tmp)) == 0) {
            /*
             * If I2C initialization and RTC status query succeeded, try
             * to determine the chip we got. On Ethernut 3.0 Rev-D the
             * X1226 had been mounted, while the X1286 is used on Rev-E
             * boards. We read register address 0x0037, which is the
             * century on the X1226, but the hundreds of seconds on the
             * X1286. Thus, the register contents on the latter will cange
             * every 10 milliseconds, while it is fixed to 19 or 20 on
             * the first one.
             */
            X12RtcReadRegs(X128xRTC_SSEC, &data[0], 1);
            NutSleep(20);
            X12RtcReadRegs(X128xRTC_SSEC, &data[1], 1);
            if (data[0] != data[1]) {
                rtc_chip = 1;
            }
#ifdef X12DEBUG
            printf("[RTC=X12%c6]", rtc_chip ? '8' : '2');
#endif
        }
    }
    return rc;
}
