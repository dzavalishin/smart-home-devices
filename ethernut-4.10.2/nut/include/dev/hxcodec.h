#ifndef _DEV_HXCODEC_H_
#define _DEV_HXCODEC_H_
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
 * $Id: hxcodec.h 2459 2009-02-13 14:46:19Z haraldkipp $
 */

#include <cfg/audio.h>
#include <sys/device.h>

/*!
 * \file dev/hxcodec.h
 * \brief Helix decoder definitions.
 */

/*!
 * \addtogroup xgHelixCodec
 */
/*@{*/

/* DAC min/max gain in dB. */
#ifndef AUDIO_DAC_MIN_GAIN
#define AUDIO_DAC_MIN_GAIN      -73
#endif
#ifndef AUDIO_DAC_MAX_GAIN
#define AUDIO_DAC_MAX_GAIN      6
#endif

/*
 * I/O control codes.
 */
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


/*
 * Status of the decoder
 */
#define CODEC_STATUS_IDLE      0
#define CODEC_STATUS_PLAYING   1


extern NUTDEVICE devHelixCodec;

/*@}*/

#endif
