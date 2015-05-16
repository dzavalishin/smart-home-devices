/*
 * Copyright (C) 2001-2005 by egnite Software GmbH. All rights reserved.
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
 */

/*!
 * \file pro/ftpd.c
 * \brief FTP server.
 *
 * \verbatim
 *
 * By Malte Marwedel:
 *  - Adding move command (RNFR, RNTO)
 *  - (AVR) Moving strings from RAM to flash
 *  - Add support for list with -la option
 *  - Checks if file is a real file before accepting a GET
 *  - Fixing GET if network is busy
 *  - Fixing "cd foo", "cd ../bar" bug (path should be "/bar" was "//bar")
 *  - Note: Passive mode is not implemented properly, try to use active instead
 *
 * $Log$
 * Revision 1.12  2009/02/13 14:52:05  haraldkipp
 * Include memdebug.h for heap management debugging support.
 *
 * Revision 1.11  2009/02/06 15:40:29  haraldkipp
 * Using newly available strdup() and calloc().
 * Replaced NutHeap routines by standard malloc/free.
 * Replaced pointer value 0 by NULL.
 *
 * Revision 1.10  2008/08/11 07:00:35  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.9  2008/04/01 10:16:35  haraldkipp
 * Added S_ISDIR and S_ISREG macro.
 *
 * Revision 1.8  2006/10/08 16:48:22  haraldkipp
 * Documentation fixed
 *
 * Revision 1.7  2006/03/16 15:25:38  haraldkipp
 * Changed human readable strings from u_char to char to stop GCC 4 from
 * nagging about signedness.
 *
 * Revision 1.6  2006/02/23 15:48:08  haraldkipp
 * Added missing file truncation flag for overriding existing files.
 *
 * Revision 1.5  2006/01/25 18:47:42  haraldkipp
 * Fixes wrong implementation of readdir() and simplifies the code.
 * Thanks to Jesper Hansen.
 *
 * Revision 1.4  2006/01/22 17:45:29  haraldkipp
 * CreateFullPathName() partly returned double slashes or empty paths.
 *
 * Revision 1.3  2005/08/02 17:47:04  haraldkipp
 * Major API documentation update.
 *
 * Revision 1.2  2005/02/07 18:57:51  haraldkipp
 * ICCAVR compile errors fixed
 *
 * Revision 1.1  2005/02/05 20:45:38  haraldkipp
 * First release.
 *
 *
 * \endverbatim
 */

//#define FTPD_DEBUG

#include <sys/version.h>

#include <sys/socket.h>
#include <netinet/tcp.h>
#include <sys/timer.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <memdebug.h>

#include <pro/ftpd.h>

#ifdef FTPD_DEBUG
#include <sys/heap.h>
#endif

/*!
 * \addtogroup xgFTPD
 *
 * Internet File Transfer Protocol Server.
 */
/*@{*/

/*!
 * \name FTP Server Configuration
 *
 * The Nut/OS Configurator may be used to override the default values.
 */
/*@{*/

/*!
 * \brief Default FTP root path.
  *
 * \showinitializer
*/
#ifndef FTP_ROOTPATH
#define FTP_ROOTPATH "PNUT:"
#endif

/*!
 * \brief Default data port.
 *
 * \showinitializer
 */
#ifndef FTP_DATA_PORT
#define FTP_DATA_PORT   20
#endif

/*@}*/

static char *ftp_root;
static char *ftp_user;
static char *ftp_pass;

/*
 * On Harvard architectures constant strings are stored in ROM,
 * because RAM is usually a scarce resource on these platforms.
 */
static prog_char cmd_cwd_P[] = "CWD";
static prog_char cmd_dele_P[] = "DELE";
static prog_char cmd_list_P[] = "LIST";
static prog_char cmd_mkd_P[] = "MKD";
static prog_char cmd_xmkd_P[] = "XMKD";
static prog_char cmd_nlst_P[] = "NLST";
static prog_char cmd_noop_P[] = "NOOP";
static prog_char cmd_pass_P[] = "PASS";
static prog_char cmd_pasv_P[] = "PASV";
static prog_char cmd_port_P[] = "PORT";
static prog_char cmd_pwd_P[] = "PWD";
static prog_char cmd_xpwd_P[] = "XPWD";
static prog_char cmd_quit_P[] = "QUIT";
static prog_char cmd_retr_P[] = "RETR";
static prog_char cmd_rmd_P[] = "RMD";
static prog_char cmd_xrmd_P[] = "XRMD";
static prog_char cmd_stor_P[] = "STOR";
static prog_char cmd_syst_P[] = "SYST";
static prog_char cmd_type_P[] = "TYPE";
static prog_char cmd_user_P[] = "USER";
static prog_char cmd_rename1_P[] = "RNFR";
static prog_char cmd_rename2_P[] = "RNTO";


static char *mon_name = "JanFebMarAprMayJunJulAugSepOctNovDec";

static prog_char rep_banner[] = "220 Nut/OS FTP %s ready at %.3s%3d %02d:%02d:%02d\r\n";

/*!
 * \brief Break a string into a command and an argument string.
 *
 * \param line Pointer to the buffer containing the string. Note, that
 *             its contents is not preserved.
 * \param cmd  Receives a pointer to the command word in the given
 *             string. The routine will convert all lower case letters
 *             into upper case.
 * \param args Receives a pointer to the list of arguments, if any.
 *             Trailing carriage return or linefeed characters are
 *             cut off.
 */
static void SplitCmdArg(char * line, char ** cmd, char ** args)
{
    /* Skip leading spaces. */
    while (*line && *line <= ' ') {
        line++;
    }

    /* The first word is the command. Convert it to upper case. */
    *cmd = line;
    while (*line > ' ') {
        if (*line >= (uint8_t) 'a' && *line <= (uint8_t) 'z') {
            *line -= (uint8_t) 'a' - 'A';
        }
        line++;
    }

    /* Mark end of the command word. */
    if (*line) {
        *line++ = '\0';
    }

    /* Skip spaces. */
    while (*line && *line <= ' ') {
        ++line;
    }

    /* Arguments start here. */
    *args = line;
    while (*line && *line != '\r' && *line != '\n') {
        line++;
    }

    /* Mark end of arguments. */
    *line = 0;
}

/*!
 * \brief Parses arguments of an FTP PORT command.
 *
 * FTP clients send a target port definition as a comma separated list
 * of 6 ASCII byte values. The first four bytes specify an IP address
 * and the remaining two a port number.
 *
 * \param arg  Pointer to the argument string.
 * \param ip   Pointer to an unsigned long, which receives the IP
 *             address in network byte order.
 * \param port Pointer to an unsigned short, which receives the port
 *             number in host byte order.
 *
 * \return The number of converted byte values. Should be 6.
 */
static int ParseIpPort(CONST char * arg, uint32_t * ip, uint16_t * port)
{
    int rc;

    *ip = 0;
    *port = 0;
    for (rc = 0; rc < 6; rc++) {
        if (*arg < '0' || *arg > '9') {
            break;
        }
        if (rc < 4) {
            *ip >>= 8;
            *ip += atol(arg) << 24;
        } else {
            *port <<= 8;
            *port += atoi(arg);
        }
        while (*arg && *arg != ',') {
            arg++;
        }
        if (*arg == ',') {
            arg++;
        }
    }
    return rc;
}

/*!
 * \brief Send a positive response.
 *
 * \param session Pointer to an \ref FTPSESSION structure, obtained by a
 *                previous call to NutFtpOpenSession().
 * \param code    Response code.
 *
 * \return -1 if the session ended. Otherwise 0 is returned.
 */
int NutFtpRespondOk(FTPSESSION * session, int code)
{
    static prog_char fmt_P[] = "%d OK\r\n";

#ifdef FTPD_DEBUG
    printf("\n<'%d OK' ", code);
#endif
    fprintf_P(session->ftp_stream, fmt_P, code);
    fflush(session->ftp_stream);

    return 0;
}

/*!
 * \brief Send a negative response.
 *
 * \param session Pointer to an \ref FTPSESSION structure, obtained by a
 *                previous call to NutFtpOpenSession().
 * \param code    Response code.
 *
 * \return -1 if the session ended. Otherwise 0 is returned.
 */
int NutFtpRespondBad(FTPSESSION * session, int code)
{
    static prog_char fmt_P[] = "%d Failed\r\n";

#ifdef FTPD_DEBUG
    printf("\n<'%d Failed' ", code);
#endif
    fprintf_P(session->ftp_stream, fmt_P, code);
    fflush(session->ftp_stream);

    return 0;
}

/*!
 * \brief Send a response including the specified transfer mode.
 *
 * \param session Pointer to an \ref FTPSESSION structure, obtained by a
 *                previous call to NutFtpOpenSession().
 * \param binary  0 indicates ASCII transfer mode.
 *
 * \return -1 if the session ended. Otherwise 0 is returned.
 */
int NutFtpSendMode(FTPSESSION * session, int binary)
{
    static prog_char intro_P[] = "150 Opening ";
    static prog_char amode_P[] = "ASCII.\r\n";
    static prog_char bmode_P[] = "BINARY.\r\n";

#ifdef FTPD_DEBUG
    printf("\n<'150 Opening %s' ", binary ? "BINARY" : "ASCII");
#endif
    fputs_P(intro_P, session->ftp_stream);
    fputs_P(binary ? bmode_P : amode_P, session->ftp_stream);
    fflush(session->ftp_stream);

    return 0;
}

/*!
 * \brief Create an absolute path name.
 *
 * Combines an absolute directory path with a relative path name to a
 * full absolute path name. The absolute directory is split into two
 * parts, the root and the current work directory. The resulting path
 * will never be above the specified root.
 *
 * \param root Non-empty absolute root directory path including the
 *             device name, but without a trailing slash.
 * \param work Absolute work directory path below root including a
 *             leading, but without a trailing slash. This path is
 *             ignored if the relative path starts with a slash.
 * \param path Relative path name of a file or directory without any
 *             trailing slash.
 *
 * \return Pointer to the resulting path name. The buffer for this name
 *         is allocated from heap memory. The caller is responsible for
 *         freeing it. In case of an error, a NULL pointer is returned.
 */
char *CreateFullPathName(char *root, char *work, char *path)
{
    char *full;
    char *cp;
    size_t rl = root ? strlen(root) : 0;
    size_t wl = work ? strlen(work) : 0;
    size_t pl = path ? strlen(path) : 0;

    /* Ignore trailing slashes in root and work. */
    if (rl && *(root + rl - 1) == '/') {
        rl--;
    }
    if (wl && *(work + wl - 1) == '/') {
        wl--;
    }

    if ((full = malloc(rl + wl + pl + 3)) != NULL) {
        /* Put the root in front. */
        cp = full;
        if (rl) {
            cp = strcpy(full, root) + rl;
        }

        /* If path is relative, prepend the working directory. */
        if(pl == 0 || *path != '/') {
            if (wl) {
                if (*work != '/') {
                    *cp++ = '/';
                }
                cp = strcpy(cp, work) + wl;
            }
            *cp++ = '/';
            rl++;
        }

        if (pl) {
            *cp = 0;
            work = full + rl;

            while (*path) {
                /* Ingore duplicate slashes. */
                if (*path == '/') {
                    /* No double slash in target if we are at the root */
                    if ((cp == full) || (*(cp-1) != '/')) {
                        *cp++ = *path++;
                    }
                    while (*path == '/') {
                        path++;
                    }
                }
                /* Ignore single dots. */
                if (*path == '.') {
                    path++;
                    if (*path == '/') {
                        path++;
                        continue;
                    }
                    if (*path == 0) {
                        break;
                    }
                    if (*path == '.') {
                        path++;
                        if (*path == '/' || *path == 0) {
                            if (cp != work) {
                                cp--;
                                while (cp != work) {
                                    cp--;
                                    if (*cp == '/') {
                                        break;
                                    }
                                }
                            }
                            continue;
                        }
                        path--;
                    }
                    path--;
                }
                /* Copy the current path component. */
                while (*path && *path != '/') {
                    *cp++ = *path++;
                }
            }
        }
        *cp = 0;
    }
    return full;
}

/*!
 * \brief Establish an FTP connection for data transfer.
 *
 * \param session Pointer to an \ref FTPSESSION structure, obtained by a
 *                previous call to NutFtpOpenSession().
 *
 * \return Socket descriptor of the newly created data connection or
 *         0 if we were unable to establish the connection.
 */
TCPSOCKET *NutFtpDataConnect(FTPSESSION * session)
{
#ifdef FTPD_DEBUG
    static prog_char errorcode_P[] = "errorcode of active socket: %i\n";
    static prog_char socfailed_P[] = "Create socket failed";
#endif
    TCPSOCKET *sock;
    int rc;

    if ((sock = NutTcpCreateSocket()) != 0) {

        if (session->ftp_maxseg) {
            NutTcpSetSockOpt(sock, TCP_MAXSEG, &session->ftp_maxseg, sizeof(session->ftp_maxseg));
        }
        if (session->ftp_passive) {
            rc = NutTcpAccept(sock, session->ftp_data_port);
        } else {
            rc = NutTcpConnect(sock, session->ftp_data_ip, session->ftp_data_port);
#ifdef FTPD_DEBUG
            if (rc) {
              int errorcode = NutTcpError(sock);
              printf_P(errorcode_P, errorcode);
            }
#endif
        }
        if (rc) {
            NutTcpCloseSocket(sock);
            sock = 0;
        }
    }
#ifdef FTPD_DEBUG
    else {
        puts_P(socfailed_P);
    }
#endif
    return sock;
}

/*!
 * \brief Register the FTP server's root directory.
 *
 * Only one root directory is supported. Subsequent calls will
 * override previous settings.
 *
 * \param path Path name of the root directory. Must include the
 *             device name followed by a colon followed by an
 *             absolute directory path. May be set to NULL for
 *             the default #FTP_ROOTPATH.
 *
 * \return 0 on success, -1 otherwise.
 */
int NutRegisterFtpRoot(CONST char *path)
{
    /* Reset path to default. */
    if (path == NULL || *path == 0) {
        /* Release previously allocate space. */
        if (ftp_root) {
            free(ftp_root);
        }
        if ((ftp_root = malloc(sizeof(FTP_ROOTPATH))) == 0) {
            return -1;
        }
        strcpy(ftp_root, FTP_ROOTPATH);
    }

    /* Set a specified path. */
    else {
        char *cp = strchr(path, ':');
        int len = strlen(path);

        /* Make sure that the path fulfills all requirements. */
        if (len < 2 || cp == 0 || (*++cp && *cp != '/')) {
            return -1;
        }

        /* Allocate space for new path, but preserve the current one. */
        if ((cp = malloc(len + 1)) == 0) {
            return -1;
        }

        /* Take over, releasing previously allocate space. */
        strcpy(cp, path);
        if (ftp_root) {
            free(ftp_root);
        }
        ftp_root = cp;

        /* Chop off an optional trailing slash. */
        cp = cp + strlen(cp) - 1;
        if (*cp == '/') {
            *cp = 0;
        }
    }
    return 0;
}

/*!
 * \brief Register an FTP user.
 *
 * Only one username/password pair is supported. Subsequent calls will
 * override previous settings.
 *
 * \warning Not successfully registering any will make file systems
 *          accessible by anyone.
 *
 * \param user User's name.
 * \param pass Uncrypted password for the specified user.
 *
 * \return 0 on success. -1 is returned on failures, in which case
 *         no protection may be assumed.
 */
int NutRegisterFtpUser(CONST char *user, CONST char *pass)
{
    if (ftp_user) {
        free(ftp_user);
        ftp_user = NULL;
    }
    if (user && *user) {
        if ((ftp_user = strdup(user)) == NULL) {
            return -1;
        }
    }
    if (ftp_pass) {
        free(ftp_pass);
        ftp_pass = NULL;
    }
    if (pass && *pass) {
        if ((ftp_pass = strdup(pass)) == NULL) {
            return -1;
        }
    }
    return 0;
}


/*!
 * \brief Open an FTP server session.
 *
 * \param sock Socket of an established TCP connection.
 *
 * \return Pointer to an \ref FTPSESSION structure, which can be used
 *         in subsequent API calls. NULL is returned in case of any
 *         error.
 */
FTPSESSION *NutFtpOpenSession(TCPSOCKET * sock)
{
    FTPSESSION *session;

    session = malloc(sizeof(FTPSESSION));

    if (session) {
        memset(session, 0, sizeof(FTPSESSION));
        session->ftp_data_port = FTP_DATA_PORT;
        session->ftp_maxseg = sock->so_mss;
        session->ftp_sock = sock;

        /* Set initial working directory. */
        if ((session->ftp_cwd = malloc(2)) == 0) {
            free(session);
            session = 0;
        } else {
            session->ftp_cwd[0] = '/';
            session->ftp_cwd[1] = 0;

            /*
             * Open a stream and associate it with the socket, so
             * we can use standard I/O. Note, that socket streams
             * currently do support text mode.
             */
            if ((session->ftp_stream = _fdopen((int) sock, "r+b")) == 0) {
                free(session->ftp_cwd);
                free(session);
                session = 0;
            }
        }
    }
    return session;
}

/*!
 * \brief Close an FTP server session.
 *
 * \param session Pointer to an \ref FTPSESSION structure, obtained by a
 *                previous call to NutFtpOpenSession().
 */
void NutFtpCloseSession(FTPSESSION * session)
{
    if (session) {
        /* Close the stream. */
        fclose(session->ftp_stream);
        if (session->ftp_cwd) {
            free(session->ftp_cwd);
        }
        if (session->ftp_renamesource) {
            free(session->ftp_renamesource);
        }
        free(session);
    }
}

/*!
 * \brief Process an FTP client's CWD command.
 *
 * \param session Pointer to an \ref FTPSESSION structure, obtained by a
 *                previous call to NutFtpOpenSession().
 * \param path    Full absolute path name of the directory.
 *
 * \return 0 if the command has been processed or -1 if the
 *         session has been aborted.
 */
int NutFtpProcessCwd(FTPSESSION * session, char *path)
{
    struct stat st;
    char *cp = path + strlen(ftp_root);

    if (*cp && strcmp(cp, "/")) {
        /*
         * Check, if the path exists and if this is a directory.
         */
        if (stat(path, &st) || !S_ISDIR(st.st_mode)) {
            return NutFtpRespondBad(session, 550);
        }
    }

    /*
     * Store the new working directory excluding our root part.
     */
    if (*cp == 0) {
        cp = "/";
    }
    if (session->ftp_cwd) {
        free(session->ftp_cwd);
    }
    if ((session->ftp_cwd = strdup(cp)) == NULL) {
        return NutFtpRespondBad(session, 550);
    }
    return NutFtpRespondOk(session, 250);
}

/*!
 * \brief Process an FTP client's DELE command.
 *
 * Causes the specified file to be deleted.
 *
 * \param session Pointer to an \ref FTPSESSION structure, obtained by a
 *                previous call to NutFtpOpenSession().
 * \param path    Full absolute path name of the file.
 *
 * \return 0 if the command has been processed or -1 if the
 *         session has been aborted.
 */
int NutFtpProcessDelete(FTPSESSION * session, char *path)
{
    if (unlink(path)) {
        return NutFtpRespondBad(session, 550);
    }
    return NutFtpRespondOk(session, 250);
}

/*!
 * \brief Transfer a file to or from the FTP client.
 *
 * \param session Pointer to an \ref FTPSESSION structure, obtained by a
 *                previous call to NutFtpOpenSession().
 * \param path    Full absolute path name of the file.
 * \param mode    If set to zero, the server will accept the data
 *                transferred via the data connection and to store the
 *                data as a file. If the file exists, then its contents
 *                will be replaced by the data being transferred. A new
 *                file is created, if the file does not already exist.
 *                If this parameter is not equal zero, then the server
 *                will transfer a copy of the specified file.
 *
 * \return 0 if the command has been processed or -1 if the
 *         session has been aborted.
 */
int NutFtpTransferFile(FTPSESSION * session, char *path, int mode)
{
#ifdef FTPD_DEBUG
    static prog_char dataconnectfailed_P[] = "NutFtpDataConnect failed";
#endif
    TCPSOCKET *sock;
    int ec = 550;
    int fh = -1;
    /* Open the file to send. */
    if (mode) {
        /* Some clients open file as directorys. Giving them a direct failure */
        struct stat st;
        if (stat(path, &st) == 0) {
            if (S_ISREG(st.st_mode)) {
                fh = _open(path, _O_BINARY | _O_RDONLY);
            } else {
#ifdef FTPD_DEBUG
               printf("Not a regular file\n");
#endif
            }
        } else {
#ifdef FTPD_DEBUG
            printf("'%s' stat() failed\n", path);
#endif
        }
    }
    /* Open the file to receive. */
    else {
        fh = _open(path, _O_CREAT | _O_TRUNC);
    }
    if (fh != -1) {
        /* File status OK, opening data connection */
        NutFtpSendMode(session, session->ftp_tran_mode);
        if ((sock = NutFtpDataConnect(session)) != 0) {
            uint16_t mss = sock->so_mss;
            uint8_t *buf;

            if (mss < 256) {
                mss = 256;
            }
            if ((buf = malloc(mss)) != 0) {
                int got;

                ec = 0;

                /* Send a file. */
                if (mode) {
                    while ((got = _read(fh, buf, mss)) > 0) {
                        int send = 0;
                        int retcode;
                        uint8_t timeout = 0;
                        /* Mentioned in inetq.c, NutTcpSend does not gurantee
                           to send all bytes, so we use a loop and do a timeout if
                           no byte has been send within two seconds. */
                        do {
                            retcode = NutTcpSend(sock, buf+send, got-send);
                            send += retcode; //if < 0, we will leave the loop anyway
                            if (send == 0) {
                                timeout++;
                                NutSleep(10);
                            } else {
                                timeout = 0;
                            }
                        } while ((send < got ) && (retcode >= 0) && (timeout < 200));
                        if (retcode <= 0) {
#ifdef FTPD_DEBUG
                            printf("Error in NutTcpSend\n");
#endif
                            ec = 551;
                            break;
                        }
                    }
                }

                /* Receive a file. */
                else {
                    while ((got = NutTcpReceive(sock, buf, mss)) > 0) {
                        int x;
                        if ((x = _write(fh, buf, got)) != got) {
                            ec = 552;
                            break;
                        }
                    }
                }
                free(buf);
            }
            NutTcpCloseSocket(sock);
        }
#ifdef FTPD_DEBUG
          else {
          puts_P(dataconnectfailed_P);
        }
#endif
        _close(fh);

        /* Remove files received with an error. */
        if (mode == 0 && ec) {
            unlink(path);
        }
    }
    if (ec) {
        return NutFtpRespondBad(session, ec);
    }
    return NutFtpRespondOk(session, 226);
}

/*!
 * \brief Process an FTP client's LIST or NLST command.
 *
 * Causes a directory listing to be sent to the client.
 *
 * \param session Pointer to an \ref FTPSESSION structure, obtained by a
 *                previous call to NutFtpOpenSession().
 * \param path    Full absolute path name of the directory.
 *
 * \param options Set to 1 to incude filenames beginning with a "." (unix hidden files)
 * \return 0 if the command has been processed or -1 if the
 *         session has been aborted.
 */
int NutFtpTransferDirectoryOptions(FTPSESSION * session, char *path, int options)
{
    static prog_char fileattributes_P[] = "rw-rw-rw-  1 0 0 %6lu ";
    static prog_char dateattribute_P[] = "%.3s %u ";
    static prog_char timeattribute_P[] = "%02u:%02u ";
    TCPSOCKET *sock;
    FILE *fp;

    struct stat st;
    DIR *dir;
    struct dirent *d_ent;
    tm *gmt;
    uint32_t size;
    int ec = 550;
    char *name;
    size_t pathlen;

    dir = opendir(path);
    if (dir) {
        NutFtpSendMode(session, 0);
        if ((sock = NutFtpDataConnect(session)) != 0) {
            if ((fp = _fdopen((int) sock, "r+b")) != 0) {
                ec = 0;
                pathlen = strlen(path);
                while ((d_ent = readdir(dir)) != 0) {
                    if ((d_ent->d_name[0] == '.')  && ((options & 1) == 0)) {
                        continue;
                    }
                    if ((name = malloc(pathlen + strlen(d_ent->d_name) + 2)) != 0) {
                        strcpy(name, path);
                        if (pathlen == 0 || name[pathlen - 1] != '/') {
                            strcat(name, "/");
                        }
                        strcat(name, d_ent->d_name);
                        if (stat(name, &st) == 0) {
                            if (S_ISDIR(st.st_mode)) {
                                fputc('d', fp);
                                size = 0;
                            } else {
                                fputc('-', fp);
                                size = st.st_size;
                            }
                            fprintf_P(fp, fileattributes_P, size);
                            gmt = gmtime(&st.st_mtime);
                            //fprintf(fp, "%s %u %u ", mon_name[gmt->tm_mon], gmt->tm_mday, 1900 + gmt->tm_year);
                            fprintf_P(fp, dateattribute_P, mon_name + gmt->tm_mon * 3, gmt->tm_mday);
                            //fprintf(fp, "%02u:%02u:%02u ", gmt->tm_hour, gmt->tm_min, gmt->tm_sec);
                            fprintf_P(fp, timeattribute_P, gmt->tm_hour, gmt->tm_min);
                            fputs(d_ent->d_name, fp);
                            fputs("\r\n", fp);
                        }
                        free(name);
                    }
                }
                fclose(fp);
            }
            NutTcpCloseSocket(sock);
        }
        closedir(dir);
    }
    if (ec) {
        return NutFtpRespondBad(session, ec);
    }
    return NutFtpRespondOk(session, 226);
}

/*!
 * \brief Process an FTP client's LIST or NLST command.
 *
 * Causes a directory listing to be sent to the client.
 *
 * \param session Pointer to an \ref FTPSESSION structure, obtained by a
 *                previous call to NutFtpOpenSession().
 * \param path    Full absolute path name of the directory.
 *
 * \return 0 if the command has been processed or -1 if the
 *         session has been aborted.
 */
int NutFtpTransferDirectory(FTPSESSION * session, char *path)
{
    return NutFtpTransferDirectoryOptions(session, path, 0);
}

/*!
 * \brief Process an FTP client's MKD command.
 *
 * Causes the specified directory to be created.
 *
 * \param session Pointer to an \ref FTPSESSION structure, obtained by a
 *                previous call to NutFtpOpenSession().
 * \param path    Full absolute path name of the directory.
 *
 * \return 0 if the command has been processed or -1 if the
 *         session has been aborted.
 */
int NutFtpProcessMkd(FTPSESSION * session, char *path)
{
    if (mkdir(path, 0777)) {
        return NutFtpRespondBad(session, 550);
    }
    return NutFtpRespondOk(session, 257);
}


int NutFtpRenamePrepare(FTPSESSION * session, char *path)
{
    if (session->ftp_renamesource) {
        free(session->ftp_renamesource);
        session->ftp_renamesource = NULL;
    }
    if (path) {
        struct stat st;
        if (stat(path, &st) == 0) {
            session->ftp_renamesource = strdup(path);
            if (session->ftp_renamesource) {
                return NutFtpRespondOk(session, 350);
            }
        } else {
          return NutFtpRespondBad(session, 450);
        }
    }
    return NutFtpRespondBad(session, 501);
}

int NutFtpRenameAction(FTPSESSION * session, char *path)
{
    uint16_t responsebad;
    if (session->ftp_renamesource) {
         if (path) {
             int res = rename(session->ftp_renamesource, path);
             free(session->ftp_renamesource);
             session->ftp_renamesource = NULL;
             if (res == 0) {
                 return NutFtpRespondOk(session, 250);
             } else {
                 responsebad = 550;
             }
         } else {
             responsebad = 501;
         }
    } else {
         responsebad = 503;
    }
    return NutFtpRespondBad(session, responsebad);
}


/*!
 * \brief Process an FTP client's PASS command.
 *
 * Only one login per session is accepted.
 *
 * \param session Pointer to an \ref FTPSESSION structure, obtained by a
 *                previous call to NutFtpOpenSession().
 * \param pass    Password.
 *
 * \return 0 if the command has been processed or -1 if the
 *         session has been aborted.
 */
int NutFtpProcessPass(FTPSESSION * session, char *pass)
{
    if (ftp_pass && *ftp_pass) {
        if (session->ftp_login != 1 || strcmp(ftp_pass, pass)) {
            session->ftp_login = 0;
            return NutFtpRespondBad(session, 550);
        }
    }
    session->ftp_login = 2;
    return NutFtpRespondOk(session, 230);
}

/*!
 * \brief Process an FTP client's PASV command.
 *
 * This command requests the server to listen on a data port and to wait
 * for a connection rather than initiate one upon receipt of a transfer
 * command.
 *
 * \param session Pointer to an \ref FTPSESSION structure, obtained by a
 *                previous call to NutFtpOpenSession().
 *
 * \return 0 if the command has been processed or -1 if the
 *         session has been aborted.
 */
int NutFtpProcessPassiv(FTPSESSION * session)
{
    static prog_char passiveprint_P[] = "227 Passive (%u,%u,%u,%u,%u,%u).\r\n";
    uint32_t ip = session->ftp_sock->so_local_addr;
    uint16_t port = 20;

    fprintf_P(session->ftp_stream, passiveprint_P,        /* */
            (uint8_t) ip, (uint8_t) (ip >> 8), (uint8_t) (ip >> 16), (uint8_t) (ip >> 24),  /* */
            (uint8_t) (port >> 8), (uint8_t) port);
    fflush(session->ftp_stream);
    session->ftp_passive = 1;

    return 0;
}

/*!
 * \brief Process an FTP client's PORT command.
 *
 * \param session Pointer to an \ref FTPSESSION structure, obtained by a
 *                previous call to NutFtpOpenSession().
 * \param args    Command argument, which is the concatenation of the
 *                32-bit internet host address and a 16-bit TCP port
 *                address. This address information is broken into 8-bit
 *                fields and the value of each field is transmitted as
 *                a decimal number.
 *
 * \return 0 if the command has been processed or -1 if the
 *         session has been aborted.
 */
int NutFtpProcessPort(FTPSESSION * session, char *args)
{
    if (ParseIpPort(args, &session->ftp_data_ip, &session->ftp_data_port) == 6) {
        if (session->ftp_sock->so_remote_addr == session->ftp_data_ip) {
            return NutFtpRespondOk(session, 200);
        }
        return NutFtpRespondBad(session, 425);
    }
    return NutFtpRespondBad(session, 501);;
}

/*!
 * \brief Process an FTP client's PWD command.
 *
 * Causes the name of the current working directory to be
 * returned in the reply.
 *
 * \param session Pointer to an \ref FTPSESSION structure, obtained by a
 *                previous call to NutFtpOpenSession().
 *
 * \return 0 if the command has been processed or -1 if the
 *         session has been aborted.
 */
int NutFtpProcessPwd(FTPSESSION * session)
{
    static prog_char pwdanswer_P[] = "257 \"%s\"\r\n";
#ifdef FTPD_DEBUG
    printf("\n<'257 \"%s\"' ", session->ftp_cwd);
#endif
    fprintf_P(session->ftp_stream, pwdanswer_P, session->ftp_cwd);
    return 0;
}

/*!
 * \brief Process an FTP client's RMD command.
 *
 * Causes the specified directory to be removed.
 *
 * \param session Pointer to an \ref FTPSESSION structure, obtained by a
 *                previous call to NutFtpOpenSession().
 * \param path    Full absolute path name of the directory.
 *
 * \return 0 if the command has been processed or -1 if the
 *         session has been aborted.
 */
int NutFtpProcessRmd(FTPSESSION * session, char *path)
{
    if (rmdir(path)) {
        return NutFtpRespondBad(session, 451);
    }
    return NutFtpRespondOk(session, 257);
}

/*!
 * \brief Process an FTP client's SYST command.
 *
 * \param session Pointer to an \ref FTPSESSION structure, obtained by a
 *                previous call to NutFtpOpenSession().
 *
 * \return 0 if the command has been processed or -1 if the
 *         session has been aborted.
 */
int NutFtpProcessSystem(FTPSESSION * session)
{
    static prog_char unixtype_P[] = "215 UNIX Type: L8\r\n";
#ifdef FTPD_DEBUG
    printf("\n<'215 UNIX Type: L8' ");
#endif
    fputs_P(unixtype_P, session->ftp_stream);
    return 0;
}

/*!
 * \brief Process an FTP client's TYPE command.
 *
 * The type is not fully checked. Any argument starting with
 * the letters 'A' or 'a' will switch the transfer mode to
 * ASCII. Otherwise binary mode is set.
 *
 * \param session  Pointer to an \ref FTPSESSION structure, obtained by a
 *                 previous call to NutFtpOpenSession().
 * \param typecode Command arguments.
 *
 * \return 0 if the command has been processed or -1 if the
 *         session has been aborted.
 */
int NutFtpProcessType(FTPSESSION * session, char *typecode)
{
    session->ftp_tran_mode = (*typecode != 'A') && (*typecode != 'a');
    return NutFtpRespondOk(session, 200);
}

/*!
 * \brief Process an FTP client's USER command.
 *
 * Only one login per session is accepted.
 *
 * \param session Pointer to an \ref FTPSESSION structure, obtained by a
 *                previous call to NutFtpOpenSession().
 * \param user    User name.
 *
 * \return 0 if the command has been processed or -1 if the
 *         session has been aborted.
 */
int NutFtpProcessUser(FTPSESSION * session, char *user)
{
    if (ftp_user && *ftp_user) {
        if (session->ftp_login && strcmp(ftp_user, user)) {
            session->ftp_login = 0;
            return NutFtpRespondBad(session, 550);
        }
    }

    /* Need a password too. */
    if (ftp_pass && *ftp_pass) {
        session->ftp_login = 1;
        return NutFtpRespondOk(session, 331);
    }

    /* No password required. */
    session->ftp_login = 2;
    return NutFtpRespondOk(session, 230);
}

/*!
 * \brief Process an FTP request.
 *
 * This routine implements the protocol interpreter of the FTP server.
 *
 * \param session Pointer to an \ref FTPSESSION structure, obtained by a
 *                previous call to NutFtpOpenSession().
 * \param request Request string.
 *
 * \return -1 if the session ended. Otherwise 0 is returned.
 */
int NutFtpProcessRequest(FTPSESSION * session, char *request)
{
    int rc = 0;
    char *cmd;
    char *args;

    /* Split the line into command and argument part. */
    SplitCmdArg(request, &cmd, &args);
#ifdef FTPD_DEBUG
    printf("\n>'%s %s' ", cmd, args);
#endif

    /* Clean-up if rename request is incomplete */
    if ((session->ftp_renamesource) && (strcmp_P(cmd, cmd_rename2_P))) {
          free(session->ftp_renamesource);
          session->ftp_renamesource = NULL;
    }
    /* QUIT - Terminate session. */
    if (strcmp_P(cmd, cmd_quit_P) == 0) {
        NutFtpRespondOk(session, 221);
        rc = -1;
    }
    /* USER <username> - Check user name. */
    else if (strcmp_P(cmd, cmd_user_P) == 0) {
        rc = NutFtpProcessUser(session, args);
    }

    /* PASS <password> - Check user password. */
    else if (strcmp_P(cmd, cmd_pass_P) == 0) {
        rc = NutFtpProcessPass(session, args);
    } else if (strcmp_P(cmd, cmd_noop_P) == 0) {
        NutFtpRespondOk(session, 200);
    }
    /* Anything else requires a successful login. */
    else if (session->ftp_login < 2) {
        rc = NutFtpRespondBad(session, 530);
    }

    /* Valid login. */
    else {

        /* PASV - Prepare passive transfer. */
        if (strcmp_P(cmd, cmd_pasv_P) == 0) {
            rc = NutFtpProcessPassiv(session);
        }

        /* PORT <host-port> - Set data connection. */
        else if (strcmp_P(cmd, cmd_port_P) == 0) {
            rc = NutFtpProcessPort(session, args);
        }

        /* [X]PWD - Send name of current working directory. */
        else if (strcmp_P(cmd, cmd_pwd_P) == 0 || strcmp_P(cmd, cmd_xpwd_P) == 0) {
            rc = NutFtpProcessPwd(session);
        }

        /* SYST - Send system identifier. */
        else if (strcmp_P(cmd, cmd_syst_P) == 0) {
            rc = NutFtpProcessSystem(session);
        }

        /* TYPE <type-code> - Receive transfer mode. */
        else if (strcmp_P(cmd, cmd_type_P) == 0) {
            rc = NutFtpProcessType(session, args);
        }
        /* Commands with path name argument. */
        else {
            char *path;
            char *argsredux = args;
            int options = 0;
            if (args[0] == '-') {
                 if (strchr(args, 'a')) {
                   options = 1;
                 }
                 argsredux = strchr(args, ' ');
                 if ((argsredux) && (strlen(argsredux) > 0)) {
                     argsredux++; //remove space
                 } else {
                     argsredux = "";
                 }
            }
            if ((path = CreateFullPathName(ftp_root, session->ftp_cwd, argsredux)) == 0) {
                rc = NutFtpRespondBad(session, 451);
            }

            /* CWD <pathname> - Change working directory. */
            else if (strcmp_P(cmd, cmd_cwd_P) == 0) {
                rc = NutFtpProcessCwd(session, path);
            }

            /* DELE <pathname> - Delete a file. */
            else if (strcmp_P(cmd, cmd_dele_P) == 0) {
                rc = NutFtpProcessDelete(session, path);
            }

            /* LIST | NLST [<pathname>] - Send list of files in a directory. */
            else if (strcmp_P(cmd, cmd_list_P) == 0 || strcmp_P(cmd, cmd_nlst_P) == 0) {
                rc = NutFtpTransferDirectoryOptions(session, path, options);
            }

            /* MKD <pathname> - Make a directory. */
            else if (strcmp_P(cmd, cmd_mkd_P) == 0 || strcmp_P(cmd, cmd_xmkd_P) == 0) {
                rc = NutFtpProcessMkd(session, path);
            }

            /* RETR <pathname> - Send a file to the client. */
            else if (strcmp_P(cmd, cmd_retr_P) == 0) {
                rc = NutFtpTransferFile(session, path, 1);
            }

            /* RMD <pathname> - Remove a directory. */
            else if (strcmp_P(cmd, cmd_rmd_P) == 0 || strcmp_P(cmd, cmd_xrmd_P) == 0) {
                rc = NutFtpProcessRmd(session, path);
            }

            /* STOR <pathname> - Receive a file from the client. */
            else if (strcmp_P(cmd, cmd_stor_P) == 0) {
                rc = NutFtpTransferFile(session, path, 0);
            }
            /* RNFR <pathname> from - Rename a file on the client. */
            else if (strcmp_P(cmd, cmd_rename1_P) == 0) {
                rc = NutFtpRenamePrepare(session, path);
            }
            /* RNTO <pathname> to - Rename a file on the client. */
            else if (strcmp_P(cmd, cmd_rename2_P) == 0) {
                rc = NutFtpRenameAction(session, path);
            }
            /* Anything else is an unknown command. */
            else {
                rc = NutFtpRespondBad(session, 502);
            }

            if (path) {
                free(path);
            }
        }
    }
    return rc;
}

/*!
 * \brief Process an FTP sever session.
 *
 * Processes FTP requests on an established connection with an FTP client.
 * This routine completely implements an FTP server except TCP connect
 * and disconnect.
 *
 * For file transfers, the same maximum segment size and timeouts as set
 * for this socket will be used for the data connection.
 *
 * \param sock Socket of an established TCP connection.
 *
 * \return 0 if the session ended successfully or -1 if the session
 *         has been aborted.
 */
int NutFtpServerSession(TCPSOCKET * sock)
{
    int rc = 0;
    FTPSESSION *session;
    char *buff;
    time_t now;
    struct _tm *tip;
    char ch;

    /* Set the root path if not yet done. */
    if (NutRegisterFtpRoot(ftp_root)) {
        return -1;
    }

    /* Allocate the command line buffer. */
    if ((buff = malloc(FTP_MAX_CMDBUF)) == 0) {
        return -1;
    }

    /* Create a session structure and open a stream. */
    if ((session = NutFtpOpenSession(sock)) == 0) {
        free(buff);
        return -1;
    }

    /* Send a welcome banner including system time. */
    time(&now);
    tip = localtime(&now);
#ifdef FTPD_DEBUG
    printf("\n<'");
    printf_P(rep_banner, NutVersionString(), &mon_name[tip->tm_mon * 3],        /* */
             tip->tm_mday, tip->tm_hour, tip->tm_min, tip->tm_sec);
#endif
    fprintf_P(session->ftp_stream, rep_banner, NutVersionString(),      /* */
              &mon_name[tip->tm_mon * 3],       /* */
              tip->tm_mday, tip->tm_hour, tip->tm_min, tip->tm_sec);

    /*
     * Loop for requests.
     */
    while (rc == 0) {

        /* Flush any previous output and read a new command line. */
        fflush(session->ftp_stream);
        if (fgets(buff, FTP_MAX_CMDBUF, session->ftp_stream) == 0) {
            rc = -1;
            break;
        }

        /* Skip command lines, which are too long. */
        if ((ch = *(buff + strlen(buff) - 1)) != '\n' && ch != '\r') {
            do {
                if (fgets(buff, FTP_MAX_CMDBUF, session->ftp_stream) == 0) {
                    rc = -1;
                    break;
                }
            } while ((ch = *(buff + strlen(buff) - 1)) != '\n' && ch != '\r');
            if (rc == 0) {
                rc = NutFtpRespondBad(session, 500);
            }
        }

        /* Process the request. */
        else {
            rc = NutFtpProcessRequest(session, buff);
        }
    }

    /* Cleanup and return. */
    NutFtpCloseSession(session);
    free(buff);
    return rc;
}

/*@}*/
