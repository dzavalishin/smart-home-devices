/*
 * Copyright (c) 2005-2009 proconX Pty Ltd. All rights reserved.
 *
 * This driver has been closely modeled after the existing Nut/OS SJA1000
 * driver and the buffer management and some code snippets have been borrowed
 * from sja1000.c.
 *
 * Portions Copyright (C) 2004 by Ole Reinhardt <ole.reinhardt@kernelconcepts.de>,
 * Kernelconcepts http://www.kernelconcepts.de
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
 * Revision 1.8  2009/02/04 23:40:52  hwmaier
 * Added support for a receive buffer size configuration entry ATCAN_RX_BUF_SIZE.
 *
 * Revision 1.7  2008/08/11 06:59:14  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.6  2007/11/16 03:17:26  hwmaier
 * Updated bit timing values for a sample point of 87.5% and a SJW of 2. Required for maximum bus length as per CiA DS-102.
 *
 * Revision 1.5  2007/09/08 03:00:52  hwmaier
 * Optional time-out for receiving added, more robust handling of error interrupts.
 *
 * Revision 1.3  2007/05/24 00:55:43  henrik
 * no message
 *
 * Revision 1.2  2007/03/11 08:59:47  henrik
 * no message
 *
 * Revision 1.1  2006/11/17 01:44:00  henrik
 * no message
 *
 * Revision 1.4  2006/03/01 02:15:29  hwmaier
 * Error check for NutHeapAlloc added
 *
 * Revision 1.3  2005/12/04 10:53:59  hwmaier
 * no message
 *
 * Revision 1.2  2005/10/07 22:02:33  hwmaier
 * no message
 *
 * Revision 1.1  2005/10/04 06:00:59  hwmaier
 * Added AT90CAN128 CAN driver
 *
 */

/*!
 * \addtogroup xgCanAvr
 */
/*@{*/

#ifdef __GNUC__

#include <cfg/arch/avr.h>
#include <string.h>
#include <sys/event.h>
#include <sys/heap.h>
#include <sys/atom.h>
#include <dev/irqreg.h>
#include <dev/can_dev.h>
#include <avr/io.h>
#include <dev/atcan.h>


/*****************************************************************************
 * Definitions
 *****************************************************************************/

#define RX_MOB 8

#ifndef ATCAN_RX_BUF_SIZE
#  define ATCAN_RX_BUF_SIZE 64
#endif


/*****************************************************************************
 * CAN baud rate table
 *
 * Tried to match as closely as possible a sampling point of 87.5 %
 * which is recommended by CiA for CANOpen. DeviceNet specifies a
 * sampling point > 80%. SQW is 2.
 *****************************************************************************/

//
// 8.00 MHz
//
#if NUT_CPU_FREQ == 8000000
// 100 kbit/s 86.67% 15 quanta (BRP=7 Tprs=8 Tph1=4 Tph2=2 Tsjw=2)
#define CAN_BT1_100K 0x08
#define CAN_BT2_100K 0x0C
#define CAN_BT3_100K 0x37
// 125 kbit/s 87.50% 16 quanta (BRP=5 Tprs=8 Tph1=5 Tph2=2 Tsjw=2)
#define CAN_BT1_125K 0x0E
#define CAN_BT2_125K 0x04
#define CAN_BT3_125K 0x13
// 250 kbit/s 87.50% 16 quanta (BRP=2 Tprs=8 Tph1=5 Tph2=2 Tsjw=2)
#define CAN_BT1_250K 0x06
#define CAN_BT2_250K 0x04
#define CAN_BT3_250K 0x13
// 500 kbit/s 83.33% 12 quanta (BRP=1 Tprs=7 Tph1=2 Tph2=2 Tsjw=2)
#define CAN_BT1_500K 0x02
#define CAN_BT2_500K 0x0C
#define CAN_BT3_500K 0x37
// 1000 kbit/s 83.33% 12 quanta (BRP=0 Tprs=7 Tph1=2 Tph2=2 Tsjw=2)
#define CAN_BT1_1M 0x00
#define CAN_BT2_1M 0x04
#define CAN_BT3_1M 0x12

//
// 12.00 MHz
//
#elif NUT_CPU_FREQ == 12000000
// 10 kbit/s 85.00% 20 quanta (BRP=59 Tprs=8 Tph1=8 Tph2=3 Tsjw=2)
#define CAN_BT1_10K 0x76
#define CAN_BT2_10K 0x2E
#define CAN_BT3_10K 0x2E
// 20 kbit/s 86.67% 15 quanta (BRP=39 Tprs=8 Tph1=4 Tph2=2 Tsjw=2)
#define CAN_BT1_20K 0x4E
#define CAN_BT2_20K 0x2E
#define CAN_BT3_20K 0x16
// 50 kbit/s 87.50% 16 quanta (BRP=14 Tprs=8 Tph1=5 Tph2=2 Tsjw=2)
#define CAN_BT1_50K 0x1C
#define CAN_BT2_50K 0x2E
#define CAN_BT3_50K 0x18
// 100 kbit/s 86.67% 15 quanta (BRP=7 Tprs=8 Tph1=4 Tph2=2 Tsjw=2)
#define CAN_BT1_100K 0x0E
#define CAN_BT2_100K 0x2E
#define CAN_BT3_100K 0x16
// 125 kbit/s 87.50% 16 quanta (BRP=5 Tprs=8 Tph1=5 Tph2=2 Tsjw=2)
#define CAN_BT1_125K 0x0A
#define CAN_BT2_125K 0x2E
#define CAN_BT3_125K 0x18
// 250 kbit/s 87.50% 16 quanta (BRP=2 Tprs=8 Tph1=5 Tph2=2 Tsjw=2)
#define CAN_BT1_250K 0x04
#define CAN_BT2_250K 0x2E
#define CAN_BT3_250K 0x18
// 500 kbit/s 83.33% 12 quanta (BRP=1 Tprs=7 Tph1=2 Tph2=2 Tsjw=2)
#define CAN_BT1_500K 0x02
#define CAN_BT2_500K 0x2C
#define CAN_BT3_500K 0x12
// 800 kbit/s 86.67% 15 quanta (BRP=0 Tprs=8 Tph1=4 Tph2=2 Tsjw=2)
#define CAN_BT1_800K 0x00
#define CAN_BT2_800K 0x2E
#define CAN_BT3_800K 0x16
// 1000 kbit/s 83.33% 12 quanta (BRP=0 Tprs=7 Tph1=2 Tph2=2 Tsjw=2)
#define CAN_BT1_1M 0x00
#define CAN_BT2_1M 0x2C
#define CAN_BT3_1M 0x12

//
// 16.00 MHz
//
#elif NUT_CPU_FREQ == 16000000
// 10 kbit/s 68.00% 25 quanta (BRP=63 Tprs=8 Tph1=8 Tph2=8 Tsjw=2)
#define CAN_BT1_10K 0x7E
#define CAN_BT2_10K 0x2E
#define CAN_BT3_10K 0x7E
// 20 kbit/s 87.50% 16 quanta (BRP=49 Tprs=8 Tph1=5 Tph2=2 Tsjw=2)
#define CAN_BT1_20K 0x62
#define CAN_BT2_20K 0x2E
#define CAN_BT3_20K 0x18
// 50 kbit/s 87.50% 16 quanta (BRP=19 Tprs=8 Tph1=5 Tph2=2 Tsjw=2)
#define CAN_BT1_50K 0x26
#define CAN_BT2_50K 0x2E
#define CAN_BT3_50K 0x18
// 100 kbit/s 87.50% 16 quanta (BRP=9 Tprs=8 Tph1=5 Tph2=2 Tsjw=2)
#define CAN_BT1_100K 0x12
#define CAN_BT2_100K 0x2E
#define CAN_BT3_100K 0x18
// 125 kbit/s 87.50% 16 quanta (BRP=7 Tprs=8 Tph1=5 Tph2=2 Tsjw=2)
#define CAN_BT1_125K 0x0E
#define CAN_BT2_125K 0x2E
#define CAN_BT3_125K 0x18
// 250 kbit/s 87.50% 16 quanta (BRP=3 Tprs=8 Tph1=5 Tph2=2 Tsjw=2)
#define CAN_BT1_250K 0x06
#define CAN_BT2_250K 0x2E
#define CAN_BT3_250K 0x18
// 500 kbit/s 87.50% 16 quanta (BRP=1 Tprs=8 Tph1=5 Tph2=2 Tsjw=2)
#define CAN_BT1_500K 0x02
#define CAN_BT2_500K 0x2E
#define CAN_BT3_500K 0x18
// 800 kbit/s 85.00% 20 quanta (BRP=0 Tprs=8 Tph1=8 Tph2=3 Tsjw=2)
#define CAN_BT1_800K 0x00
#define CAN_BT2_800K 0x2E
#define CAN_BT3_800K 0x2E
// 1000 kbit/s 87.50% 16 quanta (BRP=0 Tprs=8 Tph1=5 Tph2=2 Tsjw=2)
#define CAN_BT1_1M 0x00
#define CAN_BT2_1M 0x2E
#define CAN_BT3_1M 0x18
#else
#  error Frequency not supported or not set to a Fixed MCU clock!
#endif


/*****************************************************************************
 * Buffer management
 *
 * Note: Buffer concept borrowed from existing Nut/OS sja1000.c driver
 *****************************************************************************/

struct _CANBuffer {
    CANFRAME *dataptr;    // the physical memory address where the buffer is stored
    uint8_t size;         // the allocated size of the buffer
    uint8_t datalength;   // the length of the data currently in the buffer
    uint8_t dataindex;    // the index into the buffer where the data starts
};

typedef struct _CANBuffer CANBuffer;


/*****************************************************************************
 * Driver data
 *****************************************************************************/

static CANBuffer canRxBuf;
CANINFO dcb_atcan;
IFCAN ifc_atcan;
NUTDEVICE devAtCan;


/*****************************************************************************
 * Low level functions
 *****************************************************************************/

/**
 * Returns the state of this CAN node.
 * Refer to state diagram in AT90CAN128 dataheet chapter 20.7 "Error
 * Management"
 *
 * @retval CAN_SUCCESS if CAN node is in ACTIVE state
 * @retval CAN_PASSIVE if CAN node is in PASSIVE state
 * @retval CAN_BUS_OFF if CAN node is in BUS OFF state
 */
int8_t AtCanGetBusState(void)
{
   if (CANGSTA & _BV(BOFF))
       return CAN_BUS_OFF;
   if (CANGSTA & _BV(ERRP))
       return CAN_PASSIVE;
   return CAN_SUCCESS;
}


/**
 * @internal
 * Search for a free message object
 * @return Index of free message object or -1 if none is available
 */
int8_t AtCanGetFreeMob(void)
{
   int8_t mob;
   uint8_t ctrlReg;

   for (mob = 0; mob < ATCAN_MAX_MOB; mob++)
   {
      CANPAGE = mob << 4;
      ctrlReg = CANCDMOB & (_BV(CONMOB1) | _BV(CONMOB0));
      if (ctrlReg == 0)
         return mob;
      if (ctrlReg == _BV(CONMOB0) && (bit_is_set(CANSTMOB, TXOK)))
      {
          // Free MOB
          CANCDMOB &= ~(_BV(CONMOB1) | _BV(CONMOB0));
          return mob;
      }
   }
   return -1;
}


/**
 * Configure a single message object for receiption.
 *
 * All 15 MOBS can be set up for reception,  not only RX_MOB MOBs
 * 
 * @param mob        Message object index (0-14)
 * @param id         Acceptance code
 * @param idIsExt    Flag if acceptance code is extended (0 = standard, 1 = extended)
 * @param idRemTag   Id's remote tag (0 or 1)
 * @param mask       Acceptance mask
 * @param maskIsExt  Flag if acceptance mask is extended (0 = standard, 1 = extended)
 * @param maskRemTag Mask's remote tag (0 or 1)
 * @return Result code. See @ref CAN_RESULT
 */
int8_t AtCanEnableMsgObj(uint8_t mob,
                         uint32_t id, int8_t idIsExt, int8_t idRemTag,
                         uint32_t mask, int8_t maskIsExt, int8_t maskRemTag)
{
    if (mob < ATCAN_MAX_MOB)
    {
        // Select MOb
        CANPAGE = mob << 4;
        // Abort MOb
        CANCDMOB = 0;
        // Set identifier and mask
        if (idIsExt)
        {
            CANCDMOB |= _BV(IDE);
            CANIDT1 = (((uint8_t *) &(id))[3] << 3) + (((uint8_t *) &(id))[2] >> 5);
            CANIDT2 = (((uint8_t *) &(id))[2] << 3) + (((uint8_t *) &(id))[1] >> 5);
            CANIDT3 = (((uint8_t *) &(id))[1] << 3) + (((uint8_t *) &(id))[0] >> 5);
            CANIDT4 = (((uint8_t *) &(id))[0] << 3);
            CANIDM1 = (((uint8_t *) &(mask))[3] << 3) + (((uint8_t *) &(mask))[2] >> 5);
            CANIDM2 = (((uint8_t *) &(mask))[2] << 3) + (((uint8_t *) &(mask))[1] >> 5);
            CANIDM3 = (((uint8_t *) &(mask))[1] << 3) + (((uint8_t *) &(mask))[0] >> 5);
            CANIDM4 = (((uint8_t *) &(mask))[0] << 3);
        }
        else
        {
            CANIDT1 = (((uint8_t *) &(id))[1] << 5) + (((uint8_t *) &(id))[0] >> 3);
            CANIDT2 = (((uint8_t *) &(id))[0] << 5);
            CANIDT3 = 0;
            CANIDT4 = 0;
            CANIDM1 = (((uint8_t *) &(mask))[1] << 5) + (((uint8_t *) &(mask))[0] >> 3);
            CANIDM2 = (((uint8_t *) &(mask))[0] << 5);
            CANIDM3 = 0;
            CANIDM4 = 0;
        }
        if (idRemTag)
            CANIDT4 |= _BV(RTRTAG);
        if (maskIsExt)
            CANIDM4 |= _BV(IDEMSK);
        if (maskRemTag)
            CANIDM4 |= _BV(RTRMSK);
        // Enable reception
        CANCDMOB |= _BV(CONMOB1);
        return CAN_SUCCESS;
    }
    else
        return CAN_ILLEGAL_MOB;
}


/**
 * Configure message objects for receiption.
 *
 * @param noOfMsgObjs Number of message objects used for receiption (1-14).
 *                    Message objects not configured for receiption are used for
 *                    transmission.
 *                    The same acceptance filter and mask is applied to the
 *                    message objects.
 *                    If it is required to set-up more complex
 *                    filtering mechanisms use canEnableMsgObj() instead, which
 *                    allows setting filters on a per message object basis.
 * @param id          Acceptance code
 * @param idIsExt     Flag if acceptance code is extended (0 = standard, 1 = extended)
 * @param idRemTag    Id's remote tag (0 or 1)
 * @param mask        Acceptance mask
 * @param maskIsExt   Flag if acceptance mask is extended (0 = standard, 1 = extended)
 * @param maskRemTag  Mask's remote tag (0 or 1)
 * @return Result code. See @ref CAN_RESULT
 */
int8_t AtCanEnableRx(uint8_t noOfMsgObjs,
                     uint32_t id, int8_t idIsExt, int8_t idRemTag,
                     uint32_t mask, int8_t maskIsExt, int8_t maskRemTag)
{
    int8_t i;
    int8_t result;

    for (i = 0; i < noOfMsgObjs; i++)
    {
        result = AtCanEnableMsgObj(i, id, idIsExt, idRemTag, mask, maskIsExt, maskRemTag);
        if (result != CAN_SUCCESS)
            return result;
    }
    return CAN_SUCCESS;
}


/**
 * Send a CAN message
 *
 * @param frame Container for CAN message to be sent
 * @return Result code. See @ref CAN_RESULT
 */
int8_t AtCanSendMsg(CANFRAME *frame)
{
    signed char mob, j;

    mob = AtCanGetFreeMob();
    if (mob < 0)
        return CAN_TXBUF_FULL;
    // Select MOB
    CANPAGE = mob << 4;
    // Abort MOB and set length fields
    CANCDMOB = frame->len;
    // Check for ID type
    if (frame->ext)
    {
        CANIDT1 = (((uint8_t *) &(frame->id))[3] << 3) + (((uint8_t *) &(frame->id))[2] >> 5);
        CANIDT2 = (((uint8_t *) &(frame->id))[2] << 3) + (((uint8_t *) &(frame->id))[1] >> 5);
        CANIDT3 = (((uint8_t *) &(frame->id))[1] << 3) + (((uint8_t *) &(frame->id))[0] >> 5);
        CANIDT4 = (((uint8_t *) &(frame->id))[0] << 3);
        CANCDMOB |= _BV(IDE);
    }
    else
    {
        CANIDT1 = (((uint8_t *) &(frame->id))[1] << 5) + (((uint8_t *) &(frame->id))[0] >> 3);
        CANIDT2 = (((uint8_t *) &(frame->id))[0] << 5);
        CANIDT3 = 0;
        CANIDT4 = 0;
    }
    if (frame->rtr)
        CANIDT4 |= _BV(RTRTAG);
    for (j = 0; j < 8; j++)
        CANMSG = frame->byte[j];
    // Enable transmission
    CANCDMOB |= _BV(CONMOB0);

    return CAN_SUCCESS;
}


/*!
 * AtCan interrupt service routine
 */
static void AtCanInterrupt(void *arg)
{
    uint8_t savedCanPage;
    CANINFO *ci = (CANINFO *) (((NUTDEVICE *) arg)->dev_dcb);

    savedCanPage = CANPAGE; // Save the current CAN page
    ci->can_interrupts++;

    //
    // Check for MOb interrupt
    //
    if ((CANHPMOB & 0xF0) != 0xF0)
    {
        CANPAGE = CANHPMOB; // Switch page
        // RX interrupt ?
        if (bit_is_set(CANSTMOB, RXOK))
        {
            // Space in buffer ?
            if (canRxBuf.datalength < canRxBuf.size)
            {
                int8_t j;
                CANFRAME * bufPtr = &canRxBuf.dataptr[(canRxBuf.dataindex + canRxBuf.datalength) % canRxBuf.size];

                // Extended or standard ID?
                bufPtr->ext = bit_is_set(CANCDMOB, IDE);
                if (bufPtr->ext)
                {
                    ((uint8_t *) &(bufPtr->id))[3] = CANIDT1 >> 3;
                    ((uint8_t *) &(bufPtr->id))[2] = (CANIDT2 >> 3) | (CANIDT1 << 5);
                    ((uint8_t *) &(bufPtr->id))[1] = (CANIDT3 >> 3) | (CANIDT2 << 5);
                    ((uint8_t *) &(bufPtr->id))[0] = (CANIDT4 >> 3) | (CANIDT3 << 5);
                }
                else
                {
                    ((uint8_t *) &(bufPtr->id))[3] = 0;
                    ((uint8_t *) &(bufPtr->id))[2] = 0;
                    ((uint8_t *) &(bufPtr->id))[1] = CANIDT1 >> 5;
                    ((uint8_t *) &(bufPtr->id))[0] = (CANIDT1 << 3) | (CANIDT2 >> 5);
                }
                bufPtr->len = CANCDMOB & (_BV(DLC0) | _BV(DLC1) | _BV(DLC2) | _BV(DLC3));
                bufPtr->rtr = bit_is_set(CANIDT4, RTRTAG);
                for (j = 0; j < 8; j++)
                    bufPtr->byte[j] = CANMSG;
                // Increment buffer length
                canRxBuf.datalength++;
                NutEventPostFromIrq(&ci->can_rx_rdy);
                // Stat houskeeping
                ci->can_rx_frames++;
            }
            else
            {
                // Stat houskeeping
                ci->can_overruns++;
            }
        }
        // TX interrupt ?
        else if (bit_is_set(CANSTMOB, TXOK))
        {
            NutEventPostFromIrq(&ci->can_tx_rdy);
            ci->can_tx_frames++;
        }
        // Error interrupts ?
        else
        {
           // Stat houskeeping
           ci->can_errors++;
        }

        //
        // Acknowledge all MOB interrupts and manage MObs
        //
        CANSTMOB = CANSTMOB & ~(_BV(TXOK) | _BV(RXOK) | _BV(BERR) |
                                _BV(SERR) | _BV(CERR) | _BV(FERR) | _BV(AERR));

        if (bit_is_set(CANCDMOB, CONMOB1))
           // Re-enable MOb for reception by re-writing CONMOB1 bit
           CANCDMOB |= _BV(CONMOB1);
        else
           // Re-claim MOb as send buffer
           CANCDMOB &= ~(_BV(CONMOB1) | _BV(CONMOB0));

    }
    else
    {
        // Acknowledge general CAN interrupt
        CANGIT = _BV(BOFFIT) | _BV(BXOK) | _BV(SERG) | _BV(CERG) | _BV(FERG) | _BV(AERG);
        // Stat houskeeping
        ci->can_errors++;
    }
    CANPAGE = savedCanPage; // Restore CAN page register
}


/*****************************************************************************
 * Function definitions
 *****************************************************************************/

/*!
 * Checks if data is available in input buffer
 *
 * \param dev Pointer to the device structure
 * \return Number of frames available
 */
uint8_t AtCanRxAvail(NUTDEVICE * dev)
{
    return canRxBuf.datalength;
}


/*!
 * Checks if there's still space in output buffer
 *
 * \param dev Pointer to the device structure
 * \return 1 if space is available
 */
uint8_t AtCanTxFree(NUTDEVICE * dev)
{
    return (AtCanGetFreeMob() >= 0);
}


/*!
 * Write a frame from to output buffer
 *
 * This function writes a frame to the output buffer. If the output buffer
 * is full the function will block until frames are send.
 *
 * \param dev Pointer to the device structure
 * \param frame Pointer to the receive frame
 */
void AtCanOutput(NUTDEVICE * dev, CANFRAME * frame)
{
    CANINFO * ci = (CANINFO *) dev->dev_dcb;

    while (AtCanSendMsg(frame) == CAN_TXBUF_FULL)
    {
        NutEventWait(&ci->can_tx_rdy, NUT_WAIT_INFINITE);
    };
}


/*!
 * Reads a frame from input buffer
 *
 * This function reads a frame from the input buffer. If the input buffer
 * is empty the function will block unitl new frames are received,
 * or the timeout is reached.
 *
 * \param dev Pointer to the device structure
 * \param frame Pointer to the receive frame
 * \return 1 if timeout, 0 otherwise
 */
uint8_t AtCanInput(NUTDEVICE * dev, CANFRAME * frame)
{
    CANINFO * ci = (CANINFO *) dev->dev_dcb;

    while (canRxBuf.datalength == 0)
    {
        uint32_t timeout =  ((IFCAN *) (dev->dev_icb))->can_rtimeout;

        if (NutEventWait(&ci->can_rx_rdy, timeout))
            return 1;
    }
    NutEnterCritical();
    // Get the first frame from buffer
    *frame = canRxBuf.dataptr[canRxBuf.dataindex];
    // Move index down and decrement length
    canRxBuf.dataindex++;
    if (canRxBuf.dataindex >= canRxBuf.size)
        canRxBuf.dataindex %= canRxBuf.size;
    canRxBuf.datalength--;
    NutExitCritical();

    return 0;
}


/*!
 * Sets the acceptance code
 *
 * \param dev Pointer to the device structure
 * \param ac 4 byte char array with the acceptance code
 */
void AtCanSetAccCode(NUTDEVICE * dev, uint8_t * ac)
{
    memcpy(((IFCAN *) (dev->dev_icb))->can_acc_code, ac, 4);
    AtCanEnableRx(RX_MOB, 0, 0, 0, 0, 0, 0); //ttt   TODO: Implement it!
}


/*!
 * Sets the acceptance mask
 *
 * \param dev Pointer to the device structure
 * \param am 4 byte char array with the acceptance mask
 */
void AtCanSetAccMask(NUTDEVICE * dev, uint8_t * am)
{
    memcpy(((IFCAN *) (dev->dev_icb))->can_acc_mask, am, 4);
    AtCanEnableRx(RX_MOB, 0, 0, 0, 0, 0, 0); //ttt   TODO: Implement it!
}


/*!
 * Sets the CAN baud rate
 *
 * \param dev Pointer to the device structure
 * \param baudrate Baud rate (One of the defined baud rates. See AtCan.h)
 * \return 0 for success
 */
uint8_t AtCanSetBaudrate(NUTDEVICE * dev, uint32_t baudrate)
{
    switch (baudrate)
    {
#if NUT_CPU_FREQ != 8000000
        case CAN_SPEED_10K:
            CANBT1 = CAN_BT1_10K;
            CANBT2 = CAN_BT2_10K;
            CANBT3 = CAN_BT3_10K;
        break;
        case CAN_SPEED_20K:
            CANBT1 = CAN_BT1_20K;
            CANBT2 = CAN_BT2_20K;
            CANBT3 = CAN_BT3_20K;
        break;
        case CAN_SPEED_50K:
            CANBT1 = CAN_BT1_50K;
            CANBT2 = CAN_BT2_50K;
            CANBT3 = CAN_BT3_50K;
        break;
#endif
        case CAN_SPEED_100K:
            CANBT1 = CAN_BT1_100K;
            CANBT2 = CAN_BT2_100K;
            CANBT3 = CAN_BT3_100K;
        break;
        case CAN_SPEED_125K:
            CANBT1 = CAN_BT1_125K;
            CANBT2 = CAN_BT2_125K;
            CANBT3 = CAN_BT3_125K;
        break;
        case CAN_SPEED_250K:
            CANBT1 = CAN_BT1_250K;
            CANBT2 = CAN_BT2_250K;
            CANBT3 = CAN_BT3_250K;
        break;
        case CAN_SPEED_500K:
            CANBT1 = CAN_BT1_500K;
            CANBT2 = CAN_BT2_500K;
            CANBT3 = CAN_BT3_500K;
        break;
#if NUT_CPU_FREQ != 8000000
        case CAN_SPEED_800K:
            CANBT1 = CAN_BT1_800K;
            CANBT2 = CAN_BT2_800K;
            CANBT3 = CAN_BT3_800K;
        break;
#endif
        case CAN_SPEED_1M:
            CANBT1 = CAN_BT1_1M;
            CANBT2 = CAN_BT2_1M;
            CANBT3 = CAN_BT3_1M;
        break;
        case CAN_SPEED_CUSTOM:
            // Do nothing, user sets baud rate directly but don't report an error
        break;
        default:
        return 1;
    }
    ((IFCAN *) (dev->dev_icb))->can_baudrate = baudrate;
    return 0;
}


/*!
 * Initialize CAN interface.
 *
 * Applications typically do not use this function, but
 * call NutRegisterDevice().
 *
 * \param dev Identifies the device to initialize. The
 *            structure must be properly set.
 * \return 0 for successful initialisation or -1 in case init failed
 */
int AtCanInit(NUTDEVICE * dev)
{
    int8_t mob, i;

    memset(dev->dev_dcb, 0, sizeof(CANINFO));

    // Init receive buffer
    canRxBuf.dataptr = NutHeapAlloc(ATCAN_RX_BUF_SIZE * sizeof(CANFRAME));
    if (canRxBuf.dataptr == 0)
        return -1;
    canRxBuf.size = ATCAN_RX_BUF_SIZE;
    canRxBuf.dataindex = 0;
    canRxBuf.datalength = 0;

    // Disable AT90CAN128 CAN system
    CANGCON &= ~_BV(ENASTB);
    loop_until_bit_is_clear(CANGSTA, ENFG);

    // Clear all MObs
    for (mob = 0; mob < ATCAN_MAX_MOB; mob++)
    {
        CANPAGE = mob << 4;
        CANSTMOB = 0; // Clear status register
        CANCDMOB = 0; // Clear control register
        CANHPMOB = 0; // Clear HP flags
        // Clear identifier tag
        CANIDT1 = 0;
        CANIDT2 = 0;
        CANIDT3 = 0;
        CANIDT4 = 0;
        // Clear identifier mask
        CANIDM1 = 0;
        CANIDM2 = 0;
        CANIDM3 = 0;
        CANIDM4 = 0;
        for (i = 0; i < 8; i++)
        {
            CANMSG = 0;
        }
    }
    // Set baudrate
    AtCanSetBaudrate(dev, ifc_atcan.can_baudrate); // Errors silently ingnored here
    // Install IRQ handler
    if (NutRegisterIrqHandler(&sig_CAN_TRANSFER, AtCanInterrupt, dev))
        return -1;
    // Enable all interrupts
    CANIE1 = 0x7F;
    CANIE2 = 0xFF;
    CANGIE = 0xFE; // All interrupts except OVRTIM
    // Enable receiving MOBs
    AtCanEnableRx(RX_MOB, 0, 0, 0, 0, 0, 0);

    // Enable AT90CAN128 CAN system
    CANGCON |= _BV(ENASTB);
    loop_until_bit_is_set(CANGSTA, ENFG);

    return 0;
}


/*!
 * Interface information structure.
 *
 * This structure stores some interface parameters like bit rate,
 * acceptance mask, acceptance code and callback handlers.
 */
IFCAN ifc_atcan = {
    CAN_IF_2B,                 /*!< \brief Interface type. */
    CAN_SPEED_125K,            /*!< \brief Baudrate of device. */
    {0xFF, 0xFF, 0xFF, 0xFF},  /*!< \brief Acceptance mask */
    {0x00, 0x00, 0x00, 0x00},  /*!< \brief Acceptance code */
    NUT_WAIT_INFINITE,         /*!< \brief Receive time-out */
    AtCanRxAvail,              /*!< \brief Data in RxBuffer available? */
    AtCanTxFree,               /*!< \brief TxBuffer free? */
    AtCanInput,                /*!< \brief CAN Input routine */
    AtCanOutput,               /*!< \brief CAN Output routine */
    AtCanSetAccCode,           /*!< \brief Set acceptance code */
    AtCanSetAccMask,           /*!< \brief Set acceptance mask */
    AtCanSetBaudrate,          /*!< \brief Set baudrate */
};


/*!
 * Device information structure.
 *
 * Applications must pass this structure to NutRegisterDevice()
 * to bind this CAN device driver to the Nut/OS kernel.
 */
NUTDEVICE devAtCan = {
    0,                          /*!< Pointer to next device. */
    {'a', 't', 'c', 'a', 'n', '0', 0, 0, 0}, /*!< Unique device name. */
    IFTYP_CAN,                  /*!< Type of device. */
    0,                          /*!< Base address. */
    0,                          /*!< First interrupt number. */
    &ifc_atcan,                 /*!< Interface control block. */
    &dcb_atcan,                 /*!< Driver control block. */
    AtCanInit,                  /*!< Driver initialization routine. */
    0,                          /*!< Driver specific control function. */
    0,                          /*!< Read from device. */
    0,                          /*!< Write to device. */
    0,                          /*!< Write from program space data to device. */
    0,                          /*!< Open a device or file. */
    0,                          /*!< Close a device or file. */
    0                           /*!< Request file size. */
};

#else

static void keep_icc_happy(void)
{
}

#endif

/*@}*/

