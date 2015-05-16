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
 * $Id: repoptdlg.cpp 3878 2012-02-15 15:46:22Z haraldkipp $
 */

#include <wx/valgen.h>
#include <wx/filename.h>

#include "ids.h"
#include "nutconf.h"
#include "pathvalidator.h"
#include "repoptdlg.h"

IMPLEMENT_CLASS(CRepositoryOptionsDialog, wxPanel)

BEGIN_EVENT_TABLE(CRepositoryOptionsDialog, wxPanel)
    EVT_BUTTON(ID_BROWSE_REPOPATH, CRepositoryOptionsDialog::OnBrowseRepositoryFile)
END_EVENT_TABLE()

CRepositoryOptionsDialog::CRepositoryOptionsDialog(wxWindow* parent)
: wxPanel(parent, ID_SETTINGS_REPOSITORY)
{
    CSettings *opts = wxGetApp().GetSettings();
    CPathValidator repoValid(VALIDPATH_IS_PLAIN_FILE | VALIDPATH_EXISTS | VALIDPATH_SHOW_NATIVE | VALIDPATH_TO_UNIX, &opts->m_repositoryname);

    wxStaticBox *groupPath = new wxStaticBox(this, -1, wxT("Repository File"));
    m_entryPath = new wxTextCtrl(this, ID_PATH_ENTRY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, repoValid);
    wxButton *btnBrowse = new wxButton(this, ID_BROWSE_REPOPATH, wxT("&Browse..."));

    wxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);
    
    wxSizer *sizerGroup = new wxStaticBoxSizer(groupPath, wxVERTICAL);

    sizerGroup->Add(m_entryPath, 1, wxALIGN_LEFT | wxGROW | wxALL, 5);
    sizerGroup->Add(btnBrowse, 0, wxALIGN_RIGHT | wxALL, 5);
    sizerTop->Add(sizerGroup, 0, wxGROW | wxALIGN_CENTRE | wxALL, 5);

    wxStaticBox *groupConfig = new wxStaticBox(this, -1, wxT("Configurations"));
    m_chkBoxConfig = new wxCheckBox(this, -1, wxT("Enable multiple configurations"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&opts->m_mulConfig));

    wxSizer *sizerConfig = new wxStaticBoxSizer(groupConfig, wxHORIZONTAL);
    sizerConfig->Add(m_chkBoxConfig, 1, wxALIGN_LEFT | wxGROW | wxALL, 5);

    sizerTop->Add(sizerConfig, 0, wxGROW | wxALIGN_CENTRE | wxALL, 5);

    SetAutoLayout(true);
    SetSizer(sizerTop);
}

bool CRepositoryOptionsDialog::TransferDataToWindow()
{
    return wxPanel::TransferDataToWindow();
}

bool CRepositoryOptionsDialog::TransferDataFromWindow()
{
    return wxPanel::TransferDataFromWindow();
}

void CRepositoryOptionsDialog::OnBrowseRepositoryFile(wxCommandEvent& WXUNUSED(event))
{
    wxFileName fname(m_entryPath->GetValue());

    wxFileDialog dlg(this, wxT("Choose a repository file"), fname.GetPath(), fname.GetFullName(), 
        wxT("Nut/OS Repository (*.nut)|*.nut"), wxFD_OPEN);

    if (dlg.ShowModal() == wxID_OK) {
        m_repositoryPath = dlg.GetPath();
        m_repositoryPath.Replace(wxString(wxFileName::GetPathSeparator()), wxT("/"));
        m_entryPath->SetValue(m_repositoryPath);
    }
}

