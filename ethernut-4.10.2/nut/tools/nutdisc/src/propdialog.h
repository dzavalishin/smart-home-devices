#ifndef PROPDIALOG_H_
#define PROPDIALOG_H_

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
 * $Id: propdialog.h 3841 2012-02-10 18:19:35Z haraldkipp $
 */

#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/propgrid/propgrid.h>

#include "discovery.h"

class CPropDialog: public wxDialog
{
public:
    CPropDialog(const wxString& title, DISCOVERY_TELE *dist);
    ~CPropDialog();
    void GetSetup(DISCOVERY_TELE *dist);

private:
    DISCOVERY_TELE m_dist;

protected:
    wxPropertyGrid* m_grid;

    bool GetValues();

    void OnOk(wxCommandEvent &event);
    void OnCancel(wxCommandEvent &event);
    void OnClose(wxCloseEvent& event);


    DECLARE_EVENT_TABLE()
};

#endif
