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

#include "nutconf.h"
#include "stateicontext.h"
#include "conftreenode.h"

#ifdef __WXMSW__
#define strcasecmp _stricmp
#define strdup _strdup
#else
#include "bitmaps/closedfolder.xpm"
#include "bitmaps/library.xpm"
#include "bitmaps/module.xpm"
#include "bitmaps/radiooff.xpm"
#include "bitmaps/radioon.xpm"
#include "bitmaps/unchecked.xpm"
#include "bitmaps/checked.xpm"
#include "bitmaps/enumerated.xpm"
#include "bitmaps/integer.xpm"
#include "bitmaps/text.xpm"
#endif

CConfTreeNode::CConfTreeNode(CConfTreeNode* parent, CNutConfDoc *doc, NUTCOMPONENT *compo)
{
    m_parent = parent;
    m_configDoc = doc;
    m_compo = compo;
    m_option = NULL;

    m_optionType = nutOptionTypeNone;
    m_optionFlavor = nutFlavorNone;
    m_configType = nutConfigTypeNone;
    m_optionBoolFlavor = nutBoolNone;

    if (m_parent == NULL || m_compo == NULL) {
        m_configType = nutFolder;
        m_container = true;
        m_brief = m_configDoc->GetBriefDescription(compo);
        m_icon = wxICON(closedfolder);
    }
    else {
        if (m_parent->m_parent == NULL) {
            m_configType = nutLibrary;
            m_icon = wxICON(library);
        } else {
            m_configType = nutModule;
            m_icon = wxICON(module);
        }
        m_container = m_compo->nc_child || m_compo->nc_opts;
    }
#ifdef __WXMSW__
    m_icon.SetSize(16, 16);
#endif
    m_brief = m_configDoc->GetBriefDescription(m_compo);
}

CConfTreeNode::CConfTreeNode(CConfTreeNode* parent, CNutConfDoc *doc, NUTCOMPONENTOPTION *option)
{
    m_parent = parent;
    m_configDoc = doc;
    m_container = false;
    m_compo = NULL;
    m_option = option;

    m_optionType = m_configDoc->GetOptionType(m_option);
    m_configType = nutOption;
    m_optionFlavor = m_configDoc->GetOptionFlavor(m_option);
    m_optionBoolFlavor = nutBoolNone;
    if (HasBool()) {
        if (m_option->nco_exclusivity) {
            m_optionBoolFlavor = nutBoolRadio;
            m_icon_inactive = wxICON(radiooff);
            m_icon = wxICON(radioon);
        } else {
            m_icon_inactive = wxICON(unchecked);
            m_icon = wxICON(checked);
        }
    } else {
        switch (m_optionType) {
        case nutEnumerated:
            m_icon = wxICON(enumerated);
            break;
        case nutInteger:
            m_icon = wxICON(integer);
            break;
        default:
            m_icon = wxICON(text);
            break;
        }
    }

    m_value_str = m_configDoc->GetOptionValue(m_option);
#ifdef __WXMSW__
    m_icon.SetSize(16, 16);
    m_icon_inactive.SetSize(16, 16);
#endif
    m_brief = m_configDoc->GetBriefDescription(m_option);
}

CConfTreeNode::~CConfTreeNode()
{
}

wxString CConfTreeNode::GetFilename() const
{
    wxString str;

    if (m_option) {
        char *fname = GetOptionFile(m_configDoc->GetRepository(), m_option->nco_compo, m_option->nco_name);
        if (fname) {
            str = wxString(fname, wxConvLocal);
            free(fname);
        }
    }
    return str;
}

wxString CConfTreeNode::GetMacro() const
{
    if (m_option && m_option->nco_name) {
        return wxString(m_option->nco_name, wxConvLocal);
    }
    return wxEmptyString;
}

wxString CConfTreeNode::GetRequirementList() const
{
    wxString str;

    if (m_option) {
        char **requires = GetOptionRequirements(m_configDoc->GetRepository(), m_option->nco_compo, m_option->nco_name);
        if (requires) {
            for (int i = 0; requires[i]; i++) {
                if(!str.IsEmpty()) {
                    str += wxT(", ");
                }
                str += wxString(requires[i], wxConvLocal);
                free(requires[i]);
            }
            free(requires);
        }
    }
    /* Do we need component requirements? */
    if (m_compo) {
        char **requires = GetComponentRequirements(m_configDoc->GetRepository(), m_compo);
        if (requires) {
            for (int i = 0; requires[i]; i++) {
                if(!str.IsEmpty()) {
                    str += wxT(", ");
                }
                str += wxString(requires[i],wxConvLocal);
                free(requires[i]);
            }
            free(requires);
        }
    }
    return str;
}

wxString CConfTreeNode::GetProvisionList() const
{
    wxString str;
    char **provides;

    if (m_option) {
        provides = GetOptionProvisions(m_configDoc->GetRepository(), m_option->nco_compo, m_option->nco_name);
        if (provides) {
            for (int i = 0; provides[i]; i++) {
                if(!str.IsEmpty()) {
                    str += wxT(", ");
                }
                str += wxString(provides[i],wxConvLocal);
                free(provides[i]);
            }
            free(provides);
        }
    }
    if (m_compo) {
        provides = GetComponentProvisions(m_configDoc->GetRepository(), m_compo);
        if (provides) {
            for (int i = 0; provides[i]; i++) {
                if(!str.IsEmpty()) {
                    str += wxT(", ");
                }
                str += wxString(provides[i],wxConvLocal);
                free(provides[i]);
            }
            free(provides);
        }
    }
    return str;
}

wxString CConfTreeNode::GetExclusivityList() const
{
    wxString str;

    if (m_option && m_option->nco_exclusivity) {
        for (int i = 0; m_option->nco_exclusivity[i]; i++) {
            if(!str.IsEmpty()) {
                str += wxT(", ");
            }
            str += wxString(m_option->nco_exclusivity[i], wxConvLocal);
        }
    }
    if (m_compo && m_compo->nc_exclusivity) {
        for (int i = 0; m_compo->nc_exclusivity[i]; i++) {
            if(!str.IsEmpty()) {
                str += wxT(", ");
            }
            str += wxString(m_compo->nc_exclusivity[i], wxConvLocal);
        }
    }
    return str;
}

wxString CConfTreeNode::GetOptionText() const
{
    return m_value_str;
}

int CConfTreeNode::GetEnumStrings(wxArrayString & arEnumStrings) const
{
    if (m_option) {
        return m_configDoc->GetEnumStrings(m_option, arEnumStrings);
    }
    return 0;
}

nutOptionType CConfTreeNode::GetOptionType() const
{
    return m_optionType;
}

/*!
 * \brief Returns type of this item.
 *
 * \return Either \ref nutConfigTypeNone, \ref nutFolder, \ref nutLibrary, 
 *         \ref nutModule or \ref nutOption.
 */
nutComponentType CConfTreeNode::GetConfigType() const
{
    return m_configType;
}

nutOptionFlavor CConfTreeNode::GetOptionFlavor() const
{
    return m_optionFlavor;
}

wxVariant CConfTreeNode::GetStateIconText() const
{
    wxVariant var;

    if (IsActive()) {
        var << CStateIconText(m_brief, m_icon, IsEnabled());
    } else {
        var << CStateIconText(m_brief, m_icon_inactive, IsEnabled());
    }

    return var;
}

wxString CConfTreeNode::GetDescription() const
{
    wxString str;

    if (m_compo) {
        str = m_configDoc->GetDescription(m_compo);
    }
    if (m_option) {
        str = m_configDoc->GetDescription(m_option);
    }
    return str;
}

/*!
 * \brief Check if this is a boolean item.
 */
bool CConfTreeNode::HasBool() const
{
    return m_optionFlavor == nutFlavorBool || m_optionFlavor == nutFlavorBoolData;
}

/*!
 * \brief Check if this item is active.
 *
 * An option is activated by the user or automatically by active_if.
 * Only options may become incative, components are always active.
 */
bool CConfTreeNode::IsActive() const
{
    if (m_option) {
        if (HasBool()) {
            if(m_option->nco_active == 0) {
                if(m_option->nco_active_if) {
                    m_option->nco_active = m_configDoc->IsOptionActive(m_option->nco_active_if);
                }
            }
            return m_option->nco_active != 0;
        }
        m_option->nco_active = 1;
    }
    return true;
}

/*!
 * \brief Check if this item is enabled.
 *
 * An item is disabled, if it requires other items being activated.
 */
bool CConfTreeNode::IsEnabled() const
{
    if (m_option) {
        return m_option->nco_enabled != 0;
    }
    if (m_compo) {
        return m_compo->nc_enabled != 0;
    }
    return true;
}

/*!
 * \brief Set boolean value of this item.
 */
void CConfTreeNode::SetActive(bool ena)
{
    if (m_option && (m_option->nco_active != 0) != ena) {
        m_option->nco_active = ena;
        if (ena && m_option->nco_value == NULL) {
            m_option->nco_value = strdup((char*)wxT(""));
        }
        m_configDoc->ActiveStateChanged();
    }
}

/*!
 * \brief Toggle the node's active state.
 */
void CConfTreeNode::ToggleItem()
{
    if (m_configType != nutOption)
        return;

    switch (m_optionFlavor) {
    case nutFlavorBool:
    case nutFlavorBoolData:
        if (IsEnabled()) {
            if (m_option && m_option->nco_exclusivity) {
                if(!IsActive()) {
                    m_configDoc->DeactivateOptionList(m_option->nco_compo, m_option->nco_exclusivity);
                    SetActive(true);
                }
            }
            else {
                SetActive(!IsActive());
            }
        }
    default:
        break;
    }
}

bool CConfTreeNode::SetValue(const wxString & strValue)
{
    if (m_option) {
        char *newval = strdup(strValue.mb_str());

        /* Check if edited value changed. */
        if (m_option->nco_value == NULL || strcmp(m_option->nco_value, newval)) {
            /* Remove any previously edited value. */
            if (m_option->nco_value) {
                free(m_option->nco_value);
                m_option->nco_value = NULL;
            }
            /* Check if new value differs from configured value. */
            char *cfgval = GetConfigValue(m_configDoc->GetRepository(), m_option->nco_name);
            if ((cfgval == NULL && *newval) || (cfgval && strcmp(cfgval, newval))) {
                m_option->nco_value = newval;
                m_option->nco_active = 1;
            } else {
                free(newval);
            }
            free(cfgval);
#if 0
            CNutConfHint hint(&item, nutValueChanged);
            UpdateAllViews(NULL, &hint);
#endif
        } else {
            free(newval);
        }
        m_value_str = strValue;
        m_configDoc->OptionValueChanged();
    }
    return true;
}

/*!
 * \brief Launch external file viewer.
 */
bool CConfTreeNode::ViewHeaderFile() const
{
    bool rc = false;
    wxString strFile(GetFilename());

    if (!strFile.IsEmpty()) {
        if (m_configDoc->GetBuildTree().IsEmpty()) {
            wxMessageBox(wxT("Cannot display header file until configuration is saved"));
        } else {
            strFile.Prepend(wxT("/"));
            strFile.Prepend(m_configDoc->GetBuildTree());
            //rc = wxGetApp().Launch(strFile, "notepad.exe");
            rc = wxGetApp().Launch(strFile, wxEmptyString);
        }
    }
    return rc;
}


bool CConfTreeNode::IsContainer() const
{
    return m_container;
}

CConfTreeNode* CConfTreeNode::GetParent()
{
    return m_parent;
}

CConfTreeNodePtrArray& CConfTreeNode::GetChildren()
{
    return m_children;
}

CConfTreeNode* CConfTreeNode::GetNthChild(unsigned int n)
{
    return m_children.Item(n);
}

void CConfTreeNode::Insert(CConfTreeNode* child, unsigned int n)
{
    m_children.Insert(child, n);
}

void CConfTreeNode::Append(CConfTreeNode* child)
{
    m_children.Add(child);
}

unsigned int CConfTreeNode::GetChildCount() const
{
    return m_children.GetCount();
}
