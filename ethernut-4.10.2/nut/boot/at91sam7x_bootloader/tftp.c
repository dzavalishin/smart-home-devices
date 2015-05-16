/*
 * Copyright (C) 2008 by Thermotemp GmbH. All rights reserved.
 * Copyright (C) 2002-2007 by egnite Software GmbH. All rights reserved.
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

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/heap.h>

#include "default.h"
#include "debug.h"
#include "tftp.h"

tftp_header tx_frame;
tftp_header rx_frame;

int tftp_receive(char *filename, u_long tftp_ip, int (*callback)(u_char *buffer, u_short block_size, u_long offset, void* user_data), void* user_data)
{
    unsigned char retry;
    int len  = 0;
    int rx_len = 0;
    int tx_len = 0;
    unsigned short block = 0;
    char *cp;
    
    UDPSOCKET *tftp_socket;
    u_long  remote_ip;
    u_short remote_port = TFTP_PORT;
    u_long  offset = 0;
    int     total_bytes = 0;     
    
    /*
     * Do nothing if there's no TFTP host configured.
     */

    tftp_socket = NutUdpCreateSocket(TFTP_PORT);
    if (tftp_socket == 0) {
        ERROR("Could not create TFTP UDP socket\r\n");
        goto error;
    }
    
    /*
     * Prepare the transmit buffer for a file request.
     */
    tx_frame.th_opcode = htons(TFTP_RRQ);
    
    tx_len = 2;
    cp = tx_frame.th_u.tu_stuff;
    
    INFO("TFTP Loading %s\r\n", filename);
    strcpy(cp, filename);
    len     = strlen(filename) + 1;
    tx_len += len;
    cp     += len;
           
    strcpy(cp, "octet");
    tx_len += 6;
    
    /*
     * Loop until we receive a packet with less than 512 bytes of data.
     */
    do {
        /*
         * Send file request or acknowledge and receive
         * a data block. Maximum 3 retries on timeout or error
         */
        for (retry = 0; retry < 3; retry++) {
            if (NutUdpSendTo(tftp_socket, tftp_ip, remote_port, &tx_frame, tx_len) == 0) {
                rx_len = NutUdpReceiveFrom(tftp_socket, &remote_ip, &remote_port, &rx_frame, sizeof(tftp_header), 5000); 
                if (rx_len >= 4) break;
            }
        }

        /*
         * Can't reach the TFTP server or got a malformed
         * repsonse.
         */
        if (retry >= 3 || rx_len < 4) {
            goto error;
        }

        /*
         * Accept data blocks only. Anything else will stop
         * the transfer with an error.
         */
        if (ntohs(rx_frame.th_opcode) != TFTP_DATA) {
            goto error;
        }

        /*
         * If this was the first block we received, prepare
         * the send buffer for sending ACKs.
         */
        if (block == 0) {
            tx_frame.th_opcode = htons(TFTP_ACK);
            tx_len = 4;
        }

        /*
         * If this block is out of sequence, we ignore it.
         * However, if we missed the first block, return
         * with an error.
         */
        if (ntohs(rx_frame.th_u.tu_block) != block + 1) {
            if (block == 0) {
                goto error;
            }
            continue;
        }

        /*
         * Burn the received data into the flash ROM.
         */
        if (rx_len > 4) {
            //INFO("TFTP received block %d\r\n", block);
		INFO(".");
            callback(rx_frame.th_data, rx_len-4, offset, user_data);
            total_bytes += rx_len-4;
            offset += MIN(rx_len - 4, 512);
        }

        /*
         * Update our block counter.
         */
        block++;
        tx_frame.th_u.tu_block = htons(block);
    } while (rx_len >= 516);

    /*
     * Send the last ACK.
     */
    NutUdpSendTo(tftp_socket, tftp_ip, remote_port, &tx_frame, tx_len);
    
    return total_bytes;
    
error:    
    return -1;
}
