/* ----------------------------------------------------------------------------
 * Copyright (C) 2009-2011 by egnite GmbH
 * Copyright (C) 2007 by egnite Software GmbH
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
#include <wx/frame.h>
#endif

#include <wx/config.h>
//new
#include <wx/menu.h>
//
#include "mainframe.h"
#include "taskbaricon.h"

BEGIN_EVENT_TABLE(CTaskBarIcon, wxTaskBarIcon)
    EVT_MENU(ID_RESTORE, CTaskBarIcon::OnMenuRestore)
    EVT_MENU(wxID_EXIT, CTaskBarIcon::OnMenuExit)
    EVT_TASKBAR_LEFT_DCLICK(CTaskBarIcon::OnLeftButtonDClick)
END_EVENT_TABLE()


#if defined(__WXCOCOA__)
CTaskBarIcon::CTaskBarIcon(wxFrame *frame, wxTaskBarIconType iconType = DEFAULT_TYPE)
    : wxTaskBarIcon(iconType)
#else
CTaskBarIcon::CTaskBarIcon(wxFrame *frame)
#endif
{
    m_frame = frame;
}

void CTaskBarIcon::OnMenuRestore(wxCommandEvent& )
{
    m_frame->Show(true);
}

void CTaskBarIcon::OnMenuExit(wxCommandEvent& )
{
    m_frame->Close(true);
}

wxMenu *CTaskBarIcon::CreatePopupMenu()
{
    wxMenu *menu = new wxMenu;

    menu->Append(ID_RESTORE, _T("&Restore"));
    menu->AppendSeparator();
#ifndef __WXMAC_OSX__ /* Mac has built-in quit menu*/
    menu->Append(wxID_EXIT, _T("E&xit"));
#endif

    return menu;
}

void CTaskBarIcon::OnLeftButtonDClick(wxTaskBarIconEvent&)
{
    m_frame->Show(true);
}

