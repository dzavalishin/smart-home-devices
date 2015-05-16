#ifndef _SYS_NUTCONFIG_H_
#define _SYS_NUTCONFIG_H_

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
 * Revision 1.5  2005/08/02 17:46:49  haraldkipp
 * Major API documentation update.
 *
 * Revision 1.4  2005/02/10 07:06:48  hwmaier
 * Changes to incorporate support for AT90CAN128 CPU
 *
 * Revision 1.3  2004/06/07 15:08:58  olereinhardt
 * Added suport for M-CAN Config header file
 *
 * Revision 1.2  2004/03/18 15:44:47  haraldkipp
 * ICCAVR failed to compile
 *
 * Revision 1.1  2004/03/16 16:56:21  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 */

/*!
 * \file sys/nutconfig.h
 * \brief Hardware configuration.
 */

#if defined(__AVR__) || defined(ATMEGA)

#include <cfg/arch/avr.h>

#if defined(MCAN)
/* M-CAN special hardware definitions */
#include <cfg/m-can.h>
#endif

#elif defined(__H8300H__) || defined(__H8300S__)

#include <cfg/rport.h>

#endif

#endif
