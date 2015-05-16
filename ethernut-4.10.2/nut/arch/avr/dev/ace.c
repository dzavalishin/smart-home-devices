/*
 * Copyright (C) 2001-2003 by egnite Software GmbH. All rights reserved.
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
 *
 */

/*
 * $Log$
 * Revision 1.3  2007/08/29 07:43:53  haraldkipp
 * Documentation updated and corrected.
 *
 * Revision 1.2  2007/05/24 07:27:38  haraldkipp
 * Added two more interfaces. Updated by Przemyslaw Rudy.
 *
 * Revision 1.1  2005/11/24 11:24:06  haraldkipp
 * Initial check-in.
 * Many thanks to William Basser for this code and also to Przemyslaw Rudy
 * for several enhancements.
 *
 */

#include <dev/tlc16c550.h>

/*!
 * \addtogroup xgNutArchAvrAce
 */
/*@{*/


static ACEDCB dcb_ace0;
static IFSTREAM ifs_ace0;

/*!
 * \brief ACE 0 Device information structure.
 */
NUTDEVICE devAce0 = {
    0,                          /*!< Pointer to next device. */
    {'a', 'c', 'e', '0', 0, 0, 0, 0, 0},        /*!< Unique device name. */
    IFTYP_STREAM,               /*!< Type of device. */
    0x8c00,                     /*!< Base address. */
    4,                          /*!< First interrupt number. */
    &ifs_ace0,                  /*!< Interface control block. */
    &dcb_ace0,                  /*!< Driver control block. */
    AceInit,                    /*!< Driver initialization routine. */
    AceIOCtl,                   /*!< Driver specific control function. */
    AceRead,                    /*!< Read from device. */
    AceWrite,                   /*!< Write to device. */
    AceWrite_P,                 /*!< Write to device from program space. */
    AceOpen,                    /*!< Open a device or file. */
    AceClose,                   /*!< Close a device or file. */
    AceSize                     /*!< Request file size. */
};

static ACEDCB dcb_ace1;
static IFSTREAM ifs_ace1;

/*!
 * \brief ACE 1 Device information structure.
 */
NUTDEVICE devAce1 = {
    0,                          /*!< Pointer to next device. */
    {'a', 'c', 'e', '1', 0, 0, 0, 0, 0},        /*!< Unique device name. */
    IFTYP_STREAM,               /*!< Type of device. */
    0x8c08,                     /*!< Base address. */
    4,                          /*!< First interrupt number. */
    &ifs_ace1,                  /*!< Interface control block. */
    &dcb_ace1,                  /*!< Driver control block. */
    AceInit,                    /*!< Driver initialization routine. */
    AceIOCtl,                   /*!< Driver specific control function. */
    AceRead,                    /*!< Read from device. */
    AceWrite,                   /*!< Write to device. */
    AceWrite_P,                 /*!< Write to device from program space. */
    AceOpen,                    /*!< Open a device or file. */
    AceClose,                   /*!< Close a device or file. */
    AceSize                     /*!< Request file size. */
};

static ACEDCB dcb_ace2;
static IFSTREAM ifs_ace2;

/*!
 * \brief ACE 2 Device information structure.
 */
NUTDEVICE devAce2 = {
    0,                          /*!< Pointer to next device. */
    {'a', 'c', 'e', '2', 0, 0, 0, 0, 0},        /*!< Unique device name. */
    IFTYP_STREAM,               /*!< Type of device. */
    0x8c10,                     /*!< Base address. */
    4,                          /*!< First interrupt number. */
    &ifs_ace2,                  /*!< Interface control block. */
    &dcb_ace2,                  /*!< Driver control block. */
    AceInit,                    /*!< Driver initialization routine. */
    AceIOCtl,                   /*!< Driver specific control function. */
    AceRead,                    /*!< Read from device. */
    AceWrite,                   /*!< Write to device. */
    AceWrite_P,                 /*!< Write to device from program space. */
    AceOpen,                    /*!< Open a device or file. */
    AceClose,                   /*!< Close a device or file. */
    AceSize                     /*!< Request file size. */
};

static ACEDCB dcb_ace3;
static IFSTREAM ifs_ace3;

/*!
 * \brief ACE 3 Device information structure.
 */
NUTDEVICE devAce3 = {
    0,                          /*!< Pointer to next device. */
    {'a', 'c', 'e', '3', 0, 0, 0, 0, 0},        /*!< Unique device name. */
    IFTYP_STREAM,               /*!< Type of device. */
    0x8c18,                     /*!< Base address. */
    4,                          /*!< First interrupt number. */
    &ifs_ace3,                  /*!< Interface control block. */
    &dcb_ace3,                  /*!< Driver control block. */
    AceInit,                    /*!< Driver initialization routine. */
    AceIOCtl,                   /*!< Driver specific control function. */
    AceRead,                    /*!< Read from device. */
    AceWrite,                   /*!< Write to device. */
    AceWrite_P,                 /*!< Write to device from program space. */
    AceOpen,                    /*!< Open a device or file. */
    AceClose,                   /*!< Close a device or file. */
    AceSize                     /*!< Request file size. */
};

/*@}*/
