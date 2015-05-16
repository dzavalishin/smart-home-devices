/*
 * Copyright (C) 2002-2004 by egnite Software GmbH. All rights reserved.
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
 * Revision 1.3  2005/11/03 15:11:58  haraldkipp
 * Make use of the new memset/memcpy routines.
 * Some globals replaced by CONF structures.
 * Use enut.bin as a default image name.
 *
 * Revision 1.2  2004/01/30 18:18:55  haraldkipp
 * Bugfix. Loading programs > 64k failed
 *
 * Revision 1.1  2003/11/03 16:19:38  haraldkipp
 * First release
 *
 */

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/boot.h>
#include <string.h>

#include "debug.h"
#include "appload.h"
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
void FlashPage(unsigned short page, unsigned char *data)
{
    unsigned long i;
    unsigned long addr;

    addr = (unsigned long)page << 8;

    /*
     * Erase page.
     */
    boot_page_erase(addr);
    while (boot_rww_busy()) {
        boot_rww_enable();
    }

    /*
     * Fill page buffer.
     */
    for (i = addr; i < addr + SPM_PAGESIZE; i += 2) {
        boot_page_fill(i, *data + (*(data + 1) << 8));
        data += 2;
    }

    /*
     * Write page.
     */
    boot_page_write(addr);
    while (boot_rww_busy()) {
        boot_rww_enable();
    }
}

/*!
 * \brief Download a file from a TFTP server and burn it into the flash ROM.
 *
 * \return 0 on success, -1 otherwise.
 */
int TftpRecv(void)
{
    u_char retry;
    int rlen = 0;
    int slen;
    u_short tport = TPORT;
    u_short block = 0;
    u_char *cp;
    u_char *cp1;

    /*
     * Do nothing if there's no TFTP host configured.
     */
    if(confboot.cb_tftp_ip == 0) {
        return 0;
    }

    /*
     * Prepare the transmit buffer for a file request.
     */
    sframe.u.tftp.th_opcode = TFTP_RRQ;
    slen = 2;
    cp = sframe.u.tftp.th_u.tu_stuff;
    cp1 = confboot.cb_image;
    if (*cp1 == 0) {
        cp1 = "enut.bin";
    }
    do {
        *cp = *cp1++;
        slen++;
    } while (*cp++);
    memcpy_(cp, "octet", 6);
    slen += 6;

    /*
     * Lopp until we receive a packet with less than 512 bytes of data.
     */
    do {

        /*
         * Send file request or acknowledge and receive
         * a data block.
         */
        for (retry = 0; retry < 3; retry++) {
            if (UdpOutput(confboot.cb_tftp_ip, tport, SPORT, slen) >= 0) {
                if ((rlen = UdpInput(SPORT, 5000)) >= 4)
                    break;
            }
        }

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
        if (ntohs(rframe.u.tftp.th_opcode) != TFTP_DATA)
            return -1;

        /*
         * If this was the first block we received, prepare
         * the send buffer for sending ACKs.
         */
        if (block == 0) {
            tport = rframe.udp_hdr.uh_sport;
            sframe.u.tftp.th_opcode = TFTP_ACK;
            slen = 4;
        }

        /*
         * If this block is out of sequence, we ignore it.
         * However, if we missed the first block, return
         * with an error.
         */
        if (ntohs(rframe.u.tftp.th_u.tu_block) != block + 1) {
            if (block == 0) {
                return -1;
            }
            continue;
        }

        /*
         * Burn the received data into the flash ROM.
         */
        if (rlen > 4) {
            FlashPage(block << 1, rframe.u.tftp.th_data);
            if (rlen > 260)
                FlashPage((block << 1) + 1, &rframe.u.tftp.th_data[256]);
        }

        /*
         * Update our block counter.
         */
        block++;
        sframe.u.tftp.th_u.tu_block = htons(block);

    } while (rlen >= 516);

    /*
     * Send the last ACK.
     */
    UdpOutput(confboot.cb_tftp_ip, tport, SPORT, slen);

    return 0;
}

/*@}*/
