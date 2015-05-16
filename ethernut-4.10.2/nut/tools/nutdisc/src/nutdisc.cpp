/* ----------------------------------------------------------------------------
 * Copyright (C) 2009-2011 by egnite GmbH
 * Copyright (C) 2005-2006 by egnite Software GmbH
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * ----------------------------------------------------------------------------
 */

/*
 * $Id$
 */


#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/app.h>
#include <wx/frame.h>
#include <wx/listbox.h>
#endif

#include <wx/notebook.h>
#include <wx/config.h>

#include "options.h"
#include "mainframe.h"
#include "nutdisc.h"

#if !wxUSE_THREADS
    #error "This application requires thread support!"
#endif // wxUSE_THREADS

IMPLEMENT_APP(CApp)

/*!
 * \brief Application main entry.
 */
bool CApp::OnInit()
{
#if defined(__WXMSW__)
    WORD wVersionRequested;
    WSADATA wsaData;
#endif
    int ec;

    SetVendorName(wxT("egnite"));
    SetAppName(wxT("NutDiscover"));

#if defined(__WXMSW__)
    // Initialize the socket interface.
    wVersionRequested = MAKEWORD(2, 2);
    if((ec = WSAStartup(wVersionRequested, &wsaData)) != 0) {
        return 0;
    }
    if(LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        WSACleanup();
        return 0;
    }
#endif

    if (!wxApp::OnInit())
        return false;

    g_options = new COptions();
    m_frame = new CMainFrame(wxT("Nut/OS Discoverer"));
    m_frame->Show();

    return true;
}

int CApp::OnExit()
{
    delete wxConfigBase::Set((wxConfigBase *) NULL);
    
#if defined(__WXMSW__)
    WSACleanup();
#endif
    return 0;
}
