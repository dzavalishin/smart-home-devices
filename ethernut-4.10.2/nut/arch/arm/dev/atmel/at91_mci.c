/*
 * Copyright (C) 2006 by egnite Software GmbH.
 * Copyright (C) 2008, 2011 by egnite GmbH.
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
 * \brief Multimedia Card Interface.
 *
 * This simple implementation supports reading a single
 * 3.3V MultiMedia Card in slot B only.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.3  2008/10/03 11:28:58  haraldkipp
 * Corrected and modified initialization of MultiMedia and SD Cards.
 *
 * Revision 1.2  2008/08/11 06:59:04  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.1  2006/09/05 12:34:21  haraldkipp
 * Support for hardware MultiMedia Card interface added.
 * SD Cards are currently not supported.
 *
 *
 * \endverbatim
 */

#include <cfg/arch.h>
#include <cfg/arch/gpio.h>

#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <sys/heap.h>
#include <sys/timer.h>
#include <sys/event.h>
#include <fs/dospart.h>
#include <fs/fs.h>

#include <dev/blockdev.h>
#include <dev/mmcard.h>
#include <dev/at91_mci.h>


#if 0
/* Use for local debugging. */
#define NUTDEBUG
#include <stdio.h>
#endif

/*!
 * \addtogroup xgAt91Mci
 */
/*@{*/

#ifndef MMC_BLOCK_SIZE
#define MMC_BLOCK_SIZE  512
#endif

#ifndef MCI_INI_BITRATE
/* MMC starts in open drain mode with 400 kHz max. clock. */
#define MCI_INI_BITRATE 400000
#endif

#ifndef MCI_MMC_BITRATE
/* Max. clock for MMC in push-pull mode is 20 MHz. */
#define MCI_MMC_BITRATE 20000000
#endif

#ifndef MCI_SDC_BITRATE
/* Max. clock for SD-Card is 25 MHz. */
#define MCI_SDC_BITRATE 25000000
#endif

#ifndef MMCARD_VRANGE
#define MMCARD_VRANGE   (MMCARD_32_33V | MMCARD_31_32V | MMCARD_30_31V)
#endif

#ifndef MMC_PINS_A
#define MMC_PINS_A  _BV(PA8_MCCK_A)
#endif

#ifndef MMC_PINS_B
#define MMC_PINS_B  _BV(PA1_MCCDB_B) | _BV(PA0_MCDB0_B) | _BV(PA5_MCDB1_B) | _BV(PA4_MCDB2_B) | _BV(PA3_MCDB3_B)
#endif

#define MCICMD_ALL_SEND_CID         (MMCMD_ALL_SEND_CID | MCI_MAXLAT | MCI_RSPTYP_136)
#define MCICMD_DESELECT_CARD        (MMCMD_SELECT_CARD)
#define MCICMD_GO_IDLE_STATE        (MMCMD_GO_IDLE_STATE)
#define MCICMD_READ_SINGLE_BLOCK    (MMCMD_READ_SINGLE_BLOCK | MCI_TRCMD_START | MCI_TRDIR | MCI_MAXLAT | MCI_RSPTYP_48)
#define MCICMD_SELECT_CARD          (MMCMD_SELECT_CARD | MCI_MAXLAT | MCI_RSPTYP_48)
#define MCICMD_SEND_APP_CMD         (MMCMD_SEND_APP_CMD | MCI_MAXLAT | MCI_RSPTYP_48)
#define MCICMD_SEND_APP_OP_COND     (MMCMD_SEND_APP_OP_COND | MCI_MAXLAT | MCI_RSPTYP_48)
#define MCICMD_SEND_OP_COND         (MMCMD_SEND_OP_COND | MCI_MAXLAT | MCI_RSPTYP_48)
#define MCICMD_SEND_RELATIVE_ADDR   (MMCMD_SEND_RELATIVE_ADDR | MCI_MAXLAT | MCI_RSPTYP_48)
#define MCICMD_SEND_STATUS          (MMCMD_SEND_STATUS | MCI_MAXLAT | MCI_RSPTYP_48)
#define MCICMD_SET_BLOCKLEN         (MMCMD_SET_BLOCKLEN | MCI_MAXLAT | MCI_RSPTYP_48)
#define MCICMD_WRITE_BLOCK          (MMCMD_WRITE_BLOCK | MCI_TRCMD_START | MCI_MAXLAT | MCI_RSPTYP_48)

#define MCICMD_IERROR   (MCI_RTOE | MCI_RENDE | MCI_RDIRE | MCI_RINDE)
#define MCICMD_ERROR    (MCI_UNRE | MCI_OVRE | MCI_DTOE | MCI_DCRCE | MCI_RCRCE | MCICMD_IERROR)

#define MCIFLG_SDCARD   0x00000001
#define MCIFLG_4BIT     0x00000010

/*!
 * \brief Local card interface information.
 */
typedef struct _MCIFC {
    /*! \brief Configuration flags. */
    uint32_t ifc_config;
    /*! \brief Operating conditions. */
    uint32_t ifc_opcond;
    /*! \brief Relative card address. */
    uint32_t ifc_reladdr;
    /*! \brief Pointer to sector buffer. */
    uint8_t *ifc_buff;
    /*! \brief MMC response. */
    uint32_t ifc_resp[4];
    /*! \brief Card identification. */
    uint32_t ifc_cid[4];
} MCIFC;

/*!
 * \brief Local multimedia card mount information.
 */
typedef struct _MCIFCB {
    /*! \brief Attached file system device.
     */
    NUTDEVICE *fcb_fsdev;

    /*! \brief Partition table entry of the currently mounted partition.
     */
    DOSPART fcb_part;

    /*! \brief Next block number to read.
     *
     * The file system driver will send a NUTBLKDEV_SEEK control command
     * to set this value before calling the read or the write routine.
     *
     * The number is partition relative.
     */
    uint32_t fcb_blknum;

    /*! \brief Internal block buffer.
     *
     * A file system driver may use this one or optionally provide it's
     * own buffers.
     *
     * Minimal systems may share their external bus interface with
     * device I/O lines, in which case the buffer must be located
     * in internal memory.
     */
    uint8_t fcb_blkbuf[MMC_BLOCK_SIZE];
} MCIFCB;

/*
 * Several routines call NutSleep, which results in a context switch.
 * This mutual exclusion semaphore takes care, that multiple threads
 * do not interfere with each other.
 */
static HANDLE mutex;

/*!
 * \brief Get divider for a given MCI clock rate.
 *
 * \param clk Requested clock rate.
 */
static uint32_t At91MciClockDiv(uint32_t clk)
{
    uint32_t rc;

    /* MCI is driven by MCK/2. */
    rc = NutArchClockGet(NUT_HWCLK_PERIPHERAL) / 2;
    /* Compensate rounding error, but do not care about 10kHz. */
    rc += clk - 10000;
    /* Calculate the divider. */
    rc /= clk;
    /* Actual divider is 1 less, avoid underflow. */
    if (rc) {
        rc -= 1;
    }
    /* In reality, overflow will only happen when the caller requests
       a unrealistic low MCI clock. */
    if (rc > 255) {
        rc = 255;
    }
    return rc;
}

/*!
 * \brief Reset the MCI hardware.
 *
 * \param init If 0, the current settings will be kept, otherwise initial
 *             values are loaded.
 */
static void At91MciReset(int init)
{
    uint32_t mode;
    uint32_t dtmo;
    uint32_t slot;

    /* Enable MCI clock. */
    outr(PMC_PCER, _BV(MCI_ID));

    if (init) {
        outr(MCI_IDR, 0xFFFFFFFF);
        /* Set initial configuration. */
        dtmo = MCI_DTOMUL_1M | MCI_DTOCYC;
        mode = MCI_RDPROOF | MCI_WRPROOF | (2 << MCI_PWSDIV_LSB);
        /* Slow start. */
        mode |= At91MciClockDiv(MCI_INI_BITRATE) << MCI_CLKDIV_LSB;
        slot = MCI_SDCSEL_SLOTB;
    } else {
        /* Retrieve current configuration. */
        dtmo = inr(MCI_DTOR);
        mode = inr(MCI_MR) & 0xffff;
        slot = inr(MCI_SDCR);
    }

    /* Disable and software reset. */
    outr(MCI_CR, MCI_MCIDIS | MCI_SWRST);

    /* Set configuration values. */
    outr(MCI_DTOR, dtmo);
    outr(MCI_MR, mode);
    outr(MCI_SDCR, slot);

    /* Enable card interface. */
    outr(MCI_CR, MCI_MCIEN | MCI_PWSEN);
}

static void At91MciEnablePins(void)
{
    /* Disable PIO lines used for MCI. */
    outr(PIOA_PDR, MMC_PINS_A | MMC_PINS_B);
    /* Enable peripherals. */
    outr(PIOA_ASR, MMC_PINS_A);
    outr(PIOA_BSR, MMC_PINS_B);
}

static void At91MciDisablePins(void)
{
#ifdef MCI0_PIN_SHARING
    /* Enable PIO input lines used for MCI. */
    outr(PIOA_ODR, MMC_PINS_A | MMC_PINS_B);
    outr(PIOA_PER, MMC_PINS_A | MMC_PINS_B);
#endif
}

/*!
 * \brief Initialize MMC hardware interface.
 *
 * This function is automatically executed during during device
 * registration via NutRegisterDevice().
 *
 * \param dev Identifies the device to initialize.
 */
static int At91MciInit(NUTDEVICE * dev)
{
    /* Initialize the MCI hardware. */
    At91MciReset(1);

    return 0;
}

/*!
 * \brief Send command to multimedia card.
 *
 * \param ifc   Specifies the hardware interface.
 * \param cmd   Command code. See MMCMD_ macros.
 * \param param Optional command parameter.
 *
 * \return MCI status.
 */
static uint32_t At91MciTxCmd(MCIFC * ifc, uint32_t cmd, uint32_t param)
{
    uint32_t sr;
    uint32_t rl;
    uint32_t i;
    uint32_t wfs = MCI_CMDRDY;
    uint32_t ces = MCICMD_IERROR;

    /*
     * Disable PDC.
     */
#ifdef NUTDEBUG
    printf("[Cmd%lu,%lx]", cmd & MCI_CMDNB, param);
#endif
    outr(MCI_PTCR, PDC_TXTDIS | PDC_RXTDIS);
    outr(MCI_MR, inr(MCI_MR) & ~(MCI_BLKLEN | MCI_PDCMODE));
    outr(MCI_RCR, 0);
    outr(MCI_RNCR, 0);
    outr(MCI_TCR, 0);
    outr(MCI_TNCR, 0);

    if (cmd & MCI_TRCMD_START) {
        /* Data transfer. Make sure that the buffer is set. */
        if (ifc->ifc_buff == NULL) {
            errno = EINVAL;
            return MCI_OVRE | MCI_UNRE;
        }
        /* Set block length and PDC mode. */
        outr(MCI_MR, (inr(MCI_MR) & ~MCI_BLKLEN) | (MMC_BLOCK_SIZE << MCI_BLKLEN_LSB) | MCI_PDCMODE);
        if (cmd & MCI_TRDIR) {
            /* Set PDC address and counter for reading. */
            outr(MCI_RPR, (uint32_t) ifc->ifc_buff);
            outr(MCI_RCR, MMC_BLOCK_SIZE / 4);
            /* Enable PDC read. */
            outr(MCI_PTCR, PDC_RXTEN);
            /* We will wait until end of data transmission. */
            wfs = MCI_ENDRX;
        } else {
            /* Set PDC address and counter for writing. */
            outr(MCI_TPR, (uint32_t) ifc->ifc_buff);
            outr(MCI_TCR, MMC_BLOCK_SIZE / 4);
            /* We will wait until data block ended. */
            wfs = MCI_BLKE;
        }
    }
    /* Set card parameter and command. */
    outr(MCI_ARGR, param);
    outr(MCI_CMDR, cmd);

    /* When writing, enable PDC after sending the command. */
    if ((cmd & (MCI_TRCMD_START | MCI_TRDIR)) == MCI_TRCMD_START) {
        outr(MCI_PTCR, PDC_TXTEN);
    }

    /* Determine the number of words of the expected response. */
    switch (cmd & MCI_RSPTYP) {
    case MCI_RSPTYP_48:
        rl = 2;
        break;
    case MCI_RSPTYP_136:
        rl = 4;
        break;
    default:
        rl = 0;
        break;
    }
    /* Wait for MCI_CMDRDY, MCI_ENDRX or MCI_BLKE. */
    while (((sr = inr(MCI_SR)) & wfs) == 0);
    /* Check for error. */
    if (sr & ces) {
        return sr;
    }
    /* Read the resonse. */
    for (i = 0; i < rl; i++) {
        ifc->ifc_resp[i] = inr(MCI_RSPR);
    }
#ifdef NUTDEBUG
    printf("[Sta=%lx]", sr);
    if (rl) {
        printf("[Rsp");
        for (i = 0; i < rl; i++) {
            printf(" %lx", ifc->ifc_resp[i]);
        }
        putchar(']');
    }
    putchar('\n');
#endif
    /* When writing, wait until the card is not busy. */
    if (wfs & MCI_BLKE) {
        while ((inr(MCI_SR) & MCI_NOTBUSY) == 0);
    }
    /* Do we need this? */
    ifc->ifc_buff = NULL;

    return sr;
}

/*!
 * \brief Discover available cards.
 *
 * Currently this has been tested for SanDisk SD Cards and several
 * MultiMedia Cards. It doesn't seem to work with RS-MMC, though.
 *
 * \param ifc Specifies the hardware interface.
 *
 * \return 0 on success, -1 otherwise.
 */
static int At91MciDiscover(MCIFC * ifc)
{
    uint32_t sr;
    uint32_t clk = MCI_MMC_BITRATE;
    uint32_t opd = 0;
    int tmo;

    At91MciEnablePins();

    /* Put all cards in idle state. */
    At91MciTxCmd(ifc, MCICMD_GO_IDLE_STATE | MCI_SPCMD_INIT, 0);
    At91MciTxCmd(ifc, MCICMD_GO_IDLE_STATE, 0);

    /* Poll SDC operating conditions. */
    for (tmo = 1000; --tmo;) {
        sr = At91MciTxCmd(ifc, MCICMD_SEND_APP_CMD, 0);
        if ((ifc->ifc_resp[0] & (1 << 8)) != 0 && (sr & MCICMD_IERROR) == 0) {
            sr = At91MciTxCmd(ifc, MCICMD_SEND_APP_OP_COND, MMCARD_VRANGE);
            if ((sr & MCICMD_IERROR) == 0) {
                ifc->ifc_opcond = ifc->ifc_resp[0];
                if (ifc->ifc_resp[0] & MMCOP_NBUSY) {
                    ifc->ifc_config |= MCIFLG_SDCARD;
                    break;
                }
            }
        }
        NutSleep(1);
    }

    if (tmo == 0) {
        /* No SDC. Put all cards back in idle state and try MMC. */
        opd = MCI_OPDCMD;
        At91MciTxCmd(ifc, MCICMD_GO_IDLE_STATE, 0);

        /* Poll MMC operating conditions. */
        for (tmo = 100; --tmo;) {
            sr = At91MciTxCmd(ifc, MCICMD_SEND_OP_COND | opd, MMCARD_VRANGE);
            ifc->ifc_opcond = ifc->ifc_resp[0];
            if (ifc->ifc_resp[0] & MMCOP_NBUSY) {
                break;
            }
            NutSleep(1);
        }
    }

    if (tmo == 0) {
        /* No valid card. */
        At91MciDisablePins();
        return -1;
    }

    /* Discover cards. */
    ifc->ifc_reladdr = 0;
    for (tmo = 50; --tmo;) {
        sr = At91MciTxCmd(ifc, MCICMD_ALL_SEND_CID | opd, 0);
        memcpy(ifc->ifc_cid, ifc->ifc_resp, sizeof(ifc->ifc_cid));
        if (sr & MCI_RTOE) {
            /* No more cards. */
            break;
        }
        if (ifc->ifc_config & MCIFLG_SDCARD) {
            /* SD Card will send an address. */
            ifc->ifc_reladdr = 0;
        } else {
            /* MultiMedia Card will receive an address. */
            ifc->ifc_reladdr++;
        }
        At91MciTxCmd(ifc, MCICMD_SEND_RELATIVE_ADDR | opd, ifc->ifc_reladdr << 16);
        if (ifc->ifc_config & MCIFLG_SDCARD) {
            /* Store SD Card address. */
            ifc->ifc_reladdr = ifc->ifc_resp[0] >> 16;
            /* SD Cards can run at higher clock rates. */
            clk = MCI_SDC_BITRATE;
        }
    }

    At91MciDisablePins();
    if (ifc->ifc_reladdr) {
        /* Switch to high speed transfer. */
        outr(MCI_MR, (inr(MCI_MR) & ~MCI_CLKDIV) | (At91MciClockDiv(clk) << MCI_CLKDIV_LSB));
        return 0;
    }
    return -1;
}

/*!
 * \brief Read a single block.
 *
 * \param ifc Specifies the hardware interface.
 * \param blk Block number to read or verify.
 * \param buf Data buffer. Receives the data being read from the card.
 *
 * \return 0 on success, -1 otherwise.
 */
static int At91MciReadSingle(MCIFC * ifc, uint32_t blk, uint8_t * buf)
{
    int rc = -1;
    uint32_t sr;

    /* Gain mutex access. */
    NutEventWait(&mutex, 0);
    At91MciEnablePins();

    sr = At91MciTxCmd(ifc, MCICMD_SELECT_CARD, ifc->ifc_reladdr << 16);
    if ((sr & MCICMD_ERROR) == 0) {
        sr = At91MciTxCmd(ifc, MCICMD_SET_BLOCKLEN, MMC_BLOCK_SIZE);
        if ((sr & MCICMD_ERROR) == 0) {
            ifc->ifc_buff = buf;
            sr = At91MciTxCmd(ifc, MCICMD_READ_SINGLE_BLOCK, blk * MMC_BLOCK_SIZE);
            if ((sr & MCICMD_ERROR) == 0) {
                rc = 0;
            }
        }
        At91MciTxCmd(ifc, MCICMD_DESELECT_CARD, 0);
    }

    /* Release mutex access. */
    At91MciDisablePins();
    NutEventPost(&mutex);

    return rc;
}

/*!
 * \brief Write a single block.
 *
 * \param ifc Specifies the hardware interface.
 * \param blk Block number to read or verify.
 * \param buf Pointer to the data to be sent to the card.
 *
 * \return 0 on success, -1 otherwise.
 */
static int At91MciWriteSingle(MCIFC * ifc, uint32_t blk, CONST uint8_t * buf)
{
    int rc = -1;
    uint32_t sr;

    /* Gain mutex access. */
    NutEventWait(&mutex, 0);
    At91MciEnablePins();

    sr = At91MciTxCmd(ifc, MCICMD_SELECT_CARD, ifc->ifc_reladdr << 16);
    if ((sr & MCICMD_ERROR) == 0) {
        sr = At91MciTxCmd(ifc, MCICMD_SET_BLOCKLEN, MMC_BLOCK_SIZE);
        if ((sr & MCICMD_ERROR) == 0) {
            ifc->ifc_buff = (uint8_t *)buf;
            sr = At91MciTxCmd(ifc, MCICMD_WRITE_BLOCK, blk * MMC_BLOCK_SIZE);
            if ((sr & MCICMD_ERROR) == 0) {
                rc = 0;
            }
        }
        At91MciTxCmd(ifc, MCICMD_DESELECT_CARD, 0);
    }

    /* Release mutex access. */
    At91MciDisablePins();
    NutEventPost(&mutex);

    return rc;
}

/*!
 * \brief Read data blocks from a mounted partition.
 *
 * \param nfp    Pointer to a ::NUTFILE structure, obtained by a previous
 *               call to At91MciMount().
 * \param buffer Pointer to the data buffer to fill.
 * \param num    Maximum number of blocks to read. However, reading
 *               multiple blocks is not yet supported by this driver.
 *
 * \return The number of blocks actually read. A return value of -1
 *         indicates an error.
 */
static int At91MciBlockRead(NUTFILE * nfp, void *buffer, int num)
{
    MCIFCB *fcb = (MCIFCB *) nfp->nf_fcb;
    uint32_t blk = fcb->fcb_blknum;
    NUTDEVICE *dev = (NUTDEVICE *) nfp->nf_dev;
    MCIFC *ifc = (MCIFC *) dev->dev_icb;
    int rt;

    if (buffer == 0) {
        buffer = fcb->fcb_blkbuf;
    }
    blk += fcb->fcb_part.part_sect_offs;

    for (rt = 10; --rt >= 0;) {
        if (At91MciReadSingle(ifc, blk, buffer) == 0) {
            return 1;
        }
        At91MciReset(0);
    }
    return -1;
}

/*!
 * \brief Write data blocks to a mounted partition.
 *
 * \param nfp    Pointer to a \ref NUTFILE structure, obtained by a previous
 *               call to At91MciMount().
 * \param buffer Pointer to the data to be written.
 * \param num    Maximum number of blocks to write. However, writing
 *               multiple blocks is not yet supported by this driver.
 *
 * \return The number of blocks written. A return value of -1 indicates an
 *         error.
 */
static int At91MciBlockWrite(NUTFILE * nfp, CONST void *buffer, int num)
{
    MCIFCB *fcb = (MCIFCB *) nfp->nf_fcb;
    uint32_t blk = fcb->fcb_blknum;
    NUTDEVICE *dev = (NUTDEVICE *) nfp->nf_dev;
    MCIFC *ifc = (MCIFC *) dev->dev_icb;

    if (buffer == 0) {
        buffer = fcb->fcb_blkbuf;
    }
    blk += fcb->fcb_part.part_sect_offs;

    if (At91MciWriteSingle(ifc, blk, buffer) == 0) {
        return 1;
    }
    return -1;
}

/*!
 * \brief Unmount a previously mounted partition.
 *
 * Applications should not directly call this function, but use the high
 * level stdio routines for closing a previously opened volume.
 *
 * \return 0 on success, -1 otherwise.
 */
static int At91MciUnmount(NUTFILE * nfp)
{
    int rc = -1;

    if (nfp) {
        MCIFCB *fcb = (MCIFCB *) nfp->nf_fcb;

        if (fcb) {
            rc = fcb->fcb_fsdev->dev_ioctl(fcb->fcb_fsdev, FS_VOL_UNMOUNT, NULL);
            NutHeapFree(fcb);
        }
        NutHeapFree(nfp);
    }
    return rc;
}

/*!
 * \brief Mount a partition.
 *
 * Nut/OS doesn't provide specific routines for mounting. Instead routines
 * for opening files are used.
 *
 * Applications should not directly call this function, but use the high
 * level stdio routines for opening a file.
 *
 * \param dev  Pointer to the MMC device.
 * \param name Partition number followed by a slash followed by a name
 *             of the file system device. Both items are optional. If no
 *             file system driver name is given, the first file system
 *             driver found in the list of registered devices will be
 *             used. If no partition number is specified or if partition
 *             zero is given, the first active primary partition will be
 *             used.
 * \param mode Opening mode. Currently ignored, but
 *             \code _O_RDWR | _O_BINARY \endcode should be used for
 *             compatibility with future enhancements.
 * \param acc  File attributes, ignored.
 *
 * \return Pointer to a newly created file pointer to the mounted
 *         partition or NUTFILE_EOF in case of any error.
 */
static NUTFILE *At91MciMount(NUTDEVICE * dev, CONST char *name, int mode, int acc)
{
    int partno = 0;
    int i;
    NUTDEVICE *fsdev;
    NUTFILE *nfp;
    MCIFCB *fcb;
    DOSPART *part;
    MCIFC *ifc = (MCIFC *) dev->dev_icb;
    FSCP_VOL_MOUNT mparm;

    if (At91MciDiscover(ifc)) {
        errno = ENODEV;
        return NUTFILE_EOF;
    }

    /* Parse the name for a partition number and a file system driver. */
    if (*name) {
        partno = atoi(name);
        do {
            name++;
        } while (*name && *name != '/');
        if (*name == '/') {
            name++;
        }
    }
#ifdef NUTDEBUG
    printf("['%s'-PART%d]", name, partno);
#endif

    /*
     * Check the list of registered devices for the given name of the
     * files system driver. If none has been specified, get the first
     * file system driver in the list. Hopefully the application
     * registered one only.
     */
    for (fsdev = nutDeviceList; fsdev; fsdev = fsdev->dev_next) {
        if (*name == 0) {
            if (fsdev->dev_type == IFTYP_FS) {
                break;
            }
        } else if (strcmp(fsdev->dev_name, name) == 0) {
            break;
        }
    }

    if (fsdev == 0) {
#ifdef NUTDEBUG
        printf("[No FSDriver]");
#endif
        errno = ENODEV;
        return NUTFILE_EOF;
    }

    if ((fcb = NutHeapAllocClear(sizeof(MCIFCB))) == 0) {
        errno = ENOMEM;
        return NUTFILE_EOF;
    }
    fcb->fcb_fsdev = fsdev;

    /* Initialize MMC access mutex semaphore. */
    NutEventPost(&mutex);

    /* Read MBR. */
    if (At91MciReadSingle(ifc, 0, fcb->fcb_blkbuf)) {
        NutHeapFree(fcb);
        return NUTFILE_EOF;
    }
    /* Check for the cookie at the end of this sector. */
	if (fcb->fcb_blkbuf[DOSPART_MAGICPOS] != 0x55 || fcb->fcb_blkbuf[DOSPART_MAGICPOS + 1] != 0xAA) {
        NutHeapFree(fcb);
        return NUTFILE_EOF;
	}

    /* Check for the partition table. */
	if(fcb->fcb_blkbuf[DOSPART_TYPEPOS] == 'F' &&
       fcb->fcb_blkbuf[DOSPART_TYPEPOS + 1] == 'A' &&
       fcb->fcb_blkbuf[DOSPART_TYPEPOS + 2] == 'T') {
        /* No partition table. Assume FAT12 and 32MB size. */
        fcb->fcb_part.part_type = PTYPE_FAT12;
        fcb->fcb_part.part_sect_offs = 0;
        fcb->fcb_part.part_sects = 65536; /* How to find out? */
	}
    else {
        /* Read partition table. */
        part = (DOSPART *) & fcb->fcb_blkbuf[DOSPART_SECTORPOS];
        for (i = 1; i <= 4; i++) {
            if (partno) {
                if (i == partno) {
                    /* Found specified partition number. */
                    fcb->fcb_part = *part;
                    break;
                }
            } else if (part->part_state & 0x80) {
                /* Located first active partition. */
                fcb->fcb_part = *part;
                break;
            }
            part++;
        }

        if (fcb->fcb_part.part_type == PTYPE_EMPTY) {
            NutHeapFree(fcb);
            return NUTFILE_EOF;
        }
    }
    if ((nfp = NutHeapAlloc(sizeof(NUTFILE))) == 0) {
        NutHeapFree(fcb);
        errno = ENOMEM;
        return NUTFILE_EOF;
    }
    nfp->nf_next = 0;
    nfp->nf_dev = dev;
    nfp->nf_fcb = fcb;

    /*
     * Mount the file system volume.
     */
    mparm.fscp_bmnt = nfp;
    mparm.fscp_part_type = fcb->fcb_part.part_type;
    if (fsdev->dev_ioctl(fsdev, FS_VOL_MOUNT, &mparm)) {
        At91MciUnmount(nfp);
        return NUTFILE_EOF;
    }
    return nfp;
}

/*!
 * \brief Perform MMC control functions.
 *
 * This function is called by the ioctl() function of the C runtime
 * library. Applications should not directly call this function.
 *
 * \todo Card change detection should verify the serial card number.
 *
 * \param dev  Identifies the device that receives the device-control
 *             function.
 * \param req  Requested control function. May be set to one of the
 *             following constants:
 *             - \ref NUTBLKDEV_MEDIAAVAIL
 *             - \ref NUTBLKDEV_MEDIACHANGE
 *             - \ref NUTBLKDEV_INFO
 *             - \ref NUTBLKDEV_SEEK
 *             - \ref MMCARD_GETOCR
 *
 * \param conf Points to a buffer that contains any data required for
 *             the given control function or receives data from that
 *             function.
 * \return 0 on success, -1 otherwise.
 */
static int At91MciIOCtrl(NUTDEVICE * dev, int req, void *conf)
{
    int rc = 0;
    MCIFC *ifc = (MCIFC *) dev->dev_icb;

    switch (req) {
    case NUTBLKDEV_MEDIAAVAIL:
        *((int *) conf) = 1;
        break;
    case NUTBLKDEV_MEDIACHANGE:
        *((int *) conf) = 0;
        break;
    case NUTBLKDEV_INFO:
        {
            BLKPAR_INFO *par = (BLKPAR_INFO *) conf;
            MCIFCB *fcb = (MCIFCB *) par->par_nfp->nf_fcb;

            par->par_nblks = fcb->fcb_part.part_sects;
            par->par_blksz = MMC_BLOCK_SIZE;
            par->par_blkbp = fcb->fcb_blkbuf;
        }
        break;
    case NUTBLKDEV_SEEK:
        {
            BLKPAR_SEEK *par = (BLKPAR_SEEK *) conf;
            MCIFCB *fcb = (MCIFCB *) par->par_nfp->nf_fcb;

            fcb->fcb_blknum = par->par_blknum;
        }
        break;
    case MMCARD_GETOCR:
        *((uint32_t *) conf) = ifc->ifc_opcond;
        break;
    default:
        rc = -1;
        break;
    }
    return rc;
}

static MCIFC mci0_info;

/*!
 * \brief Multimedia card device information structure.
 *
 * A pointer to this structure must be passed to NutRegisterDevice()
 * to bind this driver to the Nut/OS kernel. An application may then
 * call
 * /verbatim
 * _open("MCI0:", _O_RDWR | _O_BINARY);
 * /endverbatim
 * to mount the first active primary partition with any previously
 * registered file system driver (typically devPhat0).
 */
NUTDEVICE devAt91Mci0 = {
    0,                          /*!< Pointer to next device, dev_next. */
    {'M', 'C', 'I', '0', 0, 0, 0, 0, 0}
    ,                           /*!< Unique device name, dev_name. */
    0,                          /*!< Type of device, dev_type. Obsolete. */
    0,                          /*!< Base address, dev_base. Unused. */
    0,                          /*!< First interrupt number, dev_irq. Unused. */
    &mci0_info,                 /*!< Interface control block, dev_icb. */
    0,                          /*!< Driver control block used by the low level part, dev_dcb. */
    At91MciInit,                /*!< Driver initialization routine, dev_init. */
    At91MciIOCtrl,              /*!< Driver specific control function, dev_ioctl. */
    At91MciBlockRead,           /*!< Read data from a file, dev_read. */
    At91MciBlockWrite,          /*!< Write data to a file, dev_write. */
    At91MciMount,               /*!< Mount a file system, dev_open. */
    At91MciUnmount,             /*!< Unmount a file system, dev_close. */
    0                           /*!< Return file size, dev_size. */
};

/*@}*/
