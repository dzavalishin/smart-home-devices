#ifndef _DEV_VS10XX_H_
#define _DEV_VS10XX_H_
/*
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

/*
 * $Log$
 * Revision 1.3  2009/01/17 11:26:47  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.2  2008/08/11 06:59:59  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.1  2007/04/12 08:59:55  haraldkipp
 * VS10XX decoder support added.
 *
 */

#include <cfg/audio.h>
#include <sys/types.h>
#include <stdint.h>

/*!
 * \file dev/vs10xx.h
 * \brief Network interface controller definitions.
 */

/*!
 * \addtogroup xgVs10xx
 */
/*@{*/



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
#define VS_SM_LAYER12       0x0002
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
#define VS_SM_FFWD          0x0002
/*! \brief Soft reset.
 *
 * Bit is cleared automatically.
 */
#define VS_SM_RESET         0x0004
/*! \brief VS1001 MPEG Layers I & II enable.
 *
 * May not work reliable due to firmware problems.
 *
 * If you enable Layer I and Layer II decoding, you are liable for any patent 
 * issues that may arise.
 */
#define VS_SM_MP12          0x0008
/*! \brief Jump out of wave decoding. 
 *
 * Set this bit to stop WAV, WMA and MIDI file playing before the end of 
 * the file has been reached. Continue sending zeros for WAV and more data
 * for MIDI and WMA until this bit is cleared again.
 *
 * Not available on the VS1001K.
 */
#define VS_SM_OUTOFWAV      0x0008
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
#define VS_SM_PDOWN         0x0010
/*! \brief VS1001K unknown function. */
#define VS_SM_DAC           0x0020
/*! \brief Allow SDI tests.
 *
 * Not required for the VS1001K, where tests are enabled by default.
 */
#define VS_SM_TESTS         0x0020
/*! \brief VS1001K unknown function. */
#define VS_SM_DACMONO       0x0040
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
#define VS_SM_STREAM        0x0040
/*! \brief VS1001K bass/treble enhancer. */
#define VS_SM_BASS          0x0080
/*! \brief Active edge of the data clock. 
 *
 * If clear data is read at the rising edge, and if set data is read 
 * at the falling edge.
 */
#define VS_SM_DACT          0x0100
/*! \brief . */
#define VS_SM_BYTEORD       0x0200
/*! \brief SDI most significant bit last. 
 *
 * Bytes are, however, still sent in default order.
 */
#define VS_SM_SDIORD        0x0200
/*! \brief VS1001K master mode. 
 *
 * Untested and not recommended.
 */
#define VS_SM_IBMODE        0x0400
/*! \brief Share SPI chip select. */
#define VS_SM_SDISHARE      0x0400
/*! \brief VS1001K master mode clock speed. 
 *
 * Untested and not recommended.
 */
#define VS_SM_IBCLK         0x0800
/*! \brief VS1002 native SDI mode.
 *
 * Default is 0 on the VS1011E and 1 on the VS1002.
 *
 * Not available on the VS1001K.
 */
#define VS_SM_SDINEW        0x0800
/*! \brief ADPCM recording.
 *
 * Available on VS1033.
 */
#define VS_SM_ADPCM         0x1000
/*! \brief ADPCM high pass filter.
 *
 * If set at the same time as \ref VS_SM_ADPCM and \ref VS_SM_RESET, 
 * ADPCM mode will start with a high-pass filter. This may help 
 * intelligibility of speech when there is lots of background noise.
 *
 * Available on VS1033.
 */
#define VS_SM_ADPCM_HP      0x2000
/*! \brief ADPCM recording selector.
 *
 * Used to select the input for ADPCM recording. If cleared, microphone 
 * input is used. If set to 1, LINEIN is used instead.
 *
 * Available on VS1033.
 */
#define VS_SM_LINE_IN       0x4000
/*! \brief Input clock range.
 *
 * Activates a clock divider at the XTAL input. Should be set as soon 
 * as possible after a chip reset.
 *
 * Available on VS1033.
 */
#define VS_SM_CLK_RANGE     0x8000
/*@}*/

/*!
 * \name Status Register
 */
/*@{*/
/* ------------------------------------------------------------- */
/*! \brief Register index. */
#define VS_STATUS_REG       1
/*! \brief Internal analog volume control mask. 
 *
 * These bits is meant to be used by the internal firmware only.
 *
 * - 0 for 0 dB 
 * - 1 for -6 dB 
 * - 3 for -12 dB
 */
#define VS_SS_AVOL          0x0003
/*! \brief Internal analog volume control LSB. */
#define VS_SS_AVOL_LSB  0
/*! \brief Internal analog power down. 
 *
 * This bit is meant to be used by the internal firmware only.
 */
#define VS_SS_APDOWM1       0x0004
/*! \brief Analog driver power down. 
 *
 * This bit is typically controlled by the internal firmware. However, 
 * if the application wants to power down the decoder with a minimum 
 * power-off transient, this bit should be set to 1 a few milliseconds 
 * before activating reset.
 */
#define VS_SS_APDOWN2       0x0008
/*! \brief Version mask.
 *
 * - 0 for VS1001
 * - 1 for VS1011
 * - 2 for VS1002 and VS1011E, see \ref VS_SM_SDINEW
 * - 3 for VS1003
 */
#define VS_SS_VER           0x0070
/*! \brief Version LSB. */
#define VS_SS_VER_LSB       4
/*@}*/

/*!
 * \name VS1001 internal frequency control register.
 */
/* ------------------------------------------------------------- */
/*@{*/
/*! \brief Register index.
 * 
 * Use only on older chip revisions?
 */
#define VS_INT_FCTLH_REG    2
/*@}*/

/*!
 * \name Bass/Treble Enhancer Register
 */
/* ------------------------------------------------------------- */
/*@{*/
/*! \brief Register index.
 *
 * For the VS1001K see \ref VS_SM_BASS.
 */
#define VS_BASS_REG         2
/*! \brief Bass enhancer lower limit frequency mask.
 *
 * Specified in 10 Hz steps.
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
#define VS_ST_FREQLIMIT     0x000F
/*! \brief Lower limit frequency LSB. */
#define VS_ST_FREQLIMIT_LSB 0
/*! \brief Treble control mask. 
 *
 * Treble control in 1.5 dB steps, or disabled when zero.
 */
#define VS_ST_AMPLITUDE     0x00F0
/*! \brief Bass enhancement LSB. */
#define VS_ST_AMPLITUDE_LSB 4
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
 * Should be set to crystal clock divided by 2000, if the clock
 * is not 24.576 MHz.
 *
 * Used for the VS1001, VS1011 and VS1002.
 */
#define VS_SC_FREQ          0x7FFF
/*! \brief VS1003/VS1033 clock frequency mask.
 *
 * Should be set to crystal clock divided by 4000, if the clock
 * is not 12.288 MHz.
 *
 * Used for the VS1003 and VS1033.
 */
#define VS_SC_X3FREQ        0x07FF
/*! \brief Clock frequency LSB. */
#define VS_SC_FREQ_LSB      0
/*! \brief Allowed multiplier addition. 
 *
 * Used for WMA decoding with the VS1003 and VS1033.
 */
#define VS_SC_ADD           0x1800
/*! \brief Clock multiplier. 
 *
 * Used for the VS1003 and VS1033.
 */
#define VS_SC_MULT          0xE000
/*! \brief Clock doubler enable. 
 *
 * Used for the VS1001, VS1011 and VS1002.
 */
#define VS_CF_DOUBLER       0x8000
/*@}*/

/*!
 * \name Decode Time Register
 */
/* ------------------------------------------------------------- */
/*@{*/
/*! \brief Register index. 
 *
 * This read-only register contains the decode time in full seconds or 
 * zero after resetting the decoder.
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
 * Set to 1 for stereo and 0 for mono. Read-only.
 */
#define VS_AD_STEREO        0x0001
/*! \brief Current sampling rate mask.
 *
 * Value is sample rate divided by 2. Writing to this register will 
 * change the sample rate on the run.
 */
#define VS_AD_SRATE         0xFFFE
/*! \brief Crystal clock LSB. */
#define VS_AD_SRATE_LSB     1
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
 * Not available in the VS1103.
 */
#define VS_HDAT0_REG        8
/*@}*/

/*!
 * \name VS1103 Input Register 0
 */
/* ------------------------------------------------------------- */
/*@{*/
/*! \brief Register index. */
#define VS_IN0_REG          8
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
#define VS_HDAT1_REG        9
/*@}*/

/*!
 * \name VS1103 Input Register 1
 */
/* ------------------------------------------------------------- */
/*@{*/
/*! \brief Register index. */
#define VS_IN1_REG          9
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
/*! \brief Register index. */
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
/*@}*/

/*!
 * \name VS1103 Mixer Volume Register
 */
/* ------------------------------------------------------------- */
/*@{*/
/*! \brief Register index.
 *
 */
#define VS_MIXERVOL_REG     12
#define VS_SMV_GAIN1        0x001F
#define VS_SMV_GAIN2        0x03E0
#define VS_SMV_GAIN3        0x7C00
#define VS_SMV_ACTIVE       0x8000
/*@}*/

/*!
 * \name VS1103 IMA ADPCM Record Control Register
 */
/* ------------------------------------------------------------- */
/*@{*/
/*! \brief Register index.
 *
 */
#define VS_ADPCMRECCTL_REG  13
#define VS_SARC_GAIN4       0x003F
#define VS_SARC_MANUALGAIN  0x0040
#define VS_SARC_OUTOFADPCM  0x0080
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
#define VS_AICTRL0_REG     12
/*! \brief VS1001K application register 0 index.
 *
 * A second register is available at VS_AICTRL0_REG + 1.
 */
#define VS_AICTRL_REG      13
/*! \brief Application register 1 index. 
 *
 * Register 0 on the VS1001. Also used for IMA ADPCM record control in 
 * the VS1103.
 */
#define VS_AICTRL1_REG     13
/*! \brief Application register 2 index. 
 *
 * Register 1 on the VS1001.
 */
#define VS_AICTRL2_REG     14
/*! \brief Application register 3 index. 
 *
 * Do not use with VS1001.
 */
#define VS_AICTRL3_REG     15
/*@}*/


/*
 * Bytes needed to flush internal VS buffer (size of VS interbal buffer)
 */
#define VS_FLUSH_BYTES 2048

/*
 * Status of the decoder
 */
#define VS_STATUS_STOPPED 0
#define VS_STATUS_RUNNING 1
#define VS_STATUS_EOF     2
#define VS_STATUS_EMPTY   4

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

__BEGIN_DECLS
/* Function prototypes */

extern int VsPlayerInit(void);
extern int VsPlayerReset(uint16_t mode);
extern int VsPlayerSetMode(uint16_t mode);
extern int VsPlayerKick(void);
extern int VsPlayerStop(void);
extern int VsPlayerFlush(void);
extern ureg_t VsPlayerInterrupts(ureg_t enable);
extern ureg_t VsPlayerThrottle(ureg_t on);

extern uint16_t VsPlayTime(void);
extern unsigned int VsGetStatus(void);
#ifdef __GNUC__
extern int VsGetHeaderInfo(VS_HEADERINFO *vshi);
#endif
extern uint16_t VsMemoryTest(void);

extern int VsSetVolume(ureg_t left, ureg_t right);
extern int VsBeep(uint8_t fsin, uint8_t ms);

/*@}*/

__END_DECLS
/* End of prototypes */

#endif
