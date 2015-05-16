/*
 * Copyright (C) 2003-2005 by egnite Software GmbH. All rights reserved.
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
 */

/*
 * $Log$
 * Revision 1.4  2005/11/03 14:58:39  haraldkipp
 * Runtime initialization removed. We have a stratup file now.
 * Debug output via macros avoids ifdef contamination.
 * Some globals replaced by CONF structures.
 * Subroutines moved to new source files.
 *
 * Revision 1.3  2005/06/06 10:42:36  haraldkipp
 * Fix after avr-libc removed outb/outp.
 *
 * Revision 1.2  2004/02/20 12:31:36  haraldkipp
 * Ignore target IP if local IP is not configured
 *
 * Revision 1.1  2003/11/03 16:19:38  haraldkipp
 * First release
 *
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/boot.h>

#include "debug.h"
#include "ether.h"
#include "dhcp.h"
#include "appload.h"

BOOTFRAME sframe;
BOOTFRAME rframe;

/*
 * Application loader entry point.
 */
int main(void)
{
    /*
     * Enable external data and address bus.
     */
    MCUCR = _BV(SRE) | _BV(SRW);

    DEBUGINIT();
    DEBUG("\nAppLoad 1.1.1\n");

    BootConfigRead();

    /*
     * Initialize the network interface controller hardware.
     */
    DEBUG("ETHERNET ");
    if(EtherInit() == 0) {
        DEBUG("OK\n");

        /*
         * DHCP query and TFTP download.
         */
        if (DhcpQuery() == 0) {
            TftpRecv();
        }
    }
    else {
        DEBUG("No\n");
    }
    return 0;
}

