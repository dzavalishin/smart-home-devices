#ifndef _CONFIG_H
#define _CONFIG_H

/*
 * Copyright (C) 2003-2005 by egnite Software GmbH. All rights reserved.
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
 * Revision 1.1  2005/11/03 15:14:27  haraldkipp
 * First import.
 *
 */

#include <stddef.h>
#include "types.h"

/*!
 * \brief Network configuration type.
 */
typedef struct _CONFNET CONFNET;

/*!
 * \brief Network configuration structure.
 */
struct _CONFNET {
    u_char cd_size;         /*!< \brief Size of this structure. */
    u_char cd_name[9];      /*!< \brief Magic cookie. */
    u_char cdn_mac[6];      /*!< \brief Ethernet MAC address. */
    u_long cdn_ip_addr;     /*!< \brief Last used IP address. */
    u_long cdn_ip_mask;     /*!< \brief IP netmask. */
    u_long cdn_gateway;     /*!< \brief Default route. */
    u_long cdn_cip_addr;    /*!< \brief Configured IP address. */
};

extern CONFNET confnet;

/*!
 * \brief Boot configuration type.
 */
typedef struct _CONFBOOT CONFBOOT;

/*!
 * \brief Boot configuration structure.
 */
struct _CONFBOOT {
    u_char cb_size;         /*!< \brief Size of this structure. */
    u_char cb_flags;
    u_long cb_tftp_ip;
    u_char cb_image[128];
};

extern CONFBOOT confboot;

extern void BootConfigRead(void);

#endif

