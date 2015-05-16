#ifndef _DEV_WATCHDOG_H_
#define	_DEV_WATCHDOG_H_

/*
 * Copyright (C) 2006 by egnite Software GmbH. All rights reserved.
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

/*
 * $Log$
 * Revision 1.4  2008/09/02 14:32:25  haraldkipp
 * Includes stdint header.
 *
 * Revision 1.3  2008/08/11 06:59:59  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.2  2006/05/25 09:13:23  haraldkipp
 * Platform independent watchdog API added.
 *
 * Revision 1.1  2006/02/23 15:36:35  haraldkipp
 * Added support for AT91 watchdog timer.
 *
 */

#include <sys/types.h>
#include <stdint.h>

/*!
 * \addtogroup xgWatchDog
 */
/*@{*/


__BEGIN_DECLS
/* Prototypes */

/*!
 * \brief Start the watch dog timer.
 *
 * This function can be used by applications to prevent hang-ups.
 *
 * \param ms    Watch dog time out in milliseconds.
 * \param xmode Hardware specific mode. If 0, the default mode is used. 
 *              In this mode, the watch dog will reset the CPU if not 
 *              restarted within the specified time out period.
 *
 * \return The actual time out value, which may differ from the
 *         specified value due to hardware limitations. The watch
 *         dog timer will be automatically enabled on return.
 *
 * The following code fragment starts the watch timer with a time out
 * of 550 milliseconds and restarts it every 500 milliseconds.
 *
 * \code
 * #include <dev/watchdog.h>
 * #include <sys/timer.h>
 *
 * NutWatchDogStart(550, 0);
 * for(;;) {
 *     NutWatchDogRestart();
 *     NutSleep(500);
 * }
 *
 * \endcode
 *
 * \todo AVR implementation.
 *
 * \note The AT91 implementation does not calculate the actual time out 
 *       value, but simply returns the specified number of milliseconds.
 */
extern uint32_t NutWatchDogStart(uint32_t ms, uint32_t xmode);


/*!
 * \brief Restart the watch dog timer.
 */
extern void NutWatchDogRestart(void);

/*!
 * \brief Disables the watch dog timer.
 *
 * Applications should call this function to temporarily disable the
 * watch dog timer. To re-enable it, call NutWatchDogEnable().
 *
 * \code
 * #include <dev/watchdog.h>
 *
 * NutWatchDogStart(100, 0);
 *
 * //Some code here.
 *
 * NutWatchDogRestart();
 *
 * //Some code here.
 *
 * NutWatchDogDisable();
 *
 * //Some lengthy code here, like writing to flash memory.
 *
 * NutWatchDogEnable();
 *
 * \endcode
 */
extern void NutWatchDogDisable(void);

/*!
 * \brief Enables the watch dog timer.
 *
 * The function can be safely used within nested subroutines.
 * The watch dog will be enabled only, if this function is called
 * the same number of times as NutWatchDogDisable(). If enabled,
 * the watch dog timer will also have been re-started and the
 * full time out value is available before another NutWatchDogRestart()
 * is required.
 *
 * If the watch has not been started by NutWatchDogStart(), then this
 * function does nothing.
 */
extern void NutWatchDogEnable(void);

#if defined(__AVR__)
extern uint32_t AvrWatchDogStart(uint32_t ms);
extern void AvrWatchDogRestart(void);
extern void AvrWatchDogDisable(void);
extern void AvrWatchDogEnable(void);
#elif defined(MCU_AT91R40008) || defined(MCU_AT91SAM7X) || defined(MCU_AT91SAM7S) || defined(MCU_AT91SAM7SE)
extern uint32_t At91WatchDogStart(uint32_t ms, uint32_t xmode);
extern void At91WatchDogRestart(void);
extern void At91WatchDogDisable(void);
extern void At91WatchDogEnable(void);
#elif defined(__AVR32__)
extern uint32_t Avr32WatchDogStart(uint32_t ms);
extern void Avr32WatchDogRestart(void);
extern void Avr32WatchDogDisable(void);
extern void Avr32WatchDogEnable(void);
#endif

__END_DECLS
/* End of prototypes */

/*@}*/

#endif
 
