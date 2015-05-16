/****************************************************************************
*  This file is part of the Ethernut port for the LPC2XXX
*
*  Copyright (c) 2005 by Michael Fischer. All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*  1. Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*  2. Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in the
*     documentation and/or other materials provided with the distribution.
*  3. Neither the name of the author nor the names of its contributors may
*     be used to endorse or promote products derived from this software
*     without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
*  THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
*  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
*  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
*  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
*  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
*  SUCH DAMAGE.
*
****************************************************************************
*
*  History:
*
*  24.09.05  mifi   First Version
*                   The CrossWorks for ARM toolchain will be used.
****************************************************************************/

#include <string.h>

#include <arch/arm/lpc2xxx.h>
#include <dev/debug.h>

#include <sys/device.h>
#include <sys/file.h>

static NUTFILE dbgfile;

/*!
 * \brief Handle I/O controls for debug device.
 *
 * The debug device doesn't support any.
 *
 * \return Always -1.
 */
int DebugIOCtl(NUTDEVICE * dev, int req, void *conf)
{
  return -1;
}

/*!
 * \brief Initialize debug device.
 *
 * Simply enable the device. 115.2 kBaud at 14.7456 MHz.
 *
 * \return Always 0.
 */
int DebugInit(NUTDEVICE * dev)
{
  if        (dev->dev_name[4] == '0') {
    U0LCR = _BV(7);
    U0DLL = 0x08;
    U0DLM = 0x00;
    U0LCR = 0x03;
    U0IER = 0x00;
    U0FCR = 0x00;

    PINSEL0  |= 0x00000005;

  } else if (dev->dev_name[4] == '1') {
    U1LCR = _BV(7);
    U1DLL = 0x08;
    U1DLM = 0x00;
    U1LCR = 0x03;
    U1IER = 0x00;
    U1FCR = 0x00;

    PINSEL0  |= 0x00050000;
  }

  return 0;
}

/*!
 * \brief Send a single character to debug device 0.
 *
 * A carriage return character will be automatically appended
 * to any linefeed.
 */
void DebugPut0(char ch)
{
  if(ch == '\n') {
	  while ((U0LSR & U0LSR_THRE) == 0);
	  U0THR = '\r';
  }
  while ((U0LSR & U0LSR_THRE) == 0);
  U0THR = ch;
}

/*!
 * \brief Send a single character to debug device 1.
 *
 * A carriage return character will be automatically appended
 * to any linefeed.
 */
void DebugPut1(char ch)
{
  if(ch == '\n') {
	  while ((U1LSR & U1LSR_THRE) == 0);
	  U1THR = '\r';
  }
  while ((U1LSR & U1LSR_THRE) == 0);
  U1THR = ch;
}

/*!
 * \brief Send characters to debug device.
 *
 * A carriage return character will be automatically appended
 * to any linefeed.
 *
 * \return Number of characters sent.
 */
int DebugWrite(NUTFILE * fp, CONST void *buffer, int len)
{
  int         c   = len;
  CONST char *cp  = buffer;
  NUTDEVICE  *dev = fp->nf_dev;

  if        (dev->dev_name[4] == '0') {
    while(c--)
      DebugPut0(*cp++);
  } else if (dev->dev_name[4] == '1') {
    while(c--)
      DebugPut1(*cp++);
  }

  return len;
}

/*!
 * \brief Open debug device.
 *
 * \return Pointer to a static NUTFILE structure.
 */
NUTFILE *DebugOpen(NUTDEVICE * dev, CONST char *name, int mode, int acc)
{
  dbgfile.nf_next = 0;
  dbgfile.nf_dev = dev;
  dbgfile.nf_fcb = 0;

  return (&dbgfile);
}

/*!
 * \brief Close debug device.
 *
 * \return Always 0.
 */
int DebugClose(NUTFILE * fp)
{
  return (0);
}

/*!
 * \brief Debug device 0 information structure.
 */
NUTDEVICE devDebug0 = {
  0,                          /*!< Pointer to next device, dev_next. */
  {'u', 'a', 'r', 't', '0', 0, 0, 0, 0},      /*!< Unique device name, dev_name. */
  0,                          /*!< Type of device, dev_type. */
  0xFFFD0000,                 /*!< Base address, dev_base. */
  0,                          /*!< First interrupt number, dev_irq. */
  0,                          /*!< Interface control block, dev_icb. */
  0,                          /*!< Driver control block, dev_dcb. */
  DebugInit,                  /*!< Driver initialization routine, dev_init. */
  DebugIOCtl,                 /*!< Driver specific control function, dev_ioctl. */
  0,                          /*!< dev_read. */
  DebugWrite,                 /*!< dev_write. */
  DebugOpen,                  /*!< dev_opem. */
  DebugClose,                 /*!< dev_close. */
  0                           /*!< dev_size. */
};

/*!
 * \brief Debug device 1 information structure.
 */
NUTDEVICE devDebug1 = {
  0,                          /*!< Pointer to next device, dev_next. */
  {'u', 'a', 'r', 't', '1', 0, 0, 0, 0},      /*!< Unique device name, dev_name. */
  0,                          /*!< Type of device, dev_type. */
  0xFFFCC000,                 /*!< Base address, dev_base. */
  0,                          /*!< First interrupt number, dev_irq. */
  0,                          /*!< Interface control block, dev_icb. */
  0,                          /*!< Driver control block, dev_dcb. */
  DebugInit,                  /*!< Driver initialization routine, dev_init. */
  DebugIOCtl,                 /*!< Driver specific control function, dev_ioctl. */
  0,                          /*!< dev_read. */
  DebugWrite,                 /*!< dev_write. */
  DebugOpen,                  /*!< dev_opem. */
  DebugClose,                 /*!< dev_close. */
  0                           /*!< dev_size. */
};
