/* ----------------------------------------------------------------------------
 * Copyright (C) 2007 by egnite Software GmbH
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
 * $Id: pathvalidator.cpp 3878 2012-02-15 15:46:22Z haraldkipp $
 */

#include <wx/tokenzr.h>
#include <wx/filename.h>
#include "pathvalidator.h"

IMPLEMENT_DYNAMIC_CLASS(CPathValidator, wxValidator)

CPathValidator::CPathValidator(long style, wxString *value)
{
    m_style = style;
    m_value = value;
}

CPathValidator::CPathValidator(const CPathValidator& src)
{
    Copy(src);
}

wxObject* CPathValidator::Clone() const
{
    return new CPathValidator(*this);
}

bool CPathValidator::Validate(wxWindow* parent)
{
    bool valid = true;
    wxString errormsg;

    wxTextCtrl *control = (wxTextCtrl *) m_validatorWindow;

    wxString str(control->GetValue());

    if (str.IsEmpty()) {
        if (m_style & VALIDPATH_NOT_EMPTY) {
            valid = false;
            errormsg = _("Empty field is not allowed.");
        }
    }
    else {
        wxArrayString list;

        if (m_style & VALIDPATH_LIST) {
            wxStringTokenizer tokenize(str, wxT(";"));

            while (tokenize.HasMoreTokens()) {
                list.Add(tokenize.GetNextToken());
            }
        }
        else {
            list.Add(str);
        }

        for (size_t i = 0; valid && i < list.Count(); i++) {
            bool dir_exists = ::wxDirExists(list[i]);
            bool file_exists = ::wxFileExists(list[i]);

            if (dir_exists) {
                if (m_style & VALIDPATH_IS_PLAIN_FILE) {
                    errormsg.Printf(_("'%s' is not a plain file."), list[i].c_str());
                    valid = false;
                    break;
                }
            }
            else if (file_exists) {
                if (m_style & VALIDPATH_IS_DIRECTORY) {
                    errormsg.Printf(_("'%s' is not a directory."), list[i].c_str());
                    valid = false;
                    break;
                }
            }
            else {
                if (m_style & VALIDPATH_EXISTS) {
                    errormsg.Printf(_("'%s' does not exist."), list[i].c_str());
                    valid = false;
                    break;
                }
            }
        }
    }

    if (!valid) {
        wxMessageBox(errormsg, _("Bad file or directory"), wxOK | wxICON_EXCLAMATION, parent);
        control->SetFocus();
        control->SetSelection(-1, -1);
    }
    return valid;
}

bool CPathValidator::TransferFromWindow(void)
{
    if (m_value && m_validatorWindow->IsKindOf(CLASSINFO(wxTextCtrl))) {
        wxTextCtrl *ctrl = (wxTextCtrl *) m_validatorWindow;
        *m_value = ctrl->GetValue();

        if (m_style & VALIDPATH_TO_UNIX) {
            m_value->Replace(wxString(wxFileName::GetPathSeparator()), wxT("/"));
        }
    }
    return true;
}

bool CPathValidator::TransferToWindow(void)
{
    if (m_value && m_validatorWindow->IsKindOf(CLASSINFO(wxTextCtrl))) {
        wxTextCtrl *control = (wxTextCtrl *) m_validatorWindow;

        if (m_style & VALIDPATH_SHOW_NATIVE) {
            m_value->Replace(wxT("/"), wxString(wxFileName::GetPathSeparator()));
        }
        control->SetValue(* m_value);
    }
    return true;
}


bool CPathValidator::Copy(const CPathValidator& src)
{
    wxValidator::Copy(src);

    m_style = src.m_style;
    m_value = src.m_value;

    return true;
}
