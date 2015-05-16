/****************************************************************************
*  This file is part of the AVRIDE device driver.
*
*  Copyright (c) 2002-2003 by Michael Fischer. All rights reserved.
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
*  History:
*
*  14.12.02  mifi   First Version 
*  18.01.03  mifi   Change Licence from GPL to BSD
*  25.01.03  mifi   Change IDEInit. 
*  29.01.03  mifi   Remove IDE_MAX_SUPPORTED_DEVICE, support 2 drives now.
*  29.06.03  mifi   First ATAPI-Version
****************************************************************************/
#ifndef __IDE_H__
#define __IDE_H__

#include <fs/typedefs.h>

/*-------------------------------------------------------------------------*/
/* global defines                                                          */
/*-------------------------------------------------------------------------*/
//
// Here we can switch on/off some
// feature of the software
//
#define IDE_SUPPORT_WRITE               0
#define IDE_SUPPORT_ATAPI               1

#define ATAPI_START_SEARCH_SECTOR       16
#define ATAPI_MAX_SEARCH_SECTOR         100

#define IDE_OK                          0x00
#define IDE_ERROR                       0x01
#define IDE_DRIVE_NOT_FOUND             0x02
#define IDE_PARAM_ERROR                 0x03
#define IDE_BUSY                        0x04
#define IDE_NOT_SUPPORTED               0x08

#define IDE_DRIVE_C                     0
#define IDE_DRIVE_D                     1

//
// IDE type for IDEInit
//
#define IDE_HARDDISK                    0
#define IDE_HARDDISK_7MHZ               1
#define IDE_COMPACT_FLASH               2
#define MEM_8BIT_COMPACT_FLASH          3

//
// Sector size for HD/CF-Card and CDROM
//
#define IDE_SECTOR_SIZE                 512
#define ATAPI_SECTOR_SIZE               2048
#define MAX_SECTOR_SIZE                 2048

/*-------------------------------------------------------------------------*/
/* global types                                                            */
/*-------------------------------------------------------------------------*/
typedef void IDE_MOUNT_FUNC(int nDevice);

/*-------------------------------------------------------------------------*/
/* global macros                                                           */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* Prototypes                                                              */
/*-------------------------------------------------------------------------*/
int IDEInit(int nBaseAddress, int nIDEMode, IDE_MOUNT_FUNC * pMountFunc, IDE_MOUNT_FUNC * pUnMountFunc);

int IDEMountDevice(BYTE bDevice, BYTE * pSectorBuffer);

int IDEGetSectorSize(BYTE bDevice);

int IDEIsCDROMDevice(BYTE bDevice);

int IDEIsZIPDevice(BYTE bDevice);

int IDEUnMountDevice(BYTE bDevice);

DWORD IDEGetTotalSectors(BYTE bDevice);

int IDEReadSectors(BYTE bDevice, void *pData, DWORD dwStartSector, WORD wSectorCount);

#if (IDE_SUPPORT_WRITE == 1)

int IDEWriteSectors(BYTE bDevice, void *pData, DWORD dwStartSector, WORD wSectorCount);

#endif

#if (IDE_SUPPORT_ATAPI == 1)

int IDEATAPISetCDSpeed(BYTE bDevice, WORD wSpeed);

#endif

#endif                          /* !__IDE_H__ */
