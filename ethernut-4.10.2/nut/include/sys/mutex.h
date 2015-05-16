/*
 * Copyright (C) 2000-2004 by ETH Zurich
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
 * THIS SOFTWARE IS PROVIDED BY ETH ZURICH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ETH ZURICH
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
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

/* mutex.c - a nut/os implementation of recursive mutex functions
 *
 * 2004.05.06 Matthias Ringwald <matthias.ringwald@inf.ethz.ch>
 *
 */
/*
 * $Log$
 * Revision 1.6  2008/08/11 07:00:26  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.5  2007/08/29 07:43:54  haraldkipp
 * Documentation updated and corrected.
 *
 * Revision 1.4  2005/12/22 09:36:25  freckle
 * added missing prototype for NutMutexDestroy
 *
 * Revision 1.3  2005/08/02 17:46:49  haraldkipp
 * Major API documentation update.
 *
 * Revision 1.2  2004/05/18 18:38:14  drsung
 * Added $Log keyword for CVS and avoid multiple inclusion of header file.
 *
 */

#ifndef _SYS_MUTEX_H
#define _SYS_MUTEX_H

#include <compiler.h>
#include <sys/types.h>
#include <sys/thread.h>

/*!
 * \addtogroup xgMutex
 */
/*@{*/

__BEGIN_DECLS

/*!
 * \brief Recursive mutex type.
 */
typedef struct _MUTEX MUTEX;

/*!
 * \struct _MUTEX mutex.h sys/mutex.h
 * \brief Recursive mutex.
 *
 */
struct _MUTEX {
    HANDLE qhp;             /*!< \brief Queue to wait, if mutex is not free. */
    NUTTHREADINFO *thread;  /*!< \brief Thread who owns the mutex */
    uint16_t count;          /*!< \brief Lock counter. */
};

extern void NutMutexInit(MUTEX * mutex);
extern void NutMutexLock(MUTEX * mutex);
extern int NutMutexUnlock(MUTEX * mutex);
extern int NutMutexTrylock(MUTEX * mutex);
extern int NutMutexDestroy(MUTEX * mutex);
__END_DECLS

/*@}*/

#endif /* #ifndef _SYS_MUTEX_H */

