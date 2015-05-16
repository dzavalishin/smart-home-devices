#ifndef _CFG_ETHERNUT_H_
#define _CFG_ETHERNUT_H_

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
 * Revision 1.3  2005/01/22 19:26:33  haraldkipp
 * Marked deprecated.
 *
 * Revision 1.2  2003/07/13 19:43:12  haraldkipp
 * Ethernut 2.0 support added.
 *
 * Revision 1.1.1.1  2003/05/09 14:41:04  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.2  2003/05/06 18:39:37  harald
 * Realtek definitions
 *
 * Revision 1.1  2003/03/31 14:53:23  harald
 * Prepare release 3.1
 *
 */

/*!
 * \file cfg/ethernut.h
 * \brief Ethernut hardware specification.
 *
 * Including this file is deprecated. Use cfg/arch/avr.h.
 */
/*!
 * \addtogroup xgEthernutCfg
 */
/*@{*/

#warning Using this file is deprecated, just include <cfg/arch/avr.h> instead

/*! Port output register of \ref RTL_RESET_BIT. */
#define RTL_RESET_PORT   PORTE

/*! Data direction register of \ref RTL_RESET_BIT. */
#define RTL_RESET_DDR    DDRE

/*! \brief RTL8019AS hardware reset input.

           Only used on version 1.0 and 1.1 boards. */
#define RTL_RESET_BIT    4

/*! Port output register of \ref RTL_SIGNAL_BIT. */
#define RTL_SIGNAL_PORT    PORTE

/*! Port input register of \ref RTL_SIGNAL_BIT. */	
#define RTL_SIGNAL_PIN     PINE	

/*! Data direction register of \ref RTL_SIGNAL_BIT. */
#define RTL_SIGNAL_DDR     DDRE	

/*! \brief Interrupt signal handler of \ref RTL_SIGNAL_BIT. */
#define RTL_SIGNAL  sig_INTERRUPT5

/*! 
 * \brief Interrupt signal bit for Ethernut 1.x Ethernet controller.
 */
#define RTL_SIGNAL_BIT     5


/*! 
 * \brief Ethernut 2.x Ethernet controller base address.
 */
#define NIC_BASE            0xC000

/*! Port output register of \ref LAN_SIGNAL_BIT. */
#define LAN_SIGNAL_PORT     PORTE

/*! Port input register of \ref LAN_SIGNAL_BIT. */	
#define LAN_SIGNAL_PIN      PINE	

/*! Data direction register of \ref LAN_SIGNAL_BIT. */
#define LAN_SIGNAL_DDR      DDRE	

/*! \brief Interrupt signal handler of \ref LAN_SIGNAL_BIT. */
#define LAN_SIGNAL          sig_INTERRUPT5

/*! 
 * \brief Interrupt signal bit for Ethernut 2.x Ethernet controller.
 */
#define LAN_SIGNAL_BIT      5


/*@}*/

#endif
