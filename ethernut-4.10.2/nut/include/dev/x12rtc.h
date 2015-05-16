#ifndef _DEV_X12RTC_H_
#define	_DEV_X12RTC_H_

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
 */

/*
 * $Log$
 * Revision 1.6  2009/01/17 11:26:47  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.5  2008/08/11 07:00:00  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.4  2007/06/03 08:52:16  haraldkipp
 * Wrong watchdog mode bit fixed.
 *
 * Revision 1.3  2006/10/05 17:18:49  haraldkipp
 * Hardware independant RTC layer added.
 *
 * Revision 1.2  2006/01/19 18:42:21  haraldkipp
 * Make it compile even if the calling module doesn't include time.h.
 *
 * Revision 1.1  2005/10/24 11:41:39  haraldkipp
 * Initial check-in.
 *
 */

#include <dev/rtc.h>

#include <compiler.h> // for NUT_DEPRECATED

/*! \name Non-Volatile Alarm Registers */
/*@{*/
#define X12RTC_SCA0         0x00    /*!< \brief Alarm 0 second. */
#define X12RTC_SCA1         0x08    /*!< \brief Alarm 1 second. */
#define X12RTC_SCA_ESC      0x80    /*!< \brief Second alarm enabled. */

#define X12RTC_MNA0         0x01    /*!< \brief Alarm 0 minute. */
#define X12RTC_MNA1         0x09    /*!< \brief Alarm 1 minute. */
#define X12RTC_MNA_EMN      0x80    /*!< \brief Minute alarm enabled. */

#define X12RTC_HRA0         0x02    /*!< \brief Alarm 0 hour. */
#define X12RTC_HRA1         0x0A    /*!< \brief Alarm 1 hour. */
#define X12RTC_HRA_EHR      0x80    /*!< \brief Hour alarm enabled. */

#define X12RTC_DTA0         0x03    /*!< \brief Alarm 0 day of month. */
#define X12RTC_DTA1         0x0B    /*!< \brief Alarm 1 day of month. */
#define X12RTC_DTA_EDT      0x80    /*!< \brief Day of month alarm enabled. */

#define X12RTC_MOA0         0x04    /*!< \brief Alarm 0 month. */
#define X12RTC_MOA1         0x0C    /*!< \brief Alarm 1 month. */
#define X12RTC_MOA_EMO      0x80    /*!< \brief Month alarm enabled. */

#define X12RTC_YRA0         0x05    /*!< \brief Currently unused alarm 0 register. */
#define X12RTC_YRA1         0x0D    /*!< \brief Currently unused alarm 1 register. */

#define X12RTC_DWA0         0x06    /*!< \brief Alarm 0 weekday. */
#define X12RTC_DWA1         0x0E    /*!< \brief Alarm 1 weekday. */
#define X12RTC_DWA_EDW      0x80    /*!< \brief Weekday alarm enabled. */

#define X12RTC_Y2K0         0x07    /*!< \brief Alarm 0 . */
#define X12RTC_Y2K1         0x0F    /*!< \brief Alarm 1 . */
/*@}*/

/*! \name Non-Volatile Control Registers */
/*@{*/
#define X12RTC_BL           0x10    /*!< \brief Block protection and watchdog register. */

#define X12RTC_BL_WD        0x18    /*!< \brief Watchdog configuration. */
#define X12RTC_BL_WD_1750   0x00    /*!< \brief Timeout after 1.75 seconds. */
#define X12RTC_BL_WD_750    0x08    /*!< \brief Timeout after 750 milliseconds. */
#define X12RTC_BL_WD_250    0x10    /*!< \brief Timeout after 250 milliseconds. */
#define X12RTC_BL_WD_OFF    0x18    /*!< \brief Disabled. */

#define X12RTC_BL_BP        0xE0    /*!< \brief Block protection. */
#define X12RTC_BL_BP_NONE   0x00    /*!< \brief No protection. */
#define X12RTC_BL_BP_UQUAD  0x20    /*!< \brief Upper quarter protected. */
#define X12RTC_BL_BP_UHALF  0x40    /*!< \brief Upper half protected. */
#define X12RTC_BL_BP_FULL   0x60    /*!< \brief Full array protected. */
#define X12RTC_BL_BP_FIRST1 0x80    /*!< \brief First page protected. */
#define X12RTC_BL_BP_FIRST2 0xA0    /*!< \brief First 2 pages protected. */
#define X12RTC_BL_BP_FIRST3 0xC0    /*!< \brief First 4 pages protected. */
#define X12RTC_BL_BP_FIRST8 0xE0    /*!< \brief First 8 pages protected. */

#define X12RTC_INT          0x11    /*!< \brief Interrupt control and freq. output register. */

#define X12RTC_INT_FO       0x14    /*!< \brief Programmable frequency output bits. */
#define X12RTC_INT_FO_IRQ   0x00    /*!< \brief Alarm interrupt. */
#define X12RTC_INT_FO_32KHZ 0x04    /*!< \brief 32.768kHz. */
#define X12RTC_INT_FO_100HZ 0x10    /*!< \brief 100Hz. */
#define X12RTC_INT_FO_1HZ   0x14    /*!< \brief 1Hz. */

#define X12RTC_INT_AL0E     0x20    /*!< \brief Alarm 0 interrupt enable. */
#define X12RTC_INT_AL1E     0x40    /*!< \brief Alarm 1 interrupt enable. */
#define X12RTC_INT_IM       0x80    /*!< \brief Repetitive alarm. */

#define X12RTC_ATR          0x12    /*!< \brief Analog trimming register. */

#define X12RTC_DTR          0x13    /*!< \brief Digital trimming register. */
#define X12RTC_DTR_NONE     0x00    /*!< \brief 0 PPM. */
#define X12RTC_DTR_PLUS10   0x02    /*!< \brief +10 PPM. */
#define X12RTC_DTR_PLUS20   0x01    /*!< \brief +20 PPM. */
#define X12RTC_DTR_PLUS30   0x03    /*!< \brief +30 PPM. */
#define X12RTC_DTR_MINUS10  0x06    /*!< \brief -10 PPM. */
#define X12RTC_DTR_MINUS20  0x05    /*!< \brief -20 PPM. */
#define X12RTC_DTR_MINUS30  0x07    /*!< \brief -30 PPM. */
/*@}*/

/*! \name Volatile Date and Time Registers */
/*@{*/
#define X12RTC_SC           0x30    /*!< Seconds register, 0 - 59. */
#define X12RTC_MN           0x31    /*!< Minutes register, 0 - 59. */
#define X12RTC_HR           0x32    /*!< Hours register, 0 - 23. */
#define X12RTC_HR_MIL       0x80    /*!< Use 24h format. */
#define X12RTC_DT           0x33    /*!< Day register, 1 - 31. */
#define X12RTC_MO           0x34    /*!< Month register, 1 - 12. */
#define X12RTC_YR           0x35    /*!< Year register, 0 - 99. */
#define X12RTC_DW           0x36    /*!< Day of the weeks register, 0 - 6. */
#define X128xRTC_SSEC       0x37    /*!< X1286 1/100 second register, 0 - 99 (read only). */
#define X122xRTC_Y2K        0x37    /*!< X1226 epoch register, 19 or 20. */
/*@}*/

/*! \name Volatile Status Register */
/*@{*/
#define X12RTC_SR           0x3F    /*!< Status register. */
#define X12RTC_SR_RTCF      0x01    /*!< Power failure. */
#define X12RTC_SR_WEL       0x02    /*!< Memory write enable. */
#define X12RTC_SR_RWEL      0x04    /*!< Register write enable. */
#define X12RTC_SR_AL0       0x20    /*!< Alarm 0 indicator. */
#define X12RTC_SR_AL1       0x40    /*!< Alarm 1 indicator. */
#define X12RTC_SR_BAT       0x80    /*!< Operating from battery. */
/*@}*/

extern NUTRTC rtcX12x6;

__BEGIN_DECLS
/* Prototypes */
extern int X12Init(void) NUT_DEPRECATED;

extern int X12RtcGetClock(struct _tm *tm) NUT_DEPRECATED;
extern int X12RtcSetClock(CONST struct _tm *tm) NUT_DEPRECATED;
extern int X12RtcGetAlarm(int idx, struct _tm *tm, int *aflgs) NUT_DEPRECATED;
extern int X12RtcSetAlarm(int idx, CONST struct _tm *tm, int aflgs) NUT_DEPRECATED;
extern int X12RtcGetStatus(uint32_t *sflgs) NUT_DEPRECATED;
extern int X12RtcClearStatus(uint32_t sflgs) NUT_DEPRECATED;
extern int X12RtcReadRegs(uint8_t addr, uint8_t *buff, size_t len);
extern int X12RtcWrite(int nv, CONST uint8_t *buff, size_t len);

extern int X12EepromRead(unsigned int addr, void *buff, size_t len);
extern int X12EepromWrite(unsigned int addr, CONST void *buff, size_t len);

__END_DECLS
/* End of prototypes */
#endif
