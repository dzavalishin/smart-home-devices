#ifndef _DEV_AVRTARGET_H_
#define	_DEV_AVRTARGET_H_

/*
 * Copyright (C) 2007 by egnite Software GmbH. All rights reserved.
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

#include <cfg/progif.h>
#include <stdint.h>

/*!
 * \file avrtarget.h
 * \brief External AVR device programming.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.3  2009/01/17 11:26:47  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.2  2008/08/11 06:59:59  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.1  2007/04/12 09:01:41  haraldkipp
 * New API allows to program external AVR devices.
 *
 *
 * \endverbatim
 */

#ifndef AVRTARGET_PAGESIZE
/*!
 * \brief Target's page size.
 *
 * Hardcoded for now.
 */
#define AVRTARGET_PAGESIZE      128
#endif

/*
 * AVR target device signatures.
 */
#define AVRSIGNATURE_MEGA8      0x001E9307UL
#define AVRSIGNATURE_MEGA168    0x001E9406UL
#define AVRSIGNATURE_MEGA103    0x001E9701UL
#define AVRSIGNATURE_MEGA128    0x001E9702UL
#define AVRSIGNATURE_MEGA1280   0x001E9703UL
#define AVRSIGNATURE_MEGA1281   0x001E9704UL
#define AVRSIGNATURE_90CAN128   0x001E9781UL
#define AVRSIGNATURE_MEGA2560   0x001E9801UL
#define AVRSIGNATURE_MEGA2561   0x001E9802UL

/*
 * AVR device programming commands.
 */
#define AVRCMD_PROG_ENABLE              0xAC530000UL
#define AVRCMD_CHIP_ERASE               0xAC800000UL
#define AVRCMD_READ_PROG_MEM_LO         0x20000000UL
#define AVRCMD_READ_PROG_MEM_HI         0x28000000UL
#define AVRCMD_LOAD_PROG_MEM_PAGE_LO    0x40000000UL
#define AVRCMD_LOAD_PROG_MEM_PAGE_HI    0x48000000UL
#define AVRCMD_WRITE_PROG_MEM_PAGE      0x4c000000UL
#define AVRCMD_LOAD_EXT_ADDR_BYTE       0x4d000000UL
#define AVRCMD_READ_EEPROM_MEM          0xA0000000UL
#define AVRCMD_WRITE_EEPROM_MEM         0xC0000000UL
#define AVRCMD_LOAD_EEPROM_MEM_PAGE     0xC1000000UL
#define AVRCMD_WRITE_EEPROM_MEM_PAGE    0xC2000000UL
#define AVRCMD_READ_LOCK_BITS           0x58000000UL
#define AVRCMD_WRITE_LOCK_BITS          0xACE00000UL
#define AVRCMD_READ_SIGNATURE_BYTE      0x30000000UL
#define AVRCMD_WRITE_FUSE_BITS          0xACA00000UL
#define AVRCMD_WRITE_FUSE_HI_BITS       0xACA80000UL
#define AVRCMD_WRITE_FUSE_EXT_BITS      0xACA40000UL
#define AVRCMD_READ_FUSE_BITS           0x50000000UL
#define AVRCMD_READ_FUSE_HI_BITS        0x58080000UL
#define AVRCMD_READ_FUSE_EXT_BITS       0x50080000UL
#define AVRCMD_READ_CALIBRATION_BYTE    0x38000000UL
#define AVRCMD_POLL_READY               0xF0000000UL

__BEGIN_DECLS
/* Prototypes */
extern int AvrTargetInit(void);
extern void AvrTargetSelect(ureg_t act);
extern void AvrTargetReset(ureg_t on);
extern uint32_t AvrTargetCmd(uint32_t cmd);
extern uint32_t AvrTargetSignature(void);
extern uint32_t AvrTargetFusesRead(void);
extern uint32_t AvrTargetFusesWriteSafe(uint32_t fuses);
extern int AvrTargetProgEnable(void);
extern int AvrTargetPollReady(unsigned int tmo);
extern int AvrTargetChipErase(void);
extern void AvrTargetPageLoad(uint32_t page, CONST uint8_t * data);
extern int AvrTargetPageWrite(uint32_t page);
extern int AvrTargetPageVerify(uint32_t page, CONST uint8_t * data);

__END_DECLS
/* End of prototypes */
#endif
