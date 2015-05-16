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
 * $Id: spi_vscodec0.c 2702 2009-09-17 11:01:21Z haraldkipp $
 */

#include <dev/gpio.h>
#include <dev/irqreg.h>
#include <dev/vscodec.h>
#include <dev/spibus.h>

#include <sys/event.h>
#include <sys/timer.h>
#include <sys/heap.h>
#include <sys/nutdebug.h>
#include <sys/bankmem.h>

#include <stdlib.h>
#include <string.h>

#if defined(VSCODEC0_SIGNAL_IRQ) && defined(VSCODEC0_DREQ_BIT) && defined(VSCODEC0_XCS_BIT)
#define VSCODEC0_CONFIGURED 1
#endif

#if VSCODEC0_CONFIGURED

#ifndef VSCODEC0_DREQ_PORT
#define VSCODEC0_DREQ_PORT  0
#endif
#ifndef VSCODEC0_XCS_PORT
#define VSCODEC0_XCS_PORT   0
#endif

/*!
 * \addtogroup xgVsCodec
 */
/*@{*/

#ifndef NUT_THREAD_VSCODEC0STACK
#if defined(__AVR__)
#if defined(__GNUC__)
/* avr-gcc optimized code used 88 bytes. */
#define NUT_THREAD_VSCODEC0STACK    128
#else
/* icc-avr stack usage is unknown. */
#define NUT_THREAD_VSCODEC0STACK    384
#endif
#else
/* arm-elf-gcc optimized code used 112 bytes. */
#define NUT_THREAD_VSCODEC0STACK    192
#endif
#endif

#ifndef VSCODEC0_HWRST_DURATION
/*! \brief Minimum time in milliseconds to held hardware reset low. */
#define VSCODEC0_HWRST_DURATION     1
#endif

#ifndef VSCODEC0_FREQ
/*! \brief Decoder crystal frequency. */
#define VSCODEC0_FREQ               12288000UL
#endif

#ifndef VSCODEC0_SPI_RATE
/*! \brief Decoder's initial SPI rate. */
#define VSCODEC0_SPI_RATE           (VSCODEC0_FREQ / 8)
#endif

#ifndef VSCODEC0_SPI_MODE
/*! \brief Decoder's SPI mode. */
#define VSCODEC0_SPI_MODE           SPI_MODE_0
#endif

#ifndef VSCODEC0_SPIBUS_WAIT
/*! \brief Number of milliseconds to wait for SPI bus timeout. */
#define VSCODEC0_SPIBUS_WAIT        NUT_WAIT_INFINITE
#endif

#ifndef VSCODEC0_MAX_OUTPUT_BUFSIZ
/*! \brief Output buffer size limit. */
#define VSCODEC0_MAX_OUTPUT_BUFSIZ  16384
#endif

/*
 * Determine interrupt settings.
 */
#if (VSCODEC0_SIGNAL_IRQ == NUTGPIO_EXTINT0)
#define VSCODEC0_DREQ_SIGNAL  sig_INTERRUPT0
#elif (VSCODEC0_SIGNAL_IRQ == NUTGPIO_EXTINT1)
#define VSCODEC0_DREQ_SIGNAL  sig_INTERRUPT1
#elif (VSCODEC0_SIGNAL_IRQ == NUTGPIO_EXTINT2)
#define VSCODEC0_DREQ_SIGNAL  sig_INTERRUPT2
#elif (VSCODEC0_SIGNAL_IRQ == NUTGPIO_EXTINT3)
#define VSCODEC0_DREQ_SIGNAL  sig_INTERRUPT3
#elif (VSCODEC0_SIGNAL_IRQ == NUTGPIO_EXTINT4)
#define VSCODEC0_DREQ_SIGNAL  sig_INTERRUPT4
#elif (VSCODEC0_SIGNAL_IRQ == NUTGPIO_EXTINT5)
#define VSCODEC0_DREQ_SIGNAL  sig_INTERRUPT5
#elif (VSCODEC0_SIGNAL_IRQ == NUTGPIO_EXTINT6)
#define VSCODEC0_DREQ_SIGNAL  sig_INTERRUPT6
#elif (VSCODEC0_SIGNAL_IRQ == NUTGPIO_EXTINT7)
#define VSCODEC0_DREQ_SIGNAL  sig_INTERRUPT7
#endif

/*!
 * \brief VS10xx SPI node implementation structure.
 */
static NUTSPINODE nodeSpiVsCodec0 = {
    NULL,               /*!< \brief Pointer to the bus controller driver, node_bus. */
    NULL,               /*!< \brief Pointer to device driver specific settings, node_stat. */
    VSCODEC0_SPI_RATE,  /*!< \brief Initial clock rate, node_rate. */
    VSCODEC0_SPI_MODE,  /*!< \brief Initial mode, node_mode. */
    8,                  /*!< \brief Initial data bits, node_bits. */
    0                   /*!< \brief Chip select, node_cs. */
};

static VSDCB dcbVsCodec0;

/*
 * VLSI codec 0 interrupt handler.
 *
 * \param arg Pointer to an event queue.
 */
static void VsCodec0Interrupt(void *arg)
{
    NutEventPostFromIrq((void **)arg);
}

/*!
 * \brief Check if codec 0 is ready.
 *
 * \return 0 if not.
 */
static int VsCodec0IsReady(void)
{
    return GpioPinGet(VSCODEC0_DREQ_PORT, VSCODEC0_DREQ_BIT) != 0;
}

/*
 * \brief Write to command channel of codec 0.
 *
 * This routine will not check the DREQ line.
 *
 * \param cmd  Points to the buffer. On entry it contains the data to 
 *             send. On exit it will contain the data received from
 *             the chip.
 * \param len  Number of bytes to send and receive.
 */
static int VsCodec0SendCmd(void *cmd, size_t len)
{
    int rc;

    /* Allocate the SPI bus. */
    rc = (*nodeSpiVsCodec0.node_bus->bus_alloc) (&nodeSpiVsCodec0, VSCODEC0_SPIBUS_WAIT);
    if (rc == 0) {
        /* Activate chip selects. */
#if defined(VSCODEC0_VSCS_PORT) && defined(VSCODEC0_VSCS_BIT)
        GpioPinSetLow(VSCODEC0_VSCS_PORT, VSCODEC0_VSCS_BIT);
#endif
        GpioPinSetLow(VSCODEC0_XCS_PORT, VSCODEC0_XCS_BIT);
        /* Send command bytes and receive response. */
        rc = (*nodeSpiVsCodec0.node_bus->bus_transfer) (&nodeSpiVsCodec0, cmd, cmd, len);
        /* Dectivate chip selects. */
        GpioPinSetHigh(VSCODEC0_XCS_PORT, VSCODEC0_XCS_BIT);
#if defined(VSCODEC0_VSCS_PORT) && defined(VSCODEC0_VSCS_BIT)
        GpioPinSetHigh(VSCODEC0_VSCS_PORT, VSCODEC0_VSCS_BIT);
#endif
        /* Release the SPI bus. */
        (*nodeSpiVsCodec0.node_bus->bus_release) (&nodeSpiVsCodec0);
    }
    return rc;
}

/*
 * \brief Write data to the decoder.
 *
 * Data is sent in fixed chunks. The first one is sent without
 * checking the DREQ line. Any subsequent chunk is sent only if
 * the DREQ line is still high.
 *
 * \param node Specifies the SPI node.
 * \param buf  Points to the data. May be NULL, in which case the routine
 *             will send the specified number of zeros.
 * \param len  Number of bytes to send.
 *
 * \return The number of bytes actually sent. This may be less than the
 *         specified data length. Zero is returned in case of an error.
 */
static int VsCodec0SendData(CONST uint8_t *buf, size_t len)
{
    int rc = 0;
    CONST uint8_t *bp;
    size_t chunk;

    /* Allocate the SPI bus. */
    if ((*nodeSpiVsCodec0.node_bus->bus_alloc) (&nodeSpiVsCodec0, VSCODEC0_SPIBUS_WAIT) == 0) {
#if defined(VSCODEC0_VSCS_PORT) && defined(VSCODEC0_VSCS_BIT)
        /* Activate optional VSCS line. */
        GpioPinSetLow(VSCODEC0_VSCS_PORT, VSCODEC0_VSCS_BIT);
#endif
#if defined(VSCODEC0_XDCS_PORT) && defined(VSCODEC0_XDCS_BIT)
        /* Activate optional XDCS line. */
        GpioPinSetLow(VSCODEC0_XDCS_PORT, VSCODEC0_XDCS_BIT);
#endif
        /* Set our internal buffer pointer, either to the start of the 
        ** encoded data or to a chunk of zeros. */
        bp = buf ? buf : zero_chunk;
        /* Loop until all data had been sent or DREQ goes low. */
        while (len) {
            /* Determine chunk size. */
            chunk = len > VSCODEC_DATA_CHUNK_SIZE ? VSCODEC_DATA_CHUNK_SIZE : len;
            /* Send the chunk, exit on errors. */
            if ((*nodeSpiVsCodec0.node_bus->bus_transfer) (&nodeSpiVsCodec0, bp, NULL, chunk)) {
                break;
            }
            /* Update function result. */
            rc += chunk;
            /* Check if decoder is ready for more. Exit, if not. */
            if (!VsCodec0IsReady()) {
                break;
            }
            /* Update remaining number of bytes and buffer pointer. */
            len -= chunk;
            if (buf) {
                bp += chunk;
            }
        }
#if defined(VSCODEC0_XDCS_PORT) && defined(VSCODEC0_XDCS_BIT)
        /* Deactivate optional XDCS line. */
        GpioPinSetHigh(VSCODEC0_XDCS_PORT, VSCODEC0_XDCS_BIT);
#endif
#if defined(VSCODEC0_VSCS_PORT) && defined(VSCODEC0_VSCS_BIT)
        /* Deactivate optional VSCS line. */
        GpioPinSetHigh(VSCODEC0_VSCS_PORT, VSCODEC0_VSCS_BIT);
#endif
        /* Release the SPI bus. */
        (*nodeSpiVsCodec0.node_bus->bus_release) (&nodeSpiVsCodec0);
    }
    return rc;
}

/*!
 * \brief Handle I/O controls for audio codec.
 *
 * \param dev Specifies the audio codec device.
 *
 * \return 0 on success, -1 otherwise.
 */
static int VsCodec0Control(int req, void *conf)
{
    int rc = 0;
    uint32_t *lvp = (uint32_t *) conf;

    switch (req) {
    case AUDIO_SET_DECFMTS:
        /* Enable or disable specific decoder formats. */
#if defined(VS_SM_LAYER12)
        if (*lvp & (AUDIO_FMT_MPEG1_L1 | AUDIO_FMT_MPEG1_L2)) {
            VsCodecMode(&devSpiVsCodec0, VS_SM_LAYER12, VS_SM_LAYER12);
        } else {
            VsCodecMode(&devSpiVsCodec0, 0, VS_SM_LAYER12);
        }
#endif
        break;
    case AUDIO_GET_DECFMTS:
        /* Retrieve decoder formats. */
        *lvp = VS_DECODER_CAPS;
#if defined(VS_SM_LAYER12)
        {
            uint16_t mode = VsCodecMode(&devSpiVsCodec0, 0, 0);

            if ((mode & VS_SM_LAYER12) == 0) {
                *lvp &= ~(AUDIO_FMT_MPEG1_L1 | AUDIO_FMT_MPEG1_L2);
            }
        }
#endif
        break;
    case AUDIO_GET_CODFMTS:
        /* Retrieve encoder formats. */
        *lvp = VS_ENCODER_CAPS;
#if defined(VS_SM_ADPCM)
        {
            uint16_t mode = VsCodecMode(&devSpiVsCodec0, 0, 0);

            if ((mode & VS_SM_ADPCM) == 0) {
                *lvp &= ~(AUDIO_FMT_WAV_ADPCM | AUDIO_FMT_WAV_IMA_ADPCM);
            }
        }
#endif
        break;
    case AUDIO_SET_CODFMTS:
        /* Enable or disable specific encoder formats. */
        if (*lvp & AUDIO_FMT_VORBIS) {
        }
        break;
    case AUDIO_IRQ_ENABLE:
        if (*lvp) {
            NutIrqEnable(&VSCODEC0_DREQ_SIGNAL);
        } else {
            NutIrqDisable(&VSCODEC0_DREQ_SIGNAL);
        }
        break;
    default:
        rc = -1;
        break;
    }
    return rc;
}

/*!
 * \brief Set VLSI audio codec clock.
 *
 * \param dev Specifies the audio codec device.
 * \param xtal Clock at the chip's XTAL input in Hertz.
 * \param dreq If zero, then the routine will ignore the status of the
 *             DREQ input. This is useful for early clock configuration
 *             immediately after releasing hardware reset, but will be
 *             ignored on chips earlier than VS103x.
 *
 * \return 0 on success, -1 otherwise.
 */
static int VsCodec0SetClock(uint32_t xtal, uint_fast8_t dreq)
{
    int rc = 0;

#if VS_HAS_SC_X3FREQ
    /* This hardware got the new CLOCKF register layout. */
    if (dreq) {
        /* Honor the DREQ line, if requested. */
        VsCodecWaitReady(&devSpiVsCodec0, VSCODEC_CMD_TIMEOUT);
    }
    {
        uint8_t cmd[4] = { VS_OPCODE_WRITE, VS_CLOCKF_REG, 0, 0 };
        uint16_t freq = (uint16_t)((xtal - 8000000UL) / 4000UL);

        freq |= (VS_SC_MULT_3_5 | VS_SC_ADD_1_0);
        cmd[2] = (uint8_t) (freq >> 8);
        cmd[3] = (uint8_t) freq;
        rc = VsCodec0SendCmd(cmd, 4);
    }
#else
    /* Old hardware, requires clock doubler with lower frequence crystal. */
    if (xtal < 20000000UL) {
        VsCodecReg(&devSpiVsCodec0, VS_OPCODE_WRITE, VS_CLOCKF_REG, (uint16_t)(VS_CF_DOUBLER | (xtal / 2000UL)));
    } else {
        VsCodecReg(&devSpiVsCodec0, VS_OPCODE_WRITE, VS_CLOCKF_REG, (uint16_t)(xtal / 2000UL));
    }
#endif

    /* Force frequency change (see VS1001 datasheet). */
#if defined(AUDIO0_VS1001K)
    VsCodecReg(&devSpiVsCodec0, VS_OPCODE_WRITE, VS_INT_FCTLH_REG, 0x8008);
#elif defined(AUDIO0_VSAUTO) && defined(VS_INT_FCTLH_REG)
    if (dcbVsCodec0.dcb_codec_ver == 1001) {
        VsCodecReg(&devSpiVsCodec0, VS_OPCODE_WRITE, VS_INT_FCTLH_REG, 0x8008);
    }
#endif

    /* With higher clock we can increase the SPI rate. */
    if (rc == 0) {
#if VS_HAS_SC_X3FREQ
        (*nodeSpiVsCodec0.node_bus->bus_set_rate) (&nodeSpiVsCodec0, xtal / 2);
#endif
    }
    return rc;
}

/*!
 * \brief Detect VLSI audio codec version.
 *
 * If the codec type had been pre-configured, this routine will verify
 * the hardware. Otherwise it will try to determine the codec by reading
 * the version info in the status register.
 *
 * \param dev Specifies the audio codec device.
 *
 * \return 0 on success, -1 otherwise.
 */
static int VsCodec0Detect(void)
{
    int rc = -1;
    uint_fast16_t status;

    /*
    ** Verify the hardware if chip is pre-configured.
    */
    status = VsCodecReg(&devSpiVsCodec0, VS_OPCODE_READ, VS_STATUS_REG, 0);
#if defined(AUDIO0_VS1001K)
    if ((status & VS_SS_VER) == (VS1001_SS_VER << VS_SS_VER_LSB)) {
        dcbVsCodec0.dcb_codec_ver = 1001;
        dcbVsCodec0.dcb_codec_rev = 'K';
        rc = 0;
    }
#elif defined(AUDIO0_VS1011E)
    if ((status & VS_SS_VER) == (VS1011E_SS_VER << VS_SS_VER_LSB)) {
        dcbVsCodec0.dcb_codec_ver = 1011;
        dcbVsCodec0.dcb_codec_rev = 'E';
        rc = 0;
    }
#elif defined(AUDIO0_VS1002D)
    if ((status & VS_SS_VER) == (VS1002_SS_VER << VS_SS_VER_LSB)) {
        dcbVsCodec0.dcb_codec_ver = 1002;
        dcbVsCodec0.dcb_codec_rev = 'D';
        rc = 0;
    }
#elif defined(AUDIO0_VS1003B)
    if ((status & VS_SS_VER) == (VS1003_SS_VER << VS_SS_VER_LSB)) {
        dcbVsCodec0.dcb_codec_ver = 1003;
        dcbVsCodec0.dcb_codec_rev = 'B';
        rc = 0;
    }
#elif defined(AUDIO0_VS1033C)
    if ((status & VS_SS_VER) == (VS1033_SS_VER << VS_SS_VER_LSB)) {
        dcbVsCodec0.dcb_codec_ver = 1033;
        dcbVsCodec0.dcb_codec_rev = 'C';
        rc = 0;
    }
#elif defined(AUDIO0_VS1053B)
    if ((status & VS_SS_VER) == (VS1053_SS_VER << VS_SS_VER_LSB)) {
        dcbVsCodec0.dcb_codec_ver = 1053;
        dcbVsCodec0.dcb_codec_rev = 'B';
        rc = 0;
    }
#else
    /*
    ** If not configured, try to figure it out.
    */
    rc = 0;
    switch ((status & VS_SS_VER) >> VS_SS_VER_LSB) {
    case VS1001_SS_VER:
        dcbVsCodec0.dcb_codec_ver = 1001;
        break;
    case VS1011_SS_VER:
        dcbVsCodec0.dcb_codec_ver = 1011;
        break;
    case VS1002_SS_VER:
        if (VsCodecReg(&devSpiVsCodec0, VS_OPCODE_READ, VS_MODE_REG, 0) & VS_SM_SDINEW) {
            dcbVsCodec0.dcb_codec_ver = 1002;
        } else {
            dcbVsCodec0.dcb_codec_ver = 1011;
            dcbVsCodec0.dcb_codec_rev = 'E';
        }
        break;
    case VS1003_SS_VER:
        dcbVsCodec0.dcb_codec_ver = 1003;
        break;
    case VS1053_SS_VER:
        dcbVsCodec0.dcb_codec_ver = 1053;
        break;
    case VS1033_SS_VER:
        dcbVsCodec0.dcb_codec_ver = 1033;
        break;
    case VS1103_SS_VER:
        dcbVsCodec0.dcb_codec_ver = 1103;
        break;
    default:
        rc = -1;
        break;
    }
#endif
    return rc;
}

/*!
 * \brief VS10XX hardware reset.
 *
 * This routine requires, that the codecs reset line is tied to a GPIO
 * pin. However, VsCodec0ResetHardware(0) may be called during system boot
 * to recover from power-on reset.
 *
 * \param dev Specifies the audio codec device.
 * \param on  If zero, the hardware reset will be released. Otherwise 
 *            the reset is activated.
 */
static int VsCodec0ResetHardware(int on)
{
    if (on) {
        /*
        ** Activate reset.
        */
#if defined(VSCODEC0_XRESET_PORT) && defined(VSCODEC0_XRESET_BIT)
        /* Activate the reset line. */
        GpioPinSetLow(VSCODEC0_XRESET_PORT, VSCODEC0_XRESET_BIT);
        GpioPinConfigSet(VSCODEC0_XRESET_PORT, VSCODEC0_XRESET_BIT, GPIO_CFG_OUTPUT);
        NutSleep(VSCODEC0_HWRST_DURATION);
#else
        return -1;
#endif
    } else {
        /*
        ** Deactivate reset.
        */
        uint_fast8_t clkset = 0;

#if defined(VSCODEC0_XRESET_PORT) && defined(VSCODEC0_XRESET_BIT)
        /* Release the reset line. */
        GpioPinSetHigh(VSCODEC0_XRESET_PORT, VSCODEC0_XRESET_BIT);
#if VSCODEC0_FREQ >= 24000000UL
        /* With input clocks equal or above 24MHz we must set CLOCKF early 
        ** and must not wait for rising DREQ. */
        csrc = VsCodec0SetClock(VSCODEC0_FREQ, 0) == 0;
#endif /* VSCODEC0_FREQ */
#endif /* VSCODEC0_XRESET_PORT */

#if VSCODEC0_HWRST_RECOVER
        /* Optional delay after hardware reset. */
        NutSleep(VSCODEC0_HWRST_RECOVER);
#endif
        /* Chip hardware detection. */
        if (VsCodec0Detect()) {
            return -1;
        }
#if !defined(AUDIO0_VS1001K)
        if (dcbVsCodec0.dcb_codec_ver != 1001) {
#if VS_HAS_SM_SDINEW
#if defined(VSCODEC0_XDCS_PORT) && defined(VSCODEC0_XDCS_BIT)
            VsCodecMode(&devSpiVsCodec0, VS_SM_SDINEW, VS_SM_SDINEW);
#else
            VsCodecMode(&devSpiVsCodec0, VS_SM_SDINEW | VS_SM_SDISHARE, VS_SM_SDINEW | VS_SM_SDISHARE);
#endif
#endif /* VS_HAS_SM_SDINEW */
        }
#endif /* AUDIO0_VS1001K */

        /* Set clock now, if not done successfully before. */
        if (!clkset) {
            VsCodec0SetClock(VSCODEC0_FREQ, 1);
        }
    }
    return 0;
}


/*
 * Called via dev_init pointer when the device is registered.
 *
 * \param dev Specifies the audio codec device.
 *
 * \return 0 on success, -1 otherwise.
 */
static int VsCodec0Init(NUTDEVICE * dev)
{
    size_t avail;

    /* Activate hardware reset. */
    VsCodec0ResetHardware(1);

    /* Set function pointers. */
    dcbVsCodec0.dcb_isready = VsCodec0IsReady;
    dcbVsCodec0.dcb_sendcmd = VsCodec0SendCmd;
    dcbVsCodec0.dcb_senddata = VsCodec0SendData;
    dcbVsCodec0.dcb_control = VsCodec0Control;

    /* Set capabilities. */
#ifdef VS_DECODER_CAPS
    dcbVsCodec0.dcb_dec_caps = VS_DECODER_CAPS;
#endif
#ifdef VS_ENCODER_CAPS
    dcbVsCodec0.dcb_cod_caps = VS_ENCODER_CAPS;
#endif
#ifdef VS_MIDI_CAPS
    dcbVsCodec0.dcb_midi_caps = VS_MIDI_CAPS;
#endif

    /* Initialize DREQ input. Will be later used as an external interupt. */
    GpioPinConfigSet(VSCODEC0_DREQ_PORT, VSCODEC0_DREQ_BIT, 0);
    /* Initialize chip selects, XCS and optional XDCS. */
    GpioPinSetHigh(VSCODEC0_XCS_PORT, VSCODEC0_XCS_BIT);
    GpioPinConfigSet(VSCODEC0_XCS_PORT, VSCODEC0_XCS_BIT, GPIO_CFG_OUTPUT);
#if defined(VSCODEC0_XDCS_PORT) && defined(VSCODEC0_XDCS_BIT)
    GpioPinSetHigh(VSCODEC0_XDCS_PORT, VSCODEC0_XDCS_BIT);
    GpioPinConfigSet(VSCODEC0_XDCS_PORT, VSCODEC0_XDCS_BIT, GPIO_CFG_OUTPUT);
#endif
#if defined(VSCODEC0_VSCS_PORT) && defined(VSCODEC0_VSCS_BIT)
    GpioPinSetHigh(VSCODEC0_VSCS_PORT, VSCODEC0_VSCS_BIT);
    GpioPinConfigSet(VSCODEC0_VSCS_PORT, VSCODEC0_VSCS_BIT, GPIO_CFG_OUTPUT);
#endif

    /* Register the DREQ interrupt routine. */
    NutRegisterIrqHandler(&VSCODEC0_DREQ_SIGNAL, VsCodec0Interrupt, &dcbVsCodec0.dcb_feedme);

    /* Rising edge will generate an interrupt. */
    GpioPinConfigSet(VSCODEC0_DREQ_PORT, VSCODEC0_DREQ_BIT, GPIO_CFG_DISABLED);
    NutIrqSetMode(&VSCODEC0_DREQ_SIGNAL, NUT_IRQMODE_RISINGEDGE);
    NutIrqEnable(&VSCODEC0_DREQ_SIGNAL);

    /* Deactivate hardware reset. */
    if (VsCodec0ResetHardware(0)) {
        /* Probably failed to detect the hardware. */
        return -1;
    }

    /*
    ** Initialize the decoder stream buffer.
    */
#ifdef VSCODEC0_OUTPUT_BUFSIZ
    avail = VSCODEC0_OUTPUT_BUFSIZ;
#else
    avail = NutHeapAvailable() / 2;
    if (avail > VSCODEC0_MAX_OUTPUT_BUFSIZ) {
        avail = VSCODEC0_MAX_OUTPUT_BUFSIZ;
    }
#endif
    if (VsDecoderBufferInit(dev, avail)) {
        return -1;
    }

    /* Start the feeder thread. */
    if (NutThreadCreate(dev->dev_name, FeederThread, dev, 
        (NUT_THREAD_VSCODEC0STACK * NUT_THREAD_STACK_MULT) + NUT_THREAD_STACK_ADD) == 0) {
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
NUTDEVICE devSpiVsCodec0 = {
    0,                  /* Pointer to next device, dev_next. */
    {'a', 'u', 'd', 'i', 'o', '0', 0, 0, 0},    /* Unique device name, dev_name. */
    IFTYP_CHAR,         /* Type of device, dev_type. */
    0,                  /* Codec number, dev_base. */
    0,                  /* First interrupt number, dev_irq (not used). */
    &nodeSpiVsCodec0,   /* Interface control block, dev_icb (not used). */
    &dcbVsCodec0,       /* Driver control block, dev_dcb. */
    VsCodec0Init,       /* Driver initialization routine, dev_init. */
    VsCodecIOCtl,       /* Driver specific control function, dev_ioctl. */
    VsCodecRead,        /* Read from device, dev_read. */
    VsCodecWrite,       /* Write to device, dev_write. */
#ifdef __HARVARD_ARCH__
    VsCodecWrite_P,     /* Write data from program space to device, dev_write_P. */
#endif
    VsCodecOpen,        /* Open a device or file, dev_open. */
    VsCodecClose,       /* Close a device or file, dev_close. */
    NULL                /* Request file size, dev_size. */
};

/*@}*/

#endif
