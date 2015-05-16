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

#include "stateicontext.h"

IMPLEMENT_DYNAMIC_CLASS(CStateIconText, wxDataViewIconText)

IMPLEMENT_VARIANT_OBJECT(CStateIconText)

CStateIconText::CStateIconText(const wxString& text, const wxIcon& icon, bool enabled)
: wxDataViewIconText(text, icon)
, m_enabled(enabled)
{
}

CStateIconText::CStateIconText(const CStateIconText& other)
: wxDataViewIconText(other)
, m_enabled(other.m_enabled)
{
}

bool CStateIconText::IsSameAs(const CStateIconText& other) const
{
    return wxDataViewIconText::IsSameAs(other) && m_enabled == other.m_enabled;
}

void CStateIconText::SetState(bool set)
{
    m_enabled = set;
}

bool CStateIconText::GetState() const
{
    return m_enabled;
}
