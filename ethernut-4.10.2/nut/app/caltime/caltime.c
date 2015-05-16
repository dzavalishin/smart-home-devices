/*!
 * Copyright (C) 2001-2005 by egnite Software GmbH
 * Copyright (C) 2009-2010 by egnite GmbH
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
 * $Id: caltime.c 3538 2011-08-10 16:34:10Z haraldkipp $
 */

#include <dev/board.h>
#include <dev/debug.h>

#include <sys/version.h>
#include <sys/timer.h>

#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <string.h>
#include <time.h>
#ifdef USE_BUILD_TIME
#include <pro/rfctime.h>
#endif

#ifdef USE_LINE_EDITOR
#include <gorp/edline.h>
#define LINE_MAXLENGTH  80
static char edbuff[LINE_MAXLENGTH];
static EDLINE *edline;
#endif

/*!
 * \example caltime/caltime.c
 *
 * Demonstrates Nut/OS date and time functions, which had been contributed
 * by Oliver Schulz.
 *
 * Check the Makefile for additional options.
 */
static char *version = "2.1";


/* Used for ASCII Art Animation. */
static char *rotor = "|/-\\";

static char *weekday_name[7] = {
    "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
};

/*
 * Print content of tm structure.
 */
static void PrintDateTime(CONST struct _tm *stm)
{
    printf("%s, %04d/%02d/%02d, %02d:%02d:%02d%s"
           , weekday_name[stm->tm_wday]
           , stm->tm_year + 1900, stm->tm_mon + 1, stm->tm_mday
           , stm->tm_hour, stm->tm_min, stm->tm_sec
           , stm->tm_isdst ? " DST" : ""
           );
}

/*
 * Query calendar date from user.
 *
 * Returns 0 on success, -1 otherwise.
 */
static int EnterDate(struct _tm *stm)
{
    int year = stm->tm_year + 1900;
    int month = stm->tm_mon + 1;
    int day = stm->tm_mday;

    printf("Enter date, use format YYYY/MM/DD");
#ifdef USE_LINE_EDITOR
    printf("  : ");
    sprintf(edbuff, "%04d/%02d/%02d", year, month, day);
    EdLineRead(edline, edbuff, LINE_MAXLENGTH);
    sscanf(edbuff, "%d/%d/%d", &year, &month, &day);
#else
    printf(" (%04d/%02d/%02d): ", year, month, day);
    scanf("%d/%d/%d", &year, &month, &day);
    putchar('\n');
#endif

    if (year < 1970 || year > 2038) {
        printf("Bad year: %d is not within range 1970..2038\n", year);
        return -1;
    }
    if (month < 1 || month > 12) {
        printf("Bad month: %d is not within range 1..12\n", month);
        return -1;
    }
    if (day < 1 || day > 31) {
        printf("Bad day: %d is not within range 1..31\n", day);
        return -1;
    }
    stm->tm_year = year - 1900;
    stm->tm_mon = month - 1;
    stm->tm_mday = day;

    return 0;
}

/*
 * Query time of day from user.
 *
 * Returns 0 on success, -1 otherwise.
 */
static int EnterTime(struct _tm *stm)
{
    int hour = stm->tm_hour;
    int minute = stm->tm_min;
    int second = stm->tm_sec;

    printf("Enter time, use 24h format HH:MM:SS");
#ifdef USE_LINE_EDITOR
    printf(": ");
    sprintf(edbuff, "%02d:%02d:%02d", hour, minute, second);
    EdLineRead(edline, edbuff, LINE_MAXLENGTH);
    sscanf(edbuff, "%d:%d:%d", &hour, &minute, &second);
#else
    printf(" (%02d:%02d:%02d): ", hour, minute, second);
    scanf("%d:%d:%d", &hour, &minute, &second);
    putchar('\n');
#endif

    if (hour < 0 || hour > 23) {
        printf("Bad hour: %d is not within range 0..23\n", hour);
        return -1;
    }
    if (minute < 0 || minute > 59) {
        printf("Bad minute: %d is not within range 0..59\n", minute);
        return -1;
    }
    if (second < 0 || second > 59) {
        printf("Bad second: %d is not within range 0..59\n", second);
        return -1;
    }
    stm->tm_hour = hour;
    stm->tm_min = minute;
    stm->tm_sec = second;

    return 0;
}

/*
 * Query time difference from user, specified in hours and minutes.
 *
 * 'init' specifies the default number of seconds.
 *
 * Returns the number of seconds.
 */
static long EnterTimeDiff(long init)
{
    long hours;
    long minutes;
    long seconds;

    seconds = init;
    minutes = seconds / 60L;
    hours = minutes / 60L;
    minutes = abs(minutes % 60L);

    printf("Enter time difference in format HH:MM");
#ifdef USE_LINE_EDITOR
    printf(": ");
    sprintf(edbuff, "%+03ld:%02ld", hours, minutes);
    EdLineRead(edline, edbuff, LINE_MAXLENGTH);
    sscanf(edbuff, "%ld:%ld", &hours, &minutes);
#else
    printf(" (%+03ld:%02ld): ", hours, minutes);
    scanf("%ld:%ld", &hours, &minutes);
    putchar('\n');
#endif

    if (hours < -12 || hours > 12) {
        printf("\nBad hours: %ld is not within range -12..+12\n", hours);
        return init;
    }
    if (minutes < 0 || minutes > 59) {
        printf("\nBad minutes: %ld is not within range 0..59\n", minutes);
        return init;
    }
    return (hours * 60L + minutes) * 60L;
}

/*
 * Display the elapsed seconds of the epoch.
 *
 * The value is constantly updated until the user presses a key.
 */
static void DisplaySeconds(void)
{
    uint_fast8_t i = 0;

    while (!kbhit()) {
        printf(" [%c] Seconds since epoch: %ld\r"
               , rotor[++i & 3]
               , (long) time(NULL));
        NutSleep(200);
    }
    putchar('\n');
}

/*
 * Display the coordinated universal time.
 *
 * The value is constantly updated until the user presses a key.
 */
static void DisplayZuluTime(void)
{
    time_t secs;
    struct _tm *gmt;
    uint_fast8_t i = 0;

    while (!kbhit()) {
        secs = time(NULL);
        gmt = gmtime(&secs);
        printf(" [%c] Universal time: ", rotor[++i & 3]);
        PrintDateTime(gmt);
        printf("    \r");
        NutSleep(500);
    }
    putchar('\n');
}

/*
 * Display the local time.
 *
 * The value is constantly updated until the user presses a key.
 */
static void DisplayLocalTime(void)
{
    time_t tt;
    struct _tm *ltm;
    uint_fast8_t i = 0;

    while (!kbhit()) {
        /* Get local time and print it. */
        tt = time(NULL);
        ltm = localtime(&tt);
        printf(" [%c] Local time: ", rotor[++i & 3]);
        PrintDateTime(ltm);

        /* Calculate the offset from UTC in minutes. */
        tt = _timezone;
        if (ltm->tm_isdst > 0) {
            tt += _dstbias;
        }
        tt /= -60L;

        /* Print UTC offset in format HH:MM. */
        printf(" UTC%+03ld:%02ld   \r", tt / 60L, abs(tt) % 60L);
        NutSleep(200);
    }
    putchar('\n');
}

/*
 * Display system up time.
 *
 * The value is constantly updated until the user presses a key.
 */
static void DisplayUpTime(void)
{
    uint32_t hours;
    uint32_t minutes;
    uint32_t seconds;
    uint_fast8_t i = 0;

    while (!kbhit()) {
        seconds = NutGetSeconds();
        minutes = seconds / 60UL;
        hours = minutes / 60UL;
        minutes %= 60UL;
        seconds %= 60UL;
        printf(" [%c] System is running %lu hours"
               ", %lu minutes and %lu seconds   \r"
               , rotor[++i & 3], hours, minutes, seconds);
        NutSleep(500);
    }
    putchar('\n');
}

/*
 * Display the week day of a queried calendar date.
 *
 * mktime() updates the structure members tm_yday and tm_wday.
 * This can be used to determine the week day name of any given
 * date.
 */
static void CalcWeekDay(void)
{
    struct _tm date;
    time_t secs;

    /* Use current local time as default. */
    time(&secs);
    memcpy(&date, localtime(&secs), sizeof(date));
    /* Query date and print week day name if we got a valid entry. */
    if (EnterDate(&date) == 0) {
        mktime(&date);
        puts(weekday_name[date.tm_wday]);
    }
}

/*
 * Query user for a new time zone offset.
 */
static void SetTimeZone(void)
{
    /* Nut/OS uses a global variable to store the current TZ offset. */
    _timezone = EnterTimeDiff(_timezone);
}

/*
 * Query user for a new system time.
 */
static void SetLocalTime(void)
{
    struct _tm ltm;
    time_t now;

    /* Use current local time as default. */
    time(&now);
    memcpy(&ltm, localtime(&now), sizeof(ltm));

    /* Query date and time. */
    if (EnterDate(&ltm) == 0 && EnterTime(&ltm) == 0) {
        /* Let mktime determine whether DST is in effect. */
        ltm.tm_isdst = -1;
        /* mktime expects local time and returns seconds since the epoch. */
        now = mktime(&ltm);
        /* stime expects seconds since the epoch. */
        stime(&now);
    }
}

/*
 * Application entry.
 */
int main(void)
{
    uint32_t baud = 115200;
    int cmd;

    /* Use UART device for stdin and stdout. */
    NutRegisterDevice(&DEV_CONSOLE, 0, 0);
    freopen(DEV_CONSOLE_NAME, "w", stdout);
    freopen(DEV_CONSOLE_NAME, "r", stdin);
    _ioctl(_fileno(stdout), UART_SETSPEED, &baud);
    printf("\n\nCalendar Time %s running on Nut/OS %s\n"
           , version, NutVersionString());

#ifdef USE_LINE_EDITOR
    /* Open line editor, if configured. */
    printf("Opening line editor...");
    edline = EdLineOpen(EDIT_MODE_ECHO);
    if (edline) {
        puts("OK");
    } else {
        puts("failed");
    }
#else
    puts("Note: Enable local echo!");
#endif

#if USE_TIME_ZONE
    _timezone = USE_TIME_ZONE;
#endif

#ifdef RTC_CHIP
    /* Register and query hardware RTC, if available. */
    printf("Registering RTC hardware...");
    if (NutRegisterRtc(&RTC_CHIP)) {
        puts("failed");
    } else {
        uint32_t rtc_stat;

        NutRtcGetStatus(&rtc_stat);
        if (rtc_stat & RTC_STATUS_PF) {
            puts("power failure");
        } else {
            puts("OK");
        }
    }
#elif USE_BUILD_TIME
    {
        /* Initially use the compile date and time. */
        time_t now = RfcTimeParse("Unk, " __DATE__ " " __TIME__);
        stime(&now);
        puts("Built " __DATE__ " " __TIME__);
    }
#endif

    for (;;) {
        /* Print command menu. */
        puts("\n  0 - Display seconds counter");
        puts("  1 - Display universal time");
        puts("  2 - Display local time");
        puts("  3 - Display system uptime");
        puts("  C - Calculate weekday");
        puts("  S - Set local time");
        puts("  Y - Toggle DST calculation");
        puts("  Z - Set timezone");

        printf("What is thy bidding, my master? ");

        /* Flush input buffer. */
        while (kbhit()) {
            cmd = getchar();
        }

        /* Get the next command. */
        cmd = getchar();
        putchar('\n');

        /* Process the command. */
        switch (cmd) {
        case '0':
            DisplaySeconds();
            break;
        case '1':
            DisplayZuluTime();
            break;
        case '2':
            DisplayLocalTime();
            break;
        case '3':
            DisplayUpTime();
            break;
        case 'C':
        case 'c':
            CalcWeekDay();
            break;
        case 'S':
        case 's':
            SetLocalTime();
            break;
        case 'Y':
        case 'y':
            /* Nut/OS uses a global variable to enable/disable DST. 
               Toggle the current status and display the result. */
            _daylight = _daylight == 0;
            printf("DST calculation %sabled\n", _daylight ? "en" : "dis");
            break;
        case 'Z':
        case 'z':
            SetTimeZone();
            break;
        }
    }
    return 0;
}
