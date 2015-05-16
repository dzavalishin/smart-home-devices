#ifndef SETTINGSDLG_H_
#define SETTINGSDLG_H_

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
 * $Id: settingsdlg.h 3878 2012-02-15 15:46:22Z haraldkipp $
 */

#include <wx/wx.h>
#include <wx/config.h>
#include <wx/notebook.h>

#include "repoptdlg.h"
#include "bldoptdlg.h"
#include "tooloptdlg.h"
#include "appoptdlg.h"

class CSettingsDialog: public wxDialog
{
DECLARE_CLASS(CSettingsDialog)

public:
    CSettingsDialog(wxWindow* parent);
    virtual ~CSettingsDialog();

protected:
    wxNotebook* m_notebook;
    CRepositoryOptionsDialog* m_repositoryOptions;
    CBuildOptionsDialog* m_buildOptions;
    CToolOptionsDialog* m_toolOptions;
    CAppOptionsDialog* m_appOptions;

    virtual bool TransferDataToWindow();
    virtual bool TransferDataFromWindow();
    virtual bool Validate();

};

#endif
