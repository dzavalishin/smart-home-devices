/*
 * Copyright 2011 by egnite GmbH
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

#include <QSysInfo>
#include <QString>

#include "systeminfo.h"

SystemInfo::SystemInfo()
{
}

QString SystemInfo::GetOsString()
{
#if defined(Q_OS_LINUX)
    return "Linux";
#else
#if defined(Q_OS_MAC)
    switch(QSysInfo::MacintoshVersion) {
    case QSysInfo::MV_10_0:
        return "Mac OS X 10.0 \"Cheetah\"";
    case QSysInfo::MV_10_1:
        return "Mac OS X 10.1 \"Puma\"";
    case QSysInfo::MV_10_2:
        return "Mac OS X 10.2 \"Jaguar\"";
    case QSysInfo::MV_10_3:
        return "Mac OS X 10.3 \"Panther\"";
    case QSysInfo::MV_10_4:
        return "Mac OS X 10.4 \"Tiger\"";
    case QSysInfo::MV_10_5:
        return "Mac OS X 10.5 \"Leopard\"";
    case QSysInfo::MV_10_6:
        return "Mac OS X 10.6 \"Snow Leopard\"";
    case QSysInfo::MV_10_6 + 1:
        return "Mac OS X 10.7 \"Lion\"";
    default:
        return "Unknown Mac OS Version";
    }
#elif defined(Q_WS_WIN)
    switch(QSysInfo::WindowsVersion) {
    case QSysInfo::WV_NT:
        return "Windows NT";
    case QSysInfo::WV_2000:
        return "Windows 2000";
    case QSysInfo::WV_XP:
        return "Windows XP";
    case QSysInfo::WV_2003:
        return "Windows Server 2003";
    case QSysInfo::WV_VISTA:
        return "Windows Vista";
    case QSysInfo::WV_WINDOWS7:
        return "Windows 7";
    default:
        return "Unknown Windows OS";
    }
#endif
    return "unknown OS";
#endif
}
