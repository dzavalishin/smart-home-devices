#ifndef _DEV_NICRTL_H_
#define _DEV_NICRTL_H_

/*
 * Copyright (C) 2001-2004 by egnite Software GmbH. All rights reserved.
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
 * Revision 1.5  2008/08/11 06:59:59  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.4  2004/05/17 19:14:57  haraldkipp
 * Added Bengt Florin's RTL8019 driver mods
 *
 * Revision 1.3  2003/11/03 16:58:42  haraldkipp
 * *** empty log message ***
 *
 * Revision 1.2  2003/07/20 16:40:52  haraldkipp
 * CrystalTek 8900A driver added.
 *
 * Revision 1.1.1.1  2003/05/09 14:41:07  haraldkipp
 * Initial using 3.2.1
 *
 */

#include <dev/netbuf.h>
#include <net/if_var.h>


/*!
 * \file dev/nicrtl.h
 * \brief Network interface controller definitions.
 */

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \struct _NICINFO nicrtl.h dev/nicrtl.h
 * \brief Network interface controller information structure.
 */
struct _NICINFO {
    HANDLE volatile ni_rx_rdy;      /*!< Receiver event queue. */
    uint32_t ni_rx_packets;           /*!< Number of packets received. */
    uint32_t ni_tx_packets;           /*!< Number of packets sent. */
    uint32_t ni_interrupts;           /*!< Number of interrupts. */
    uint32_t ni_rx_overruns;          /*!< Number of packet overruns. */
    uint32_t ni_rx_frame_errors;      /*!< Number of frame errors. */
    uint32_t ni_rx_crc_errors;        /*!< Number of CRC errors. */
    uint32_t ni_rx_missed_errors;     /*!< Number of missed packets. */
    uint32_t ni_rx_size_errors;       /*!< Number of size missmatch in NIC. */
    uint32_t ni_tx_errors;            /*!< Number of NIC transmit errors. */
};

/*!
 * \brief Network interface controller information type.
 */
typedef struct _NICINFO NICINFO;



/*
 * Available drivers.
 */
extern NUTDEVICE devEth0;
extern NUTDEVICE devEth0cs;

#ifndef DEV_ETHER
#define DEV_ETHER   devEth0
#endif

/*
 * Driver routines.
 */
extern int NicInit(NUTDEVICE *dev);
extern int NicOutput(NUTDEVICE *dev, NETBUF *nb);

#ifdef __cplusplus
}
#endif

#endif
