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
*  18.01.03  mifi   Change Licence from GPL to BSD.
*  19.06.03  mifi   Change address for ide port from 0x8000 to 0xE000
*  29.06.03  mifi   First ATAPI-Version
****************************************************************************/
#ifndef __IDEP_H__
#define __IDEP_H__

#include <dev/ide.h>
#include <fs/typedefs.h>

/*-------------------------------------------------------------------------*/
/* global defines                                                          */
/*-------------------------------------------------------------------------*/
//
// If the driver should support the old
// CHS drives, set IDE_SUPPORT_CHS to 1
// 
#define IDE_SUPPORT_CHS         0

#define IDE_BASE_ADDRESS        0xE000
#define IDECS0                  0x0000
#define IDECS1                  0x0500

#define DATA_WRITE_REG_LOW      (IDECS0 + 0x0100)
#define DATA_WRITE_REG_HIGH     (IDECS0 + 0x0200)

#define DATA_READ_REG_LOW       (IDECS0 + 0x0300)
#define DATA_READ_REG_HIGH      (IDECS0 + 0x0400)

#define ERROR_REG               (IDECS0 + 1)
#define FEATURE_REG             (IDECS0 + 1)
#define SECTOR_COUNT_REG        (IDECS0 + 2)
#define SECTOR_REG              (IDECS0 + 3)
#define CYLINDER_LOW_REG        (IDECS0 + 4)
#define CYLINDER_HIGH_REG       (IDECS0 + 5)
#define DISK_HEAD_REG           (IDECS0 + 6)
#define COMMAND_REG             (IDECS0 + 7)
#define STATUS_REG              (IDECS0 + 7)

#define LBA_0_7                 (IDECS0 + 3)
#define LBA_8_15                (IDECS0 + 4)
#define LBA_16_23               (IDECS0 + 5)

#define CF_ERROR_REG            (IDECS1 + 0)

#define CONTROL_REG             (IDECS1 + 6)
#define ALTERNATE_STATUS        (IDECS1 + 6)

#define STATUS_BUSY             0x80
#define STATUS_DRIVE_READY      0x40
#define STATUS_DEVIDE_FAULT     0x20
#define STATUS_SEEK_COMPLETE    0x10
#define STATUS_DATA_REQUEST     0x08
#define STATUS_CORRECTED_DATA   0x04
#define STATUS_INDEX            0x02
#define STATUS_ERROR            0x01

#define ERROR_CRC               0x80
#define ERROR_UNC               0x40
#define ERROR_IDN               0x10
#define ERROR_ABT               0x04
#define ERROR_T0N               0x02
#define ERROR_AMN               0x01

#define COMMAND_DEVICE_INFO     0xEC
#define COMMAND_DEVICE_INFO_P   0xA1
#define COMMAND_READ_SECTORS    0x20
#define COMMAND_WRITE_SECTORS   0x30
#define COMMAND_RECALIBRATE     0x10
#define COMMAND_DIAG            0x90

#define SERIAL_SIZE             20
#define REV_SIZE                8
#define MODEL_SIZE              40

//
// ATAPI stuff
//
#define ATAPI_CMD_READ_CAPACITY 0x25
#define ATAPI_CMD_MODE_SELECT   0x55
#define ATAPI_CMD_MODE_SENSE    0x5A
#define ATAPI_CMD_READ12        0xA8
#define ATAPI_CMD_SET_CD_SPEED  0xBB


/*-------------------------------------------------------------------------*/
/* global types                                                            */
/*-------------------------------------------------------------------------*/

typedef struct {
    unsigned UnUsed0:1;         //  0
    unsigned HardSector_O:1;    //  0
    unsigned SoftSector_O:1;    //  0
    unsigned MFM_O:1;           //  0
    unsigned SlowHead_O:1;      //  0
    unsigned SynchDriveMotor_O:1;       //  0
    unsigned FixedDevice:1;     //  0
    unsigned RemoveableDevice:1;        //  0
    unsigned DiskTransferRate_O:3;      //  0
    unsigned InaccurateSpeed_O:1;       //  0
    unsigned DataStrobeOffset_O:1;      //  0
    unsigned TrackOffsetOption_O:1;     //  0
    unsigned FormatGapRequired_O:1;     //  0
    unsigned ATAPI:1;           //  0

    WORD Cylinders;             //  1
    WORD UnUsed1;               //  2
    WORD Heads;                 //  3
    WORD BytesPerTrack_O;       //  4
    WORD BytesPerSector_O;      //  5
    WORD SectorsPerTrack;       //  6
    WORD VendorSpecific[3];     //  7 - 9
    char SerialNumber[SERIAL_SIZE];     //  10 - 19
    WORD BufferType_O;          //  20
    WORD BufferSizeSectors_O;   //  21
    WORD ECCBytes;              //  22
    char FirmwareRevision[REV_SIZE];    //  23 - 26
    char Model[MODEL_SIZE];     //  27 - 46
    BYTE MaxMultipleSectors;    //  47
    BYTE UnUsed2;               //  47
    WORD Can32Bit_O;            //  48

    unsigned CanDMA_O:1;        //  49
    unsigned UnUsed3:9;         //  49
    unsigned CanDisableIORDY:1; //  49
    unsigned IORDYSupported:1;  //  49
    unsigned AdvancedTransferMode:1;    //  49
    unsigned ATA3StandbyTimer:1;        //  49
    unsigned UnUsed4:2;

    WORD UnUsed5;               //  50

    BYTE UnUsed6;               //  51
    BYTE PIOTransferSpeed;      //  51
    BYTE UnUsed7;               //  52
    BYTE DMATransferSpeed;      //  52

    unsigned Fields_6C_75_Valid:1;      //  53
    unsigned Fields_80_8C_Valid:1;      //  53
    unsigned UnUsed8:14;        //  53

    WORD Cylinders2;            //  54
    WORD Heads2;                //  55
    WORD SectorsPerTrack2;      //  56
    DWORD Sectors;              //  57,58

    BYTE MaxSectorsPerInt;      //  59
    unsigned MaxSectorsPerIntIsValid:1; //  59
    unsigned UnUsed9:7;         //  59

    DWORD LBASectors;           //  60,61

    WORD UnUsed10;              //  62

    BYTE SupportedDMAModes;     //  63
    BYTE ActiveDMAModes;        //  63

    BYTE PIOModesSupported;     //  64
    BYTE UnUsed11;              //  64

    WORD DMACycleTime;          //  65
    WORD RecommendedDMACycleTime;       //  66
    WORD PIOCycleTime;          //  67
    WORD IORDYPIOCycleTime;     //  68

    WORD Reserved1[2];          //  69 - 70
    WORD Reserved2[4];          //  71 - 74

    WORD QueueDepth;            //  75

    WORD Reserved3[4];          //  76 - 79

    WORD ATAVersionMajor;       //  80
    WORD ATAVersionMinor;       //  81

    WORD CommandSetSupported[2];        //  82 - 83
    WORD CommandSetSupportedExtension;  //  84
    WORD CommandSetEnabled[2];  //  85 - 86
    WORD CommandSetDefault;     //  87

    WORD UltraDMAMode;          //  88

    WORD SecurityStatus[2];     //  89 - 90

    WORD AdvancedPowerManagement;       //  91
    WORD MasterPasswordRevisionCode;    //  92

    WORD HardwareResetResult;   //  93

    WORD AcousticManagementValue;       //  94

    WORD Reserved4[5];          //  95 - 99

    WORD MaximumUserLBA48[4];   //  100 - 103

    WORD Reserved5[23];         //  104 - 126

} IDEDEVICEINFO;

/*-------------------------------------------------------------------------*/
/* global macros                                                           */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* Prototypes                                                              */
/*-------------------------------------------------------------------------*/

#endif                          /* !__IDE_H__ */
