#ifndef _CFG_RPORT_H_
#define _CFG_RPORT_H_

/*
 * Copyright (C) 2004 by Jan Dubiec. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY JAN DUBIEC AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL JAN DUBIEC
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * $Log$
 * Revision 1.1  2004/03/16 16:56:21  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 */

/*!
 * \file cfg/rport.h
 * \brief RPort hardware specification.
 */
/*!
 * \addtogroup xgRPortCfg
 */
/*@{*/

#include <h83068f.h>

/*!
 * Low level sensing interrupt; Very important!
 * High poriority;
 * CKE1-0 in SCR, default setting;
 * C/A# in SMR, default setting;
 * P95DDR, default setting:
 * P9DDR = B_1100_0000;
 * SCI1.SMR.BIT.CA = 0;
 */
#define NutInitSysIrq()                 \
    INTC.ISCR.BIT.IRQ0SC = 0;           \
    INTC.IPRA.BIT._IRQ0  = 0; 

/** 
 * IO base address of RTL chip.
 */
#define NIC_IO_BASE     (0x800000UL + 0x300UL)

/**
 * Check Nic Int is set or not
 */
#define NicIntIsEnabled()  (INTC.IER.BIT.IRQ0E == 1)

/**
 * Disable Nic Interruption
 */
#define NicDisableInt() INTC.IER.BIT.IRQ0E = 0

/**
 * Enable Nic Interruption
 */
#define NicEnableInt()  INTC.IER.BIT.IRQ0E = 1

/**
 * Switch MCU data bus (area 4) into 16 bit mode
 */
#define NicMcu16bitBus() BSC.ABWCR.BIT.ABW4 = 0

/**
 * Switch MCU data bus (area 4) into 8 bit mode
 */
#define NicMcu8bitBus() BSC.ABWCR.BIT.ABW4 = 1

/**
 * interruption signal handler of RTL_SIGNAL_BIT. 
 */
#define RTL_SIGNAL      sig_INT0

/*@}*/

#endif /* #ifndef _CFG_RPORT_H_ */
