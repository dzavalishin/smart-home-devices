/*
 * Copyright (C) 2004 by Ole Reinhardt <ole.reinhardt@kernelconcepts.de>,
 *                       Kernelconcepts http://www.kernelconcepts.de
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
 * \file dev/can_dev.c
 * \brief Driver for CAN-Bus devices
 *
 *
 */


/*
 * $Log$
 * Revision 1.7  2008/08/11 06:59:41  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.6  2007/09/08 03:01:11  hwmaier
 * Changes to support RX time-out, CAN_SetRxTimeout() added.
 *
 * Revision 1.5  2005/10/07 21:38:44  hwmaier
 * CAN_SetSpeed function added.
 *
 * Revision 1.4  2004/08/25 15:45:18  olereinhardt
 * Added function to set acceptance filter
 *
 * Revision 1.3  2004/08/02 09:56:13  olereinhardt
 * changed typing of CAN_TryRxFrame
 *
 * Revision 1.2  2004/06/23 10:17:04  olereinhardt
 * Added buffer monitoring functions (free / avail)
 *
 * Revision 1.1  2004/06/07 15:15:28  olereinhardt
 * Initial checkin
 *
 */

/*!
 * \addtogroup xgCanDev
 */
/*@{*/


/* Not ported. */
#ifdef __GNUC__

#include <stdio.h>
#include <sys/timer.h>
#include <sys/device.h>
#include <dev/can_dev.h>


uint8_t CAN_SetSpeed(NUTDEVICE *dev, uint32_t baudrate)
{
    return (((IFCAN *)(dev->dev_icb))->can_set_baud)(dev, baudrate);
}

void CAN_SetFilter(NUTDEVICE *dev, uint8_t *ac, uint8_t *am)
{
    (((IFCAN *)(dev->dev_icb))->can_set_ac)(dev, ac);
    (((IFCAN *)(dev->dev_icb))->can_set_am)(dev, am);
}

void CAN_TxFrame(NUTDEVICE *dev, CANFRAME *frame)
{
    (((IFCAN *)(dev->dev_icb))->can_send)(dev, frame);
}

uint8_t CAN_TryTxFrame(NUTDEVICE *dev, CANFRAME *frame)
{
    if (((IFCAN *)(dev->dev_icb))->can_txfree(dev)) {
        (((IFCAN *)(dev->dev_icb))->can_send)(dev, frame);
        return 0;
    }
    return 1;
}

uint8_t CAN_TxFree(NUTDEVICE *dev)
{
    return ((IFCAN *)(dev->dev_icb))->can_txfree(dev);
}

uint8_t CAN_RxFrame(NUTDEVICE *dev, CANFRAME *frame)
{
  return (((IFCAN *)(dev->dev_icb))->can_recv)(dev, frame);
}

uint8_t CAN_TryRxFrame(NUTDEVICE *dev, CANFRAME *frame)
{
    if (((IFCAN *)(dev->dev_icb))->can_rxavail(dev)) {
        (((IFCAN *)(dev->dev_icb))->can_recv)(dev, frame);
        return 0;
    }
    return 1;
}

uint8_t CAN_RxAvail(NUTDEVICE *dev)
{
    return ((IFCAN *)(dev->dev_icb))->can_rxavail(dev);
}

/*!
 * Sets the CAN receive timeout
 *
 * \param dev Pointer to the device structure
 * \param timeout Timeout in milliseconds, NUT_WAIT_INFINITE = no time-out
 
 * \warning Timeout values are given in milliseconds and are limited to 
 *          the granularity of the system timer. To disable timeout,
 *          set the parameter to NUT_WAIT_INFINITE.
 */
void CAN_SetRxTimeout(NUTDEVICE *dev, uint32_t timeout)
{
    ((IFCAN *)(dev->dev_icb))->can_rtimeout = timeout;
}


#else

void keep_icc_happy(void)
{
}

#endif
/*@}*/

