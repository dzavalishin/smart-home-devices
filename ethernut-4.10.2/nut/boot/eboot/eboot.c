/*
 * Copyright (C) 2002-2004 by egnite Software GmbH
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
 * $Id: eboot.c 3006 2010-05-10 12:39:44Z haraldkipp $
 */

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>

#include "config.h"
#include "debug.h"
#include "ether.h"
#include "dhcp.h"
#include "tftp.h"
#include "util.h"
#include "eboot.h"

BOOTFRAME sframe;
BOOTFRAME rframe;
u_long sid;

/*!
 * \addtogroup xgEBoot
 */
/*@{*/

/*!
 * \brief Boot loader entry.
 *
 * This boot loader is very special. It is completely self
 * contained, which means that it runs without any library.
 * This entry point must be linked first and will be located
 * at byte address 0x1F000 in the program flash ROM.
 *
 * \return Never, but jumps at absolute address 0 when done.
 */
int main(void)
{
#if defined(__AVR_ATmega2561__)
    /* unlike ATMega128 the ATMega2561 does not disbale the watchdog */
    /* after a reset, so we need to do this here                     */
 
    MCUSR = 0;
    wdt_disable();
#endif

    /*
     * Enable external data and address bus.
     */
    MCUCR = _BV(SRE) | _BV(SRW);
    DEBUGINIT();
    DEBUG("\neboot 2.0.0\n");

    BootConfigRead();

    /*
     * Initialize the network interface controller hardware.
     */
    DEBUG("ETHERNET ");
    if (NicInit() == 0) {
        DEBUG("OK\n");

        /*
        * DHCP query and TFTP download.
        */
        if (DhcpQuery() == 0 && confboot.cb_image[0])
            TftpRecv();
    } else {
        DEBUG("No\n");
    }

    /*
     * Will jump to the application.
     */
    return 0;
}

/*@}*/
