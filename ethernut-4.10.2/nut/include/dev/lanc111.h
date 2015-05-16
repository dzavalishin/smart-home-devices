#ifndef _DEV_LANC111_H_
#define _DEV_LANC111_H_

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
 */

/*
 * $Log$
 * Revision 1.3  2003/11/03 16:58:00  haraldkipp
 * NICINFO structure removed to allow apps for different Ethernet controller
 *
 * Revision 1.2  2003/07/20 20:07:38  haraldkipp
 * Conflicting Ethernet driver routine names solved.
 *
 * Revision 1.1  2003/07/13 19:40:01  haraldkipp
 * LAN91C111 driver redesigned.
 *
 */

#include <dev/netbuf.h>
#include <net/if_var.h>


/*!
 * \file dev/lanc111.h
 * \brief Network interface controller definitions.
 */

__BEGIN_DECLS

/*
 * Available drivers.
 */
extern NUTDEVICE devSmsc111;

#ifndef DEV_ETHER
#define DEV_ETHER   devSmsc111
#endif

/*
 * Driver routines.
 */
extern int LancInit(NUTDEVICE *dev);
extern int LancOutput(NUTDEVICE *dev, NETBUF *nb);

__END_DECLS

#endif
