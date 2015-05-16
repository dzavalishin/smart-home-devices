#ifndef _CFG_MEMORY_H_
#define _CFG_MEMORY_H_

/*
 * Copyright (C) 2004 by egnite Software GmbH. All rights reserved.
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

/*!
 * $Log$
 * Revision 1.1  2004/08/25 10:56:10  haraldkipp
 * More general memory layout definitions moved from cfg/bankmem.h to cfg/memory.h.
 *
 */

/*!
 * \file cfg/memory.h
 * \brief Default memory layout.
 *
 * Values can be changed by the configurator.
 */

#ifndef NUTMEM_SIZE
/*!
 * \brief Number of bytes available in fast data memory.
 *
 * On most platforms this value specifies the total number of bytes 
 * available in RAM. 
 *
 * On Harvard architectures this value specifies the size of the data 
 * memory. It will be occupied by global variables and static data. 
 * Any remaining space will be added to the Nut/OS heap during system 
 * initialization.
 *
 */
#define NUTMEM_SIZE 4096
#endif

#ifndef NUTMEM_START
/*!
 * \brief First address of fast data memory.
 */
#define NUTMEM_START 0x100
#endif

#ifndef NUTMEM_RESERVED
/*!
 * \brief Number of bytes reserved for special purposes.
 *
 * Right now this is used with the AVR platform only. The specified 
 * number of bytes may be used by a device driver when the external 
 * memory interface is disabled.
 */
#define NUTMEM_RESERVED 64
#endif

#ifndef NUTXMEM_SIZE
/*!
 * \brief Number of bytes available in extended data memory.
 */
#define NUTXMEM_SIZE 28416
#endif

#ifndef NUTXMEM_START
/*
 * \brief First address of extended data memory.
 */
#define NUTXMEM_START 0x1100
#endif

#ifndef NUTBANK_COUNT
/*!
 * \brief Number of memory banks.
 *
 * For systems without banked memory this is set to zero.
 * Ethernut 2 has 30 memory banks.
 */
#define NUTBANK_COUNT   0
#endif

#ifndef NUTBANK_START
/*!
 * \brief Start address of memory banks.
 *
 * For systems without banked memory this is ignored.
 */
#define NUTBANK_START   0x8000
#endif

#ifndef NUTBANK_SIZE
/*!
 * \brief Size of a single memory bank.
 *
 * For systems without banked memory this is ignored.
 */
#define NUTBANK_SIZE    0x4000
#endif

#ifndef NUTBANK_SR
/*!
 * \brief Address of the bank select register.
 *
 * For systems without banked memory this is ignored.
 */
#define NUTBANK_SR      0xFF00
#endif

/*
 * CLPD Code for Arthernet
 */
#ifndef ARTHERCPLDSTART
#define ARTHERCPLDSTART  0x1100
#endif 

#ifndef ARTHERCPLDSPI
#define ARTHERCPLDSPI    0x1200
#endif

#endif
