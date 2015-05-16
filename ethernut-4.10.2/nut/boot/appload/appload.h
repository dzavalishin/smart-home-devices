#ifndef _APPLOAD_H
#define _APPLOAD_H

/*
 * Copyright (C) 2003 by egnite Software GmbH. All rights reserved.
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
 * Revision 1.2  2005/11/03 14:59:16  haraldkipp
 * Some globals replaced by CONF structures.
 * Subroutines moved to new source files.
 *
 * Revision 1.1  2003/11/03 16:19:38  haraldkipp
 * First release
 *
 */

#include "ether.h"
#include "ip.h"
#include "udp.h"
#include "dhcp.h"
#include "tftp.h"
#include "config.h"
#include "utils.h"

typedef struct {
    ETHERHDR eth_hdr;
    IPHDR    ip_hdr;
    UDPHDR   udp_hdr;
    union {
        TFTPHDR  tftp;
        BOOTPHDR bootp;
    } u;
} BOOTFRAME;

/*
 * Outgoing frame.
 */
extern BOOTFRAME sframe;

/*
 * Incoming frame.
 */
extern BOOTFRAME rframe;

#endif
