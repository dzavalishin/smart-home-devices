#ifndef _TFTP_H
#define _TFTP_H

/*
 * Copyright (C) 2002-2007 by egnite Software GmbH
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

/*
 * $Id: tftp.h 2954 2010-04-03 11:22:40Z haraldkipp $
 */

/*!
 * \addtogroup xgTftp
 */
/*@{*/

typedef struct __attribute__ ((packed)) tftphdr {
    short th_opcode;            /* packet type */
    union __attribute__ ((packed)) {
        short tu_block;         /* block # */
        short tu_code;          /* error code */
        char tu_stuff[1];       /* request packet stuff */
    } th_u;
    char th_data[512];          /* data or error string */
} TFTPHDR;


#define TFTP_RRQ     0x0100     /*!< \brief TFTP read request packet. */
#define TFTP_WRQ     02         /*!< \brief TFTP write request packet. */
#define TFTP_DATA    03         /*!< \brief TFTP data packet. */
#define TFTP_ACK     0x0400     /*!< \brief TFTP acknowledgement packet. */
#define TFTP_ERROR   05         /*!< \brief TFTP error packet. */
#define TFTP_OACK    06         /*!< \brief TFTP option acknowledgement packet. */

/*@}*/

#define SPORT   0x0004
#define TPORT   0x4500

extern int TftpRecv(void);

#endif
