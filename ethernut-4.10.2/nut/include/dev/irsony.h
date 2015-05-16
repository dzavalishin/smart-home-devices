#ifndef _DEV_IRSONY_H_
#define _DEV_IRSONY_H_

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
 * Revision 1.2  2004/03/16 16:48:28  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 * Revision 1.1  2003/07/21 18:22:38  haraldkipp
 * First check in
 *
 */


#include <dev/ir.h>

/*!
 * \file dev/irsony.h
 * \brief Sony remote control device definitions.
 */

/* 
 * Sony TV codes.
 */
#define IR_DEVICE       1
#define IRCMD_CHAN_1    0
#define IRCMD_CHAN_2    1
#define IRCMD_CHAN_3    2
#define IRCMD_CHAN_4    3
#define IRCMD_CHAN_5    4
#define IRCMD_CHAN_6    5
#define IRCMD_CHAN_7    6
#define IRCMD_CHAN_8    7
#define IRCMD_CHAN_9    8

#define IRCMD_CHAN_UP   16
#define IRCMD_CHAN_DN   17

#define IRCMD_VOL_UP    18
#define IRCMD_VOL_DN    19
#define IRCMD_MUTE      20

#define IRCMD_OFF       21

#define IRCMD_SELECT    56
#define IRCMD_VTEXT     63

__BEGIN_DECLS
/* Prototypes */
extern int NutIrInitSony(void);

__END_DECLS
#endif
