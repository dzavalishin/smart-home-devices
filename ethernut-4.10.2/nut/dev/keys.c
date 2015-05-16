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
 * THIS SOFTWARE IS PROVIDED BY EMBEDDED IT AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL EMBEDDED IT
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * For additional information see http://www.ethernut.de/
 *
 */

/*
 * \file dev/keys.c
 * \brief Key handling driver.
 *
 * Driver for flexible key, switch and button handling.
 *
 * \verbatim
 *
 * $Log$
 *
 * Revision 0.3  2009/09/12 ulrichprinz
 * First checkin, new push button driver example
 * (currently SAM7X256 is tested only)
 *
 * \endverbatim
 */

#include <compiler.h>
#include <cfg/os.h>

#include <stdlib.h>
#include <string.h>
#include <sys/heap.h>
#include <sys/event.h>
#include <sys/timer.h>
#include <sys/atom.h>

#include <dev/board.h>

#include <dev/gpio.h>

#include <sys/nutdebug.h>

#include <cfg/pca9555.h>
#ifdef KEY_SUPPORT_IOEXP
#include <dev/pca9555.h>
#else
#define IOXP_PORT0 0x80
#endif

// #define DEBUG_KEYS
#ifdef DEBUG_KEYS
#include <stdio.h>
#define KPRINTF(args,...) printf(args,##__VA_ARGS__)
#else
#define KPRINTF(args,...)
#endif

#include <dev/keys.h>

/*!
 * \addtogroup xgDevKeys
 */
/*@{*/

typedef struct {
    void*    next;      /**< Pointer to next key or NULL on last key */
    HANDLE*  event;     /**< Handle for key event */
    void (*callback)(void);     /**< Function Pointer if key is activated */
    int      bank;		/**< GPIO bank of key */
    int      pin;       /**< GPIO pin of key */
	int      lastState;	/**< last state sampled from port */
	int      newState;	/**< current state sampled from port */
    int      fx;		/**< Action type of key */
	uint32_t fxt;		/**< time for action */
	uint32_t TimeDown;  /**< System time in ms at key down recognized */
} KEYEventT;

static KEYEventT *first_key;

HANDLE key_tmr = NULL;
HANDLE key_evt = NULL;

/*!
 * \brief reads actual state of a key
 *
 * Applications can call this functions to determine which key issued
 * the event to a calling thread. This helps if more than ome key has
 * been assigned to a single event mutex. If the key was the one which
 * raised the event, it has bit 1 set. This bit will be cleared on calling
 * this function.
 *
 * \param keyh Handle of the key to query.
 *
 * \return bit 0: 1 if key still pressed, bit 1: 1 if key raised the event.
 */
int NutGetKeyState( HANDLE keyh)
{
	KEYEventT *key = (KEYEventT *)keyh;
    int rc = key->newState;
    key->newState &= ~KEY_PENDING;
    return rc;
}

/*!
 * \brief reads the time in ms of how long the key is or was pressed
 *
 * Applications can call this functions to determine how long a key
 * is pressed.
 *
 * \param keyh Handle of the key to query.
 *
 * \return time key was pressed in ms.
 */
uint32_t NutGetKeyTime( HANDLE keyh)
{
	KEYEventT *key = (KEYEventT *)keyh;
	uint32_t now = NutGetMillis();

    return (now - key->TimeDown);
}

/*!
 * \brief Key-Timer callback handler.
 *
 * This is an internal function called by NutRegisterKey() and the
 * EventTimer for key handling.
 *
 * \param timer Internal Handle of the key timer.
 * \param arg Not used.
 */
void KeyTimerCb(HANDLE timer, void *arg)
{
    NutEventPostAsync(arg);
}

/*!
 * \brief Key-Thread.
 *
 * This is the internal key thread triggered by the key event timer.
 *
 */
THREAD( sys_key, arg)
{
	KEYEventT *key;
	uint32_t now;
#ifdef KEY_SUPPORT_IOEXP
	int ioxread, ioxstate;
#endif

    NUTASSERT( arg != NULL);

	NutThreadSetPriority( 16);
	for(;;)
	{
#ifdef KEY_SUPPORT_IOEXP
		ioxread = 0;
#endif
		if( NutEventWait( arg, NUT_WAIT_INFINITE)==0) {
			key = first_key;
			now = NutGetMillis();
			while( key)
			{
				/*
				 * Read in keys from ports
				 */
				key->newState &= ~KEY_IS_DOWN;

				if( key->bank < IOXP_PORT0) {
                    /* Save inverted key state (low-active) */
					key->newState |= (GpioPinGet( key->bank, key->pin))?0:1;
				}
#ifdef KEY_SUPPORT_IOEXP
				else {
					/* read io-expander only on first key connected
					** and buffer the result to keep bus silent
					*/
					if( ioxread == 0) {
						IOExpRawRead( key->bank, &ioxstate);
						ioxread = 1;
 					}
                    /* Save inverted key state (low-active) */
					key->newState |= ((ioxstate & (1<<key->pin))?0:1);
				}
#endif

				/*
				 * Process key status change
				 */
				if( (key->newState & KEY_IS_DOWN) > (key->lastState & KEY_IS_DOWN)) {
					/* key up->down change */
					key->TimeDown = now;
                    if( key->fx == KEY_ACTION_DOWN) {
                        KPRINTF("KD %08lx E:%08lx\n", (uint32_t)key, (uint32_t)(key->event));
                        key->newState |= KEY_PENDING;
                        if( key->event) NutEventPost( key->event);
                        if( key->callback) (*key->callback)();
                    }
				}
				else if( (key->newState & KEY_IS_DOWN) < (key->lastState & KEY_IS_DOWN)) {
					/* key down->up change */
                    key->newState &= ~KEY_IS_LOCKED;
                    if( key->fx == KEY_ACTION_UP) {
                        KPRINTF("KU %08lx E:%08lx\n", (uint32_t)key, (uint32_t)(key->event));
                        key->newState |= (KEY_ACTION_UP | KEY_PENDING);
                        if( key->event) NutEventPost( key->event);
                        if( key->callback) (*key->callback)();
                    }
                    else if( ( key->fx == KEY_ACTION_SHORT) && ( (now - key->TimeDown) < key->fxt)) {
                        KPRINTF("KS %08lx E:%08lx\n", (uint32_t)key, (uint32_t)(key->event));
                        key->newState |= (KEY_ACTION_SHORT | KEY_PENDING);
                        if( key->event) NutEventPost( key->event);
                        if( key->callback) (*key->callback)();
                    }
				}
                else if( (key->newState & KEY_IS_DOWN) && (key->fx==KEY_ACTION_HOLD)) {
                    /* key still down */
                    if( ((now - key->TimeDown) > key->fxt) && ((key->newState & KEY_IS_LOCKED) == 0)) {
                        KPRINTF("KH %08lx E:%08lx\n", (uint32_t)key, (uint32_t)(key->event));
                        key->newState |= (KEY_ACTION_HOLD | KEY_PENDING);
                        if( key->event) NutEventPost( key->event);
                        if( key->callback) (*key->callback)();
                        key->newState |= KEY_IS_LOCKED;
                    }
                }

                /* Backup new state of key */
                key->lastState = key->newState;
				/* Advance to next key */
				key = key->next;
			}
		}
	}
}

/*!
 * \brief Internal functionn to setup a GPIO port for reading a connected key.
 *
 * \param key Handle to the key, the port will be assigned to.
 *
 * \return 0 if key could be set up, -1 if not.
 */
int InitKEY( KEYEventT *key )
{
    if( key->bank < IOXP_PORT0) {
        /* configure standard GPIO pin */
        GpioPinConfigSet( key->bank, key->pin, GPIO_CFG_PULLUP|GPIO_CFG_DEBOUNCE);
        return 0;
    }
#ifdef KEY_SUPPORT_IOEXP
    else {
        /* configure externally connected GPIO pin as input */
        IOExpPinConfigSet( key->bank, key->pin, 0);
        return 0;
    }
#endif
	return -1;
}

int NutAssignKeyEvt( HANDLE *keyhp, HANDLE *event)
{
    KEYEventT *key;
    if( keyhp==NULL) return -1;

    key = (KEYEventT*)keyhp;
	key->event = event;
    key->newState &= ~KEY_PENDING;
    return 0;
}

int NutAssignKeyFkt( HANDLE *keyhp, void (*callback)(void))
{
    KEYEventT *key;
    if( keyhp==NULL) return -1;

    key = (KEYEventT*)keyhp;
	key->callback = callback;
    key->newState &= ~KEY_PENDING;
    return 0;
}

/*!
 * \brief register a key and describe its function
 *
 * \param keyh The handle to the LED that should be controlled.
 * \param event The event handle for locking a task until the key is active.
 * \param bank The GPIO or PCA9555 port, where key is connected to.
 * \param pin Pin of the bank, where key is connected to.
 * \param fx Action of key for releasing the event.
 * \param fxt Time parameter for action.
 *
 * \return 0 on success, -1 on any error.
 *
 * \note Following actions can be assigned to a key:
 * <table>
 * <tr><th>fx</th>               <th>fxt</th><th>Event released</th></tr>
 * <tr><td>>KEY_ACTION_DOWN</td> <td>0</td>  <td>on Key press</td></tr>
 * <tr><td>>KEY_ACTION_UP</td>   <td>0</td>  <td>on Key release</td></tr>
 * <tr><td>>KEY_ACTION_HOLD</td> <td>n</td>  <td>on Key pressed for n ms</td></tr>
 * <tr><td>>KEY_ACTION_SHORT</td><td>n</td>  <td>on Key released before n ms</td></tr>
 * </table>
 */
int NutRegisterKey( HANDLE *keyhp, int bank, int pin, int fx, uint32_t fxt)
{
    KEYEventT *key;

    NUTASSERT( keyhp!=NULL);

    /* Check memory constraints and assign memory to new led struct */
    key = malloc( sizeof(KEYEventT));
	*keyhp = (void*)key;

	if( key == NULL) {
		return -1;
	}

    /* Preset new key struct */
    key->bank = bank;
    key->pin = pin;
    key->fx = fx;
    key->fxt = fxt;
    key->callback = NULL;
    key->event = NULL;
    key->lastState = key->newState = 1;
    key->TimeDown = 0;

    /* Initalize key hardware */
    InitKEY( key);

    /* Assign the key to the key chain */
    NutEnterCritical();
    if( first_key == NULL) {
        /* it is the first key */
        first_key = key;
    }
    else {
        key->next = first_key;
        first_key = key;
    }
    NutExitCritical();

	if( key_tmr == NULL) {
		NutThreadCreate( "sys_key", sys_key, &key_evt, 192);
		key_tmr = NutTimerStart(10, KeyTimerCb, &key_evt, 0);
	}

    KPRINTF("KREG %08lx E:%08lx\n", (uint32_t)key, (uint32_t)(key->event));

    return 0;
}

/*@}*/
