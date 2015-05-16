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
 * $Id: tftp.c 2954 2010-04-03 11:22:40Z haraldkipp $
 */

#include <string.h>

#include "dialog.h"
#include "bootmon.h"
#include "tftp.h"

/*!
 * \addtogroup xgTftp
 */
/*@{*/

/*
 * Write the contents of a buffer to a specified program memory address.
 *
 * \param addr Program memory byte address to start writing.
 * \param data Points to a buffer which contains the data to be written.
 *
 * \todo Would be fine to verify the result and return a result to the
 *       caller.
 */
void StoreBlock(unsigned short page, unsigned char *data)
{
    unsigned long i;
    unsigned long *addr;
    unsigned long val;

    addr = (unsigned long *) ((unsigned long) page << 9);
    for (i = 0; i < 512; i += 4) {
        val = *data++;
        val |= *data++ << 8;
        val |= *data++ << 16;
        val |= *data++ << 24;
        *addr++ = val;
    }
}

/*!
 * \brief Download a file from a TFTP server and burn it into the flash ROM.
 *
 * \return 0 on success, -1 otherwise.
 */
int TftpRecv(void)
{
    unsigned char retry;
    int rlen = 0;
    int slen;
    unsigned short tport = TPORT;
    unsigned short block = 0;
    unsigned char *cp;
    char *cp1;

    /*
     * Do nothing if there's no TFTP host configured.
     */
    if (confboot.cb_tftp_ip == 0) {
        DEBUG("TFTP skipped\n");
        return 0;
    }

    /*
     * Prepare the transmit buffer for a file request.
     */
    sframe.eth.udp.u.tftp.th_opcode = TFTP_RRQ;
    slen = 2;
    cp = (unsigned char*)sframe.eth.udp.u.tftp.th_u.tu_stuff;
    cp1 = (char *)confboot.cb_image;
    if (*cp1 == 0) {
        cp1 = "enut.bin";
    }
    DEBUG("Loading ");
    DEBUG(cp1);
    do {
        *cp = *cp1++;
        slen++;
    } while (*cp++);
    memcpy_(cp, (unsigned char*)"octet", 6);
    slen += 6;

    /*
     * Loop until we receive a packet with less than 512 bytes of data.
     */
    do {

        /*
         * Send file request or acknowledge and receive
         * a data block.
         */
        for (retry = 0; retry < 3; retry++) {
            DEBUG("[RQ TFTP]");
            if (UdpOutput(confboot.cb_tftp_ip, tport, SPORT, slen) >= 0) {
                if ((rlen = UdpInput(SPORT, 500)) >= 4)
                    break;
            }
        }
        DEBUG("[GO-ON]");

        /*
         * Can't reach the TFTP server or got a malformed
         * repsonse.
         */
        if (retry >= 3 || rlen < 4) {
            return -1;
        }


        /*
         * Accept data blocks only. Anything else will stop
         * the transfer with an error.
         */
        if (ntohs(rframe.eth.udp.u.tftp.th_opcode) != TFTP_DATA)
            return -1;

        /*
         * If this was the first block we received, prepare
         * the send buffer for sending ACKs.
         */
        if (block == 0) {
            tport = rframe.eth.udp.udp_hdr.uh_sport;
            sframe.eth.udp.u.tftp.th_opcode = TFTP_ACK;
            slen = 4;
        }

        /*
         * If this block is out of sequence, we ignore it.
         * However, if we missed the first block, return
         * with an error.
         */
        if (ntohs(rframe.eth.udp.u.tftp.th_u.tu_block) != block + 1) {
            if (block == 0) {
                return -1;
            }
            continue;
        }

        /*
         * Burn the received data into the flash ROM.
         */
        if (rlen > 4) {
            StoreBlock(block, (unsigned char*)rframe.eth.udp.u.tftp.th_data);
        }

        /*
         * Update our block counter.
         */
        block++;
        sframe.eth.udp.u.tftp.th_u.tu_block = htons(block);

    } while (rlen >= 516);

    /*
     * Send the last ACK.
     */
    UdpOutput(confboot.cb_tftp_ip, tport, SPORT, slen);

    return 0;
}

/*@}*/
