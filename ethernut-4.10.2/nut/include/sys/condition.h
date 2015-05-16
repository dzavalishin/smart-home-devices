#ifndef _SYS_CONDITION_H_
#define _SYS_CONDITION_H_

/*
 * Copyright (C) 2008 by EmbeddedIT,
 * Ole Reinhardt <ole.reinhardt@embedded-it.de> All rights reserved.
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
 * $Log$
 * Revision 1.2  2008/04/29 01:51:52  thiagocorrea
 * Compile fix
 *
 * Revision 1.1  2008/04/21 22:24:53  olereinhardt
 * Implemented condition variables to use with NutOS as an application candy
 *
 */

#include <sys/atom.h>
#include <sys/mutex.h>
#include <sys/types.h>

/*!
 * \addtogroup xgConditionVariables
 */
/*@{*/

/*!
 * \brief Condition variable type.
 */
typedef struct _CONDITION CONDITION;

/*!
 * \struct _CONDITION condition.h sys/condition.h
 * \brief Condition variable
 *
 */
struct _CONDITION {
    HANDLE event; /*!< \brief Event queue used for signaling */
    MUTEX  mutex; /*!< \brief Mutex used for locking */
};


__BEGIN_DECLS

extern int NutConditionInit(CONDITION * cond);
extern void NutConditionLock(CONDITION * cond);
extern void NutConditionUnlock(CONDITION * cond);
extern int NutConditionWait(CONDITION * cond);
extern int NutConditionSignal(CONDITION * cond);
extern int NutConditionBroadcast(CONDITION * cond);
extern void NutConditionFree(CONDITION * cond);

__END_DECLS

/*@}*/

#endif
