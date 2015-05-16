/*
 * Copyright (C) 2007 by egnite Software GmbH. All rights reserved.
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
 * \file pro/rfctime.c
 * \brief RFC date and time conversion.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.2  2009/01/04 21:06:53  olereinhardt
 * 2009-01-04  Ole Reinhardt <ole.reinhardt@thermotemp.de>
 *
 * 	* pro/rfctime.c: Fixed time parsing in RfcTimeParse
 * 	                 Thanks to Michael Fischer!
 *
 * Revision 1.1  2008/02/15 16:46:09  haraldkipp
 * First release.
 *
 *
 * \endverbatim
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <pro/rfctime.h>

static char rfc1123_buf[32];
static char *wkdays[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
static char *months[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

/*!
 * \brief Skip spaces and tabs.
 *
 * \param str Pointer to the string containing the leading spaces.
 *
 * \return Pointer into the string after the last space character.
 */
static char *skip_spaces(CONST char *str)
{
    while (*str == ' ' || *str == '\t')
        str++;
    return (char *)str;
}

/*!
 * \brief Parse digits.
 *
 * Number base is 10.
 *
 * \param str  Pointer to the string containing the digits.
 * \param year Points to the variable that will receive the binary value.
 *
 * \return Pointer into the string after the parsed characters.
 */
static char *parse_digits(CONST char *str, int *val)
{
    *val = 0;
    while (isdigit((unsigned char)*str)) {
        *val *= 10;
        *val += *str++ - '0';
    }
    return (char *)str;
}

/*!
 * \brief Parse string containing the year.
 *
 * Works with years including and excluding the century. If the
 * resulting value is lower than 70, the 21st century is assumed.
 * Values
 *
 * \param str  Pointer to the time string.
 * \param year Points to the variable that will receive the years since 1900.
 *
 * \return Pointer into the string after the parsed characters.
 */
char *TimeParseYear(CONST char *str, int *year)
{
    str = parse_digits(str, year);
    if (*year < 70) {
        *year += 100;
    }
    else if (*year > 1900) {
        *year -= 1900;
    }
    return (char *)str;
}

/*!
 * \brief Parse a string for the name of a month.
 *
 * Does basic comparision only, no range checking etc. For example, if
 * the string starts with capital letter 'D', December is assumed without
 * further checking the rest. It will work for abbreviated as well as
 * full names.
 *
 * \param str   Pointer to the time string. Must start with a capital letter
 *              followed by lower case characters.
 * \param month Points to the variable that will receive the month (0..11).
 *
 * \return Pointer into the string after the parsed characters.
 */
char *TimeParseMonth(CONST char *str, int *month)
{
    if (*str == 'A') {
        if (*++str == 'p' || *str == 'P') {
            /* April */
            *month = 3;
        }
        else {
            /* August */
            *month = 7;
        }
    }
    else if (*str == 'D') {
        /* December */
        *month = 11;
    }
    else if (*str == 'F') {
        /* February */
        *month = 1;
    }
    else if (*str == 'J') {
        if (*++str == 'a' || *str == 'A') {
            /* January */
            *month = 0;
        }
        else if (*str && (*++str == 'l' || *str == 'L')) {
            /* July */
            *month = 6;
        }
        else {
            /* June */
            *month = 5;
        }
    }
    else if (*str == 'M') {
        if (*++str == 'a' && (*++str == 'r' || *str == 'R')) {
            /* March */
            *month = 2;
        }
        else {
            /* May */
            *month = 4;
        }
    }
    else if (*str == 'N') {
        /* November */
        *month = 10;
    }
    else if (*str == 'O') {
        /* October */
        *month = 9;
    }
    else {
        /* September */
        *month = 8;
    }
    while (isalpha((unsigned char)*str)) {
        str++;
    }
    return (char *)str;
}

/*!
 * \brief Parse date string.
 *
 * Parses a string of the format day 'x' month-name 'x' year, where
 * 'x' is any non-alphanumeric character.
 *
 * \param str  Pointer to the date string.
 * \param mday Points to the variable that will receive the day (1..31).
 * \param mon  Points to the variable that will receive the month (0..11).
 * \param year Points to the variable that will receive the years since 1900.
 *
 * \return Pointer into the string after the parsed characters.
 */
char *TimeParseDmy(CONST char *str, int *mday, int *mon, int *year)
{
    str = parse_digits(str, mday);
    while (*str && !isalpha((unsigned char)*str)) {
        str++;
    }
    str = TimeParseMonth(str, mon);
    while (*str && !isdigit((unsigned char)*str)) {
        str++;
    }
    str = TimeParseYear(str, year);

    return (char *)str;
}

/*!
 * \brief Parse time of day string.
 *
 * Parses a string of the format hour ':' minute ':' second.
 *
 * \param str  Pointer to the time string.
 * \param hour Points to the variable that will receive the hour (0..23).
 * \param min  Points to the variable that will receive the minute (0..59).
 * \param sec  Points to the variable that will receive the second (0..59).
 *
 * \return Pointer into the string after the parsed characters.
 */
char *TimeParseHms(CONST char *str, int *hour, int *min, int *sec)
{
    str = parse_digits(str, hour);
    if (*str == ':') {
        str = parse_digits(str + 1, min);
        if (*str == ':') {
            str = parse_digits(str + 1, sec);
        }
        else {
            *sec = 0;
        }
    }
    else {
        *min = 0;
    }
    return (char *)str;
}

/*!
 * \brief Parse RFC date and time string.
 *
 * This routine accepts RFC 850, RFC 1123 and asctime time formats.
 *
 * \param str Pointer to the date and time string.
 *
 * \return Number of seconds since epoch or -1 in case of any error.
 */
time_t RfcTimeParse(CONST char *str)
{
    struct _tm dts = { 0, 0, 0, 1, 0, 0, 0, 0, 0 };

    /* Skip leading whitespace. */
    str = skip_spaces(str);

    /* Skip weekday, optional in RFC 822. */
    if (isalpha((unsigned char)*str)) {
        while (*str && *str != ' ' && *str != '\t')
            str++;
        str = skip_spaces(str);
    }

    if (isalpha((unsigned char)*str)) {
        /* asctime format 'Fri Feb 2 2007 07:30:05'. */
        str = TimeParseMonth(str, &dts.tm_mon);
        str = skip_spaces(str);
        str = parse_digits(str, &dts.tm_mday);
        str = skip_spaces(str);
        str = TimeParseYear(str, &dts.tm_year);
        str = skip_spaces(str);
        str = TimeParseHms(str, &dts.tm_hour, &dts.tm_min, &dts.tm_sec);
    }
    else if (*str) {
        /* RFC 850 'Friday, 02-Feb-2007 07:30:05 GMT'. */
        /* RFC 1123 'Fri, 02 Feb 2007 07:30:05 GMT'. */
        str = TimeParseDmy(str, &dts.tm_mday, &dts.tm_mon, &dts.tm_year);
        str = skip_spaces(str);
        str = TimeParseHms(str, &dts.tm_hour, &dts.tm_min, &dts.tm_sec);
    }
    str = skip_spaces(str);
    if (strcmp(str, "GMT") == 0) {
        return mktime(&dts);
    }
    return _mkgmtime(&dts);
}

/*!
 * \brief Create RFC 1123 date and time string.
 *
 * \param tm Pointer to the date and time structure.
 *
 * \return Pointer to a static buffer containing the string.
 */
char *Rfc1123TimeString(struct _tm *tm)
{
    sprintf(rfc1123_buf, "%s, %02d %s %04d %02d:%02d:%02d",
            wkdays[tm->tm_wday],
            tm->tm_mday, months[tm->tm_mon], tm->tm_year + 1900,
            tm->tm_hour, tm->tm_min, tm->tm_sec);

    return rfc1123_buf;
}
