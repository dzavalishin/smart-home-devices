/* ----------------------------------------------------------------------------
 * Copyright (C) 2004-2005 by egnite Software GmbH
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

#include <wx/wx.h>

#include "nutconf.h"
#include "nutconfdoc.h"
#include "finddlg.h"

#include <wx/app.h>

BEGIN_EVENT_TABLE(CFindDialog, wxFindReplaceDialog)
    EVT_FIND(wxID_ANY, CFindDialog::OnFind)
    EVT_FIND_NEXT(wxID_ANY, CFindDialog::OnFind)
    EVT_FIND_CLOSE(wxID_ANY, CFindDialog::OnClose)
END_EVENT_TABLE()

wxFindReplaceData CFindDialog::m_findData;

CFindDialog::CFindDialog(wxWindow *parent, const wxString& title, long style)
        : wxFindReplaceDialog(parent, &m_findData, title, style)
{
    wxConfigBase *pConfig = wxConfigBase::Get();
    Move(pConfig->Read(wxT("/FindDialog/x"), 50), pConfig->Read(wxT("/FindDialog/y"), 50));
}

void CFindDialog::OnFind(wxFindDialogEvent& event)
{
    wxString string = event.GetFindString();
#if 0
    bool matchCase = ((event.GetFlags() & wxFR_MATCHCASE) != 0);
    bool matchWord = ((event.GetFlags() & wxFR_WHOLEWORD) != 0);

    /* Locate the currently selected item. */
    CConfigTree *treeCtrl = wxGetApp().GetMainFrame()->GetTreeCtrl();
    wxTreeItemId sel = treeCtrl->GetSelection();
    if (!sel.IsOk()) {
        sel = treeCtrl->GetRootItem();
    }

    if (sel.IsOk()) {
        sel = treeCtrl->FindNextItemId(sel, string, sel != m_lastId, matchCase, matchWord);
    }
    if (sel.IsOk()) {
        m_lastId = sel;
        treeCtrl->SelectItem(sel);
    }
    else {
        wxMessageBox(wxT("No more matches."), wxT("Search"), wxOK | wxICON_INFORMATION, this);
    }
#endif
}

void CFindDialog::OnClose(wxFindDialogEvent& event)
{
    wxConfigBase *pConfig = wxConfigBase::Get();
    if (pConfig) {
        wxString lastPath = pConfig->GetPath();
        pConfig->SetPath(wxT("/FindDialog"));

        int x, y;
        GetPosition(&x, &y);
        pConfig->Write(wxT("x"), (long) x);
        pConfig->Write(wxT("y"), (long) y);

        bool flg = ((event.GetFlags() & wxFR_MATCHCASE) != 0);
        pConfig->Write(wxT("case"), flg);
        flg = ((event.GetFlags() & wxFR_WHOLEWORD) != 0);
        pConfig->Write(wxT("word"), flg);

        pConfig->SetPath(lastPath);
    }
    this->Destroy();
    ((CMainFrame*) GetParent())->SetFindDialog(NULL);
}

