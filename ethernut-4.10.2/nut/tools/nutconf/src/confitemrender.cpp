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

#include "conftreemodel.h"
#include "confitemrender.h"
#include <wx/renderer.h>
#include <wx/font.h>

ConfItemRenderer::ConfItemRenderer()
    : wxDataViewCustomRenderer(
        "CStateIconText", // Variant type.
        wxDATAVIEW_CELL_ACTIVATABLE) // Will call ActivateCell() if the icon is clicked.
{
}

bool ConfItemRenderer::Render(wxRect rect, wxDC *dc, int state)
{
    const wxIcon& icon = m_value.GetIcon();
    bool enabled = m_value.GetState();
    int xoffset = 0;

    if (icon.IsOk()) {
        const wxSize sizeIcon = icon.GetSize();
        wxRect rectIcon(rect.GetPosition(), sizeIcon);
        rectIcon.x += xoffset;
        rectIcon = rectIcon.CentreIn(rect, wxVERTICAL);

        if (!enabled) {
            wxBitmap bmp = wxBitmap(icon).ConvertToDisabled();
            wxIcon greyed_icon;
            greyed_icon.CopyFromBitmap(bmp);
#ifdef __WXMSW__
            greyed_icon.SetSize(16, 16);
#endif
            dc->DrawIcon(greyed_icon, rectIcon.GetPosition());
        } else {
            dc->DrawIcon(icon, rectIcon.GetPosition());
        }

        xoffset += sizeIcon.x + 4;
    }
    if (!enabled) {
        dc->SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));
    }
    RenderText(m_value.GetText(), xoffset, rect, dc, state);

    return true;
}

/*!
 * \brief Activate cell.
 *
 * User clicked on the cell or pressed the space bar.
 *
 * This method will only be called if the cell has the wxDATAVIEW_CELL_ACTIVATABLE mode.
 * We override the base class method to toggle the item's active state.
 */
bool ConfItemRenderer::ActivateCell(const wxRect& WXUNUSED(cell),
                              wxDataViewModel *model,
                              const wxDataViewItem &item,
                              unsigned int WXUNUSED(col),
                              const wxMouseEvent *mouseEvent)
{
    if (mouseEvent) {
        /* Triggered by mouse, check if the icon had been clicked. */
        if (!wxRect(0, 0, 16, 16).Contains(mouseEvent->GetPosition())) {
            /* If that click was not on the icon, ignore it. */
            return false;
        }
    }
    /* Request the model to toggle the item's active state and do 
       all necessary updates. */
    static_cast<CConfTreeModel*>(model)->ToggleItem(item);

    return false;
}

wxSize ConfItemRenderer::GetTextExtent(const wxString& str) const
{
    const wxDataViewCtrl *view = GetView();

    return view->GetTextExtent(str);
}

wxSize ConfItemRenderer::GetSize() const
{
    wxSize size(0, 0);

    if (m_value.GetIcon().IsOk()) {
        const wxSize sizeIcon = m_value.GetIcon().GetSize();

        if (sizeIcon.y > size.y)
            size.y = sizeIcon.y;

        size.x += sizeIcon.x + 4;
    }
    wxString text = m_value.GetText();
    if ( text.empty() )
        text = "Dummy";

    const wxSize sizeText = GetTextExtent(text);
    if ( sizeText.y > size.y )
        size.y = sizeText.y;
    size.x += sizeText.x;

    return size;
}

bool ConfItemRenderer::SetValue(const wxVariant &value)
{
    m_value << value;

    return true;
}

bool ConfItemRenderer::GetValue(wxVariant &WXUNUSED(value)) const
{
    return false;
}
