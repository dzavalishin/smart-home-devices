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

#include "mainframe.h"
#include "conftreectrl.h"

IMPLEMENT_DYNAMIC_CLASS(CConfTreeCtrl, wxDataViewCtrl)

BEGIN_EVENT_TABLE(CConfTreeCtrl, wxDataViewCtrl)
   EVT_SIZE( CConfTreeCtrl::OnSize )
END_EVENT_TABLE()

CConfTreeCtrl::CConfTreeCtrl()
{
}

CConfTreeCtrl::CConfTreeCtrl(wxWindow * parent, wxWindowID id, const wxPoint & pt, const wxSize & sz)
:  wxDataViewCtrl(parent, id, pt, sz, wxDV_VERT_RULES | wxDV_HORIZ_RULES)
{
    m_render_col1 = new ConfItemRenderer;
    m_col1 = new wxDataViewColumn("Item", m_render_col1, CConfTreeModel::MODEL_COLUMN_ITEM, 200);
    AppendColumn(m_col1);

    m_render_col2 = new ConfValueRenderer(this);
    m_col2 = new wxDataViewColumn("Value", m_render_col2, CConfTreeModel::MODEL_COLUMN_VALUE, 100, wxALIGN_LEFT,
        wxDATAVIEW_COL_SORTABLE | wxDATAVIEW_COL_REORDERABLE | wxDATAVIEW_COL_RESIZABLE);
    AppendColumn(m_col2);
}

void CConfTreeCtrl::OnSize(wxSizeEvent &event)
{
    if (GetColumnCount()) {
        int width1 = GetColumn(1)->GetWidth();
        int unfilled = GetClientSize().x - (GetColumn(0)->GetWidth() + width1);

        if (width1 + unfilled >= 100) {
            GetColumn(1)->SetWidth(width1 + unfilled);
        }
    }
    event.Skip(true);
}
