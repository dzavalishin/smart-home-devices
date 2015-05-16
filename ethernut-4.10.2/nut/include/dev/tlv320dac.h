#ifndef _DEV_TLV320DAC_H_
#define _DEV_TLV320DAC_H_

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
 * \file tlv320dac.h
 * \brief TLV320AIC23B driver.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.2  2009/01/17 11:26:47  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.1  2008/10/05 16:51:46  haraldkipp
 * Added suport for the TLV320 audio DAC.
 *
 *
 * \endverbatim
 */

/*! \name Left Line Input Channel Volume Control Register */
/*@{*/
#define DAC_LLI_VOL             0x00    /*!< \brief Register address. */
#define DAC_LLI_VOL_LRS         0x0100  /*!< \brief Left/right simultaneous volume/mute update. */
#define DAC_LLI_VOL_LIM         0x0080  /*!< \brief Mute. */
#define DAC_LLI_VOL_LIV         0x001F  /*!< \brief Volume control mask.
                                         *
                                         * From 0x00 (-34.5dB) to 0x1F (+12dB) in 1.5dB steps.
                                         */
#define DAC_LLI_VOL_LIV_LSB         0   /*!< \brief Volume control LSB. */
/*@}*/

/*! \name Right Line Input Channel Volume Control Register */
/*@{*/
#define DAC_RLI_VOL             0x01    /*!< \brief Register address. */
#define DAC_RLI_VOL_RLS         0x0100  /*!< \brief Right/left simultaneous volume/mute update. */
#define DAC_RLI_VOL_RIM         0x0080  /*!< \brief Mute. */
#define DAC_RLI_VOL_RIV         0x001F  /*!< \brief Volume control mask.
                                         *
                                         * From 0x00 (-34.5dB) to 0x1F (+12dB) in 1.5dB steps.
                                         */
#define DAC_RLI_VOL_RIV_LSB         0   /*!< \brief Volume control LSB. */
/*@}*/


/*! \name Left Channel Headphone Volume Control */
/*@{*/
#define DAC_LHP_VOL             0x02    /*!< \brief Register address. */
#define DAC_LHP_VOL_LRS         0x0100  /*!< \brief Left/right simultaneous volume/mute update. */
#define DAC_LHP_VOL_LZC         0x0080  /*!< \brief Zero cross detect. */
#define DAC_LHP_VOL_LHV         0x007F  /*!< \brief Volume control mask.
                                         *
                                         * From 0x30 (-73dB, mute) to 0x3F (+6dB).
                                         */
#define DAC_LHP_VOL_LHV_LSB         0   /*!< \brief Volume control LSB. */
/*@}*/

/*! \name Right Channel Headphone Volume Control */
/*@{*/
#define DAC_RHP_VOL             0x03    /*!< \brief Register address. */
#define DAC_RHP_VOL_RLS         0x0100  /*!< \brief Right/left simultaneous volume/mute update. */
#define DAC_RHP_VOL_LZC         0x0080  /*!< \brief Zero cross detect. */
#define DAC_RHP_VOL_LHV         0x007F  /*!< \brief Volume control mask.
                                         *
                                         * From 0x30 (-73dB, mute) to 0x3F (+6dB).
                                         */
#define DAC_RHP_VOL_LHV_LSB         0   /*!< \brief Volume control LSB. */
/*@}*/

/*! \name Analog Audio Path Control */
/*@{*/
#define DAC_ANA_PATH            0x04    /*!< \brief Register address. */
#define DAC_ANA_PATH_STA        0x01C0  /*!< \brief Side tone mask. */
#define DAC_ANA_PATH_STA_LSB         6  /*!< \brief Side tone LSB. */
#define DAC_ANA_PATH_STA_0DB    0x0100  /*!< \brief Added side tone of 0dB. */
#define DAC_ANA_PATH_STA_M6DB   0x0000  /*!< \brief Added side tone of -6dB. */
#define DAC_ANA_PATH_STA_M9DB   0x0040  /*!< \brief Added side tone of -9dB. */
#define DAC_ANA_PATH_STA_M12DB  0x0080  /*!< \brief Added side tone of -12dB. */
#define DAC_ANA_PATH_STA_M18DB  0x00C0  /*!< \brief Added side tone of -18dB. */
#define DAC_ANA_PATH_STE        0x0020  /*!< \brief Side tone enable. */
#define DAC_ANA_PATH_DAC        0x0010  /*!< \brief DAC select. */
#define DAC_ANA_PATH_BYP        0x0008  /*!< \brief Bypass. */
#define DAC_ANA_PATH_INSEL      0x0004  /*!< \brief ADC microphone input. */
#define DAC_ANA_PATH_MICM       0x0002  /*!< \brief Microphone mute. */
#define DAC_ANA_PATH_MICB       0x0001  /*!< \brief Microphone boost. */
/*@}*/

/*! \name Digital Audio Path Control */
/*@{*/
#define DAC_DIG_PATH            0x05    /*!< \brief Register address. */
#define DAC_DIG_PATH_DACM       0x0008  /*!< \brief DAC soft mute. */
#define DAC_DIG_PATH_DEEMP      0x0006  /*!< \brief De-emphases control mask. */
#define DAC_DIG_PATH_DEEMP_LSB      1   /*!< \brief De-emphases control LSB. */
#define DAC_DIG_PATH_DEEMP_NONE     0x0000  /*!< \brief De-emphases disabled. */
#define DAC_DIG_PATH_DEEMP_32KHZ    0x0002  /*!< \brief De-emphases 32 kHz. */
#define DAC_DIG_PATH_DEEMP_44_1KHZ  0x0004  /*!< \brief De-emphases 44.1 kHz. */
#define DAC_DIG_PATH_DEEMP_48KHZ    0x0006  /*!< \brief De-emphases 48 kHz. */
#define DAC_DIG_PATH_ADCHP      0x0001  /*!< \brief ADC high-pass filter. */
/*@}*/

/*! \name Digital Audio Path Control */
/*@{*/
#define DAC_PWRDN               0x06    /*!< \brief Register address. */
#define DAC_PWRDN_OFF           0x0080  /*!< \brief Device power off. */
#define DAC_PWRDN_CLK           0x0040  /*!< \brief Clock off. */
#define DAC_PWRDN_OSC           0x0020  /*!< \brief Oscillator off. */
#define DAC_PWRDN_OUT           0x0010  /*!< \brief Outputs off. */
#define DAC_PWRDN_DAC           0x0008  /*!< \brief DAC off. */
#define DAC_PWRDN_ADC           0x0004  /*!< \brief ADC off. */
#define DAC_PWRDN_MIC           0x0002  /*!< \brief Microphone input off. */
#define DAC_PWRDN_LINE          0x0001  /*!< \brief Line input off. */
/*@}*/

/*! \name Digital Audio Interface Format */
/*@{*/
#define DAC_DAI_FMT             0x07    /*!< \brief Register address. */
#define DAC_DAI_FMT_MS          0x0040  /*!< \brief Maser mode. */
#define DAC_DAI_FMT_LRSWAP      0x0020  /*!< \brief DAC left/right swap. */
#define DAC_DAI_FMT_LRP         0x0010  /*!< \brief DAC left/right phase. */
#define DAC_DAI_FMT_IWL         0x000C  /*!< \brief Input bit length mask. */
#define DAC_DAI_FMT_IWL_LSB         2   /*!< \brief Input bit length LSB. */
#define DAC_DAI_FMT_IWL_16      0x0000  /*!< \brief 16 bit input length. */
#define DAC_DAI_FMT_IWL_20      0x0004  /*!< \brief 20 bit input length. */
#define DAC_DAI_FMT_IWL_24      0x0008  /*!< \brief 24 bit input length. */
#define DAC_DAI_FMT_IWL_32      0x000C  /*!< \brief 32 bit input length. */
#define DAC_DAI_FMT_FOR         0x0003  /*!< \brief Data format mask. */
#define DAC_DAI_FMT_FOR_LSB         0   /*!< \brief Data format LSB. */
#define DAC_DAI_FMT_FOR_RA      0x0000  /*!< \brief MSB first, right aligned. */
#define DAC_DAI_FMT_FOR_LA      0x0001  /*!< \brief MSB first, left aligned. */
#define DAC_DAI_FMT_FOR_I2S     0x0002  /*!< \brief I2S format, MSB first, left – 1 aligned. */
#define DAC_DAI_FMT_FOR_DSP     0x0003  /*!< \brief DSP format, frame sync followed by two data words. */
/*@}*/

/*! \name Sample Rate Control */
/*@{*/
#define DAC_SRATE               0x08    /*!< \brief Register address. */
#define DAC_SRATE_CLKOUT        0x0080  /*!< \brief Clock output divider. */
#define DAC_SRATE_CLKIN         0x0040  /*!< \brief Clock input divider. */
#define DAC_SRATE_SR            0x003C  /*!< \brief Sampling rate mask. */
#define DAC_SRATE_SR_LSB            2   /*!< \brief Sampling rate LSB. */
#define DAC_SRATE_BOSR          0x0002  /*!< \brief Base oversampling rate. 
                                         *
                                         * USB mode   : 0=250 fs, 1=272 fs
                                         * Normal mode: 0=256 fs, 1=384 fs
                                         */
#define DAC_SRATE_USB           0x0001  /*!< \brief USB clock mode. */
/*@}*/

/*! \name Digital Interface Activation */
/*@{*/
#define DAC_DI_ACT              0x09    /*!< \brief Register address. */
#define DAC_DI_ACT_ACT          0x0001  /*!< \brief Interface active. */
/*@}*/

/*! \name Reset Register */
/*@{*/
#define DAC_RESET               0x0F    /*!< \brief Register address.
                                         *
                                         * Writing zero to this register triggers reset.
                                         */
/*@}*/

#define DAC_MAX_VOLUME  6
#define DAC_MIN_VOLUME  -73

__BEGIN_DECLS
/* Prototypes */

extern int Tlv320DacInit(unsigned int rate);
extern int Tlv320DacWrite(void *buf, int len);
extern int Tlv320DacFlush(void);

extern u_char Tlv320DacReadReg(unsigned int reg);
extern void Tlv320DacWriteReg(unsigned int reg, unsigned int val);

extern int Tlv320DacSetVolume(int left, int right);
extern int Tlv320DacSetRate(unsigned int rate);
extern int Tlv320SwitchMode(void);

__END_DECLS
/* End of prototypes */

#endif
