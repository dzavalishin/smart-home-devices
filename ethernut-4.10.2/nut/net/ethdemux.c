/*
 * Copyright (C) 2008 by egnite GmbH.
 * Copyright (c) 1982, 1989, 1993 by The Regents of the University of California.
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
 * \file net/ethdemux.c
 * \brief Supports Ethernet protocol handlers, registered at runtime.
 *
 * This module is added to the application code only if 
 * NutRegisterEthHandler() is called.
 *
 * \verbatim
 * $Id: ethdemux.c 2463 2009-02-13 14:52:05Z haraldkipp $
 * \endverbatim
 */

#include <sys/types.h>
#include <sys/heap.h>
#include <netinet/if_ether.h>
#include <net/ether.h>

#include <stdlib.h>
#include <memdebug.h>

/*!
 * \addtogroup xgEthernet
 */
/*@{*/

/*!
 * \brief Linked list of registered Ethernet protocols.
 */
typedef struct _ETH_PROTOCOLS ETH_PROTOCOLS;

struct _ETH_PROTOCOLS {
    ETH_PROTOCOLS *ethp_next;
    uint16_t ethp_type;
    uint16_t ethp_mask;
    int (*ethp_input)(NUTDEVICE *, NETBUF *);
};

static ETH_PROTOCOLS *eth_prot;

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
static int NutEtherDemux(NUTDEVICE * dev, NETBUF * nb)
{
    ETH_PROTOCOLS *ep;
    uint16_t type = ntohs(((ETHERHDR *)nb->nb_dl.vp)->ether_type);

    for (ep = eth_prot; ep; ep = ep->ethp_next) {
        if ((type & ep->ethp_mask) == ep->ethp_type) {
            if (ep->ethp_input && (*ep->ethp_input) (dev, nb) == 0) {
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
int NutRegisterEthHandler(uint16_t type, uint16_t mask, int (*hdlr)(NUTDEVICE *, NETBUF *))
{
    ETH_PROTOCOLS *ep;

    /* Check existing registrations. */
    for (ep = eth_prot; ep; ep = ep->ethp_next) {
        if (ep->ethp_type == type && ep->ethp_mask == mask) {
            /* Found one. */
            break;
        }
    }

    if (ep == NULL) {
        /* No existing entry. Allocate a new one. */
        ep = calloc(1, sizeof(ETH_PROTOCOLS));
        if (ep == NULL) {
            return -1;
        }
        /* Set type and mask of our new entry. */
        ep->ethp_type = type;
        ep->ethp_mask = mask;
        if (eth_prot == NULL) {
            /* This is the first registration. Set the list root ... */
            eth_prot = ep;
            /* ... and enable our demultiplexer. */
            ether_demux = NutEtherDemux;
        } else {
            /* Not the first registration. Insert new handler at the top. */
            ep->ethp_next = eth_prot;
            eth_prot = ep;
        }
    }
    /* Finally set the handler function pointer of the new or existing 
    ** entry. */
    ep->ethp_input = hdlr;

    return 0;
}

/*@}*/
