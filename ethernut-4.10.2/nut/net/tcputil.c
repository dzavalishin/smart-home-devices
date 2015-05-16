/*
 * Copyright (C) 2001-2004 by egnite Software GmbH. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY EGNITE SOFTWARE GMBH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL EGNITE
 * SOFTWARE GMBH OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * For additional information see http://www.ethernut.de/
 *
 */

/*
 * $Log$
 * Revision 1.4  2008/08/11 07:00:32  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.3  2008/07/27 11:43:22  haraldkipp
 * Configurable TCP retransmissions.
 *
 * Revision 1.2  2004/07/30 19:54:46  drsung
 * Some code of TCP stack redesigned. Round trip time calculation is now
 * supported. Fixed several bugs in TCP state machine. Now TCP connections
 * should be more reliable under heavy traffic or poor physical connections.
 *
 * Revision 1.1.1.1  2003/05/09 14:41:45  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.10  2003/05/06 18:23:15  harald
 * Still not done
 *
 * Revision 1.9  2003/02/04 18:14:57  harald
 * Version 3 released
 *
 * Revision 1.8  2002/06/26 17:29:36  harald
 * First pre-release with 2.4 stack
 *
 */

#include <cfg/tcp.h>

#include <netinet/tcputil.h>
#include <sys/timer.h>
#include <stdio.h>

/*!
 * \addtogroup xgTCP
 */
/*@{*/

/* Limit retransmission timeout to 200ms as lower and 20secs as upper boundary */
#ifndef TCP_RTTO_MIN
#define TCP_RTTO_MIN    200
#endif
#ifndef TCP_RTTO_MAX
#define TCP_RTTO_MAX    20000
#endif

#define min(a,b) ((a>b)?b:a)
#define max(a,b) ((a>b)?a:b)

/*
 * Calculate round trip time.
 */
void NutTcpCalcRtt(TCPSOCKET * sock)
{
    uint16_t delta;
    
    if (sock->so_retran_time == 0)
        return;
        
    delta = (uint16_t) NutGetMillis() - (sock->so_retran_time & ~1);
    
    /* According to RFC793 (or STD007), page 41, we use 0.8 for ALPHA and 2.0 for BETA. */
    sock->so_rtto = min (TCP_RTTO_MAX, max(TCP_RTTO_MIN, (delta * 4 + sock->so_rtto * 8) / 10));
    //@@@printf ("[%04X] new retran timeout: %u, delta: %u\n", (u_short) sock, sock->so_rtto, delta);
}

/*@}*/
