/* ----------------------------------------------------------------------------
 * Copyright (C) 2012 by egnite GmbH
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
 * $Id$
 */

#include <wx/wx.h>
#include <wx/dataview.h>
#include <wx/renderer.h>
#include <wx/font.h>
#include <wx/spinctrl.h>

#include "ids.h"
#include "conftreectrl.h"
#include "confvalrender.h"

ConfValueRenderer::ConfValueRenderer(CConfTreeCtrl *tree)
    : wxDataViewCustomRenderer("string", wxDATAVIEW_CELL_EDITABLE)
{
    m_tree = tree;
}

bool ConfValueRenderer::SetValue(const wxVariant &value)
{
    m_value = value.GetString();
    return true;
}

/*!
 * \brief Retrieves the value from the renderer.
 *
 * Used to transfer the value back to the data model.
 *
 * \return false on failure. 
 */
bool ConfValueRenderer::GetValue(wxVariant& value) const
{
    value = m_value;
    return true;
}

/*!
 * \brief Indicate if this renderer supports in-place editing.
 *
 * \note wxWidgets 2.9.3 never calls this method.
 */
bool ConfValueRenderer::HasEditorCtrl() const
{
    return true;
}

wxWindow* ConfValueRenderer::CreateEditorCtrl(wxWindow *parent, wxRect labelRect, const wxVariant &value)
{
    wxWindow *window = NULL;
    CConfTreeNode *node = NULL;
    wxDataViewItem sel = m_tree->GetSelection();
    if (sel.IsOk()) {
        node = (CConfTreeNode*) sel.GetID();
    }
    if (node) {
        switch (node->GetOptionType()) {
        case nutEnumerated:
            /* Create drop down box for enumerated values. */
            {
                wxArrayString arEnumStrings;
                node->GetEnumStrings(arEnumStrings);
                wxChoice* ctrl = new wxChoice(parent, wxID_ANY, labelRect.GetTopLeft(), wxSize(labelRect.GetWidth(), -1), arEnumStrings);
                ctrl->SetStringSelection(value.GetString());
                window = ctrl;
            }
            break;
        case nutInteger:
            /* Use a spin control for integer values. */
            {
                long l = value.GetLong();
                wxSize size = labelRect.GetSize();
                wxString str;
                str.Printf(wxT("%d"), (int) l);

                wxSpinCtrl *ctrl = NULL;
                ctrl = new wxSpinCtrl(parent, ID_ITEM_EDIT_WINDOW, str, labelRect.GetTopLeft(), size, wxSP_ARROW_KEYS | wxTE_PROCESS_ENTER, -32000, 32000, l);
                window = ctrl;
            }
            break;
        case nutString:
            /* Normal entry control for string values. */
            {
                wxTextCtrl *ctrl = new wxTextCtrl(parent, ID_ITEM_EDIT_WINDOW, value, wxPoint(labelRect.x,labelRect.y), wxSize(labelRect.width,labelRect.height), wxTE_PROCESS_ENTER);
                ctrl->SetInsertionPointEnd();
                ctrl->SelectAll();
                window = ctrl;
            }
            break;
        }
    }
    return window;
}

bool ConfValueRenderer::GetValueFromEditorCtrl(wxWindow *editor, wxVariant &value)
{
    CConfTreeNode *node = NULL;
    wxDataViewItem sel = m_tree->GetSelection();
    if (sel.IsOk()) {
        node = (CConfTreeNode*) sel.GetID();
    }
    if (node) {
        switch (node->GetOptionType()) {
        case nutEnumerated:
            {
                wxChoice *ctrl = (wxChoice*) editor;
                wxString str = ctrl->GetStringSelection();
                value = str;
            }
            break;
        case nutInteger:
            {
                wxSpinCtrl *ctrl = (wxSpinCtrl*) editor;
                long l = ctrl->GetValue();
                wxString str;
                str.Printf(wxT("%d"), (int) l);
                value = str;
            }
            break;
        case nutString:
            {
                wxTextCtrl *ctrl = (wxTextCtrl*) editor;
                value = ctrl->GetValue();
            }
            break;
        }
    }
    return true;
}

bool ConfValueRenderer::Render(wxRect rect, wxDC *dc, int state)
{
    RenderText(m_value, 0, rect, dc, state);

    return true;
}

wxSize ConfValueRenderer::GetTextExtent(const wxString& str) const
{
    const wxDataViewCtrl *view = GetView();

    return view->GetTextExtent(str);
}

wxSize ConfValueRenderer::GetSize() const
{
    wxString text = m_value;
    wxSize size(0, 0);

    if (text.empty())
        text = "Ethernut";

    const wxSize sizeText = GetTextExtent(text);
    if (sizeText.y > size.y)
        size.y = sizeText.y;
    size.x += sizeText.x;

    return size;
}

