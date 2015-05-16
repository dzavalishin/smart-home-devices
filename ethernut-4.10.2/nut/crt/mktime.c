/*
 * Copyright (C) 2001-2003 by egnite Software GmbH. All rights reserved.
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
 * Portions of the following functions are derived from material which is 
 * Copyright (c) 1985 by Microsoft Corporation.  All rights are reserved.
 */
/*
 * $Log$
 * Revision 1.4  2009/01/17 15:37:52  haraldkipp
 * Added some NUTASSERT macros to check function parameters.
 *
 * Revision 1.3  2008/08/11 06:59:40  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.2  2003/12/19 22:26:37  drsung
 * Dox written.
 *
 * Revision 1.1  2003/11/24 18:07:37  drsung
 * first release
 *
 *
 */

#include <stdint.h>

#include <time.h>
#include "ctime.h"

#include <sys/nutdebug.h>

#define __need_NULL
#include <stddef.h>

/*
 * ChkAdd evaluates to TRUE if dest = src1 + src2 has overflowed
 */
#define ChkAdd(dest, src1, src2)   ( ((src1 >= 0L) && (src2 >= 0L) \
    && (dest < 0L)) || ((src1 < 0L) && (src2 < 0L) && (dest >= 0L)) )

/*
 * ChkMul evaluates to TRUE if dest = src1 * src2 has overflowed
 */
#define ChkMul(dest, src1, src2)   ( src1 ? (dest/src1 != src2) : 0 )

/*!
 * \addtogroup xgCrtTime
 * @{
 */

static time_t _make_time_t(tm * tb, int ultflag)
{
    long tmptm1, tmptm2, tmptm3;
    tm *tbtemp;

    NUTASSERT(tb != NULL);

    /*
     * First, make sure tm_year is reasonably close to being in range.
     */
    if (((tmptm1 = tb->tm_year) < _BASE_YEAR - 1) || (tmptm1 > _MAX_YEAR + 1))
        goto err_mktime;


    /*
     * Adjust month value so it is in the range 0 - 11.  This is because
     * we don't know how many days are in months 12, 13, 14, etc.
     */

    if ((tb->tm_mon < 0) || (tb->tm_mon > 11)) {

        /*
         * no danger of overflow because the range check above.
         */
        tmptm1 += (tb->tm_mon / 12);

        if ((tb->tm_mon %= 12) < 0) {
            tb->tm_mon += 12;
            tmptm1--;
        }

        /*
         * Make sure year count is still in range.
         */
        if ((tmptm1 < _BASE_YEAR - 1) || (tmptm1 > _MAX_YEAR + 1))
            goto err_mktime;
    }

        /***** HERE: tmptm1 holds number of elapsed years *****/

    /*
     * Calculate days elapsed minus one, in the given year, to the given
     * month. Check for leap year and adjust if necessary.
     */
    tmptm2 = _days[tb->tm_mon];
    if (!(tmptm1 & 3) && (tb->tm_mon > 1))
        tmptm2++;

    /*
     * Calculate elapsed days since base date (midnight, 1/1/70, UTC)
     *
     *
     * 365 days for each elapsed year since 1970, plus one more day for
     * each elapsed leap year. no danger of overflow because of the range
     * check (above) on tmptm1.
     */
    tmptm3 = (tmptm1 - _BASE_YEAR) * 365L + ((tmptm1 - 1L) >> 2)
        - _LEAP_YEAR_ADJUST;

    /*
     * elapsed days to current month (still no possible overflow)
     */
    tmptm3 += tmptm2;

    /*
     * elapsed days to current date. overflow is now possible.
     */
    tmptm1 = tmptm3 + (tmptm2 = (long) (tb->tm_mday));
    if (ChkAdd(tmptm1, tmptm3, tmptm2))
        goto err_mktime;

        /***** HERE: tmptm1 holds number of elapsed days *****/

    /*
     * Calculate elapsed hours since base date
     */
    tmptm2 = tmptm1 * 24L;
    if (ChkMul(tmptm2, tmptm1, 24L))
        goto err_mktime;

    tmptm1 = tmptm2 + (tmptm3 = (long) tb->tm_hour);
    if (ChkAdd(tmptm1, tmptm2, tmptm3))
        goto err_mktime;

        /***** HERE: tmptm1 holds number of elapsed hours *****/

    /*
     * Calculate elapsed minutes since base date
     */

    tmptm2 = tmptm1 * 60L;
    if (ChkMul(tmptm2, tmptm1, 60L))
        goto err_mktime;

    tmptm1 = tmptm2 + (tmptm3 = (long) tb->tm_min);
    if (ChkAdd(tmptm1, tmptm2, tmptm3))
        goto err_mktime;

        /***** HERE: tmptm1 holds number of elapsed minutes *****/

    /*
     * Calculate elapsed seconds since base date
     */

    tmptm2 = tmptm1 * 60L;
    if (ChkMul(tmptm2, tmptm1, 60L))
        goto err_mktime;

    tmptm1 = tmptm2 + (tmptm3 = (long) tb->tm_sec);
    if (ChkAdd(tmptm1, tmptm2, tmptm3))
        goto err_mktime;

        /***** HERE: tmptm1 holds number of elapsed seconds *****/

    if (ultflag) {

        /*
         * Adjust for timezone. No need to check for overflow since
         * localtime() will check its arg value
         */

        tmptm1 += _timezone;

        /*
         * Convert this second count back into a time block structure.
         * If localtime returns NULL, return an error.
         */
        if ((tbtemp = localtime(&tmptm1)) == NULL)
            goto err_mktime;

        /*
         * Now must compensate for DST. The ANSI rules are to use the
         * passed-in tm_isdst flag if it is non-negative. Otherwise,
         * compute if DST applies. Recall that tbtemp has the time without
         * DST compensation, but has set tm_isdst correctly.
         */
        if ((tb->tm_isdst > 0) || ((tb->tm_isdst < 0) && (tbtemp->tm_isdst > 0))) {
            tmptm1 += _dstbias;
            tbtemp = localtime(&tmptm1);        /* reconvert, can't get NULL */
        }

    } else {
        if ((tbtemp = gmtime(&tmptm1)) == NULL)
            goto err_mktime;
    }

        /***** HERE: tmptm1 holds number of elapsed seconds, adjusted *****/
        /*****       for local time if requested                      *****/

    *tb = *tbtemp;
    return (time_t) tmptm1;

  err_mktime:
    /*
     * All errors come to here
     */
    return (time_t) (-1);
}

/*!
 * \brief Convert the local time to a calendar value.
 *
 * The \b mktime function converts the supplied time structure (possibly incomplete)
 * pointed to by \e timeptr into a fully defined structure with normalized values 
 * and then converts it to a ::time_t calendar time value. The converted time has 
 * the same encoding as the values returned by the ::time function. The original 
 * values of the \c tm_wday and \c tm_yday components of the \e timeptr structure are 
 * ignored, and the original values of the other components are not restricted 
 * to their normal ranges. 
 *
 * After an adjustment to Greenwich Mean Time (GMT), \b mktime handles dates from 
 * midnight, January 1, 1970, to January 19, 3:14:07, 2038. This adjustment may 
 * cause \b mktime to return -1 (cast to ::time_t) even though the date you specify 
 * is within range. For example, if you are in Cairo, Egypt, which is two hours ahead
 * of GMT, two hours will first be subtracted from the date you specify in \e timeptr; 
 * this may now put your date out of range.
 *
 * If successful, \b mktime sets the values of \c tm_wday and \c tm_yday as appropriate 
 * and sets the other components to represent the specified calendar time, but with 
 * their values forced to the normal ranges. The final value of tm_mday is not set until 
 * tm_mon and tm_year are determined. When specifying a tm structure time, set 
 * the \c tm_isdst field to: 
 * 
 * - Zero (0) to indicate that standard time is in effect. 
 * - A value greater than 0 to indicate that daylight savings time is in effect. 
 *
 * \c tm_isdst is a required field. If not set, its value is undefined and the return value
 * from \b mktime is unpredictable. If \e timeptr points to a ::tm structure returned by a 
 * previous call to ::gmtime or ::localtime, the \c tm_isdst field contains the correct value.
 *
 * \note Note that ::gmtime and ::localtime use a single statically allocated buffer 
 * for the conversion. If you supply this buffer to \b mktime, the previous contents are destroyed.
 *
 * \param timeptr Pointer to time structure.
 * \return \b mktime returns the specified calendar time encoded as a value of type
 * ::time_t. If \e timeptr references a date before midnight, January 1, 1970, or 
 * if the calendar time cannot be represented, \b mktime returns –1 cast to type ::time_t. 
 * When using \b mktime and if \e timeptr references a date after 3:14:07 January 19, 2038, UTC,
 * it will return –1 cast to type ::time_t. 
 *
 */
time_t mktime(tm * timeptr)
{
    return (_make_time_t(timeptr, 1));
}

time_t _mkgmtime(tm * timeptr)
{
    return (_make_time_t(timeptr, 0));
}

/*@}*/
