/* ----------------------------------------------------------------------------
 * Copyright (C) 2004-2005 by egnite Software GmbH
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
 * $Id: propertylist.cpp 3878 2012-02-15 15:46:22Z haraldkipp $
 */

#include "nutconf.h"
#include "utils.h"
#include "propertylist.h"

const wxChar *CPropertyList::m_propertyName[nutMAXFIELDTYPE] = { 
    wxT("Enabled"), 
    wxT("Active"),
    wxT("Requires"), 
    wxT("Provides"), 
    wxT("Exclusivity"), 
    wxT("Type"), 
    wxT("File"), 
    wxT("Macro"), 
    wxT("Value")
};


/*
 * CPropertyList
 */

IMPLEMENT_CLASS(CPropertyList, wxListCtrl)

BEGIN_EVENT_TABLE(CPropertyList, wxListCtrl)
    EVT_RIGHT_DOWN(CPropertyList::OnRightClick)
    EVT_LEFT_DCLICK(CPropertyList::OnDoubleClick)
END_EVENT_TABLE();

CPropertyList::CPropertyList(wxWindow * parent, wxWindowID id, const wxPoint & pt, const wxSize & sz, long style)
    :wxListCtrl(parent, id, pt, sz, style)
{
    m_node = NULL;

    AddColumns();
}

void CPropertyList::OnRightClick(wxMouseEvent & WXUNUSED(event))
{
}

void CPropertyList::AddColumns()
{
    InsertColumn(0, wxT("Property"), wxLIST_FORMAT_LEFT, 100);
    InsertColumn(1, wxT("Value"), wxLIST_FORMAT_LEFT, 300);
}

/*!
 * \brief Fill property list.
 *
 * Retrieves the properties of the currently selected item and updates
 * the list in the property window.
 *
 * \param pti Pointer to the currently selected item.
 */
void CPropertyList::Fill(const CConfTreeNode* node)
{
    /*
     * Clear the list.
     */
    ClearAll();
    AddColumns();
    m_node = node;

    /*
     * Display this item's properties.
     */
    if (m_node) {
        int i;

        /*
         * Remove previously set item data.
         */
        for (i = GetItemCount() - 1; i >= 0; --i) {
            SetItemData(i, 0);
        }

        /*
         * Set the file name property. Typically a relative path to a
         * source code header file.
         */
        wxString value = node->GetFilename();
        if (!value.IsEmpty()) {
            SetItem(nutFile, value);
        }

        /*
         * Set the macro property.
         */
        value = m_node->GetMacro();
        if (!value.IsEmpty()) {
            SetItem(nutMacro, value);

            /*
             * The macro's value is optional.
             */
            switch (m_node->GetOptionType()) {
            case nutOptionTypeNone:
                break;
            case nutEnumerated:
            case nutString:
                if(!m_node->GetOptionText().IsEmpty()) {
                    SetItem(nutValue, m_node->GetOptionText());
                }
                break;
            case nutBool:
            case nutInteger:
                break;
            default:
                wxASSERT(false);
                break;
            }

            /*
             * Boolean options must be active to be included.
             */
            if (m_node->HasBool()) {
                SetItem(nutActive, m_node->IsActive()? wxT("True") : wxT("False"));
            }
        }

        /*
         * An item may be disabled, if it requires a property which
         * is not provided by any active component.
         */
        SetItem(nutEnabled, m_node->IsEnabled()? wxT("Yes") : wxT("No"));

        /*
         * Remove all items from the property list without previously
         * set data.
         */
        for (i = GetItemCount() - 1; i >= 0; --i) {
            if (GetItemData(i) == 0) {
                DeleteItem(i);
            }
        }

        value = m_node->GetRequirementList();
        if (!value.IsEmpty()) {
            SetItem(nutRequires, value);
        }
        value = m_node->GetProvisionList();
        if (!value.IsEmpty()) {
            SetItem(nutProvides, value);
        }
        value = m_node->GetExclusivityList();
        if (!value.IsEmpty()) {
            SetItem(nutExclusivity, value);
        }
    }
}

void CPropertyList::RefreshValue()
{
    if(m_node) {
        if (m_node->HasBool()) {
            SetItem(nutActive, m_node->IsActive()? wxT("True") : wxT("False"));
        }
        SetItem(nutEnabled, m_node->IsEnabled()? wxT("Yes") : wxT("No"));
        if (m_node->GetOptionType () != nutOptionTypeNone) {
            if(!m_node->GetOptionText().IsEmpty()) {
                SetItem(nutValue, m_node->GetOptionText());
            }
        }
    }
}

int CPropertyList::SetItem(nutPropertyType type, const wxString & value)
{
    int nIndex;

    /* Check if this item is in the list already. */
    nIndex = FindItem(0, m_propertyName[type]);
    if (nIndex == -1) {
        nIndex = InsertItem(GetItemCount(), m_propertyName[type]);
    }
    wxListCtrl::SetItem(nIndex, 1, value);
    SetItemData(nIndex, 1);

    return nIndex;
}

void CPropertyList::OnDoubleClick(wxMouseEvent & event)
{
    int flags;
    long item = HitTest(event.GetPosition(), flags);

    if (item > -1) {
        wxString strText;
        wxListItem listItem;
        listItem.m_mask = wxLIST_MASK_TEXT;
        listItem.m_itemId = item;
        listItem.m_col = 0;
        if (GetItem(listItem))
            strText = listItem.m_text;
        else
            strText = wxEmptyString;

        if (strText == wxT("File")) {
            m_node->ViewHeaderFile();
        }
    }
    event.Skip();
}

