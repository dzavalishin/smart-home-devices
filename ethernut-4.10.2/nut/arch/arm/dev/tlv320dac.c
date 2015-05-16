/*!
 * Copyright (C) 2008 by egnite GmbH.
 * Copyright (C) 2007 by egnite Software GmbH.
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

/*!
 * \file tlv320dac.c
 * \brief DAC routines.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.3  2009/02/13 14:52:05  haraldkipp
 * Include memdebug.h for heap management debugging support.
 *
 * Revision 1.2  2009/01/17 11:26:37  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.1  2008/10/05 16:51:46  haraldkipp
 * Added suport for the TLV320 audio DAC.
 *
 *
 * \endverbatim
 */

#include <cfg/os.h>
#include <cfg/clock.h>
#include <dev/board.h>
#include <dev/irqreg.h>
#include <dev/twif.h>

#include <stdlib.h>
#include <string.h>
#include <memdebug.h>

#include <sys/event.h>
#include <sys/timer.h>

#include <dev/tlv320dac.h>

#ifndef TWI_SLA_DAC
#define TWI_SLA_DAC     0x1A
#endif

/*
 * Initial volume.
 */
#ifndef TLV320DAC_VOL
#define TLV320DAC_VOL   0x18
#endif

/*
 * Number of PCM buffers.
 */
#ifndef SAMPLE_BUFFERS
#if defined (AT91SAM9260_EK)
#define SAMPLE_BUFFERS  32
#else
#define SAMPLE_BUFFERS  3
#endif
#endif

/*
 * Select I2S pins.
 */
#if defined (MCU_AT91SAM9260) /* _EK */

#define DACI2S_PIO_ID   PIOB_ID
#define DACI2S_PINS_A   _BV(PB18_TD0_A) | _BV(PB17_TF0_A) | _BV(PB16_TK0_A)
#define DACI2S_PINS_B   0

#else                           /* _EK */
#define DACI2S_PIO_ID   PIOA_ID
#define DACI2S_PINS_A   _BV(PA23_TD_A) | _BV(PA21_TF_A) | _BV(PA22_TK_A)
#define DACI2S_PINS_B   0
#endif                          /* _EK */

/*
 * Determine PIO used by I2S.
 */
#if DACI2S_PIO_ID == PIOA_ID    /* DACI2S_PIO_ID */
#define DACI2S_PDR  PIOA_PDR
#define DACI2S_ASR  PIOA_ASR
#define DACI2S_BSR  PIOA_BSR
#elif DACI2S_PIO_ID == PIOB_ID  /* DACI2S_PIO_ID */
#define DACI2S_PDR  PIOB_PDR
#define DACI2S_ASR  PIOB_ASR
#define DACI2S_BSR  PIOB_BSR
#endif                          /* DACI2S_PIO_ID */

volatile uint32_t irq_counter;

/*!
 * \brief I2S event queue.
 */
static HANDLE i2s_que;

/*!
 * \brief Number of channels.
 */
#define PCM_CHANS   2

/*!
 * \brief Number of bits per sample.
 */
#define PCM_BITS    16

/*!
 * \brief PCM buffer type.
 */
typedef struct _PCM_BUFFER {
    u_short *wbf_dat;           /*!< Pointer to PCM data. */
    int wbf_siz;                /*!< Buffer size. */
    int wbf_len;                /*!< Number of valid bytes buffered. */
} PCM_BUFFER;

/* Enable PDC hardware. */
unsigned int use_pdc = 1;
/* PCM buffer queue. */
static PCM_BUFFER pcm_bufq[SAMPLE_BUFFERS];
/* PCM buffer read index. */
static volatile unsigned int brd_idx;
/* PCM buffer read position. */
static volatile int brd_pos;
/* PCM buffer write index. */
static unsigned int bwr_idx;

/*!
 * \brief Set PDC pointer and counter registers.
 *
 * SSC interrupts must have been disabled befor calling this function.
 */
static void I2sPdcFill(void)
{
    if (brd_idx != bwr_idx) {
        if (inr(SSC_TNCR) == 0) {
            if (++brd_idx >= SAMPLE_BUFFERS) {
                brd_idx = 0;
            }
            outr(SSC_TNPR, (unsigned int) pcm_bufq[brd_idx].wbf_dat);
            outr(SSC_TNCR, pcm_bufq[brd_idx].wbf_len);
        }
    }
}

/*!
 * \brief SSC interrupt handler.
 *
 * Called when PDC transmission done.
 */
static void I2sInterrupt(void *arg)
{
    irq_counter++;
    if (use_pdc) {
        I2sPdcFill();
        NutEventPostFromIrq(&i2s_que);
    } else {
        if (brd_pos >= pcm_bufq[brd_idx].wbf_len) {
            brd_pos = 0;
            NutEventPostFromIrq(&i2s_que);
            if (brd_idx == bwr_idx) {
                outr(SSC_THR, 0);
                return;
            }
            if (++brd_idx >= SAMPLE_BUFFERS) {
                brd_idx = 0;
            }
        }
        outr(SSC_THR, pcm_bufq[brd_idx].wbf_dat[brd_pos]);
        brd_pos++;
    }
}

/*!
 * \brief Read value from specified DAC register.
 *
 * Not implemented, because the TLV320AIC23B is a write-only device.
 *
 * \param reg DAC register address.
 *
 * \return Always 0xFF.
 */
u_char Tlv320DacReadReg(unsigned int reg)
{
    return 0xFF;
}

/*!
 * \brief Write value to specified DAC register.
 *
 * Communicates with the DAC chip via TWI.
 *
 * \param reg DAC register address.
 * \param val Value to store in specified register.
 */
void Tlv320DacWriteReg(unsigned int reg, unsigned int val)
{
    u_char txdata[2];

    txdata[0] = (u_char)(reg << 1) | (u_char)(val >> 8);
    txdata[1] = (u_char)val;
    TwMasterTransact(TWI_SLA_DAC, txdata, 2, NULL, 0, 0);
}

/*!
 * \brief Enable I2S interface with a given rate.
 *
 * \param rate PCM sample rate in Hertz.
 *
 * \return Always 0.
 */
static int Tlv320I2sEnable(unsigned int rate)
{
    /* Enable SSC clock. */
    outr(PMC_PCER, _BV(SSC_ID));

    /* Select SSC peripheral functions. */
    outr(DACI2S_ASR, DACI2S_PINS_A);
    outr(DACI2S_BSR, DACI2S_PINS_B);

    /* Enable SSC peripheral pins. */
    outr(DACI2S_PDR, DACI2S_PINS_A | DACI2S_PINS_B);

    /* Configure 16-bit stereo I2S transmit format. */
    outr(SSC_CMR, 0);
    outr(SSC_TCMR,              /* Set transmit clock mode. */
        SSC_CKS_PIN |            /* Use external clock at TK. */
        SSC_START_EDGE_RF |     /* Start transmission on any edge. */
        (1 << SSC_STTDLY_LSB)); /* Delay start by 1 cycle. */
    outr(SSC_TFMR,              /* Set transmit frame mode. */
        ((PCM_BITS - 1) << SSC_DATLEN_LSB) |   /* Transmit 16 bits. */
        SSC_MSBF);              /* Most significant bit first. */

    if (use_pdc) {
        /* Enable transmitter in PDC mode. */
        outr(SSC_PTCR, PDC_TXTEN);
    }
    outr(SSC_CR, SSC_TXEN);

    return 0;
}

/*!
 * \brief Disable I2S interface.
 *
 * \return Always 0.
 */
static int Tlv320I2sDisable(void)
{
    /* Disable all interrupts. */
    outr(SSC_IDR, 0xFFFFFFFF);

    /* Disable SSC interrupt. */
    NutIrqDisable(&sig_SSC);

    /* Disable SSC clock. */
    outr(PMC_PCDR, _BV(SSC_ID));

    /* Reset receiver and transmitter. */
    outr(SSC_CR, SSC_SWRST | SSC_RXDIS | SSC_TXDIS);
    outr(SSC_RCMR, 0);
    outr(SSC_RFMR, 0);
    outr(SSC_PTCR, PDC_RXTDIS);
    outr(SSC_PTCR, PDC_TXTDIS);
    outr(SSC_TNCR, 0);
    outr(SSC_TCR, 0);

    return 0;
}

/*!
 * \brief Configure the SSC for I2S mode.
 *
 * \param rate Sampling rate in Hertz.
 *
 * \return Always 0.
 */
static int Tlv320I2sInit(unsigned int rate)
{
    /* Register SSC interrupt handler. */
    NutRegisterIrqHandler(&sig_SSC, I2sInterrupt, 0);

    Tlv320I2sDisable();
    Tlv320I2sEnable(rate);

    /* Enable SSC interrupt. */
    NutIrqEnable(&sig_SSC);

    return 0;
}

int Tlv320DacSetRate(unsigned int rate)
{
    switch(rate) {
    case 8000:
#ifdef AT91SAM7X_EK
        Tlv320DacWriteReg(DAC_SRATE, (3 << DAC_SRATE_SR_LSB));
#else
        Tlv320DacWriteReg(DAC_SRATE, (3 << DAC_SRATE_SR_LSB) | DAC_SRATE_USB);
#endif
        break;
    case 8021:      
        Tlv320DacWriteReg(DAC_SRATE, (11 << DAC_SRATE_SR_LSB) | DAC_SRATE_BOSR | DAC_SRATE_USB);
        break;
    case 44100:     
        Tlv320DacWriteReg(DAC_SRATE, (8 << DAC_SRATE_SR_LSB) | DAC_SRATE_BOSR | DAC_SRATE_USB);
        break;
    case 48000:     
        Tlv320DacWriteReg(DAC_SRATE, (0 << DAC_SRATE_SR_LSB) | DAC_SRATE_USB);
        break;
    case 88200:     
        Tlv320DacWriteReg(DAC_SRATE, (15 << DAC_SRATE_SR_LSB) | DAC_SRATE_BOSR | DAC_SRATE_USB);
        break;
    case 96000:     
        Tlv320DacWriteReg(DAC_SRATE, (7 << DAC_SRATE_SR_LSB) | DAC_SRATE_USB);
        break;
    default:        
        return -1;
    }
    return 0;
}

/*!
 * \brief Initialize TLV320AIC23B DAC interface.
 *
 * \param rate Sample rate.
 *
 * \return 0 on success, -1 otherwise.
 */
int Tlv320DacInit(unsigned int rate)
{
    /* Initialize TWI. */
    TwInit(0);

    Tlv320DacWriteReg(DAC_RESET, 0);
    /* Power down line in. */
    Tlv320DacWriteReg(DAC_PWRDN, DAC_PWRDN_LINE);
    Tlv320DacWriteReg(DAC_PWRDN, 0);
    /* Set sampling rate. */
    if (Tlv320DacSetRate(rate)) {
        Tlv320DacWriteReg(DAC_RESET, 0);
        return -1;
    }
    Tlv320DacWriteReg(DAC_ANA_PATH, DAC_ANA_PATH_DAC | DAC_ANA_PATH_INSEL | DAC_ANA_PATH_MICB);
    Tlv320DacWriteReg(DAC_DIG_PATH, 0);
    /* I2S master. */
    Tlv320DacWriteReg(DAC_DAI_FMT, DAC_DAI_FMT_MS | DAC_DAI_FMT_FOR_I2S);
    Tlv320DacWriteReg(DAC_DI_ACT, DAC_DI_ACT_ACT);

    Tlv320DacWriteReg(DAC_LHP_VOL, DAC_LHP_VOL_LRS | (0x60 << DAC_LHP_VOL_LHV_LSB));

    /* Initialize I2S. */
    return Tlv320I2sInit(rate);
}

/*!
 * \brief Start transmitting audio samples.
 *
 * \return Always 0.
 */
static int Tlv320DacStart(void)
{
    NutIrqDisable(&sig_SSC);
    if (use_pdc) {
        outr(SSC_IDR, SSC_TXEMPTY);
        /* Enable transmitter in PDC mode. */
        outr(SSC_PTCR, PDC_TXTEN);
        I2sPdcFill();
        outr(SSC_IER, SSC_ENDTX);
    } else {
        outr(SSC_IDR, SSC_ENDTX);
        /* Disable transmitter in PDC mode. */
        outr(SSC_PTCR, PDC_TXTDIS);
        outr(SSC_IER, SSC_TXEMPTY);
    }
    /* Enable transmitter. */
    outr(SSC_CR, SSC_TXEN);
    NutIrqEnable(&sig_SSC);

    return 0;
}

/*!
 * \brief Wait until all buffered samples have been transmitted.
 *
 * \return Always 0.
 */
int Tlv320DacFlush(void)
{
    int rc = 0;

    while (bwr_idx != brd_idx) {
        Tlv320DacStart();
        if ((rc = NutEventWait(&i2s_que, 500)) != 0) {
            break;
        }
    }
    return rc;
}

/*!
 * \brief Add audio samples to the TLV320AIC23B transmit queue.
 *
 * \param buf Points to PCM data.
 * \param len Number of samples.
 *
 * \return 0 on success or -1 if out of memory.
 */
int Tlv320DacWrite(void *buf, int len)
{
    unsigned int idx;

    /* Move to the next buffer to write to. */
    idx = bwr_idx + 1;
    if (idx >= SAMPLE_BUFFERS) {
        idx = 0;
    }

    /* If all buffers are filled, wait for an event posted by the
       interrupt routine. */
    while (idx == brd_idx) {
        if (NutEventWait(&i2s_que, 100)) {
            Tlv320DacStart();
        }
    }

    /*
     * Check, if the current buffer size is too small or not allocated.
     */
    if (pcm_bufq[idx].wbf_siz < len) {
        if (pcm_bufq[idx].wbf_siz) {
            free(pcm_bufq[idx].wbf_dat);
            pcm_bufq[idx].wbf_siz = 0;
        }
        pcm_bufq[idx].wbf_dat = malloc(len * 2);
        if (pcm_bufq[idx].wbf_dat == NULL) {
            /* Out of memory. */
            return -1;
        }
        pcm_bufq[idx].wbf_siz = len;
    }

    /*
     * At this point we got an available buffer with sufficient size.
     * Move the data to it, set the number of valid bytes and update
     * the write (producer) index.
     */
    memcpy(pcm_bufq[idx].wbf_dat, buf, len * 2);
    pcm_bufq[idx].wbf_len = len;
    bwr_idx = idx;

    return 0;
}

/*!
 * \brief Set volume.
 *
 * Sets the master playback gain. Range is +6..-73 dB.
 *
 * \param left  Left channel gain in dB.
 * \param right Right channel gain in dB.
 *
 * \return 0 on success, -1 otherwise.
 */
int Tlv320DacSetVolume(int left, int right)
{
    /* Cut to limits. */
    if (left > DAC_MAX_VOLUME) {
        left = DAC_MAX_VOLUME;
    }
    else if (left < DAC_MIN_VOLUME) {
        left = DAC_MIN_VOLUME;
    }
    if (right > DAC_MAX_VOLUME) {
        right = DAC_MAX_VOLUME;
    }
    else if (right < DAC_MIN_VOLUME) {
        right = DAC_MIN_VOLUME;
    }
    Tlv320DacWriteReg(DAC_LHP_VOL, (unsigned int)(left + 121));
    Tlv320DacWriteReg(DAC_RHP_VOL, (unsigned int)(right + 121));

    return 0;
}

int Tlv320SwitchMode(void)
{
    NutIrqDisable(&sig_SSC);
    use_pdc = !use_pdc;
    Tlv320DacStart();
    return 0;
}
