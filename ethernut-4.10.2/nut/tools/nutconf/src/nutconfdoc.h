#ifndef NUTCONFDOC_H_
#define NUTCONFDOC_H_

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
 * $Id: nutconfdoc.h 3878 2012-02-15 15:46:22Z haraldkipp $
 */

#include <wx/wx.h>
#include <wx/docview.h>

#include "nutconfview.h"
#include "nutcomponent.h"
#include "conftreemodel.h"

class CConfTreeModel;

/*!
 * \brief Configuration document class.
 */
class CNutConfDoc:public wxDocument {
    DECLARE_DYNAMIC_CLASS(CNutConfDoc)
  public:
    CNutConfDoc();
    ~CNutConfDoc();

    virtual bool OnCreate(const wxString & path, long flags);
    virtual bool OnOpenDocument(const wxString & filename);
    virtual bool OnSaveDocument(const wxString& filename);
    virtual bool OnNewDocument();
    virtual bool OnCloseDocument();

    bool ReadRepository(const wxString & repositoryname, const wxString & configname);
    void ReleaseRepository();
    NUTREPOSITORY * GetRepository();
    NUTCOMPONENT * GetRootComponent();
    wxString GetBriefDescription(NUTCOMPONENT * compo) const;
    wxString GetBriefDescription(NUTCOMPONENTOPTION* option) const;
    wxString GetDescription(NUTCOMPONENT * compo) const;
    wxString GetDescription(NUTCOMPONENTOPTION* option) const;

    nutOptionType GetOptionType(NUTCOMPONENTOPTION* option) const;
    nutOptionFlavor GetOptionFlavor(NUTCOMPONENTOPTION* option) const;
    wxString GetOptionValue(NUTCOMPONENTOPTION* option) const;
    int GetEnumStrings(NUTCOMPONENTOPTION* option, wxArrayString & arEnumStrings) const;

    void DeleteItems();
    void AddAllItems();

    wxList & GetItems();
    void ActiveStateChanged();
    void OptionValueChanged();

    wxString GetBuildTree();
    bool GenerateBuildTree();
    bool GenerateApplicationTree();

    wxString GetInstallDir();

    NUTCOMPONENTOPTION *FindOptionByName(NUTCOMPONENT * compo, char *name) const;
    bool IsOptionActive(char *name) const;
    void DeactivateOptionList(NUTCOMPONENT *compo, char **exlist);

  protected:
    NUTREPOSITORY *m_repository;
    NUTCOMPONENT *m_compo_root;
    wxList m_items;
    bool IsRequirementProvided(NUTCOMPONENT *compo, char *requirement);
    bool IsRequirementProvided(char *requirement);
    void SaveComponentOptions(FILE *fp, NUTCOMPONENT * compo);

private:
    CConfTreeModel *m_model;
};

#endif
