#ifndef STATEICONTEXT_H_
#define STATEICONTEXT_H_

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

#include <wx/dataview.h>

/*!
 * \brief Variant type class containing state, icon and text.
 *
 * This class is used to represent a value in the ConfItemRenderer.
 */
class CStateIconText : public wxDataViewIconText
{
public:
    CStateIconText(const wxString& text = wxString(),
                            const wxIcon& icon = wxNullIcon,
                            bool enabled = false);
    CStateIconText(const CStateIconText& other);
    bool IsSameAs(const CStateIconText& other) const;
    void SetState(bool set);
    bool GetState() const;

private:
    bool m_enabled;
    DECLARE_DYNAMIC_CLASS(CStateIconText);
};

DECLARE_VARIANT_OBJECT(CStateIconText)

#endif
