#ifndef PRO_SMTPC_H
#define PRO_SMTPC_H

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

/*!
 * \file pro/smtpc.h
 * \brief SMTP client functions.
 *
 * \verbatim
 * $Id$
 * \endverbatim
 */

#include <stdio.h>
#include <time.h>

#include <cfg/smtp.h>

/*!
 * \addtogroup xgSMTPC
 */
/*@{*/

#ifndef MAX_MAIL_RCPTS
/*! \brief Maximum number of recipients. */
#define MAX_MAIL_RCPTS  4
#endif

/*! \name Recipient Status Bits */
/*@{*/
/*! \brief Recipient type mask. */
#define MAIL_RCPT_TYPE  0x03
/*! \brief Standard recipient type. */
#define MAIL_RCPT_TO    0x01
/*! \brief Carbon copy recipient type. */
#define MAIL_RCPT_CC    0x02
/*! \brief Blind carbon copy recipient type. */
#define MAIL_RCPT_BCC   0x03
/*! \brief Recipient accepted flag. */
#define MAIL_RCPT_ACPT  0x20
/*! \brief Recipient processed flag. */
#define MAIL_RCPT_SENT  0x40
/*! \brief Recipient failed flag. */
#define MAIL_RCPT_FAIL  0x80
/*! \brief Recipient done mask. */
#define MAIL_RCPT_DONE  (MAIL_RCPT_SENT | MAIL_RCPT_FAIL)
/*@}*/

/*!
 * \brief Mail envelope structure type.
 */
typedef struct _MAILENVELOPE {
    /*! \brief Creation date and time. */
    time_t mail_date;
    /*! \brief Initial sender. */
    char *mail_from;
    /*! \brief Email's subject. */
    char *mail_subj;
    /*! \brief Email's text body. */
    char *mail_body;
    /*! \brief List of recipients. */
    char *mail_rcpt[MAX_MAIL_RCPTS];
    /*! \brief Recipients' status. */
    uint8_t mail_rcpt_stat[MAX_MAIL_RCPTS];
} MAILENVELOPE;

#ifndef SMTP_BUFSIZ
/*! \brief Size of the SMTP buffer. */
#define SMTP_BUFSIZ     256
#endif

/*! \name Server Features */
/*@{*/
/*! \brief Server doesn't support ESMTP. */
#define SMTPFEAT_VINTAGE        0x00000001
/*! \brief Server supports LOGIN authentication. */
#define SMTPFEAT_AUTH_LOGIN     0x00000002
/*! \brief Server supports PLAIN authentication. */
#define SMTPFEAT_AUTH_PLAIN     0x00000004
/*@}*/

/*!
 * \brief SMTP session structure type.
 */
typedef struct _SMTPCLIENTSESSION {
    /*! \brief Socket of this session. */
    TCPSOCKET *smtp_sock;
    /*! \brief Stream of this session. */
    FILE *smtp_stream;
    /*! \brief Server features. */
    uint32_t smtp_feat;
    /*! \brief SMTP buffer. */
    char smtp_buff[SMTP_BUFSIZ];
} SMTPCLIENTSESSION;

/*@}*/

__BEGIN_DECLS
/* Prototypes */

extern SMTPCLIENTSESSION * NutSmtpConnect(uint32_t ip, uint16_t port);
extern void NutSmtpDisconnect(SMTPCLIENTSESSION *si);
extern int NutSmtpLogin(SMTPCLIENTSESSION *si, char *host, char *user, char *pass);

extern CONST char *NutSmtpSendMail(SMTPCLIENTSESSION *si, MAILENVELOPE *me);
extern int NutSmtpSendMailRequest(SMTPCLIENTSESSION *si, MAILENVELOPE *me);
extern int NutSmtpSendMailHeader(SMTPCLIENTSESSION *si, MAILENVELOPE *me);
extern int NutSmtpSendEncodedLines(SMTPCLIENTSESSION *si, CONST char *text);

extern CONST char *NutSmtpSendCommand(SMTPCLIENTSESSION *si, CONST char *fmt, ...);
extern CONST char *NutSmtpReceiveResponse(SMTPCLIENTSESSION *si);

__END_DECLS
/* End of prototypes */
#endif
