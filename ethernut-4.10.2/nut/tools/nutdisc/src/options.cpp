#include <wx/config.h>

#include "discovery.h"
#include "options.h"

COptions *g_options;

COptions::COptions()
{
    wxConfigBase *pConfig = wxConfigBase::Get();
    pConfig->SetPath(wxT("/Options"));
    m_scanip = pConfig->Read(wxT("ScanAddress"), wxT("255.255.255.255"));
    m_scanport = pConfig->Read(wxT("ScanPort"), wxString::Format(wxT("%u"), DISCOVERY_PORT));
    m_scantime = pConfig->Read(wxT("ScanTime"), wxT("5"));
}

void COptions::Save()
{
    wxConfigBase *pConfig = wxConfigBase::Get();
    if (pConfig) {
        pConfig->Write(wxT("/Options/ScanAddress"),  g_options->m_scanip);
        pConfig->Write(wxT("/Options/ScanPort"),  g_options->m_scanport);
        pConfig->Write(wxT("/Options/ScanTime"),  g_options->m_scantime);
    }
}

COptionsDialog::COptionsDialog(wxWindow *parent)
    : wxDialog(parent, wxID_ANY, wxT("Options"), wxDefaultPosition, wxDefaultSize, 
        wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
    wxGridSizer *gridsizer = new wxGridSizer(2, 5, 5);

    wxSizerFlags flagsLabel(0);
    flagsLabel.Border(wxALL, 5);

    wxSizerFlags flagsEntry(1);
    flagsEntry.Border(wxALL, 5);

    m_scanip_entry = new wxTextCtrl(this, wxID_ANY, g_options->m_scanip);
    wxTextValidator scanip_valid(wxFILTER_EMPTY | wxFILTER_INCLUDE_CHAR_LIST, &g_options->m_scanip);
    scanip_valid.SetCharIncludes(".0123456789");
    m_scanip_entry->SetValidator(scanip_valid);

    m_scanport_entry = new wxTextCtrl(this, wxID_ANY, g_options->m_scanport);
    wxTextValidator scanport_valid(wxFILTER_EMPTY | wxFILTER_DIGITS, &g_options->m_scanport);
    m_scanport_entry->SetValidator(scanport_valid);

    m_scantime_entry = new wxTextCtrl(this, wxID_ANY, g_options->m_scantime);
    wxTextValidator scantime_valid(wxFILTER_EMPTY | wxFILTER_DIGITS , &g_options->m_scantime);
    m_scantime_entry->SetValidator(scantime_valid);

    gridsizer->Add(new wxStaticText(this, wxID_ANY, wxT("Scan IP Address")), flagsLabel);
    gridsizer->Add(m_scanip_entry, flagsEntry);

    gridsizer->Add(new wxStaticText(this, wxID_ANY, wxT("Scan UDP Port")), flagsLabel);
    gridsizer->Add(m_scanport_entry, flagsEntry);

    gridsizer->Add(new wxStaticText(this, wxID_ANY, wxT("Scan Timer (seconds)")), flagsLabel);
    gridsizer->Add(m_scantime_entry, flagsEntry);

    wxStdDialogButtonSizer *btn = new wxStdDialogButtonSizer();
    wxButton *okBtn = new wxButton(this, wxID_OK);
    okBtn->SetDefault();
    btn->AddButton(okBtn);
    btn->AddButton(new wxButton(this, wxID_CANCEL));
    btn->Realize();

    wxBoxSizer *mainsizer = new wxBoxSizer( wxVERTICAL );
    mainsizer->Add(gridsizer, 1, wxGROW | wxALL, 10);
    mainsizer->Add(btn, 0, wxGROW | wxALL, 10);
    SetSizer(mainsizer);
    mainsizer->SetSizeHints(this);

    /*
     * Restore frame position and size.
     */
    wxConfigBase *pConfig = wxConfigBase::Get();
    pConfig->SetPath(wxT("/OptionsFrame"));
    Move(pConfig->Read(wxT("x"), 50), pConfig->Read(wxT("y"), 50));
    SetClientSize(pConfig->Read(wxT("w"), 350), pConfig->Read(wxT("h"), 200));
}

/*!
 * \brief Destructor.
 */
COptionsDialog::~COptionsDialog()
{
    /*
     * Save frame size and position.
     */
    wxConfigBase *pConfig = wxConfigBase::Get();
    if (pConfig) {
        int x, y;
        GetPosition(&x, &y);
        pConfig->Write(wxT("/OptionsFrame/x"), (long) x);
        pConfig->Write(wxT("/OptionsFrame/y"), (long) y);
        int w, h;
        GetClientSize(&w, &h);
        pConfig->Write(wxT("/OptionsFrame/w"), (long) w);
        pConfig->Write(wxT("/OptionsFrame/h"), (long) h);
    }
}
