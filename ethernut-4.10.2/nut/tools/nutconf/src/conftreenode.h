#ifndef CONFTREENODE_H_
#define CONFTREENODE_H_

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

#include <wx/wx.h>
#include <wx/dataview.h>

#include "nutcomponent.h"

/*!
 * \brief Component types.
 *
 * The Nut/OS configuration is organized in a tree structure.
 */
enum nutComponentType {
    /*! \brief No specific type or type not yet assigned. */
    nutConfigTypeNone,

    /*! \brief Folder component. 
     *
     * A folder may contain any number of libraries. 
     * Seems to be currently unused.
     */
    nutFolder,

    /*! \brief Library component.
     *
     * A library may contain any number of modules or options.
     */
    nutLibrary,

    /*! \brief Module component. 
     * 
     * A module may contain any number of options. Modules are
     * typically related to source code files.
     */
    nutModule,

    /*! \brief Component option. 
     *
     * Each component option specifies a configuration value.
     */
    nutOption
};

/*!
 * \brief Option types.
 */
enum nutOptionType {
    /*! \brief No specific option. 
     *
     */
    nutOptionTypeNone,

    /*! \brief Integer value. 
     *
     * Option is used to configure an integer value.
     */
    nutInteger,

    /*! \brief Boolean value. 
     *
     * Option is used to configure a boolean value.
     */
    nutBool,

    /*! \brief String value. 
     *
     * Option is used to configure a textual value.
     */
    nutString,

    /*! \brief Enumerated value. 
     *
     * Used to select a configuration option from a predefined set.
     */
    nutEnumerated
};

/*!
 * \brief Option flavors.
 */
enum nutOptionFlavor {
    /*! \brief No specific flavor. 
     *
     */
    nutFlavorNone,

    /*! \brief Boolean flavor.
     *
     * Option may be explicitely enabled or disabled, either by a
     * check box or a radio button control.
     */
    nutFlavorBool,

    /*! \brief Boolean data flavor.
     *
     * Option may be explicitely enabled or disabled, either by a
     * check box or a radio button control. In addition this option
     * contains a data value, which is empty if the option is
     * disabled.
     */
    nutFlavorBoolData,

    /*! \brief Data flavor.
     *
     * This option contains a data value.
     */
    nutFlavorData
};

enum nutOptionBoolFlavor {
    nutBoolNone,
    nutBoolCheck,
    nutBoolRadio
};

class CNutConfDoc;
class CConfTreeNode;
WX_DEFINE_ARRAY_PTR(CConfTreeNode*, CConfTreeNodePtrArray);

class CConfTreeNode {
public:
    CConfTreeNode(CConfTreeNode* parent, CNutConfDoc *doc, NUTCOMPONENT *compo);
    CConfTreeNode(CConfTreeNode* parent, CNutConfDoc *doc, NUTCOMPONENTOPTION *option);
    virtual ~CConfTreeNode();

    wxString GetFilename() const;
    wxString GetMacro() const;
    wxString GetRequirementList() const;
    wxString GetProvisionList() const;
    wxString GetExclusivityList() const;

    nutComponentType GetConfigType() const;
    nutOptionType GetOptionType() const;
    nutOptionFlavor GetOptionFlavor() const;
    wxString GetOptionText() const;

    wxVariant GetStateIconText() const;
    wxString GetDescription() const;
    int GetEnumStrings(wxArrayString & arEnumStrings) const;

    bool HasBool() const;
    bool IsActive() const;
    bool IsEnabled() const;
    void SetActive(bool ena);
    void ToggleItem();
    bool SetValue(const wxString & strValue);

    bool ViewHeaderFile() const;

    bool IsContainer() const;

    CConfTreeNode* GetParent();
    CConfTreeNodePtrArray& GetChildren();
    CConfTreeNode* GetNthChild(unsigned int n);
    void Insert(CConfTreeNode* child, unsigned int n);
    void Append(CConfTreeNode* child);
    unsigned int GetChildCount() const;

    bool m_container;
public:     // public to avoid getters/setters
    wxString m_brief;
private:
    bool Create(CConfTreeNode* parent, const wxIcon &icon, CNutConfDoc *doc, NUTCOMPONENT *compo, NUTCOMPONENTOPTION *option);

    CNutConfDoc* m_configDoc;
    CConfTreeNode *m_parent;
    CConfTreeNodePtrArray m_children;
    /*! \brief Pointer to the component data structure. */
    NUTCOMPONENT *m_compo;
    /*! \brief Pointer to the option data structure. */
    NUTCOMPONENTOPTION *m_option;
    /*! 
     * \brief Component type of this item.
     *
     * Either library, module or option.
     */
    nutComponentType m_configType;

    /*!
     * \brief Option type of this item.
     *
     * If this item is an option, then it is either a string,
     * an integer, a boolean or an enumerated value.
     */
    nutOptionType m_optionType;
    nutOptionFlavor m_optionFlavor;

    nutOptionBoolFlavor m_optionBoolFlavor;

    wxString m_value_str;
    wxIcon m_icon;
    wxIcon m_icon_inactive;
};

#endif


