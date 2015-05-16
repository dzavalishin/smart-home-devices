#ifndef _UDPTHREAD_H_
#define _UDPTHREAD_H_

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
 * $Id: udpthread.h 3841 2012-02-10 18:19:35Z haraldkipp $
 */

#include "mainframe.h"

#define UDP_EVENT    200

class CUdpThread : public wxThread
{
public:
    CUdpThread(CMainFrame *frame);

    virtual void *Entry();
    virtual void OnExit();
    bool Broadcast(DISCOVERY_TELE *dist = NULL);

public:
    CMainFrame *m_frame;
};

#endif
