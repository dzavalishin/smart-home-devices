/*
 * Copyright (C) 2008-2009 by egnite GmbH
 * Copyright (C) 2001-2007 by egnite Software GmbH
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
 * $Id: spi_vscodec.c 2702 2009-09-17 11:01:21Z haraldkipp $
 */

#include <dev/vscodec.h>

#include <sys/event.h>
#include <sys/timer.h>
#include <sys/nutdebug.h>
#include <sys/bankmem.h>

#include <stdlib.h>
#include <string.h>
#include <memdebug.h>
#include <fcntl.h>

/*!
 * \addtogroup xgVsCodec
 */
/*@{*/

/*! \name Internal Command Flags */
/*@{*/
/*! \brief Force immediate player start. */
#define VSREQ_PLAY      0x00000001
/*! \brief Force immediate player stop. */
#define VSREQ_CANCEL    0x00000002
/*! \brief Volume update. */
#define VSREQ_VOLUPD    0x00000004
/*! \brief Audio enhancement update. */
#define VSREQ_AUDIOE    0x00000008
/*! \brief Sine wave test. */
#define VSREQ_BEEP      0x00000010
/*! \brief Start recording. */
#define VSREQ_RECORD    0x00000020
/*@}*/


/* Used to send zeros to the decoder. */
uint8_t zero_chunk[VSCODEC_DATA_CHUNK_SIZE];

/*!
 * \brief Wait until codec is ready.
 *
 * \param dev Sepcifies the codec device.
 * \param tmo Maximum number of milliseconds to wait.
 *
 * \return 0 on success, -1 on time out.
 */
int VsCodecWaitReady(NUTDEVICE *dev, uint32_t tmo)
{
    VSDCB *dcb = (VSDCB *) dev->dev_dcb;

    while (!(* dcb->dcb_isready)()) {
        if (NutEventWait(&dcb->dcb_feedme, tmo)) {
            return -1;
        }
    }
    return 0;
}

/*
 * \brief Read from or write to a VLSI audio codec register.
 *
 * \param dev  Specifies the codec device.
 * \param op   Operation, either \ref VS_OPCODE_READ or
 *             \ref VS_OPCODE_WRITE.
 * \param reg  The register index.
 * \param val  This value will be stored in the register on a write
 *             operation. It is ignored on read operations.
 *
 * \return Register contents. On write operations, the contents before
 *         the modification is returned.
 */
uint16_t VsCodecReg(NUTDEVICE *dev, uint_fast8_t op, uint_fast8_t reg, uint_fast16_t val)
{
    uint8_t cmd[4];
    VSDCB *dcb = (VSDCB *) dev->dev_dcb;

    /* Assemble command buffer. */
    cmd[0] = (uint8_t) op;
    cmd[1] = (uint8_t) reg;
    cmd[2] = (uint8_t) (val >> 8);
    cmd[3] = (uint8_t) val;

    VsCodecWaitReady(dev, VSCODEC_CMD_TIMEOUT);
    (*dcb->dcb_sendcmd)(cmd, 4);
    val = cmd[2];
    val <<= 8;
    val |= cmd[3];
    return (uint16_t) val;
}

/*!
 * \brief Read and modify VLSI audio codec mode flags.
 *
 * Set both, flags and mask to zero to read the flags without change.
 *
 * \param dev   Specifies the audio codec device.
 * \param flags Mode flags to set. All other flags are disabled, if they
 *              appear in the mask.
 * \param mask  Mode flags to update. All other flags are left unchanged.
 *
 * \return Previous mode flags.
 */
uint16_t VsCodecMode(NUTDEVICE *dev, uint_fast16_t flags, uint_fast16_t mask)
{
    uint16_t rc;

    /* Read the mode register. */
    rc = VsCodecReg(dev, VS_OPCODE_READ, VS_MODE_REG, 0);
    if (mask | flags) {
        VsCodecReg(dev, VS_OPCODE_WRITE, VS_MODE_REG, (rc & ~mask) | flags);
        /* Add delay after software reset, if configured. */
#if defined(VSCODEC_SWRST_RECOVER)
        if (flags & VS_SM_RESET) {
            NutSleep(VSCODEC_SWRST_RECOVER);
        }
#endif
    }
    return rc;
}

/*!
 * \brief Set volume.
 *
 * \param dev   Specifies the audio codec device.
 * \param left  Left channel gain in db.
 * \param right Right channel gain in db.
 *
 * \return Always 0.
 */
int VsDecoderSetVolume(NUTDEVICE *dev, int left, int right)
{
    VSDCB *dcb = (VSDCB *)dev->dev_dcb;
    uint16_t l;
    uint16_t r;

    /* Honor limits. */
    left = left > AUDIO_DAC_MAX_GAIN ? AUDIO_DAC_MAX_GAIN : left;
    left = left < AUDIO_DAC_MIN_GAIN ? AUDIO_DAC_MIN_GAIN : left;
    right = right > AUDIO_DAC_MAX_GAIN ? AUDIO_DAC_MAX_GAIN : right;
    right = right < AUDIO_DAC_MIN_GAIN ? AUDIO_DAC_MIN_GAIN : right;

    /* Convert to register values. */
    l = (uint16_t)(-2 * left);
    r = (uint16_t)(-2 * right);

    VsCodecReg(dev, VS_OPCODE_WRITE, VS_VOL_REG, (l << VS_VOL_LEFT_LSB) | (r << VS_VOL_RIGHT_LSB));
    dcb->dcb_lvol = left;
    dcb->dcb_rvol = right;

    return 0;
}

/*!
 * \brief Set Bass.
 *
 * \param dev   Specifies the audio codec device.
 * \param treb  Treble amplitude x 1.5dB.
 * \param tset  Treble frequency limit x 1000Hz
 * \param bass  Bass amplitude x 1dB.
 * \param bset  Bass frequency limit x 10Hz
 *
 * \return Always 0.
 */
int VsDecoderSetBass(NUTDEVICE *dev, int treb, int tfin, int bass, int bfin)
{
#ifdef VS_HAS_BASS_REG
    VSDCB *dcb = (VSDCB *)dev->dev_dcb;
//    uint16_t t;
//    uint16_t b;

    //printf("bass: t:%ddB tf:%dHz b:%ddB bf:%dHz\n", treb, tfin*1000, bass, bfin);
    /* Honor limits. */
    treb = treb > AUDIO_DAC_MAX_TREB ? AUDIO_DAC_MAX_TREB : treb;
    treb = treb < 0 ? 0 : treb;
    tfin = tfin > AUDIO_DAC_MAX_TFIN ? AUDIO_DAC_MAX_TFIN : tfin;
    tfin = tfin < 0 ? 0 : tfin;
    bass = bass > AUDIO_DAC_MAX_BASS ? AUDIO_DAC_MAX_BASS : bass;
    bass = bass < 0 ? 0 : bass;
    bfin = bfin > AUDIO_DAC_MAX_BFIN ? AUDIO_DAC_MAX_BFIN : bfin;
    bfin = bfin < 0 ? 0 : bfin;

    /* Convert to register values. */
    /*
    l = (uint16_t)(-2 * left);
    r = (uint16_t)(-2 * right);
    */

    VsCodecReg(dev, VS_OPCODE_WRITE, VS_BASS_REG,
        (treb << VS_ST_AMPLITUDE_LSB) | (tfin << VS_ST_FREQLIMIT_LSB) |
        (treb << VS_SB_AMPLITUDE_LSB) | (tfin << VS_SB_FREQLIMIT_LSB)
    );

    dcb->dcb_treb = treb;
    dcb->dcb_tfin = tfin;
    dcb->dcb_bass = bass;
    dcb->dcb_bfin = bfin;
#endif

    return 0;
}

/*!
 * \brief Start or stop sine wave beeper.
 *
 * Old, VS1001 compatible routine, which supports a limit number of
 * frequencies only. It will try to find the best value.
 *
 * \param dev Specifies the audio codec device.
 * \param fsin Frequency, or 0 to switch beeper off.
 *
 * \return Actual frequency set.
 */
uint16_t VsCodecBeep(NUTDEVICE *dev, uint16_t fsin)
{
    uint16_t rc = 0;
    static uint8_t on[] = { 0x53, 0xEF, 0x6E, 0x3F, 0x00, 0x00, 0x00, 0x00 };
    static CONST uint8_t off[] = { 0x45, 0x78, 0x69, 0x74, 0x00, 0x00, 0x00, 0x00 };
    static CONST uint16_t ftab[] = { 44100, 48000, 32000, 22050, 24000, 16000, 11025, 12000 };
    static uint16_t mode;
    VSDCB *dcb = (VSDCB *)dev->dev_dcb;

    if (fsin) {
        uint_fast16_t s;
        uint32_t f;
        uint_fast16_t d;
        int_fast8_t i = sizeof(ftab) / sizeof(ftab[0]);
        int_fast8_t ie = 0;
        int_fast16_t dmin = fsin;
        uint32_t fs = (long)fsin * 128;

        while (--i >= ie) {
            /* Calculate skip speed. */
            f = ftab[i];
            s = (fs + f / 2) / f;
            /* Check that skip is within 5-bit range. */
            if (s && s < 32) {
                /* Calculate the absolute deviation. */
                f *= s;
                f += 64;
                f >>= 7;
                d = (uint_fast16_t)(fsin > f ? fsin - f : f - fsin);
                /* Check if this is a new minimum. */
                if (d < dmin) {
                    dmin = d;
                    rc = (uint16_t) f;
                    /* Set new skip speed and frequency index. */
                    on[3] = (i << 5) | s;
                    /* We can stop earlier with this fit. */
                    if (i > 1) {
                        ie = i - 2;
                    }
                }
            }
        }
        mode = VsCodecMode(dev, VS_SM_RESET, VS_SM_RESET);
#ifdef VS_SM_TESTS
        VsCodecMode(dev, mode | VS_SM_TESTS, 0xffff);
#endif
        (*dcb->dcb_senddata)(on, sizeof(on));
    } else {
        (*dcb->dcb_senddata)(off, sizeof(off));
#ifdef VS_SM_TESTS
        VsCodecMode(dev, VS_SM_RESET, VS_SM_RESET | VS_SM_TESTS);
#else
        VsCodecMode(dev, VS_SM_RESET, VS_SM_RESET);
#endif
        VsCodecMode(dev, mode, 0xffff);
    }
    return rc;
}

/*
 * Initialize the stream output buffer with a given size.
 *
 * \param dev Specifies the audio codec device.
 */
int VsDecoderBufferInit(NUTDEVICE *dev, uint32_t size)
{
    VSDCB *dcb = (VSDCB *)dev->dev_dcb;

    /* Make sure that the decoder is idle. */
    if (dcb->dcb_pbstat != CODEC_STATUS_IDLE) {
        return -1;
    }
    /* Set new buffer size. */
    if (NutSegBufInit((size_t)size) == NULL) {
        return -1;
    }
    /* Set watermark defaults. */
    dcb->dcb_pbwlo = NutSegBufAvailable() / 3;
    dcb->dcb_pbwhi = dcb->dcb_pbwlo * 2;

    return 0;
}

/*
 * VLSI decoder feeding thread.
 */
THREAD(FeederThread, arg)
{
    uint8_t *bp;
    size_t avail;
    int filled;
    uint_fast8_t intest = 0;
    uint_fast16_t idlefill = 0;
    NUTDEVICE *dev = (NUTDEVICE *)arg;
    VSDCB *dcb = (VSDCB *)dev->dev_dcb;

    /* We are a high priority thread. */
    NutThreadSetPriority(7);
    for (;;) {
        /*
        ** Idle mode processing.
        */
        if (dcb->dcb_pbstat == CODEC_STATUS_IDLE) {
            /* Loop while in test mode. */
            do {
                /* Wait for a request or a buffer update. */
                NutEventWait(&dcb->dcb_feedme, NUT_WAIT_INFINITE);
                /* Process beep commands. */
                if (dcb->dcb_scmd & VSREQ_BEEP) {
                    if ((*dcb->dcb_isready)()) {
                        dcb->dcb_scmd &= ~VSREQ_BEEP;
                        intest = VsCodecBeep(dev, dcb->dcb_sinefreq) != 0;
                    }
                }
            } while (intest);
            /* Check if we should change to record mode. */
            if (dcb->dcb_scmd & VSREQ_RECORD) {
                dcb->dcb_scmd &= ~(VSREQ_RECORD | VSREQ_CANCEL);
                dcb->dcb_pbstat = CODEC_STATUS_RECORDING;
                /* Activate the encoder. */
                VsCodecReg(dev, VS_OPCODE_WRITE, VS_AIADDR_REG, 0x0034);
            }
            /* Check if we should change to play mode. This will happen
            ** if we receive a play request or if the buffer contents
            ** reaches the high watermark. */
            else if ((dcb->dcb_scmd & VSREQ_PLAY) != 0 || NutSegBufUsed() >=  dcb->dcb_pbwhi) {
                dcb->dcb_scmd &= ~(VSREQ_PLAY | VSREQ_CANCEL);
                dcb->dcb_pbstat = CODEC_STATUS_PLAYING;
                VsDecoderSetVolume(dev, dcb->dcb_lvol, dcb->dcb_rvol);
            }
        }

        /*
        ** Play mode processing.
        */
        if (dcb->dcb_pbstat == CODEC_STATUS_PLAYING) {
            /* Wait while decoder is busy. */
            VsCodecWaitReady(dev, NUT_WAIT_INFINITE);
            /* Process cancel requests. */
            if (dcb->dcb_scmd & VSREQ_CANCEL) {
                dcb->dcb_scmd &= ~VSREQ_CANCEL;
                NutSegBufReset();
#if VS_HAS_SM_CANCEL
                VsCodecMode(dev, VS_SM_CANCEL, VS_SM_CANCEL);
#endif
            }
            /* Retrieve new data from the input buffer. */
            bp = (uint8_t *) NutSegBufReadRequest(&avail);
            if (avail) {
                /* More data available, send as much as possible to the
                ** decoder. If no data was sent, then the bus may be
                ** blocked by another device. */
                filled = (*dcb->dcb_senddata)(bp, avail);
                if (filled) {
                    /* Update the buffer's read position. */
                    NutSegBufReadLast(filled);
                    NutEventPost(&dcb->dcb_bufque);
                    idlefill = 2048;
                }
                if (dcb->dcb_scmd & VSREQ_VOLUPD) {
                    VsDecoderSetVolume(dev, dcb->dcb_lvol, dcb->dcb_rvol);
                    dcb->dcb_scmd &= ~VSREQ_VOLUPD;
                }
                if (dcb->dcb_scmd & VSREQ_AUDIOE) {
                    VsDecoderSetBass( dev, dcb->dcb_treb, dcb->dcb_tfin, dcb->dcb_bass, dcb->dcb_bfin);
                    dcb->dcb_scmd &= ~VSREQ_AUDIOE;
                }
            } else if (NutSegBufUsed() == 0) {
                /* Running out of data. */
                if (idlefill) {
                    /* For some reason the HDAT0/HDAT1 registers in the
                    ** VS1053 do not contain zero when the codec runs
                    ** empty. I expected this when reading the datasheet.
                    ** Instead we fill the buffer with zeros, to make
                    ** sure that the whole buffer is decoded before we
                    ** enter idle mode. */
                    idlefill -= (*dcb->dcb_senddata)(NULL, idlefill);
                }
                if (idlefill == 0) {
                    /* Finally we reached idle mode. */
                    dcb->dcb_pbstat = CODEC_STATUS_IDLE;
                    NutEventPost(&dcb->dcb_bufque);
                }
            }
        }

        /* 
        ** Record mode processing. 
        */
        else if (dcb->dcb_pbstat == CODEC_STATUS_RECORDING) {
            for (;;) {
                /* Process cancel requests. */
                if (dcb->dcb_scmd & VSREQ_CANCEL) {
                    dcb->dcb_scmd &= ~VSREQ_CANCEL;
                    dcb->dcb_pbstat = CODEC_STATUS_IDLE;
                    break;
                }
                bp = (uint8_t *)NutSegBufWriteRequest(&avail);
                avail >>= 1;
                if (avail == 0) {
                    /* Buffer overflow. */
                    dcb->dcb_pbstat = CODEC_STATUS_IDLE;
                    break;
                }
#if defined(VS_HDAT0_REG) && defined(VS_HDAT1_REG)
                /* TODO: Missing register definitions give compiler error. */
                filled = VsCodecReg(dev, VS_OPCODE_READ, VS_HDAT1_REG, 0);
                if (filled > 255) {
                    if (avail > filled) {
                        avail = filled;
                    } else {
                        filled = avail;
                    }
                    while (filled--) {
                        uint16_t data = VsCodecReg(dev, VS_OPCODE_READ, VS_HDAT0_REG, 0);
                        *bp++ = (uint8_t) data >> 8;
                        *bp++ = (uint8_t) data >> 8;
                    }
                    NutSegBufWriteLast(avail << 1);
                    NutEventPost(&dcb->dcb_bufque);
                } else {
                    NutSleep(200);
                }
#endif
            }
            NutEventPost(&dcb->dcb_bufque);
        }
    }
}

/* TODO: This is a preview and may not work as expected. */
static int VsCodecLoadPlugIn(NUTDEVICE *dev, VS_PLUGIN_INFO *plg)
{
    uint_fast8_t reg;
    uint_fast16_t cnt;
    size_t i = 0;

#ifdef VS_SM_ADPCM
    VsCodecMode(dev, 0, VS_SM_ADPCM);
#endif
    VsCodecReg(dev, VS_OPCODE_WRITE, VS_CLOCKF_REG, 0xC000);
    NutSleep(100);
#if VS_HAS_BASS_REG
    VsCodecReg(dev, VS_OPCODE_WRITE, VS_BASS_REG, 0);
#endif
	VsCodecReg(dev, VS_OPCODE_WRITE, VS_AIADDR_REG, 0);
	VsCodecReg(dev, VS_OPCODE_WRITE, VS_WRAMADDR_REG, 0xC01A);
	VsCodecReg(dev, VS_OPCODE_WRITE, VS_WRAM_REG, 0x0002);
    while (i + 3 <= plg->vsplg_size) {
        reg = (uint_fast8_t)plg->vsplg_data[i++];
        cnt = plg->vsplg_data[i++];

        if (cnt & 0x8000) {
            cnt &= 0x7FFF;
            while (cnt--) {
                VsCodecReg(dev, VS_OPCODE_WRITE, reg, plg->vsplg_data[i]);
            }
            i++;
        } 
        else if (i + cnt <= plg->vsplg_size) {
            while (cnt--) {
                VsCodecReg(dev, VS_OPCODE_WRITE, reg, plg->vsplg_data[i]);
                i++;
            }
        } 
        else {
            break;
        }
    }
    if (i != plg->vsplg_size) {
        return -1;
    }

#ifdef VS_SM_ADPCM
	// set bits 12 and 13 of register SCI_MODE
	// 12 is the ADPCM bit, 13 is the MIC/LINE bit.
    VsCodecMode(dev, VS_SM_LINE_IN | VS_SM_ADPCM, VS_SM_LINE_IN | VS_SM_ADPCM);
#endif

#if VS_HAS_AICTRL0_REG
	// write 0 to SCI_AICTRL0 (maximum signal level, set by encoder to read later on)
	VsCodecReg(dev, VS_OPCODE_WRITE, VS_AICTRL0_REG, 0);
#endif

    // recording gain 0: automatic gain control on
	//CodecReg(&codec_node, VS_OPCODE_WRITE, VS_AICTRL1_REG, 0);

#if VS_HAS_AICTRL1_REG
	// recording gain 1
	VsCodecReg(dev, VS_OPCODE_WRITE, VS_AICTRL1_REG, 1024);
#endif

#if VS_HAS_AICTRL2_REG
	// maximum autogain amplification, 4096 (=4x) is recommended
	VsCodecReg(dev, VS_OPCODE_WRITE, VS_AICTRL2_REG, 4096);
#endif

#if VS_HAS_AICTRL3_REG	
	// setting SCI_AICTRL3 should be fine, too...
	VsCodecReg(dev, VS_OPCODE_WRITE, VS_AICTRL3_REG, 0);
#endif

    return 0;
}

/*!
 * \brief Handle I/O controls for audio codec.
 *
 * \param dev Specifies the audio codec device.
 *
 * - AUDIO_PLAY         Force playback start, even if the buffer level is too lower.
 * - AUDIO_CANCEL       Cancel playback and discard all buffered data.
 * - AUDIO_GET_STATUS   Sets an int to 1 if the player is running, 0 if idle.
 * - AUDIO_GET_PLAYGAIN Sets an int to the current playback gain, 0..-127.
 * - AUDIO_SET_PLAYGAIN Reads the requested playback gain from an int, 0..-127.
 * - AUDIO_GET_PBSIZE   Sets an unsigned long with the size of the playback buffer.
 * - AUDIO_SET_PBSIZE   Sets the size the playback buffer using an unsigned long.
 * - AUDIO_GET_PBLEVEL  Sets an unsigned long with the number of bytes in the playback buffer.
 * - AUDIO_GET_PBWLOW   Sets an unsigned long with the low watermark of the playback buffer.
 * - AUDIO_SET_PBWLOW   Sets the low watermark (unsigned long) of the playback buffer.
 * - AUDIO_GET_PBWHIGH  Sets an unsigned long with the high watermark of the playback buffer.
 * - AUDIO_SET_PBWHIGH  Sets the high watermark (unsigned long) of the playback buffer.
 * - AUDIO_BEEP         Plays a short sine wave beep.
 *
 * \return 0 on success, -1 otherwise.
 */
int VsCodecIOCtl(NUTDEVICE * dev, int req, void *conf)
{
    int rc = 0;
    uint32_t *lvp = (uint32_t *) conf;
    int *ivp = (int *) conf;
    VSDCB *dcb = (VSDCB *)dev->dev_dcb;

    switch (req) {
    case AUDIO_PLAY:
        if (dcb->dcb_pbstat == CODEC_STATUS_IDLE) {
            /* Immediately start playing. */
            dcb->dcb_scmd |= VSREQ_PLAY;
        }
        NutEventPost(&dcb->dcb_feedme);
        break;
    case AUDIO_CANCEL:
        if (dcb->dcb_pbstat == CODEC_STATUS_PLAYING) {
            /* Immediately stop playing and discard buffer. */
            dcb->dcb_scmd |= VSREQ_CANCEL;
        }
        NutEventPost(&dcb->dcb_feedme);
        break;
    case AUDIO_GET_STATUS:
        *ivp = dcb->dcb_pbstat;
        break;
    case AUDIO_GET_PLAYGAIN:
        *ivp = dcb->dcb_rvol > dcb->dcb_lvol ? dcb->dcb_rvol : dcb->dcb_lvol;
        break;
    case AUDIO_SET_PLAYGAIN:
        dcb->dcb_lvol = *ivp;
        dcb->dcb_rvol = *ivp;
        dcb->dcb_scmd |= VSREQ_VOLUPD;
        break;
    case AUDIO_SET_TREB:
        dcb->dcb_treb = *ivp;
        dcb->dcb_scmd |= VSREQ_AUDIOE;
        break;
    case AUDIO_SET_TFIN:
        dcb->dcb_tfin = *ivp;
        dcb->dcb_scmd |= VSREQ_AUDIOE;
        break;
    case AUDIO_SET_BASS:
        dcb->dcb_bass = *ivp;
        dcb->dcb_scmd |= VSREQ_AUDIOE;
        break;
    case AUDIO_SET_BFIN:
        dcb->dcb_bfin = *ivp;
        dcb->dcb_scmd |= VSREQ_AUDIOE;
        break;
    case AUDIO_GET_PBSIZE:
        *lvp = NutSegBufAvailable() + NutSegBufUsed();
        break;
    case AUDIO_SET_PBSIZE:
        rc = VsDecoderBufferInit(dev, *lvp);
        break;
    case AUDIO_GET_PBLEVEL:
        *lvp = NutSegBufUsed();
        break;
    case AUDIO_GET_PBWLOW:
        *lvp = dcb->dcb_pbwlo;
        break;
    case AUDIO_SET_PBWLOW:
        dcb->dcb_pbwlo = *lvp;
        break;
    case AUDIO_GET_PBWHIGH:
        *lvp = dcb->dcb_pbwhi;
        break;
    case AUDIO_SET_PBWHIGH:
        dcb->dcb_pbwhi = *lvp;
        break;
    case AUDIO_SETWRITETIMEOUT:
        dcb->dcb_wtmo = *lvp;
        break;
    case AUDIO_GETWRITETIMEOUT:
        *lvp = dcb->dcb_wtmo;
        break;
    case AUDIO_BEEP:
        if (dcb->dcb_pbstat == CODEC_STATUS_IDLE) {
            dcb->dcb_sinefreq = *((uint16_t *) conf);
            dcb->dcb_scmd |= VSREQ_BEEP;
            NutEventPost(&dcb->dcb_feedme);
        } else {
            rc = -1;
        }
        break;
    case AUDIO_GET_DECCAPS:
        /* Retrieve decoder capabilities. */
        *lvp = dcb->dcb_dec_caps;
        break;
    case AUDIO_GET_CODCAPS:
        /* Retrieve encoder capabilities. */
        *lvp = dcb->dcb_cod_caps;
        break;
    case AUDIO_GET_MIDCAPS:
        /* Retrieve midi capabilities. */
        *lvp = dcb->dcb_midi_caps;
        break;

#if 0
    case AUDIO_GET_DECINFO:
        /* Retrieve decoder information. */
        break;
    case AUDIO_GET_CODINFO:
        /* Retrieve encoder information. */
        break;
    case AUDIO_GET_MIDINFO:
        /* Retrieve midi information. */
        break;
#endif
    case AUDIO_PLUGIN_UPLOAD:
        rc = VsCodecLoadPlugIn(dev, (VS_PLUGIN_INFO *) conf);
        break;

    default:
        rc = (*dcb->dcb_control)(req, conf);
        break;
    }
    return rc;
}

/*!
 * \brief Flush VLSI audio decoder buffer.
 *
 * Waits until all currently buffered data had been processed by the
 * decoder. This makes sure that the end of the stream will not be
 * cut off.
 *
 * \param dev Specifies the audio codec device.
 * \param tmo Timeout in milliseconds.
 *
 * \return 0 on success, -1 on timeout.
 */
static int VsDecoderBufferFlush(NUTDEVICE *dev, uint32_t tmo)
{
    int rc = 0;
    VSDCB *dcb = dev->dev_dcb;

    for (;;) {
        /* Keep the player running if the buffer contains data. */
        if (NutSegBufUsed()) {
            VsCodecIOCtl(dev, AUDIO_PLAY, NULL);
        }
        /* No data in buffer. If idle, then we are done. */
        else if (dcb->dcb_pbstat == CODEC_STATUS_IDLE) {
            /* No data and player is idle. */
            break;
        }
        /* Wait for a new buffer update. */
        rc = NutEventWait(&dcb->dcb_bufque, tmo);
        if (rc) {
            break;
        }
    }
    return rc;
}

/*!
 * \brief Read from the encoder.
 *
 * \param nfp  Pointer to a \ref NUTFILE structure, obtained by a previous 
 *             call to VsCodecOpen().
 * \param data Pointer to the data buffer. If NULL, the buffered data
 *             will be flushed.
 * \param len  Number of bytes to read. If 0, all buffered data will be 
 *             flushed.
 *
 * \return Number of characters sent. If a read timeout had been set,
 *         then this may be less than the specified length.
 */
int VsCodecRead(NUTFILE * nfp, void *data, int len)
{
    int rc = 0;
    uint8_t *bp;
    uint8_t *dp;
    size_t rbytes;
    NUTDEVICE *dev;
    VSDCB *dcb;

    dev = nfp->nf_dev;
    dcb = dev->dev_dcb;

    /* Flush buffer if data pointer is a NULL pointer. */
    if (data == NULL || len == 0) {
        return 0;
    }
    if (dcb->dcb_pbstat == CODEC_STATUS_PLAYING) {
        /* Cannot read decoder data. */
        return -1;
    } else if (dcb->dcb_pbstat == CODEC_STATUS_IDLE) {
        /* Start recording if idle. */
        if (dcb->dcb_cod_mode & AUDIO_FMT_VORBIS) {
        }
        dcb->dcb_scmd |= VSREQ_RECORD;
        NutEventPost(&dcb->dcb_feedme);
    }
    dp = data;
    while (len) {
        /* Retrieve new data from the input buffer. */
        bp = (uint8_t *) NutSegBufReadRequest(&rbytes);
        if (rbytes) {
            if (rbytes > len) {
                rbytes = len;
            }
            memcpy(dp, bp, rbytes);
            NutSegBufReadLast(rbytes);
            NutEventPost(&dcb->dcb_feedme);
            len -= rbytes;
            rc += rbytes;
            dp += rbytes;
        }
        /* Wait for data. */
        else if (NutEventWait(&dcb->dcb_bufque, dcb->dcb_rtmo)) {
            /* Read timeout. */
            break;
        }
    }
    return rc;
}

/*!
 * \brief Write to decoder.
 *
 * \param nfp  Pointer to a \ref NUTFILE structure, obtained by a previous
 *             call to VsCodecOpen().
 * \param data Pointer to the data buffer. If NULL, the buffered data
 *             will be flushed.
 * \param len  Number of bytes that are available in the buffer. If 0,
 *             all buffered data will be flushed.
 *
 * \return Number of characters sent. If a write timeout had been set,
 *         then this may be less than the specified length.
 */
int VsCodecWrite(NUTFILE * nfp, CONST void *data, int len)
{
    int rc = 0;
    uint8_t *bp;
    CONST uint8_t *dp;
    size_t rbytes;
    VSDCB *dcb = nfp->nf_dev->dev_dcb;

    /* Flush buffer if data pointer is a NULL pointer. */
    if (data == NULL || len == 0) {
        return VsDecoderBufferFlush(nfp->nf_dev, dcb->dcb_wtmo);
    }
    dp = data;
    while (len) {
        bp = (uint8_t *)NutSegBufWriteRequest(&rbytes);
        if (rbytes == 0) {
            /* No buffer space, wait for change. */
            if (NutEventWait(&dcb->dcb_bufque, dcb->dcb_wtmo)) {
                /* Write timeout. */
                break;
            }
        } else {
            if (rbytes > len) {
                rbytes = len;
            }
            memcpy(bp, dp, rbytes);
            NutSegBufWriteLast(rbytes);
            NutEventPost(&dcb->dcb_feedme);
            len -= rbytes;
            rc += rbytes;
            dp += rbytes;
        }
    }
    return rc;
}

#ifdef __HARVARD_ARCH__
/*!
 * \brief Write program data to decoder.
 *
 * Similar to VsCodecWrite() except that the data is expected in program memory.
 *
 * This function is implemented for CPUs with Harvard Architecture only.
 *
 * This function is called by the low level output routines of the
 * \ref xrCrtLowio "C runtime library", using the
 * \ref _NUTDEVICE::dev_write_P entry.
 *
 * \param nfp    Pointer to a \ref NUTFILE structure, obtained by a previous
 *               call to VsCodecOpen().
 * \param buffer Pointer to the data in program space. If zero, then the
 *               output buffer will be flushed.
 * \param len    Number of bytes to write.
 *
 * \return The number of bytes written. A return value of -1 indicates an
 *         error. Currently this function is not implemented and always
 *         returns -1.
 *
 */
int VsCodecWrite_P(NUTFILE * nfp, PGM_P buffer, int len)
{
    return -1;
}
#endif

/*
 * Open codec stream.
 */
NUTFILE *VsCodecOpen(NUTDEVICE * dev, CONST char *name, int mode, int acc)
{
    NUTFILE *nfp;
    VSDCB *dcb;

#if defined(VS_SW_RESET_ON_OPEN)
    VsCodecMode(dev, VS_SM_RESET, VS_SM_RESET);
#endif

    dcb = dev->dev_dcb;
    if (mode & _O_WRONLY) {
        uint32_t on = 1;
        /* Decoder. */
        if (VsCodecIOCtl(dev, AUDIO_IRQ_ENABLE, &on)) {
            return NULL;
        }
    } else {
        /* Encoder. */
        if (strcmp(name, "vorbis") == 0) {
            dcb->dcb_cod_mode = AUDIO_FMT_VORBIS;
        } else {
            dcb->dcb_cod_mode = AUDIO_FMT_WAV_IMA_ADPCM;
        }
    }

    nfp = malloc(sizeof(NUTFILE));
    if (nfp) {
        nfp->nf_next = NULL;
        nfp->nf_dev = dev;
        nfp->nf_fcb = NULL;
    }
    NutSegBufReset();

    return nfp;
}

/*
 * Close codec stream.
 */
int VsCodecClose(NUTFILE * nfp)
{
    VSDCB *dcb = nfp->nf_dev->dev_dcb;

    int rc = VsDecoderBufferFlush(nfp->nf_dev, dcb->dcb_wtmo);

    if (nfp) {
        free(nfp);
    }
    return rc;
}

/*@}*/
