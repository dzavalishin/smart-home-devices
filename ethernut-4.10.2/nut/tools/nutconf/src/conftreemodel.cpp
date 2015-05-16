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

#include "nutconf.h"
#include "conftreemodel.h"
#include "nutconfdoc.h"

CConfTreeModel::CConfTreeModel()
{
    m_config = NULL;
    m_root_node = new CConfTreeNode(NULL, m_config, (NUTCOMPONENT *)NULL);
}

CConfTreeModel::~CConfTreeModel()
{
    delete m_root_node;
}

CConfTreeModel::CConfTreeModel(CNutConfDoc *doc, NUTCOMPONENT *root_component)
{
    m_config = doc;
    m_root_node = new CConfTreeNode(NULL, m_config, (NUTCOMPONENT *)NULL);
    AddChildItems(root_component, m_root_node);
}

/*!
 * Overrides the pure virtual method of the base class.
 */
unsigned int CConfTreeModel::GetColumnCount() const
{
    return 2;
}

/*!
 * Overrides the pure virtual method of the base class.
 */
wxString CConfTreeModel::GetColumnType(unsigned int col) const
{
    if (col == MODEL_COLUMN_ITEM)
        return wxT("CStateIconText");

    return wxT("string");
}

void CConfTreeModel::AddChildItems(NUTCOMPONENT * compo, CConfTreeNode *parent)
{
    if(compo) {
        compo = compo->nc_child;
        while (compo) {
            CConfTreeNode *cnode = new CConfTreeNode(parent, m_config, compo);

            NUTCOMPONENTOPTION *opts = compo->nc_opts;
            while (opts) {
                cnode->Append(new CConfTreeNode(cnode, m_config, opts));
                opts = opts->nco_nxt;
            }
            parent->Append(cnode);

            AddChildItems(compo, cnode);
            compo = compo->nc_nxt;
        }
    }
}

/*!
 * Overrides the pure virtual method of the base class.
 */
void CConfTreeModel::GetValue(wxVariant &variant, const wxDataViewItem &item, unsigned int col) const
{
    wxASSERT(item.IsOk());

    CConfTreeNode *node = (CConfTreeNode*) item.GetID();
    switch (col) {
    case MODEL_COLUMN_ITEM:
        variant = node->GetStateIconText();
        break;
    case MODEL_COLUMN_VALUE:
        variant = node->GetOptionText();
        break;
    default:
        wxFAIL_MSG("Illegal model column");
        break;
    }
}

bool CConfTreeModel::SetValue(const wxVariant &variant, const wxDataViewItem &item, unsigned int col)
{
    wxASSERT(item.IsOk());
    CConfTreeNode *node = (CConfTreeNode*) item.GetID();
    wxASSERT(col == MODEL_COLUMN_VALUE);

    node->SetValue(variant.GetString());

    return true;
}

wxDataViewItem CConfTreeModel::GetRootItem() const
{
    return wxDataViewItem((void*) m_root_node);
}

/*!
 * Overrides the pure virtual method of the base class.
 */
wxDataViewItem CConfTreeModel::GetParent(const wxDataViewItem &item) const
{
    if (!item.IsOk())
        return wxDataViewItem(0);

    CConfTreeNode *node = (CConfTreeNode*) item.GetID();

    if (node == m_root_node)
        return wxDataViewItem(0);

    return wxDataViewItem((void*) node->GetParent());
}

/*!
 * Overrides the pure virtual method of the base class.
 */
bool CConfTreeModel::IsContainer(const wxDataViewItem &item) const
{
    if (!item.IsOk())
        return true;

    CConfTreeNode *node = (CConfTreeNode*) item.GetID();
    return node->IsContainer();
}

bool CConfTreeModel::IsEnabled(const wxDataViewItem &item, unsigned int WXUNUSED(col)) const
{
    wxASSERT(item.IsOk());

    CConfTreeNode *node = (CConfTreeNode*) item.GetID();

    return node->IsEnabled();
}

/*!
 * Overrides the pure virtual method of the base class.
 */
unsigned int CConfTreeModel::GetChildren(const wxDataViewItem &parent, wxDataViewItemArray &array) const
{
    CConfTreeNode *node = (CConfTreeNode*) parent.GetID();
    if (!node) {
        array.Add(wxDataViewItem((void*) m_root_node));
        return 1;
    }

    if (node->GetChildCount() == 0) {
        return 0;
    }

    unsigned int count = node->GetChildren().GetCount();
    for (unsigned int pos = 0; pos < count; pos++) {
        CConfTreeNode *child = node->GetChildren().Item(pos);
        array.Add(wxDataViewItem((void*) child));
    }
    return count;
}

/*!
 * \brief Toggle the item's active state.
 */
void CConfTreeModel::ToggleItem(const wxDataViewItem &item)
{
    CConfTreeNode *node = (CConfTreeNode*) item.GetID();

    /* Toggle the node's state. */
    node->ToggleItem();
}

unsigned CConfTreeModel::GetRow(const wxDataViewItem& WXUNUSED(item)) const
{
    return 0;
}

wxDataViewItem CConfTreeModel::GetItem(unsigned int WXUNUSED(row)) const
{
    return wxDataViewItem();
}
