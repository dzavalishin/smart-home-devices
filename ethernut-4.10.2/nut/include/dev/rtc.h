#ifndef _DEV_RTC_H_
#define	_DEV_RTC_H_

/*
 * Copyright (C) 2005-2006 by egnite Software GmbH. All rights reserved.
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
 */

/*
 * $Log$
 * Revision 1.2  2008/08/11 06:59:59  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.1  2006/10/05 17:18:49  haraldkipp
 * Hardware independant RTC layer added.
 *
 */

#include <sys/types.h>
#include <stdint.h>

#include <time.h>

#define RTC_STATUS_PF       0x00000001
#define RTC_STATUS_AL0      0x00000020
#define RTC_STATUS_AL1      0x00000040

#define RTC_ALARM_SECOND    0x00000001
#define RTC_ALARM_MINUTE    0x00000002
#define RTC_ALARM_HOUR      0x00000004
#define RTC_ALARM_MDAY      0x00000008
#define RTC_ALARM_MONTH     0x00000010
#define RTC_ALARM_WDAY      0x00000080

/*!
 * \brief Convert binary coded decimal to binary value.
 */
#define BCD2BIN(x) ((((uint8_t)(x)) >> 4) * 10 + ((x) & 0x0F))

/*!
 * \brief Convert binary to binary coded decimal value.
 */
#define BIN2BCD(x) (((((uint8_t)(x)) / 10) << 4) + (x) % 10)

/*!
 * \brief RTC device type.
 */
typedef struct _NUTRTC NUTRTC;

/*!
 * \brief RTC device structure.
 */
struct _NUTRTC {
    int (*rtc_init) (void);
    int (*rtc_gettime) (struct _tm *);
    int (*rtc_settime) (CONST struct _tm *);
    int (*rtc_getalarm) (int idx, struct _tm *, int *);
    int (*rtc_setalarm) (int idx, CONST struct _tm *, int);
    int (*rtc_getstatus) (uint32_t *);
    int (*rtc_clrstatus) (uint32_t);
};

__BEGIN_DECLS
/* Prototypes */
extern int NutRegisterRtc(NUTRTC *rtc);

extern int NutRtcGetTime(struct _tm *tm);
extern int NutRtcSetTime(CONST struct _tm *tm);

extern int NutRtcGetAlarm(int idx, struct _tm *tm, int *aflags);
extern int NutRtcSetAlarm(int idx, CONST struct _tm *tm, int aflags);

extern int NutRtcGetStatus(uint32_t *sflags);
extern int NutRtcClearStatus(uint32_t sflags);

__END_DECLS
/* End of prototypes */
#endif
