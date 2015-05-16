#ifndef _DEV_CHAT_H_
#define _DEV_CHAT_H_

/*
 * Copyright (C) 2001-2004 by egnite Software GmbH. All rights reserved.
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
 * Revision 1.10  2008/08/11 06:59:59  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.9  2006/03/16 15:25:32  haraldkipp
 * Changed human readable strings from u_char to char to stop GCC 4 from
 * nagging about signedness.
 *
 * Revision 1.8  2005/04/30 16:42:41  chaac
 * Fixed bug in handling of NUTDEBUG. Added include for cfg/os.h. If NUTDEBUG
 * is defined in NutConf, it will make effect where it is used.
 *
 * Revision 1.7  2004/11/24 15:25:15  haraldkipp
 * Made options cnfigurable
 *
 * Revision 1.6  2004/03/17 11:29:20  haraldkipp
 * Bugfix for ICCAVR
 *
 * Revision 1.5  2004/03/16 16:48:28  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 * Revision 1.4  2004/03/08 11:18:54  haraldkipp
 * Debug output added.
 *
 * Revision 1.3  2004/01/06 18:12:36  drsung
 * Bugfix multiple define: '_chat_report' under ICCAVR
 *
 * Revision 1.2  2003/10/13 10:15:54  haraldkipp
 * Added Jelle's report function
 *
 * Revision 1.1.1.1  2003/05/09 14:41:05  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.1  2003/03/31 14:53:23  harald
 * Prepare release 3.1
 *
 */

#include <cfg/os.h>
#include <cfg/chat.h>
#include <stdint.h>

#define CHAT_ARG_SEND           0
#define CHAT_ARG_ABORT          1
#define CHAT_ARG_TIMEOUT        2
#define CHAT_ARG_REPORT         3

/*!
 * \brief Maximum number of abort strings.
 */
#ifndef CHAT_MAX_ABORTS
#define CHAT_MAX_ABORTS         10
#endif

/*!
 * \brief Maximum size of report strings.
 */
#ifndef CHAT_MAX_REPORT_SIZE
#define CHAT_MAX_REPORT_SIZE    32
#endif

/*!
 * \brief Default timeout.
 */
#ifndef CHAT_DEFAULT_TIMEOUT
#define CHAT_DEFAULT_TIMEOUT    45
#endif

typedef struct {
    int chat_fd;
    uint8_t chat_arg;
    uint8_t chat_aborts;
    char *chat_abort[CHAT_MAX_ABORTS];
    uint8_t chat_abomat[CHAT_MAX_ABORTS];
    char *chat_report_search;
    uint8_t chat_repmat;
    char chat_report_state;
} NUTCHAT;

extern uint8_t *chat_report;

#ifdef NUTDEBUG
#include <stdio.h>
#endif

__BEGIN_DECLS

#ifdef NUTDEBUG
void NutTraceChat(FILE * stream, uint8_t flags);
#endif

int NutChatExpectString(NUTCHAT *ci, char *str);
int NutChatExpect(NUTCHAT *ci, char *str);
int NutChatSend(NUTCHAT *ci, char *str);
NUTCHAT *NutChatCreate(int fd);
void NutChatDestroy(NUTCHAT *ci);
int NutChat(int fd, CONST char *script);
#ifdef __HARVARD_ARCH__
int NutChat_P(int fd, PGM_P script);
#endif

__END_DECLS

#endif
