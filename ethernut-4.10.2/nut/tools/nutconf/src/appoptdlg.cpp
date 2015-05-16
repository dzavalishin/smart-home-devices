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
 * $Id: appoptdlg.cpp 3878 2012-02-15 15:46:22Z haraldkipp $
 */

#ifdef __GNUG__
    #pragma implementation "appoptdlg.h"
#endif

#include <wx/valgen.h>
#include <wx/dir.h>

#include "ids.h"
#include "nutconf.h"
#include "pathvalidator.h"
#include "appoptdlg.h"

IMPLEMENT_CLASS(CAppOptionsDialog, wxPanel)

BEGIN_EVENT_TABLE(CAppOptionsDialog, wxPanel)
    EVT_BUTTON(ID_BROWSE_APPDIR, CAppOptionsDialog::OnBrowseAppDir)
    EVT_TEXT_ENTER(ID_COMBO_APPDIR, CAppOptionsDialog::OnProgrammerEnter)
    EVT_TEXT(ID_ENTRY_APPDIR, CAppOptionsDialog::OnAppDirChange) 
END_EVENT_TABLE()

CAppOptionsDialog::CAppOptionsDialog(wxWindow* parent)
: wxPanel(parent, ID_SETTINGS_BUILD)
{
    CSettings *opts = wxGetApp().GetSettings();
    CPathValidator appDirValid(VALIDPATH_NOT_EMPTY | VALIDPATH_IS_DIRECTORY | VALIDPATH_SHOW_NATIVE | VALIDPATH_TO_UNIX, &opts->m_app_dir);

    wxStaticBox *grpApp = new wxStaticBox(this, -1, wxT("Application Directory"));
    m_entAppDir = new wxTextCtrl(this, ID_ENTRY_APPDIR, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, appDirValid);
    wxButton *btnAppDir = new wxButton(this, ID_BROWSE_APPDIR, wxT("Browse..."), wxDefaultPosition, wxDefaultSize, 0);
    wxStaticText *lblProgrammer = new wxStaticText(this, -1, wxT("Programmer"));
    m_cbxProgrammer = new wxComboBox(this, ID_COMBO_APPDIR, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0, wxGenericValidator(&opts->m_programmer));

    wxSizer *sizerTop = new wxBoxSizer(wxVERTICAL);

    wxSizer *szrApp = new wxStaticBoxSizer(grpApp, wxVERTICAL);
    wxSizer *szrAppDir = new wxBoxSizer(wxHORIZONTAL);
    wxSizer *szrProgrammer = new wxBoxSizer(wxHORIZONTAL);

    szrAppDir->Add(m_entAppDir, 1, wxALIGN_LEFT | wxGROW | wxALL, 5);
    szrAppDir->Add(btnAppDir, 0, wxALIGN_RIGHT | wxGROW | wxALL, 5);

    szrProgrammer->Add(lblProgrammer, 0, wxALIGN_LEFT | wxGROW | wxALL, 5);
    szrProgrammer->Add(m_cbxProgrammer, 0, wxALIGN_LEFT | wxGROW | wxALL, 5);

    szrApp->Add(szrAppDir, 0, wxGROW | wxALL, 5);
    szrApp->Add(szrProgrammer, 0, wxGROW | wxALL, 5);

    sizerTop->Add(szrApp, 0, wxGROW | wxALIGN_CENTRE | wxALL, 5);

    SetAutoLayout(true);
    SetSizer(sizerTop);

    PopulateProgrammer();
}

/*! 
 * \brief Transfers values to child controls from data areas specified by their validators. 
 *
 * \return false if a transfer failed.
 */
bool CAppOptionsDialog::TransferDataToWindow()
{
    return wxPanel::TransferDataToWindow();
}

/*!
 * \brief Transfers values from child controls to data areas specified by their validators. 
 *
 * \return false if a transfer failed.
 */
bool CAppOptionsDialog::TransferDataFromWindow()
{
    return wxPanel::TransferDataFromWindow();
}

/*! 
 * \brief Executed when user clicks the browse button. 
 *
 * \param event Contains information about the command event.
 */
void CAppOptionsDialog::OnBrowseAppDir(wxCommandEvent& WXUNUSED(event))
{
    wxString path = m_entAppDir->GetValue();

    wxDirDialog dlg(this, wxT("Choose a source directory"), path);

    if (dlg.ShowModal() == wxID_OK) {
        wxString val = dlg.GetPath();
#ifdef _WIN32
        val.Replace(wxT("\\"), wxT("/"));
#endif
        m_entAppDir->SetValue(val);
    }
}

/*!
 * \brief Executed when user presses ENTER in the programmer selection combo.
 *
 * \todo This routine doesn't do anything. Shall we remove it?
 *
 * \param event Contains information about the command event.
 */
void CAppOptionsDialog::OnProgrammerEnter(wxCommandEvent& WXUNUSED(event))
{
}

/*!
 * \brief Fills the programmer selection combo box.
 *
 * Scans subdirectory 'app' within the source tree for files with
 * a base name of 'Makeburn'. The extensions of all files found are 
 * added to the combo box.
 */
void CAppOptionsDialog::PopulateProgrammer()
{
    CSettings *opts = wxGetApp().GetSettings();
    wxString src_dir = opts->m_source_dir + wxT("/app");
    wxString programmer = m_cbxProgrammer->GetValue();

    if(wxDir::Exists(src_dir)) {
        wxDir dir(src_dir);
        if(dir.IsOpened()) {
            wxString entry;

            m_cbxProgrammer->Clear();
            bool cont = dir.GetFirst(&entry, wxT("Makeburn.*"));
            while (cont) {
                m_cbxProgrammer->Append(entry.AfterLast('.'));
                cont = dir.GetNext(&entry);
            }
            if(!programmer.IsEmpty()) {
                m_cbxProgrammer->SetValue(programmer);
            }
        }
    }
}

/*! 
 * \brief Handle sample directory changes.
 * 
 * Executed when text in the sample directory entry field changes. 
 *
 * \param event Contains information about the command event.
 */
void CAppOptionsDialog::OnAppDirChange(wxCommandEvent& WXUNUSED(event))
{
    PopulateProgrammer();
}
