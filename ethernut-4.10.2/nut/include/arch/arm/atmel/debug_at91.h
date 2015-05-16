#ifndef _ARCH_ARM_DEV_DEBUG_H_
#define _ARCH_ARM_DEV_DEBUG_H_

/*
 * Copyright (C) 2010 by egnite GmbH
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

/*!
 * \file arch/arm/debug_at91.h
 * \brief AT91 debug output device.
 *
 * \verbatim
 * $Id$
 * \endverbatim
 */

#include <sys/file.h>
#include <sys/device.h>

__BEGIN_DECLS
/* Prototypes */

extern NUTFILE *At91DevDebugOpen(NUTDEVICE * dev, CONST char *name, int mode, int acc);
extern int At91DevDebugClose(NUTFILE * fp);
extern int At91DevDebugWrite(NUTFILE * fp, CONST void *buffer, int len);
extern int At91DevDebugRead(NUTFILE * fp, void *buffer, int size);
extern long At91DevDebugSize(NUTFILE *fp);
extern int At91DevDebugIOCtl(NUTDEVICE * dev, int req, void *conf);
extern uint32_t At91BaudRateDiv(uint32_t baud);

__END_DECLS
/* End of prototypes */
#endif
