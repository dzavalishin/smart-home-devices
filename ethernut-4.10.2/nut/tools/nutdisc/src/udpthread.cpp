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

// For compilers that support precompilation, includes "wx/wx.h".
//#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifdef __WXMSW__
#include <winsock2.h>
#include <Ws2tcpip.h>
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/thread.h>

// linux
#ifndef __WXMSW__
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif

#include "udpthread.h"
#include "discovery.h"
#include "options.h"
#include "nutdisc.h"

// define this to use wxExecute in the exec tests, otherwise just use system
#define USE_EXECUTE

#ifdef USE_EXECUTE
    #define EXEC(cmd) wxExecute((cmd), wxEXEC_SYNC)
#else
    #define EXEC(cmd) system(cmd)
#endif

CUdpThread::CUdpThread(CMainFrame *frame)
    : wxThread()
{
    m_frame = frame;
}

void CUdpThread::OnExit()
{
}

static char szBuffer[1024];

bool CUdpThread::Broadcast(DISCOVERY_TELE *dist)
{
    int sock;
    int on = 1;
    struct sockaddr_in client;
    int rc;

    gethostname(szBuffer, sizeof(szBuffer));
    struct hostent *host = gethostbyname(szBuffer);
    struct in_addr *addr = (struct in_addr *)host->h_addr_list[0];
    struct in_addr *addr2 = (struct in_addr *)host->h_addr_list[1];
    struct in_addr *addr3 = (struct in_addr *)host->h_addr_list[2];

    // Create client socket.
    if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
         return false;
    }
    rc = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (const char *)&on, sizeof(on));

    memset(&client, 0, sizeof(client));
    client.sin_family = AF_INET;
    client.sin_port = htons((unsigned short) wxAtoi(g_options->m_scanport));
    client.sin_addr.s_addr = inet_addr(g_options->m_scanip);
    if(dist) {
        rc = sendto(sock, (char *)dist, sizeof(DISCOVERY_TELE), 0, (struct sockaddr *)&client, sizeof(client));
    }
    else {
        DISCOVERY_TELE dummy;
        memset(&dummy, 0, sizeof(dummy));
        rc = sendto(sock, (char *)&dummy, sizeof(DISCOVERY_TELE), 0, (struct sockaddr *)&client, sizeof(client));
    }
#if defined(__WXMSW__)
    rc = closesocket(sock);
#else
    close(sock);
#endif
    return true;
}

void *CUdpThread::Entry()
{
    int sock;
    unsigned short lport = DISCOVERY_PORT;
    struct sockaddr_in server;
    struct sockaddr_in client;
#if defined(__WXMSW__)
    int cl;
#else
    socklen_t cl;
#endif
    int got;
    int on = 1;
    char buf[1500];
    DISCOVERY_TELE *dist;

    // Open and bind server socket.
    if((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
         return NULL;
    }
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&on, sizeof(on));
    server.sin_family = AF_INET;
    server.sin_port = htons((unsigned short) wxAtoi(g_options->m_scanport));
    server.sin_addr.s_addr = INADDR_ANY;
    if(bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
         return NULL;
    }

    for(;;) {
        cl = sizeof(client);
        got = recvfrom(sock, buf, 512, 0, (struct sockaddr *)&client, &cl);
        if(got > sizeof(DISCOVERY_TELE) - sizeof(dist->dist_appendix)) {
            dist = (DISCOVERY_TELE *)malloc(sizeof(DISCOVERY_TELE));
            memset(dist, 0, sizeof(DISCOVERY_TELE));
            memcpy(dist, buf, got);
            if(dist->dist_type == DIST_ANNOUNCE) {
                wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, UDP_EVENT);
                event.SetInt((int)dist);
                wxPostEvent(m_frame, event);
            }
            else {
                free(dist);
            }
        }
    }
    //return NULL;
}

