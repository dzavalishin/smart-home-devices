/*
 * Copyright 2010 by egnite GmbH
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

/*
 * \file pro/smtpc.c
 * \brief Simple mail transfer protocol client.
 *
 * \verbatim
 * $Id$
 * \endverbatim
 */

#include <sys/nutdebug.h>
#include <sys/confnet.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <pro/rfctime.h>

#include <memdebug.h>
#include <string.h>

#include <pro/smtpc.h>

/*!
 * \addtogroup xgSMTPC
 */
/*@{*/

#ifndef SMTP_TIMEOUT
#define SMTP_TIMEOUT    600000
#endif

/*!
 * \brief BASE64 encoder.
 *
 * The encoder that is available in the gorp library can't be used for
 * sending SMTP authentication strings containing ASCII 0 characters.
 * This local implementation fixes this and takes a user supplied buffer
 * for the encoded string, avoiding costly heap allocation.
 *
 * In opposite to the original gorp routine, this function will not 
 * insert any line breaks. Note, that the encoder loop will not release 
 * the CPU. Thus, the caller should split large buffers into smaller 
 * chunks and add any required line breaks.
 *
 * \param sptr Pointer to the source buffer that contains the data to 
 *             encode.
 * \param slen Number of bytes available in the source buffer.
 * \param dptr Pointer to the destination buffer that receives the
 *             encoded data. A terminating zero byte will be appended.
 *             The caller must make sure, that the size of this buffer 
 *             is large enough to hold the resulting string. A save 
 *             calculation is (slen / 3) * 4 + 5.
 */
static void NutBase64Encode(CONST uint8_t * sptr, size_t slen, char *dptr)
{
    static CONST char base64set[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    uint_fast8_t i;
    uint32_t blk;
    uint_fast8_t pad = 0;

    /* Normally we do not check for NULL pointers. */
    NUTASSERT(sptr != NULL);
    NUTASSERT(dptr != NULL);

    while (slen) {
        blk = (uint32_t) (*sptr++) << 16;
        if (slen > 1) {
            blk |= (uint32_t) (*sptr++) << 8;
            if (slen > 2) {
                blk |= *sptr++;
            } else {
                pad = 1;
            }
        } else {
            pad = 2;
        }
        for (i = pad; i < 4; i++) {
            *dptr++ = base64set[(blk >> 18) & 63];
            blk <<= 6;
        }
        for (i = 0; i < pad; i++) {
            *dptr++ = '=';
        }
        slen -= 3 - pad;
    }
    *dptr = '\0';
}

/*!
 * \brief Read a response line from the server.
 *
 * \return Pointer to a buffer containing the response. In case of a
 *         broken connection or a line overflow, a NULL pointer is 
 *         returned.
 */
CONST char *NutSmtpReceiveResponse(SMTPCLIENTSESSION * si)
{
    char *cp;

    /* Normally we do not check for NULL pointers. */
    NUTASSERT(si != NULL);

    if (fgets(si->smtp_buff, sizeof(si->smtp_buff), si->smtp_stream)) {
        cp = strchr(si->smtp_buff, '\r');
        if (cp == NULL) {
            cp = strchr(si->smtp_buff, '\n');
        }
        if (cp) {
            *cp = '\0';
            return si->smtp_buff;
        }
        /* Line overflow. */
    }
    return NULL;
}

/*!
 * \brief Send command to the server and return the first response line.
 *
 * If a multi-line response is expected, the caller may use 
 * NutSmtpReceiveResponse() to receive additional response lines.
 *
 * \param fmt Format string containing conversion specifications like 
 *            printf.
 */
CONST char *NutSmtpSendCommand(SMTPCLIENTSESSION * si, CONST char *fmt, ...)
{
    va_list ap;

    /* Normally we do not check for NULL pointers. */
    NUTASSERT(si != NULL);
    NUTASSERT(fmt != NULL);

    va_start(ap, fmt);
    vfprintf(si->smtp_stream, (char *) fmt, ap);
    va_end(ap);
    fputs("\r\n", si->smtp_stream);
    fflush(si->smtp_stream);

    return NutSmtpReceiveResponse(si);
}

/*!
 * \brief Terminate an SMTP session.
 *
 * Gracefully closes the SMTP connection.
 * 
 * \param si Pointer to the \ref SMTPCLIENTSESSION structure, obtained
 *           from a previous call to NutSmtpConnect().
 */
void NutSmtpDisconnect(SMTPCLIENTSESSION * si)
{
    /* Normally we do not check for NULL pointers. */
    NUTASSERT(si != NULL);

    if (si->smtp_sock) {
        if (si->smtp_stream) {
            NutSmtpSendCommand(si, "QUIT");
            fclose(si->smtp_stream);
        }
        NutTcpCloseSocket(si->smtp_sock);
    }
    free(si);
}

/*!
 * \brief Start an SMTP session.
 *
 * \param ip   IP address of the host to connect.
 * \param port Port number to connect. Typically port 25 is used by SMTP.
 *
 * \return A pointer to a newly create \ref SMTPCLIENTSESSION structure,
 *         if the server is connected and ready to accept commands.
 *         Otherwise a NULL pointer is returned.
 */
SMTPCLIENTSESSION *NutSmtpConnect(uint32_t ip, uint16_t port)
{
    SMTPCLIENTSESSION *si;

    si = calloc(1, sizeof(SMTPCLIENTSESSION));
    if (si) {
        si->smtp_sock = NutTcpCreateSocket();
        if (si->smtp_sock && NutTcpConnect(si->smtp_sock, ip, port) == 0) {
            uint32_t tmo = SMTP_TIMEOUT;
            NutTcpSetSockOpt(si->smtp_sock, SO_RCVTIMEO, &tmo, sizeof(tmo));
            si->smtp_stream = _fdopen((int) ((intptr_t) si->smtp_sock), "r+b");
            if (si->smtp_stream && *NutSmtpReceiveResponse(si) == '2') {
                return si;
            }
        }
        NutSmtpDisconnect(si);
    }
    return NULL;
}

/*!
 * \brief Send EHLO/HELO command to the SMTP server.
 *
 * \param si   Pointer to the \ref SMTPCLIENTSESSION structure, obtained
 *             from a previous call to NutSmtpConnect().
 * \param cmd  Either EHLO or HELO.
 * \param host Primary host name, or NULL if the local host has no name.
 *
 * \return
 */
static CONST char *SayHello(SMTPCLIENTSESSION * si, char *cmd, char *host)
{
    if (host) {
        return NutSmtpSendCommand(si, "%s %s", cmd, host);
    }
    return NutSmtpSendCommand(si, "%s [%s]", cmd, inet_ntoa(confnet.cdn_ip_addr));
}

/*!
 * \brief Identify the client SMTP to the server.
 *
 * \param si   Pointer to the \ref SMTPCLIENTSESSION structure, obtained
 *             from a previous call to NutSmtpConnect().
 * \param host Primary host name, or NULL if the local host has no name.
 * \param user Login name. Set to NULL if authorization is not required.
 * \param pass Login password. Ignored if login name is set to NULL.
 *
 * \return 0 on success, -1 otherwise.
 */
int NutSmtpLogin(SMTPCLIENTSESSION * si, char *host, char *user, char *pass)
{
    CONST char *rsp;

    /* Normally we do not check for NULL pointers. */
    NUTASSERT(si != NULL);

    /* We start an SMTP session by issuing the EHLO command. */
    rsp = SayHello(si, "EHLO", host);
    if (rsp && *rsp == '5') {
        /* If EHLO is not supported, we fall back to HELO. */
        si->smtp_feat |= SMTPFEAT_VINTAGE;
        rsp = SayHello(si, "HELO", host);
    }
    if (rsp && *rsp == '2') {
        if ((si->smtp_feat & SMTPFEAT_VINTAGE) != SMTPFEAT_VINTAGE) {
            for (;;) {
                if (strncmp(rsp + 4, "AUTH ", 5) == 0) {
                    if (strstr(rsp + 9, "LOGIN")) {
                        si->smtp_feat |= SMTPFEAT_AUTH_LOGIN;
                    }
                    if (strstr(rsp + 9, "PLAIN")) {
                        si->smtp_feat |= SMTPFEAT_AUTH_PLAIN;
                    }
                }
                if (*(rsp + 3) != '-') {
                    break;
                }
                rsp = NutSmtpReceiveResponse(si);
                if (rsp == NULL) {
                    break;
                }
            }
        }
        if (user == NULL) {
            /* Return with success, if authorization not required. */
            return 0;
        }
        if (si->smtp_feat & SMTPFEAT_AUTH_PLAIN) {
            int lu = strlen(user);
            int lp = strlen(pass);
            uint8_t *auth = malloc(lu + lp + 3);

            *auth = '\0';
            memcpy(auth + 1, user, lu + 1);
            memcpy(auth + 1 + lu + 1, pass, lp);

            NutBase64Encode(auth, lu + lp + 2, si->smtp_buff);
            rsp = NutSmtpSendCommand(si, "AUTH PLAIN %s", si->smtp_buff);
            if (rsp && *rsp == '2') {
                return 0;
            }
        } else if (si->smtp_feat & SMTPFEAT_AUTH_LOGIN) {
        }
    }
    return -1;
}

/*!
 * \brief Send mail request.
 *
 * \param si Pointer to the \ref SMTPCLIENTSESSION structure, obtained
 *           from a previous call to NutSmtpConnect().
 * \param me Pointer to the \ref MAILENVELOPE structure.
 *
 * \return Number of recipients accepted by the server. If 0, no mail
 *         should be sent. In any case the caller should inspect the
 *         status of each recipient address in the mail envelope
 *         structure.
 */
int NutSmtpSendMailRequest(SMTPCLIENTSESSION * si, MAILENVELOPE * me)
{
    int rc = 0;
    CONST char *rsp;

    /* Normally we do not check for NULL pointers. */
    NUTASSERT(me != NULL);

    rsp = NutSmtpSendCommand(si, "MAIL FROM:%s", me->mail_from);
    if (rsp && *rsp == '2') {
        int i;

        for (i = 0; i < MAX_MAIL_RCPTS; i++) {
            /* Request all unprocessed recipients. */
            if (me->mail_rcpt[i] && (me->mail_rcpt_stat[i] & MAIL_RCPT_DONE) == 0) {
                rsp = NutSmtpSendCommand(si, "RCPT TO:%s", me->mail_rcpt[i]);
                if (rsp) {
                    if (*rsp == '2') {
                        /* Recipient accepted. */
                        me->mail_rcpt_stat[i] |= MAIL_RCPT_ACPT;
                        rc++;
                    } else {
                        /* Recipient not accepted. */
                        me->mail_rcpt_stat[i] &= ~MAIL_RCPT_ACPT;
                        if (*rsp == '5') {
                            /* Recipient refused. */
                            me->mail_rcpt_stat[i] |= MAIL_RCPT_FAIL;
                        }
                    }
                }
            }
        }
    }
    return rc;
}

/*!
 * \brief Send header line with all recipients of a specified type.
 *
 * \param stream Stream to send to.
 * \param me     Pointer to the \ref MAILENVELOPE structure.
 * \param type   Tpye of recipients, either MAIL_RCPT_TO or MAIL_RCPT_CC.
 */
static int SendMailHeaderRecipient(FILE *stream, MAILENVELOPE * me, uint8_t type)
{
    uint_fast8_t i;
    int cnt;

    /* Process the list of recipients given by this envelope. */
    for (i = 0, cnt = 0; i < MAX_MAIL_RCPTS; i++) {
        /* Check for the recipient's type. */
        if ((me->mail_rcpt_stat[i] & MAIL_RCPT_TYPE) == type) {
            if (cnt) {
                /* Additional recipient. */
                fputs(",\r\n    ", stream);
            } else {
                /* First one found. */
                fputs(type == MAIL_RCPT_TO ? "To: " : "CC: ", stream);
            }
            fputs(me->mail_rcpt[i], stream);
            cnt++;
        }
    }
    if (cnt) {
        fputs("\r\n", stream);
    }
    return 0;
}

/*!
 * \brief Send major mail header lines.
 *
 * This function will send the following header lines:
 *
 * - Date
 * - From
 * - Subject
 * - To
 * - CC
 *
 * The header information must be supplied by the caller in the mail
 * envelope structure.
 *
 * Note, that BCC recipients are not included.
 *
 * The caller may add additional headers using stdio functions with the 
 * stream handle available in the \ref SMTPCLIENTSESSION structure.
 * 
 * \param si Pointer to the \ref SMTPCLIENTSESSION structure, obtained
 *           from a previous call to NutSmtpConnect().
 * \param me Pointer to the \ref MAILENVELOPE structure.
 *
 * \return Always 0 right now. Later versions may return -1 in case of
 *         an error.
 */
int NutSmtpSendMailHeader(SMTPCLIENTSESSION * si, MAILENVELOPE * me)
{
    /* Normally we do not check for NULL pointers. */
    NUTASSERT(si != NULL);
    NUTASSERT(si->smtp_stream != NULL);
    NUTASSERT(me != NULL);

    if (me->mail_date) {
        fprintf(si->smtp_stream, "Date: %s\r\n", Rfc1123TimeString(gmtime(&me->mail_date)));
    }
    fprintf(si->smtp_stream, "From: %s\r\n", me->mail_from);
    fprintf(si->smtp_stream, "Subject: %s\r\n", me->mail_subj);
    SendMailHeaderRecipient(si->smtp_stream, me, MAIL_RCPT_TO);
    SendMailHeaderRecipient(si->smtp_stream, me, MAIL_RCPT_CC);

    return 0;
}

/*!
 * \brief Send encoded email text lines.
 *
 * Each line will be terminated by a carriage return / linefeed pair.
 * Lines larger than \ref SMTP_BUFSIZ are split. This may occure in
 * the middle of a word. A dot will be put in front of lines that start
 * with a dot.
 *
 * \param si   Pointer to the \ref SMTPCLIENTSESSION structure, obtained
 *             from a previous call to NutSmtpConnect().
 * \param text Pointer to a string containing the text lines to send.
 *             Lines should be separated by linefeeds and may be
 *             optionally prepended by a carriage return. If the pointer
 *             points to an empty string, nothing will be sent.
 *
 * \return 0 on success, -1 otherwise. An error typically indicates a
 *         broken connection.
 */
int NutSmtpSendEncodedLines(SMTPCLIENTSESSION * si, CONST char *text)
{
    /* Sanity checks. */
    NUTASSERT(si != NULL);
    NUTASSERT(text != NULL);

    while (*text) {
        char *bufp = si->smtp_buff;
        int i;

        /* Collect a line or a complete buffer. */
        for (i = 0; *text && i < SMTP_BUFSIZ; text++) {
            /* Ignore carriage returns. */
            if (*text != '\r') {
                /* Stop at linefeeds or at the end of the message. */
                if (*text == '\n' || *text == '\0') {
                    break;
                } else {
                    /* Send this character unchanged. */
                    *bufp++ = *text;
                    i++;
                }
            }
        }
        if (i) {
            *bufp = '\0';
            /* Insert a dot in front of any line that starts with a dot. */
            if (si->smtp_buff[0] == '.') {
                fputc('.', si->smtp_stream);
            }
            /* Send the buffer with CR/LF appended. */
            fputs(si->smtp_buff, si->smtp_stream);
            if (fputs("\r\n", si->smtp_stream) == EOF) {
                /* Connection broken. */
                return -1;
            }
        }
    }
    return 0;
}

/*!
 * \brief Send an email via an active SMTP session.
 *
 * Applications may use the following basic sequence to send an email:
 *
 * \code
 * #include <pro/smtpc.h>
 *
 * MAILENVELOPE email = {
 *     0, "<me@ethernut.de>", "Problem",
 *     "Be warned.",
 *     { "<admin@ethernut.de>", NULL, NULL, NULL },
 *     { MAIL_RCPT_TO, 0, 0, 0 }
 * };
 * SMTPCLIENTSESSION *smtp;
 *
 * smtp = NutSmtpConnect(daemon_ip, 25);
 * NutSmtpLogin(smtp, NULL, NULL, NULL);
 * NutSmtpSendMail(smtp, &email);
 * NutSmtpDisconnect(smtp);
 * \endcode
 *
 * More advanced mail transfers may be implemented by using the
 * other routines of this API for sending commands or parts of 
 * an email individually. Even the stream in the session structure
 * may be used with stdio calls.
 *
 * \param si Pointer to the \ref SMTPCLIENTSESSION structure, obtained
 *           from a previous call to NutSmtpConnect().
 * \param me Pointer to the \ref MAILENVELOPE structure. On return,
 *           the caller should inspect the status of each recipient.
 *
 * \return Pointer to a buffer containing the last response. The status
 *         of the recipients in the envelope will have been updated. In 
 *         case of a fatal error or if all recipients had been rejected, 
 *         a NULL pointer is returned.
 */
CONST char *NutSmtpSendMail(SMTPCLIENTSESSION * si, MAILENVELOPE * me)
{
    CONST char *rsp = NULL;

    /* Normally we do not check for NULL pointers. */
    NUTASSERT(si != NULL);
    NUTASSERT(me != NULL);

    /* Send a request for sending mail. The function returns the number
       of accepted recipients. */
    if (NutSmtpSendMailRequest(si, me) > 0) {
        /* Start sending the email. */
        rsp = NutSmtpSendCommand(si, "DATA");
        /* Server wants us to go ahead? */
        if (rsp && *rsp == '3') {
            /* Send the major header lines. */
            NutSmtpSendMailHeader(si, me);
            fputs("\r\n", si->smtp_stream);
            /* If available, send the mail body's text. */
            if (me->mail_body) {
                NutSmtpSendEncodedLines(si, me->mail_body);
            }
            fputs(".\r\n", si->smtp_stream);
            fflush(si->smtp_stream);
            /* Check the result of the data transfer. */
            rsp = NutSmtpReceiveResponse(si);
            /* On success, update the status of the previously accepted
               recipients. */
            if (rsp && *rsp == '2') {
                uint_fast8_t i;
                for (i = 0; i < MAX_MAIL_RCPTS; i++) {
                    if (me->mail_rcpt[i]) {
                        if ((me->mail_rcpt_stat[i] & MAIL_RCPT_ACPT) == MAIL_RCPT_ACPT) {
                            me->mail_rcpt_stat[i] |= MAIL_RCPT_SENT;
                        }
                    }
                }
            }
        }
    }
    return rsp;
}

/*@}*/
