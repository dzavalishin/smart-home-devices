# if 0
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
 * \file pro/syslog.c
 * \brief Syslog Client
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.5  2009/02/13 14:52:05  haraldkipp
 * Include memdebug.h for heap management debugging support.
 *
 * Revision 1.4  2008/08/11 07:00:36  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.3  2005/08/02 17:47:04  haraldkipp
 * Major API documentation update.
 *
 * Revision 1.2  2004/10/03 18:41:43  haraldkipp
 * RAM saving calls added
 *
 * Revision 1.1  2004/09/19 11:18:45  haraldkipp
 * Syslog client added
 *
 * \endverbatim
 */

#include <cfg/syslog.h>
#include <sys/confos.h>
#include <sys/confnet.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <io.h>
#include <time.h>
#include <memdebug.h>

#define SYSLOG_INTERNAL
#include <sys/syslog.h>

#ifndef SYSLOG_PERROR_ONLY

#include <sys/socket.h>
#include <arpa/inet.h>
static UDPSOCKET *syslog_sock;

#ifndef SYSLOG_PORT
#define SYSLOG_PORT     514
#endif

static uint32_t syslog_server;
static uint16_t syslog_port = SYSLOG_PORT;

#endif                          /* SYSLOG_PERROR_ONLY */

/*!
 * \addtogroup xgSyslog
 */
/*@{*/

static int syslog_fac = LOG_USER;
static int syslog_mask = 0xFF;

static int syslog_stat;
static size_t syslog_taglen;
static char *syslog_tag;
char *syslog_buf;

/*!
 * \brief Assemble syslog header.
 *
 * For internal use only.
 *
 * \param pri Value of the syslog PRI part.
 *
 * \return Number of characters in the global syslog buffer.
 */
size_t syslog_header(int pri)
{
    size_t rc;

    /* Remove invalid bits. */
    pri &= LOG_PRIMASK | LOG_FACMASK;

    /* Check priority against setlog mask values. */
    if ((LOG_MASK(LOG_PRI(pri)) & syslog_mask) == 0) {
        return 0;
    }

    /* Set default facility if none specified. */
    if ((pri & LOG_FACMASK) == 0) {
        pri |= syslog_fac;
    }

    /* Open log if not done before. */
    if (syslog_buf == 0) {
        openlog(0, syslog_stat | LOG_NDELAY, syslog_fac);
    }

    /* PRI field.
    ** This is common to all syslog formats. */
    rc = sprintf(syslog_buf, "<%d>", pri);

    /* VERSION field.
    ** Note, that there is no space separator. */
#ifdef SYSLOG_RFC5424
    syslog_buf[rc++] = '1';
#endif

    /* TIMESTAMP field. */
#ifdef SYSLOG_OMIT_TIMESTAMP

#ifdef SYSLOG_RFC5424
    syslog_buf[rc++] = ' ';
    syslog_buf[rc++] = '-';
#endif

#else
    {
        time_t now;
        struct _tm *tip;

        time(&now);

#ifdef SYSLOG_RFC5424
        tip = gmtime(&now);
        rc += sprintf(&syslog_buf[rc], " %04d-%02d-%02dT%02d:%02d:%02dZ",
            tip->tm_year + 1900, tip->tm_mon + 1, tip->tm_mday,
            tip->tm_hour, tip->tm_min, tip->tm_sec);
#else
        {
            static char mon_name[] = "JanFebMarAprMayJunJulAugSepOctNovDec";

            tip = localtime(&now);
            rc += sprintf(&syslog_buf[rc], "%.3s%3d %02d:%02d:%02d",
                &mon_name[tip->tm_mon * 3], tip->tm_mday,
                tip->tm_hour, tip->tm_min, tip->tm_sec);
        }
#endif /* SYSLOG_RFC5424 */

    }
#endif /* SYSLOG_OMIT_TIMESTAMP */

    /* HOSTNAME field. */
#ifdef SYSLOG_OMIT_HOSTNAME

#ifdef SYSLOG_RFC5424
    syslog_buf[rc++] = ' ';
    syslog_buf[rc++] = '-';
#endif

#else

#ifdef SYSLOG_RFC5424
    syslog_buf[rc++] = ' ';
    if (confnet.cdn_cip_addr) {
        strcpy(&syslog_buf[rc], inet_ntoa(confnet.cdn_cip_addr));
        rc += strlen(&syslog_buf[rc]);
    }
    else if (confos.hostname[0]) {
        strcpy(&syslog_buf[rc], confos.hostname);
        rc += strlen(&syslog_buf[rc]);
    }
    else if (confnet.cdn_ip_addr) {
        strcpy(&syslog_buf[rc], inet_ntoa(confnet.cdn_ip_addr));
        rc += strlen(&syslog_buf[rc]);
    } else {
        syslog_buf[rc++] = '-';
    }
#else
    syslog_buf[rc++] = ' ';
    strcpy(&syslog_buf[rc], confos.hostname);
    rc += strlen(confos.hostname);
#endif /* SYSLOG_RFC5424 */

#endif /* SYSLOG_OMIT_HOSTNAME */

    /* APP-NAME field. */
    if (syslog_taglen) {
        syslog_buf[rc++] = ' ';
        strcpy(&syslog_buf[rc], syslog_tag);
        rc += syslog_taglen;
#ifndef SYSLOG_RFC5424
        syslog_buf[rc++] = ':';
#endif
    }

    /* No PROCID and MSGID fields. */
#ifdef SYSLOG_RFC5424
    syslog_buf[rc++] = ' ';
    syslog_buf[rc++] = '-';
    syslog_buf[rc++] = ' ';
    syslog_buf[rc++] = '-';
#endif

    syslog_buf[rc++] = ' ';
    syslog_buf[rc] = '\0';

    return rc;
}

#ifndef SYSLOG_PERROR_ONLY
/*!
 * \brief Send syslog buffer.
 *
 * For internal use only.
 *
 * \param len Number of valid characters in buffer.
 */
void syslog_flush(size_t len)
{
    /* Output to stderr if requested */
    //if (syslog_stat & LOG_PERROR)
    {
        //_write(_fileno(stderr), syslog_buf, len);
        //_write(_fileno(stderr), "\n", 1);
        _write(_fileno(stdout), syslog_buf, len);
        _write(_fileno(stdout), "\n", 1);
    }
/*
    // Output the message to a remote logger.
    if (syslog_server) {
        NutUdpSendTo(syslog_sock, syslog_server, syslog_port, syslog_buf, len);
    }*/
}
#endif

/*!
 * \brief Print log message.
 *
 * Alternate form of syslog(), in which the arguments have already been captured
 * using the variable-length argument facilities.
 *
 * \param pri Priority level of this message. See syslog().
 * \param fmt Format string containing conversion specifications like printf.
 * \param ap  List of arguments.
 */
void vsyslog(int pri, const char *fmt, va_list ap)
{
    if( syslog_buf == 0 )
    {
        puts("Buffer == 0");
        return;
    }

    /* Build the header. */
    size_t cnt = 0; //syslog_header(pri);


    //_write(_fileno(stdout), fmt, strlen(fmt));
#if 1
    //if (cnt)
    {
        /* Potentially dangerous. We need vsnprintf() * /
        if (cnt + strlen(fmt) >= SYSLOG_MAXBUF) {
            puts("Buffer overflow");
            return;
        } */
        //cnt += vsprintf(&syslog_buf[cnt], fmt, ap);
        //if(fmt) cnt += vsprintf(syslog_buf+cnt, fmt, ap);
        if(fmt) cnt += vsnprintf(syslog_buf+cnt, SYSLOG_MAXBUF-cnt, fmt, ap);
        syslog_flush(cnt);
    }
#endif
}

/*!
 * \brief Print log message.
 *
 * The message is tagged with priority.
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
 * \param fmt Format string containing conversion specifications like printf.
 */
void syslog(int pri, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vsyslog(pri, (char *) fmt, ap);
    va_end(ap);
}

/*!
 * \brief Set the log priority mask level.
 *
 * Calls to syslog() with a priority not set are ignored. The default allows all
 * priorities to be logged.
 *
 * \param logmask New priority mask.
 *
 * \return Previous mask.
 */
int setlogmask(int logmask)
{
    int rc = syslog_mask;

    if (logmask) {
        syslog_mask = logmask;
    }
    return rc;
}

/*!
 * \brief Set the log server's IP address.
 *
 * \param ip   IP address in network byte order. If 0, no messages will be sent out.
 * \param port Port number. If 0, then standard port is used.
 *
 * \return Previous IP or (uint32_t)-1 (all bits set) if UDP is not supported.
 */
uint32_t setlogserver(uint32_t ip, uint16_t port)
{
#ifdef SYSLOG_PERROR_ONLY
    return (uint32_t)-1;
#else
    uint32_t rc = syslog_server;

    syslog_server = ip;
    if (port) {
        syslog_port = port;
    }
    return rc;
#endif
}

/*!
 * \brief Open logging for specialized processing.
 *
 * This function provides for more specialized processing of the messages sent by
 * syslog() and vsyslog().
 *
 * \param ident   This string that will be prepended to every message.
 * \param logstat A bit field specifying logging options, which is formed by
 *                OR'ing one or more of the following values:
 *                - LOG_CONS   Ignored but accepted for compatibility.
 *                - LOG_NDELAY Ignored but accepted for compatibility.
 *                - LOG_PERROR Additionally write the message to standard error output.
 *                - LOG_PID    Ignored but accepted for compatibility.
 * \param logfac  Encodes a default facility to be assigned to all messages that do not
 *                have an explicit facility encoded:
 *                - LOG_AUTH     The authorization system:
 *                - LOG_AUTHPRIV The same as LOG_AUTH.
 *                - LOG_CRON     The cron daemon,
 *                - LOG_DAEMON   System daemons, that are not provided for explicitly by
 *                               other facilities.
 *                - LOG_FTP      The file transfer protocol daemon,
 *                - LOG_KERN     Messages generated by the kernel. These should not be
 *                               generated by applications.
 *                - LOG_LPR      The line printer spooling system.
 *                - LOG_MAIL     The mail system.
 *                - LOG_NEWS     The network news system.
 *                - LOG_SYSLOG   Messages generated internally by syslog.
 *                - LOG_USER     This is the default facility identifier if none is specified.
 *                - LOG_UUCP     The UUCP system.
 *                - LOG_LOCAL0..LOG_LOCAL7 Reserved for local use.
 */
void openlog(const char *ident, int logstat, int logfac)
{
    if (ident == 0) {
        ident = syslog_tag;
        syslog_taglen = 0;
    }

    closelog();

    syslog_stat = logstat;
    syslog_fac = logfac;
    if (ident && *ident) {
        syslog_taglen = strlen(ident);
        syslog_tag = malloc(syslog_taglen + 1);
        strcpy(syslog_tag, ident);
    }
    //if (syslog_stat & LOG_NDELAY)
    {
        if (syslog_buf == 0) {
            syslog_buf = malloc(SYSLOG_MAXBUF);
        }
#ifndef SYSLOG_PERROR_ONLY
        if (syslog_sock == 0) {
            syslog_sock = NutUdpCreateSocket(514);
        }
#endif
    }
}

/*!
 * \brief Release system resources occupied by syslog().
 */
void closelog(void)
{
    if (syslog_buf) {
        free(syslog_buf);
        syslog_buf = 0;
    }
    if (syslog_taglen) {
        free(syslog_tag);
        syslog_taglen = 0;
    }
#ifndef SYSLOG_PERROR_ONLY
    if (syslog_sock) {
        NutUdpDestroySocket(syslog_sock);
        syslog_sock = 0;
    }
#endif
}

/*@}*/


#endif // 0

