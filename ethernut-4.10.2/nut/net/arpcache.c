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
 * \file net/arpcache.c
 * \brief ARP cache.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.16  2009/02/13 14:52:05  haraldkipp
 * Include memdebug.h for heap management debugging support.
 *
 * Revision 1.15  2008/08/11 07:00:29  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.14  2006/10/05 17:24:41  haraldkipp
 * On ARP transmit errors the incomplete cache entry is not removed.
 * Subsequent queries will never send out new ARP requests. Many thanks
 * to Michael Jones for providing a first solution. I added a check,
 * which avoids to remove an already completed entry. Fixes bug #1567783.
 *
 * Revision 1.13  2005/08/02 17:46:49  haraldkipp
 * Major API documentation update.
 *
 * Revision 1.12  2005/06/05 16:49:09  haraldkipp
 * Do not do ARP retries by default.
 *
 * Revision 1.11  2005/05/16 08:41:25  haraldkipp
 * Bugfix: Empty queue before removing entry.
 *
 * Revision 1.10  2005/04/30 16:42:42  chaac
 * Fixed bug in handling of NUTDEBUG. Added include for cfg/os.h. If NUTDEBUG
 * is defined in NutConf, it will make effect where it is used.
 *
 * Revision 1.9  2005/03/13 13:40:32  haraldkipp
 * If NutArpOutput() failed, then NutArpCacheQuery() released the already
 * released NETBUF. This bug had been fixed. If NutArpAllocNetBuf() fails,
 * the ARP entry will now be removed immediately. Additional check for
 * valid entry pointer added. These modifications had been suggested by
 * Dusan Ferbas. Finally memcpy() has been replaced by a simple loop in order
 * to provide a work around for GCC Bug #18251.
 *
 * Revision 1.8  2005/02/07 18:57:49  haraldkipp
 * ICCAVR compile errors fixed
 *
 * Revision 1.7  2005/02/07 09:26:56  haraldkipp
 * Argh! Committed wrong source.
 *
 * Revision 1.5  2005/02/04 14:55:08  haraldkipp
 * Almost a complete redesign. Replaced the ARP timer thread by calling an
 * ARP aging routine before each incoming ARP and outgoing IP packet.
 * This also fixes the bug, which generated two ARP requests per query.
 * Thanks to Dusam Ferbas and Rostislav Hlebak for their help.
 *
 * Revision 1.4  2004/07/27 19:38:30  drsung
 * Under certain circumstances the same ARPENTRY was
 * allocated twice.
 *
 * Revision 1.3  2004/03/18 10:18:01  haraldkipp
 * Comments updated
 *
 * Revision 1.2  2004/02/08 17:14:05  drsung
 * arp entries with set ATF_PERM flag are not removed any longer.
 *
 * Revision 1.1.1.1  2003/05/09 14:41:26  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.16  2003/02/04 18:14:56  harald
 * Version 3 released
 *
 * Revision 1.15  2002/06/26 17:29:35  harald
 * First pre-release with 2.4 stack
 *
 * \endverbatim
 */

#include <cfg/os.h>
#include <cfg/arp.h>

#include <sys/event.h>
#include <sys/timer.h>

#include <stdlib.h>
#include <string.h>
#include <memdebug.h>

#include <net/if_var.h>
#include <netinet/if_ether.h>
#include <arpa/inet.h>

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
 * \addtogroup xgARP
 */
/*@{*/

/*!
 * \name ARP Configuration
 *
 * The Nut/OS Configurator may be used to override the default values.
 */
/*@{*/

/*! \brief Maximum age of an entry in the ARP cache in minutes.
 *
 * Outdated entries will be regularly removed, forcing the Ethernet
 * interface to generate new ARP requests. This way MAC address
 * changes are detected.
 *
 * \showinitializer
 */
#ifndef MAX_ARPAGE
#define MAX_ARPAGE 9
#endif

/*! \brief Maximum number of ARP requests generated per query.
 *
 * If no ARP response is received after sending out the specified
 * number of request, the related IP address is considered unreachable.
 *
 * \showinitializer
 */
#ifndef MAX_ARPREQUESTS
#define MAX_ARPREQUESTS 1
#endif

/*! \brief Minimum wait before sending out a new ARP request.
 *
 * The specified number of milliseconds will be doubled on each retry.
 *
 * \showinitializer
 */
#ifndef MIN_ARPWAIT
#define MIN_ARPWAIT 500
#endif

/*@}*/


/*!
 * \brief Remove all entries marked for removal.
 *
 * \param ifn The network interface.
 */
static void ArpCacheFlush(IFNET * ifn)
{
    ARPENTRY *ae = ifn->arpTable;
    ARPENTRY **aep = &ifn->arpTable;

    while (ae) {
        if (ae->ae_flags & ATF_REM) {
            /* Remove all waiting threads from the queue of this
               entry, but do not give up the CPU. If some other
               thread takes over and deals with ARP, we are dead. */
            NutEventBroadcastAsync(&ae->ae_tq);
#ifdef NUTDEBUG
            if (__tcp_trf & NET_DBG_ARP) {
                fprintf(__tcp_trs, "[ARP-DEL %s]", inet_ntoa(ae->ae_ip));
            }
#endif
            *aep = ae->ae_next;
            free(ae);
            ae = *aep;
        } else {
            aep = &ae->ae_next;
            ae = ae->ae_next;
        }
    }
}

/*!
 * \brief Update the age of all ARP entries of all Ethernet devices.
 *
 * Increments the age of all ARP entries. Any entry with an age above
 * \ref MAX_ARPAGE will be removed.
 *
 * If less less than one minute elapsed since the last update, the
 * routine will return without updating any entry.
 */
static void ArpCacheAging(void)
{
    static uint32_t last_update;
    NUTDEVICE *dev;

    if (NutGetSeconds() - last_update >= 60) {
        last_update = NutGetSeconds();

        /*
         * Loop through the list of all registered devices.
         */
        for (dev = nutDeviceList; dev; dev = dev->dev_next) {

            /* Process network devices only. */
            if (dev->dev_type == IFTYP_NET) {
                IFNET *ifn = dev->dev_icb;

                /* Process Ethernet interfaces only. */
                if (ifn && ifn->if_type == IFT_ETHER) {
                    ARPENTRY *ae;
                    uint8_t rmf = 0;

                    /* Loop through all ARP entries of this interface. */
                    for (ae = ifn->arpTable; ae; ae = ae->ae_next) {
                        if ((ae->ae_flags & ATF_PERM) == 0 &&   /* Not permanent. */
                            ae->ae_outdated++ >= MAX_ARPAGE) {  /* Outdated. */
                            ae->ae_flags |= ATF_REM;
                        }
                        rmf |= ae->ae_flags;
#ifdef NUTDEBUG
                        if (__tcp_trf & NET_DBG_ARP) {
                            fprintf(__tcp_trs, "[ARP-AGE %s %u]",       /* */
                                    inet_ntoa(ae->ae_ip), ae->ae_outdated);
                        }
#endif
                    }
                    if (rmf & ATF_REM) {
                        ArpCacheFlush(ifn);
                    }
                }
            }
        }
    }
}

/*!
 * \brief Locate an interface's ARP entry for a given IP address.
 *
 * \param ifn Pointer to the network interface.
 * \param ip  IP address to search, given in network byte order.
 *
 * \return Pointer to the ARP cache entry, if found, or NULL if no
 *         entry exists.
 */
static ARPENTRY *ArpCacheLookup(IFNET * ifn, uint32_t ip)
{
    ARPENTRY *entry;

    for (entry = ifn->arpTable; entry; entry = entry->ae_next) {
        if (entry->ae_ip == ip)
            break;
    }
    return entry;
}


/*!
 * \brief Create a new entry in the interface's ARP cache.
 *
 * The new entry is added on top of the cache list.
 *
 * \param ifn Pointer to the network interface.
 * \param ip  IP address of the new entry, given in network byte order.
 * \param ha  Pointer to the MAC address. If NULL, an incomplete entry
 *            will be created.
 *
 * \return Pointer to the new entry or NULL if not enough memory is
 *         available.
 */
static ARPENTRY *ArpCacheNew(IFNET * ifn, uint32_t ip, uint8_t * ha)
{
    ARPENTRY *entry;

    /* Remove outdated entries before adding a new one. */
    ArpCacheAging();

    if ((entry = malloc(sizeof(ARPENTRY))) != 0) {
        memset(entry, 0, sizeof(ARPENTRY));
        entry->ae_ip = ip;
        if (ha) {
            memcpy(entry->ae_ha, ha, 6);
            entry->ae_flags = ATF_COM;
        }
        entry->ae_next = ifn->arpTable;
        ifn->arpTable = entry;

#ifdef NUTDEBUG
        if (__tcp_trf & NET_DBG_ARP) {
            fprintf(__tcp_trs, "\n[ARP-NEW %s", inet_ntoa(ip));
            if (ha) {
                fprintf(__tcp_trs, " %02x%02x%02x%02x%02x%02x", /* */
                        ha[0], ha[1], ha[2], ha[3], ha[4], ha[5]);
            }
            fputc(']', __tcp_trs);
        }
#endif
    }
    return entry;
}

/*!
 * \brief Update an ARP entry.
 *
 * If an entry with the same IP address exists, then this entry is
 * updated. If no entry exists, a new one is created. All threads
 * waiting for address resolution are woken up.
 *
 * \note This function is automatically called on each incoming
 *       ARP telegram. Applications typically do not call this
 *       function.
 *
 * \param dev Identifies the device.
 * \param ip  Requested IP address in network byte order.
 * \param ha  Pointer to a buffer which receives the MAC address.
 *
 */
void NutArpCacheUpdate(NUTDEVICE * dev, uint32_t ip, uint8_t * ha)
{
    ARPENTRY *entry;

    /*
     * If an entry with this IP exists, wake up waiting threads. If the
     * entry is not permanent, then update it and mark it completed first.
     */
    if ((entry = ArpCacheLookup(dev->dev_icb, ip)) != 0) {

#ifdef NUTDEBUG
        if (__tcp_trf & NET_DBG_ARP) {
            fprintf(__tcp_trs, "[ARP-UPD %s", inet_ntoa(ip));
            if (ha) {
                fprintf(__tcp_trs, " %02x%02x%02x%02x%02x%02x", /* */
                        ha[0], ha[1], ha[2], ha[3], ha[4], ha[5]);
            }
            fputc(']', __tcp_trs);
        }
#endif

        if ((entry->ae_flags & ATF_PERM) == 0) {
            entry->ae_outdated = 0;
            memcpy(entry->ae_ha, ha, 6);
            entry->ae_flags |= ATF_COM;
        }
        NutEventBroadcast(&entry->ae_tq);
    }

    /*
     * If no entry with this IP exists, then create a new completed one.
     */
    else {
        ArpCacheNew(dev->dev_icb, ip, ha);
    }
}

/*!
 * \brief Query MAC address for a specified IP address.
 *
 * If no entry is available in the ARP cache, an incomplete entry is
 * created and ARP requests are generated on increasing time intervals.
 * The calling thread is suspended until a matching ARP response is
 * received or until a number of requests have been generated without
 * receiving a response.
 *
 * \note This function is automatically called on each outgoing
 *       IP packet. Applications typically do not call this function.
 *
 * \param dev  Identifies the device.
 * \param ip   IP address of which the caller asked the MAC address.
 * \param mac  Buffer for the retrieved MAC address.
 *
 * \return 0 if address resolved, -1 otherwise.
 */
int NutArpCacheQuery(NUTDEVICE * dev, CONST uint32_t ip, uint8_t * mac)
{
    int rc = -1;
    ARPENTRY *entry;
    IFNET *ifn = dev->dev_icb;
    NETBUF *nb = 0;
    uint_fast8_t retries = MAX_ARPREQUESTS;
    uint32_t tmo = MIN_ARPWAIT;

    /* Aging the cache on each query adds some processing to the path
     * which we want to be as fast as possible. But when calling this
     * function in NutArpCacheNew only, we will never detect when a
     * node changes its MAC address. Anyway, the previous solution of
     * running a timer thread consumed too much RAM.
     */
    ArpCacheAging();

    /*
     * Search a matching entry. If none exists, create a new incomplete
     * entry and a request packet. If another thread has entered this
     * routine, an incomplete entry exists and the current thread will
     * not create a request packet and send out requests.
     */
    if ((entry = ArpCacheLookup(ifn, ip)) == 0) {
        if ((entry = ArpCacheNew(ifn, ip, 0)) == 0) {
            return -1;
        }
        if ((nb = NutArpAllocNetBuf(ARPOP_REQUEST, ip, 0)) == 0) {
            entry->ae_flags |= ATF_REM;
            ArpCacheFlush(ifn);
            return -1;
        }
    }

    /*
     * We enter a loop, which will send ARP requests on increasing
     * time intervals until our ARP entry gets completed. Give up
     * after a configured number of retries.
     */
    for (;;) {
        /* If completed, provide the MAC address and exit. */
        if (entry->ae_flags & ATF_COM) {
            //Work around for GCC 3.4.3 bug #18251
            //memcpy(mac, entry->ae_ha, 6);
            //rc = 0;
            rc = 6;
            do {
                rc--;
                mac[rc] = entry->ae_ha[rc];
            } while(rc);
            break;
        }
#ifdef NUTDEBUG
        if (__tcp_trf & NET_DBG_ARP) {
            fprintf(__tcp_trs, "[%u.ARP-%s %s]",        /* */
                    MAX_ARPREQUESTS - retries + 1,      /* */
                    nb ? "QRY" : "WAIT",        /* */
                    inet_ntoa(ip));
        }
#endif

        /* Give up on too many retries. */
        if (retries-- == 0) {
            break;
        }
        /* Mark buffer released and remove incomplete entry on transmit errors. */
        if (nb && NutArpOutput(dev, nb)) {
            nb = 0;
            /* Even if the transmit failed, we may have received a response in the meantime. */
            if ((entry = ArpCacheLookup(ifn, ip)) != NULL && (entry->ae_flags & ATF_COM) == 0) {
                entry->ae_flags |= ATF_REM;
                ArpCacheFlush(ifn);
            }
            break;
        }
        /* Sleep until woken up by an update of this ARP entry
           or until timeout. Double the timeout on each retry. */
        NutEventWait(&entry->ae_tq, tmo);
        tmo += tmo;

        /* During our sleep, another thread, which created the
           incomplete entry, may have given up and removed the entry.
           In this case we should also return an error. */
        if ((entry = ArpCacheLookup(ifn, ip)) == 0) {
            break;
        }
    }

    /* Only the thread that created the entry, allocated a request
       packet. If this thread fails, it should also remove the entry. */
    if (nb) {
        NutNetBufFree(nb);
        /* Play save and check, if the entry still exists. */
        if (rc && entry) {
            entry->ae_flags |= ATF_REM;
            ArpCacheFlush(ifn);
        }
    }
    return rc;
}

/*@}*/
