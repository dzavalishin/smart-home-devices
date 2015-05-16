/*
 * Copyright 2009 by egnite GmbH
 *
 * All rights reserved.
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

/*
 * \file gorp/edline/vt100.c
 * \brief VT100 key mapping.
 *
 * \verbatim
 * $Id$
 * \endverbatim
 */

#include <gorp/edline.h>

/*!
 * \addtogroup xgEdLine
 */
/*@{*/

/*!
 * \brief Optional VT100 key mapping routine.
 *
 * Pass this function to \ref EdLineRegisterKeymap to enable VT100
 * key mapping.
 *
 * This routine has been tested with the TeraTerm terminal emulator
 * after loading the keymap contained in ibmkeyb.cnf. It allows to
 * use the arrow keys for moving the cursor and walking through
 * the history. This should work with most other VT100 emulations as 
 * well.
 *
 * The following command sequences are remapped:
 *
 * - ESC [ A remapped to \ref EDIT_KEY_UP.
 * - ESC [ B remapped to \ref EDIT_KEY_DOWN.
 * - ESC [ C remapped to \ref EDIT_KEY_RIGHT.
 * - ESC [ D remapped to \ref EDIT_KEY_LEFT.
 *
 * Internally calls \ref EdLineKeyMap to handle carriage return and 
 * linefeed.
 *
 * \param key Input character to remap.
 * \param seq Pointer to an integer, which is used by the key mapping
 *            routine to store the current state. Note, that its value
 *            must be preserved by the caller between calls to 
 *            \ref EdLineRead.
 *
 * \return The mapped character. If \ref EDIT_KEY_IGNORE is returned
 *         the caller must ignore this character.
 */
int EdLineKeyMapVt100(int key, int *seq)
{
    if (*seq > 0) {
        if (*seq == 1 && key == '[') {
            key = EDIT_KEY_IGNORE;
            *seq = 2;
        } else {
            /* Right arrow. */
            if (key == 'C') {
                key = EDIT_KEY_RIGHT;
            }
            /* Left arrow. */
            else if (key == 'D') {
                key = EDIT_KEY_LEFT;
            }
#ifndef EDIT_DISABLE_HISTORY
            /* Up arrow. */
            else if (key == 'A') {
                key = EDIT_KEY_UP;
            }
            /* Down arrow. */
            else if (key == 'B') {
                key = EDIT_KEY_DOWN;
            }
#endif
            *seq = 0;
        }
    }
    else if (key == 0x1b) {
        (*seq) = 1;
        key = EDIT_KEY_IGNORE;
    }
    else {
        key = EdLineKeyMap(key, seq);
    }
    return key;
}

/*@}*/
