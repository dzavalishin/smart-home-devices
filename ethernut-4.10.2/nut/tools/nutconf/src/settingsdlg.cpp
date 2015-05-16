/* ----------------------------------------------------------------------------
 * Copyright (C) 2004-2007 by egnite Software GmbH
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
 * $Id: settingsdlg.cpp 3878 2012-02-15 15:46:22Z haraldkipp $
 */

#include <wx/notebook.h>

#include "ids.h"
#include "nutconf.h"
#include "settingsdlg.h"

IMPLEMENT_CLASS(CSettingsDialog, wxDialog)

CSettingsDialog::CSettingsDialog(wxWindow* parent)
: wxDialog(parent, -1, wxGetApp().GetAppName() + wxT(" Settings"), wxDefaultPosition, 
           wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
    m_notebook = new wxNotebook(this, ID_SETTINGS_NOTEBOOK);

    m_repositoryOptions = new CRepositoryOptionsDialog(m_notebook);
    m_buildOptions = new CBuildOptionsDialog(m_notebook);
    m_toolOptions = new CToolOptionsDialog(m_notebook);
    m_appOptions = new CAppOptionsDialog(m_notebook);

    m_notebook->AddPage(m_repositoryOptions, wxT("Repository"));
    m_notebook->AddPage(m_buildOptions, wxT("Build"));
    m_notebook->AddPage(m_toolOptions, wxT("Tools"));
    m_notebook->AddPage(m_appOptions, wxT("Samples"));

    wxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);

    sizerTop->Add(m_notebook, 1, wxGROW | wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT | wxTOP, 5);

    wxSizer *sizerBot = new wxBoxSizer(wxHORIZONTAL);

    wxButton *okButton = new wxButton(this, wxID_OK, wxT("&OK"), wxDefaultPosition, wxDefaultSize, 0);
    wxButton *cancelButton = new wxButton( this, wxID_CANCEL, wxT("&Cancel"), wxDefaultPosition, wxDefaultSize, 0);
    sizerBot->Add(okButton, 0, wxALIGN_CENTRE | wxALL, 5);
    sizerBot->Add( cancelButton, 0, wxALIGN_CENTRE | wxALL, 5);

    sizerTop->Add(4, 4, 0, wxALIGN_CENTRE|wxALL, 0);
    sizerTop->Add(sizerBot, 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL | wxALL, 0);

    /*
     * Restore frame position and size.
     */
    wxConfigBase *pConfig = wxConfigBase::Get();
    if(pConfig) {
        wxString lastPath = pConfig->GetPath();
        pConfig->SetPath(wxT("/SettingsDlg"));
        SetSize(pConfig->Read(wxT("x"), 50), pConfig->Read(wxT("y"), 50),
                pConfig->Read(wxT("w"), 400), pConfig->Read(wxT("h"), 460));
        m_notebook->SetSelection((long)pConfig->Read(wxT("page"), (long)0));
        pConfig->SetPath(lastPath);
    }

    this->SetAutoLayout(true);
    this->SetSizer(sizerTop);

    okButton->SetDefault();
    okButton->SetFocus();

    Layout();

    m_repositoryOptions->Layout();

    Centre(wxBOTH);
}

/*!
 * \brief Destructor.
 */
CSettingsDialog::~CSettingsDialog()
{
    // Save frame size and position.
    wxConfigBase *pConfig = wxConfigBase::Get();
    if (pConfig) {
        wxString lastPath = pConfig->GetPath();
        pConfig->SetPath(wxT("/SettingsDlg"));

        /*
         * Dialog frame window position and client window size.
         */
        int x, y;
        int w, h;
        GetPosition(&x, &y);
        pConfig->Write(wxT("x"), (long) x);
        pConfig->Write(wxT("y"), (long) y);
        GetSize(&w, &h);
        pConfig->Write(wxT("w"), (long) w);
        pConfig->Write(wxT("h"), (long) h);

        pConfig->Write(wxT("page"), m_notebook->GetSelection());

        pConfig->SetPath(lastPath);
    }
}

bool CSettingsDialog::TransferDataToWindow()
{
    bool rc = m_repositoryOptions->TransferDataToWindow();
    rc = rc && m_buildOptions->TransferDataToWindow();
    rc = rc && m_toolOptions->TransferDataToWindow();
    rc = rc && m_appOptions->TransferDataToWindow();

    return rc;
}

bool CSettingsDialog::TransferDataFromWindow()
{
    bool rc = m_repositoryOptions->TransferDataFromWindow();
    rc = rc && m_buildOptions->TransferDataFromWindow();
    rc = rc && m_toolOptions->TransferDataFromWindow();
    rc = rc && m_appOptions->TransferDataFromWindow();

    wxGetApp().GetSettings()->Save(wxGetApp().GetSettings()->m_configname);

    return rc;
}

bool CSettingsDialog::Validate()
{
    m_notebook->SetSelection(0);
    if (!m_repositoryOptions->Validate()) {
        return false;
    }
    m_notebook->SetSelection(1);
    if (!m_buildOptions->Validate()) {
        return false;
    }
    m_notebook->SetSelection(2);
    if (!m_toolOptions->Validate()) {
        return false;
    }
    m_notebook->SetSelection(3);
    if (!m_appOptions->Validate()) {
        return false;
    }
    return true;
}
