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
 * -
 * Portions Copyright (c) 1993 by Digital Equipment Corporation.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies, and that
 * the name of Digital Equipment Corporation not be used in advertising or
 * publicity pertaining to distribution of the document or software without
 * specific, written prior permission.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND DIGITAL EQUIPMENT CORP. DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS.   IN NO EVENT SHALL DIGITAL EQUIPMENT
 * CORPORATION BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

/*!
 * \file pro/dhcpc.c
 * \brief DHCP client.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.27  2009/02/13 14:52:05  haraldkipp
 * Include memdebug.h for heap management debugging support.
 *
 * Revision 1.26  2009/02/06 15:37:40  haraldkipp
 * Added stack space multiplier and addend. Adjusted stack space.
 *
 * Revision 1.25  2009/01/17 11:26:52  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.24  2008/10/05 16:46:15  haraldkipp
 * Added missing attributes 'packed'. This may fix a problem with
 * different ARM compiler optimization settings.
 *
 * Revision 1.23  2008/08/11 07:00:34  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.22  2006/08/01 07:38:41  haraldkipp
 * DHCP client failed because of alignment errors. Removed the dispensable
 * 'packed' attribute from the dyn_cfg structure.
 *
 * Revision 1.21  2006/01/23 19:52:10  haraldkipp
 * Added required typecasts before left shift.
 *
 * Revision 1.20  2006/01/23 17:35:54  haraldkipp
 * BOOTP and DYNCFG structures must be packed.
 * Fixed memory alignment bug, which retrieved wrong values from DHCP options.
 *
 * Revision 1.19  2005/08/02 17:47:04  haraldkipp
 * Major API documentation update.
 *
 * Revision 1.18  2005/04/05 17:44:57  haraldkipp
 * Made stack space configurable.
 *
 * Revision 1.17  2005/03/28 18:26:59  mrjones4u
 * Fixed non-release bug in DCHP client
 *
 * Revision 1.16  2005/02/03 14:33:56  haraldkipp
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
 * Revision 1.15  2004/12/31 10:51:40  drsung
 * Bugfixes from Michel Hendriks applied:
 * Allocate empty dyncfg's to prevent freeing twice of domain/hostname
 * Clear pointer after ReleaseDynCfg to prevent freeing twice
 * Make sure SelectOffer is never called with 0 offers
 * When falling back from rebooting to selecting, clear
 * the received ip address so NutDhcpIfConfig does not
 * assume it has been configured properly
 *
 * Revision 1.14  2004/04/15 07:16:47  drsung
 * Now it works... :-/
 *
 * Revision 1.13  2004/04/15 07:11:50  drsung
 * Added hostname support to NutDHCPDiscover
 *
 * Revision 1.12  2004/03/18 11:24:01  haraldkipp
 * Deprecated functions removed
 *
 * Revision 1.11  2004/03/03 17:53:28  drsung
 * Support for new field 'hostname' in structure confos added.
 *
 * Revision 1.10  2004/02/25 16:34:32  haraldkipp
 * New API added to relinguish the DHCP lease. Collecting more
 * than one offer is now disabled, if the application sets timeout
 * below three times of MAX_DHCP_WAIT (5 seconds). The lease renewal
 * will now start when 3/4 has elapsed, opposed to 1/2 used previously.
 * Finally the DHCP thread will sleep as long as possible, while the
 * previous version woke up every ten seconds.
 *
 * Revision 1.9  2004/02/02 18:54:43  drsung
 * gateway ip address was not set, if static network configuration from EEPROM is used.
 *
 * Revision 1.8  2004/01/14 17:50:41  drsung
 * Fix for Win2k DHCP server applied. Thanks to Damian Slee
 *
 * Revision 1.7  2003/11/03 16:22:59  haraldkipp
 * Renewal disabled if lease time is set to zero
 *
 * Revision 1.6  2003/10/13 10:18:08  haraldkipp
 * Using new seconds counter
 *
 * Revision 1.5  2003/08/07 09:09:08  haraldkipp
 * Redesign to follow RFC 2131 more closely.
 *
 * Revision 1.4  2003/07/20 18:25:40  haraldkipp
 * Support secondary DNS.
 *
 * Revision 1.3  2003/07/17 09:44:14  haraldkipp
 * Removed default MAC address and let the caller have a second chance.
 *
 * Revision 1.2  2003/05/15 14:25:38  haraldkipp
 * Some DHCP servers discard discover telegrams.
 * Thanks to Tomohiro Haraikawa.
 *
 * Revision 1.1.1.1  2003/05/09 14:41:57  haraldkipp
 * Initial using 3.2.1
 *
 * \endverbatim
 */

#include <cfg/os.h>
#include <sys/thread.h>
#include <sys/event.h>
#include <sys/timer.h>
#include <sys/confnet.h>
#include <sys/confos.h>

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <memdebug.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <net/route.h>
#include <sys/socket.h>
#include <pro/dhcp.h>

#ifdef NUTDEBUG
#include <net/netdebug.h>
#endif

#if 0
/* Use for local debugging. */
#define NUTDEBUG
#include <stdio.h>
#define __tcp_trs stdout
static uint_fast8_t __tcp_trf = 1;
#endif

/*!
 * \addtogroup xgDHCPC
 */
/*@{*/

/*!
 * \name DHCP Client Configuration
 *
 * The Nut/OS Configurator may be used to override the default values.
 */
/*@{*/

/*!
 * \brief UDP port of DHCP server.
 *
 * \showinitializer
 */
#ifndef DHCP_SERVERPORT
#define DHCP_SERVERPORT      67
#endif

/*!
 * \brief UDP port of DHCP client.
 *
 * \showinitializer
 */
#ifndef DHCP_CLIENTPORT
#define DHCP_CLIENTPORT      68
#endif

/*!
 * \brief Maximum DHCP message size we can accept.
 *
 * RFC 2131 demands, that a DHCP client must be prepared to receive DHCP
 * messages with an options field length of at least 312 octets. This
 * implies that we must be able to accept messages of up to 576 octets.
 *
 * \showinitializer
 */
#ifndef MAX_DHCP_MSGSIZE
#define MAX_DHCP_MSGSIZE    576
#endif

/*!
 * \brief Minimum DHCP message length.
 *
 * Used to maintain BOOTP compatibility of outgoing messages.
 *
 * \showinitializer
 */
#ifndef MIN_DHCP_MSGSIZE
#define MIN_DHCP_MSGSIZE    300
#endif

/*!
 * \brief Maximum UDP buffer size used by the DHCP client.
 *
 * If this item is not equal zero, the DHCP client will use its value to
 * set #SO_RCVBUF by calling NutUdpSetSockOpt().
 *
 * If this item is set to zero, NutUdpSetSockOpt() is not called and the
 * UDP socket interface will buffer the last incoming datagram on the
 * #DHCP_CLIENTPORT socket port only. Any previously received datagram is
 * silently discarded. As long as one DHCP server is expected in the local
 * network, this will be fine and save some heap memory while DHCP is
 * active.
 *
 * \showinitializer
 */
#ifndef MAX_DHCP_BUFSIZE
#define MAX_DHCP_BUFSIZE    1728
#endif

/*!
 * \brief Minimum number of milliseconds to wait for a response.
 *
 * If we receive no response from a DHCP server within this time, we
 * will double this value up to \ref MAX_DHCP_WAIT and repeat our
 * request up to \ref MAX_DCHP_RETRIES times before giving up.
 *
 * \showinitializer
 */
#ifndef MIN_DHCP_WAIT
#define MIN_DHCP_WAIT       4000
#endif

/*!
 * \brief Maximum number of milliseconds to wait for a response.
 *
 * The timeout value for receiving server responses will be doubled
 * on each retry but limited by this value.
 *
 * \showinitializer
 */
#ifndef MAX_DHCP_WAIT
#define MAX_DHCP_WAIT       64000
#endif

/*!
 * \brief Maximum number of request retries.
 *
 * We will give up after resending this number of requests without
 * receiving a response.
 *
 * \showinitializer
 */
#ifndef MAX_DCHP_RETRIES
#define MAX_DCHP_RETRIES    3
#endif

/*!
 * \brief Maximum number of release retries.
 *
 * RFC 2131 doesn't specify a server response to release messages from
 * the client. If the message gets lost, then the lease isn't released.
 *
 * \showinitializer
 */
#ifndef MAX_DCHP_RELEASE_RETRIES
#define MAX_DCHP_RELEASE_RETRIES    0
#endif

/*!
 * \brief Default lease time in seconds.
 *
 * This value is used if the server doesn't provide a lease time.
 *
 * \showinitializer
 */
#ifndef DHCP_DEFAULT_LEASE
#define DHCP_DEFAULT_LEASE  43200
#endif

/*!
 * \brief Maximum sleep time in seconds.
 *
 * \showinitializer
 */
#ifndef MAX_DHCP_NAPTIME
#define MAX_DHCP_NAPTIME    4294967
#endif

/*!
 * \brief Stack size of the DHCP client thread.
 *
 * \showinitializer
 */
#ifndef NUT_THREAD_DHCPSTACK
#if defined(__AVR__)
#if defined(__GNUC__)
/* avr-gcc size optimized code used 192 bytes. */
#define NUT_THREAD_DHCPSTACK    288
#else
/* icc-avr v7.19 used 360 bytes. */
#define NUT_THREAD_DHCPSTACK    512
#endif
#else
/* arm-elf-gcc used 276 bytes with size optimized, 680 bytes with debug code. */
/* arm-none-eabi creates stack overflow with 384 bytes on EIR 1.0 board. */
#define NUT_THREAD_DHCPSTACK    512
#endif
#endif

/*@}*/

/*!
 * \name DHCP Message Types
 *
 * See RFC 2131.
 */
/*@{*/

/*! \brief Client broadcast to locate available servers.
 */
#define DHCP_DISCOVER   1

/*! \brief Server to client in response to DHCP_DISCOVER.
 *
 * Contains an offer of configuration parameters.
 */
#define DHCP_OFFER      2

/*! \brief Client message to servers.
 *
 * Used for
 * - requesting offered parameters from one server and implicitly declining offers from all others.
 * - confirming correctness of previously allocated address after, e.g., system reboot.
 * - extending the lease on a particular network address.
 */
#define DHCP_REQUEST    3

/*! \brief Client to server indicating network address is already in use.
 *
 * Not used by Nut/Net.
 */
#define DHCP_DECLINE    4

/*! \brief Server to client with configuration parameters.
 *
 * Contains committed network address.
 */
#define DHCP_ACK        5

/*! \brief Server to client indicating client's notion of network address is incorrect.
 *
 * May be caused by the client's move to new subnet or by expiration of the client's lease.
 */
#define DHCP_NAK        6

/*! \brief Client to server relinquishing network address and cancelling remaining lease.
 */
#define DHCP_RELEASE    7

/*! \brief Client to server, asking only for local configuration parameters.
 *
 * Used, if the client already has externally configured network address.
 */
#define DHCP_INFORM     8

/*@}*/


/*!
 * \name DHCP Options
 *
 * Nut/Net recognizes a subset of options defined in RFC 2132.
 */
/*@{*/

/*!
 * \brief DHCP pad option.
 *
 * The pad option can be used to cause subsequent fields to align on
 * word boundaries.
 */
#define DHCPOPT_PAD          0

/*!
 * \brief DHCP subnet mask option.
 */
#define DHCPOPT_NETMASK      1

/*!
 * \brief DHCP router option.
 */
#define DHCPOPT_GATEWAY      3

/*!
 * \brief DHCP domain name server option.
 */
#define DHCPOPT_DNS          6

/*!
 * \brief DHCP host name option.
 */
#define DHCPOPT_HOSTNAME     12

/*!
 * \brief DHCP domain name option.
 */
#define DHCPOPT_DOMAIN       15

/*!
 * \brief DHCP broadcast address option.
 */
#define DHCPOPT_BROADCAST    28

/*!
 * \brief DHCP requested IP address option.
 */
#define DHCPOPT_REQESTIP     50

/*!
 * \brief DHCP IP address lease time option.
 */
#define DHCPOPT_LEASETIME    51

/*!
 * \brief DHCP message type option.
 */
#define DHCPOPT_MSGTYPE      53

/*!
 * \brief DHCP server identifier option.
 */
#define DHCPOPT_SID          54

/*!
 * \brief DHCP parameter request list option.
 */
#define DHCPOPT_PARAMREQUEST 55

/*!
 * \brief Maximum DHCP message size option.
 */
#define DHCPOPT_MAXMSGSIZE   57

/*!
 * \brief DHCP renewal time option.
 */
#define DHCPOPT_RENEWALTIME  58

/*!
 * \brief DHCP rebinding time option.
 */
#define DHCPOPT_REBINDTIME   59

/*!
 * \brief DHCP end option.
 */
#define DHCPOPT_END          255

/*@}*/

/*!
 * \brief BOOTP message structure type.
 */
typedef struct bootp BOOTP;

/*!
 * \brief BOOTP message structure.
 */
struct __attribute__ ((packed)) bootp {
    uint8_t bp_op;              /*!< \brief Packet opcode type: 1=request, 2=reply */
    uint8_t bp_htype;           /*!< \brief Hardware address type: 1=Ethernet */
    uint8_t bp_hlen;            /*!< \brief Hardware address length: 6 for Ethernet */
    uint8_t bp_hops;            /*!< \brief Gateway hops */
    uint32_t bp_xid;              /*!< \brief Transaction ID */
    uint16_t bp_secs;            /*!< \brief Seconds since boot began */
    uint16_t bp_flags;           /*!< \brief RFC1532 broadcast, etc. */
    uint32_t bp_ciaddr;           /*!< \brief Client IP address */
    uint32_t bp_yiaddr;           /*!< \brief 'Your' IP address */
    uint32_t bp_siaddr;           /*!< \brief Server IP address */
    uint32_t bp_giaddr;           /*!< \brief Gateway IP address */
    uint8_t bp_chaddr[16];      /*!< \brief Client hardware address */
    char bp_sname[64];          /*!< \brief Server host name */
    char bp_file[128];          /*!< \brief Boot file name */
    uint8_t bp_options[312];    /*!< \brief Vendor-specific area */
};

/*!
 * \brief Dynamic configuration structure type.
 */
typedef struct dyn_cfg DYNCFG;

/*!
 * \brief Dynamic configuration structure.
 */
struct dyn_cfg {
    uint8_t dyn_msgtyp;         /*!< \brief DHCP message type */
    uint32_t dyn_yiaddr;          /*!< \brief Offered IP address. */
    uint32_t dyn_netmask;         /*!< \brief Local IP netmask. */
    uint32_t dyn_broadcast;       /*!< \brief Local IP broadcast address. */
    uint32_t dyn_gateway;         /*!< \brief Default gate IP address. */
    uint32_t dyn_pdns;            /*!< \brief Primary DNS IP address. */
    uint32_t dyn_sdns;            /*!< \brief Secondary DNS IP address. */
    uint32_t dyn_sid;             /*!< \brief Server identifier. */
    uint32_t dyn_renewalTime;     /*!< \brief Renewal time in seconds. */
    uint32_t dyn_rebindTime;      /*!< \brief Rebind time in seconds. */
    uint32_t dyn_leaseTime;       /*!< \brief Offered lease time in seconds. */
    uint8_t *dyn_hostname;      /*!< \brief Local hostname. */
    uint8_t *dyn_domain;        /*!< \brief Name of local domain. */
};

/*!
 * \brief Current configuration.
 *
 * This structure is filled by parsing offer or acknowledge messages from
 * the server.
 */
static DYNCFG *dhcpConfig;

/*!
 * \brief Client thread identifier.
 *
 * Used to determine if the client thread is running. Zero indicates that
 * it is not.
 */
static HANDLE dhcpThread;

/*!
 * \brief Current state of the DHCP client state machine.
 */
static uint8_t dhcpState;

/*!
 * \brief Latest DHCP error code.
 */
static int dhcpError;

/*!
 * \brief DHCP wake up queue.
 *
 * The DHCP state machine can be woken up by posting to this queue.
 */
static HANDLE dhcpWake;

/*!
 * \brief DHCP waiting queue.
 *
 * Application threads wait on this queue until DHCP success or failure.
 */
static HANDLE dhcpDone;

/*!
 * \brief Maximum number of milliseconds to wait on \ref dhcpDone.
 *
 * Specified by the application when calling the DHCP client API.
 */
static uint32_t dhcpApiTimeout;

/*!
 * \brief Time at which the application started to wait.
 *
 * Used in conjunction with \ref dhcpApiTimeout to limit the maximum
 * wait time for server responses.
 */
static uint32_t dhcpApiStart;

/*!
 * \brief DHCP device.
 *
 * The ARM port doesn't provide parameter passing to thread routines.
 * Thus we use a global variable to store the NUTDEVICE pointer.
 */
#ifdef __arm__
static NUTDEVICE *dhcpDev;
#endif

/*!
 * \brief Dynamic string copy.
 *
 * \param dst Points to a string pointer. If the pointer is not NULL, it
 *            is assumed that it points to a previously allocated buffer
 *            and this buffer will be released first. Then a new buffer
 *            will be allocated and the source string will be copied to
 *            this buffer.
 * \param src The source string. No delimiter required.
 * \param len Length of the source string.
 */
static void copy_str(uint8_t ** dst, void *src, int len)
{
    if (*dst) {
        free(*dst);
    }
    if ((*dst = malloc(len + 1)) != 0) {
        if (len) {
            memcpy(*dst, src, len);
        }
        *(*dst + len) = 0;
    }
}

/*!
 * \brief Release DYNCFG structure.
 *
 * Frees all memory occupied by a \ref DYNCFG structure.
 *
 * \param dyncfg This structure will be released.
 */
static void ReleaseDynCfg(DYNCFG * dyncfg)
{
    if (dyncfg) {
        if (dyncfg->dyn_hostname) {
            free(dyncfg->dyn_hostname);
        }
        if (dyncfg->dyn_domain) {
            free(dyncfg->dyn_domain);
        }
        free(dyncfg);
    }
}

/*!
 * \brief Parse a DHCP reply message.
 *
 * \param bp  Pointer to the reply message. The caller must make sure,
 *            that this contains a valid BOOTP reply header with at
 *            least five bytes in the options field.
 * \param len Number of valid bytes in the reply message.
 *
 * \return Pointer to config structure. Must be released by the caller.
 *         NULL is returned in case of parsing errors.
 */
static DYNCFG *ParseReply(BOOTP *bp, int len)
{
    uint8_t *op;
    int left;
    DYNCFG *cfgp;

    /* Allocate and initialize a new structure. */
    if ((cfgp = malloc(sizeof(DYNCFG))) == 0) {
        return 0;
    }
    memset(cfgp, 0, sizeof(DYNCFG));
    cfgp->dyn_leaseTime = DHCP_DEFAULT_LEASE;

    /* Set the assigned IP address. */
    memcpy(&cfgp->dyn_yiaddr, &bp->bp_yiaddr, 4);

    /*
     * Parse options until an end option is found or until we reached
     * the end of the message.
     */
    op = bp->bp_options + 4;
    left = len - (sizeof(*bp) - sizeof(bp->bp_options)) - 4;
    while (*op != DHCPOPT_END && left > 0) {
        uint8_t ol;

#ifdef NUTDEBUG
        if (__tcp_trf) {
            fprintf(__tcp_trs, "[DHCP-Opt-%u]", *op);
        }
#endif
        /* Pad option is used for boundary alignment. */
        if (*op == DHCPOPT_PAD) {
            op++;
            left--;
            continue;
        }

        /* Reject if option length exceeds total length. */
        if ((ol = *(op + 1)) > left) {
            break;
        }

        /* Type of this message. */
        if (*op == DHCPOPT_MSGTYPE) {
            if (ol != 1) {
                break;
            }
            cfgp->dyn_msgtyp = *(op + 2);

#ifdef NUTDEBUG
            if (__tcp_trf & NET_DBG_DHCP) {
                switch(cfgp->dyn_msgtyp) {
                    case DHCP_OFFER:
                        fprintf(__tcp_trs, "+MSGT-OFFER+\n");
                        break;
                    case DHCP_ACK:
                        fprintf(__tcp_trs, "+MSGT-ACK+\n");
                        break;
                    case DHCP_NAK:
                        fprintf(__tcp_trs, "-MSGT-NAK-\n");
                        break;
                    default:
                        fprintf(__tcp_trs, "#MSGT-UNK#\n");
                        break;
                }
            }
#endif
        }
        /* Our host name. May or may not include the domain. */
        else if (*op == DHCPOPT_HOSTNAME) {
            copy_str(&cfgp->dyn_hostname, op + 2, ol);
        }
        /* Name of the domain we are in. */
        else if (*op == DHCPOPT_DOMAIN) {
            copy_str(&cfgp->dyn_domain, op + 2, ol);
        }

        /* All remaining options require at least 4 octets. */
        else if (ol >= 4) {
            /* Preset most often used long value. */
            uint32_t lval = *(op + 2);
            lval += (uint32_t)(*(op + 3)) << 8;
            lval += (uint32_t)(*(op + 4)) << 16;
            lval += (uint32_t)(*(op + 5)) << 24;

            /* Our IP network mask. */
            if (*op == DHCPOPT_NETMASK) {
                cfgp->dyn_netmask = lval;
            }
            /* Our IP broadcast address. */
            else if (*op == DHCPOPT_BROADCAST) {
                cfgp->dyn_broadcast = lval;
            }
            /* Our IP default gate. More than one gateway may be
               specified. We take the fist one only and ignore the
               rest. */
            else if (*op == DHCPOPT_GATEWAY) {
                cfgp->dyn_gateway = lval;
            }
            /* Our DNS server. Updated by Jelle Martijn Kok to
               support a secondary DNS. */
            else if (*op == DHCPOPT_DNS) {
                cfgp->dyn_pdns = lval;
                if (ol >= 8) {
                    cfgp->dyn_sdns = *(op + 6);
                    cfgp->dyn_sdns += (uint32_t)(*(op + 7)) << 8;
                    cfgp->dyn_sdns += (uint32_t)(*(op + 8)) << 16;
                    cfgp->dyn_sdns += (uint32_t)(*(op + 9)) << 24;
                }
            }
            /* Server identifier. */
            else if (*op == DHCPOPT_SID) {
                cfgp->dyn_sid = lval;
            }
            /* Renewal time. */
            else if (*op == DHCPOPT_RENEWALTIME) {
                cfgp->dyn_renewalTime = ntohl(lval);
            }
            /* Rebinding time. */
            else if (*op == DHCPOPT_REBINDTIME) {
                cfgp->dyn_rebindTime = ntohl(lval);
            }
            /* Total lease time granted. */
            else if (*op == DHCPOPT_LEASETIME) {
                cfgp->dyn_leaseTime = ntohl(lval);
            }
        }
        op += ol + 2;
        left -= ol + 2;
    }

    /*
     * Discard this configuration if parsing stopped before reaching
     * the end option or if we didn't receive an expected message type.
     */
    if (*op != DHCPOPT_END ||
        (cfgp->dyn_msgtyp != DHCP_OFFER &&
         cfgp->dyn_msgtyp != DHCP_ACK &&
         cfgp->dyn_msgtyp != DHCP_NAK)) {
#ifdef NUTDEBUG
        if (__tcp_trf & NET_DBG_DHCP) {
            fprintf(__tcp_trs, "[DHCP-Parse Error]");
        }
#endif
        ReleaseDynCfg(cfgp);
        return 0;
    }

    /* Calculate renewal and rebind times. */
    if (cfgp->dyn_renewalTime == 0) {
        cfgp->dyn_renewalTime = cfgp->dyn_leaseTime / 2;
    }
    if (cfgp->dyn_rebindTime == 0) {
        cfgp->dyn_rebindTime = cfgp->dyn_renewalTime +  /* */
            cfgp->dyn_renewalTime / 2 + /* */
            cfgp->dyn_renewalTime / 4;
    }
    return cfgp;
}

/*!
 * \brief Add variable length option.
 *
 * \param op  Pointer into the option buffer.
 * \param ot  Option type.
 * \param ov  Pointer to buffer to copy from.
 * \param len Number of octets to copy.
 *
 * \return Total number of octets added, include type and length.
 */
static size_t DhcpAddOption(uint8_t * op, uint8_t ot, void *ov, uint8_t len)
{
    *op++ = ot;
    *op++ = len;
    memcpy(op, ov, len);

    return 2 + len;
}

/*!
 * \brief Add single octet option.
 *
 * \param op Pointer into the option buffer.
 * \param ot Option type.
 * \param ov Option value.
 *
 * \return Total number of octets added, include type and length.
 */
static size_t DhcpAddByteOption(uint8_t * op, uint8_t ot, uint8_t ov)
{
    *op++ = ot;
    *op++ = 1;
    *op++ = ov;

    return 3;
}

/*!
 * \brief Add double octet option.
 *
 * \param op Pointer into the option buffer.
 * \param ot Option type.
 * \param ov Option value in host byte order. Will be converted to network
 *           byte order.
 *
 * \return Total number of octets added, include type and length.
 */
static size_t DhcpAddShortOption(uint8_t * op, uint8_t ot, uint16_t ov)
{
    *op++ = ot;
    *op++ = 2;
    ov = htons(ov);
    memcpy(op, &ov, 2);

    return 4;
}

/*!
 * \brief Add parameter request option.
 *
 * RFC 2131 demands to use the same parameter request in discover and
 * request messages.
 *
 * \param op Pointer into the option buffer.
 * \param ot Option type.
 * \param ov Option value in host byte order.
 *
 * \return Total number of octets added, include type and length.
 */
static size_t DhcpAddParmReqOption(uint8_t * op)
{
    *op++ = DHCPOPT_PARAMREQUEST;
    *op++ = 3;                  /* Adjust when adding more options! */
    *op++ = DHCPOPT_NETMASK;    /* Typically sent by default, but play safe. */
    *op++ = DHCPOPT_GATEWAY;    /* We want a default gateway. */
    *op++ = DHCPOPT_DNS;        /* We want the DNS' IP. */
    return 5;                   /* Adjust when adding more options! */
}

/*!
 * \brief Prepare a BOOTP request message header.
 *
 * A BOOTP header will be initialized at the specified buffer address
 * and the routine will add the DHCP magic cookie (RFC 1497) and an
 * additional option containing the specified DHCP message type.
 *
 * All other fields are cleared to zero.
 *
 * \param bp     Pointer to the buffer to initialize.
 * \param msgtyp DHCP message type, either \ref DHCP_DISCOVER,
 *               \ref DHCP_REQUEST or \ref DHCP_RELEASE.
 * \param xid    Random transaction identifier.
 * \param ciaddr Our IP address. Should be set to zero unless we are in
 *               BOUND, RENEW or REBINDING state and can respond to ARP
 *               requests.
 * \param secs   Seconds elapsed, since we began address acquisition or
 *               renewal.
 *
 * \return Total number of octets added.
 */
static unsigned int DhcpPrepHeader(BOOTP *bp, uint8_t msgtyp, uint32_t xid, uint32_t ciaddr, uint16_t secs)
{
    uint8_t *op;

    memset(bp, 0, sizeof(*bp));
    /* Clients send bootp requests (op code 1) only. */
    bp->bp_op = 1;
    /* Ethernet addresses are type 1 with 6 octets. */
    bp->bp_htype = 1;
    bp->bp_hlen = 6;
    memcpy(bp->bp_chaddr, confnet.cdn_mac, 6);
    /* Transaction identifier. */
    bp->bp_xid = xid;
    /* Seconds elapsed since address acquisition. */
    bp->bp_secs = htons(secs);

#ifdef DHCP_BROADCAST_FLAG
    /*
     * We do not need the broadcast flag, because our stack accepts IP
     * messages to any destination if no local address has been assigned,
     * However, we continue supporting this compile time option.
     */
    bp->bp_flags = htons(0x8000);
#endif

    bp->bp_ciaddr = ciaddr;

    /* Add the DHCP magic cookie according to RFC 1497. */
    op = bp->bp_options;
    *op++ = 0x63;
    *op++ = 0x82;
    *op++ = 0x53;
    *op++ = 0x63;

    /* Add the DHCP message type option. */
    return DhcpAddByteOption(op, DHCPOPT_MSGTYPE, msgtyp) + 4;
}

/*!
 * \brief Send a DHCP message to the server.
 *
 * This routine will also add an end of option identifier and take care,
 * that the message length will not fall below the minimum expected by
 * BOOTP.
 *
 * \param sock Socket descriptor. This pointer must have been
 *             retrieved by calling NutUdpCreateSocket().
 * \param addr Destination IP addres.
 * \param bp   Pointer to a buffer to be used for transmission.
 *             Must contain a fully initialized header and option
 *             fields.
 * \param len  Total number of DHCP option octets.
 *
 * \return 0 on success. On errors -1 is returned and \ref dhcpError will
 *         be set to \ref DHCPERR_TRANSMIT.
 */
static int DhcpSendMessage(UDPSOCKET * sock, uint32_t addr, BOOTP *bp, size_t len)
{
    /* Add 'end of options'. */
    bp->bp_options[len++] = DHCPOPT_END;

    /* Maintain a BOOTP compatible minimum packet size of 300 octets.
       Thanks to Tomohiro Haraikawa. */
    if ((len += sizeof(BOOTP) - sizeof(bp->bp_options)) < MIN_DHCP_MSGSIZE) {
        len = MIN_DHCP_MSGSIZE;
    }
#ifdef NUTDEBUG
    if (__tcp_trf & NET_DBG_DHCP) {
        fprintf(__tcp_trs, "[DHCP-Send to %s]", inet_ntoa(addr));
    }
#endif
    if (NutUdpSendTo(sock, addr, DHCP_SERVERPORT, bp, len) < 0) {
        dhcpError = DHCPERR_TRANSMIT;
        return -1;
    }
    return 0;
}

/*!
 * \brief Receive a DHCP reply from the server.
 *
 * \param sock Socket descriptor.
 * \param xid  Expected transaction identifier. Incoming messages with a
 *             different identifier are silently discarded.
 * \param bp   Pointer to the receive buffer.
 * \param tmo  Maximum number of milliseconds to wait for a valid message.
 *
 * \return The number of bytes received, if successful. The return
 *         value -1 indicates an error, in which case dhcpError is
 *         set to an error code. On timeout 0 is returned.
 */
static int DhcpRecvMessage(UDPSOCKET * sock, uint32_t xid, BOOTP *bp, uint32_t tmo)
{
    int rc;
    uint16_t port;
    uint32_t addr;
    uint32_t etim;
    uint32_t wtim;

    /* Set our start time. */
    etim = NutGetMillis();
    /* Set the initial receive timeout. */
    wtim = tmo;
    for (;;) {
        rc = NutUdpReceiveFrom(sock, &addr, &port, bp, sizeof(BOOTP), wtim);
#ifdef NUTDEBUG
        if (__tcp_trf & NET_DBG_DHCP) {
            if (rc > 0) {
                fprintf(__tcp_trs, "[DHCP-Recv from %s]", inet_ntoa(addr));
            } else if (rc < 0) {
                fprintf(__tcp_trs, "[DHCP-Recv Error]");
            } else {
                fprintf(__tcp_trs, "[DHCP-Recv Timeout %lu]", tmo);
            }
        }
#endif
        /* Immediately return on receive errors and timeouts. */
        if (rc <= 0) {
            if (rc < 0) {
                dhcpError = DHCPERR_RECEIVE;
            }
            break;
        }
        /* The message must at least include the BOOTP header plus five
           bytes of options (magic and end). We are quite liberal here. */
        if (rc > sizeof(BOOTP) - sizeof(bp->bp_options) + 5) {
            /* The message must be a BOOTP reply with the expected XID. */
            if (bp->bp_op == 2 && bp->bp_xid == xid) {
                /* Message is acceptable. */
                break;
            }
        }
        /* Calculate the remaining timeout for not getting trapped here
           on a busy network, which regularly broadcasts DHCP messages. */
        wtim = NutGetMillis() - etim;
        if (wtim >= tmo - 250) {
            /* Less than 250 ms left, return timeout. */
            rc = 0;
            break;
        }
        wtim = tmo - wtim;
    }
    return rc;
}

/*!
 * \brief Broadcast a DHCP discover message.
 *
 * \param sock  Socket descriptor. This pointer must have been retrieved
 *              by calling NutUdpCreateSocket().
 * \param bp    Pointer to a buffer to be used for transmission. No specific
 *              initialization required.
 * \param xid   Random transaction identifier.
 * \param raddr Requested IP address. Optional.
 * \param secs  Seconds elapsed since start of address acquisition. Related
 *              requests must use the same value.
 *
 * \return 0 on success, -1 if send failed.
 */
static int DhcpBroadcastDiscover(UDPSOCKET * sock, BOOTP *bp, uint32_t xid, uint32_t raddr, uint16_t secs)
{
    size_t optlen;
    int len;
    uint8_t *op = bp->bp_options;

    optlen = DhcpPrepHeader(bp, DHCP_DISCOVER, xid, 0, secs);

    /* Request a specific IP if one had been assigned previously. */
    if (raddr) {
        optlen += DhcpAddOption(op + optlen, DHCPOPT_REQESTIP, &raddr, sizeof(raddr));
    }

    optlen += DhcpAddParmReqOption(op + optlen);

    /* Pass host name if specified in confos structure.
     * Win2k DHCP server can register this as dynamic DNS entry.
     * Also viewing DHCP lease table shows something sensible.
     */
    len = strlen(confos.hostname);
    if (len > 0) {
        optlen += DhcpAddOption(op + optlen, DHCPOPT_HOSTNAME, confos.hostname, len);
    }

    /* Request a maximum message size. */
    optlen += DhcpAddShortOption(op + optlen, DHCPOPT_MAXMSGSIZE, MAX_DHCP_MSGSIZE);

    return DhcpSendMessage(sock, INADDR_BROADCAST, bp, optlen);
}


/*!
 * \brief Send a DHCP request message.
 *
 * \param sock  Socket descriptor. This pointer must have been retrieved by
 *              calling NutUdpCreateSocket().
 * \param daddr IP address of the DHCP server or \ref INADDR_BROADCAST.
 * \param bp    Pointer to a buffer to be used for transmission.
 * \param xid   Random transaction identifier.
 * \param caddr Our IP address. Should be set only if we are able to respond
 *              to ARP requests. Otherwise must be set to 0.
 * \param raddr Requested IP address. Required.
 * \param sid   Server identifier. If this request is not an offer response,
 *              then set it to zero.
 * \param secs  Seconds elapsed since start of address acquisition. If this
 *              request is sent in reponse to an offer, the same value must
 *              be used.
 *
 * \return 0 on success, -1 if send failed.
 */
static int DhcpSendRequest(UDPSOCKET * sock, uint32_t daddr, BOOTP *bp, uint32_t xid,        /* */
                           uint32_t caddr, uint32_t raddr, uint32_t sid, uint16_t secs)
{
    size_t optlen;
    int len;
    uint8_t *op = bp->bp_options;

    /* Initialize the BOOTP header. */
    optlen = DhcpPrepHeader(bp, DHCP_REQUEST, xid, caddr, secs);

    /* Add specified options. */
    if (raddr) {
        optlen += DhcpAddOption(op + optlen, DHCPOPT_REQESTIP, &raddr, sizeof(raddr));
    }
    if (sid) {
        optlen += DhcpAddOption(op + optlen, DHCPOPT_SID, &sid, sizeof(sid));
    }
    optlen += DhcpAddParmReqOption(op + optlen);

    /* Pass host name if specified in confos structure.  */
    /* viewing DHCP lease table shows something sensible. */
    len = strlen(confos.hostname);
    if (len > 0) {
        optlen += DhcpAddOption(op + optlen, DHCPOPT_HOSTNAME, confos.hostname, len);
    }

    return DhcpSendMessage(sock, daddr, bp, optlen);
}

/*!
 * \brief Broadcast a DHCP request message.
 *
 * \param sock  Socket descriptor. This pointer must have been retrieved by
 *              calling NutUdpCreateSocket().
 * \param bp    Pointer to a buffer to be used for transmission.
 * \param xid   Random transaction identifier.
 * \param caddr Our IP address. Should be set only if we are able to respond
 *              to ARP requests. Otherwise must be set to 0.
 * \param raddr Requested IP address. Required.
 * \param sid   Server identifier. If this request is not an offer response,
 *              then set it to zero.
 * \param secs  Seconds elapsed since start of address acquisition. If this
 *              request is sent in reponse to an offer, the same value must
 *              be used.
 *
 * \return 0 on success, -1 if send failed.
 */
static int DhcpBroadcastRequest(UDPSOCKET * sock, BOOTP *bp, uint32_t xid, /* */
                                uint32_t caddr, uint32_t raddr, uint32_t sid, uint16_t secs)
{
    return DhcpSendRequest(sock, INADDR_BROADCAST, bp, xid, caddr, raddr, sid, secs);
}

/*!
 * \brief Relinguish our DHCP lease.
 *
 * \param sock  Socket descriptor. This pointer must have been retrieved by
 *              calling NutUdpCreateSocket().
 * \param daddr IP address of the DHCP server.
 * \param bp    Pointer to a buffer to be used for transmission.
 * \param xid   Random transaction identifier.
 * \param caddr Our IP address. Should be set only if we are able to respond
 *              to ARP requests. Otherwise must be set to 0.
 * \param sid   Server identifier. If this request is not an offer response,
 *              then set it to zero.
 *
 * \return 0 on success, -1 if send failed.
 */
static int DhcpSendRelease(UDPSOCKET * sock, uint32_t daddr, BOOTP *bp, uint32_t xid, uint32_t caddr, uint32_t sid)
{
    size_t optlen;
    uint8_t *op = bp->bp_options;

    /* Prepare BOOTP header. 'secs' is set to zero. */
    optlen = DhcpPrepHeader(bp, DHCP_RELEASE, xid, caddr, 0);

    /* Optionally add server identifier. */
    if (sid) {
        optlen += DhcpAddOption(op + optlen, DHCPOPT_SID, &sid, sizeof(sid));
    }
    return DhcpSendMessage(sock, daddr, bp, optlen);
}

/*!
 * \brief Inform DHCP about an externally allocated address.
 *
 * \param sock  Socket descriptor. This pointer must have been retrieved by
 *              calling NutUdpCreateSocket().
 * \param daddr IP address of the DHCP server or INADDR_BROADCAST.
 * \param bp    Pointer to a buffer to be used for transmission.
 * \param xid   Random transaction identifier.
 * \param caddr Our IP address. Required.
 *
 * \return 0 on success, -1 if send failed.
 */
static int DhcpSendInform(UDPSOCKET * sock, uint32_t daddr, BOOTP *bp, uint32_t xid, uint32_t caddr)
{
    size_t optlen;
    size_t len;
    uint8_t *op = bp->bp_options;

    /* Prepare BOOTP header. 'secs' is set to zero. */
    optlen = DhcpPrepHeader(bp, DHCP_INFORM, xid, caddr, 0);

    /* Additional options we want. */
    optlen += DhcpAddParmReqOption(op + optlen);

    /* Add our configured host name. */
    len = strlen(confos.hostname);
    if (len > 0) {
        optlen += DhcpAddOption(op + optlen, DHCPOPT_HOSTNAME, confos.hostname, len);
    }

    /* We should provide the maximum message size. */
    optlen += DhcpAddShortOption(op + optlen, DHCPOPT_MAXMSGSIZE, MAX_DHCP_MSGSIZE);

    return DhcpSendMessage(sock, daddr, bp, optlen);
}


/*!
 * \brief Check a new offer.
 *
 * \param dyncfg Current configuration, may be NULL.
 * \param ip     DHCP server IP address.
 * \param bp     DHCP offer message.
 * \param len    DHCP message length.
 *
 * \return Updated configuration.
 */
static DYNCFG *CheckOffer(DYNCFG * dyncfg, BOOTP *bp, size_t len)
{
    DYNCFG *offer;

#ifdef NUTDEBUG
    if (__tcp_trf & NET_DBG_DHCP) fprintf(__tcp_trs, "\n[chkoffer %s ", inet_ntoa( dyncfg->dyn_yiaddr));
#endif

    /* Parse the new offer. If it's invalid, return the current
       configuration. */
    if ((offer = ParseReply(bp, len)) == 0) {
#ifdef NUTDEBUG
        if (__tcp_trf & NET_DBG_DHCP) fprintf(__tcp_trs, "TAKE: %s]", inet_ntoa( offer->dyn_yiaddr));
#endif
        return dyncfg;
    }

    /* Discard anything which is not an offer. */
    if (offer->dyn_msgtyp != DHCP_OFFER) {
        ReleaseDynCfg(offer);
#ifdef NUTDEBUG
        if (__tcp_trf & NET_DBG_DHCP) fprintf(__tcp_trs, "NO-OFFER]");
#endif
        return dyncfg;
    }

    /* First offer, take it. */
    if (dyncfg == 0) {
        dyncfg = offer;
#ifdef NUTDEBUG
        if (__tcp_trf & NET_DBG_DHCP) fprintf(__tcp_trs, "FIRST: %s]", inet_ntoa(offer->dyn_yiaddr));
#endif
    }

    /*
     * Check if the new offer is better than the current configuration:
     */
    else {
        /* If we remember a previously allocated IP which isn't in the
           current configuration but in the new offer, then let's take
           the new one. */
#ifdef NUTDEBUG
        if (__tcp_trf & NET_DBG_DHCP) fprintf(__tcp_trs, "PREV ");
#endif
        if (confnet.cdn_ip_addr & confnet.cdn_ip_mask) {
            if (dyncfg->dyn_yiaddr != confnet.cdn_ip_addr &&    /* */
                offer->dyn_yiaddr == confnet.cdn_ip_addr) {
                ReleaseDynCfg(dyncfg);
                dyncfg = offer;
#ifdef NUTDEBUG
                if (__tcp_trf & NET_DBG_DHCP) fprintf(__tcp_trs, "OLD: %s]", inet_ntoa( dyncfg->dyn_yiaddr));
#endif
            }
        }
        /* In the second place prefer long lease times. */
        else if (offer->dyn_leaseTime > dyncfg->dyn_leaseTime) {
            ReleaseDynCfg(dyncfg);
            dyncfg = offer;
#ifdef NUTDEBUG
            if (__tcp_trf & NET_DBG_DHCP) fprintf(__tcp_trs, "LONG: %s]", inet_ntoa( dyncfg->dyn_yiaddr));
#endif
        }
        /* The new one deosn't offer anything interesting. Discard it. */
        else {
            ReleaseDynCfg(offer);
#ifdef NUTDEBUG
            if (__tcp_trf & NET_DBG_DHCP) fprintf(__tcp_trs, "DSIC]");
#endif
        }
    }
    return dyncfg;
}

#ifdef NUTDEBUG
void DhcpStateDebug( uint_fast8_t n, uint_fast8_t s, int li, int lt, ureg_t retries)
{
    if (__tcp_trf & NET_DBG_DHCP) {
        if( n)
            fprintf(__tcp_trs, "\n[%u.DHCP-", retries + 1);
        else
            fprintf(__tcp_trs, "\n->[%u.DHCP-", retries + 1);

        switch (dhcpState) {
        case DHCPST_INIT:
            fprintf(__tcp_trs, "INIT]");
            break;
        case DHCPST_SELECTING:
            fprintf(__tcp_trs, "SELECTING]");
            break;
        case DHCPST_REQUESTING:
            fprintf(__tcp_trs, "REQUESTING]");
            break;
        case DHCPST_REBOOTING:
            fprintf(__tcp_trs, "REBOOTING %s]", inet_ntoa(li));
            break;
        case DHCPST_BOUND:
            fprintf(__tcp_trs, "BOUND %lu]", NutGetSeconds() - lt);
            break;
        case DHCPST_RENEWING:
            fprintf(__tcp_trs, "RENEWING %lu]", NutGetSeconds() - lt);
            break;
        case DHCPST_REBINDING:
            fprintf(__tcp_trs, "REBINDING %lu]", NutGetSeconds() - lt);
            break;
        case DHCPST_INFORMING:
            fprintf(__tcp_trs, "INFORMING]");
            break;
        case DHCPST_RELEASING:
            fprintf(__tcp_trs, "RELEASING]");
            break;
        case DHCPST_IDLE:
            if (dhcpError) {
                fprintf(__tcp_trs, "ERROR %u]", dhcpError);
            } else {
                fprintf(__tcp_trs, "IDLE]");
            }
            break;
        default:
            fprintf(__tcp_trs, "UNKNOWN %u]", dhcpState);
            break;
        }
    }
}
#endif

/*! \fn NutDhcpClient(void *arg)
 * \brief DHCP client thread.
 *
 * This thread implements a DHCP state machine and is automatically started
 * when calling NutDhcpIfConfig() or NutDhcpInform().
 *
 * \bug We are not able to shutdown our interface, which may cause problems
 *      if out original DHCP server dies.
 *
 * \todo We are using a bunch of global variables, which must be associated
 *       to a specific interfase if we want to support more than one
 *       Ethernet port.
 */
THREAD(NutDhcpClient, arg)
{
    DYNCFG *reply = 0;
    UDPSOCKET *sock = 0;
    BOOTP *bp = 0;
    int n;
    uint32_t xid;
    IFNET *nif;
    uint16_t secs = 0;
    uint32_t aqsTime = NutGetSeconds();
    uint32_t leaseTime = 0;
    uint32_t napTime;
    ureg_t retries;
    uint32_t tmo = MIN_DHCP_WAIT;
    uint32_t last_ip = confnet.cdn_ip_addr;
    uint32_t server_ip;

    /*
     * Hack alert: Our ARM port doesn't allow parameter
     * passing to threads.
     */
#ifdef __arm__
    nif = dhcpDev->dev_icb;
#else
    nif = ((NUTDEVICE *) arg)->dev_icb;
#endif

    /*
     * Generate a random transaction identifier based on our MAC
     * address with the least significant byte of the MAC address
     * becoming the most significant byte of the identifier. This
     * should give a sufficient unique value when several Ethernuts
     * are started concurrently.
     */
    xid = 0;
    for (retries = 0; retries < sizeof(xid); retries++) {
        xid <<= 8;
        xid += nif->if_mac[5 - retries];
    }
    retries = 0;

    for (;;) {

#ifdef NUTDEBUG
        DhcpStateDebug( 0, dhcpState, last_ip, leaseTime, retries);
#endif

        /*
         * Setup some values based on the number of retry attempts.
         */
        server_ip = INADDR_BROADCAST;   /* Broadcasting is default. */
        if (retries) {
            /* Double our timeout on each retry. */
            tmo += tmo;
            if (tmo > MAX_DHCP_WAIT) {
                tmo = MAX_DHCP_WAIT;
            }
        } else {
            /* Start with minimum timeout first. */
            tmo = MIN_DHCP_WAIT;
            /* Use a new xid for the first message in each state except
             * when requesting, where we should continue using the xid
             * from the offer message we received.
             */
            if (dhcpState != DHCPST_REQUESTING) {
                xid++;
            }

            /* If we know the server's IP, try to unicast on the first
               attempt. */
            if (dhcpConfig && dhcpConfig->dyn_sid) {
                server_ip = dhcpConfig->dyn_sid;
            }
        }

        /*
         * Keep track of the API timeout.
         */
        if (dhcpState != DHCPST_IDLE && dhcpApiTimeout != NUT_WAIT_INFINITE) {
            uint32_t tt = NutGetMillis() - dhcpApiStart;

            if (dhcpApiTimeout <= tt) {
                dhcpError = DHCPERR_TIMEOUT;
                dhcpState = DHCPST_IDLE;
                continue;
            }
            if ((tt = dhcpApiTimeout - tt) < tmo) {
                tmo = tt;
            }
        }

        /*
         * Keep track of acquisition time.
         */
        if ((dhcpState == DHCPST_SELECTING) || (dhcpState == DHCPST_RENEWING) || (dhcpState == DHCPST_REBINDING)) {
            /* For retries make sure that secs doesn't overflow. */
            if (retries) {
                if (NutGetSeconds() - aqsTime > 0xffffUL) {
                    secs = 0xffff;
                } else {
                    secs = (uint16_t) (NutGetSeconds() - aqsTime);
                }
            }
            /* For first transmissions make sure that secs is zero. */
            else {
                aqsTime = NutGetSeconds();
                secs = 0;
            }
        }

        /*
         * Release UDP socket and buffer in states with long inactive time.
         */
        if ((dhcpState == DHCPST_BOUND) || (dhcpState == DHCPST_IDLE)) {
            if (sock) {
                NutUdpDestroySocket(sock);
                sock = 0;
            }
            if (bp) {
                free(bp);
                bp = 0;
            }
        }

        /*
         * In all other states we need the socket and the buffer.
         */
        else {
            /*
             * Check if something else configured our interface.
             */
            if (dhcpConfig == 0 && nif->if_local_ip) {
                /* If we need additional configuration, we can sent
                   a DHCP Inform message here. */
                dhcpState = DHCPST_IDLE;
                continue;
            }

            if ((sock == 0) || (bp == 0)) {
                if (sock == 0) {
                    sock = NutUdpCreateSocket(DHCP_CLIENTPORT);
                }
                if (bp == 0) {
                    bp = malloc(sizeof(BOOTP));
                }
                if ((sock == 0) || (bp == 0)) {
                    /* Looks like we are out of memory. */
                    dhcpError = DHCPERR_SYSTEM;
                    dhcpState = DHCPST_IDLE;
                    /* At this point either socket or buffer may be allocated.
                       Thus we need to jump back to the top of our state loop
                       to release it. */
                    continue;
                }
#if MAX_DHCP_BUFSIZE
                {
                    uint16_t max_ms = MAX_DHCP_BUFSIZE;
                    NutUdpSetSockOpt(sock, SO_RCVBUF, &max_ms, sizeof(max_ms));
                }
#endif
            }
        }

        /*
         * (Re)Start.
         */
        if (dhcpState == DHCPST_INIT) {
            /* Clear the retry counter. */
            retries = 0;
            /* Use a new XID on each attempt. */
            xid++;
            /* Determine whether this is an initial boot or a reboot. */
            if ((last_ip & confnet.cdn_ip_mask) == 0) {
                /* No previous IP, start from ground up. */
                dhcpState = DHCPST_SELECTING;
            } else {
                /* We got a previously allocated IP configuration from
                 * non-volatile configuration memory. Try to re-use it. */
                dhcpState = DHCPST_REBOOTING;
            }
        }

#ifdef NUTDEBUG
//        DhcpStateDebug( 1, dhcpState, last_ip, leaseTime, retries);
#endif
        /*
         * Broadcast discover and collect incoming offers.
         */
        else if (dhcpState == DHCPST_SELECTING) {
#ifdef NUTDEBUG
            if (__tcp_trf & NET_DBG_DHCP) fprintf(__tcp_trs, "[SEL: TICK %p]", dhcpConfig);
#endif

            if (retries++ > MAX_DCHP_RETRIES) {
                if( dhcpConfig) {
                    /* Ulrich says: we got at least one valid offer from a DHCP server */
                    reply = 0;
                    leaseTime = aqsTime;
                    dhcpState = DHCPST_BOUND;
                }
                else {
                    /* Too many retries while discovering DHCP. Give up. */
                    dhcpError = DHCPERR_TIMEOUT;
                    dhcpState = DHCPST_IDLE;
                }
            }
            /* Send the discovering broadcast. */
            else if (DhcpBroadcastDiscover(sock, bp, xid, last_ip, secs) < 0) {
                /* Fatal transmit error on broadcast. */
#ifdef NUTDEBUG
                if (__tcp_trf & NET_DBG_DHCP) fprintf(__tcp_trs, "[SEL-TXERR]");
#endif
                dhcpState = DHCPST_IDLE;
            } else {
                /* Collect incoming offers. */
                while ((n = DhcpRecvMessage(sock, xid, bp, tmo)) > 0) {
                    /* Check if this is a valid offer. */
                    if ((dhcpConfig = CheckOffer(dhcpConfig, bp, n)) != 0) {
#ifdef NUTDEBUG
                        if (__tcp_trf & NET_DBG_DHCP) fprintf(__tcp_trs, "[SEL-OFFER n=%d]", n);
#endif
                        /* If the callers timeout is low, do not collect
                           more than one. Thanks to Jelle Kok. */
                        if (dhcpApiTimeout < MIN_DHCP_WAIT * 3) {
#ifdef NUTDEBUG
                            if (__tcp_trf & NET_DBG_DHCP) fprintf(__tcp_trs, "[NOTL]");
#endif
                            break;
                        }
                        /* Switch to lowest timeout after we received
                           a first response. */
                        tmo = MIN_DHCP_WAIT;
                    }
                }
                /* Change to ERROR state on fatal receive errors. */
                if (n < 0) {
#ifdef NUTDEBUG
                    if (__tcp_trf & NET_DBG_DHCP) fprintf(__tcp_trs, "[SEL-FATERR]");
#endif
                    dhcpState = DHCPST_IDLE;
                }
                /* Change to REQUESTING state if we got a valid offer.
                   Otherwise we stay in SELECTING state. */
                else if (dhcpConfig) {
                    /* Wait for at least one period for other DHCP servers to offer */
                    retries = MAX_DCHP_RETRIES+1;
//                    dhcpState = DHCPST_REQUESTING;
#ifdef NUTDEBUG
                    if (__tcp_trf & NET_DBG_DHCP) fprintf(__tcp_trs, "[SEL-WAIT]");
#endif
                }
            }
        }

        /*
         * Send request and wait for an acknowledge.
         */
        else if (dhcpState == DHCPST_REQUESTING) {
            if (retries++ > MAX_DCHP_RETRIES) {
                /* Too many retries with this server, fall back to discovery. */
                dhcpState = DHCPST_INIT;
            }
            /* Request an offered configuration. According to RFC 2131 this
               has to be broadcasted. */
            else if (DhcpBroadcastRequest(sock, bp, xid, 0, dhcpConfig->dyn_yiaddr, dhcpConfig->dyn_sid, secs) < 0) {
                /* Fatal transmit error on broadcast. Give up. */
                dhcpState = DHCPST_IDLE;
            } else if ((n = DhcpRecvMessage(sock, xid, bp, tmo)) < 0) {
                /* Fatal receive error. */
                dhcpState = DHCPST_IDLE;
            } else if (n > 0 && (reply = ParseReply(bp, n)) != 0) {
                /* The server accepted our request. We are bound. */
                if (reply->dyn_msgtyp == DHCP_ACK) {
                    ReleaseDynCfg(dhcpConfig);
                    dhcpConfig = reply;
                    reply = 0;
                    leaseTime = aqsTime;
                    dhcpState = DHCPST_BOUND;
                }
                /* The server declines a previously offered configuration.
                   Restart discovery. */
                else if (reply->dyn_msgtyp == DHCP_NAK) {
#ifdef NUTDEBUG
        if (__tcp_trf & NET_DBG_DHCP) fprintf(__tcp_trs, "[DHCP-NAK1]");
#endif
                    dhcpState = DHCPST_INIT;
                }
            }
        }

        /*
         * Reusing a previously allocated network address after reboot.
         */
        else if (dhcpState == DHCPST_REBOOTING) {
            if (++retries > MAX_DCHP_RETRIES) {
                /* Too many retries, fall back to discovery. */
                last_ip = 0;
                dhcpState = DHCPST_INIT;
            }
            /* Broadcast a request for our previous configuration. */
            else if (DhcpBroadcastRequest(sock, bp, xid, 0, last_ip, 0, secs) < 0) {
                /* Fatal transmit error on broadcast. Give up. */
#ifdef NUTDEBUG
                    if (__tcp_trf & NET_DBG_DHCP) fprintf(__tcp_trs, "[BC-FATAL]");
#endif
                dhcpState = DHCPST_IDLE;
            } else if ((n = DhcpRecvMessage(sock, xid, bp, tmo)) < 0) {
                /* Fatal receive error. */
#ifdef NUTDEBUG
                    if (__tcp_trf & NET_DBG_DHCP) fprintf(__tcp_trs, "[RCV-FATAL]");
#endif
                dhcpState = DHCPST_IDLE;
            } else if (n > 0 && (reply = ParseReply(bp, n)) != 0) {
                if (reply->dyn_msgtyp == DHCP_ACK) {
                    ReleaseDynCfg(dhcpConfig);
                    dhcpConfig = reply;
                    reply = 0;
                    leaseTime = aqsTime;
                    dhcpState = DHCPST_BOUND;
#ifdef NUTDEBUG
                    if (__tcp_trf & NET_DBG_DHCP) fprintf(__tcp_trs, "[OK]");
#endif
                } else if (reply->dyn_msgtyp == DHCP_NAK) {
                    /* Either our previous address had been allocated by
                       someone else or we changed the network. Remove the
                       previous address and restart. */
#ifdef NUTDEBUG
                    if (__tcp_trf & NET_DBG_DHCP) fprintf(__tcp_trs, "[DHCP-NAK2]");
#endif
                    last_ip = 0;
                    dhcpState = DHCPST_INIT;
                }
            }
        }

        /*
         * Maintain lease time.
         */
        else if (dhcpState == DHCPST_BOUND) {
            retries = 0;
            NutEventBroadcast(&dhcpDone);
            if (dhcpConfig->dyn_renewalTime <= NutGetSeconds() - leaseTime) {
                dhcpState = DHCPST_RENEWING;
            } else {
                /* Calculate the remaining lease time and take a nap. */
                napTime = dhcpConfig->dyn_renewalTime - (NutGetSeconds() - leaseTime);
                if (napTime > MAX_DHCP_NAPTIME) {
                    napTime = MAX_DHCP_NAPTIME;
                }
                NutEventWait(&dhcpWake, napTime * 1000UL);
            }
        }

        /*
         * Waiting for an acknowledge of our renewal request.
         */
        else if (dhcpState == DHCPST_RENEWING) {
            retries++;
            if (tmo / 1000 > dhcpConfig->dyn_rebindTime - (NutGetSeconds() - leaseTime)) {
                tmo = dhcpConfig->dyn_rebindTime - (NutGetSeconds() - leaseTime) * 1000;
            }
            if (dhcpConfig->dyn_rebindTime <= NutGetSeconds() - leaseTime) {
                retries = 0;
                dhcpState = DHCPST_REBINDING;
            }
            /* Send a request to our leasing server. We must not include
               the server identifier. */
            else if (DhcpSendRequest(sock, dhcpConfig->dyn_sid, bp, xid, dhcpConfig->dyn_yiaddr, dhcpConfig->dyn_yiaddr, 0, secs) <
                     0) {
                /* Unicast transmit error. */
                retries = 0;
                dhcpState = DHCPST_REBINDING;
            } else if ((n = DhcpRecvMessage(sock, xid, bp, tmo)) < 0) {
                /* Fatal receive error. */
                dhcpState = DHCPST_IDLE;
            } else if (n > 0 && (reply = ParseReply(bp, n)) != 0) {
                if (reply->dyn_msgtyp == DHCP_ACK) {
                    /* Got an acknowledge, return to bound state. */
                    ReleaseDynCfg(dhcpConfig);
                    dhcpConfig = reply;
                    reply = 0;
                    leaseTime = aqsTime;
                    dhcpState = DHCPST_BOUND;
                } else if (reply->dyn_msgtyp == DHCP_NAK) {
                    /* Unexpected NAK. */
#ifdef NUTDEBUG
                    if (__tcp_trf & NET_DBG_DHCP) fprintf(__tcp_trs, "[DHCP-UXNAK]");
#endif
                    retries = 0;
                    dhcpState = DHCPST_REBINDING;
                }
            }
        }

        /*
         * Waiting for an acknowledge of our rebind request.
         */
        else if (dhcpState == DHCPST_REBINDING) {
            retries++;
            if (tmo > dhcpConfig->dyn_rebindTime - (NutGetSeconds() - leaseTime)) {
                tmo = dhcpConfig->dyn_rebindTime - NutGetSeconds() - leaseTime;
            }
            if (dhcpConfig->dyn_rebindTime <= NutGetSeconds() - leaseTime) {
                retries = 0;
                dhcpState = DHCPST_REBINDING;
            }
            /* Broadcast a request for our previous configuration. We
               must not include a server identifier. */
            else if (DhcpBroadcastRequest(sock, bp, xid, dhcpConfig->dyn_yiaddr, dhcpConfig->dyn_yiaddr, 0, secs) < 0) {
                /* Fatal transmit error on broadcast. Give up. */
                dhcpState = DHCPST_IDLE;
            } else if ((n = DhcpRecvMessage(sock, xid, bp, tmo)) < 0) {
                /* Fatal receive error. */
                dhcpState = DHCPST_IDLE;
            } else if (n > 0 && (reply = ParseReply(bp, n)) != 0) {
                if (reply->dyn_msgtyp == DHCP_ACK) {
                    /* Got an acknowledge, return to bound state. */
                    ReleaseDynCfg(dhcpConfig);
                    dhcpConfig = reply;
                    reply = 0;
                    leaseTime = aqsTime;
                    dhcpState = DHCPST_BOUND;
                } else if (reply->dyn_msgtyp == DHCP_NAK) {
                    /*
                     * We have a problem here if the last DHCP server died.
                     * If a backup server exists, it may probe our IP address
                     * using ARP or ICMP. Our interface is up and responding,
                     * so the backup server may think that the IP address
                     * is in use and respond with NAK. Without shutting
                     * down our interface (not yet implemented) we are stuck.
                     * We switch to discovery state, but the problem remains.
                     */
#ifdef NUTDEBUG
                    if (__tcp_trf & NET_DBG_DHCP) fprintf(__tcp_trs, "[DHCP-NAK3]");
#endif
                    dhcpState = DHCPST_INIT;
                }
            }
        }

        /*
         * Send an inform and wait for its (optional) echo.
         */
        else if (dhcpState == DHCPST_INFORMING) {
            if (retries++ > MAX_DCHP_RETRIES) {
                dhcpState = DHCPST_IDLE;
            } else if (DhcpSendInform(sock, server_ip, bp, xid, nif->if_local_ip) < 0) {
                if (server_ip == INADDR_BROADCAST) {
                    dhcpState = DHCPST_IDLE;
                }
            } else if ((n = DhcpRecvMessage(sock, xid, bp, tmo)) != 0) {
                if (n > 0 &&    /* No receive error. */
                    (reply = ParseReply(bp, n)) != 0 && /* No parser error. */
                    reply->dyn_msgtyp == DHCP_ACK) {    /* Acknowledged. */
                    /* Take over this configuration. */
                    ReleaseDynCfg(dhcpConfig);
                    dhcpConfig = reply;
                    reply = 0;
                }
                dhcpState = DHCPST_IDLE;
            }
        }

        /*
         * Send a release and wait for its (optional) echo.
         */
        else if (dhcpState == DHCPST_RELEASING) {
            if (dhcpConfig == 0 ||      /* Not configured. */
                retries++ > MAX_DCHP_RELEASE_RETRIES || /* Too many retries. */
                DhcpSendRelease(sock, server_ip, bp, xid, dhcpConfig->dyn_yiaddr, dhcpConfig->dyn_sid) < 0) {
                if (server_ip == INADDR_BROADCAST) {
                    dhcpState = DHCPST_IDLE;
                }
            } else if ((n = DhcpRecvMessage(sock, xid, bp, tmo)) < 0) {
                /* Fatal receive error. */
                dhcpState = DHCPST_IDLE;
            } else if (n > 0 && (reply = ParseReply(bp, n)) != 0) {
                if (reply->dyn_msgtyp == DHCP_ACK) {
                    dhcpState = DHCPST_IDLE;
                } else if (reply->dyn_msgtyp == DHCP_NAK) {
#ifdef NUTDEBUG
                    if (__tcp_trf & NET_DBG_DHCP) fprintf(__tcp_trs, "[DHCP-FNAK]");
#endif
                    dhcpState = DHCPST_IDLE;
                }
            }
        }

        /*
         * We are done somehow. Either a fatal error occured or we
         * reached the specified timeout time or our lease has been
         * release or something else configured our interface.
         * Release all resources and wait for a new API call to
         * wake us up.
         */
        else if (dhcpState == DHCPST_IDLE) {
            ReleaseDynCfg(dhcpConfig);
            dhcpConfig = 0;
            retries = 0;
            NutEventBroadcast(&dhcpDone);
            NutEventWait(&dhcpWake, NUT_WAIT_INFINITE);
        }

        /* Release any received reply. */
        if (reply) {
            ReleaseDynCfg(reply);
            reply = 0;
        }
    }
}

/*!
 * \brief Activate the DHCP client thread.
 *
 * Start the DHCP thread if not running or wake it up. Pass the caller's
 * timeout to the thread and wait an infinite time. We rely on the thread
 * to wake us up on timeout.
 *
 * \param name    Name of the registered Ethernet device.
 * \param state   State to start with.
 * \param timeout Maximum number of milliseconds to wait. To disable
 *                timeout, set this parameter to \ref NUT_WAIT_INFINITE.
 *                This value must be larger than 3 times of \ref MIN_DHCP_WAIT
 *                to enable collection of offers from multiple servers.
 */
static int DhcpKick(CONST char *name, uint8_t state, uint32_t timeout)
{
    NUTDEVICE *dev;
    IFNET *nif;

    /* Lookup the Ethernet device. */
    if ((dev = NutDeviceLookup(name)) == 0 ||   /* No device */
        dev->dev_type != IFTYP_NET ||   /* Wrong type */
        (nif = dev->dev_icb) == 0 ||    /* No netif */
        nif->if_type != IFT_ETHER) {    /* Wrong if type */
        dhcpError = DHCPERR_BADDEV;
        return -1;
    }

    /* Initialize timeout checking. */
    dhcpApiStart = NutGetMillis();
    dhcpApiTimeout = timeout;

    dhcpState = state;
    if (dhcpThread == 0) {
#ifdef __arm__
        dhcpDev = dev;
#endif
        dhcpThread = NutThreadCreate("dhcpc", NutDhcpClient, dev,
            (NUT_THREAD_DHCPSTACK * NUT_THREAD_STACK_MULT) + NUT_THREAD_STACK_ADD);
    }
    NutEventPost(&dhcpWake);
    NutEventWait(&dhcpDone, NUT_WAIT_INFINITE);

    return 0;
}

/*!
 * \brief Automatically configure an Ethernet network interface.
 *
 * If no MAC address is specified, this routine will try to read a
 * previously stored configuration from the EEPROM. If this retrieves
 * a fixed IP configuration, then the network interface will be
 * immediately configured with these values by calling NutNetIfConfig().
 * If no valid IP configuration has been read, then this routine will
 * start the DHCP client thread and wait upto a given number of
 * milliseconds for an acknowledged configuration from a DHCP server.
 *
 * If a MAC address has been specified, this routine will not read the
 * EEPROM configuration. If the application has set the global
 * \ref CONFNET structure to a valid IP configuration before calling
 * this function, then the network interface will be immediately
 * configured with these values by calling NutNetIfConfig(). Otherwise
 * the DHCP client thread will be started and this routine will wait
 * upto a given number of milliseconds for an acknowledged configuration
 * from a DHCP server.
 *
 * \param name    Name of the registered Ethernet device.
 * \param mac     MAC address of the destination. Set NULL to use the
 *                configuration stored in the EEPROM.
 * \param timeout Maximum number of milliseconds to wait. To disable
 *                timeout, set this parameter to \ref NUT_WAIT_INFINITE.
 *                Otherwise the value must be larger than 3 times of
 *                \ref MIN_DHCP_WAIT to enable collection of offers
 *                from multiple servers.
 *
 * \return 0 if the interface had been successfully configured. In most
 *         cases this information is sufficient, because the application
 *         will not care whether the configuration had been provided by
 *         a DHCP server or EEPROM values. However, if EEPROM values had
 *         been used, then no DNS servers had been set. The application
 *         can call NutDhcpStatus() to check the DHCP bound state. -1 is
 *         returned if the interface configuration failed. In this case
 *         NutDhcpError() can be called to get a more specific error
 *         code.
 */
int NutDhcpIfConfig(CONST char *name, uint8_t * mac, uint32_t timeout)
{
    uint8_t mac0[6];
    uint8_t macF[6];
    NUTDEVICE *dev;
    IFNET *nif;

    /*
     * Lookup the Ethernet device.
     */
    if ((dev = NutDeviceLookup(name)) == 0 ||   /* No device */
        dev->dev_type != IFTYP_NET ||   /* Wrong type */
        (nif = dev->dev_icb) == 0 ||    /* No netif */
        nif->if_type != IFT_ETHER) {    /* Wrong if type */
        dhcpError = DHCPERR_BADDEV;
        return -1;
    }

    /*
     * We determine whether the interface is enabled by checking
     * the MAC address. This is so bloody brain dead.
     */
    memset(mac0, 0x00, sizeof(mac0));   /* Uses more code but less RAM... */
    memset(macF, 0xFF, sizeof(macF));   /* ...than init in declaration.   */
    if (memcmp(nif->if_mac, mac0, 6) == 0 || memcmp(nif->if_mac, macF, 6) == 0) {
        /*
         * If the caller specified a MAC address, we use it and
         * overwrite the configuration.
         */
        if (mac) {
            memcpy(confnet.cdn_mac, mac, sizeof(confnet.cdn_mac));
        }

        /*
         * If no MAC address has been specified, read the configuration
         * from EEPROM. If this fails, we do not continue any further,
         * but let the caller know that something is wrong. He may call
         * us again with a valid MAC address.
         */
        else if (NutNetLoadConfig(name)) {
            dhcpError = DHCPERR_NOMAC;
            return -1;
        }

        /*
         * Copy the MAC address to the interface structure. This will
         * magically brain dead enable the interface.
         */
        memcpy(nif->if_mac, confnet.cdn_mac, 6);
        NutSleep(500);
    }

    /*
     * Zero out the ip address and mask. This allows to switch between
     * DHCP and static IP addresses without resetting/power cycling.
     * See patch #2903940.
     */
    nif->if_local_ip = 0;
    nif->if_mask = confnet.cdn_ip_mask;

    /*
     * If the EEPROM contains a fixed network configuration, we skip DHCP.
     */
    if ((confnet.cdn_cip_addr & confnet.cdn_ip_mask) != 0) {
        /* Give up a previously allocated lease. See patch #2903940. */
        (void)NutDhcpRelease(name, (3*MIN_DHCP_WAIT));
        confnet.cdn_ip_addr = confnet.cdn_cip_addr;
        NutNetIfConfig2(name,
                        confnet.cdn_mac,
                        confnet.cdn_ip_addr,
                        confnet.cdn_ip_mask,
                        confnet.cdn_gateway);
        return 0;
    }

    /*
     * Start the DHCP thread if not running or wake it up. Pass the caller's
     * timeout to the thread and wait an infinite time. We rely on the thread
     * to wake us up on timeout.
     */
    if (DhcpKick(name, DHCPST_INIT, timeout) == 0) {
        /*
         * The thread finished its task. If it reached the bound state, then
         * we got a valid configuration from DHCP.
         */
        if (dhcpState == DHCPST_BOUND) {
#ifdef NUTDEBUG
            if (__tcp_trf & NET_DBG_DHCP) {
                fprintf(__tcp_trs, "[DHCP-Config %s]", inet_ntoa(dhcpConfig->dyn_yiaddr));
            }
#endif
            NutNetIfSetup(dev, dhcpConfig->dyn_yiaddr, dhcpConfig->dyn_netmask, dhcpConfig->dyn_gateway);
            NutDnsConfig2(NULL, NULL, dhcpConfig->dyn_pdns, dhcpConfig->dyn_sdns);
            return 0;
        }

        /*
         * Our interface has been configured externally, possibly by auto
         * ARP or a similar function implemented by the application.
         */
        if (nif->if_local_ip) {
#ifdef NUTDEBUG
            if (__tcp_trf & NET_DBG_DHCP) {
                fprintf(__tcp_trs, "[DHCP-External %s]", inet_ntoa(nif->if_local_ip));
            }
#endif
            return 0;
        }

        /*
         * DHCP failed. In case we remember a previously allocated address,
         * then let's use it.
         */
        if ((confnet.cdn_ip_addr & confnet.cdn_ip_mask) != 0) {
#ifdef NUTDEBUG
            if (__tcp_trf & NET_DBG_DHCP) {
                fprintf(__tcp_trs, "[DHCP-Reusing %s]", inet_ntoa(confnet.cdn_ip_addr));
            }
#endif
            NutNetIfConfig2(name, confnet.cdn_mac, confnet.cdn_ip_addr, confnet.cdn_ip_mask, confnet.cdn_gateway);
            return 0;
        }
    }
    return -1;
}

/*!
 * \brief Relinguish our DHCP lease.
 *
 * This function may be called by the application if we are moving
 * to another network. It helps the DHCP server to tidy up his
 * allocation table, but is not a required DHCP function.
 *
 * Upon return, the system should be shut down within 20 seconds.
 *
 * The client must reside in state \ref DHCPST_BOUND.
 *
 * \param name    Name of the registered Ethernet device, currently ignored.
 * \param timeout Maximum number of milliseconds to wait.
 *
 * \return 0 on success or -1 in case of an error.
 */
int NutDhcpRelease(CONST char *name, uint32_t timeout)
{
    /* Check the state. */
    if (dhcpState != DHCPST_BOUND) {
        dhcpError = DHCPERR_STATE;
        return -1;
    }

    /* Action! */
    return DhcpKick(name, DHCPST_RELEASING, timeout);
}

/*!
 * \brief Inform DHCP about an allocated address.
 *
 * The client must reside in state \ref DHCPST_IDLE.
 *
 * \param name    Name of the registered Ethernet device, currently ignored.
 * \param timeout Maximum number of milliseconds to wait.
 *
 * \return 0 on success or -1 in case of an error.
 */
int NutDhcpInform(CONST char *name, uint32_t timeout)
{
    /* Check the state. */
    if (dhcpState != DHCPST_IDLE) {
        dhcpError = DHCPERR_STATE;
        return -1;
    }

    /* Action! */
    return DhcpKick(name, DHCPST_INFORMING, timeout);
}

/*!
 * \brief Return DHCP client status.
 *
 * \param name Name of the registered Ethernet device, currently ignored.
 *
 * \return DHCP status code, which may be any of the following:
 *         - \ref DHCPST_INIT
 *         - \ref DHCPST_SELECTING
 *         - \ref DHCPST_REQUESTING
 *         - \ref DHCPST_REBOOTING
 *         - \ref DHCPST_BOUND
 *         - \ref DHCPST_RENEWING
 *         - \ref DHCPST_REBINDING
 *         - \ref DHCPST_INFORMING
 *         - \ref DHCPST_RELEASING
 *         - \ref DHCPST_IDLE
 */
int NutDhcpStatus(CONST char *name)
{
    return dhcpState;
}

/*!
 * \brief Return DHCP error code.
 *
 * Possible error codes are
 *
 *         - \ref DHCPERR_TIMEOUT
 *         - \ref DHCPERR_NOMAC
 *         - \ref DHCPERR_BADDEV
 *         - \ref DHCPERR_SYSTEM
 *         - \ref DHCPERR_TRANSMIT
 *         - \ref DHCPERR_RECEIVE
 *
 * The error will be cleared upon return.
 *
 * \param name Name of the registered Ethernet device, currently ignored.
 *
 * \return DHCP error code or 0 if no error occured.
 */
int NutDhcpError(CONST char *name)
{
    int rc = dhcpError;
    dhcpError = 0;
    return rc;
}

/*!
 * \brief Check if DHCP has configured our interface
 *
 * \deprecated Applications should use NutDhcpStatus().
 *
 * \return 0 if DHCP is in bound state.
 */
int NutDhcpIsConfigured(void)
{
    return (dhcpState == DHCPST_BOUND);
}

/*@}*/
