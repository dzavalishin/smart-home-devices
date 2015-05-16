#ifndef _DEV_VSCODEC_H_
#define _DEV_VSCODEC_H_
/*
 * Copyright (C) 2009 by egnite GmbH
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
 * $Id: vscodec.h 2661 2009-09-01 18:10:54Z haraldkipp $
 */

#include <cfg/audio.h>
#include <sys/device.h>

#include <stdint.h>

/* For backward compatibility. */
#if defined(AUDIO_VS1001K)
#define AUDIO0_VS1001K
#elif defined(AUDIO_VS1011E)
#define AUDIO0_VS1011E
#elif defined(AUDIO_VS1002D)
#define AUDIO0_VS1002D
#elif defined(AUDIO_VS1003B)
#define AUDIO0_VS1003B
#elif defined(AUDIO_VS1033C)
#define AUDIO0_VS1033C
#elif defined(AUDIO_VS1053B)
#define AUDIO0_VS1053B
#elif defined(AUDIO_VSAUTO)
#define AUDIO0_VSAUTO
#endif

/*
** Include codec features.
*/
#if defined(AUDIO0_VS1001K)
#include <dev/vs1001.h>
#elif defined(AUDIO0_VS1002D)
#include <dev/vs1002.h>
#elif defined(AUDIO0_VS1003B)
#include <dev/vs1003.h>
#elif defined(AUDIO0_VS1011E)
#include <dev/vs1011.h>
#elif defined(AUDIO0_VS1033C)
#include <dev/vs1033.h>
#elif defined(AUDIO0_VS1053B)
#include <dev/vs1053.h>
#elif !defined(AUDIO0_VSAUTO)
#define AUDIO0_VSAUTO
#endif

/*!
 * \file dev/vscodec.h
 * \brief VLSI decoder definitions.
 */

/*!
 * \addtogroup xgVsCodec
 */
/*@{*/

/* DAC min/max gain in dB. */
#ifndef AUDIO0_DAC_MIN_GAIN
#define AUDIO0_DAC_MIN_GAIN      -127
#endif
#ifndef AUDIO0_DAC_MAX_GAIN
#define AUDIO0_DAC_MAX_GAIN      0
#endif

/* Backward compatibility, still used. */
#ifndef AUDIO_DAC_MIN_GAIN
#define AUDIO_DAC_MIN_GAIN      AUDIO0_DAC_MIN_GAIN
#endif
#ifndef AUDIO_DAC_MAX_GAIN
#define AUDIO_DAC_MAX_GAIN      AUDIO0_DAC_MAX_GAIN
#endif

/* Audio decoder treble anhancement limits */
#ifndef AUDIO_DAC_MAX_TREB
#define AUDIO_DAC_MAX_TREB       8      /* VLSI1053 limits */
#endif

#ifndef AUDIO_DAC_MAX_TFIN
#define AUDIO_DAC_MAX_TFIN      15      /* VLSI1053 limits */
#endif

#ifndef AUDIO_DAC_MAX_BASS
#define AUDIO_DAC_MAX_BASS      15      /* VLSI1053 limits */
#endif

#ifndef AUDIO_DAC_MAX_BFIN
#define AUDIO_DAC_MAX_BFIN      15      /* VLSI1053 limits */
#endif

/*
 * I/O control codes.
 */
#define AUDIO_SETWRITETIMEOUT   0x010d
#define AUDIO_GETWRITETIMEOUT   0x010e

/*! \brief Immediately start playing. */
#define AUDIO_PLAY          0x3001
/*! \brief Immediately stop playing and discard buffer. */
#define AUDIO_CANCEL        0x3002
#define AUDIO_GET_STATUS    0x3003
#define AUDIO_GET_PLAYGAIN  0x3004
#define AUDIO_SET_PLAYGAIN  0x3005
#define AUDIO_GET_PBSIZE    0x3006
#define AUDIO_SET_PBSIZE    0x3007
#define AUDIO_GET_PBLEVEL   0x3008
#define AUDIO_GET_PBWLOW    0x3009
#define AUDIO_SET_PBWLOW    0x300A
#define AUDIO_GET_PBWHIGH   0x300B
#define AUDIO_SET_PBWHIGH   0x300C
#define AUDIO_BEEP          0x300F
/*! \brief Retrieve decoder information. */
#define AUDIO_GET_DECINFO   0x3010
/*! \brief Retrieve decoder capabilities. */
#define AUDIO_GET_DECCAPS   0x3011
/*! \brief Retrieve decoder formats. */
#define AUDIO_GET_DECFMTS   0x3012
#define AUDIO_FMT_PLUGIN        0x00000001
#define AUDIO_FMT_WAV_PCM       0x00000002
#define AUDIO_FMT_WAV_ADPCM     0x00000004
#define AUDIO_FMT_WAV_IMA_ADPCM 0x00000008
#define AUDIO_FMT_VORBIS        0x00000100
#define AUDIO_FMT_MPEG1_L1      0x00001000
#define AUDIO_FMT_MPEG1_L2      0x00002000
#define AUDIO_FMT_MPEG1_L3      0x00004000
#define AUDIO_FMT_MPEG2_AAC     0x00010000
#define AUDIO_FMT_MPEG4_AAC     0x00020000
#define AUDIO_FMT_WMA_V2        0x00100000
#define AUDIO_FMT_WMA_V7        0x00200000
#define AUDIO_FMT_WMA_V8        0x00400000
#define AUDIO_FMT_WMA_V9        0x00800000

/*! \brief Enable or disable specific decoder formats. */
#define AUDIO_SET_DECFMTS   0x3013
/*! \brief Retrieve encoder information. */
#define AUDIO_GET_CODINFO   0x3014
/*! \brief Retrieve encoder capabilities. */
#define AUDIO_GET_CODCAPS   0x3015
/*! \brief Retrieve encoder formats. */
#define AUDIO_GET_CODFMTS   0x3016
/*! \brief Enable or disable specific encoder formats. */
#define AUDIO_SET_CODFMTS   0x3017
/*! \brief Retrieve midi information. */
#define AUDIO_GET_MIDINFO   0x3018
/*! \brief Retrieve midi capabilities. */
#define AUDIO_GET_MIDCAPS   0x3019
#define AUDIO_FMT_GMIDI1        0x00000001
#define AUDIO_FMT_GMIDI2        0x00000002
#define AUDIO_FMT_SPMIDI0       0x00000004
/*! \brief Set audio enhancement treble value. */
#define AUDIO_SET_TREB      0x301a
/*! \brief Get audio enhancement treble value. */
#define AUDIO_GET_TREB      0x301b
/*! \brief Set audio enhancement treble frequency. */
#define AUDIO_SET_TFIN      0x301c
/*! \brief Get audio enhancement treble frequency. */
#define AUDIO_GET_TFIN      0x301d
/*! \brief Set audio enhancement bass value. */
#define AUDIO_SET_BASS      0x301e
/*! \brief Get audio enhancement bass value. */
#define AUDIO_GET_BASS      0x301f
/*! \brief Set audio enhancement bass frequency. */
#define AUDIO_SET_BFIN      0x3020
/*! \brief Get audio enhancement bass frequency. */
#define AUDIO_GET_BFIN      0x3021

/*! \brief Upload plug-in. */
#define AUDIO_PLUGIN_UPLOAD 0x3030

/*! \brief Enable interrupts. */
#define AUDIO_IRQ_ENABLE    0x3031

/*! \brief Set internal clock. */
#define AUDIO_SET_ICLOCK    0x3032

typedef struct _VS_PLUGIN_INFO VS_PLUGIN_INFO;

struct _VS_PLUGIN_INFO {
    size_t vsplg_size;
    CONST uint16_t *vsplg_data;
};

/*
 * Instruction opcodes.
 */
#define VS_OPCODE_READ  3
#define VS_OPCODE_WRITE 2

/*!
 * \name Mode Control Register
 */
/* ------------------------------------------------------------- */
/*@{*/
/*! \brief Register index. */
#define VS_MODE_REG         0
/*! \brief Left channel inverted. */
#define VS_SM_DIFF          0x0001

/*! \brief Allow MPEG Layers I & II. 
 *
 * Determines whether it is allowed to decode MPEG 1 and 2 Layers I and II 
 * in addition to Layer III. If you enable Layer I and Layer II decoding, 
 * you are liable for any patent issues that may arise. Joint licensing of 
 * MPEG 1.0 / 2.0 Layer III does not cover all patents pertaining to 
 * Layers I and II.
 */
#if VS_HAS_SM_LAYER12
#define VS_SM_LAYER12       0x0002
#endif

/*! \brief VS1001K fast forward.
 *
 * By setting this bit, the player starts to accept data at a high speed, and 
 * just decodes the audio headers silently without playing any audio data. 
 * This can be used to fast-forward data with safe landing.
 * Register \ref VS_DECODE_TIME_REG is updated during a fast-forward just as 
 * normal.
 *
 * Note, that this bit is available on the VS1001K only and is used as MPEG 
 * Layer I & II enable on other members of the VS10xx family.
 */
#if VS_HAS_SM_FFWD
#define VS_SM_FFWD          0x0002
#endif

/*! \brief Soft reset.
 *
 * Bit is cleared automatically.
 */
#define VS_SM_RESET         0x0004


/*! \brief VS1001 MPEG Layers I & II enable.
 *
 * This bit is marked unused in later VS1001 datasheet versions and may 
 * not work due to firmware problems.
 *
 * Note, that this bit was available on the VS1001K only and is used as 
 * a cancel flag on other members of the VS10xx family.
 *
 * If you enable Layer I and Layer II decoding, you are liable for any 
 * patent issues that may arise.
 */
#if VS_HAS_SM_MP12
#define VS_SM_MP12          0x0008
#endif

/*! \brief Jump out of wave decoding. 
 *
 * Set this bit to stop WAV, WMA and MIDI file playing before the end of 
 * the file has been reached. Continue sending zeros for WAV and more data
 * for MIDI and WMA until this bit is cleared again.
 *
 * Not available on the VS1001K.
 */
#if VS_HAS_SM_OUTOFWAV
#define VS_SM_OUTOFWAV      0x0008
#endif

/*! \brief Stop decoding in the middle. 
 *
 * Set this bit to stop decoding a in the middle, and continue sending 
 * data honouring DREQ. When this flag is detected by a codec, it will 
 * stop decoding and discard stream buffer content. The flag is cleared
 * automatically and SCI HDAT1 will also be cleared.
 *
 * Available on the VS1053.
 */
#if VS_HAS_SM_CANCEL
#define VS_SM_CANCEL        0x0008
#endif

/*! \brief Power down.
 *
 * During powerdown, no audio is played and no SDI operations are performed.
 * It is recommended to set \ref VS_VOL_REG to 0xFFFF before setting this 
 * bit.
 *
 * For best power down efficiency, activate hardware reset.
 *
 * Not available on the VS1011. Although defined, this is not implemented 
 * in the VS1001K firmware.
 */
#if VS_HAS_SM_PDOWN
#define VS_SM_PDOWN         0x0010
#endif

/*! \brief EarSpeaker spatial processing.
 */
#if VS_HAS_SM_EARSPEAKER
#define VS_SM_EARSPEAKER_LO     0x0010
#define VS_SM_SPATIAL_OFF       0
#define VS_SM_SPATIAL_MINIMAL   VS_SM_EARSPEAKER_LO
#define VS_SM_SPATIAL_NORMAL    VS_SM_EARSPEAKER_HI
#define VS_SM_SPATIAL_EXTREME   (VS_SM_EARSPEAKER_LO | VS_SM_EARSPEAKER_LO)
#endif

/*! \brief VS1001K DAC mode. 
 *
 * 16-bit DAC mode if set, MP3 mode otherwise.
 *
 * This bit is marked unused in later VS1001 datasheet versions and may 
 * not work due to firmware problems.
 *
 * Note, that this bit was available on the VS1001 only and is used as 
 * a test mode flag on other members of the VS10xx family.
 */
#if VS_HAS_SM_DAC
#define VS_SM_DAC           0x0020
#endif

/*! \brief Allow SDI tests.
 *
 * Not required for the VS1001K, where tests are enabled by default.
 */
#if VS_HAS_SM_TESTS
#define VS_SM_TESTS         0x0020
#endif

/*! \brief VS1001K DAC mono mode. 
 *
 * DAC mono mode if set, stereo mode otherwise.
 *
 * This bit is marked unused in later VS1001 datasheet versions and may 
 * not work due to firmware problems.
 *
 * Note, that this bit was available on the VS1001 only and is used as 
 * a streaming mode flag on other members of the VS10xx family.
 */
#if VS_HAS_SM_DACMONO
#define VS_SM_DACMONO       0x0040
#endif

/*! \brief Stream mode.
 *
 * When stream mode is activated, data should be sent with as even 
 * intervals as possible (and preferable with data blocks of less 
 * than 512 bytes). The decoder makes every attempt to keep its input 
 * buffer half full by changing its playback speed upto 5%. For best 
 * quality sound, the average speed error should be within 0.5%, the 
 * bitrate should not exceed 160 kbit/s and VBR should not be used.
 *
 * Not available on the VS1001K.
 */
#if VS_HAS_SM_STREAM
#define VS_SM_STREAM        0x0040
#endif

/*! \brief VS1001K bass/treble enhancer.
 *
 * When set, this bit turns on the built-in bass and treble enhancer.
 */
#if VS_HAS_SM_BASS
#define VS_SM_BASS          0x0080
#endif

#if VS_HAS_SM_EARSPEAKER
#define VS_SM_EARSPEAKER_HI 0x0080
#endif

/*! \brief Active edge of the data clock. 
 *
 * If clear data is read at the rising edge, and if set data is read 
 * at the falling edge.
 */
#define VS_SM_DACT          0x0100

/*! \brief Data bus bit order.
 *
 * When clear the most significant bit is sent first, which is the default.
 * This register bit has no effect on the SCI bus.
 */
#define VS_SM_BITORD        0x0200

/*! \brief SDI most significant bit last. 
 *
 * Same as \ref VS_SM_BITORD.
 */
#define VS_SM_SDIORD        0x0200

/*! \brief VS1001K master mode. 
 *
 * Untested and not recommended.
 *
 * Note, that this bit is available on the VS1001 only and is used as 
 * a chip select mode flag on other members of the VS10xx family.
 */
#if VS_HAS_SM_IBMODE
#define VS_SM_IBMODE        0x0400
#endif

/*! \brief Share SPI chip select. */
#if VS_HAS_SM_SDISHARE
#define VS_SM_SDISHARE      0x0400
#endif

/*! \brief VS1001K master mode clock speed. 
 *
 * Untested and not recommended.
 *
 * Note, that this bit is available on the VS1001 only and is used as 
 * an SDI mode flag on other members of the VS10xx family.
 */
#if VS_HAS_SM_IBCLK
#define VS_SM_IBCLK         0x0800
#endif

/*! \brief VS1002 native SDI mode.
 *
 * Default is 0 on the VS1011E and 1 on the VS1002.
 *
 * Not available on the VS1001K.
 */
#if VS_HAS_SM_SDINEW
#define VS_SM_SDINEW        0x0800
#endif

/*! \brief ADPCM recording.
 *
 * Available on VS1033 and VS1053.
 */
#if VS_HAS_SM_ADPCM
#define VS_SM_ADPCM         0x1000
#endif

/*! \brief ADPCM high pass filter.
 *
 * If set at the same time as \ref VS_SM_ADPCM and \ref VS_SM_RESET, 
 * ADPCM mode will start with a high-pass filter. This may help 
 * intelligibility of speech when there is lots of background noise.
 *
 * Available on VS1033.
 */
#if VS_HAS_SM_ADPCM_HP
#define VS_SM_ADPCM_HP      0x2000
#endif

/*! \brief ADPCM recording selector.
 *
 * Used to select the input for ADPCM recording. If cleared, microphone 
 * input is used. If set to 1, LINEIN is used instead.
 *
 * Available on VS1033 and VS1053.
 */
#if VS_HAS_SM_LINE_IN
#define VS_SM_LINE_IN       0x4000
#endif

/*! \brief Input clock range.
 *
 * Activates a clock divider at the XTAL input. Should be set as soon 
 * as possible after a chip reset.
 *
 * Available on VS1033 and VS1053.
 */
#if VS_HAS_SM_CLK_RANGE
#define VS_SM_CLK_RANGE     0x8000
#endif
/*@}*/

/*!
 * \name Status Register
 */
/*@{*/
/* ------------------------------------------------------------- */
/*! \brief Register index. 
 */
#define VS_STATUS_REG       1

/*! \brief Internal analog volume control mask. 
 *
 * These bits is meant to be used by the internal firmware only.
 *
 * - 0 for 0 dB 
 * - 1 for -6 dB 
 * - 3 for -12 dB
 */
#if VS_HAS_SS_AVOL
#define VS_SS_AVOL          0x0003
/*! \brief Internal analog volume control LSB. */
#define VS_SS_AVOL_LSB      0
#endif

/*! \brief Reference voltage selection.
 *
 * If AVDD is at least higher than 3.3V, then this bit can be set to 
 * select 1.65V reference voltage to increase the analog output swing.
 *
 * See \ref VS_SS_AD_CLOCK for more information about using this bit
 * with the VS1053b.
 */
#if VS_HAS_SS_VREF_1_65
#define VS_SS_VREF_1_65     0x0001
#endif

/*! \brief AD clock divider.
 *
 * This bit can be set to divide the AD modulator frequency by 2.
 *
 * Due to a firmware bug in VS1053b, volume calculation routine clears 
 * \ref VS_SS_AD_CLOCK and \ref VS_SS_VREF_1_65 bits. Write to 
 * \ref VS_STATUS_REG or \ref VS_VOL_REG as well as sample rate change
 * with bass enhancer or treble control actived, causes the volume 
 * calculation routine to be called. 
 *
 * As a workaround you can write to \ref VS_STATUS_REG through 
 * \ref VS_WRAMADDR_REG and \ref VS_WRAM_REG after each volume change.
 * Write 0xC001 to \ref VS_WRAMADDR_REG, then write the value to 
 * \ref VS_WRAM_REG. However, the difference in performance between the 
 * modes is not significant, so it is easier to just use the default mode.
 */
#if VS_HAS_SS_AD_CLOCK
#define VS_SS_AD_CLOCK      0x0002
#endif

/*! \brief Internal analog power down. 
 *
 * This bit is meant to be used by the internal firmware only.
 */
#define VS_SS_APDOWM1       0x0004

/*! \brief Analog driver power down. 
 *
 * This bit is typically controlled by the internal firmware. However, 
 * if the application wants to power down the VS1001 and VS1011 with a 
 * minimum power-off transient, this bit should be set to 1 a few 
 * milliseconds before activating reset.
 *
 * With newer chips set the volume register to 0xFFFF a few
 * milliseconds before reset.
 */
#define VS_SS_APDOWN2       0x0008

/*! \brief Version mask.
 *
 * - 0 for VS1001
 * - 1 for VS1011
 * - 2 for VS1002 and VS1011E, see \ref VS_SM_SDINEW
 * - 3 for VS1003
 * - 4 for VS1053
 * - 5 for VS1033
 * - 7 for VS1103
 */
#define VS_SS_VER           0x00F0
/*! \brief Version LSB. */
#define VS_SS_VER_LSB       4

#define VS1001_SS_VER       0
#define VS1011_SS_VER       1
#define VS1011E_SS_VER      2
#define VS1002_SS_VER       2
#define VS1003_SS_VER       3
#define VS1053_SS_VER       4
#define VS1033_SS_VER       5
#define VS1103_SS_VER       7

/*! \brief GBUF overload detection.
 *
 * Some chips contain a GBUF protection circuit, which disconnects the 
 * GBUF driver when too much current is drawn, indicating a short-circuit 
 * to ground. This bit is high while the overload is detected. 
 */
#if VS_HAS_SS_VCM_DISABLE
#define VS_SS_VCM_DISABLE   0x0400
#endif

/*! \brief GBUF overload indicator.
 *
 * Set this bit to disable the GBUF protection circuit.
 */
#if VS_HAS_SS_VCM_OVERLOAD
#define VS_SS_VCM_OVERLOAD  0x0800
#endif

/*! \brief Increase DAC swing.
 *
 * Allows to go above the 0 dB volume setting in 0.5 db steps. Although 
 * the range of the register bits offer up to 7 steps (+3.5 db), higher 
 * settings than 2 cause the DAC modulator to be overdriven and should 
 * not be used.
 *
 * Useful with I2S to control the amount of headroom.
 */
#if VS_HAS_SS_SWING
#define VS_SS_SWING         0x7000
#define VS_SS_SWING_LSB     12
#define VS_SS_SWING_NORMAL  0
#define VS_SS_SWING_MAX     2
#endif

/*! \brief Header in decode indicator.
 *
 * Set when WAV, Ogg Vorbis, WMA, MP4, or AAC-ADIF header is being decoded
 * and jumping to another location in the file is not allowed.
 */
#if VS_HAS_SS_DO_NOT_JUMP
#define VS_SS_DO_NOT_JUMP   0x8000
#endif

/*@}*/

/*!
 * \name VS1001 internal frequency control register.
 */
/* ------------------------------------------------------------- */
/*@{*/
/*! \brief Register index.
 * 
 * Used only on older chip revisions?
 */
#if VS_HAS_INT_FCTLH_REG
#define VS_INT_FCTLH_REG    2
#endif

/*@}*/

/*!
 * \name Bass/Treble Enhancer Register
 */
/* ------------------------------------------------------------- */
/*@{*/
/*! \brief Register index.
 *
 * On some earlier chips, audio interrupts may be missed when writing 
 * to this register.
 *
 * For the VS1001K see \ref VS_SM_BASS.
 */
#if VS_HAS_BASS_REG
#define VS_BASS_REG         2

/*! \brief Bass enhancer lower limit frequency mask.
 *
 * Specified in 10 Hz steps.
 *
 * The bass enhancer is a powerful bass boosting DSP algorithm, which 
 * tries to take the most out of the user's earphones without causing
 * clipping.
 *
 */
#define VS_SB_FREQLIMIT     0x000F
/*! \brief Lower limit frequency LSB. */
#define VS_SB_FREQLIMIT_LSB 0

/*! \brief Bass enhancement mask. 
 * 
 * Bass enhancement in 1 dB steps, or disabled when zero.
 */
#define VS_SB_AMPLITUDE     0x00F0
/*! \brief Bass enhancement LSB. */
#define VS_SB_AMPLITUDE_LSB 4

/*! \brief Treble control lower limit frequency mask. 
 *
 * Specified in 1 kHz steps.
 */
#define VS_ST_FREQLIMIT     0x0F00
/*! \brief Lower limit frequency LSB. */
#define VS_ST_FREQLIMIT_LSB 8

/*! \brief Treble control mask. 
 *
 * Treble control in 1.5 dB steps, or disabled when zero.
 */
#define VS_ST_AMPLITUDE     0xF000
/*! \brief Bass enhancement LSB. */
#define VS_ST_AMPLITUDE_LSB 12
#endif
/*@}*/

/*!
 * \name Clock Frequency and Multiplier Register
 */
/* ------------------------------------------------------------- */
/*@{*/
/*! \brief Register index. */
#define VS_CLOCKF_REG       3

/*! \brief Clock frequency mask.
 *
 * Should be set to crystal clock divided by 2000, if the clock is not 
 * 24.576 MHz.
 *
 * Must be set before beginning decoding MP3 data. Otherwise the sample 
 * rate will not be set correctly.
 *
 * Used for the VS1001, VS1011 and VS1002.
 */
#if VS_HAS_SC_FREQ
#define VS_SC_FREQ          0x7FFF
/*! \brief Clock frequency LSB. */
#define VS_SC_FREQ_LSB      0
#endif

/*! \brief VS1003/VS1033 clock frequency mask.
 *
 * Should be set to crystal clock divided by 4000, if the clock
 * is not 12.288 MHz.
 *
 * Used for the VS1003, VS1033 and VS1053.
 */
#if VS_HAS_SC_X3FREQ
#define VS_SC_X3FREQ        0x07FF
#endif

/*! \brief Allowed multiplier addition. 
 *
 * Used for WMA decoding with the VS1003, VS1033 and VS1053.
 *
 * Summand 0.5 is not available on the VS1053.
 */
#if VS_HAS_SC_ADD
#define VS_SC_ADD           0x1800
#define VS_SC_ADD_NONE      0x0000
#if VS_HAS_SC_ADD2
#define VS_SC_ADD_1_0       0x0800
#define VS_SC_ADD_1_5       0x1000
#define VS_SC_ADD_2_0       0x1800
#else
#define VS_SC_ADD_0_5       0x0800
#define VS_SC_ADD_1_0       0x1000
#define VS_SC_ADD_1_5       0x1800
#endif
#endif

/*! \brief Clock multiplier. 
 *
 * Available on the VS1003, VS1033 and VS1053. However, multiplier 1.5 
 * is not available on the VS1053.
 */
#if VS_HAS_SC_MULT
#define VS_SC_MULT          0xE000
#define VS_SC_MULT_1_0      0x0000
#if VS_HAS_SC_MULT5
#define VS_SC_MULT_2_0      0x2000
#define VS_SC_MULT_2_5      0x4000
#define VS_SC_MULT_3_0      0x6000
#define VS_SC_MULT_3_5      0x8000
#define VS_SC_MULT_4_0      0xA000
#define VS_SC_MULT_4_5      0xC000
#define VS_SC_MULT_5_0      0xE000
#else
#define VS_SC_MULT_1_5      0x2000
#define VS_SC_MULT_2_0      0x4000
#define VS_SC_MULT_2_5      0x6000
#define VS_SC_MULT_3_0      0x8000
#define VS_SC_MULT_3_5      0xA000
#define VS_SC_MULT_4_0      0xC000
#define VS_SC_MULT_4_5      0xE000
#endif
#endif

/*! \brief Clock doubler enable. 
 *
 * Used for the VS1001, VS1011 and VS1002.
 */
#if VS_HAS_CF_DOUBLER
#define VS_CF_DOUBLER       0x8000
#endif

/*@}*/

/*!
 * \name Decode Time Register
 */
/* ------------------------------------------------------------- */
/*@{*/
/*! \brief Register index. 
 *
 * This register contains the decode time in full seconds or zero after 
 * resetting the decoder.
 *
 * The user may change the value of this register. However, in that case 
 * the new value should be written twice.
 */
#define VS_DECODE_TIME_REG  4
/*@}*/

/*!
 * \name Miscellaneous Audio Data Register
 */
/* ------------------------------------------------------------- */
/*@{*/
/*! \brief Register index.
 *
 * Usage on the VS1001K differs.
 */
#define VS_AUDATA_REG       5
/*! \brief Stereo flag.
 *
 * 1 for stereo and 0 for mono. Read-only.
 *
 * On the VS1001K the most significant bit is used as a stereo indicator.
 */
#if VS_HAS_AD_STEREO_BIT0
#define VS_AD_STEREO        0x0001
#else
#define VS_AD_STEREO        0x8000
#endif

/*! \brief Current sampling rate mask.
 *
 * Value is sample rate divided by 2. Writing to this register will 
 * change the sample rate on the run.
 *
 * To reduce the digital power consumption when idle, you can set
 * a low sample rate.
 */
#if VS_HAS_AD_SAMRATE
#if VS_HAS_AD_STEREO_BIT0
#define VS_AD_SAMRATE       0xFFFE
/*! \brief Crystal clock LSB. */
#define VS_AD_SAMRATE_LSB   1
#else
#define VS_AD_SAMRATE       0x7FFF
#define VS_AD_SAMRATE_LSB   0
#endif
#endif

/*! \brief Current bit rate mask.
 */
#if VS_HAS_AD_BITRATE
#define VS_AD_BITRATE       0x01FF
#define VS_AD_BITRATE_LSB   0
#endif

/*! \brief Current sampling rate index mask.
 */
#if VS_HAS_AD_SAMRATE_IDX
#define VS_AD_SAMRATE_IDX       0x1E00
#define VS_AD_SAMRATE_IDX_LSB   9

#define VS_AD_SAMRATE_44100     1
#define VS_AD_SAMRATE_48000     2
#define VS_AD_SAMRATE_32000     3
#define VS_AD_SAMRATE_22050     4
#define VS_AD_SAMRATE_24000     5
#define VS_AD_SAMRATE_16000     6
#define VS_AD_SAMRATE_11025     7
#define VS_AD_SAMRATE_12000     8
#define VS_AD_SAMRATE_8000      9
#endif

/*@}*/

/*!
 * \name RAM Read/Write Register
 */
/* ------------------------------------------------------------- */
/*@{*/
/*! \brief Register index.
 *
 * Used to upload application programs and data to instruction and data RAM.
 */
#define VS_WRAM_REG         6
/*@}*/

/*!
 * \name RAM Read/Write Base Address Register
 */
/* ------------------------------------------------------------- */
/*@{*/
/*! \brief Register index.
 *
 * Used to upload application programs and data to instruction and data RAM.
 */
#define VS_WRAMADDR_REG     7
/*@}*/

/*!
 * \name Stream Header Data Register 0
 */
/* ------------------------------------------------------------- */
/*@{*/
/*! \brief Register index. 
 *
 * For WAV, AAC, WMA and MIDI data, the register contains the (average)
 * data rate measured in bytes per second.
 *
 * For MP3 streams, the register contains MP3 header information.
 *
 * Not available in the VS1103.
 */
#if VS_HAS_HDAT0_REG
#define VS_HDAT0_REG            8

#define VS_HDAT0_EMPHAS         0x0003
#define VS_HDAT0_EMPHAS_LSB     0

#define VS_HDAT0_EMPHAS_NONE    0
#define VS_HDAT0_EMPHAS_5015    1
#define VS_HDAT0_EMPHAS_J17     3

#define VS_HDAT0_ORIGIN         0x0004

#define VS_HDAT0_COPY           0x0008

#define VS_HDAT0_EXT            0x0030

#define VS_HDAT0_MODE           0x00C0
#define VS_HDAT0_MODE_LSB       6

#define VS_HDAT0_MODE_STEREO    0
#define VS_HDAT0_MODE_JSTEREO   1
#define VS_HDAT0_MODE_DUAL      2
#define VS_HDAT0_MODE_MONO      3

#define VS_HDAT0_PRIV           0x0100

#define VS_HDAT0_PAD            0x0200

#define VS_HDAT0_SAMRATE        0x0C00
#define VS_HDAT0_SAMRATE_LSB    10

#define VS_HDAT0_SAMRATE_11B    0
#define VS_HDAT0_SAMRATE_12B    1
#define VS_HDAT0_SAMRATE_8B     2

#define VS_HDAT0_BITRATE        0xF000
#endif
/*@}*/

/*!
 * \name VS1103 Input Register 0
 */
/* ------------------------------------------------------------- */
/*@{*/
/*! \brief Register index. */
#if VS_HAS_IN0_REG
#define VS_IN0_REG          8
#endif
/*@}*/

/*!
 * \name Stream Header Data Register 1
 */
/* ------------------------------------------------------------- */
/*@{*/
/*! \brief Register index. 
 *
 * Not available in the VS1103.
 */
#if VS_HAS_HDAT1_REG
#define VS_HDAT1_REG        9

#define VS_HDAT1_PROT       0x0001
#define VS_HDAT1_LAYER      0x0006
#define VS_HDAT1_ID         0x0018

#define VS_HDAT1_SYNC       0xFFE0
#define VS_HDAT1_WAV        0x7665
#define VS_HDAT1_AAC_ADTS   0x4154
#define VS_HDAT1_AAC_ADIF   0x4144
#define VS_HDAT1_AAC_MP4    0x4D34
#define VS_HDAT1_WMA        0x574D
#define VS_HDAT1_MIDI       0x4D54
#define VS_HDAT1_OGG        0x4F67

#endif
/*@}*/

/*!
 * \name VS1103 Input Register 1
 */
/* ------------------------------------------------------------- */
/*@{*/
/*! \brief Register index. */
#if VS_HAS_IN1_REG
#define VS_IN1_REG          9
#endif
/*@}*/

/*!
 * \name Application Start Address Register
 *
 * Used to activate previously uploaded application programs.
 */
/* ------------------------------------------------------------- */
/*@{*/
/*! \brief Register index. */
#define VS_AIADDR_REG      10
/*@}*/

/*!
 * \name Volume Control Register
 */
/* ------------------------------------------------------------- */
/*@{*/
/*! \brief Register index. 
 *
 * After hardware reset, volume is at maximum value. Software reset does
 * not change volume settings.
 *
 * Setting this register to \ref VS_VOL_OFF will put the chip in analog
 * power down mode.
 *
 * On some earlier chips, audio interrupts may be missed when writing 
 * to this register. This will not happen on the VS1053, where volume
 * changes are delayed until the next batch of samples are sent to the
 * audio FIFO.
 */
#define VS_VOL_REG         11

/*! \brief Right channel volume mask. 
 *
 * Attenuation is given in 0.5 dB steps or 255 for mute.
 */
#define VS_VOL_RIGHT        0x00FF

/*! \brief Right channel volume LSB. */
#define VS_VOL_RIGHT_LSB    0

/*! \brief Right channel volume mask.
 *
 * Attenuation is given in 0.5 dB steps or 255 for mute.
 */
#define VS_VOL_LEFT         0xFF00

/*! \brief Right channel volume LSB. */
#define VS_VOL_LEFT_LSB     8

#define VS_VOL_MAX          0x0000
#define VS_VOL_MIN          0xFEFE
#define VS_VOL_OFF          0xFFFF
/*@}*/

/*!
 * \name VS1103 Mixer Volume Register
 */
/* ------------------------------------------------------------- */
/*@{*/
/*! \brief Register index.
 *
 */
#if VS_HAS_MIXERVOL_REG
#define VS_MIXERVOL_REG     12
#define VS_SMV_GAIN1        0x001F
#define VS_SMV_GAIN2        0x03E0
#define VS_SMV_GAIN3        0x7C00
#define VS_SMV_ACTIVE       0x8000
#endif
/*@}*/

/*!
 * \name VS1103 IMA ADPCM Record Control Register
 */
/* ------------------------------------------------------------- */
/*@{*/
/*! \brief Register index.
 *
 */
#if VS_HAS_ADPCMRECCTL_REG
#define VS_ADPCMRECCTL_REG  13
#define VS_SARC_GAIN4       0x003F
#define VS_SARC_MANUALGAIN  0x0040
#define VS_SARC_OUTOFADPCM  0x0080
#endif
/*@}*/

/*!
 * \name Application Control Registers
 */
/* ------------------------------------------------------------- */
/*@{*/
/*! \brief Application register 0 index.
 *
 * Do not use with VS1001. Also used for mixer volume on the VS1103.
 */
#if VS_HAS_AICTRL0_REG
#define VS_AICTRL0_REG     12
#endif

/*! \brief VS1001K application register 0 index.
 *
 * A second register is available at VS_AICTRL_REG + 1.
 */
#if VS_HAS_AICTRL_REG
#define VS_AICTRL_REG      13
#endif
/*! \brief Application register 1 index. 
 *
 * Register 0 on the VS1001. Also used for IMA ADPCM record control in 
 * the VS1103.
 */
#if VS_HAS_AICTRL1_REG
#define VS_AICTRL1_REG     13
#endif
/*! \brief Application register 2 index. 
 *
 * Register 1 on the VS1001.
 */
#if VS_HAS_AICTRL2_REG
#define VS_AICTRL2_REG     14
#endif
/*! \brief Application register 3 index. 
 *
 * Do not use with VS1001.
 */
#if VS_HAS_AICTRL3_REG
#define VS_AICTRL3_REG     15
#endif
/*@}*/


/*
 * Bytes needed to flush internal VS buffer (size of VS internal buffer)
 */
#ifndef VS_FLUSH_BYTES
#define VS_FLUSH_BYTES 2048
#endif

/*
 * Status of the decoder
 */
#define CODEC_STATUS_IDLE       0
#define CODEC_STATUS_PLAYING    1
#define CODEC_STATUS_RECORDING  2

/*
 * Header info filled by VsGetHeaderInfo.
 */
#ifdef __GNUC__
typedef struct __attribute__((packed)) {
    uint16_t vshi_no_crc:1;
    uint16_t vshi_layer:2;
    uint16_t vshi_id:2;
    uint16_t vshi_syncword:11;

    uint16_t vshi_emphasis:2;
    uint16_t vshi_original:1;
    uint16_t vshi_copyright:1;
    uint16_t vshi_extension:2;
    uint16_t vshi_mode:2;
    uint16_t vshi_private_bit:1;
    uint16_t vshi_pad_bit:1;
    uint16_t vshi_sample_rate:2;
    uint16_t vshi_bitrate:4;
} VS_HEADERINFO;
#endif

#ifndef VSCODEC_CMD_TIMEOUT
/*! \brief Minimum time in milliseconds to held hardware reset low. */
#define VSCODEC_CMD_TIMEOUT         NUT_WAIT_INFINITE
#endif

/*!
 * \brief Internal codec control block.
 */
typedef struct _VSDCB {
    int dcb_pbstat;         /*!< \brief Playback status. */
    uint32_t dcb_scmd;      /*!< \brief Requested command flags, see VSREQ_ flags. */
    int dcb_rvol;           /*!< \brief Volume of right channel. */
    int dcb_lvol;           /*!< \brief Volume of left channel. */
    int dcb_treb;           /*!< \brief Treble enhancement *1.5dB. */
    int dcb_tfin;           /*!< \brief Treble limit frequency *1000Hz. */
    int dcb_bass;           /*!< \brief Bass enhancement *1dB. */
    int dcb_bfin;           /*!< \brief Bass limit frequency *10Hz. */
    uint16_t dcb_sinefreq;  /*!< \brief Requested beep frequency. */
    uint32_t dcb_pbwlo;     /*!< \brief Playback buffer low watermark. */
    uint32_t dcb_pbwhi;     /*!< \brief Playback buffer high watermark. */
    uint32_t dcb_wtmo;      /*!< \brief Write timeout. */
    uint32_t dcb_rtmo;      /*!< \brief Read timeout. */
    HANDLE dcb_bufque;      /*!< \brief Buffer change event. */
    HANDLE dcb_feedme;      /*!< \brief Decoder hungry event. */
    uint16_t dcb_codec_ver; /*!< \brief Chip version, 1001, 1002, 1053 etc. */
    char dcb_codec_rev;     /*!< \brief Chip revision character. */
    uint32_t dcb_dec_caps;  /*!< \brief Decoder capabilities. */
    uint32_t dcb_cod_caps;  /*!< \brief Encoder capabilities. */
    uint32_t dcb_midi_caps; /*!< \brief MIDI capabilities. */
    uint32_t dcb_cod_mode;  /*!< \brief Encoder mode. */
    int (*dcb_isready)(void);
    int (*dcb_sendcmd)(void *, size_t);
    int (*dcb_senddata)(CONST uint8_t *, size_t);
    int (*dcb_control)(int req, void *conf);
} VSDCB;

#ifndef VSCODEC_DATA_CHUNK_SIZE
/*! \brief Number of data bytes we can send without checking DREQ. */
#define VSCODEC_DATA_CHUNK_SIZE     32
#endif

extern uint8_t zero_chunk[VSCODEC_DATA_CHUNK_SIZE];

extern NUTDEVICE devVsCodec;
extern NUTDEVICE devSpiVsCodec0;

/*@}*/

__BEGIN_DECLS
/* Function prototypes */
extern int VsCodecWaitReady(NUTDEVICE *dev, uint32_t tmo);
extern uint16_t VsCodecReg(NUTDEVICE *dev, uint_fast8_t op, uint_fast8_t reg, uint_fast16_t val);
extern uint16_t VsCodecMode(NUTDEVICE *dev, uint_fast16_t flags, uint_fast16_t mask);
extern int VsDecoderSetVolume(NUTDEVICE *dev, int left, int right);
extern int VsDecoderSetBass(NUTDEVICE *dev, int treb, int tfin, int bass, int bfin);
extern uint16_t VsCodecBeep(NUTDEVICE *dev, uint16_t fsin);
extern int VsDecoderBufferInit(NUTDEVICE *dev, uint32_t size);
extern int VsCodecIOCtl(NUTDEVICE * dev, int req, void *conf);
extern int VsCodecRead(NUTFILE * nfp, void *data, int len);
extern int VsCodecWrite(NUTFILE * nfp, CONST void *data, int len);
NUTFILE *VsCodecOpen(NUTDEVICE * dev, CONST char *name, int mode, int acc);
extern int VsCodecClose(NUTFILE * nfp);

#ifdef __HARVARD_ARCH__
extern int VsCodecWrite_P(NUTFILE * nfp, PGM_P buffer, int len);
#endif

extern void FeederThread(void *arg) __attribute__ ((noreturn));

__END_DECLS
/* End of prototypes */

#endif
