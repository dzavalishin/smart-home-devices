#ifndef PATHVALIDATOR_H_
#define PATHVALIDATOR_H_

/* ----------------------------------------------------------------------------
 * Copyright (C) 2007 by egnite Software GmbH
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
 * $Id: pathvalidator.h 3878 2012-02-15 15:46:22Z haraldkipp $
 */

#include <wx/wx.h>

/*! \brief Empty entries are not valid. */
#define VALIDPATH_NOT_EMPTY     0x0001
/*! \brief Directory name is valid. */
#define VALIDPATH_IS_DIRECTORY  0x0002
/*! \brief File name is valid. */
#define VALIDPATH_IS_PLAIN_FILE 0x0004
/*! \brief List is valid. */
#define VALIDPATH_LIST          0x0008
/*! \brief Only valid, if it exists. */
#define VALIDPATH_EXISTS        0x0010
/*! \brief Native path separators are valid. */
#define VALIDPATH_NATIVE        0x0020
/*! \brief UNIX path separators are valid. */
#define VALIDPATH_UNIX          0x0040
/*! \brief Automatically convert native to UNIX. */
#define VALIDPATH_TO_UNIX       0x1000
/*! \brief Automatically convert UNIX to native. */
#define VALIDPATH_SHOW_NATIVE   0x2000


class CPathValidator : public wxValidator
{
DECLARE_DYNAMIC_CLASS(CPathValidator)
public:
    CPathValidator(long style = wxFILTER_NONE, wxString *value = NULL);
    CPathValidator(const CPathValidator& src);
    virtual wxObject* Clone() const;
    
    bool Copy(const CPathValidator& val);


    virtual bool Validate(wxWindow* parent);
    virtual bool TransferFromWindow();
    virtual bool TransferToWindow();

protected:
    long m_style;
    wxString *m_value;

private:
    CPathValidator& operator=(const CPathValidator& src);
};

#endif

