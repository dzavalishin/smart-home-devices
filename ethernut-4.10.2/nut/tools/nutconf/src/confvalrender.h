#ifndef CONFVALRENDER_H_
#define CONFVALRENDER_H_

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

class CConfTreeCtrl;

class ConfValueRenderer: public wxDataViewCustomRenderer
{
public:
    ConfValueRenderer(CConfTreeCtrl *tree);

    virtual bool SetValue(const wxVariant &value);
    virtual bool GetValue(wxVariant &WXUNUSED(value)) const;
    virtual wxSize GetSize() const;

    virtual bool HasEditorCtrl() const;
    virtual wxWindow* CreateEditorCtrl(wxWindow *parent, wxRect labelRect, const wxVariant &value);
    virtual bool GetValueFromEditorCtrl(wxWindow *editor, wxVariant &value);

    virtual bool Render(wxRect rect, wxDC *dc, int state);

protected:
    wxSize GetTextExtent(const wxString& str) const;

private:
    CConfTreeCtrl *m_tree;
    wxString m_value;
};

#endif
