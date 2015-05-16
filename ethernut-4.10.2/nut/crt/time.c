/*
 * Copyright (C) 2001-2007 by egnite Software GmbH. All rights reserved.
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
/*
 * $Log$
 * Revision 1.7  2009/02/13 14:52:05  haraldkipp
 * Include memdebug.h for heap management debugging support.
 *
 * Revision 1.6  2008/08/11 06:59:41  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.5  2007/06/03 08:49:56  haraldkipp
 * Bug #1724015 fixed. Simplified code in time().
 *
 * Revision 1.4  2006/10/05 17:20:13  haraldkipp
 * Standard C functions now support hardware RTC.
 * Ugly type conversion replaced.
 *
 * Revision 1.3  2003/12/19 22:26:38  drsung
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
#include <sys/timer.h>
#include <sys/atom.h>
#include <stdlib.h>
#include <memdebug.h>

#include <dev/rtc.h>

static uint32_t epo_offs;

/*!
 * \addtogroup xgCrtTime
 *
 */
/*@{*/

/*!
 * \brief Get the system time.
 *
 * The \b time function returns the number of seconds elapsed since midnight (00:00:00), January 1, 1970, 
 * coordinated universal time (UTC), according to the system clock. The return value is stored in the location 
 * given by \e timer. This parameter may be \b NULL, in which case the return value is not stored.
 *
 * \param timer Pointer to the storage location for time. 
 * \return Return the time in elapsed seconds. There is no error return. 
 *
 */
time_t time(time_t * timer)
{
    struct _tm *tm;
    /* Initially use internal seconds counter. */
    time_t r = epo_offs + NutGetSeconds();

    /* Try to get time from hardware clock. */
    if ((tm = malloc(sizeof(struct _tm))) != NULL) {
        tm->tm_isdst = -1;
        if (NutRtcGetTime(tm) == 0) {
            /* Success. */
            r = _mkgmtime(tm);
        }
        free(tm);
    }

    /* Return result. */
    if (timer) {
        *timer = r;
    }
    return r;
}

/*!
 * \brief Set the system time.
 *
 * \param timer Pointer to the storage location for time.
 * \return This function always returns 0.
 *
 */
int stime(time_t * timer)
{
    /* Try to set hardware clock. */
    NutRtcSetTime(gmtime(timer));
    /* Set internal seconds counter. */
    epo_offs = (uint32_t)(*timer) - NutGetSeconds();

    return 0;
}

/*@}*/
