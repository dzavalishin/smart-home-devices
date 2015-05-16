#ifndef OPTIONS_H_
#define OPTIONS_H_

/* ----------------------------------------------------------------------------
 * Copyright (C) 2009-2011 by egnite GmbH
 * Copyright (C) 2005-2006 by egnite Software GmbH
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
 * $Id: propdialog.h 2839 2009-12-17 11:57:55Z haraldkipp $
 */

#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

class COptions
{
public:
    COptions();
    void Save();

    wxString m_scanip;
    wxString m_scanport;
    wxString m_scantime;
};

extern COptions *g_options;

class COptionsDialog : public wxDialog
{
public:
    COptionsDialog(wxWindow *parent);
    virtual ~COptionsDialog();

protected:
    wxTextCtrl *m_scanip_entry;
    wxTextCtrl *m_scanport_entry;
    wxTextCtrl *m_scantime_entry;
};

#endif
