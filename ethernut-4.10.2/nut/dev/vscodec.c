/*
 * Copyright (C) 2008 by egnite GmbH. All rights reserved.
 * Copyright (C) 2001-2007 by egnite Software GmbH. All rights reserved.
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
 * -
 *
 * This software has been inspired by all the valuable work done by
 * Jesper Hansen and Pavel Chromy. Many thanks for all their help.
 */

/*
 * $Log$
 * Revision 1.8  2009/02/13 14:52:05  haraldkipp
 * Include memdebug.h for heap management debugging support.
 *
 * Revision 1.7  2009/01/30 08:59:30  haraldkipp
 * Make sure, that used registers are defined.
 *
 * Revision 1.6  2009/01/17 11:26:46  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.5  2008/10/23 08:54:07  haraldkipp
 * Include the correct header file.
 *
 * Revision 1.4  2008/09/18 09:51:58  haraldkipp
 * Use the correct PORT macros.
 *
 * Revision 1.3  2008/08/11 06:59:42  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.2  2008/04/01 10:15:27  haraldkipp
 * VS10xx ioctl() returned -1 on success. Fixed.
 *
 * Revision 1.1  2008/02/15 16:45:41  haraldkipp
 * First release.
 *
 * Revision 1.1  2007/04/12 08:59:55  haraldkipp
 * VS10XX decoder support added.
 *
 */

#include <sys/atom.h>
#include <sys/event.h>
#include <sys/timer.h>
#include <sys/heap.h>

#include <cfg/arch/gpio.h>
#include <cfg/audio.h>
#include <dev/irqreg.h>
#include <dev/vscodec.h>

#include <sys/bankmem.h>

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <memdebug.h>

/*!
 * \addtogroup xgVsCodec
 */
/*@{*/

#if !defined(AUDIO_VS1001K) && !defined(AUDIO_VS1011E) && !defined(AUDIO_VS1002D) && !defined(AUDIO_VS1003B) && !defined(AUDIO_VS1033C) && !defined(AUDIO_VS1053C)
#define AUDIO_VS1001K
#endif

#ifndef VS10XX_FREQ
/*! \brief Decoder crystal frequency. */
#define VS10XX_FREQ             12288000UL
#endif

#ifndef VS10XX_HWRST_DURATION
/*! \brief Minimum time in milliseconds to held hardware reset low. */
#define VS10XX_HWRST_DURATION   1
#endif

#ifndef VS10XX_HWRST_RECOVER
/*! \brief Milliseconds to wait after hardware reset. */
#define VS10XX_HWRST_RECOVER    4
#endif

#ifndef VS10XX_SWRST_RECOVER
/*! \brief Milliseconds to wait after software reset. */
#define VS10XX_SWRST_RECOVER    2
#endif

#ifndef VS10XX_SCI_MODE
#define VS10XX_SCI_MODE         0
#endif

#ifndef VS10XX_SCI_RATE
#define VS10XX_SCI_RATE         (VS10XX_FREQ / 6)
#endif

#ifndef VS10XX_SDI_MODE
#define VS10XX_SDI_MODE         0
#endif

#ifndef VS10XX_SDI_RATE
#define VS10XX_SDI_RATE         (VS10XX_FREQ / 6)
#endif


/* -------------------------------------------------
 * AT91 port specifications.
 */
#if defined (MCU_AT91R40008) || defined (MCU_AT91SAM7X) || defined (MCU_AT91SAM7SE512) || defined (MCU_AT91SAM9260)

#if defined(ELEKTOR_IR1)

#define VS_XRESET_BIT           31 /* PB31 */
#define VS10XX_XCS_BIT          PA31_SPI0_NPCS1_A
#define VS10XX_XCS_PORT       PIOA_ID
#define VS10XX_XDCS_BIT         PB30_SPI0_NPCS2_A
#define VS10XX_XDCS_PORT      PIOB_ID
#define VS10XX_DREQ_BIT         PA30_IRQ1_A
#define VS10XX_DREQ_PIO_ID      PIOA_ID
#define VS10XX_SIGNAL           sig_INTERRUPT1

#endif /* ELEKTOR_IR1 */

#if defined(VS10XX_XCS_BIT)

#if !defined(VS10XX_XCS_PORT)
#define VS10XX_XCS_PE_REG        PIO_PER
#define VS10XX_XCS_OE_REG        PIO_OER
#define VS10XX_XCS_COD_REG       PIO_CODR
#define VS10XX_XCS_SOD_REG       PIO_SODR
#elif VS10XX_XCS_PORT == PIOA_ID
#define VS10XX_XCS_PE_REG        PIOA_PER
#define VS10XX_XCS_OE_REG        PIOA_OER
#define VS10XX_XCS_COD_REG       PIOA_CODR
#define VS10XX_XCS_SOD_REG       PIOA_SODR
#elif VS10XX_XCS_PORT == PIOB_ID
#define VS10XX_XCS_PE_REG        PIOB_PER
#define VS10XX_XCS_OE_REG        PIOB_OER
#define VS10XX_XCS_COD_REG       PIOB_CODR
#define VS10XX_XCS_SOD_REG       PIOB_SODR
#elif VS10XX_XCS_PORT == PIOC_ID
#define VS10XX_XCS_PE_REG        PIOC_PER
#define VS10XX_XCS_OE_REG        PIOC_OER
#define VS10XX_XCS_COD_REG       PIOC_CODR
#define VS10XX_XCS_SOD_REG       PIOC_SODR
#endif
#define VS10XX_XCS_ENA() \
    outr(VS10XX_XCS_PE_REG, _BV(VS10XX_XCS_BIT)); \
    outr(VS10XX_XCS_OE_REG, _BV(VS10XX_XCS_BIT))
#define VS10XX_XCS_CLR()   outr(VS10XX_XCS_COD_REG, _BV(VS10XX_XCS_BIT))
#define VS10XX_XCS_SET()   outr(VS10XX_XCS_SOD_REG, _BV(VS10XX_XCS_BIT))

#else /* VS10XX_XCS_BIT */

#define VS10XX_XCS_ENA()
#define VS10XX_XCS_CLR()
#define VS10XX_XCS_SET()

#endif /* VS10XX_XCS_BIT */

#if defined(VS10XX_XDCS_BIT)

#if !defined(VS10XX_XDCS_PORT)
#define VS10XX_XDCS_PE_REG        PIO_PER
#define VS10XX_XDCS_OE_REG        PIO_OER
#define VS10XX_XDCS_COD_REG       PIO_CODR
#define VS10XX_XDCS_SOD_REG       PIO_SODR
#elif VS10XX_XDCS_PORT == PIOA_ID
#define VS10XX_XDCS_PE_REG        PIOA_PER
#define VS10XX_XDCS_OE_REG        PIOA_OER
#define VS10XX_XDCS_COD_REG       PIOA_CODR
#define VS10XX_XDCS_SOD_REG       PIOA_SODR
#elif VS10XX_XDCS_PORT == PIOB_ID
#define VS10XX_XDCS_PE_REG        PIOB_PER
#define VS10XX_XDCS_OE_REG        PIOB_OER
#define VS10XX_XDCS_COD_REG       PIOB_CODR
#define VS10XX_XDCS_SOD_REG       PIOB_SODR
#elif VS10XX_XDCS_PORT == PIOC_ID
#define VS10XX_XDCS_PE_REG        PIOC_PER
#define VS10XX_XDCS_OE_REG        PIOC_OER
#define VS10XX_XDCS_COD_REG       PIOC_CODR
#define VS10XX_XDCS_SOD_REG       PIOC_SODR
#endif
#define VS10XX_XDCS_ENA() \
    outr(VS10XX_XDCS_PE_REG, _BV(VS10XX_XDCS_BIT)); \
    outr(VS10XX_XDCS_OE_REG, _BV(VS10XX_XDCS_BIT))
#define VS10XX_XDCS_CLR()   outr(VS10XX_XDCS_COD_REG, _BV(VS10XX_XDCS_BIT))
#define VS10XX_XDCS_SET()   outr(VS10XX_XDCS_SOD_REG, _BV(VS10XX_XDCS_BIT))

#else /* VS10XX_XDCS_BIT */

#define VS10XX_XDCS_ENA()
#define VS10XX_XDCS_CLR()
#define VS10XX_XDCS_SET()

#endif /* VS10XX_XDCS_BIT */

#if defined(VS10XX_DREQ_BIT)

#if !defined(VS10XX_DREQ_PIO_ID)
#define VS10XX_DREQ_PD_REG       PIO_PDR
#define VS10XX_DREQ_OD_REG       PIO_ODR
#define VS10XX_DREQ_PDS_REG      PIO_PDSR
#elif VS10XX_DREQ_PIO_ID == PIOA_ID
#define VS10XX_DREQ_PD_REG       PIOA_PDR
#define VS10XX_DREQ_OD_REG       PIOA_ODR
#define VS10XX_DREQ_PDS_REG      PIOA_PDSR
#elif VS10XX_DREQ_PIO_ID == PIOB_ID
#define VS10XX_DREQ_PD_REG       PIOB_PDR
#define VS10XX_DREQ_OD_REG       PIOB_ODR
#define VS10XX_DREQ_PDS_REG      PIOB_PDSR
#elif VS10XX_DREQ_PIO_ID == PIOC_ID
#define VS10XX_DREQ_PD_REG       PIOC_PDR
#define VS10XX_DREQ_OD_REG       PIOC_ODR
#define VS10XX_DREQ_PDS_REG      PIOC_PDSR
#endif

#define VS10XX_DREQ_ENA() \
    outr(VS10XX_DREQ_PD_REG, _BV(VS10XX_DREQ_BIT)); \
    outr(VS10XX_DREQ_OD_REG, _BV(VS10XX_DREQ_BIT))
#define VS10XX_DREQ_TST()    ((inr(VS10XX_DREQ_PDS_REG) & _BV(VS10XX_DREQ_BIT)) == _BV(VS10XX_DREQ_BIT))

#else /* VS10XX_DREQ_BIT */

#define VS10XX_DREQ_ENA()
#define VS10XX_DREQ_TST()   0

#endif /* VS10XX_DREQ_BIT */

/* -------------------------------------------------
 * End of port specifications.
 */
#endif


#define VSREQ_PLAY      0x00000001
#define VSREQ_CANCEL    0x00000002
#define VSREQ_BEEP      0x00000004

typedef struct _VSDCB {
    int dcb_pbstat;     /*!< \brief Playback status. */
    uint32_t dcb_scmd;  /*!< \brief Requested command flags, see VSREQ_ flags. */
    int dcb_crvol;      /*!< \brief Current volume of right channel. */
    int dcb_srvol;      /*!< \brief Requested volume of right channel. */
    int dcb_clvol;      /*!< \brief Current volume of left channel. */
    int dcb_slvol;      /*!< \brief Requested volume of left channel. */
    int dcb_ctreb;      /*!< \brief Current bass enhancement. */
    int dcb_streb;      /*!< \brief Requested bass enhancement. */
    int dcb_ctfin;      /*!< \brief Current bass frequency.*/
    int dcb_stfin;      /*!< \brief Requested bass frequency. */
    int dcb_cbass;      /*!< \brief Current treble enhancement. */
    int dcb_sbass;      /*!< \brief Requested bass enhancement. */
    int dcb_cbfin;      /*!< \brief Current treble frequency. */
    int dcb_sbfin;      /*!< \brief Requested treble frequency. */
    uint32_t dcb_pbwlo; /*!< \brief Playback buffer low watermark. */
    uint32_t dcb_pbwhi; /*!< \brief Playback buffer high watermark. */
} VSDCB;

static VSDCB dcb;
static unsigned int vs_chip;

/*
 * Interlink not ready yet. Provide some basic SPI routines.
 */

static uint8_t SpiByte(uint8_t val)
{
    /* Transmission is started by writing the transmit data. */
    outr(SPI0_TDR, val);
    /* Wait for receiver data register full. */
    while((inr(SPI0_SR) & SPI_RDRF) == 0);
    /* Read data. */
    val = (uint8_t)inr(SPI0_RDR);

    return val;
}

static void SciSelect(void)
{
    outr(SPI0_CSR0, (24 << SPI_SCBR_LSB) | SPI_NCPHA);
    outr(SPI0_MR, SPI_MODFDIS | SPI_MSTR);
    outr(PIOA_CODR, _BV(VS10XX_XCS_BIT));
}

static void SciDeselect(void)
{
    outr(PIOA_SODR, _BV(VS10XX_XCS_BIT));
}

/*!
 * \brief Wait for decoder ready.
 *
 * This function will check the DREQ line. Decoder interrupts must have
 * been disabled before calling this function.
 */
static int VsWaitReady(void)
{
    int tmo = 16384;

    do {
        if (VS10XX_DREQ_TST()) {
            return 0;
        }
    } while (tmo--);

    return -1;
}

/*
 * \brief Write a specified number of bytes to the VS10XX data interface.
 *
 * This function will check the DREQ line. Decoder interrupts must have
 * been disabled before calling this function.
 */
static int VsSdiWrite(CONST uint8_t * data, size_t len)
{
    while (len--) {
        if (!VS10XX_DREQ_TST() && VsWaitReady()) {
            return -1;
        }
        SpiByte(*data);
        data++;
    }
    return 0;
}

/*
 * \brief Write a specified number of bytes from program space to the
 *        VS10XX data interface.
 *
 * This function is similar to VsSdiWrite() except that the data is
 * located in program space.
 */
static int VsSdiWrite_P(PGM_P data, size_t len)
{
    while (len--) {
        if (!VS10XX_DREQ_TST() && VsWaitReady()) {
            return -1;
        }
        SpiByte(PRG_RDB(data));
        data++;
    }
    return 0;
}

/*
 * \brief Write to a decoder register.
 *
 * Decoder interrupts must have been disabled before calling this function.
 */
static void VsRegWrite(ureg_t reg, uint16_t data)
{
    VsWaitReady();
    SciSelect();
    SpiByte(VS_OPCODE_WRITE);
    SpiByte((uint8_t) reg);
    SpiByte((uint8_t) (data >> 8));
    SpiByte((uint8_t) data);
    SciDeselect();
}

/*
 * \brief Read from a register.
 *
 * Decoder interrupts must have been disabled before calling this function.
 *
 * \return Register contents.
 */
static uint16_t VsRegRead(ureg_t reg)
{
    uint16_t data;

    VsWaitReady();
    SciSelect();
    SpiByte(VS_OPCODE_READ);
    SpiByte((uint8_t) reg);
    data = (uint16_t)SpiByte(0) << 8;
    data |= SpiByte(0);
    SciDeselect();

    return data;
}

/*!
 * \brief Sine wave beep.
 *
 * Untested.
 *
 * \param fsin Frequency.
 * \param ms   Duration.
 *
 * \return Always 0.
 */
static int VsBeep(uint8_t fsin, uint8_t ms)
{
    static prog_char on[] = { 0x53, 0xEF, 0x6E };
    static prog_char off[] = { 0x45, 0x78, 0x69, 0x74 };
    static prog_char end[] = { 0x00, 0x00, 0x00, 0x00 };

    VsRegWrite(VS_MODE_REG, VS_SM_TESTS | VS_SM_SDINEW);

    fsin = 56 + (fsin & 7) * 9;
    VsSdiWrite_P(on, sizeof(on));
    VsSdiWrite(&fsin, 1);
    VsSdiWrite_P(end, sizeof(end));
    NutDelay(ms);
    VsSdiWrite_P(off, sizeof(off));
    VsSdiWrite_P(end, sizeof(end));

    VsRegWrite(VS_MODE_REG, VS_SM_SDINEW);

    return 0;
}

static HANDLE vs_ready;

static void VsInterrupt(void *arg)
{
    NutEventPostFromIrq(&vs_ready);
}

THREAD(FeederThread, arg)
{
    char *bp;
    size_t avail;
    int filled;
    uint8_t crgain;
    uint8_t srgain;
    uint8_t clgain;
    uint8_t slgain;

    NutSleep(500);

    dcb.dcb_slvol = dcb.dcb_clvol = -12;
    dcb.dcb_srvol = dcb.dcb_crvol = -12;
    srgain = (uint8_t)(-2 * dcb.dcb_srvol);
    crgain = 254;
    slgain = (uint8_t)(-2 * dcb.dcb_slvol);
    clgain = 254;
    VsRegWrite(VS_VOL_REG, ((uint16_t)clgain << VS_VOL_LEFT_LSB) | ((uint16_t)crgain << VS_VOL_RIGHT_LSB));

    /* Register the interrupt routine */
    while (NutRegisterIrqHandler(&VS10XX_SIGNAL, VsInterrupt, NULL)) {
        NutSleep(1000);
    }

    /* Rising edge will generate an interrupt. */
    NutIrqSetMode(&VS10XX_SIGNAL, NUT_IRQMODE_RISINGEDGE);

    VS10XX_DREQ_ENA();
    NutIrqEnable(&VS10XX_SIGNAL);

    for (;;) {
        NutEventWait(&vs_ready, 100);
        if (!VS10XX_DREQ_TST()) {
            continue;
        }
        if (dcb.dcb_scmd) {
            if (dcb.dcb_scmd & VSREQ_CANCEL) {
                NutSegBufReset();
            }
            if (dcb.dcb_scmd & VSREQ_BEEP) {
                VsBeep(2, 100);
            }
            dcb.dcb_scmd &= VSREQ_PLAY;
        }
        if (NutSegBufUsed() < dcb.dcb_pbwlo) {
            dcb.dcb_pbstat = CODEC_STATUS_IDLE;
            if (crgain != 254) {
                clgain = crgain = 254;
                VsRegWrite(VS_VOL_REG, ((uint16_t)clgain << VS_VOL_LEFT_LSB) | ((uint16_t)crgain << VS_VOL_RIGHT_LSB));
            }
            while (NutSegBufUsed() <  dcb.dcb_pbwhi) {
                if (dcb.dcb_scmd) {
                    if (dcb.dcb_scmd & VSREQ_PLAY) {
                        dcb.dcb_pbwhi = dcb.dcb_pbwlo = NutSegBufUsed() / 2;
                    }
                    break;
                }
                NutSleep(100);
            }
        }
        dcb.dcb_scmd &= ~VSREQ_PLAY;
        if (dcb.dcb_scmd) {
            continue;
        }

        outr(SPI0_CSR0, (12 << SPI_SCBR_LSB) | SPI_NCPHA);
        outr(SPI0_MR, SPI_MODFDIS | SPI_MSTR);
        if (dcb.dcb_pbstat != CODEC_STATUS_PLAYING) {
            outr(PIOB_CODR, _BV(30));
            while (!VS10XX_DREQ_TST()) {
                SpiByte(0);
            }
            outr(PIOB_SODR, _BV(30));
        }

        for (;;) {
            if (!VS10XX_DREQ_TST()) {
                break;
            }
            bp = NutSegBufReadRequest(&avail);
            if (avail == 0) {
                dcb.dcb_pbstat = CODEC_STATUS_IDLE;
                if (crgain != 254) {
                    clgain = crgain = 254;
                    VsRegWrite(VS_VOL_REG, ((uint16_t)clgain << VS_VOL_LEFT_LSB) | ((uint16_t)crgain << VS_VOL_RIGHT_LSB));
                }
                break;
            }
            outr(PIOB_CODR, _BV(30));
            for (filled = 0; avail--; filled++, bp++) {
                if (!VS10XX_DREQ_TST()) {
                    dcb.dcb_pbstat = CODEC_STATUS_PLAYING;
                    break;
                }
                SpiByte(*bp);
            }
            outr(PIOB_SODR, _BV(30));
            NutSegBufReadLast(filled);
        }
        if (dcb.dcb_clvol != dcb.dcb_slvol || dcb.dcb_crvol != dcb.dcb_srvol) {
            srgain = (uint8_t)(-2 * dcb.dcb_srvol);
            slgain = (uint8_t)(-2 * dcb.dcb_slvol);

            dcb.dcb_clvol = dcb.dcb_slvol;
            dcb.dcb_crvol = dcb.dcb_srvol;
        }
        else if (srgain != crgain || slgain != clgain) {
            int diff = (int)srgain - (int)crgain;

            if (diff > 4) {
                diff = 4;
            }
            else if (diff < -4) {
                diff = -4;
            }
            crgain = (uint8_t)((int)crgain + diff);

            diff = (int)slgain - (int)clgain;
            if (diff > 4) {
                diff = 4;
            }
            else if (diff < -4) {
                diff = -4;
            }
            clgain = (uint8_t)((int)clgain + diff);
            VsRegWrite(VS_VOL_REG, ((uint16_t)clgain << VS_VOL_LEFT_LSB) | ((uint16_t)crgain << VS_VOL_RIGHT_LSB));
        }
    }
}

static int VsPlayerFlush(void)
{
    int tmo = 1000; /* TODO: Configurable timeout. */

    /* Stupid polling for now. */
    while(dcb.dcb_pbstat == CODEC_STATUS_PLAYING) {
        NutSleep(1);
        if (tmo-- <= 0) {
            return -1;
        }
    }
    return 0;
}

/*!
 * \brief Send data to the decoder.
 *
 * A carriage return character will be automatically appended
 * to any linefeed.
 *
 * \return Number of characters sent.
 */
static int VsWrite(NUTFILE * fp, CONST void *data, int len)
{
    char *buf;
    size_t rbytes;

    /* Flush buffer if data pointer is a NULL pointer. */
    if (data == NULL || len == 0) {
        return VsPlayerFlush();
    }
    if (len) {
        buf = NutSegBufWriteRequest(&rbytes);
        if (len > rbytes) {
            len = rbytes;
        }
        if (len) {
            memcpy(buf, data, len);
        }
        NutSegBufWriteLast(len);
    }
    return len;
}

#ifdef __HARVARD_ARCH__
/*!
 * \brief Write to device.
 *
 * Similar to VsWrite() except that the data is expected in program memory.
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
static int VsWrite_P(NUTFILE * nfp, PGM_P buffer, int len)
{
    return -1;
}
#endif

/*!
 * \brief Open codec device.
 *
 * \return Pointer to a static NUTFILE structure.
 */
static NUTFILE *VsOpen(NUTDEVICE * dev, CONST char *name, int mode, int acc)
{
    NUTFILE *nfp;

    VsRegWrite(VS_MODE_REG, VS_SM_RESET | VS_SM_SDINEW);
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
static int VsClose(NUTFILE * nfp)
{
    int rc = VsPlayerFlush();

    if (nfp) {
        free(nfp);
    }
    return rc;
}

static int VsPlayBufferInit(uint32_t size)
{
    if (dcb.dcb_pbstat != CODEC_STATUS_IDLE) {
        return -1;
    }
    if (NutSegBufInit((size_t)size) == NULL) {
        return -1;
    }
    dcb.dcb_pbwlo = NutSegBufAvailable() / 3;
    dcb.dcb_pbwhi = dcb.dcb_pbwlo * 2;

    return 0;
}

/*!
 * \brief Handle I/O controls for audio codec.
 *
 * - AUDIO_PLAY         Force playback start, even if the buffer level is too lower.
 * - AUDIO_CANCEL       Cancel playback and discard all buffered data.
 * - AUDIO_GET_STATUS   Sets an int to 1 if the player is running, 0 if idle.
 * - AUDIO_GET_PLAYGAIN Sets an int to the current playback gain, 0..-127.
 * - AUDIO_SET_PLAYGAIN Reads the requested playback gain from an int, 0..-127.
 * - AUDIO_GET_TREB     Reads audio enhancement treble value as int in 1.5dB steps
 * - AUDIO_SET_TREB     Sets audio enhancement treble value as int in 1.5dB steps
 * - AUDIO_GET_TFIN     Reads audio enhancement treable cutoff frequency in 1000Hz steps
 * - AUDIO_SET_TFIN     Sets audio enhancement treable cutoff frequency in 1000Hz steps
 * - AUDIO_GET_BASS     Reads audio enhancement bass value as int in 1dB steps
 * - AUDIO_SET_BASS     Sets audio enhancement bass value as int in 1dB steps
 * - AUDIO_GET_BFIN     Reads audio enhancement bass cutoff frequency in 10Hz steps
 * - AUDIO_SET_BFIN     Sets audio enhancement bass cutoff frequency in 10Hz steps
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
static int VsIOCtl(NUTDEVICE * dev, int req, void *conf)
{
    int rc = 0;
    uint32_t *lvp = (uint32_t *) conf;
    int *ivp = (int *) conf;
    int iv = *ivp;

    switch (req) {
    case AUDIO_PLAY:
        /* Immediately start playing. */
        if (dcb.dcb_pbstat != CODEC_STATUS_PLAYING) {
            dcb.dcb_scmd |= VSREQ_PLAY;
        }
        break;
    case AUDIO_CANCEL:
        /* Immediately stop playing and discard buffer. */
        if (dcb.dcb_pbstat == CODEC_STATUS_PLAYING) {
            dcb.dcb_scmd |= VSREQ_CANCEL;
        }
        break;
    case AUDIO_GET_STATUS:
        *ivp = dcb.dcb_pbstat;
        break;
    case AUDIO_GET_PLAYGAIN:
        if (dcb.dcb_crvol >= dcb.dcb_clvol) {
            *ivp = dcb.dcb_crvol;
        }
        else {
            *ivp = dcb.dcb_clvol;
        }
        break;
    case AUDIO_SET_PLAYGAIN:
        if (iv > AUDIO_DAC_MAX_GAIN) {
            iv = AUDIO_DAC_MAX_GAIN;
        }
        if (iv < AUDIO_DAC_MIN_GAIN) {
            iv = AUDIO_DAC_MIN_GAIN;
        }
        dcb.dcb_slvol = dcb.dcb_srvol = iv;
        break;
    case AUDIO_GET_TREB:
        *lvp = dcb.dcb_ctreb;
        break;
    case AUDIO_SET_TREB:
        if( iv > AUDIO_DAC_MAX_TREB) {
            iv = AUDIO_DAC_MAX_TREB;
        }
        dcb.dcb_streb = iv;
        break;
    case AUDIO_GET_TFIN:
        *lvp = dcb.dcb_ctfin;
        break;
    case AUDIO_SET_TFIN:
        if( iv > AUDIO_DAC_MAX_TFIN) {
            iv = AUDIO_DAC_MAX_TFIN;
        }
        dcb.dcb_stfin = iv;
        break;
    case AUDIO_GET_BASS:
        *lvp = dcb.dcb_cbass;
        break;
    case AUDIO_SET_BASS:
        if( iv > AUDIO_DAC_MAX_BASS) {
            iv = AUDIO_DAC_MAX_BASS;
        }
        dcb.dcb_sbass = iv;
    case AUDIO_GET_BFIN:
        *lvp = dcb.dcb_cbass;
        break;
    case AUDIO_SET_BFIN:
        if( iv > AUDIO_DAC_MAX_BFIN) {
            iv = AUDIO_DAC_MAX_BFIN;
        }
        dcb.dcb_sbass = iv;
    case AUDIO_GET_PBSIZE:
        *lvp = NutSegBufAvailable() + NutSegBufUsed();
        break;
    case AUDIO_SET_PBSIZE:
        rc = VsPlayBufferInit(*lvp);
        break;
    case AUDIO_GET_PBLEVEL:
        *lvp = NutSegBufUsed();
        break;
    case AUDIO_GET_PBWLOW:
        *lvp = dcb.dcb_pbwlo;
        break;
    case AUDIO_SET_PBWLOW:
        dcb.dcb_pbwlo = *lvp;
        break;
    case AUDIO_GET_PBWHIGH:
        *lvp = dcb.dcb_pbwhi;
        break;
    case AUDIO_SET_PBWHIGH:
        dcb.dcb_pbwhi = *lvp;
        break;
    case AUDIO_BEEP:
        dcb.dcb_scmd |= VSREQ_BEEP;
        break;
#if 0
    case AUDIO_GET_DECINFO:
        /* Retrieve decoder information. */
        break;
    case AUDIO_GET_DECCAPS:
        /* Retrieve decoder capabilities. */
        break;
    case AUDIO_GET_DECFMTS:
        /* Retrieve decoder formats. */
        break;
    case AUDIO_SET_DECFMTS:
        /* Enable or disable specific decoder formats. */
        break;
    case AUDIO_GET_CODINFO:
        /* Retrieve encoder information. */
        break;
    case AUDIO_GET_CODCAPS:
        /* Retrieve encoder capabilities. */
        break;
    case AUDIO_GET_CODFMTS:
        /* Retrieve encoder formats. */
        break;
    case AUDIO_SET_CODFMTS:
        /* Enable or disable specific encoder formats. */
        break;
    case AUDIO_GET_MIDINFO:
        /* Retrieve midi information. */
        break;
    case AUDIO_GET_MIDCAPS:
        /* Retrieve midi capabilities. */
        break;
#endif
    default:
        rc = -1;
        break;
    }
    return rc;
}

/*
 * Called via dev_init pointer when the device is registered.
 */
static int VsInit(NUTDEVICE * dev)
{
    uint16_t mode;

    /* Release reset line and read the status register. */
    outr(PIOB_PER, _BV(VS_XRESET_BIT));
    outr(PIOB_SODR, _BV(VS_XRESET_BIT));
    outr(PIOB_OER, _BV(VS_XRESET_BIT));
    NutSleep(3);

    VsRegRead(VS_MODE_REG); /* Dummy read. TODO: Why? */
    mode = VsRegRead(VS_MODE_REG);
    if ((mode & VS_SM_SDINEW) == 0) {
        VsRegWrite(VS_MODE_REG, VS_SM_RESET | VS_SM_SDINEW);
        NutSleep(2);
    }
    vs_chip = (VsRegRead(VS_STATUS_REG) & VS_SS_VER) >> VS_SS_VER_LSB;
#if VS10XX_FREQ < 20000000UL && defined(VS_CF_DOUBLER)
    VsRegWrite(VS_CLOCKF_REG, (uint16_t)(VS_CF_DOUBLER | (VS10XX_FREQ / 2000UL)));
#else
    VsRegWrite(VS_CLOCKF_REG, (uint16_t)(VS10XX_FREQ / 2000UL));
#endif
#if defined(VS_INT_FCTLH_REG)
    if (vs_chip == 0) {
        /* Force frequency change (see datasheet). */
        VsRegWrite(VS_INT_FCTLH_REG, 0x8008);
    }
#endif

    if (VsPlayBufferInit(0)) {
        return -1;
    }
    if (NutThreadCreate("vsdeco", FeederThread, NULL, 1024) == 0) {
        return -1;
    }
    return 0;
}

/*!
 * \brief VS10XX device information structure.
 *
 * An application must pass a pointer to this structure to
 * NutRegisterDevice() before using this driver.
 *
 * The device is named \b audio0.
 *
 * \showinitializer
 */
NUTDEVICE devVsCodec = {
    0,              /* Pointer to next device, dev_next. */
    {'a', 'u', 'd', 'i', 'o', '0', 0, 0, 0},    /* Unique device name, dev_name. */
    IFTYP_CHAR,     /* Type of device, dev_type. */
    0,              /* Base address, dev_base (not used). */
    0,              /* First interrupt number, dev_irq (not used). */
    0,              /* Interface control block, dev_icb (not used). */
    &dcb,           /* Driver control block, dev_dcb. */
    VsInit,         /* Driver initialization routine, dev_init. */
    VsIOCtl,        /* Driver specific control function, dev_ioctl. */
    NULL,           /* Read from device, dev_read. */
    VsWrite,        /* Write to device, dev_write. */
#ifdef __HARVARD_ARCH__
    VsWrite_P,      /* Write data from program space to device, dev_write_P. */
#endif
    VsOpen,         /* Open a device or file, dev_open. */
    VsClose,        /* Close a device or file, dev_close. */
    NULL            /* Request file size, dev_size. */
};

/*@}*/
