/* ----------------------------------------------------------------------------
 * Copyright (C) 2008-2012 by egnite GmbH
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
 * $Id: nutconf.cpp 3878 2012-02-15 15:46:22Z haraldkipp $
 */

#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/app.h>
#include <wx/frame.h>
#include <wx/listbox.h>
#endif

#include <wx/notebook.h>
#include <wx/config.h>
#include <wx/mimetype.h>
#include <wx/cmdline.h>
#include <wx/image.h>
#include <wx/filename.h>
#include <wx/splash.h>
#include <wx/bitmap.h>
#include <wx/busyinfo.h>

#include <wx/filesys.h>
#include <wx/fs_zip.h>


#include "mainframe.h"
#include "nutconf.h"

#if !defined(__WXMSW__)
#include "bitmaps/nutconf.xpm"
#endif

IMPLEMENT_APP(NutConfApp);

/*!
 * \brief Application main entry.
 */
bool NutConfApp::OnInit()
{
    m_docManager = NULL;
    m_currentDoc = NULL;
    wxString settings_name(wxT("NutConf"));
    wxString settings_ext;

    static const wxCmdLineEntryDesc cmdLineDesc[] =
    {
        { wxCMD_LINE_SWITCH, "v", "verbose", "be verbose", wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL },
        { wxCMD_LINE_SWITCH, "h", "help", "show usage", wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
        { wxCMD_LINE_OPTION, "s", "settings", "use alternate settings", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_NEEDS_SEPARATOR },
        { wxCMD_LINE_NONE }
    };

    wxCmdLineParser parser(cmdLineDesc, argc, argv);
    parser.SetLogo(_T("Nut/OS Configurator\n") _T(VERSION)
                      _T("Copyright (c) 2008-2012 by egnite GmbH\n")
                      _T("Copyright (c) 2004-2007 by egnite Software GmbH\n")
                      _T("Copyright (C) 1998, 1999, 2000 Red Hat, Inc."));
    if(parser.Parse()) {
        return false;
    }
    if(parser.Found(wxT("v"))) {
        wxLog::GetActiveTarget()->SetVerbose();
    }
    else {
        wxLog::GetActiveTarget()->SetVerbose(false);
    }

    wxImage::AddHandler(new wxGIFHandler);
    wxImage::AddHandler(new wxPNGHandler);
    wxFileSystem::AddHandler(new wxZipFSHandler);

    /*
     * Load settings early.
     */
    SetVendorName(wxT("egnite"));
    if(parser.Found(wxT("s"), &settings_ext)) {
        settings_name += settings_ext;
    }
    SetAppName(settings_name);
    m_settings = new CSettings();
    m_settings->m_topdir = ::wxGetCwd();

    /*
     * Create a wxConfig object early.
     */
    argc = 1;
    if (!wxApp::OnInit())
        return false;

    wxGetEnv(wxT("PATH"), &m_initialPath);

    /*
     * The document manager will handle non application specific menu commands.
     */
    m_docManager = new wxDocManager;

    /*
     * The document template defines the relationship between document and view.
     */
    new wxDocTemplate(m_docManager,     /* Manages this object. */
        wxT("Nut/OS Configuration"),    /* Displayed in the file selector. */
        wxT("*.conf"),                  /* File filter. */
        wxT(""),                        /* Default directory. */
        wxT("conf"),                    /* Default file extension. */
        wxT("NutconfDoc"),              /* Unique document type name. */
        wxT("NutconfView"),             /* Unique view type name. */
        CLASSINFO(CNutConfDoc),         /* Document class info. */
        CLASSINFO(CNutConfView));       /* View class info. */
    m_docManager->SetMaxDocsOpen(1);

    m_mainFrame = new CMainFrame(m_docManager, wxT("Nut/OS Configurator"));

    SetTopWindow(m_mainFrame);
    m_mainFrame->Show();

    if (!m_mainFrame->GetHelpController().Initialize(wxT("nutoshelp"))) {
        wxLogMessage(wxT("Failed to load help file"));
    }

    /* 
     * Detect directory change (version upgrade). 
     */
    wxString initWork = m_settings->FindAbsoluteDir(wxT("conf/repository.nut"));
    if (!initWork.IsEmpty()) {
        m_docManager->SetLastDirectory(initWork + wxT("/conf"));
        if (::wxIsAbsolutePath(m_settings->m_configname)) {
            if (!m_settings->m_configname.Lower().StartsWith(initWork.Lower())) {
                if (wxMessageBox(wxT("Configuration path has changed.\n\nOld path was ") +
                    wxPathOnly(m_settings->m_configname) + wxT("\nNew path is ") + initWork + wxT("/conf\n\nSwitch to new path?"), 
                    wxT("Path Change"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION) == wxYES) {
                        m_settings->m_configname = m_settings->m_relsrcpath + wxString(wxT("/conf/")) + m_settings->m_configname.AfterLast('/');
                        m_settings->m_repositoryname = m_settings->m_repositoryname_default;
                        m_settings->Save(m_settings->m_configname);
                }
            }
        }
        if (::wxIsAbsolutePath(m_settings->m_repositoryname)) {
            if (!m_settings->m_repositoryname.StartsWith(initWork)) {
                if (wxMessageBox(wxT("Repository path has changed. Do you want to use relative paths?"), 
                    wxT("Path Change"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION) == wxYES) {
                        m_settings->m_repositoryname = m_settings->m_repositoryname_default;
                        m_settings->Save(m_settings->m_configname);
                }
            }
        }
    }

    /*
     * Splash display.
     */
    wxSplashScreen* splash = new wxSplashScreen(
          wxBITMAP(SSB_NUTCONF),
          wxSPLASH_CENTRE_ON_PARENT | wxSPLASH_TIMEOUT,
          2000, m_mainFrame, wxID_ANY, wxDefaultPosition, wxDefaultSize,
          wxSIMPLE_BORDER|wxSTAY_ON_TOP);

    /*
     * Create the document. 
     */
    m_docManager->CreateDocument(m_settings->m_configname, 0);
#ifdef __WXMSW__
    if(splash) {
        splash->Destroy();
    }
#endif
    return true;
}

int NutConfApp::OnExit()
{
    delete wxConfigBase::Set((wxConfigBase *) NULL);
    if(m_docManager) {
        delete m_docManager;
    }
    if(m_settings) {
        delete m_settings;
    }
    return 0;
}

CNutConfDoc *NutConfApp::GetNutConfDoc() const
{
    if (m_currentDoc)
        return m_currentDoc;

    if (!m_docManager)
        return NULL;

    return wxDynamicCast(m_docManager->GetCurrentDocument(), CNutConfDoc);
}

void NutConfApp::SetStatusText(const wxString & text)
{
    CMainFrame *mainFrame = GetMainFrame();
    if (mainFrame) {
        mainFrame->GetStatusBar()->SetStatusText(text, 0);
#ifdef __WXMSW__
        ::UpdateWindow((HWND) mainFrame->GetHWND());
#endif
    }
}

CMainFrame *NutConfApp::GetMainFrame() const
{
    return m_mainFrame;
}

wxDocManager *NutConfApp::GetDocManager() const
{
    return m_docManager;
}

CSettings* NutConfApp::GetSettings() 
{ 
    return m_settings; 
}

bool NutConfApp::Launch(const wxString & strFileName, const wxString & strViewer)
{
    bool ok = false;
    wxString cmd;
    wxString filePath(strFileName);

    filePath.Replace(wxT("/"), wxString(wxFileName::GetPathSeparator()));

    if (!strViewer.IsEmpty()) {
        cmd = strViewer + wxString(wxT(" ")) + filePath;
    } else {
        wxString path, filename, ext;
         wxFileName::SplitPath(filePath, &path, &filename, &ext);

        wxFileType *ft = wxTheMimeTypesManager->GetFileTypeFromExtension(ext);
        if (ft == NULL) {
            ft = wxTheMimeTypesManager->GetFileTypeFromExtension(wxT(".txt"));
            if (ft == NULL) {
                return false;
            }
        }

        ok = ft->GetOpenCommand(&cmd, wxFileType::MessageParameters(filePath, wxT("")));
        delete ft;

        if (!ok) {
            ft = wxTheMimeTypesManager->GetFileTypeFromExtension(wxT(".txt"));
            ok = ft->GetOpenCommand(&cmd, wxFileType::MessageParameters(filePath, wxT("")));
            delete ft;
        }
    }

    if (ok) {
        ok = (wxExecute(cmd, false) != 0);
    }
    return ok;
}

bool NutConfApp::Build(const wxString &target)
{
    CNutConfDoc *doc = GetNutConfDoc();
    if(doc == NULL) {
        return false;
    }

    /*
     * Change working directory to the top build directory.
     */
    wxString initialCwd = wxFileName::GetCwd();
    if (::wxIsAbsolutePath(m_settings->m_buildpath)) {
        wxFileName::SetCwd(m_settings->m_buildpath);
    }
    else {
        wxFileName::SetCwd(m_settings->m_relsrcpath + wxString(wxT("/../")) + m_settings->m_buildpath);
    }

    /* Add tool directories to the PATH. */
#ifdef _WIN32
    wxString newPath(m_settings->m_toolpath);
    newPath.Replace(wxT("/"), wxT("\\"));
    newPath += wxT(";");
    newPath += m_initialPath;
    wxSetEnv(wxT("PATH"), newPath);
#endif

    /*
     * Assemble the command.
     */
    wxString cmd = wxT("make ") + target;

    /*
     * Start execution. stdout and stderr output is collected
     * in string arrays. Not very nice here, but works for now.
     */
    wxLogMessage(wxT("----- Running '%s' -----"), cmd.c_str());

#ifdef _WIN32
    wxPathList exePaths;
    wxString exePathName;

    exePaths.AddEnvList(wxT("PATH"));
    exePathName = exePaths.FindAbsoluteValidPath(_T("make.exe"));
    if (exePathName.IsEmpty()) {
        wxLogMessage(wxT("Make tool not found in ") + newPath);
    }
    wxLogMessage(wxT("Located ") + exePathName);
#endif

    wxBusyInfo wait(wxT("Please wait, running '") + cmd + wxT("'..."));
    wxArrayString output, errors;
    int code = wxExecute(cmd, output, errors);

    /*
     * Display collected output in the output window.
     */
    if(code != -1) {
        wxTextCtrl *outwin = wxGetApp().GetMainFrame()->GetOutputWindow();
        size_t i;
        size_t count;

        if(wxLog::GetVerbose()) {
            count = output.GetCount();
            for (i = 0; i < count; i++) {
                outwin->AppendText(output[i]);
                outwin->AppendText(wxT("\n"));
                outwin->ShowPosition(outwin->GetLastPosition());
            }
        }
        count = errors.GetCount();
        for (i = 0; i < count; i++) {
            outwin->AppendText(errors[i]);
            outwin->AppendText(wxT("\n"));
            outwin->ShowPosition(outwin->GetLastPosition());
        }
    }
    if(code) {
        wxLogMessage(wxT("----- '%s' failed with error %d -----"), cmd.c_str(), code);
    }
    else {
        wxLogMessage(wxT("----- '%s' terminated successfully -----"), cmd.c_str(), code);
    }

    /* 
     * Restore initial environment. 
     */
    wxSetEnv(wxT("PATH"), m_initialPath);
    wxFileName::SetCwd(initialCwd);

    return (code == 0);
}
