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
 * \file gorp/edline/edline.c
 * \brief Simple line editor.
 *
 * \verbatim
 * $Id$
 * \endverbatim
 */

#include <gorp/edline.h>

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

/*!
 * \addtogroup xgEdLine
 */
/*@{*/

/*! \brief Maximum number of history entries.
 *
 * The first entry with index 0 is used to restore the default value.
 */
#if !defined(EDIT_MAX_HISTORY) && !defined(EDIT_DISABLE_HISTORY)
#define EDIT_MAX_HISTORY    16
#endif

/*!
 * \brief Default input routine.
 *
 * Uses stdin and may be replaced by calling EdLineRegisterInput().
 *
 * \param param Optional parameter, ignored here.
 *
 * \return Character read or EOF in case of an error.
 */
static int EdLineGetChar(void *param)
{
    return getchar();
}

/*!
 * \brief Default output routine.
 *
 * Uses stdout and may be replaced by calling EdLineRegisterOutput().
 *
 * \param param Optional parameter, ignored here.
 *
 * \return Character written or EOF in case of an error.
 */
static int EdLinePutChar(void *param, int ch)
{
    return putchar(ch);
}

/*!
 * \brief Register an input routine.
 *
 * Applications may use this function to replace the default character
 * input routine.
 *
 * \param el    Pointer to an \ref EDLINE structure, obtained by a 
 *              previous call to \ref EdLineOpen.
 * \param get   Pointer to the new input routine. If NULL, then the
 *              default routine is restored.
 * \param param Optional parameter, which is passed to the input
 *              routine.
 */
void EdLineRegisterInput(EDLINE *el, EDLINEGET get, void *param)
{
    if (get) {
        el->el_get = get;
    } else {
        el->el_get = EdLineGetChar;
    }
    el->el_iparm = param;
}

/*!
 * \brief Register an output routine.
 *
 * Applications may use this function to replace the default character
 * output routine.
 *
 * \param el    Pointer to an \ref EDLINE structure, obtained by a 
 *              previous call to \ref EdLineOpen.
 * \param put   Pointer to the new output routine. If NULL, then the
 *              default routine is restored.
 * \param param Optional parameter, which is passed to the output
 *              routine.
 */
void EdLineRegisterOutput(EDLINE *el, EDLINEPUT put, void *param)
{
    if (put) {
        el->el_put = put;
    } else {
        el->el_put = EdLinePutChar;
    }
    el->el_oparm = param;
}

/*!
 * \brief Default key mapping routine.
 *
 * Replaces carriage returns and linefeeds by \ref EDIT_KEY_ENTER.
 * Linefeeds immediately following carriage returns are ignored.
 * May be replaced by calling EdLineRegisterKeymap().
 *
 * This routine may be called by other key mapping routines.
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
int EdLineKeyMap(int key, int *seq)
{
    if (key == '\r') {
        *seq = -1;
        key = EDIT_KEY_ENTER;
    }
    else {
        if (key == '\n') {
            if (*seq < 0) {
                key = EDIT_KEY_IGNORE;
            } else {
                key = EDIT_KEY_ENTER;
            }
        }
        *seq = 0;
    }
    return key;
}

/*!
 * \brief Register a key mapping routine.
 *
 * \param el    Pointer to an \ref EDLINE structure, obtained by a 
 *              previous call to \ref EdLineOpen.
 * \param map   Pointer to the new mapping routine. If NULL, then the
 *              default routine is restored.
 */
void EdLineRegisterKeymap(EDLINE *el, EDLINEMAP map)
{
    if (map) {
        el->el_map = map;
    } else {
        el->el_map = EdLineKeyMap;
    }
}

/*!
 * \brief Open a line editor.
 *
 * \param mode Mode flags, may contain any combination of
 *             - EDIT_MODE_ECHO enables echoing of input characters.
 *             - EDIT_MODE_BINARY enables binary mode (currently unused).
 *             - EDIT_MODE_HISTORY enables input line history.
 *
 * \return Pointer to a new EDLINE structure.
 */
EDLINE *EdLineOpen(uint16_t mode)
{
    EDLINE *el;

    el = calloc(1, sizeof(EDLINE));
    if (el) {
        el->el_mode = mode;
        EdLineRegisterInput(el, NULL, stdin);
        EdLineRegisterOutput(el, NULL, stdout);
        EdLineRegisterKeymap(el, NULL);
#ifndef EDIT_DISABLE_HISTORY
        if (mode & EDIT_MODE_HISTORY) {
            el->el_hist = EditHistoryCreate(EDIT_MAX_HISTORY);
        }
#endif
    }
    return el;
}

/*!
 * \brief Close a line editor.
 *
 * Releases all occupied memory.
 *
 * \param el Pointer to an \ref EDLINE structure, obtained by a 
 *           previous call to \ref EdLineOpen.
 */
void EdLineClose(EDLINE *el)
{
    if (el) {
#ifndef EDIT_DISABLE_HISTORY
        EditHistoryDestroy(el->el_hist);
#endif
        free(el);
    }
}

/*!
 * \brief Print a string.
 *
 * Uses the currently registered character output routine.
 *
 * \param el    Pointer to an \ref EDLINE structure, obtained by a 
 *              previous call to \ref EdLineOpen.
 */
static int PrintString(EDLINE *el, char *str)
{
    while (*str) {
        (*el->el_put)(el->el_oparm, *str);
        str++;
    }
    return 0;
}

/*!
 * \brief Print a character multiple times.
 *
 * Uses the currently registered character output routine.
 *
 * \param el  Pointer to an \ref EDLINE structure, obtained by a 
 *            previous call to \ref EdLineOpen.
 * \param ch  The character to print.
 * \param num The number of times the character should be printed.
 */
static void PrintCharacter(EDLINE *el, int ch, int num)
{
    while (num-- > 0) {
        (*el->el_put)(el->el_oparm, ch);
    }
}

#ifndef EDIT_DISABLE_HISTORY
/*!
 * \brief Clear a given number of characters on the right side.
 *
 * Replaces a given number of characters on the right side of the
 * current cursor position. The cursor position is restored by
 * printing the same number of backspace characters.
 *
 * \param el  Pointer to an \ref EDLINE structure, obtained by a 
 *            previous call to \ref EdLineOpen.
 * \param num Number of characters to clear.
 */
static void ClearLineEnd(EDLINE *el, int num)
{
    PrintCharacter(el, EDIT_CHAR_SPACE, num);
    PrintCharacter(el, EDIT_CHAR_BACKSPACE, num);
}
#endif

/*!
 * \brief Read a line.
 *
 * This functions offers some editing capabilities and is typically
 * used for text input by human users.
 *
 * Line editing can be done by entering any of the following control 
 * characters:
 *
 * - \ref EDIT_KEY_RESTORE Restores initial default line (default ESC)
 * - \ref EDIT_KEY_REMOVE Deletes character on the left of the cursor (default CTRL-H)
 * - \ref EDIT_KEY_HOME Moves cursor to the beginning of the line (default Ctrl-A).
 * - \ref EDIT_KEY_END Moves cursor to the beginning of the line (default CTRL-E)
 * - \ref EDIT_KEY_LEFT Moves cursor one character to the left (default CTRL-B)
 * - \ref EDIT_KEY_RIGHT Moves cursor one character to the right (default CTRL-F)
 * - \ref EDIT_KEY_UP Walks up the list of previously entered lines (default CTRL-R)
 * - \ref EDIT_KEY_DOWN Walks down the list of previously entered lines (default CTRL-V)
 *
 * Note, that these commands may be modified by the currently registered 
 * remapping routine.
 *
 * \param el  Pointer to an \ref EDLINE structure, obtained by a 
 *            previous call to \ref EdLineOpen.
 * \param buf Pointer to the buffer that receives the text line.
 *            If it contains a string on entry, this will be used
 *            as the default value.
 * \param siz Number of bytes available in the buffer. The maximum
 *            length of the text string is 1 less, so the string
 *            is always properly terminated.
 *
 * \return Number of characters or -1 on errors.
 *
 * \todo Hidden entry for password input.
 */
int EdLineRead(EDLINE *el, char *buf, int siz)
{
    int ch;
    int cpos;
    int i;
#ifndef EDIT_DISABLE_HISTORY
    int ipos;
    int hidx = 0;
    int refresh = 0;
#endif

    /* Make sure that the string is terminated. */
    buf[siz - 1] = '\0';

#ifndef EDIT_DISABLE_HISTORY
    EditHistorySet(el->el_hist, 0, buf);
#endif
    PrintString(el, buf);
    cpos = strlen(buf);

    for (;;) {
        ch = (*el->el_get)(el->el_iparm);
        if (ch == EOF) {
            return -1;
        }
        ch = (*el->el_map)(ch, &el->el_seq);
        if (ch == EDIT_KEY_ENTER) {
            break;
        }
        /* Backspace removes the character in front of the cursor. */
        if (ch == EDIT_KEY_REMOVE) {
            if (cpos) {
                cpos--;
                for (i = cpos; i < siz - 1; i++) {
                    buf[i] = buf[i + 1];
                }
                if (el->el_mode & EDIT_MODE_ECHO) {
                    (*el->el_put)(el->el_oparm, EDIT_CHAR_BACKSPACE);
                }
                PrintString(el, &buf[cpos]);
                (*el->el_put)(el->el_oparm, EDIT_CHAR_SPACE);
                PrintCharacter(el, EDIT_CHAR_BACKSPACE, strlen(buf) + 1 - cpos);
            }
        }
#ifndef EDIT_DISABLE_HISTORY
        else if (ch == EDIT_KEY_RESTORE) {
            hidx = 0;
            refresh = 1;
        }
        else if (ch == EDIT_KEY_UP) {
            if (EditHistoryGet(el->el_hist, hidx + 1, NULL, 0) >= 0) {
                hidx++;
                refresh = 1;
            }
        }
        else if (ch == EDIT_KEY_DOWN) {
            if (hidx > 0) {
                hidx--;
                refresh = 1;
            }
        }
#endif
        else if (ch == EDIT_KEY_RIGHT) {
            if (cpos < strlen(buf)) {
                if (el->el_mode & EDIT_MODE_ECHO) {
                    (*el->el_put)(el->el_oparm, buf[cpos]);
                }
                cpos++;
            }
        }
        else if (ch == EDIT_KEY_LEFT) {
            if (cpos) {
                if (el->el_mode & EDIT_MODE_ECHO) {
                    (*el->el_put)(el->el_oparm, EDIT_CHAR_BACKSPACE);
                }
                cpos--;
            }
        }
        else if (ch == EDIT_KEY_HOME) {
            PrintCharacter(el, EDIT_CHAR_BACKSPACE, cpos);
            cpos = 0;
        }
        else if (ch == EDIT_KEY_END) {
            PrintString(el, &buf[cpos]);
            cpos = strlen(buf);
        }
        /* Normal character, insert at cursor position if buffer is not full. */
        else if (isprint(ch) && strlen(buf) < siz - 1) {
            for (i = siz - 1; i > cpos; i--) {
                buf[i] = buf[i - 1];
            }
            buf[cpos++] = ch;
            if (el->el_mode & EDIT_MODE_ECHO) {
                (*el->el_put)(el->el_oparm, ch);
            }
            PrintString(el, &buf[cpos]);
            PrintCharacter(el, EDIT_CHAR_BACKSPACE, strlen(buf) - cpos);
        } else {
            /* Beep on buffer overflow. */
            (*el->el_put)(el->el_oparm, EDIT_CHAR_ALARM);
        }
#ifndef EDIT_DISABLE_HISTORY
        if (refresh && (el->el_mode & EDIT_MODE_HISTORY) != 0) {
            refresh = 0;
            PrintCharacter(el, EDIT_CHAR_BACKSPACE, cpos);
            ipos = strlen(buf);
            EditHistoryGet(el->el_hist, hidx, buf, siz);
            cpos = strlen(buf);
            PrintString(el, buf);
            ClearLineEnd(el, ipos - cpos);
        }
#endif
    }
    PrintString(el, EDIT_STR_EOL);
#ifndef EDIT_DISABLE_HISTORY
    EditHistoryInsert(el->el_hist, 1, buf);
#endif
    return strlen(buf);
}

/*@}*/
