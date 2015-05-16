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

#include <wx/config.h>

#ifdef __WXMSW__
#pragma warning(disable:4996)
#endif

#include "setup.h"
#include "propdialog.h"

BEGIN_EVENT_TABLE(CPropDialog, wxDialog)
    EVT_BUTTON(wxID_OK, CPropDialog::OnOk)
    EVT_BUTTON(wxID_CANCEL, CPropDialog::OnCancel)
    EVT_CLOSE(CPropDialog::OnClose)
END_EVENT_TABLE()

CPropDialog::CPropDialog(const wxString& title, DISCOVERY_TELE *dist)
    : wxDialog(NULL, -1, title, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
    , m_dist(*dist)
{
    wxBoxSizer* topSizer;

    topSizer = new wxBoxSizer(wxVERTICAL);

    m_grid = new wxPropertyGrid(this, 9991, wxDefaultPosition, wxDefaultSize,
        wxPG_BOLD_MODIFIED | wxPG_SPLITTER_AUTO_CENTER | wxPG_TOOLTIPS |
        wxTAB_TRAVERSAL | wxSUNKEN_BORDER | wxPG_DEFAULT_STYLE);

    topSizer->Add(m_grid, 1, wxEXPAND | wxALL);
    topSizer->Add(CreateButtonSizer(wxOK | wxCANCEL));
    m_grid->Append(new wxPropertyCategory(wxT("General")));
    m_grid->Append(new wxStringProperty(wxT("MAC"), wxT("ics_mac"), CSetup::MacToString(dist->dist_mac)));

    char *hostname;
    if (dist->dist_ver == DISCOVERY_VERSION_1_0) {
        hostname = strdup((char *)dist->dist_appendix);
    } else {
        size_t nlen = dist->dist_appendix[0];
        hostname = (char *)malloc(nlen + 1);
        memcpy(hostname, &dist->dist_appendix[1], nlen);
        hostname[nlen] = '\0';
    }
    m_grid->Append(new wxStringProperty(wxT("Host Name"), wxT("ics_hostname"), wxString(hostname, wxConvLocal)));

    m_grid->Append(new wxPropertyCategory(wxT("IP")));
    m_grid->Append(new wxStringProperty(wxT("Fixed Address"), wxT("ics_cip_addr"), CSetup::IpToString(dist->dist_cip_addr)));
    m_grid->Append(new wxStringProperty(wxT("Mask"), wxT("ics_ip_mask"), CSetup::IpToString(dist->dist_ip_mask)));
    m_grid->Append(new wxStringProperty(wxT("Gateway"), wxT("ics_gateway"), CSetup::IpToString(dist->dist_gateway)));

    SetSizer(topSizer);
    topSizer->SetSizeHints(this);

    /*
     * Restore frame position and size.
     */
    wxConfigBase *pConfig = wxConfigBase::Get();
    if(pConfig) {
        pConfig->SetPath(wxT("/PropDialog"));
        Move(pConfig->Read(wxT("x"), 50), pConfig->Read(wxT("y"), 50));
        SetClientSize(pConfig->Read(wxT("w"), 350), pConfig->Read(wxT("h"), 200));
    }
    else {
        Centre();
    }
}

CPropDialog::~CPropDialog()
{
    /*
     * Save frame size and position.
     */
    wxConfigBase *pConfig = wxConfigBase::Get();
    if (pConfig) {
        int x, y;
        GetPosition(&x, &y);
        pConfig->Write(wxT("/PropDialog/x"), (long) x);
        pConfig->Write(wxT("/PropDialog/y"), (long) y);
        int w, h;
        GetClientSize(&w, &h);
        pConfig->Write(wxT("/PropDialog/w"), (long) w);
        pConfig->Write(wxT("/PropDialog/h"), (long) h);
    }
}

bool CPropDialog::GetValues()
{
    wxString val = m_grid->GetPropertyValueAsString("ics_mac");
    if (!CSetup::StringToMac(val, m_dist.dist_mac)) {
        ::wxMessageBox(wxT("Bad MAC address!"), wxT("Error"), wxOK | wxICON_ERROR, this);
        return false;
    }

    val = m_grid->GetPropertyValueAsString("ics_hostname");
    if (m_dist.dist_ver == DISCOVERY_VERSION_1_0) {
        if (val.Len() > 8) {
            ::wxMessageBox(wxT("Host Name too long!"), wxT("Error"), wxOK | wxICON_ERROR, this);
            return false;
        }
        strncpy((char *)m_dist.dist_appendix, val, 7);
        m_dist.dist_appendix[7] = 0;
    } else {
        m_dist.dist_appendix[0] = (u_char)val.Len();
        strncpy((char *)&m_dist.dist_appendix[1], val, val.Len());
    }

    val = m_grid->GetPropertyValueAsString("ics_cip_addr");
    if (!CSetup::StringToIp(val, &m_dist.dist_cip_addr)) {
        wxMessageBox(wxT("Bad IP address!"), wxT("Error"), wxOK | wxICON_ERROR, this);
        return false;
    }

    val = m_grid->GetPropertyValueAsString("ics_ip_mask");
    if (!CSetup::StringToIp(val, &m_dist.dist_ip_mask)) {
        wxMessageBox(wxT("Bad IP mask!"), wxT("Error"), wxOK | wxICON_ERROR, this);
        return false;
    }

    val = m_grid->GetPropertyValueAsString("ics_gateway");
    if (!CSetup::StringToIp(val, &m_dist.dist_gateway)) {
        wxMessageBox(wxT("Bad gateway address!"), wxT("Error"), wxOK | wxICON_ERROR, this);
        return false;
    }
    return true;
}

void CPropDialog::GetSetup(DISCOVERY_TELE *dist)
{
    *dist = m_dist;
}

void CPropDialog::OnOk(wxCommandEvent &event)
{
    if(GetValues()) {
        event.Skip();
    }
}

void CPropDialog::OnCancel(wxCommandEvent &event)
{
    event.Skip();
}

void CPropDialog::OnClose(wxCloseEvent& event)
{
    if (event.CanVeto()) {
        wxMessageBox(_T("Use the menu item to close this dialog"),
                     _T("Modeless dialog"), wxOK | wxICON_INFORMATION, this);
        event.Veto();
    }
}
