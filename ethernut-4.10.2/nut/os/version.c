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
 * Revision 1.67  2009/03/09 08:28:03  haraldkipp
 * Beta 4.9
 *
 * Revision 1.66  2009/03/09 08:22:36  haraldkipp
 * Release branch 4.8 created
 *
 * Revision 1.65  2009/03/09 08:15:14  haraldkipp
 * Release branch 4.8 announced
 *
 * Revision 1.64  2009/01/09 17:54:14  haraldkipp
 * Next beta will be 4.7.5.
 *
 * Revision 1.63  2008/10/10 11:52:25  haraldkipp
 * Lua added.
 *
 * Revision 1.62  2008/08/29 15:03:04  haraldkipp
 * Fixed Configurator bug #2082123. Options now correctly enabled.
 *
 * Revision 1.61  2008/08/28 16:11:31  haraldkipp
 * Fixed bitbanging SPI for ARM targets.
 *
 * Revision 1.60  2008/08/25 17:05:27  haraldkipp
 * Beta release 4.7.1.
 *
 * Revision 1.59  2008/08/11 07:00:34  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.58  2008/07/29 13:06:21  haraldkipp
 * Starting beta 4.7
 *
 * Revision 1.57  2008/07/29 13:04:18  haraldkipp
 * Release branch 4.6 created
 *
 * Revision 1.56  2008/07/29 12:13:38  haraldkipp
 * Release branch 4.6 announced
 *
 * Revision 1.55  2008/07/26 09:39:16  haraldkipp
 * Preparing 4.5.7 beta.
 *
 * Revision 1.54  2008/07/24 17:06:07  haraldkipp
 * Preparing 4.5.6 beta.
 *
 * Revision 1.53  2008/07/14 13:14:38  haraldkipp
 * Second build of version 4.5.5.
 *
 * Revision 1.52  2008/07/10 12:17:41  haraldkipp
 * Preparing beta version 4.5.5.
 *
 * Revision 1.51  2008/04/18 13:12:31  haraldkipp
 * ICCAVR fixes, many shorts to int, PPP+RS485 for AT91.
 *
 * Revision 1.50  2008/04/02 09:39:28  haraldkipp
 * Fixed another PHAT file pointer bug.
 *
 * Revision 1.49  2008/04/01 10:09:55  haraldkipp
 * Beta version 4.5.2.
 *
 * Revision 1.48  2008/02/15 16:50:33  haraldkipp
 * Beta release 4.5.1.
 *
 * Revision 1.47  2007/09/11 15:40:45  haraldkipp
 * Release branch 4.4 created
 *
 * Revision 1.46  2007/08/30 12:15:06  haraldkipp
 * Configurable MMC timings.
 *
 * Revision 1.45  2007/08/29 13:35:19  haraldkipp
 * Announcing release branch 4.4.
 *
 * Revision 1.44  2007/05/24 07:30:48  haraldkipp
 * Bugfixes for preparing release candidate.
 *
 * Revision 1.43  2007/05/02 11:35:25  haraldkipp
 * ATmega2561 and other fixes.
 * IGMP support started, but not finished.
 * Socket ioctl started, but not finished.
 *
 * Revision 1.42  2007/04/12 08:54:39  haraldkipp
 * Supports configurable SPI, VS10XX MP3 decoder chips and Mega256 for
 * avr-gcc 4.1.1.
 *
 * Revision 1.41  2007/02/15 15:59:12  haraldkipp
 * First 4.3 Beta Release
 *
 * Revision 1.40  2006/10/17 11:26:02  haraldkipp
 * Release branch 4.2 created.
 *
 * Revision 1.39  2006/10/05 17:26:58  haraldkipp
 * Follows unofficial rc10.
 *
 * Revision 1.38  2006/09/29 12:19:34  haraldkipp
 * RC10
 *
 * Revision 1.37  2006/08/31 19:19:55  haraldkipp
 * No time to write comments. ;-)
 *
 * Revision 1.36  2006/08/01 07:33:02  haraldkipp
 * Preparing new release candidate.
 *
 * Revision 1.35  2006/07/26 11:14:52  haraldkipp
 * Release candidate 6
 *
 * Revision 1.34  2006/07/10 14:29:03  haraldkipp
 * Release candidate 4.1.9.4.
 * C++ support added. These patches had been kindly contributed by
 * Matthias Wilde.
 *
 * Revision 1.33  2006/07/05 08:03:54  haraldkipp
 * Second trial to move to 4.2.
 *
 * Revision 1.32  2006/06/28 14:43:06  haraldkipp
 * Release candidate prepared.
 *
 * Revision 1.31  2006/05/15 12:48:49  haraldkipp
 * Prepare pre-release 4.1.6.
 *
 * Revision 1.30  2006/04/07 13:59:55  haraldkipp
 * Pre-release 4.1.5.4.
 *
 * Revision 1.29  2006/02/08 15:20:56  haraldkipp
 * ATmega2561 Support
 *
 * Revision 1.28  2006/01/23 19:53:33  haraldkipp
 * Minor fixes
 *
 * Revision 1.27  2006/01/23 17:37:08  haraldkipp
 * Mostly memory alignment fixes.
 *
 * Revision 1.26  2006/01/22 17:46:18  haraldkipp
 * Pre-release 4.1.3
 *
 * Revision 1.25  2006/01/19 18:39:33  haraldkipp
 * Pre-release 4.1.2.
 *
 * Revision 1.24  2006/01/08 11:55:52  haraldkipp
 * Prepare preview snapshot.
 *
 * Revision 1.23  2005/12/04 18:42:21  haraldkipp
 * Release branch X.Y created.
 *
 * Revision 1.22  2005/12/02 11:36:12  haraldkipp
 * Release 4.0 scheduled
 *
 * Revision 1.21  2005/11/20 14:36:07  haraldkipp
 * Preparing last version 4 preview.
 *
 * Revision 1.20  2005/08/02 17:47:04  haraldkipp
 * Major API documentation update.
 *
 * Revision 1.19  2005/07/20 09:14:40  haraldkipp
 * Release 3.9.8
 *
 * Revision 1.18  2005/06/06 10:40:31  haraldkipp
 * Emergency bugfix release 3.9.7.1.
 *
 * Revision 1.17  2005/05/25 10:04:58  haraldkipp
 * Win 32 release.
 *
 * Revision 1.16  2005/05/24 16:12:51  haraldkipp
 * Sigh!
 *
 * Revision 1.15  2005/05/22 16:05:40  haraldkipp
 * *** empty log message ***
 *
 * Revision 1.14  2005/04/05 17:37:12  haraldkipp
 * First Nut/OS running ARM responds to http requests
 *
 * Revision 1.13  2005/02/07 15:24:46  haraldkipp
 * ARP cache bugfix.
 *
 * Revision 1.12  2005/02/06 16:35:33  haraldkipp
 * *** empty log message ***
 *
 * Revision 1.11  2005/01/02 10:07:10  haraldkipp
 * Replaced platform dependant formats in debug outputs.
 *
 * Revision 1.10  2004/10/03 18:44:40  haraldkipp
 * A little bit of Game Boy
 *
 * Revision 1.9  2004/09/08 09:55:19  haraldkipp
 * Although this is no final release, there is a lot of new hardware waiting
 * for Nut/OS support. So I marked this with version 3.9.1 before creating
 * the long awaited 4.0 branch.
 *
 * Revision 1.8  2004/01/30 11:50:26  haraldkipp
 * Release branch 3.4 created
 *
 * Revision 1.7  2004/01/27 13:41:52  haraldkipp
 * Branch 3.4 announced
 *
 * Revision 1.6  2003/12/15 19:32:32  haraldkipp
 * Preview
 *
 * Revision 1.5  2003/11/04 17:59:04  haraldkipp
 * Corrected
 *
 * Revision 1.4  2003/11/03 16:28:20  haraldkipp
 * Release version 3.3.2
 *
 * Revision 1.3  2003/08/14 15:22:24  haraldkipp
 * New release
 *
 * Revision 1.2  2003/07/21 18:26:31  haraldkipp
 * Version 3.3
 *
 * Revision 1.1.1.1  2003/05/09 14:41:55  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.3  2003/05/06 18:58:16  harald
 * Prepare final release
 *
 * Revision 1.2  2003/03/31 14:40:57  harald
 * *** empty log message ***
 *
 * Revision 1.1  2003/02/04 18:17:07  harald
 * Version 3 released
 *
 */

#include <sys/version.h>

#if defined(__GNUC__)
#define NUM2STR(x)  #x
#define OS_VERSION_STRING(a, b, c, d)   NUM2STR(a) "." NUM2STR(b) "." NUM2STR(c) "." NUM2STR(d)
static CONST char *os_version_string = 
    OS_VERSION_STRING(NUT_VERSION_MAJOR, NUT_VERSION_MINOR, NUT_VERSION_RELEASE, NUT_VERSION_BUILD);
#else
static CONST char os_version_string[] = {
#if NUT_VERSION_MAJOR >= 10
    (NUT_VERSION_MAJOR / 10) + '0',
#endif
    (NUT_VERSION_MAJOR % 10) + '0', '.',
#if NUT_VERSION_MINOR >= 10
    (NUT_VERSION_MINOR / 10) + '0',
#endif
    (NUT_VERSION_MINOR % 10) + '0', '.',
#if NUT_VERSION_RELEASE >= 10
    (NUT_VERSION_RELEASE / 10) + '0',
#endif
    (NUT_VERSION_RELEASE % 10) + '0', '.',
#if NUT_VERSION_BUILD >= 10
    (NUT_VERSION_BUILD / 10) + '0',
#endif
    (NUT_VERSION_BUILD % 10) + '0', 0
};
#endif

/*!
 * \addtogroup xgNutVersion
 */
/*@{*/


/*!
 * \brief Return Nut/OS version.
 *
 * \return Version coded into an unsigned long. Most significant byte
 *         is the major version number, followed by the minor version
 *         number, again followed by the release number and finally
 *         the build number in the least significant byte.
 */
uint32_t NutVersion(void)
{
    return OS_VERSION_NUMBER;
}

/*!
 * \brief Return Nut/OS version string.
 *
 * \return Pointer to a printable string containing the version number.
 *         The string contains 4 numbers separated by dots. The first
 *         is the major version number, followed by the minor version
 *         number, again followed by the release number and finally
 *         followed by the build number. In non-official releases this 
 *         may be optionally followed by a space and an additional 
 *         release state.
 */
CONST char *NutVersionString(void)
{
    return os_version_string;
}

/*@}*/
