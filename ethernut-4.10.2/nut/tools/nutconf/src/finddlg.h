#ifndef FINDDLG_H_
#define FINDDLG_H_

/* ----------------------------------------------------------------------------
 * Copyright (C) 2006 by egnite Software GmbH
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
 * $Id: finddlg.h 3878 2012-02-15 15:46:22Z haraldkipp $
 */

#include <wx/wx.h>
#include <wx/config.h>
#include <wx/fdrepdlg.h>

class CFindDialog: public wxFindReplaceDialog
{
public:
    CFindDialog(wxWindow* parent, const wxString& title, long style = 0);

    void OnFind(wxFindDialogEvent& event);
    void OnClose(wxFindDialogEvent& event);

    static wxFindReplaceData m_findData;
    static wxString m_lastItem;
    wxTreeItemId m_lastId;

private:
    DECLARE_EVENT_TABLE()
};

#endif
