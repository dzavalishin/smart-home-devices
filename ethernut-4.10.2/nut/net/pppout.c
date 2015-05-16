/*!
 * Copyright (C) 2002 by Call Direct Cellular Solutions Pty. Ltd. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY CALL DIRECT CELLULAR SOLUTIONS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL CALL DIRECT
 * CELLULAR SOLUTIONS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * For additional information see http://www.calldirect.com.au/
 * -
 *
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
 *
 * -
 */

/*
 * $Log$
 * Revision 1.9  2009/01/17 11:26:51  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.8  2008/08/11 07:00:32  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.7  2005/04/30 16:42:42  chaac
 * Fixed bug in handling of NUTDEBUG. Added include for cfg/os.h. If NUTDEBUG
 * is defined in NutConf, it will make effect where it is used.
 *
 * Revision 1.6  2005/04/08 15:20:51  olereinhardt
 * added <sys/types.h> (__APPLE__) and <netinet/in.h> (__linux__)
 * for htons and simmilar.
 *
 * Revision 1.5  2004/03/16 16:48:45  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 * Revision 1.4  2004/03/08 11:28:23  haraldkipp
 * HDLC functions moved to async HDLC driver.
 *
 * Revision 1.3  2003/08/14 15:15:28  haraldkipp
 * Unsuccessful try to fix ICCAVR bug
 *
 * Revision 1.2  2003/07/13 19:09:59  haraldkipp
 * Debug output fixed.
 *
 * Revision 1.1.1.1  2003/05/09 14:41:37  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.2  2003/05/06 18:17:58  harald
 * PPP hack for simple UART support
 *
 * Revision 1.1  2003/03/31 14:53:28  harald
 * Prepare release 3.1
 *
 */

#include <cfg/os.h>
#include <string.h>

#include <dev/ppp.h>
#include <dev/ahdlc.h>
#include <netinet/in.h>
#include <netinet/if_ppp.h>
#include <net/ppp.h>
#include <sys/types.h>
#include <sys/timer.h>

#ifdef NUTDEBUG
#include <net/netdebug.h>
#endif

/*!
 * \addtogroup xgPPP
 */
/*@{*/


/*!
 * \brief Send PPP frame.
 *
 * Send a PPP frame of a given type using the specified device.
 *
 * \param dev   Identifies the network device to use.
 * \param type  Type of this frame.
 * \param ha    Hardware address of the destination, ignored with PPP.
 * \param nb    Network buffer structure containing the packet to be sent.
 *              The structure must have been allocated by a previous
 *              call NutNetBufAlloc() and will be freed if this function
 *              returns with an error.
 *
 * \return 0 on success, -1 in case of any errors.
 */
int NutPppOutput(NUTDEVICE * dev, uint16_t type, uint8_t * ha, NETBUF * nb)
{
    PPPHDR *ph;
    IFNET *nif = dev->dev_icb;
    PPPDCB *dcb = dev->dev_dcb;

    /*
     * Allocate and set the HDLC header.
     */
    if (NutNetBufAlloc(nb, NBAF_DATALINK, sizeof(PPPHDR)) == 0)
        return -1;

    ph = (PPPHDR *) nb->nb_dl.vp;
    ph->address = AHDLC_ALLSTATIONS;
    ph->control = AHDLC_UI;
    ph->prot_type = htons(type);

#ifdef NUTDEBUG
    if (__ppp_trf) {
        fputs("\nPPP<", __ppp_trs);
        NutDumpPpp(__ppp_trs, nb);
    }
#elif defined(__IMAGECRAFT__)
    /*
     * No idea what this is, but ICCAVR fails if this call isn't there.
     */
    NutSleep(100);
#endif

    /*
     * Call the physical device output routine.
     */
    if (nif->if_send && (*nif->if_send) ((((NUTFILE *) (uintptr_t) (dcb->dcb_fd)))->nf_dev, nb) == 0) {
        return 0;
    }
    NutNetBufFree(nb);
    return -1;
}


/*@}*/
