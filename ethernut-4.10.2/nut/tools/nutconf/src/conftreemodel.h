#ifndef CONFTREEMODEL_H_
#define CONFTREEMODEL_H_

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

#include "conftreenode.h"

class CConfTreeModel:public wxDataViewModel {
  public:
    CConfTreeModel();
    ~CConfTreeModel();
    CConfTreeModel(CNutConfDoc *doc, NUTCOMPONENT *root_component);

    enum {
        MODEL_COLUMN_ITEM,
        MODEL_COLUMN_VALUE,
        MODEL_COLUMN_MAX
    };

    void AddChildItems(NUTCOMPONENT * compo, CConfTreeNode *parent);

    virtual unsigned int GetColumnCount() const;
    virtual wxString GetColumnType(unsigned int col) const;

    virtual void GetValue(wxVariant &variant, const wxDataViewItem &item, unsigned int col) const;
    virtual bool SetValue(const wxVariant &variant, const wxDataViewItem &item, unsigned int col);
    wxDataViewItem GetRootItem() const;
    virtual wxDataViewItem GetParent(const wxDataViewItem &item) const;
    virtual bool IsContainer(const wxDataViewItem &item) const;
    virtual bool IsEnabled(const wxDataViewItem &item, unsigned int col) const;
    virtual unsigned int GetChildren(const wxDataViewItem &parent, wxDataViewItemArray &array) const;
    virtual unsigned GetRow(const wxDataViewItem &item) const;
    wxDataViewItem GetItem(unsigned int row) const;
    void ToggleItem(const wxDataViewItem &item);
private:
    CNutConfDoc* m_config;
    CConfTreeNode* m_root_node;
};

#endif

