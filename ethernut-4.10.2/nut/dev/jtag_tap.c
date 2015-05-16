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
 * Initially the code had been taken from the GPL'ed JTAG-O-MAT project.
 * Its author explicitly confirmed to publish this under BSDL.
 *
 * For additional information see http://www.ethernut.de/
 */

/*!
 * \file dev/jtag_tap.c
 * \brief JTAG TAP controller.
 *
 * \verbatim
 * $Id$
 * \endverbatim
 */

#include <sys/timer.h>
#include <stdlib.h>

#include <dev/jtag_tap.h>

typedef struct {
    uint8_t len;
    uint8_t seq;
} TEST_MODE_PATH;

/*!
 * \brief Test mode path table.
 */
static TEST_MODE_PATH mode_path[256] = {
    {0,0x00},{1,0x00},{2,0x02},{3,0x02},{4,0x02},{4,0x0a},{5,0x0a},{6,0x2a},
    {5,0x1a},{3,0x06},{4,0x06},{5,0x06},{5,0x16},{6,0x16},{7,0x56},{6,0x36},
    {3,0x07},{0,0x00},{1,0x01},{2,0x01},{3,0x01},{3,0x05},{4,0x05},{5,0x15},
    {4,0x0d},{2,0x03},{3,0x03},{4,0x03},{4,0x0b},{5,0x0b},{6,0x2b},{5,0x1b},
    {2,0x03},{3,0x03},{0,0x00},{1,0x00},{2,0x00},{2,0x02},{3,0x02},{4,0x0a},
    {3,0x06},{1,0x01},{2,0x01},{3,0x01},{3,0x05},{4,0x05},{5,0x15},{4,0x0d},
    {5,0x1f},{3,0x03},{3,0x07},{0,0x00},{1,0x00},{1,0x01},{2,0x01},{3,0x05},
    {2,0x03},{4,0x0f},{5,0x0f},{6,0x0f},{6,0x2f},{7,0x2f},{8,0xaf},{7,0x6f},
    {5,0x1f},{3,0x03},{3,0x07},{4,0x07},{0,0x00},{1,0x01},{2,0x01},{3,0x05},
    {2,0x03},{4,0x0f},{5,0x0f},{6,0x0f},{6,0x2f},{7,0x2f},{8,0xaf},{7,0x6f},
    {4,0x0f},{2,0x01},{2,0x03},{3,0x03},{4,0x03},{0,0x00},{1,0x00},{2,0x02},
    {1,0x01},{3,0x07},{4,0x07},{5,0x07},{5,0x17},{6,0x17},{7,0x57},{6,0x37},
    {5,0x1f},{3,0x03},{3,0x07},{4,0x07},{2,0x01},{3,0x05},{0,0x00},{1,0x01},
    {2,0x03},{4,0x0f},{5,0x0f},{6,0x0f},{6,0x2f},{7,0x2f},{8,0xaf},{7,0x6f},
    {4,0x0f},{2,0x01},{2,0x03},{3,0x03},{1,0x00},{2,0x02},{3,0x02},{0,0x00},
    {1,0x01},{3,0x07},{4,0x07},{5,0x07},{5,0x17},{6,0x17},{7,0x57},{6,0x37},
    {3,0x07},{1,0x00},{1,0x01},{2,0x01},{3,0x01},{3,0x05},{4,0x05},{5,0x15},
    {0,0x00},{2,0x03},{3,0x03},{4,0x03},{4,0x0b},{5,0x0b},{6,0x2b},{5,0x1b},
    {1,0x01},{2,0x01},{3,0x05},{4,0x05},{5,0x05},{5,0x15},{6,0x15},{7,0x55},
    {6,0x35},{0,0x00},{1,0x00},{2,0x00},{2,0x02},{3,0x02},{4,0x0a},{3,0x06},
    {5,0x1f},{3,0x03},{3,0x07},{4,0x07},{5,0x07},{5,0x17},{6,0x17},{7,0x57},
    {6,0x37},{4,0x0f},{0,0x00},{1,0x00},{1,0x01},{2,0x01},{3,0x05},{2,0x03},
    {5,0x1f},{3,0x03},{3,0x07},{4,0x07},{5,0x07},{5,0x17},{6,0x17},{7,0x57},
    {6,0x37},{4,0x0f},{5,0x0f},{0,0x00},{1,0x01},{2,0x01},{3,0x05},{2,0x03},
    {4,0x0f},{2,0x01},{2,0x03},{3,0x03},{4,0x03},{4,0x0b},{5,0x0b},{6,0x2b},
    {5,0x1b},{3,0x07},{4,0x07},{5,0x07},{0,0x00},{1,0x00},{2,0x02},{1,0x01},
    {5,0x1f},{3,0x03},{3,0x07},{4,0x07},{5,0x07},{5,0x17},{6,0x17},{7,0x57},
    {6,0x37},{4,0x0f},{5,0x0f},{2,0x01},{3,0x05},{0,0x00},{1,0x01},{2,0x03},
    {4,0x0f},{2,0x01},{2,0x03},{3,0x03},{4,0x03},{4,0x0b},{5,0x0b},{6,0x2b},
    {5,0x1b},{3,0x07},{4,0x07},{1,0x00},{2,0x02},{3,0x02},{0,0x00},{1,0x01},
    {3,0x07},{1,0x00},{1,0x01},{2,0x01},{3,0x01},{3,0x05},{4,0x05},{5,0x15},
    {4,0x0d},{2,0x03},{3,0x03},{4,0x03},{4,0x0b},{5,0x0b},{6,0x2b},{0,0x00}
};

/*!
 * \brief Open a TAP controller instance and assiociate it with a given cable driver.
 *
 * \param Cable driver structure.
 *
 * \return Pointer to a TAP controller structure, which must be passed
 *         to all other routines.
 */
JTAG_TAP *TapOpen(JTAG_CABLE *cable)
{
    JTAG_TAP *tap = NULL;
    void *cib = cable->cable_open();

    if (cib) {
        tap = calloc(1, sizeof(JTAG_TAP));
        tap->tap_state = JTAG_UNKNOWN_STATE;
        tap->tap_irsize = 0;
        tap->tap_cable = cable;
        tap->tap_cib = cib;
    }
    return tap;
}

/*!
 * \brief Release the TAP controller.
 *
 * \param tap Pointer to the TAP controller structure, obtained by 
 *            a previous call to JtagCable0Open().
 */
void TapClose(JTAG_TAP *tap)
{
    tap->tap_cable->cable_close(tap->tap_cib);
    free(tap);
}

/*!
 * \brief Toggles the target reset line.
 *
 * \param tap Pointer to the TAP controller structure, obtained by 
 *            a previous call to JtagCable0Open().
 * \param us  Time to keep the reset activated, given in microseconds.
 */
void TapTargetReset(JTAG_TAP *tap, uint32_t us)
{
    tap->tap_cable->cable_reset_target(tap->tap_cib, 1);
    if (us) {
        NutMicroDelay(us);
    }
    tap->tap_cable->cable_reset_target(tap->tap_cib, 0);
    tap->tap_state = JTAG_UNKNOWN_STATE;
}

/*!
 * \brief Move the TAP controller to the specified state.
 *
 * \param tap   Pointer to the TAP controller structure, obtained by 
 *              a previous call to JtagCable0Open().
 * \param state Requested state.
 */
void TapStateWalk(JTAG_TAP *tap, int state)
{
    int idx;

    if (tap->tap_state == JTAG_UNKNOWN_STATE || state == JTAG_TEST_LOGIC_RESET) {
        tap->tap_cable->cable_tms_put(tap->tap_cib, 0x1F, 5);
        tap->tap_state = JTAG_TEST_LOGIC_RESET;
    }
    idx = state;
    idx += tap->tap_state << 4;
    tap->tap_cable->cable_tms_put(tap->tap_cib, mode_path[idx].seq, mode_path[idx].len);
    tap->tap_state = state;
}

/*!
 * \brief Set instruction register size.
 *
 * Actually we can determine the size automatically. For now the user
 * must set it by calling this function.
 *
 * \param tap  Pointer to the TAP controller structure, obtained by 
 *             a previous call to JtagCable0Open().
 * \param len  Number of bits in the instruction register.
 */
void TapSetIrSize(JTAG_TAP *tap, uint_fast8_t len)
{
    tap->tap_irsize = len;
}

/*!
 * \brief Read and write data register.
 *
 * \param tap  Pointer to the TAP controller structure, obtained by 
 *             a previous call to JtagCable0Open().
 * \param rbuf Points to the buffer that receives the read data.
 *             May be NULL if the caller is not interested.
 * \param wbuf Points to the buffer that contains the write data.
 *             If NULL, all outgoing bits are zero.
 * \param len  Number of bits to read and/or write.
 * \param last Indicates the last data transfer. If set, the TAP 
 *             controller will move to the Exit1-DR state on the
 *             last clock cycle.
 */
void TapData(JTAG_TAP *tap, uint8_t *rbuf, CONST uint8_t *wbuf, uint32_t len, uint_fast8_t last)
{
    TapStateWalk(tap, JTAG_SHIFT_DR);
    tap->tap_cable->cable_transfer_data(tap->tap_cib, rbuf, wbuf, len, last);
    if (last) {
        tap->tap_state++;
    }
}

/*!
 * \brief Store instruction code in instruction register.
 *
 * \param tap  Pointer to the TAP controller structure, obtained by 
 *             a previous call to JtagCable0Open().
 * \param last Indicates the last data transfer. If set, the TAP 
 *             controller will move to the Exit1-IR state on the
 *             last clock cycle.
 */
void TapInstruction(JTAG_TAP *tap, uint32_t code, uint_fast8_t last)
{
    TapStateWalk(tap, JTAG_SHIFT_IR);
    tap->tap_cable->cable_transfer_data(tap->tap_cib, NULL, (uint8_t *) &code, tap->tap_irsize, last);
    if (last) {
        tap->tap_state++;
    }
}
