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
 * Revision 1.6  2009/01/17 15:37:52  haraldkipp
 * Added some NUTASSERT macros to check function parameters.
 *
 * Revision 1.5  2008/08/11 06:59:40  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.4  2004/10/14 16:43:00  drsung
 * Fixed compiler warning "comparison between signed and unsigned"
 *
 * Revision 1.3  2003/12/19 22:26:37  drsung
 * Dox written.
 *
 * Revision 1.2  2003/11/26 11:14:32  haraldkipp
 * Portability issues
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

#define LONG_MAX   2147483647L

/*!
 * \addtogroup xgCrtTime
 * @{
 */

/*!
 * \brief Convert a time value and correct for the local time zone.
 *
 * Thread safe version of \b localtime. See ::localtime for more information.
 *
 * \param timer Pointer to stored time. 
 * \param ptm Pointer to structure ::tm where the converted time is stored. 
 * \return Always return 0.
 *
 */
int localtime_r(CONST time_t * timer, tm * ptm)
{
    long ltime;

    NUTASSERT(timer != NULL);
    if ((*timer > (time_t)(3 * _DAY_SEC)) && (*timer < (time_t)(LONG_MAX - 3 * _DAY_SEC))) {
        /*
         * The date does not fall within the first three, or last
         * three, representable days of the Epoch. Therefore, there
         * is no possibility of overflowing or underflowing the
         * time_t representation as we compensate for timezone and
         * Daylight Savings Time.
         */

        ltime = (long) *timer - _timezone;
        gmtime_r((time_t *) & ltime, ptm);

        /*
         * Check and adjust for Daylight Saving Time.
         */
        if (_daylight && _isindst(ptm)) {
            ltime -= _dstbias;
            gmtime_r((time_t *) & ltime, ptm);
            ptm->tm_isdst = 1;
        }
    } else {
        gmtime_r(timer, ptm);

        /*
         * The date falls with the first three, or last three days
         * of the Epoch. It is possible the time_t representation
         * would overflow or underflow while compensating for
         * timezone and Daylight Savings Time. Therefore, make the
         * timezone and Daylight Savings Time adjustments directly
         * in the tm structure. The beginning of the Epoch is
         * 00:00:00, 01-01-70 (UCT) and the last representable second
         * in the Epoch is 03:14:07, 01-19-2038 (UCT). This will be
         * used in the calculations below.
         *
         * First, adjust for the timezone.
         */
        if (_isindst(ptm))
            ltime = (long) ptm->tm_sec - (_timezone + _dstbias);
        else
            ltime = (long) ptm->tm_sec - _timezone;
        ptm->tm_sec = (int) (ltime % 60);
        if (ptm->tm_sec < 0) {
            ptm->tm_sec += 60;
            ltime -= 60;
        }

        ltime = (long) ptm->tm_min + ltime / 60;
        ptm->tm_min = (int) (ltime % 60);
        if (ptm->tm_min < 0) {
            ptm->tm_min += 60;
            ltime -= 60;
        }

        ltime = (long) ptm->tm_hour + ltime / 60;
        ptm->tm_hour = (int) (ltime % 24);
        if (ptm->tm_hour < 0) {
            ptm->tm_hour += 24;
            ltime -= 24;
        }

        ltime /= 24;

        if (ltime > 0L) {
            /*
             * There is no possibility of overflowing the tm_mday
             * and tm_yday fields since the date can be no later
             * than January 19.
             */
            ptm->tm_wday = (ptm->tm_wday + ltime) % 7;
            ptm->tm_mday += ltime;
            ptm->tm_yday += ltime;
        } else if (ltime < 0L) {
            /*
             * It is possible to underflow the tm_mday and tm_yday
             * fields. If this happens, then adjusted date must
             * lie in December 1969.
             */
            ptm->tm_wday = (ptm->tm_wday + 7 + ltime) % 7;
            if ((ptm->tm_mday += ltime) <= 0) {
                ptm->tm_mday += 31;
                ptm->tm_yday = 364;
                ptm->tm_mon = 11;
                ptm->tm_year--;
            } else {
                ptm->tm_yday += ltime;
            }
        }


    }
    return 0;
}


/*!
 * \brief Convert a time value and correct for the local time zone.
 *
 * The localtime function converts a time stored as a time_t value and stores the 
 * result in a structure of type ::tm. The long value \e timer represents the seconds 
 * elapsed since midnight (00:00:00), January 1, 1970, UTC. This value is usually 
 * obtained from the ::time function.
 *
 * ::gmtime, ::mktime, and \b localtime all use a single statically allocated ::tm structure 
 * for the conversion. Each call to one of these routines destroys the result of the 
 * previous call.
 *
 * \b localtime corrects for the local time zone if the user first sets the global 
 * variable #_timezone. 
 * 
 * \param timer Pointer to stored time. 
 * \return Return a pointer to the structure result. If the value in \e timer 
 * represents a date before midnight, January 1, 1970, return \b NULL.
 *
 */
/*
  Note: This function is *not* thread safe, because it uses a static variable
  to store the calculated values. To be safe, you must surround the call to localtime 
  _and_ the usage of the returned pointer with NutEnterCritical() and NutExitCritical()!
  Provided for compatibility to std c lib.
*/
tm *localtime(CONST time_t * timer)
{
    if (localtime_r(timer, &_tb))
        return NULL;
    else
        return &_tb;
}

/*@}*/
