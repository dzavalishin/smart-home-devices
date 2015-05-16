#ifndef _PRO_DHCP_H_
#define _PRO_DHCP_H_

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
 * -
 * Portions Copyright (c) 1983, 1993 by
 *  The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*!
 * \file pro/dhcp.h
 * \brief DHCP protocol definitions.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.10  2008/08/11 07:00:23  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.9  2006/10/08 16:48:22  haraldkipp
 * Documentation fixed
 *
 * Revision 1.8  2005/08/02 17:46:49  haraldkipp
 * Major API documentation update.
 *
 * Revision 1.7  2005/02/03 14:34:05  haraldkipp
 * Several bug fixes and enhancements. The most important fix will
 * avoid hanging, when Ethernut is reset while ICMP pings are
 * received. A large number of changes make the client to follow
 * RFC 2131 more closely, like maintaining renewal and rebind time.
 * Two new API calls, NutDhcpStatus() and NutDhcpError(), allow
 * to query the current status of the DHCP client. The previously
 * introduced API call NutDhcpIsConfigured() has been marked
 * deprecated. Another problem was with timeout, because the client
 * continued negotiation with the server after the timeout time
 * given in the API called elapsed. Now all internal timeouts are
 * adjusted to the API timeout. Furthermore the previous version
 * let the client continue on fatal errors like UDP receive or UDP
 * broadcast failures. Now the client stops on such errors and
 * informs the caller. Finally the documentation has been enhanced.
 *
 * Revision 1.6  2004/12/31 10:52:18  drsung
 * Bugfixes from Michel Hendriks applied.
 *
 * Revision 1.5  2004/03/18 11:22:03  haraldkipp
 * Deprecated functions removed
 *
 * Revision 1.4  2004/02/25 16:24:13  haraldkipp
 * DHCP release added
 *
 * Revision 1.3  2004/01/14 17:51:05  drsung
 * Fix for Win2k DHCP server applied. Thanks to Damian Slee
 *
 * Revision 1.2  2003/08/05 20:20:36  haraldkipp
 * Comments added
 *
 * Revision 1.1.1.1  2003/05/09 14:41:18  haraldkipp
 * Initial using 3.2.1
 *
 * \endverbatim
 */

#include <cfg/dhcp.h>
#include <sys/types.h>
#include <stdint.h>

#include <compiler.h> // For NUT_DEPRECATED

/*!
 * \addtogroup xgDHCPC
 */
/*@{*/


/*!
 * \name DHCP Client States.
 *
 * Applications can request the current state of the DHCP client by 
 * calling NutDhcpStatus().
 */
/*@{*/

/*! \brief DHCP state: Stopped.
 *
 * Indicates that the DHCP client is inactive. Either it just started,
 * gave up the lease or ran into an error. In the latter case the
 * application may call NutDhcpError() to retrieve the specific
 * error code.
 *
 * In order to save heap memory, the client will not open the UDP socket
 * while inactive.
 */
#define DHCPST_IDLE         0

/*! \brief DHCP state: Starting.
 * 
 * Indicates that the DHCP client started to request a configuration from
 * the DHCP server. If any previously allocated IP address is available
 * in the non-volatile configuration memory, then the client will continue 
 * with \ref DHCPST_REBOOTING. Otherwise it will move to 
 * \ref DHCPST_SELECTING.
 */
#define DHCPST_INIT         1

/*! \brief DHCP state: Selecting.
 *
 * In this state the client will transmit a DHCP discover message and
 * collect incoming offers from DHCP servers. If at least one acceptable
 * offer has been received, it will change to \ref DHCPST_REQUESTING.
 */
#define DHCPST_SELECTING    2

/*! \brief DHCP state: Requesting.
 *
 * Indicates that the client received and selected an offer from a
 * DHCP server. It is now sending a request for an offered 
 * configuration and waiting for an acknowledge, which will change
 * the client's state to \ref DHCPST_BOUND.
 */
#define DHCPST_REQUESTING   3

/*! \brief DHCP state: Rebooting.
 *
 * The client enters this state to request a previously assigned
 * configuration.
 */
#define DHCPST_REBOOTING    4

/*! \brief DHCP state: Bound.
 *
 * This state indicates, that the DHCP client has successfully allocated
 * a network address. Any thread blocked in NutDhcpIfConfig() will be
 * woken up.
 *
 * The client remains in this state until the renewal time elapses, in
 * which case it moves to \ref DHCPST_RENEWING.
 *
 * In order to save heap memory, the client will close the UDP socket
 * while in bound state.
 */
#define DHCPST_BOUND        5

/*! \brief DHCP state: Renewing.
 *
 * In this state the client tries to extend its lease by sending
 * a request to the DHCP server. If the server responds with an
 * acknowledge, the client returns to \ref DHCPST_BOUND.
 *
 * If no acknowledge is received until the rebind time elapses,
 * the client moves to \ref DHCPST_REBINDING.
 */
#define DHCPST_RENEWING     6

/*! \brief DHCP state: Rebinding.
 *
 * The client enters this state after the no acknowledge has been
 * received during \ref DHCPST_RENEWING and the rebind time
 * elapsed. It will broadcast a request to extend its lease.
 *
 * If no acknowledge is received until the lease time elapsed,
 * the client will move to \ref DHCPST_INIT.
 */
#define DHCPST_REBINDING    7

/*! \brief DHCP state: Informing.
 *
 * The client enters this state when the application calls
 * NutDhcpInform().
 */
#define DHCPST_INFORMING    8

/*! \brief DHCP state: Releasing.
 * 
 * The client enters this state when the application calls
 * NutDhcpRelease().
 */
#define DHCPST_RELEASING    9

/*@}*/


/*!
 * \name DHCP Error Codes
 *
 * Applications can request the lastest error code of the DHCP client by 
 * calling NutDhcpError().
 */
/*@{*/

/*!
 * \brief DHCP timeout error.
 *
 * No server response within the specified number of milliseconds. Either
 * the timeout value has been too low or no DHCP server is available in
 * the local network.
 */
#define DHCPERR_TIMEOUT     1

/*!
 * \brief DHCP MAC error.
 *
 * No Ethernet MAC address found in the non-volatile configuration memory
 * and none specified by calling NutDhcpIfConfig().
 */
#define DHCPERR_NOMAC       2

/*!
 * \brief DHCP state error.
 *
 * Either NutDhcpInform() has been called while not in state 
 * \ref DHCPST_IDLE or NutDhcpRelease() has been called while not in
 * state \ref DHCPST_BOUND.
 */
#define DHCPERR_STATE       3

/*!
 * \brief DHCP error: Bad device.
 *
 * The specified device name hasn't been registered or is not an
 * Ethernet device.
 */
#define DHCPERR_BADDEV      17

/*!
 * \brief DHCP system error.
 *
 * A system error occured during DHCP processing. Most probably the
 * system ran out of memory.
 */
#define DHCPERR_SYSTEM      18

/*!
 * \brief DHCP transmit error.
 *
 * Failed to send a UDP datagram. The DHCP client considers it a fatal
 * error if NutUdpSendTo() to the broadcast address fails.
 */
#define DHCPERR_TRANSMIT    19

/*!
 * \brief DHCP receive error.
 *
 * Failed to receive a UDP datagram. The DHCP client considers it a fatal
 * error if NutUdpReceiveFrom() fails.
 */
#define DHCPERR_RECEIVE     20

/*@}*/

/*@}*/

__BEGIN_DECLS
/* Function prototypes. */
extern int NutDhcpIfConfig(CONST char *name, uint8_t * mac, uint32_t timeout);
extern int NutDhcpRelease(CONST char *name, uint32_t timeout);
extern int NutDhcpInform(CONST char *name, uint32_t timeout);
extern int NutDhcpStatus(CONST char *name);
extern int NutDhcpError(CONST char *name);

extern int NutDhcpIsConfigured(void) NUT_DEPRECATED;

__END_DECLS
/* */
#endif
