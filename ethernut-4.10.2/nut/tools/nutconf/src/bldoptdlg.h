#ifndef BLDOPTDLG_H_
#define BLDOPTDLG_H_

/* ----------------------------------------------------------------------------
 * Copyright (C) 2008-2012 by egnite GmbH
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
 * $Id: bldoptdlg.h 3878 2012-02-15 15:46:22Z haraldkipp $
 */

#include <wx/wx.h>
#include <wx/config.h>

/*!
 * \brief Settings dialog for build options.
 */
class CBuildOptionsDialog: public wxPanel
{
DECLARE_CLASS(CBuildOptionsDialog)
public:
    CBuildOptionsDialog(wxWindow* parent);
    virtual bool TransferDataToWindow();
    virtual bool TransferDataFromWindow();
private:
    /*! \brief Combo box control used to select the compiler platform.
    */
    wxComboBox *m_cbxPlatform;

    /*! \brief Text control used to specify the source directory path.
    */
    wxTextCtrl *m_entSourceDir;

    /*! \brief Text control used to specify the include directory path that is searched first.
    */
    wxTextCtrl *m_entInclFirstDir;

    /*! \brief Text control used to specify the include directory path that is searched last.
    */
    wxTextCtrl *m_entInclLastDir;

    /*! \brief Text control used to specify the build directory path.
    */
    wxTextCtrl *m_entBuildDir;

    /*! \brief Text control used to specify the install directory path.
    */
    wxTextCtrl *m_entInstallDir;

    /*! \brief The source directory path scanned last time.
    */
    wxString m_lastSourceDir;

    void OnBrowseBuildPath(wxCommandEvent& event);
    void OnBrowseInstallPath(wxCommandEvent& event);
    void OnBrowseSourceDir(wxCommandEvent& event);
	void OnBrowseIncludeFirst(wxCommandEvent& event);
	void OnBrowseIncludeLast(wxCommandEvent& event);
	void OnPlatformEnter(wxCommandEvent& event);
    void OnSourceDirChange(wxCommandEvent& event);
    void PopulatePlatform();

    DECLARE_EVENT_TABLE()

};

#endif
