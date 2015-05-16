/*
 * Copyright (C) 2004 by egnite Software GmbH. All rights reserved.
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
 * -
 * Portions Copyright (c) 1983, 1988, 1993
 *      The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*!
 * \file pro/syslog_P.c
 * \brief Syslog client routines for Harvard architecture
 *
 * \verbatim
 * $Id$
 * \endverbatim
 */

#include <cfg/syslog.h>

#include <stdio.h>
#include <stdarg.h>
#include <memdebug.h>

#define SYSLOG_INTERNAL
#include <sys/syslog.h>

/*!
 * \addtogroup xgSyslog
 */
/*@{*/

#ifdef __HARVARD_ARCH__
/*!
 * \brief Print log message.
 *
 * Alternate form of syslog(), in which the arguments have already been captured
 * using the variable-length argument facilities.
 *
 * This variant is useful for Harvard architectures only. It allows
 * to place the format string in program space. For other CPUs it
 * is mapped to vsyslog().
 *
 * \param pri   Priority level of this message. See syslog().
 * \param fmt_P Format string containing conversion specifications like printf.
 * \param ap    List of arguments.
 */
void vsyslog_P(int pri, PGM_P fmt_P, va_list ap)
{
    /* Build the header. */
    size_t cnt = syslog_header(pri);

    if (cnt) {
#ifdef SYSLOG_PERROR_ONLY
        fputs(syslog_buf, stderr);
        vfprintf_P(stderr, fmt_P, ap);
        fputc('\n', stderr);
#else
        /* Potentially dangerous. We need vsnprintf() */
        if (cnt + strlen_P(fmt_P) >= SYSLOG_MAXBUF) {
            return;
        }
        cnt += vsprintf_P(&syslog_buf[cnt], fmt_P, ap);
        syslog_flush(cnt);
#endif /* SYSLOG_PERROR_ONLY */
    }
}

/*!
 * \brief Print log message.
 *
 * The message is tagged with priority.
 *
 * This variant is useful for Harvard architectures only. It allows
 * to place the format string in program space. For other CPUs it
 * is mapped to syslog().
 *
 * \param pri Priority level of this message, selected from the following
 *            ordered list (high to low):
 *            - LOG_EMERG   A panic condition.
 *            - LOG_ALERT   A condition that should be corrected immediately.
 *            - LOG_CRIT    Critical conditions, e.g., hard device errors.
 *            - LOG_ERR     Errors.
 *            - LOG_WARNING Warning messages.
 *            - LOG_NOTICE  Conditions that are not error conditions, but should 
 *                          possibly be handled specially.
 *            - LOG_INFO    Informational messages.
 *            - LOG_DEBUG   Messages that contain information normally of use only 
 *                          when debugging a program.
 * \param fmt_P Format string containing conversion specifications like printf.
 */
void syslog_P(int pri, PGM_P fmt_P, ...)
{
    va_list ap;

    va_start(ap, fmt_P);
    vsyslog_P(pri, fmt_P, ap);
    va_end(ap);
}

#endif /* __HARVARD_ARCH__ */

/*@}*/

