/*!
 * Copyright (C) 2007 by egnite Software GmbH. All rights reserved.
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

/*!
 * \file avrtarget.c
 * \brief External AVR device programming.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.3  2009/01/17 11:26:46  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.2  2008/08/11 06:59:41  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.1  2007/04/12 09:01:38  haraldkipp
 * New API allows to program external AVR devices.
 *
 *
 * \endverbatim
 */

#include <sys/timer.h>
#include <dev/avrtarget.h>

/*!
 * \addtogroup xgAvrTarget
 */
/*@{*/

#ifndef AVRTARGET_CHIPERASE_TIMEOUT
/*!
 * \brief Maximum time (in milliseconds) to wait for chip erase done.
 */
#define AVRTARGET_CHIPERASE_TIMEOUT 200
#endif

#ifndef AVRTARGET_PAGEWRITE_TIMEOUT
/*!
 * \brief Maximum time (in milliseconds) to wait for page write done.
 */
#define AVRTARGET_PAGEWRITE_TIMEOUT 10
#endif

/*!
 * \brief Safe fuse programming mask.
 *
 * If a bit is set in this mask, the corresponding fuse should
 * never be programmed to 0.
 *
 * Used in AvrTargetFusesWriteSafe() to avoid dead chips. However,
 * it is still possible to break hardware. Triple check any new
 * fuse configuration.
 *
 * This unsigned long value is interpreted as follows:
 *
 * - Least significant byte is fuse low byte.
 * - Second byte is fuse high byte.
 * - Third byte is fuse extended byte.
 * - Most significant byte is not used.
 */
#define AVR_FUSES_NEVER_PROG     0x00F88062UL

/*!
 * \brief Safe fuse unprogramming mask.
 *
 * If a bit is set in this mask, the corresponding fuse should
 * never be left unprogrammed (set to 1).
 *
 * Used in AvrTargetFusesWriteSafe() to avoid dead chips. However,
 * it is still possible to break hardware. Triple check any new
 * fuse configuration.
 *
 * This unsigned long value is interpreted as follows:
 *
 * - Least significant byte is fuse low byte.
 * - Second byte is fuse high byte.
 * - Third byte is fuse extended byte.
 * - Most significant byte is not used.
 */
#define AVR_FUSES_ALWAYS_PROG   0x0000201DUL

#ifndef AVRTARGET_SPI_MODE
#define AVRTARGET_SPI_MODE   0
#endif

#ifndef AVRTARGET_SPI_RATE
#define AVRTARGET_SPI_RATE   500000
#endif

#if defined(AVRTARGET_SPI0_DEVICE) /* SPI device. */

#include <dev/sppif0.h>
#if defined(AVRTARGET_SELECT_ACTIVE_HIGH)
#define TargetReset(act)    Sppi0ChipReset(AVRTARGET_SPI0_DEVICE, act)
#else
#define TargetReset(act)    Sppi0ChipReset(AVRTARGET_SPI0_DEVICE, !act)
#endif
#define SpiSetMode()        Sppi0SetMode(AVRTARGET_SPI0_DEVICE, AVRTARGET_SPI_MODE)
#define SpiSetSpeed()       Sppi0SetSpeed(AVRTARGET_SPI0_DEVICE, AVRTARGET_SPI_RATE)
#if defined(AVRTARGET_SELECT_ACTIVE_HIGH)
#define SpiSelect()         Sppi0SelectDevice(AVRTARGET_SPI0_DEVICE)
#define SpiDeselect()       Sppi0DeselectDevice(AVRTARGET_SPI0_DEVICE)
#else
#define SpiSelect()         Sppi0NegSelectDevice(AVRTARGET_SPI0_DEVICE)
#define SpiDeselect()       Sppi0NegDeselectDevice(AVRTARGET_SPI0_DEVICE)
#endif
#define SpiByte             Sppi0Byte

#elif defined(AVRTARGET_SBBI0_DEVICE) /* SPI device. */

#include <dev/sbbif0.h>
#if defined(AVRTARGET_SELECT_ACTIVE_HIGH)
#define TargetReset(act)    Sbbi0ChipReset(AVRTARGET_SBBI0_DEVICE, act)
#else
#define TargetReset(act)    Sbbi0ChipReset(AVRTARGET_SBBI0_DEVICE, !act)
#endif
#define SpiSetMode()        Sbbi0SetMode(AVRTARGET_SBBI0_DEVICE, AVRTARGET_SPI_MODE)
#define SpiSetSpeed()       Sbbi0SetSpeed(AVRTARGET_SBBI0_DEVICE, AVRTARGET_SPI_RATE)
#if defined(AVRTARGET_SELECT_ACTIVE_HIGH)
#define SpiSelect()         Sbbi0SelectDevice(AVRTARGET_SBBI0_DEVICE)
#define SpiDeselect()       Sbbi0DeselectDevice(AVRTARGET_SBBI0_DEVICE)
#else
#define SpiSelect()         Sbbi0NegSelectDevice(AVRTARGET_SBBI0_DEVICE)
#define SpiDeselect()       Sbbi0NegDeselectDevice(AVRTARGET_SBBI0_DEVICE)
#endif
#define SpiByte             Sbbi0Byte

#elif defined(AVRTARGET_SBBI1_DEVICE) /* SPI device. */

#include <dev/sbbif1.h>
#if defined(AVRTARGET_SELECT_ACTIVE_HIGH)
#define TargetReset(act)    Sbbi1ChipReset(AVRTARGET_SBBI1_DEVICE, act)
#else
#define TargetReset(act)    Sbbi1ChipReset(AVRTARGET_SBBI1_DEVICE, !act)
#endif
#define SpiSetMode()        Sbbi1SetMode(AVRTARGET_SBBI1_DEVICE, AVRTARGET_SPI_MODE)
#define SpiSetSpeed()       Sbbi1SetSpeed(AVRTARGET_SBBI1_DEVICE, AVRTARGET_SPI_RATE)
#if defined(AVRTARGET_SELECT_ACTIVE_HIGH)
#define SpiSelect()         Sbbi1SelectDevice(AVRTARGET_SBBI1_DEVICE)
#define SpiDeselect()       Sbbi1DeselectDevice(AVRTARGET_SBBI1_DEVICE)
#else
#define SpiSelect()         Sbbi1NegSelectDevice(AVRTARGET_SBBI1_DEVICE)
#define SpiDeselect()       Sbbi1NegDeselectDevice(AVRTARGET_SBBI1_DEVICE)
#endif
#define SpiByte             Sbbi1Byte

#elif defined(AVRTARGET_SBBI2_DEVICE) /* SPI device. */

#include <dev/sbbif2.h>
#if defined(AVRTARGET_SELECT_ACTIVE_HIGH)
#define TargetReset(act)    Sbbi2ChipReset(AVRTARGET_SBBI2_DEVICE, act)
#else
#define TargetReset(act)    Sbbi2ChipReset(AVRTARGET_SBBI2_DEVICE, !act)
#endif
#define SpiSetMode()        Sbbi2SetMode(AVRTARGET_SBBI2_DEVICE, AVRTARGET_SPI_MODE)
#define SpiSetSpeed()       Sbbi2SetSpeed(AVRTARGET_SBBI2_DEVICE, AVRTARGET_SPI_RATE)
#if defined(AVRTARGET_SELECT_ACTIVE_HIGH)
#define SpiSelect()         Sbbi2SelectDevice(AVRTARGET_SBBI2_DEVICE)
#define SpiDeselect()       Sbbi2DeselectDevice(AVRTARGET_SBBI2_DEVICE)
#else
#define SpiSelect()         Sbbi2NegSelectDevice(AVRTARGET_SBBI2_DEVICE)
#define SpiDeselect()       Sbbi2NegDeselectDevice(AVRTARGET_SBBI2_DEVICE)
#endif
#define SpiByte             Sbbi2Byte

#elif defined(AVRTARGET_SBBI3_DEVICE) /* SPI device. */

#include <dev/sbbif3.h>
#if defined(AVRTARGET_SELECT_ACTIVE_HIGH)
#define TargetReset(act)    Sbbi3ChipReset(AVRTARGET_SBBI3_DEVICE, act)
#else
#define TargetReset(act)    Sbbi3ChipReset(AVRTARGET_SBBI3_DEVICE, !act)
#endif
#define SpiSetMode()        Sbbi3SetMode(AVRTARGET_SBBI3_DEVICE, AVRTARGET_SPI_MODE)
#define SpiSetSpeed()       Sbbi3SetSpeed(AVRTARGET_SBBI3_DEVICE, AVRTARGET_SPI_RATE)
#if defined(AVRTARGET_SELECT_ACTIVE_HIGH)
#define SpiSelect()         Sbbi3SelectDevice(AVRTARGET_SBBI3_DEVICE)
#define SpiDeselect()       Sbbi3DeselectDevice(AVRTARGET_SBBI3_DEVICE)
#else
#define SpiSelect()         Sbbi3NegSelectDevice(AVRTARGET_SBBI3_DEVICE)
#define SpiDeselect()       Sbbi3NegDeselectDevice(AVRTARGET_SBBI3_DEVICE)
#endif
#define SpiByte             Sbbi3Byte

#else /* SPI device. */

#define AVRTARGET_NO_SPI_DEVICE

#endif /* SPI device. */

/*!
 * \brief Initialize programming interface.
 *
 * Sets the configured SPI mode and rate. This must be called before
 * using any other routine in this module.
 *
 * \return 0 on success, -1 otherwise.
 */
int AvrTargetInit(void)
{
#if defined(AVRTARGET_NO_SPI_DEVICE)
    return -1;
#else
    if (SpiSetMode()) {
        return -1;
    }
    SpiSetSpeed();
    SpiDeselect();

    return 0;
#endif
}

/*!
 * \brief Set or clear the chip select line.
 *
 * Must be called before sending programming commands to the target device,
 * even if no chip selects are implemented by the hardware. Beside activating
 * an optional chip select, this routine will also properly initialize the
 * SPI hardware.
 *
 * \param act When 0, the chip will be deactivated and the serial peripheral
 *            interface may be used to communicate with another device.
 *            When not 0, the SPI bus is allocated for the attached AVR target.
 */
void AvrTargetSelect(ureg_t act)
{
#if !defined(AVRTARGET_NO_SPI_DEVICE)
    if (act) {
        SpiSelect();
    } else {
        SpiDeselect();
    }
#endif
}

/*!
 * \brief Set or release the target's reset line.
 *
 * The target must be held in reset state during programming.
 *
 * \param on When not 0, the reset line will be activated.
 */
void AvrTargetReset(ureg_t act)
{
#if !defined(AVRTARGET_NO_SPI_DEVICE)
    TargetReset(act);
#endif
}

/*!
 * \brief Send programming command and return response.
 *
 * \param cmd 4-byte command.
 *
 * \return 4-byte response.
 */
uint32_t AvrTargetCmd(uint32_t cmd)
{
    uint32_t rc = 0;
#if !defined(AVRTARGET_NO_SPI_DEVICE)
    ureg_t i;

    for (i = 0; i < 4; i++) {
        rc <<= 8;
        rc |= SpiByte((uint8_t) (cmd >> 24));
        cmd <<= 8;
    }
#endif
    return rc;
}

/*!
 * \brief Enable target's programming mode.
 *
 * \return 0 on success, -1 otherwise.
 */
int AvrTargetProgEnable(void)
{
#if defined(AVRTARGET_NO_SPI_DEVICE)
    return -1;
#else
    if ((uint8_t) (AvrTargetCmd(AVRCMD_PROG_ENABLE) >> 8) != 0x53) {
        return -1;
    }
    return 0;
#endif
}

/*!
 * \brief Retrieve the target's signature.
 *
 * \return Signature in the 3 least significant bytes.
 */
uint32_t AvrTargetSignature(void)
{
    uint32_t rc = 0;
#if !defined(AVRTARGET_NO_SPI_DEVICE)
    ureg_t i;

    for (i = 0; i < 3; i++) {
        rc <<= 8;
        rc |= (uint8_t) AvrTargetCmd(AVRCMD_READ_SIGNATURE_BYTE | ((uint32_t) i << 8));
    }
#endif
    return rc;
}

/*!
 * \brief Retrieve the target's fuses.
 *
 * \return Fuse bits in the 3 least significant bytes.
 */
uint32_t AvrTargetFusesRead(void)
{
    uint32_t rc = 0;

    rc = (uint8_t) AvrTargetCmd(AVRCMD_READ_FUSE_EXT_BITS);
    rc <<= 8;
    rc |= (uint8_t) AvrTargetCmd(AVRCMD_READ_FUSE_HI_BITS);
    rc <<= 8;
    rc |= (uint8_t) AvrTargetCmd(AVRCMD_READ_FUSE_BITS);

    return rc;
}

/*!
 * \brief Program the target's fuses in a safe way.
 *
 * Fuses with corresponding bits set in \ref AVR_FUSES_NEVER_PROG will
 * never be programmed. Fuses with corresponding bits set in
 * \ref AVR_FUSES_ALWAYS_PROG will always be programmed.
 *
 * \param fuses Fuse bits are expected in the 3 least significant bytes,
 *              Lower fuses in the LSB, followed by the high fuses and
 *              last no least the extended fuses.
 */
uint32_t AvrTargetFusesWriteSafe(uint32_t fuses)
{
    uint32_t rc;

    fuses |= AVR_FUSES_NEVER_PROG;
    fuses &= ~AVR_FUSES_ALWAYS_PROG;
    rc = fuses;

    AvrTargetCmd(AVRCMD_WRITE_FUSE_BITS | (uint8_t)fuses);
    fuses >>= 8;
    AvrTargetCmd(AVRCMD_WRITE_FUSE_HI_BITS | (uint8_t)fuses);
    fuses >>= 8;
    AvrTargetCmd(AVRCMD_WRITE_FUSE_EXT_BITS | (uint8_t)fuses);
    fuses >>= 8;

    return rc;
}

/*!
 * \brief Poll target for becoming ready.
 *
 * May switch to another thread while waiting. After switching back to
 * the calling thread, the SPI is re-selected.
 *
 * \param tmo Maximum number of milliseconds to wait.
 *
 * \return 0 on success, -1 on time out.
 */
int AvrTargetPollReady(unsigned int tmo)
{
#if !defined(AVRTARGET_NO_SPI_DEVICE)
    while (AvrTargetCmd(AVRCMD_POLL_READY) & 1UL) {
        if (tmo-- == 0) {
            return -1;
        }
        NutSleep(1);
        /* We may have lost the SPI. */
        SpiSelect();
    }
#endif
    return 0;
}

/*!
 * \brief Erase the target chip.
 *
 * Be aware, that this routine calls AvrTargetPollReady(), which may
 * initiate a context switch while waiting for the erase to finish.
 *
 * \return 0 on success, -1 on time out.
 */
int AvrTargetChipErase(void)
{
    AvrTargetCmd(AVRCMD_CHIP_ERASE);

    return AvrTargetPollReady(AVRTARGET_CHIPERASE_TIMEOUT);
}

/*!
 * \brief Load target's program memory page buffer.
 *
 * \param page Program memory page number.
 * \param data Points to the data.
 *
 */
void AvrTargetPageLoad(uint32_t page, CONST uint8_t * data)
{
    ureg_t bc;

    page <<= 6;
    for (bc = 0; bc < (AVRTARGET_PAGESIZE / 2); bc++) {
        AvrTargetCmd(AVRCMD_LOAD_PROG_MEM_PAGE_LO | (page << 8) | *data);
        data++;
        AvrTargetCmd(AVRCMD_LOAD_PROG_MEM_PAGE_HI | (page << 8) | *data);
        data++;
        page++;
    }
}

/*!
 * \brief Write page buffer contents to program memory.
 *
 * Calls AvrTargetPollReady(), which may initiate a context switch 
 * while waiting for the page write to finish.
 *
 * \param page Program memory page number.
 *
 * \return 0 on success, -1 on time out.
 */
int AvrTargetPageWrite(uint32_t page)
{
    AvrTargetCmd(AVRCMD_WRITE_PROG_MEM_PAGE | (page << 14));

    return AvrTargetPollReady(AVRTARGET_PAGEWRITE_TIMEOUT);
}

/*!
 * \brief Verify program memory page contents.
 *
 * \param page Program memory page number.
 * \param data Points to a buffer with the expected contents.
 *
 * \return 0 if the buffer contents equals the memory page contents or
 *         -1 if it doesn't.
 */
int AvrTargetPageVerify(uint32_t page, CONST uint8_t * data)
{
    uint32_t waddr = page << 6;
    ureg_t bc;

    for (bc = 0; bc < (AVRTARGET_PAGESIZE / 2); bc++) {
        if ((uint8_t) AvrTargetCmd(AVRCMD_READ_PROG_MEM_LO | (waddr << 8)) != *data++) {
            return -1;
        }
        if ((uint8_t) AvrTargetCmd(AVRCMD_READ_PROG_MEM_HI | (waddr << 8)) != *data++) {
            return -1;
        }
        waddr++;
    }
    return 0;
}

/*@}*/
