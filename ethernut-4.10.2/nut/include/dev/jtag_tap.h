#ifndef _DEV_JTAG_TAP_H_
#define _DEV_JTAG_TAP_H_

/*
 * Copyright 2009 by egnite GmbH
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
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

/*!
 * \file dev/jtag_tap.h
 * \brief JTAG TAP controller definitions and prototypes.
 *
 * \verbatim
 * $Id$
 * \endverbatim
 */

#include <dev/jtag_cable.h>

#define JTAG_TEST_LOGIC_RESET    0
#define JTAG_RUN_TEST_IDLE       1
#define JTAG_SELECT_DR_SCAN      2
#define JTAG_CAPTURE_DR          3
#define JTAG_SHIFT_DR            4
#define JTAG_EXIT1_DR            5
#define JTAG_PAUSE_DR            6
#define JTAG_EXIT2_DR            7
#define JTAG_UPDATE_DR           8
#define JTAG_SELECT_IR_SCAN      9
#define JTAG_CAPTURE_IR         10
#define JTAG_SHIFT_IR           11
#define JTAG_EXIT1_IR           12
#define JTAG_PAUSE_IR           13
#define JTAG_EXIT2_IR           14
#define JTAG_UPDATE_IR          15
#define JTAG_UNKNOWN_STATE      16

typedef struct {
    /*! \brief Current state of the TAP controller. */
    uint_fast8_t tap_state;
    /*! \brief Instruction register size. */
    uint_fast8_t tap_irsize;
    /*! \brief Associated cable driver. */
    JTAG_CABLE *tap_cable;
    /*! \brief Cable specific information structure. */
    void *tap_cib;
} JTAG_TAP;

extern JTAG_TAP *TapOpen(JTAG_CABLE *cable);
extern void TapClose(JTAG_TAP *tap);
extern void TapSetIrSize(JTAG_TAP *tap, uint_fast8_t len);

extern void TapStateWalk(JTAG_TAP *tap, int state);
extern void TapData(JTAG_TAP *tap, uint8_t *rbuf, CONST uint8_t *wbuf, uint32_t len, uint_fast8_t last);
extern void TapInstruction(JTAG_TAP *tap, uint32_t code, uint_fast8_t last);

#endif
