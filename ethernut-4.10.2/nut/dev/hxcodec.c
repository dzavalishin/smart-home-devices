/*
 * Copyright (C) 2008-2009 by egnite GmbH.
 * Copyright (C) 2001-2007 by egnite Software GmbH.
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
 * $Log$
 * Revision 1.4  2009/02/13 14:46:19  haraldkipp
 * Using the VS10xx driver as a template for redesign.
 *
 * Revision 1.3  2009/01/17 11:26:46  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.2  2008/10/23 08:54:07  haraldkipp
 * Include the correct header file.
 *
 * Revision 1.1  2008/10/05 16:56:15  haraldkipp
 * Added Helix audio device.
 *
 */

#include <sys/atom.h>
#include <sys/event.h>
#include <sys/timer.h>
#include <sys/heap.h>

#include <cfg/arch/gpio.h>
#include <cfg/audio.h>
#include <dev/irqreg.h>

#include <contrib/hxmp3/mp3dec.h>
#include <contrib/hxmp3/hermite.h>
#include <dev/hxcodec.h>
#include <dev/tlv320dac.h>

#include <sys/bankmem.h>

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <memdebug.h>

#if 0
/* Use for local debugging. */
#define NUTDEBUG
#include <stdio.h>
#endif

#ifndef NUT_THREAD_HXCODEC0STACK
/* arm-elf-gcc optimized code used 540 bytes. */
#define NUT_THREAD_HXCODEC0STACK    768
#endif

#ifndef HXCODEC0_MAX_OUTPUT_BUFSIZ
/*! \brief Output buffer size limit. */
#define HXCODEC0_MAX_OUTPUT_BUFSIZ  16384
#endif

#ifndef DAC_OUTPUT_RATE
#if defined (AT91SAM9260_EK)
#define DAC_OUTPUT_RATE     44100
#else
#define DAC_OUTPUT_RATE     8000
#endif
#endif

#ifndef MP3_BUFSIZ
#if defined (AT91SAM9260_EK)
#define MP3_BUFSIZ          1048576
#else
#define MP3_BUFSIZ          (4 * MAINBUF_SIZE)
#endif
#endif

#ifdef HXCODEC0_RESAMPLER
static int rs_maxout;
static short *rs_pcmbuf;
#endif

/*! \name Internal Command Flags */
/*@{*/
/*! \brief Force immediate player start. */
#define HXREQ_PLAY      0x00000001
/*! \brief Force immediate player stop. */
#define HXREQ_CANCEL    0x00000002
/*! \brief Volume update. */
#define HXREQ_VOLUPD    0x00000004
/*! \brief Sine wave test. */
#define HXREQ_BEEP      0x00000008
/*@}*/

/*!
 * \addtogroup xgHelixCodec
 */
/*@{*/

typedef struct _HXDCB {
    int dcb_pbstat;     /*!< \brief Playback status. */
    uint32_t dcb_scmd;    /*!< \brief Requested command flags, see HXREQ_ flags. */
    int dcb_rvol;           /*!< \brief Volume of right channel. */
    int dcb_lvol;           /*!< \brief Volume of left channel. */
    uint32_t dcb_pbwlo;   /*!< \brief Playback buffer low watermark. */
    uint32_t dcb_pbwhi;   /*!< \brief Playback buffer high watermark. */
    uint32_t dcb_wtmo;      /*!< \brief Write timeout. */
    HANDLE dcb_bufque;      /*!< \brief Buffer change event. */
    HANDLE dcb_feedme;      /*!< \brief Decoder hungry event. */
} HXDCB;

static HXDCB dcb;

typedef struct _MP3PLAYERINFO {
    HMP3Decoder mpi_mp3dec;
    MP3FrameInfo mpi_frameinfo;
} MP3PLAYERINFO;

static MP3PLAYERINFO mpi;

static short pi_pcmbuf[MAX_NCHAN * MAX_NGRAN * MAX_NSAMP];

static int first_frame;
static int samprate;

/*!
 * \brief Set volume.
 *
 * \param dev   Specifies the audio codec device.
 * \param left  Left channel gain in db.
 * \param right Right channel gain in db.
 *
 * \return Always 0.
 */
static int HelixPlayerSetVolume(NUTDEVICE *dev, int left, int right)
{
    HXDCB *dcb = (HXDCB *)dev->dev_dcb;

    /* Honor limits. */
    left = left > AUDIO_DAC_MAX_GAIN ? AUDIO_DAC_MAX_GAIN : left;
    left = left < AUDIO_DAC_MIN_GAIN ? AUDIO_DAC_MIN_GAIN : left;
    right = right > AUDIO_DAC_MAX_GAIN ? AUDIO_DAC_MAX_GAIN : right;
    right = right < AUDIO_DAC_MIN_GAIN ? AUDIO_DAC_MIN_GAIN : right;

    Tlv320DacSetVolume(left, right);

    dcb->dcb_lvol = left;
    dcb->dcb_rvol = right;

    return 0;
}

static int DecodeFrame(uint8_t *buf, int len)
{
    int rc = len;
    int skip;

    while (len > 2 * MAINBUF_SIZE) {
        if ((skip = MP3FindSyncWord(buf, len)) < 0) {
		    return -1;
	    }
        if (skip) {
            len -= skip;
            buf += skip;
        } else {
            if (MP3Decode(mpi.mpi_mp3dec, &buf, &len, pi_pcmbuf, 0)) {
                return -1;
            }
            if (first_frame) {
                MP3GetLastFrameInfo(mpi.mpi_mp3dec, &mpi.mpi_frameinfo);
                if (mpi.mpi_frameinfo.nChans == 1) {
                    samprate = mpi.mpi_frameinfo.samprate / 2;
                }
                else if (mpi.mpi_frameinfo.nChans == 2) {
                    samprate = mpi.mpi_frameinfo.samprate;
                }
                else {
                    return -1;
                }
                if (mpi.mpi_frameinfo.samprate < 8000 || samprate > DAC_OUTPUT_RATE) {
                    return -1;
                }
                if (mpi.mpi_frameinfo.bitsPerSample != 16) {
                    return -1;
                }
#ifdef HXCODEC0_RESAMPLER
                /* If needed, initialize resampler. */
                if (samprate != DAC_OUTPUT_RATE) {
                    void *hres;

                    if ((hres = RAInitResamplerHermite(samprate, DAC_OUTPUT_RATE, mpi.mpi_frameinfo.nChans)) == NULL) {
                        return -1;
                    }
                    rs_maxout = RAGetMaxOutputHermite(mpi.mpi_frameinfo.outputSamps, hres);
                    if ((rs_pcmbuf = malloc(rs_maxout * 2)) == NULL) {
                        return -1;
                    }
                }
#endif
                first_frame = 0;
            }
            if (Tlv320DacWrite(pi_pcmbuf, mpi.mpi_frameinfo.outputSamps)) {
                return -1;
            }
            break;
        }
    }
    return rc - len;
}

/*
 * Initialize the stream output buffer with a given size.
 *
 * \param dev Specifies the audio codec device.
 */
static int HelixPlayerBufferInit(NUTDEVICE *dev, uint32_t size)
{
    HXDCB *dcb = (HXDCB *)dev->dev_dcb;

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
    uint8_t *backbuf = NULL;
    int backcnt = 0;
    size_t avail;
    int filled;
    NUTDEVICE *dev = (NUTDEVICE *)arg;
    HXDCB *dcb = (HXDCB *)dev->dev_dcb;

    /* We are a high priority thread. */
    NutThreadSetPriority(7);
    for (;;) {
        /* 
        ** Idle mode processing. 
        */
        if (dcb->dcb_pbstat == CODEC_STATUS_IDLE) {
            /* Wait for a request or a buffer update. */
            NutEventWait(&dcb->dcb_feedme, NUT_WAIT_INFINITE);
            /* Check if we should change to play mode. This will happen
            ** if we receive a play request or if the buffer contents
            ** reaches the high watermark. */
            if ((dcb->dcb_scmd & HXREQ_PLAY) != 0 || NutSegBufUsed() >=  dcb->dcb_pbwhi) {
                dcb->dcb_scmd &= ~(HXREQ_PLAY | HXREQ_CANCEL);
                dcb->dcb_pbstat = CODEC_STATUS_PLAYING;
                HelixPlayerSetVolume(dev, dcb->dcb_lvol, dcb->dcb_rvol);
            }
        }

        /* 
        ** Play mode processing. 
        */
        if (dcb->dcb_pbstat == CODEC_STATUS_PLAYING) {
            /* Process cancel requests. */
            if (dcb->dcb_scmd & HXREQ_CANCEL) {
                dcb->dcb_scmd &= ~HXREQ_CANCEL;
                NutSegBufReset();
            }
            if (dcb->dcb_scmd & HXREQ_VOLUPD) {
                HelixPlayerSetVolume(dev, dcb->dcb_lvol, dcb->dcb_rvol);
                dcb->dcb_scmd &= ~HXREQ_VOLUPD;
            }
            /* Retrieve new data from the input buffer. */
            bp = (uint8_t *) NutSegBufReadRequest(&avail);
            if (backcnt && backcnt + avail >= 4 * MAINBUF_SIZE) {
                memcpy(&backbuf[backcnt], bp, 4 * MAINBUF_SIZE - backcnt);
                filled = DecodeFrame(backbuf, 4 * MAINBUF_SIZE);
                if (filled < 0) {
                    backcnt = 0;
                    free(backbuf);
                } else if (filled < backcnt) {
                    bp = backbuf;
                    backcnt -= filled;
                    backbuf = malloc(4 * MAINBUF_SIZE);
                    memcpy(backbuf, &bp[filled], backcnt);
                    free(bp);
                } else {
                    if (filled > backcnt) {
                        NutSegBufReadLast(filled - backcnt);
                    }
                    backcnt = 0;
                    free(backbuf);
                }
            } else if (avail > 2 * MAINBUF_SIZE) {
                filled = DecodeFrame(bp, avail);
                if (filled < 0) {
                    NutSegBufReadLast(1);
                } else if (filled) {
                    NutSegBufReadLast(filled);
                }
                NutEventPost(&dcb->dcb_bufque);
            }
            else if (NutSegBufUsed() == 0) {
                /* Finally we reached idle mode. */
                dcb->dcb_pbstat = CODEC_STATUS_IDLE;
                NutEventPost(&dcb->dcb_bufque);
            } else if (avail && backcnt == 0) {
                backcnt = avail;
                backbuf = malloc(4 * MAINBUF_SIZE);
                memcpy(backbuf, bp, backcnt);
                NutSegBufReadLast(backcnt);
                NutEventPost(&dcb->dcb_bufque);
            } else {
                /* Wait for a request or a buffer update. */
                NutEventWait(&dcb->dcb_feedme, 1000);
            }
        }
    }
}

/*!
 * \brief Handle I/O controls for audio codec.
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
static int HelixIOCtl(NUTDEVICE * dev, int req, void *conf)
{
    int rc = 0;
    uint32_t *lvp = (uint32_t *) conf;
    int *ivp = (int *) conf;
    HXDCB *dcb = (HXDCB *)dev->dev_dcb;

    switch (req) {
    case AUDIO_PLAY:
        if (dcb->dcb_pbstat == CODEC_STATUS_IDLE) {
            /* Immediately start playing. */
            dcb->dcb_scmd |= HXREQ_PLAY;
        }
        NutEventPost(&dcb->dcb_feedme);
        break;
    case AUDIO_CANCEL:
        if (dcb->dcb_pbstat == CODEC_STATUS_PLAYING) {
            /* Immediately stop playing and discard buffer. */
            dcb->dcb_scmd |= HXREQ_CANCEL;
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
        dcb->dcb_scmd |= HXREQ_VOLUPD;
        break;
    case AUDIO_GET_PBSIZE:
        *lvp = NutSegBufAvailable() + NutSegBufUsed();
        break;
    case AUDIO_SET_PBSIZE:
        rc = HelixPlayerBufferInit(dev, *lvp);
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
    default:
        rc = -1;
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
static int HelixPlayerFlush(NUTDEVICE *dev, uint32_t tmo)
{
    int rc = 0;
    HXDCB *dcb = dev->dev_dcb;

    for (;;) {
        /* Keep the player running if the buffer contains data. */
        if (NutSegBufUsed()) {
            HelixIOCtl(dev, AUDIO_PLAY, NULL);
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
 * \brief Send data to the decoder.
 *
 * A carriage return character will be automatically appended 
 * to any linefeed.
 *
 * \return Number of characters sent.
 */
static int HelixWrite(NUTFILE * nfp, CONST void *data, int len)
{
    int rc = 0;
    uint8_t *bp;
    CONST uint8_t *dp;
    size_t rbytes;
    HXDCB *dcb = nfp->nf_dev->dev_dcb;

    dcb->dcb_wtmo = 3000; //DEBUG
    /* Flush buffer if data pointer is a NULL pointer. */
    if (data == NULL || len == 0) {
        return HelixPlayerFlush(nfp->nf_dev, dcb->dcb_wtmo);
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
 * \brief Write to device.
 *
 * Similar to HelixWrite() except that the data is expected in program memory.
 *
 * This function is implemented for CPUs with Harvard Architecture only.
 *
 * This function is called by the low level output routines of the 
 * \ref xrCrtLowio "C runtime library", using the 
 * \ref _NUTDEVICE::dev_write_P entry.
 *
 * \param nfp    Pointer to a \ref NUTFILE structure, obtained by a previous 
 *               call to PnutFileOpen().
 * \param buffer Pointer to the data in program space. If zero, then the
 *               output buffer will be flushed.
 * \param len    Number of bytes to write.
 *
 * \return The number of bytes written. A return value of -1 indicates an 
 *         error. Currently this function is not implemented and always
 *         returns -1.
 *
 */
static int HelixWrite_P(NUTFILE * nfp, PGM_P buffer, int len)
{
    return -1;
}
#endif

/*!
 * \brief Open codec device.
 *
 * \return Pointer to a static NUTFILE structure.
 */
static NUTFILE *HelixOpen(NUTDEVICE * dev, CONST char *name, int mode, int acc)
{
    NUTFILE *nfp;

    if ((mpi.mpi_mp3dec = MP3InitDecoder()) == NULL) {
        return NUTFILE_EOF;
    }

    first_frame = 1;

    NutSleep(2);

    nfp = malloc(sizeof(NUTFILE));
    nfp->nf_next = NULL;
    nfp->nf_dev = dev;
    nfp->nf_fcb = NULL;

    NutSegBufReset();

    return nfp;
}

/*! 
 * \brief Close codec device.
 */
static int HelixClose(NUTFILE * nfp)
{
    HXDCB *dcb = nfp->nf_dev->dev_dcb;
    int rc = HelixPlayerFlush(nfp->nf_dev, dcb->dcb_wtmo);

    if (nfp) {
        if (mpi.mpi_mp3dec) {
            MP3FreeDecoder(mpi.mpi_mp3dec);
            mpi.mpi_mp3dec = NULL;
        }
        free(nfp);
    }
    return rc;
}

/*
 * Called via dev_init pointer when the device is registered.
 */
static int HelixInit(NUTDEVICE * dev)
{
    size_t avail;

    Tlv320DacInit(DAC_OUTPUT_RATE);

    dcb.dcb_rvol = -32;
    dcb.dcb_lvol = -32;
    Tlv320DacSetVolume(dcb.dcb_lvol, dcb.dcb_rvol);

    /*
    ** Initialize the decoder stream buffer.
    */
#ifdef HXCODEC0_OUTPUT_BUFSIZ
    avail = HXCODEC0_OUTPUT_BUFSIZ;
#else
    avail = NutHeapAvailable() / 2;
    if (avail > HXCODEC0_MAX_OUTPUT_BUFSIZ) {
        avail = HXCODEC0_MAX_OUTPUT_BUFSIZ;
    }
#endif
    if (HelixPlayerBufferInit(dev, avail)) {
        return -1;
    }

    /* Start the feeder thread. */
    if (NutThreadCreate(dev->dev_name, FeederThread, dev, 
        (NUT_THREAD_HXCODEC0STACK * NUT_THREAD_STACK_MULT) + NUT_THREAD_STACK_ADD) == 0) {
        return -1;
    }
    return 0;
}

/*!
 * \brief Device information structure.
 *
 * An application must pass a pointer to this structure to 
 * NutRegisterDevice() before using this driver.
 *
 * The device is named \b audio0.
 *
 * \showinitializer
 */
NUTDEVICE devHelixCodec = {
    0,              /* Pointer to next device, dev_next. */
    {'a', 'u', 'd', 'i', 'o', '0', 0, 0, 0},    /* Unique device name, dev_name. */
    IFTYP_CHAR,     /* Type of device, dev_type. */
    0,              /* Base address, dev_base (not used). */
    0,              /* First interrupt number, dev_irq (not used). */
    0,              /* Interface control block, dev_icb (not used). */
    &dcb,           /* Driver control block, dev_dcb. */
    HelixInit,         /* Driver initialization routine, dev_init. */
    HelixIOCtl,        /* Driver specific control function, dev_ioctl. */
    NULL,           /* Read from device, dev_read. */
    HelixWrite,        /* Write to device, dev_write. */
#ifdef __HARVARD_ARCH__
    HelixWrite_P,      /* Write data from program space to device, dev_write_P. */
#endif
    HelixOpen,         /* Open a device or file, dev_open. */
    HelixClose,        /* Close a device or file, dev_close. */
    NULL            /* Request file size, dev_size. */
};

/*@}*/
