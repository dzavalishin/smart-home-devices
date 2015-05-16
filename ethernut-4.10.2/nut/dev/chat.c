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
 * Revision 1.11  2009/02/13 14:52:05  haraldkipp
 * Include memdebug.h for heap management debugging support.
 *
 * Revision 1.10  2009/02/06 15:40:29  haraldkipp
 * Using newly available strdup() and calloc().
 * Replaced NutHeap routines by standard malloc/free.
 * Replaced pointer value 0 by NULL.
 *
 * Revision 1.9  2008/08/11 06:59:41  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.8  2007/08/17 11:34:00  haraldkipp
 * Default timeout needs to be multiplied by 1000.
 *
 * Revision 1.7  2005/04/30 16:42:41  chaac
 * Fixed bug in handling of NUTDEBUG. Added include for cfg/os.h. If NUTDEBUG
 * is defined in NutConf, it will make effect where it is used.
 *
 * Revision 1.6  2004/03/16 16:48:27  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 * Revision 1.5  2004/03/08 11:12:59  haraldkipp
 * Debug output added.
 *
 * Revision 1.4  2004/01/15 08:02:35  drsung
 * Copyright added
 *
 */

#include <cfg/os.h>
#include <sys/timer.h>
#include <dev/uart.h>

#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <memdebug.h>

#include <dev/chat.h>

uint8_t *chat_report;

#ifdef NUTDEBUG

static FILE *__chat_trs;        /*!< \brief Chat trace output stream. */
static uint8_t __chat_trf;       /*!< \brief Chat trace flags. */

/*!
 * \brief Control chat tracing.
 *
 * \param stream Pointer to a previously opened stream or null to 
 *               disable trace output.
 * \param flags  Flags to enable specific traces.
 */
void NutTraceChat(FILE * stream, uint8_t flags)
{
    if (stream)
        __chat_trs = stream;
    if (__chat_trs) {
        static prog_char dbgfmt[] = "Chat trace flags=0x%02X\n";
        __chat_trf = flags;
        fprintf_P(__chat_trs, dbgfmt, flags);
    } else
        __chat_trf = 0;
}

#endif

/*
 * Special version of strchr, handling escaped characters.
 */
static char *strechr(CONST char *str, int c)
{
    while (*str) {
        if (*str == '\\') {
            if (*++str)
                str++;
        } else if (*str == c)
            return (char *) str;
        else
            str++;
    }
    return 0;
}

/*!
 * \brief Wait for a specific string to appear.
 *
 * \param ci  Pointer to a NUTCHAT structure, which must have been 
 *            created by NutChatCreate().
 * \param str Expected string. May be empty if nothing is expected.
 *
 * \return 0 on success, 3 in case of a timeout error while waiting
 *         for an expected string, or the index of an abort string 
 *         plus 4, if one has been received. 
 */
int NutChatExpectString(NUTCHAT * ci, char *str)
{
    char ch;
    uint8_t m;
    uint8_t i;
    char *cp = str;

#ifdef NUTDEBUG
    if (__chat_trf) {
        static prog_char dbgfmt[] = "Expect '%s', got '";
        fprintf_P(__chat_trs, dbgfmt, str);
    }
#endif

    while (*cp) {

        /*
         * Read the next character. Return on timeout.
         */
        if (_read(ci->chat_fd, &ch, 1) != 1) {
#ifdef NUTDEBUG
            if (__chat_trf) {
                static prog_char dbgmsg[] = "' TIMEOUT\n";
                fputs_P(dbgmsg, __chat_trs);
            }
#endif
            return 3;
        }
#ifdef NUTDEBUG
        if (__chat_trf) {
            if (ch > 31 && ch < 127) {
                fputc(ch, __chat_trs);
            } else {
                fprintf(__chat_trs, "\\x%02X", ch);
            }
        }
#endif
        /*
         * If the character doesn't match the next expected one,
         * then restart from the beginning of the expected string.
         */
        if (ch != *cp) {
            cp = str;
        }

        /*
         * If the character matched, advance the pointer into
         * the expected string.
         */
        if (ch == *cp) {
            cp++;
        }

        /*
         * Check for abort strings.
         */
        for (i = 0; i < ci->chat_aborts; i++) {
            m = ci->chat_abomat[i];
            if (ch == ci->chat_abort[i][m]) {
                if (ci->chat_abort[i][++m] == 0) {
#ifdef NUTDEBUG
                    if (__chat_trf) {
                        static prog_char dbgmsg[] = "' ABORT\n";
                        fputs_P(dbgmsg, __chat_trs);
                    }
#endif
                    return i + 4;
                }
            } else
                m = (ch == ci->chat_abort[i][0]);
            ci->chat_abomat[i] = m;
        }

        /*
         * Check for report strings.
         */
        if (ci->chat_report_state > 0) {
            m = ci->chat_repmat;
            if (ci->chat_report_state == 2) {
                chat_report[m++] = ch;
            } else if (ch == ci->chat_report_search[m]) {
                chat_report[m++] = ch;
                if (ci->chat_report_search[m] == 0) {
                    ci->chat_report_state = 2;
                }
            } else {
                m = (ch == ci->chat_report_search[0]);
            }
            ci->chat_repmat = m;
        }
    }

    /*
     * Read the remainder of the string before NutChatSendString clears it
     */
    if (ci->chat_report_state == 2) {
        m = ci->chat_repmat;    /* not needed... (but not nice to remove it) */
        while (m < CHAT_MAX_REPORT_SIZE) {
            if (_read(ci->chat_fd, &ch, 1) != 1 || ch < ' ') {
                break;
            }
            chat_report[m++] = ch;

#ifdef NUTDEBUG
            if (__chat_trf) {
                if (ch > 31 && ch < 127) {
                    fputc(ch, __chat_trs);
                } else {
                    fprintf(__chat_trs, "\\x%02X", ch);
                }
            }
#endif
        }
        ci->chat_report_state = 0;      /* Only find first occurence */
        chat_report[m] = 0;
    }
#ifdef NUTDEBUG
    if (__chat_trf) {
        static prog_char dbgmsg[] = "'\n";
        fputs_P(dbgmsg, __chat_trs);
    }
#endif

    return 0;
}

/*
 * \return 0 on success or 2 in case of an I/O error.
 */
static int NutChatSendString(int fd, char *str)
{
    int rc = 0;
    uint8_t eol = 1;
    uint8_t skip;
    char ch;

#ifdef NUTDEBUG
    if (__chat_trf) {
        static prog_char dbgfmt[] = "Send '%s'\n";
        fprintf_P(__chat_trs, dbgfmt, str);
    }
#endif

    /* Flush input buffer. */
    _read(fd, 0, 0);
    while (*str && eol && rc == 0) {
        ch = *str++;
        skip = 0;
        if (ch == '^') {
            ch = *str++;
            ch &= 0x1f;
        } else if (ch == '\\') {
            ch = *str++;
            switch (ch) {
            case 'b':
                ch = '\b';
                break;
            case 'c':
                eol = 0;
                skip = 1;
                break;
            case 'd':
                NutSleep(1000);
                skip = 1;
                break;
            case 'n':
                ch = '\n';
                break;
            case 'N':
                ch = 0;
                break;
            case 'p':
                NutDelay(100);
                skip = 1;
                break;
            case 'r':
                ch = '\r';
                break;
            case 's':
                ch = ' ';
                break;
            case 't':
                ch = '\t';
                break;
            default:
                if (ch >= '0' && ch <= '7') {
                    ch &= 0x07;
                    if (*str >= '0' && *str <= '7') {
                        ch <<= 3;
                        ch |= *str++ & 0x07;
                        if (*str >= '0' && *str <= '7') {
                            ch <<= 3;
                            ch |= *str++ & 0x07;
                        }
                    }
                }
                break;
            }
        }
        if (skip)
            skip = 0;
        else {
            NutDelay(10);
            if (_write(fd, &ch, 1) != 1)
                rc = 2;
            else
                _write(fd, 0, 0);
        }
    }
    if (eol && rc == 0 && _write(fd, "\r", 1) != 1)
        rc = 2;
    else
        _write(fd, 0, 0);

    return rc;
}

/*
 * \param ci Pointer to a NUTCHAT structure, which must have been 
 *           created by NutChatCreate().
 *
 * \return 0 on success, 1 in case of invalid parameters, 2 in case
 *         of an I/O error, 3 in case of a timeout error while waiting
 *         for an expected string, or the index of an abort string plus
 *         4, if one has been received. 
 */
int NutChatExpect(NUTCHAT * ci, char *str)
{
    int rc = 0;
    char *reply;
    char *subexpect;

    /*
     * Process special keywords.
     */
    if (strcmp(str, "ABORT") == 0) {
        ci->chat_arg = CHAT_ARG_ABORT;
        return 0;
    }
    if (strcmp(str, "TIMEOUT") == 0) {
        ci->chat_arg = CHAT_ARG_TIMEOUT;
        return 0;
    }
    if (strcmp(str, "REPORT") == 0) {
        ci->chat_repmat = 0;    /* not needed ??? */
        ci->chat_report_state = 1;
        ci->chat_arg = CHAT_ARG_REPORT;
        return 0;
    }

    /*
     * Process expected string.
     */
    while (str) {
        if ((reply = strechr(str, '-')) != 0) {
            *reply++ = 0;
            if ((subexpect = strechr(reply, '-')) != 0)
                *subexpect++ = 0;
        } else
            subexpect = 0;

        if ((rc = NutChatExpectString(ci, str)) != 3 || reply == 0)
            break;
        if ((rc = NutChatSendString(ci->chat_fd, reply)) != 0)
            break;
        str = subexpect;
    }
    return rc;
}

/*!
 * \brief Process a chat send argument.
 *
 * \param ci  Pointer to a NUTCHAT structure, which must have been 
 *            created by NutChatCreate().
 * \param str String containing the chat send argument.
 *
 * \return 0 on success, 1 in case of invalid parameters, 2 in case
 *         of an I/O error, 3 in case of a timeout error while waiting
 *         for an expected string, or the index of an abort string plus
 *         4, if one has been received. 
 */
int NutChatSend(NUTCHAT * ci, char *str)
{
    char *cp;
    char ch;
    long lv;

    /*
     * Add a chat abort string.
     */
    if (ci->chat_arg == CHAT_ARG_ABORT) {
        ci->chat_arg = CHAT_ARG_SEND;

        if (ci->chat_aborts >= CHAT_MAX_ABORTS)
            return 1;
        cp = malloc(strlen(str) + 1);
        ci->chat_abort[ci->chat_aborts++] = cp;
        while (*str) {
            ch = *str++;
            if (ch == '^')
                *cp = *str++ & 0x1f;
            else if (ch == '\\') {
                ch = *str++;
                switch (ch) {
                case 'b':
                    *cp++ = '\b';
                    break;
                case 'n':
                    *cp++ = '\n';
                    break;
                case 'r':
                    *cp++ = '\r';
                    break;
                case 's':
                    *cp++ = ' ';
                    break;
                case 't':
                    *cp++ = '\t';
                    break;
                default:
                    if (ch >= '0' && ch <= '7') {
                        ch &= 0x07;
                        if (*str >= '0' && *str <= '7') {
                            ch <<= 3;
                            ch |= *str++ & 0x07;
                            if (*str >= '0' && *str <= '7') {
                                ch <<= 3;
                                ch |= *str++ & 0x07;
                            }
                        }
                    }
                    if (ch)
                        *cp++ = ch;
                    break;
                }
            } else
                *cp++ = ch;
        }
        *cp = 0;
        return 0;
    }

    /*
     * Set chat timeout.
     */
    if (ci->chat_arg == CHAT_ARG_TIMEOUT) {
        ci->chat_arg = CHAT_ARG_SEND;

        lv = atol(str) * 1000L;
        if (lv <= 0)
            lv = CHAT_DEFAULT_TIMEOUT * 1000L;

        _ioctl(ci->chat_fd, UART_SETREADTIMEOUT, &lv);

        return 0;
    }

    /*
     * Set report string
     */
    if (ci->chat_arg == CHAT_ARG_REPORT) {
        ci->chat_arg = CHAT_ARG_SEND;
        chat_report = malloc(CHAT_MAX_REPORT_SIZE + 1);
        cp = malloc(strlen(str) + 1);
        ci->chat_report_search = cp;
        while (*str)
            *cp++ = *str++;     /* Do it the easy way, not as smart and thorough as the abort string... */
        *cp = 0;

        return 0;
    }

    /*
     * Send the argument string.
     */
    return NutChatSendString(ci->chat_fd, str);
}


/*!
 * \brief Create a NUTCHAT structure.
 *
 * \return Pointer to a new NUTCHAT structure.
 */
NUTCHAT *NutChatCreate(int fd)
{
    NUTCHAT *ci;

    if ((ci = malloc(sizeof(NUTCHAT))) != 0) {
        memset(ci, 0, sizeof(NUTCHAT));
        ci->chat_fd = fd;
    }
    return ci;
}

/*!
 * \brief Destroy a previously created NUTCHAT structure.
 *
 * \param ci Pointer to a NUTCHAT structure, which must have been 
 *           created by NutChatCreate().
 */
void NutChatDestroy(NUTCHAT * ci)
{
    uint8_t i;

    if (ci) {
        for (i = 0; i < ci->chat_aborts; i++)
            free(ci->chat_abort[i]);
        free(ci);
    }
}

/*!
 * \brief Chat processor.
 *
 * \return 0 on success, 1 in case of invalid parameters, 2 in case
 *         of an I/O error, 3 in case of a timeout error while waiting
 *         for an expected string, or the index of an abort string plus
 *         4, if one has been received. 
 */
static int NutChatProc(int fd, char *script)
{
    int rc = 0;
    char sendflg = 0;
    NUTCHAT *ci;
    char *arg;
    uint32_t to;
    uint32_t irto;
    uint32_t iwto;

    /*
     * Initialize the chat info structure.
     */
    if ((ci = NutChatCreate(fd)) == 0)
        return 2;

    /*
     * Save current and set default timeouts.
     */
    _ioctl(fd, UART_GETREADTIMEOUT, &irto);
    _ioctl(fd, UART_GETWRITETIMEOUT, &iwto);
    to = 45000;
    _ioctl(fd, UART_SETREADTIMEOUT, &to);
    to = 5000;
    _ioctl(fd, UART_SETWRITETIMEOUT, &to);

    /*
     * This loop splits up the chat string into arguments and 
     * alternating calls NutChatSend and NutChatExpect.
     */
    while (*script && rc == 0) {

        /*
         * Skip leading spaces.
         */
        if (*script == ' ' || *script == '\t' || *script == '\r' || *script == '\n') {
            script++;
            continue;
        }

        /*
         * Collect a quoted argument.
         */
        if (*script == '"' || *script == '\'') {
            char quote = *script++;

            arg = script;
            while (*script != quote) {
                if (*script == 0) {
                    rc = 1;
                    break;
                }
                if (*script++ == '\\') {
                    if (*script)
                        ++script;
                }
            }
        }

        /*
         * Collect an argument upto the next space.
         */
        else {
            arg = script;
            while (*script && *script != ' ' && *script != '\t' && *script != '\r' && *script != '\n')
                ++script;
        }

        if (*script)
            *script++ = 0;

        /*
         * Either send or expect the collected argument.
         */
        if (rc == 0) {
            if (sendflg)
                rc = NutChatSend(ci, arg);
            else
                rc = NutChatExpect(ci, arg);
            sendflg = !sendflg;
        }
    }

    /*
     * Restore initial timeout values.
     */
    _ioctl(fd, UART_SETREADTIMEOUT, &irto);
    _ioctl(fd, UART_SETWRITETIMEOUT, &iwto);

    /*
     * Release allocated memory.
     */
    NutChatDestroy(ci);

    return rc;
}

/*!
 * \brief Execute a conversational exchange with a serial device.
 *
 * Its primary purpose is to establish a modem connection.
 *
 * \param fd Descriptor of a previously opened device.
 * \param script Pointer to a string containing the chat script.
 *
 * \return 0 on success, 1 in case of invalid parameters, 2 in case
 *         of an I/O error, 3 in case of a timeout error while waiting
 *         for an expected string, or the index of an abort string plus
 *         4, if one has been received. 
 */
int NutChat(int fd, CONST char *script)
{
    int rc = -1;
    char *buf;

    /*
     * Work with a local copy of the chat string.
     */
    if ((buf = strdup(script)) != NULL) {
        rc = NutChatProc(fd, buf);
        free(buf);
    }
    return rc;
}

/*!
 * \brief Execute a conversational exchange with a serial device.
 *
 * Similar to NutChat() except that the chat string is located in
 * program memory.
 *
 * \return 0 on success, 1 in case of invalid parameters, 2 in case
 *         of an I/O error, 3 in case of a timeout error while waiting
 *         for an expected string, or the index of an abort string plus
 *         4, if one has been received. 
 */
#ifdef __HARVARD_ARCH__
int NutChat_P(int fd, PGM_P script)
{
    int rc = -1;
    char *buf;

    /*
     * Work with a local copy of the chat string.
     */
    if ((buf = malloc(strlen_P(script) + 1)) != 0) {
        strcpy_P(buf, script);
        rc = NutChatProc(fd, buf);
        free(buf);
    }
    return rc;
}
#endif
