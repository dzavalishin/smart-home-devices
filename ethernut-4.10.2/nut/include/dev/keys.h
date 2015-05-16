#ifndef _KEYS_H_
#define _KEYS_H_

/*
 * Copyright (C) 2001-2009 by egnite Software GmbH. All rights reserved.
 * Copyright (C) 2009 by Ulrich Prinz (uprinz2@netscape.net)
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
 *
 * Revision 0.3  2009/09/12 ulrichprinz
 * First checkin, new push button driver example
 * (currently SAM7X256 is tested only)
 *
 */

/*!
 * \file dev/keys.h
 * \brief Key handler definitions.
 */

/*!
 * \addtogroup xgDevKeys
 */
/*@{*/

/*!
 * \brief Key state definitions.
 *
 * The key handler assignes the following states to a key.
 *
 * The user should only use KEY_IS_DOWN and KEY_PENDING to determine
 * what key released his NutEventWait() or if he uses the key functions
 * without event handling.
 */

/*!
 * \brief Key state KEY_NOT_PRESSED.
 *
 * State used internally.
 */
#define KEY_NOT_PRESSED		0x00		/* key is currently not pressed */

/*!
 * \brief Key state KEY_IS_DOWN.
 *
 * User can check if key is pressed by
 * if ((NutGetKeyState( key) & KEY_IS_DOWN) == KEY_IS_DOWN)
 */
#define KEY_IS_DOWN         0x01        /* key is currently down */

/*!
 * \brief Key state KEY_PENDING.
 *
 * User can check if the action assigned to this key has released the
 * event by using the folllowing if sequence:
 * if ((NutGetKeyState( key) & KEY_PENDING) == KEY_PENDING)
 * This state is reset on every call of NutGetKeyState().
 */
#define KEY_PENDING         0x02        /* Key action is pending, reset on NutGetKey() */

/*!
 * \brief Key state KEY_NOT_PRESSED.
 *
 * State used internally for locking a key and prevent pultiple event
 * release if the key is pressed longer than any action requires.
 */
#define KEY_IS_LOCKED       0x04        /* for preventing double execution */

/*!
 * \brief Key state definitions.
 *
 * The key handler assignes the following states to a key depending
 *
 * The user should only use KEY_IS_DOWN and KEY_PENDING to determine
 * what key released his NutEventWait() or if he uses the key functions
 * without event handling.
 */

/*!
 * \brief Key action KEY_ACTION_DOWN.
 *
 * By assigning this action to a key, it will release a pending event wait
 * if the button is pressed. Timing parameter is ignored.
 */
#define KEY_ACTION_DOWN     0x10        /* assign action on key press, no timing possible */

/*!
 * \brief Key action KEY_ACTION_UP.
 *
 * By assigning this action to a key, it will release a pending event wait
 * if the button is released. Timing parameter is ignored.
 */
#define KEY_ACTION_UP       0x20        /* assign action on key release, no timing possible*/

/*!
 * \brief Key action KEY_ACTION_HOLD.
 *
 * By assigning this action to a key, and givign a timing parameter in ms
 * the assigned event will be released if the button was pressed for at
 * least the given time.
 */
#define KEY_ACTION_HOLD     0x40        /* assign action on time pressed */

/*!
 * \brief Key action KEY_ACTION_SHORT.
 *
 * By assigning this action to a key, and givign a timing parameter in ms
 * the assigned event will be released if the button was released before the
 * given time elapsed.
 */
#define KEY_ACTION_SHORT    0x80        /* assign action on release before given time */

/*@}*/

__BEGIN_DECLS
/* Function prototypes. */

extern int NutGetKeyState( HANDLE keyh);
extern uint32_t NutGetKeyTime( HANDLE keyh);

int NutRegisterKey( HANDLE *keyhp, int bank, int pin, int fx, uint32_t fxt);
int NutAssignKeyFkt( HANDLE *keyhp, void (*callback)(void));
int NutAssignKeyEvt( HANDLE *keyhp, HANDLE *event);

__END_DECLS
/* */

#endif
