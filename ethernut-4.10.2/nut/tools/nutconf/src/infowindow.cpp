/* ----------------------------------------------------------------------------
 * Copyright (C) 2004 by egnite Software GmbH
 * Copyright (C) 1998, 1999, 2000 Red Hat, Inc.
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
 * ----------------------------------------------------------------------------
 */

/*
 * $Id: infowindow.cpp 3878 2012-02-15 15:46:22Z haraldkipp $
 */

#include "ids.h"
#include "nutconf.h"
#include "infowindow.h"

IMPLEMENT_CLASS(CInfoWindow, wxTextCtrl)

    BEGIN_EVENT_TABLE(CInfoWindow, wxTextCtrl)
    EVT_MOUSE_EVENTS(CInfoWindow::OnMouseEvent)
    END_EVENT_TABLE();

CInfoWindow::CInfoWindow(wxWindow * parent, wxWindowID id, const wxPoint & pt, const wxSize & sz, long style)
: wxTextCtrl(parent, id, wxEmptyString, pt, sz, style)
{
    m_propertiesMenu = new wxMenu;
    m_propertiesMenu->Append(ID_WHATS_THIS, wxT("&What's This?"));
}

CInfoWindow::~CInfoWindow()
{
    delete m_propertiesMenu;
}

void CInfoWindow::OnMouseEvent(wxMouseEvent & event)
{
    if (event.RightDown()) {
        PopupMenu(GetPropertiesMenu(), event.GetX(), event.GetY());
    } else {
        event.Skip();
    }
}

wxMenu *CInfoWindow::GetPropertiesMenu() const
{
    return m_propertiesMenu;
}
