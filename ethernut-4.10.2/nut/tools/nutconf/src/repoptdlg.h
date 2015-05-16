#ifndef REPOPTDLG_H_
#define REPOPTDLG_H_

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
 * $Id: repoptdlg.h 3878 2012-02-15 15:46:22Z haraldkipp $
 */

#include <wx/wx.h>
#include <wx/config.h>


class CRepositoryOptionsDialog: public wxPanel
{
DECLARE_CLASS(CRepositoryOptionsDialog)
public:
    CRepositoryOptionsDialog(wxWindow* parent);
    virtual bool TransferDataToWindow();
    virtual bool TransferDataFromWindow();
private:
    void OnBrowseRepositoryFile(wxCommandEvent& event);

    wxString m_repositoryPath;
    wxTextCtrl *m_entryPath;
    wxCheckBox *m_chkBoxConfig;


    DECLARE_EVENT_TABLE()

};

#endif
