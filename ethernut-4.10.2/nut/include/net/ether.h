#ifndef _NET_ETHER_H_
#define _NET_ETHER_H_

/*
 * Copyright (C) 2001-2003 by egnite Software GmbH. All rights reserved.
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
 * Revision 1.4  2008/08/20 06:58:04  haraldkipp
 * Corrected C++ brackets.
 * Added missing NutRegisterEthHandler prototype.
 *
 * Revision 1.3  2008/08/11 17:38:27  haraldkipp
 * Added an Ethernet protocol demultiplexer.
 *
 * Revision 1.2  2008/08/11 07:00:19  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.1.1.1  2003/05/09 14:41:11  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.6  2003/02/04 18:00:42  harald
 * Version 3 released
 *
 * Revision 1.5  2002/06/26 17:29:18  harald
 * First pre-release with 2.4 stack
 *
 */


#include <sys/types.h>
#include <dev/netbuf.h>
#include <net/if_var.h>

/*!
 * \file net/ether.h
 * \brief Ethernet protocol definitions.
 */

extern int (*ether_demux) (NUTDEVICE *, NETBUF *);

__BEGIN_DECLS
/* Function prototypes. */
extern void NutEtherInput(NUTDEVICE *dev, NETBUF *nb);
extern int NutEtherOutput(NUTDEVICE *dev, uint16_t type, uint8_t *ha, NETBUF *nb);
extern int NutRegisterEthHandler(uint16_t type, uint16_t mask, int (*hdlr)(NUTDEVICE *, NETBUF *));

__END_DECLS
/* */

#endif
