/*
 * Copyright (C) 2008 by egnite GmbH.
 *
 * All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * For additional information see http://www.ethernut.de/
 */

/*!
 * \file net/ipdemux.c
 * \brief Supports IP handlers, registered at runtime.
 *
 * \verbatim
 * $Id: ipdemux.c 2463 2009-02-13 14:52:05Z haraldkipp $
 * \endverbatim
 */

#include <sys/types.h>
#include <sys/heap.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>

#include <stdlib.h>
#include <memdebug.h>

/*!
 * \addtogroup xgIP
 */
/*@{*/

/*!
 * \brief Linked list of registered Internet protocols.
 */
typedef struct _INET_PROTOCOLS INET_PROTOCOLS;

struct _INET_PROTOCOLS {
    INET_PROTOCOLS *inet_next;
    uint8_t inet_prot;
    int (*inet_input)(NUTDEVICE *, NETBUF *);
};

static INET_PROTOCOLS *in_prots;

/*!
 * \brief Forward Ethernet frame to registered handler.
 *
 * \param dev Identifies the device that received the frame.
 * \param nb  Pointer to a network buffer structure containing 
 *            the ethernet frame.
 *
 * \return 0 if the frame will be processed by a handler. Otherwise
 *         -1 is returned.
 */
static int NutIpDemux(NUTDEVICE * dev, NETBUF * nb)
{
    INET_PROTOCOLS *inetp;
    uint8_t prot = ((IPHDR *)nb->nb_nw.vp)->ip_p;

    for (inetp = in_prots; inetp; inetp = inetp->inet_next) {
        if (prot == inetp->inet_prot && inetp->inet_input) {
            if ((*inetp->inet_input) (dev, nb) == 0) {
                return 0;
            }
        }
    }
    return -1;
}

/*!
 * \brief Register an additional Ethernet protocol handler.
 *
 * The specified mask will be applied on the type field of incoming 
 * frames before compared with the type that had been specified for
 * the handler. If more than one handler is registered for an incoming 
 * Ethernet frame, the handler being registered last is called first.
 *
 * Each handler should return 0 if it processed the frame, in which
 * case it is also assumed, that the handler releases the memory
 * allocated for the NETBUF. Otherwise the handler should return -1,
 * in which case the frame is passed to the next handler that fits.
 *
 * The protocol types ETHERTYPE_IP and ETHERTYPE_ARP are pre-registered
 * by default and processed by internal handlers after all registered
 * handlers for that frame rejected processing. This allows to install
 * filters on the Ethernet level.
 *
 * \param type Ethernet protocol type processed by this handler.
 * \param mask Ethernet protocol type mask.
 * \param hdlr Pointer to the protocol handler function. If NULL,
 *             the handler will be temporarily disabled.
 *
 * \return 0 on success, -1 otherwise.
 */
int NutRegisterIpHandler(uint8_t prot, int (*hdlr)(NUTDEVICE *, NETBUF *))
{
    INET_PROTOCOLS *inetp;

    /* Check existing registrations. */
    for (inetp = in_prots; inetp; inetp = inetp->inet_next) {
        if (inetp->inet_prot == prot) {
            /* Found one. */
            break;
        }
    }

    if (inetp == NULL) {
        /* No existing entry. Allocate a new one. */
        inetp = calloc(1, sizeof(INET_PROTOCOLS));
        if (inetp == NULL) {
            return -1;
        }
        /* Set protocol type of our new entry. */
        inetp->inet_prot = prot;
        if (in_prots == NULL) {
            /* This is the first registration. Set the list root ... */
            in_prots = inetp;
            /* ... and enable our demultiplexer. */
            ip_demux = NutIpDemux;
        } else {
            /* Not the first registration. Insert new handler at the top. */
            inetp->inet_next = in_prots;
            in_prots = inetp;
        }
    }
    /* Finally set the handler function pointer of the new or existing 
    ** entry. */
    inetp->inet_input = hdlr;

    return 0;
}

/*@}*/
