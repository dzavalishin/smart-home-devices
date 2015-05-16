#ifndef _PRO_FTPD_H_
#define _PRO_FTPD_H_

/*
 * Copyright (C) 2004-2005 by egnite Software GmbH. All rights reserved.
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

/*!
 * \file pro/ftpd.h
 * \brief FTP protocol definitions for daemons.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.2  2008/08/11 07:00:24  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.1  2005/02/05 20:38:51  haraldkipp
 * First release
 *
 *
 * \endverbatim
 */

/*!
 * \addtogroup xgFTPD
 */
/*@{*/

/*!
 * \name FTP Server Configuration
 *
 * The Nut/OS Configurator may be used to override the default values.
 */
/*@{*/

/*!
 * \brief UDP port of DHCP server.
 *
 * \showinitializer
 */
#ifndef FTP_MAX_CMDBUF
#define FTP_MAX_CMDBUF  128
#endif

/*@}*/

/*!
 * \brief FTP session information structure.
 */
typedef struct {
    /*! \brief Telnet socket of this session. */
    TCPSOCKET *ftp_sock;
    /*! \brief Stream associated to the Telnet socket. */
    FILE *ftp_stream;
    /*! \brief Current working directory for this session. */
    char *ftp_cwd;
    /*! \brief Target IP for data transfer. */
    uint32_t ftp_data_ip;
    /*! \brief TCP port for data transfer. */
    uint16_t ftp_data_port;
    /*! \brief Maximum TCP segment size for data transfer. */
    uint16_t ftp_maxseg;
    /*! \brief Login status.
     *
     *  - 0: Not logged in.
     *  - 1: Got user's name, but no password.
     *  - 2: Logged in.
     */
    reg_t ftp_login;
    /*! \brief FTP data transfer mode.
     *
     * - 0: ASCII mode.
     * - 1: Binary mode.
     */
    reg_t ftp_tran_mode;
    /*! \brief FTP data transfer connection type.
     *
     * - 0: Active.
     * - 1: Passive.
     */
    reg_t ftp_passive;
    /*! \brief Source file for renaming.
     * NULL if no "RNFR" has been send or rename is finished.
     */
    char * ftp_renamesource;
} FTPSESSION;

/*@}*/


__BEGIN_DECLS
/* Function prototypes. */
extern int NutRegisterFtpRoot(CONST char *path);
extern int NutFtpServerSession(TCPSOCKET * sock);

__END_DECLS
/* */
#endif
