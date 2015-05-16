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
 * $Id$
 */

#include "setup.h"

wxString CSetup::MacToString(unsigned char mac[6])
{
    wxString str;
    str.Printf("%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return str;
}

wxString CSetup::IpToString(unsigned long ip)
{
    unsigned char *cp = (unsigned char *)&ip;
    wxString str;
    str.Printf("%u.%u.%u.%u", cp[0], cp[1], cp[2], cp[3]);
    return str;
}

bool CSetup::StringToMac(wxString str, unsigned char *mac)
{
    unsigned long val;
    wxString sub;
    int i;

    for(i = 0; i < 6; i++) {
        sub = str.BeforeFirst(':');
        str = str.AfterFirst(':');
        if(!sub.ToULong(&val, 16)) {
            return false;
        }
        mac[i] = (unsigned char)val;
    }
    return true;
}

bool CSetup::StringToIp(wxString str, unsigned long *ip)
{
    unsigned char *cp = (unsigned char *)ip;
    wxString sub;
    unsigned long val;
    int i;

    for(i = 0; i < 4; i++) {
        sub = str.BeforeFirst('.');
        str = str.AfterFirst('.');
        if(!sub.ToULong(&val)) {
            return false;
        }
        cp[i] = (unsigned char)val;
    }
    return true;
}

