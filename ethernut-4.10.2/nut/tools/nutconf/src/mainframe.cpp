/* ----------------------------------------------------------------------------
 * Copyright (C) 2004-2007 by egnite Software GmbH
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
 * $Id: mainframe.cpp 3878 2012-02-15 15:46:22Z haraldkipp $
 */

#include <wx/config.h>
#include <wx/sysopt.h>

#include "ids.h"
#include "nutconf.h"
#include "settings.h"
#include "settingsdlg.h"
#include "conftreectrl.h"
#include "nutconfhint.h"
#include "mainframe.h"

#if !defined(__WXMSW__)
#include "bitmaps/application.xpm"
#include "bitmaps/buildlibrary.xpm"
#include "bitmaps/buildlibrary_large.xpm"
#include "bitmaps/help.xpm"
#include "bitmaps/help_large.xpm"
#include "bitmaps/open.xpm"
#include "bitmaps/open_large.xpm"
#include "bitmaps/save.xpm"
#include "bitmaps/save_large.xpm"
#include "bitmaps/search.xpm"
#include "bitmaps/search_large.xpm"
#endif

BEGIN_EVENT_TABLE(CMainFrame, wxDocParentFrame)
    EVT_MENU(wxID_EXIT, CMainFrame::OnQuit)
    EVT_MENU(ID_BUILD_LIBRARY, CMainFrame::OnBuildLibraries)
    EVT_MENU(ID_GENERATE_BUILD_TREE, CMainFrame::OnGenerateBuildTree)
    EVT_MENU(ID_BUILD_NUTOS, CMainFrame::OnBuildNutOS)
    EVT_MENU(ID_CREATE_SAMPLE_APPS, CMainFrame::OnCreateSampleDir)
    EVT_SIZE(CMainFrame::OnSize)
    EVT_SASH_DRAGGED_RANGE(ID_CONFIG_SASH_WINDOW, ID_OUTPUT_SASH_WINDOW, CMainFrame::OnSashDrag)
    EVT_MENU(ID_SETTINGS, CMainFrame::OnSettings)
    EVT_MENU(ID_TOOLBARS, CMainFrame::OnToggleToolbar)
    EVT_MENU(ID_TOOLBARSIZE, CMainFrame::OnToggleToolbarSize)
    EVT_MENU(ID_NUTOS_HELP, CMainFrame::OnHelp)
    EVT_MENU(wxID_FIND, CMainFrame::OnFind)
    EVT_MENU(ID_HELP_ABOUT, CMainFrame::OnAbout)
    EVT_DATAVIEW_ITEM_ACTIVATED(ID_CONFTREE_CTRL, CMainFrame::OnItemActivated)
    EVT_DATAVIEW_SELECTION_CHANGED(ID_CONFTREE_CTRL, CMainFrame::OnSelectionChanged)
    EVT_DATAVIEW_ITEM_VALUE_CHANGED(ID_CONFTREE_CTRL, CMainFrame::OnItemValueChanged)
END_EVENT_TABLE();

CMainFrame::CMainFrame(wxDocManager * manager, const wxString & title)
:wxDocParentFrame(manager, (wxFrame *) NULL, ID_MAIN_FRAME, title)
, m_smallToolbar(true)
, m_findDlg(NULL)
{
    SetIcon(wxICON(application));

    CreateNutMenuBar();
    CreateNutToolBar();
    CreateNutStatusBar();

    CreateNutWindows();

    /*
     * Restore frame position and size.
     */
    wxConfigBase *pConfig = wxConfigBase::Get();
    Move(pConfig->Read(wxT("/MainFrame/x"), 50), pConfig->Read(wxT("/MainFrame/y"), 50));
    SetClientSize(pConfig->Read(wxT("/MainFrame/w"), 550), pConfig->Read(wxT("/MainFrame/h"), 350));

}

/*!
 * \brief Destructor.
 */
CMainFrame::~CMainFrame()
{
    // Save frame size and position.
    wxConfigBase *pConfig = wxConfigBase::Get();
    if (pConfig) {
        wxString lastPath = pConfig->GetPath();
        pConfig->SetPath(wxT("/MainFrame"));

        /*
         * Frame window position and client window size.
         */
        int x, y;
        int w, h;
        GetPosition(&x, &y);
        pConfig->Write(wxT("x"), (long) x);
        pConfig->Write(wxT("y"), (long) y);
        GetClientSize(&w, &h);
        pConfig->Write(wxT("w"), (long) w);
        pConfig->Write(wxT("h"), (long) h);

        wxSize sz;
        sz = m_configSashWindow->GetSize();
        pConfig->Write(wxT("tree_w"), (long) sz.x);
        pConfig->Write(wxT("tree_h"), (long) sz.y);

        sz = m_propertiesSashWindow->GetSize();
        pConfig->Write(wxT("prop_w"), (long) sz.x);
        pConfig->Write(wxT("prop_h"), (long) sz.y);

        sz = m_infoSashWindow->GetSize();
        pConfig->Write(wxT("desc_w"), (long) sz.x);
        pConfig->Write(wxT("desc_h"), (long) sz.y);

        sz = m_outputSashWindow->GetSize();
        pConfig->Write(wxT("outp_w"), (long) sz.x);
        pConfig->Write(wxT("outp_h"), (long) sz.y);

        pConfig->SetPath(lastPath);
    }
}

void CMainFrame::CreateNutMenuBar()
{
    wxMenu *fileMenu = new wxMenu(wxEmptyString, wxMENU_TEAROFF);
    fileMenu->Append(wxID_OPEN, wxT("&Open\tCtrl+O"), wxT("Opens an existing configuration"));
    fileMenu->Append(wxID_SAVE, wxT("&Save\tCtrl+S"), wxT("Saves the active configuration"));
    fileMenu->Append(wxID_SAVEAS, wxT("Save &As..."), wxT("Saves the active configuration with a new name"));
    fileMenu->Append(wxID_EXIT, wxT("E&xit\tAlt+X"), wxT("Quits the application"));

    wxMenu *editMenu = new wxMenu(wxEmptyString, wxMENU_TEAROFF);
    editMenu->Append(wxID_FIND, wxT("&Find...\tCtrl+F"), wxT("Finds the specified text"));
    editMenu->Append(ID_SETTINGS, wxT("&Settings...\tCtrl+T"), wxT("Shows the settings dialog"));

    wxMenu *viewMenu = new wxMenu(wxT(""), wxMENU_TEAROFF);
    wxMenu *toolbarMenu = new wxMenu;
    toolbarMenu->AppendCheckItem(ID_TOOLBARS, wxT("&Show"), wxT("Shows or hides the toolbar"));
    toolbarMenu->Check(ID_TOOLBARS, true);
    toolbarMenu->AppendCheckItem(ID_TOOLBARSIZE, wxT("&Large Buttons"), wxT("Toggles toolbar size"));
    viewMenu->Append(-1, wxT("Toolbar"), toolbarMenu);

    wxMenu *buildMenu = new wxMenu(wxEmptyString, wxMENU_TEAROFF);
    buildMenu->Append(ID_BUILD_LIBRARY, wxT("Build Nut/OS"), wxT("Builds Nut/OS libraries"));
    buildMenu->AppendSeparator();
    buildMenu->Append(ID_CREATE_SAMPLE_APPS, wxT("Create Sample Directory"), wxT("Creates a directory with Nut/OS sample applications"));

    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(ID_NUTOS_HELP, wxT("Help &Contents"), wxT("Displays help contents"));
    helpMenu->Append(ID_HELP_ABOUT, wxT("About NutConf..."), wxT("Displays version and copyright information"));

    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, wxT("&File"));
    menuBar->Append(editMenu, wxT("&Edit"));
    menuBar->Append(viewMenu, wxT("&View"));
    menuBar->Append(buildMenu, wxT("&Build"));
    menuBar->Append(helpMenu, wxT("&Help"));

    SetMenuBar(menuBar);
}


void CMainFrame::CreateNutToolBar()
{
    wxSystemOptions::SetOption("msw.remap", 0);

    wxToolBarBase *toolBar = CreateToolBar(wxNO_BORDER | wxTB_FLAT | wxTB_HORIZONTAL | wxTB_DOCKABLE, ID_TOOLBAR);
    toolBar->SetMargins(4, 4);

    /* Load small toolbar buttons. */
    if (m_smallToolbar) {
        toolBar->AddTool(wxID_OPEN, wxT("Open"), wxBITMAP(TBB_OPEN), wxT("Load configuration"));
        toolBar->AddTool(wxID_SAVE, wxT("Save"), wxBITMAP(TBB_SAVE), wxT("Save configuration"));
        toolBar->AddTool(ID_BUILD_LIBRARY, wxT("Build"), wxBITMAP(TBB_BUILDLIBRARY), wxT("Build Nut/OS"));
        toolBar->AddTool(wxID_FIND, wxT("Search"), wxBITMAP(TBB_SEARCH), wxT("Find item"));
        toolBar->AddTool(ID_NUTOS_HELP, wxT("Help"), wxBITMAP(TBB_HELP), wxT("Show help"));
    }
    /* Load large toolbar buttons. */
    else {
        toolBar->AddTool(wxID_OPEN, wxT("Open"), wxBITMAP(TBB_OPEN_LARGE), wxT("Load configuration"));
        toolBar->AddTool(wxID_SAVE, wxT("Save"), wxBITMAP(TBB_SAVE_LARGE), wxT("Save configuration"));
        toolBar->AddTool(ID_BUILD_LIBRARY, wxT("Build"), wxBITMAP(TBB_BUILDLIBRARY_LARGE), wxT("Build Nut/OS"));
        toolBar->AddTool(wxID_FIND, wxT("Search"), wxBITMAP(TBB_SEARCH_LARGE), wxT("Find item"));
        toolBar->AddTool(ID_NUTOS_HELP, wxT("Help"), wxBITMAP(TBB_HELP_LARGE), wxT("Show help"));
        toolBar->SetToolBitmapSize(wxSize(32, 32));
    }
    toolBar->Realize();

    toolBar->SetHelpText(wxT("The toolbar allows quick access to commonly-used commands."));
}

void CMainFrame::CreateNutStatusBar()
{
    CreateStatusBar(4, wxST_SIZEGRIP);
    int widths[] = { -1, 100, 40, 80 };
    SetStatusWidths(4, widths);
    SetStatusText(wxT("Ready"));
}

void CMainFrame::CreateNutWindows()
{
    int w;
    int h;

    Show();
    /*
     * Create the bottom sash. Its client is a multiline text control,
     * which acts as a log output window. 
     */
    m_outputSashWindow = new wxSashLayoutWindow(this, ID_OUTPUT_SASH_WINDOW);
    m_outputSashWindow->SetOrientation(wxLAYOUT_HORIZONTAL);
    m_outputSashWindow->SetAlignment(wxLAYOUT_BOTTOM);
    m_outputSashWindow->SetSashVisible(wxSASH_TOP, true);

    m_outputWindow = new wxTextCtrl(m_outputSashWindow, ID_OUTPUT_WINDOW, wxEmptyString,
        wxDefaultPosition, wxDefaultSize, 
        wxTE_MULTILINE | wxCLIP_CHILDREN | wxTE_READONLY | wxTE_RICH);
    wxLog *old_log = wxLog::SetActiveTarget(new wxLogTextCtrl(m_outputWindow));
    delete old_log;

    wxLogMessage(wxT("Nut/OS Configurator Version %s"), wxT(VERSION));
    wxLogMessage(wxT("Linked to %s"), wxVERSION_STRING);
    wxLogMessage(wxT("Running on %s"), wxGetOsDescription().c_str());
    wxLogMessage(wxT("Working in %s"), ::wxGetCwd().c_str());
    m_outputWindow->ShowPosition(m_outputWindow->GetLastPosition());

    /*
     * Create the left sash for the configuration tree.
     */
    wxLogVerbose(wxT("Create ConfigSash"));
    m_configSashWindow = new wxSashLayoutWindow(this, ID_CONFIG_SASH_WINDOW);
    m_configSashWindow->SetOrientation(wxLAYOUT_VERTICAL);
    m_configSashWindow->SetAlignment(wxLAYOUT_LEFT);
    m_configSashWindow->SetSashVisible(wxSASH_RIGHT, true);

    /*
     * Create the top right sash. Its client is a property list.
     */
    wxLogVerbose(wxT("Create PropertySash"));
    m_propertiesSashWindow = new wxSashLayoutWindow(this, ID_PROPERTIES_SASH_WINDOW);
    m_propertiesSashWindow->SetOrientation(wxLAYOUT_HORIZONTAL);
    m_propertiesSashWindow->SetAlignment(wxLAYOUT_TOP);
    m_propertiesSashWindow->SetSashVisible(wxSASH_BOTTOM, true);

    wxLogVerbose(wxT("Create CPropertyList"));
    m_propertyListWindow = new CPropertyList(m_propertiesSashWindow, ID_PROPERTIES_WINDOW,
                                             wxDefaultPosition, wxDefaultSize,
                                             wxLC_REPORT | wxCLIP_CHILDREN | wxLC_VRULES | wxLC_HRULES | wxSUNKEN_BORDER);

    /*
     * Create the bottom right sash. Its client is a multiline
     * text control to display the description of a selected
     * tree item.
     */
    wxLogVerbose(wxT("Create InfoSash"));
    m_infoSashWindow = new wxSashLayoutWindow(this, ID_SHORT_DESCR_SASH_WINDOW);
    m_infoSashWindow->SetOrientation(wxLAYOUT_HORIZONTAL);
    m_infoSashWindow->SetAlignment(wxLAYOUT_TOP);

    wxLogVerbose(wxT("Create CInfoWindow"));
    m_infoWindow =
        new CInfoWindow(m_infoSashWindow, ID_SHORT_DESCR_WINDOW, wxDefaultPosition, wxDefaultSize,
                        wxTE_MULTILINE | wxCLIP_CHILDREN | wxTE_READONLY);

    /* Create the configuration tree control. */
    m_treeCtrl = new CConfTreeCtrl(m_configSashWindow, ID_CONFTREE_CTRL);

    wxConfigBase *pConfig = wxConfigBase::Get();
    wxString lastPath = pConfig->GetPath();
    pConfig->SetPath(wxT("/MainFrame"));

    wxLogVerbose(wxT("Size ConfigSash"));
    w = pConfig->Read(wxT("tree_w"), 270);
    h = pConfig->Read(wxT("tree_h"), 190);
    m_configSashWindow->SetDefaultSize(wxSize(w > 50 ? w : 50, h > 50 ? h : 50));

    wxLogVerbose(wxT("Size PropertySash"));
    w = pConfig->Read(wxT("prop_w"), 300);
    h = pConfig->Read(wxT("prop_h"), 115);
    m_propertiesSashWindow->SetDefaultSize(wxSize(w > 50 ? w : 50, h > 50 ? h : 50));

    wxLogVerbose(wxT("Size InfoSash"));
    w = pConfig->Read(wxT("desc_w"), 300);
    h = pConfig->Read(wxT("desc_h"), 75);
    m_infoSashWindow->SetDefaultSize(wxSize(w > 50 ? w : 50, h > 50 ? h : 50));

    wxLogVerbose(wxT("Size OutputSash"));
    w = pConfig->Read(wxT("outp_w"), 560);
    h = pConfig->Read(wxT("outp_h"), 90);
    m_outputSashWindow->SetDefaultSize(wxSize(w > 50 ? w : 50, h > 50 ? h : 50));

    pConfig->SetPath(lastPath);
    wxLogVerbose(wxT("+++++++++++++++ Windows created +++++++++++++++++"));
}

wxTextCtrl *CMainFrame::GetOutputWindow() const
{
    return m_outputWindow;
}

CConfTreeCtrl *CMainFrame::GetTreeCtrl() const
{
    return m_treeCtrl;
}

CInfoWindow *CMainFrame::GetInfoWindow() const
{
    return m_infoWindow;
}

CPropertyList *CMainFrame::GetPropertyListWindow() const
{
    return m_propertyListWindow;
}

CFindDialog *CMainFrame::GetFindDialog() const
{ 
    return m_findDlg; 
}

void CMainFrame::SetFindDialog(CFindDialog * dlg)
{ 
    m_findDlg = dlg;
}

void CMainFrame::OnSize(wxSizeEvent & WXUNUSED(event))
{
    wxLayoutAlgorithm layout;
    layout.LayoutFrame(this);
}

/*!
 * \brief Process sash drag event.
 *
 * 
 */
void CMainFrame::OnSashDrag(wxSashEvent & event)
{
    if (event.GetDragStatus() == wxSASH_STATUS_OUT_OF_RANGE)
        return;

    switch (event.GetId()) {
    case ID_CONFIG_SASH_WINDOW:
        /* Vertical sash dragged. */
        m_configSashWindow->SetDefaultSize(wxSize(event.GetDragRect().width, 2000));
        break;
    case ID_PROPERTIES_SASH_WINDOW:
        /* Right horizontal sash dragged. */
        m_propertiesSashWindow->SetDefaultSize(wxSize(2000, event.GetDragRect().height));
        break;
    case ID_SHORT_DESCR_SASH_WINDOW:
        m_infoSashWindow->SetDefaultSize(wxSize(2000, event.GetDragRect().height));
        break;
    case ID_OUTPUT_SASH_WINDOW:
        /* Lower horizontal sash dragged. */
        m_outputSashWindow->SetDefaultSize(wxSize(2000, event.GetDragRect().height));
        break;
    }

    wxLayoutAlgorithm layout;
    if (!layout.LayoutFrame(this)) {
        wxNode *node = (wxNode *) GetChildren().GetFirst();
        while (node) {
            wxWindow *win = (wxWindow *) node->GetData();
            if (win->IsKindOf(CLASSINFO(wxSashLayoutWindow))) {
                wxSashLayoutWindow *sashWin = (wxSashLayoutWindow *) win;
                wxSize sz = sashWin->GetSize();
                sashWin->SetDefaultSize(sz);
            }
            node = node->GetNext();
        }
    }
}

/* ------------------------- Menu Events ------------------------- */

void CMainFrame::OnQuit(wxCommandEvent & WXUNUSED(event))
{
    Close();
}

void CMainFrame::OnSettings(wxCommandEvent& WXUNUSED(event))
{
    CSettingsDialog dialog(this);
    dialog.ShowModal();
}

void CMainFrame::OnFind(wxCommandEvent& WXUNUSED(event))
{
    if (m_findDlg) {
        m_findDlg->Raise();
    }
    else {
        int style = wxFR_NOUPDOWN;
        int flags = wxFR_DOWN;
        bool val;

        wxConfigBase *pConfig = wxConfigBase::Get();
        if (pConfig) {
            wxString lastPath = pConfig->GetPath();
            pConfig->SetPath(wxT("/FindDialog"));

            pConfig->Read(wxT("case"), &val, false);
            if (val) {
                flags |= wxFR_MATCHCASE;
            }
            pConfig->Read(wxT("word"), &val, false);
            if (val) {
                flags |= wxFR_WHOLEWORD;
            }

            pConfig->SetPath(lastPath);
        }

#if 0
        ctFindReplaceDialog::sm_findData.SetFlags(flags);
#endif
        m_findDlg = new CFindDialog(this, wxT("Find text"), style);
        m_findDlg->Show(true);
    }
}

void CMainFrame::OnGenerateBuildTree(wxCommandEvent & WXUNUSED(event))
{
    CNutConfDoc *doc = wxGetApp().GetNutConfDoc();
    CSettings *cfg = wxGetApp().GetSettings();
    wxString str;
    
    str += wxT("\nSource directory: ");
    str += cfg->m_source_dir;
    str += wxT("\nBuild directory: ");
    str += cfg->m_buildpath;
    str += wxT("\nTarget platform: ");
    str += cfg->m_platform;
    str += wxT("\n\nDo you like to generate this build tree?\n");
    if(wxMessageBox(str, wxT("Generate Build"), wxOK | wxCANCEL | wxICON_QUESTION, this) == wxOK) {
        if (doc) {
            doc->GenerateBuildTree();
        }
    }
}

/*!
 * \brief Building Nut/OS event.
 */
void CMainFrame::OnBuildNutOS(wxCommandEvent & WXUNUSED(event))
{
    CSettings *cfg = wxGetApp().GetSettings();
    wxString str;

    wxString lib_dir(cfg->m_lib_dir);
    if(lib_dir.IsEmpty()) {
        lib_dir = cfg->m_buildpath + wxT("/lib");
    }
    
    /*
     * Create a message box containing all relevant information and
     * let the user decide if he really wants that.
     */
    str += wxT("\nSource directory: ");
    str += cfg->m_source_dir;
    str += wxT("\nBuild directory: ");
    str += cfg->m_buildpath;
    str += wxT("\nTarget platform: ");
    str += cfg->m_platform;
    str += wxT("\nInstall directory: ");
    str += lib_dir;
    str += wxT("\n\nDo you like to build the Nut/OS libraries?\n");
    if(wxMessageBox(str, wxT("Build Nut/OS"), wxOK | wxCANCEL | wxICON_QUESTION, this) == wxOK) {
        /* Clean up any previous build. */
        if(!wxGetApp().Build(wxT("clean"))) {
            if(wxMessageBox(wxT("Cleaning build tree failed! Continue?"), wxT("Build"), 
                wxYES_NO | wxNO_DEFAULT | wxICON_HAND) != wxYES) {
                return;
            }
        }
        /* Start the new build. */
        if(!wxGetApp().Build()) {
            if(wxMessageBox(wxT("Building Nut/OS failed! Continue?"), wxT("Build"), 
                wxYES_NO | wxNO_DEFAULT | wxICON_HAND) != wxYES) {
                return;
            }
        }
        /* Install new libraries. */
        if(!wxGetApp().Build(wxT("install"))) {
            wxMessageBox(wxT("Installing Nut/OS failed!"), wxT("Build"), wxCANCEL | wxICON_HAND);
        }
    }
}

void CMainFrame::OnBuildLibraries(wxCommandEvent &event)
{
    OnGenerateBuildTree(event);
    OnBuildNutOS(event);
}

void CMainFrame::OnCreateSampleDir(wxCommandEvent & WXUNUSED(event))
{
    CNutConfDoc *doc = wxGetApp().GetNutConfDoc();
    CSettings *cfg = wxGetApp().GetSettings();
    wxString app_source = cfg->m_source_dir + wxT("/app");
    wxString str;
    
    str += wxT("\nSource directory: ");
    str += app_source;
    str += wxT("\nTarget directory: ");
    str += cfg->m_app_dir;
    str += wxT("\nProgrammer ");
    str += cfg->m_programmer;
    str += wxT("\n\nDo you like to create or update the sample directory?\n");
    if(wxMessageBox(str, wxT("Creating samples"), wxOK | wxCANCEL | wxICON_QUESTION, this) == wxOK) {
        if (doc) {
            doc->GenerateApplicationTree();
        }
    }
}

void CMainFrame::OnToggleToolbar(wxCommandEvent& WXUNUSED(event))
{
    wxToolBar *tbar = GetToolBar();

    if (tbar == NULL) {
        CreateNutToolBar();
    }
    else {
        delete tbar;

        SetToolBar(NULL);
    }
}

void CMainFrame::OnToggleToolbarSize(wxCommandEvent& WXUNUSED(event))
{
    wxToolBar *tbar = GetToolBar();

    if (tbar) {
        delete tbar;
        SetToolBar(NULL);
    }
    m_smallToolbar = !m_smallToolbar;
    CreateNutToolBar();
}

void CMainFrame::OnHelp(wxCommandEvent& WXUNUSED(event))
{
    m_help.DisplayContents();
}

void CMainFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	wxString message(wxT("Nut/OS Configurator "));
	message += wxT(VERSION);
    wxMessageBox(message, wxT("About"), wxOK | wxICON_INFORMATION, this);
}

/* Item doubleclicked. */
void CMainFrame::OnItemActivated(wxDataViewEvent& event)
{
    wxDataViewItem item(event.GetItem());
}

/* Item clicked. */
void CMainFrame::OnSelectionChanged(wxDataViewEvent& event)
{
    wxDataViewItem item(event.GetItem());

    CNutConfDoc *doc = wxGetApp().GetNutConfDoc();
    if (doc) {
        CNutConfHint hint(NULL, nutSelChanged);
        doc->UpdateAllViews(NULL, &hint);
        if (GetScriptStatus()) {
            wxLogMessage(wxT("%s"), GetScriptErrorString());
        }
    }
}

void CMainFrame::OnItemValueChanged(wxDataViewEvent& WXUNUSED(event))
{
}
