/*
 * Copyright (c) 2005-2007 proconX Pty Ltd <www.proconx.com>
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
 * \file include/dev/atcan.h
 * \brief Header for Atmel's internal Full CAN controller driver
 */

/*!
 * \defgroup xgCanAvr AT90CAN128 CAN device
 * \ingroup xgNutArchAvrDev
 * \brief Driver for Atmel's internal Full CAN controller
 */
/*@{*/

#ifndef _ATCAN_H_
#define _ATCAN_H_

#include <dev/can_dev.h>
#include <stdint.h>

extern NUTDEVICE devAtCan;

#if defined(__AVR_AT90CAN128__) /* Fails to compile on ARM and ICCAVR. */

#define ATCAN_MAX_MOB   15   ///< Number of messages objects

/**
 * CAN result codes
 *
 * @ingroup can
 */
enum CAN_RESULT
{
   CAN_SUCCESS       =  0,   ///< Successful operation
   CAN_TXBUF_FULL    = -1,   ///< All TX message objects busy
   CAN_RXBUF_EMPTY   = -2,   ///< All RX message objects busy
   CAN_ILLEGAL_MOB   = -3,   ///< Message object index out of range
   CAN_INVALID_SPEED = -4,   ///< Invalid baud rate parameter
   CAN_PASSIVE       = -5,   ///< Bus is in passive state
   CAN_BUS_OFF       = -6,   ///< Bus is bus-off
};
 
int8_t AtCanGetBusState(void);

int8_t AtCanEnableMsgObj(uint8_t mob,
                         uint32_t id, int8_t idIsExt, int8_t idRemTag,
                         uint32_t mask, int8_t maskIsExt, int8_t maskRemTag);

int8_t AtCanEnableRx(uint8_t noOfMsgObjs,
                     uint32_t id, int8_t idIsExt, int8_t idRemTag,
                     uint32_t mask, int8_t maskIsExt, int8_t maskRemTag);
#endif

#endif
/*@}*/

