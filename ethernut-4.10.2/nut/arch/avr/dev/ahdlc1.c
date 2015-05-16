/*
 * Copyright (C) 2003-2004 by egnite Software GmbH. All rights reserved.
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

/*
 * $Log$
 * Revision 1.1  2005/07/26 18:02:27  haraldkipp
 * Moved from dev.
 *
 * Revision 1.1  2004/03/08 11:16:54  haraldkipp
 * Asynchronous HDLC driver added
 *
 */

#include <netinet/if_ppp.h>
#include <net/ppp.h>
#include <net/if_var.h>
#include <dev/ahdlcavr.h>

/*!
 * \addtogroup xgAhdlcAvr
 */
/*@{*/

static AHDLCDCB dcb_ahdlc;

/*!
 * \brief Device information structure.
 *
 * A pointer to this structure must be passed to NutRegisterDevice() 
 * to bind this device driver to the Nut/OS kernel.
 */
NUTDEVICE devAhdlc1 = {
    0,                          /* Pointer to next device. */
    {'u', 'a', 'r', 't', '1', 0, 0, 0, 0},      /* Unique device name. */
    IFTYP_CHAR,                 /* Type of device. */
    1,                          /* Base address. */
    0,                          /* First interrupt number. */
    0,                          /* Interface control block. */
    &dcb_ahdlc,                 /* Driver control block. */
    AhdlcAvrInit,               /* Driver initialization routine, dev_init. */
    AhdlcAvrIOCtl,              /* Driver specific control function, dev_ioctl. */
    AhdlcAvrRead,               /* Read from device, dev_read. */
    AhdlcAvrWrite,              /* Write to device, dev_write. */
    AhdlcAvrWrite_P,            /* Write data from program space to device, dev_write_P. */
    AhdlcAvrOpen,               /* Open a device or file, dev_open. */
    AhdlcAvrClose,              /* Close a device or file, dev_close. */
    0                           /* Request file size, dev_size. */
};


/*@}*/
