/*
 * Copyright (C) 2001-2005 by egnite Software GmbH. All rights reserved.
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
 * Revision 1.6  2009/02/13 14:52:05  haraldkipp
 * Include memdebug.h for heap management debugging support.
 *
 * Revision 1.5  2009/02/06 15:37:40  haraldkipp
 * Added stack space multiplier and addend. Adjusted stack space.
 *
 * Revision 1.4  2009/01/17 11:26:52  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.3  2009/01/16 17:03:50  haraldkipp
 * Configurable discovery protocol version and port plus
 * configurable service thread stack size. The new version 1.1
 * allows host names up to 98 characters. Added some code
 * to make sure, that nothing is overwritten with version 1.0
 * protocol and too long host names. Protocol version 1.0
 * is still the default.
 *
 * Revision 1.2  2008/08/11 07:00:35  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.1  2006/09/07 09:06:17  haraldkipp
 * Discovery service added.
 *
 */

#include <sys/confnet.h>
#include <sys/confos.h>
#include <sys/thread.h>
#include <sys/timer.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <net/if_var.h>

#include <stdlib.h>
#include <string.h>
#include <memdebug.h>

#include <pro/discover.h>

/*!
 * \addtogroup xgDiscover
 */
/*@{*/

#ifndef NUT_THREAD_DISTSTACK
#if defined(__AVR__)
#if defined(__GNUC__)
/* avr-gcc size optimized code uses 124 bytes. */
#define NUT_THREAD_DISTSTACK  224
#else
/* icc-avr v7.19 uses 244 bytes. */
#define NUT_THREAD_DISTSTACK  384
#endif
#else
/* arm-elf-gcc used 232 bytes with size optimized, 476 bytes with debug code. */
#define NUT_THREAD_DISTSTACK  320
#endif
#endif

#ifndef DISCOVERY_PORT
#define DISCOVERY_PORT  9806
#endif

typedef struct {
    uint32_t disopt_ipmask;
    uint16_t disopt_port;
    unsigned int disopt_flags;
} DISCOVERY_OPTIONS;

static DISCOVERY_OPTIONS disopt;
static uint32_t xid;

static int NutDiscoveryHandler(uint32_t ip, uint16_t port, DISCOVERY_TELE * dist, int len);
static NutDiscoveryCallback discovery_callback = NutDiscoveryHandler;

/*!
 * \brief Create an announcement datagram.
 *
 * \param dist Pointer to the datagram buffer that will be filled.
 *
 * \return Number of bytes filled into the datagram buffer.
 */
int NutDiscoveryAnnTele(DISCOVERY_TELE * dist)
{
    memset(dist, 0, sizeof(DISCOVERY_TELE));
    dist->dist_xid = xid;
    dist->dist_type = DIST_ANNOUNCE;
    dist->dist_ver = DISCOVERY_VERSION;
    memcpy(dist->dist_mac, confnet.cdn_mac, sizeof(dist->dist_mac));
    dist->dist_ip_addr = confnet.cdn_ip_addr;
    dist->dist_ip_mask = confnet.cdn_ip_mask;
    dist->dist_gateway = confnet.cdn_gateway;
    dist->dist_cip_addr = confnet.cdn_cip_addr;
#if DISCOVERY_VERSION <= 0x10
    memcpy(dist->dist_hostname, confos.hostname, sizeof(dist->dist_hostname));
    return sizeof(DISCOVERY_TELE) - sizeof(dist->dist_custom);
#else
    dist->dist_appendix[0] = (uint8_t)strlen(confos.hostname);
    memcpy(&dist->dist_appendix[1], confos.hostname, dist->dist_appendix[0]);
    return sizeof(DISCOVERY_TELE) - sizeof(dist->dist_appendix) + dist->dist_appendix[0] + 1;
#endif
}

/*!
 * \brief Apply new configuration.
 *
 * \param dist Pointer to the discovery datagram. It is assumed, that 
 *             the validity of the datagram contents had been checked 
 *             by the caller.
 */
int NutDiscoveryAppConf(DISCOVERY_TELE * dist)
{
    memset(confos.hostname, 0, sizeof(confos.hostname));
#if DISCOVERY_VERSION <= 0x10
    strncpy(confos.hostname, (char *)dist->dist_hostname, sizeof(confos.hostname) - 1);
#else
    memcpy(confos.hostname, &dist->dist_appendix[1], dist->dist_appendix[0]);
#endif
    NutSaveConfig();

    memcpy(confnet.cdn_mac, dist->dist_mac, sizeof(confnet.cdn_mac));
    confnet.cdn_ip_addr = dist->dist_ip_addr;
    confnet.cdn_ip_mask = dist->dist_ip_mask;
    confnet.cdn_gateway = dist->dist_gateway;
    confnet.cdn_cip_addr = dist->dist_cip_addr;

    return NutNetSaveConfig();
}

/*
 * \brief Default discovery datagram handler.
 *
 * \param ip    Sender's IP address.
 * \param port  Sender's UDP port.
 * \param dtel  Pointer to the UDP telegram buffer.
 * \param len   UDP telegram size.
 */
static int NutDiscoveryHandler(uint32_t ip, uint16_t port, DISCOVERY_TELE * dist, int len)
{
    int rc = -1;
#if DISCOVERY_VERSION <= 0x10
    size_t minlen = sizeof(DISCOVERY_TELE) - sizeof(dist->dist_custom);
#else
    size_t minlen = sizeof(DISCOVERY_TELE) - sizeof(dist->dist_appendix) + 1;
#endif

    /* Check minimum datagram length. */
    if (len >= minlen) {
        /* 
         * Request telegram. 
         */
        if (dist->dist_type == DIST_REQUEST) {
            /* Respond to requests. */
            rc = NutDiscoveryAnnTele(dist);
        }

        /* 
         * Apply telegram. 
         */
        else if (dist->dist_type == DIST_APPLY
                   /* Check exchange ID. */
                   && dist->dist_xid == xid
                   /* Required protocol version. */
                   && dist->dist_ver == DISCOVERY_VERSION) {    
            xid += NutGetTickCount();
            /* Store configuration. */
            rc = NutDiscoveryAppConf(dist);
        }
    }
    return rc;
}

THREAD(DiscoveryResponder, arg)
{
    UDPSOCKET *sock;
    DISCOVERY_TELE *dist;
    uint32_t raddr;
    uint16_t rport;
    int len;

    /* Insist on allocating a datagram buffer. */
    while ((dist = malloc(sizeof(DISCOVERY_TELE))) == NULL) {
        NutSleep(1000);
    }

    /* Insist on creating a socket. */
    while ((sock = NutUdpCreateSocket(disopt.disopt_port)) == NULL) {
        NutSleep(1000);
    }

    /* Nut/Net doesn't provide UDP datagram buffering by default. */
    {
        uint16_t max_ms = sizeof(DISCOVERY_TELE) * 3;

        NutUdpSetSockOpt(sock, SO_RCVBUF, &max_ms, sizeof(max_ms));
    }

    /* Create a pseudo random telegram identifier. */
    memcpy(&xid, &confnet.cdn_mac[2], sizeof(xid));
    xid += NutGetTickCount();

    /* Optionally send initial announce datagram. */
    if (disopt.disopt_flags & DISF_INITAL_ANN) {
        /* Variable sleep avoids broadcast storms in large installations. */
        NutSleep(500 + (xid & 0x7FF));
        if ((len = NutDiscoveryAnnTele(dist)) > 0) {
            NutUdpSendTo(sock, INADDR_BROADCAST, disopt.disopt_port, dist, len);
        }
    }

    /* Handle discovery telegrams in an endless loop. */
    for (;;) {
        len = NutUdpReceiveFrom(sock, &raddr, &rport, dist, sizeof(DISCOVERY_TELE), 0);
        /* Updates may be filtered by an IP mask. */
        if ((raddr & disopt.disopt_ipmask) == raddr && len > 0 && discovery_callback) {
            if ((len = discovery_callback(raddr, rport, dist, len)) > 0) {
                /* Broadcast response if destination is unreachable. */
                if ((raddr & confnet.cdn_ip_mask) != (confnet.cdn_ip_addr & confnet.cdn_ip_mask)) {
                    raddr = INADDR_BROADCAST;
                }
                NutUdpSendTo(sock, raddr, disopt.disopt_port, dist, len);
            }
        }
        /* Avoid system blocking by datagram storms. */
        NutSleep(100);
    }
}

/*!
 * \brief Register a custom discovery callback handler.
 *
 * When a callback had been registered, all datagrams matching the IP mask
 * are passed to this function, which should inspect the datagram contents
 * and take any required action like calling NutDiscoveryAnnTele() etc.
 * If the callback returns a positive value, a response will be sent
 * back, using that return value as its length.
 *
 * \param func Pointer to the callback function or NULL to disable
 *             responses.
 *
 * \return Previously used callback function pointer.
 */
NutDiscoveryCallback NutRegisterDiscoveryCallback(NutDiscoveryCallback func)
{
    NutDiscoveryCallback rc = discovery_callback;

    discovery_callback = func;

    return rc;
}

/*!
 * \brief Register discovery telegram responder.
 *
 * The first call will activate the responder thread. Any subsequent 
 * calls will return a failure.
 *
 * \note Enabling a discovery responder is a potential security hole.
 *
 * \param ipmask Update datagrams from remote hosts, which do not fit to 
 *               this mask are ignored. Set to INADDR_BROADCAST to allow 
 *               any. If zero, no updates are allowed.
 * \param port   The responder will listen to this UDP port. If zero,
 *               the default port \ref DISCOVERY_PORT is used.
 * \param flags  Option value, may contain any combination of DISF_ flags.
 *
 * \return 0 if a handler thread had been started, -1 otherwise.
 */
int NutRegisterDiscovery(uint32_t ipmask, uint16_t port, unsigned int flags)
{
    static HANDLE tid = NULL;

    if (tid == NULL) {
        disopt.disopt_ipmask = ipmask;
        disopt.disopt_port = port ? port : DISCOVERY_PORT;
        disopt.disopt_flags = flags;
        tid = NutThreadCreate("udisc", DiscoveryResponder, NULL, 
            (NUT_THREAD_DISTSTACK * NUT_THREAD_STACK_MULT) + NUT_THREAD_STACK_ADD);
        if (tid) {
            return 0;
        }
    }
    return -1;
}

/*@}*/
