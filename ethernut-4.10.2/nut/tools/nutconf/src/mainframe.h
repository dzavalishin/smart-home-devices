#ifndef MAINFRAME_H_
#define MAINFRAME_H_

/* ----------------------------------------------------------------------------
 * Copyright (C) 2012 by egnite GmbH
 * Copyright (C) 2004 by egnite Software GmbH
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
 * $Id: mainframe.h 3878 2012-02-15 15:46:22Z haraldkipp $
 */

#include <wx/wx.h>
#include <wx/laywin.h>
#include <wx/splitter.h>
#include <wx/docview.h>
#include <wx/treectrl.h>
#include <wx/help.h>

#include "conftreectrl.h"
#include "conftreemodel.h"
#include "infowindow.h"
#include "propertylist.h"
#include "finddlg.h"

#if !wxUSE_DOC_VIEW_ARCHITECTURE
#error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

/*
 * \brief Main frame acting as a document parent frame.
 */
class CMainFrame:public wxDocParentFrame {
  public:
    CMainFrame(wxDocManager * manager, const wxString & title);
    virtual ~CMainFrame();

    void OnSize(wxSizeEvent & event);
    void OnSashDrag(wxSashEvent & event);

    wxTextCtrl *GetOutputWindow() const;
    CConfTreeCtrl *GetTreeCtrl() const;
    CInfoWindow *GetInfoWindow() const;
    CPropertyList *GetPropertyListWindow() const;

    wxHelpController& GetHelpController() { return m_help; }

    CFindDialog *GetFindDialog() const;
    void SetFindDialog(CFindDialog * dlg);

    /* ---------- Menu Event Handlers ---------- */
    void OnQuit(wxCommandEvent & event);
    void OnGenerateBuildTree(wxCommandEvent & event);
    void OnBuildNutOS(wxCommandEvent & event);
    void OnBuildLibraries(wxCommandEvent & event);
    void OnCreateSampleDir(wxCommandEvent & event);
    void OnSettings(wxCommandEvent& event);
    void OnToggleToolbar(wxCommandEvent& event);
    void OnToggleToolbarSize(wxCommandEvent& event);
    void OnHelp(wxCommandEvent& event);
    void OnFind(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnItemActivated(wxDataViewEvent& event);
    void OnSelectionChanged(wxDataViewEvent& event);
    void OnItemValueChanged(wxDataViewEvent& event);

  protected:
    void CreateNutMenuBar();
    void CreateNutToolBar();
    void CreateNutStatusBar();
    void CreateNutWindows();

  protected:
    wxSashLayoutWindow *m_outputSashWindow;
    wxSashLayoutWindow *m_configSashWindow;
    wxSashLayoutWindow *m_propertiesSashWindow;
    wxSashLayoutWindow *m_infoSashWindow;

    wxPanel *m_panel;
    CConfTreeCtrl *m_treeCtrl;

    CPropertyList *m_propertyListWindow;
    CInfoWindow *m_infoWindow;

    wxTextCtrl *m_outputWindow;

    bool m_smallToolbar;

  private:
    wxHelpController m_help;
    CFindDialog* m_findDlg;

    DECLARE_EVENT_TABLE()

};

#endif
