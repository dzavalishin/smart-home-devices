#ifndef _DEV_AHDLCAVR_H_
#define _DEV_AHDLCAVR_H_

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
 */

/*
 * $Log$
 * Revision 1.3  2008/08/11 06:59:59  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.2  2004/03/16 16:48:28  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 * Revision 1.1  2004/03/09 11:49:52  haraldkipp
 * Added
 *
 */

#include <dev/netbuf.h>
#include <net/if_var.h>
#include <dev/ahdlc.h>

/*!
 * \file dev/ahdlcavr.h
 * \brief On-chip UART HDLC device definitions.
 */


/*
 * Available drivers.
 */
extern NUTDEVICE devAhdlc0;
extern NUTDEVICE devAhdlc1;

extern int AhdlcAvrInit(NUTDEVICE *dev);
extern int AhdlcAvrIOCtl(NUTDEVICE *dev, int req, void *conf);
extern int AhdlcAvrInput(NUTDEVICE *dev);
extern int AhdlcAvrOutput(NUTDEVICE *dev);
extern int AhdlcAvrFlush(NUTDEVICE *dev);

extern int AhdlcAvrGetRaw(uint8_t *cp);
extern int AhdlcAvrPutRaw(uint8_t ch);

extern int AhdlcAvrRead(NUTFILE *fp, void *buffer, int size);
extern int AhdlcAvrWrite(NUTFILE *fp, CONST void *buffer, int len);
#ifdef __HARVARD_ARCH__
extern int AhdlcAvrWrite_P(NUTFILE *fp, PGM_P buffer, int len);
#endif
extern NUTFILE *AhdlcAvrOpen(NUTDEVICE *dev, CONST char *name, int mode, int acc);
extern int AhdlcAvrClose(NUTFILE *fp);

#endif

