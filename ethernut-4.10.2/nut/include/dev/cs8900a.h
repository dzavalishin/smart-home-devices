/****************************************************************************
*  This file is part of the Ethernut port for the OLIMEX LPC-E2294 board.
*
*  Copyright (c) 2005 by Michael Fischer. All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*  
*  1. Redistributions of source code must retain the above copyright 
*     notice, this list of conditions and the following disclaimer.
*  2. Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in the 
*     documentation and/or other materials provided with the distribution.
*  3. Neither the name of the author nor the names of its contributors may 
*     be used to endorse or promote products derived from this software 
*     without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL 
*  THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS 
*  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
*  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
*  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF 
*  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
*  SUCH DAMAGE.
*
****************************************************************************
*
*  History:
*
*  24.09.05  mifi   First Version
*                   The CrossWorks for ARM toolchain will be used.
*                   This was a header file from a previous Nut/OS version. 
****************************************************************************/
#ifndef __CS8900A_H__
#define __CS8900A_H__

#include <dev/netbuf.h>
#include <net/if_var.h>


/*!
 * \file dev/cs8900a.h
 * \brief Network interface controller definitions.
 */

__BEGIN_DECLS

/*
 * Available drivers.
 */
extern NUTDEVICE devCS8900A;

#ifndef DEV_ETHER
#define DEV_ETHER   devCS8900A
#endif

/*
 * Driver routines.
 */
extern int cs8900Init(NUTDEVICE *dev);
extern int cs8900Output(NUTDEVICE *dev, NETBUF *nb);

__END_DECLS

#endif /* !__CS8900A_H__ */
/*** EOF ***/

