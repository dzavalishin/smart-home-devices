/*! \file ds2482.c \brief Dallas DS2482 I2C-to-Dallas1Wire Master Library. */
//*****************************************************************************
//
// File Name	: 'ds2482.c'
// Title		: Dallas DS2482 I2C-to-Dallas1Wire Master Library
// Author		: Pascal Stang - Copyright (C) 2004
// Created		: 2004.09.27
// Revised		: 2004.09.27
// Version		: 0.1
// Target MCU	: Atmel AVR Series
//
// NOTE: This code is currently below version 1.0, and therefore is considered
// to be lacking in some functionality or documentation, or may not be fully
// tested.  Nonetheless, you can expect most functions to work.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include <avr/io.h>
//#include <avr/signal.h>
#include <avr/interrupt.h>

#include <dev/twif.h>

#include <stdio.h>


//#include "global.h"
//#include "i2c.h"
#include "ds2482.h"

//#include "rprintf.h"
#include "util.h"

// global variables
uint8_t DS2482I2cAddr;

// Functions
uint8_t ds2482Init(uint8_t i2cAddr)
{
	// select device
	DS2482I2cAddr = i2cAddr;
	// reset DS2482 chip
	return ds2482Reset(DS2482I2cAddr);

}

// return true if devices exist
uint8_t ds2482Reset(uint8_t i2cAddr)
{
	// select device
	DS2482I2cAddr = i2cAddr;
	return ds2482SendCmd(DS2482_CMD_DRST);
}


// Returns 0 on success, err code on err
static uint8_t i2cMasterReceiveNI(uint8_t addr, uint8_t len, uint8_t *buf)
{
	int ret = TwMasterTransact( addr, 0, 0, buf, len, 50 );
    if(ret < 0)
    {
        return TwMasterError();
    }

    return 0;
}

// Returns 0 on success, err code on err
uint8_t ds2482SendCmd(uint8_t cmd)
{
#if 1
    // 50 msec is too much?
    int ret = TwMasterTransact(DS2482I2cAddr, &cmd, 1, 0, 0, 50 );

    if(ret < 0)
    {
        return TwMasterError();
    }

    return 0;
    //return ret == 0 ? 0 : -1;

#else
	uint8_t data;
	uint8_t i2cStat;

	// send command
	i2cStat = i2cMasterSendNI(DS2482I2cAddr, 1, &cmd);
	if(i2cStat == I2C_ERROR_NODEV)
	{
		printf("No I2C Device\r\n");
		return i2cStat;
	}
	// check status
	i2cStat = i2cMasterReceiveNI(DS2482I2cAddr, 1, &data);

//	rprintf("Cmd=0x%x  Status=0x%x\r\n", cmd, data);

        return (i2cStat == I2C_OK);
#endif
}

// Returns 0 on success, err code on err
uint8_t ds2482SendCmdArg(uint8_t cmd, uint8_t arg)
{
#if 1
    uint8_t data[2];

    // prepare command
    data[0] = cmd;
    data[1] = arg;

//DEBUG_PUTS("ds2482SendCmdArg TwMasterTransact.. ");
    // 50 msec is too much?
    int ret = TwMasterTransact(DS2482I2cAddr, data, 2, 0, 0, 50 );
//DEBUG_PUTS("ds2482SendCmdArg TwMasterTransact out.. ");
    if(ret < 0)
    {
        return TwMasterError();
    }

    return 0;
    //return ret == reply_size ? 0 : -1;
#else
	uint8_t data[2];
	uint8_t i2cStat;

	// prepare command
	data[0] = cmd;
	data[1] = arg;
	// send command
	i2cStat = i2cMasterSendNI(DS2482I2cAddr, 2, data);
	if(i2cStat == I2C_ERROR_NODEV)
	{
		printf("No I2C Device\r\n");
		return i2cStat;
	}
	// check status
	i2cStat = i2cMasterReceiveNI(DS2482I2cAddr, 1, data);

//	rprintf("Cmd=0x%x  Arg=0x%x  Status=0x%x\r\n", cmd, arg, data[0]);

	return (i2cStat == I2C_OK);
#endif
}

// returns 0 on success, error on failure

uint8_t ds2482BusyWait(uint8_t *status_p)
{
    uint8_t ret;
    // set read pointer to status register
//DEBUG_PUTS("ds2482BusyWait cmd.. ");
    ret = ds2482SendCmdArg(DS2482_CMD_SRP, DS2482_READPTR_SR);
//DEBUG_PUTS("ds2482BusyWait cmd done.. ");
    if( ret ) return ret;

    // check status until busy bit is cleared
//DEBUG_PUTS("i2cMasterReceiveNI.. ");
    do
    {
        i2cMasterReceiveNI(DS2482I2cAddr, 1, status_p);
//printf("ds2482BusyWait status = 0x%02X ", *status_p);
    } while(*status_p & DS2482_STATUS_1WB);
    // return the status register value
    return 0;
}

uint8_t ds2482ReadConfig(void)
{
    uint8_t status;
    // set read pointer to config register
    ds2482SendCmdArg(DS2482_CMD_SRP, DS2482_READPTR_CR);
    i2cMasterReceiveNI(DS2482I2cAddr, 1, &status);
    return status;
}



uint8_t ds2482BusReset(void)
{
	uint8_t status, err;
	// send 1-Wire bus reset command
	if( ( err = ds2482SendCmd(DS2482_CMD_1WRS) ) )
	{
		printf("2482 reset error %d", err);
		return 0; // nothing is present
	}
	// wait for bus reset to finish, and get status
	if( ( err = ds2482BusyWait(&status) ) )
	{
		printf("2482 status read error %d", err);
		return 0; // nothing is present
	}
	// return state of the presence bit
	return (status & DS2482_STATUS_PPD);
}

uint8_t ds2482BusTransferBit(uint8_t bit)
{
	uint8_t status;
	// writes and reads a bit on the bus
	// wait for DS2482 to be ready
        if( ds2482BusyWait(&status) )
            return 0; // return error somehow?
	// send 1WSB command
	ds2482SendCmdArg(DS2482_CMD_1WSB, bit?0x00:0x80);
	// wait for command to finish
        if( ds2482BusyWait(&status) )
            return 0; // return error somehow?
	// return read-slot bit value
	if(status & DS2482_STATUS_SBR)
		return 1;
	else
		return 0;
}

uint8_t ds2482BusTriplet(uint8_t dir)
{
	uint8_t status;
	// this command is used to simplify search-rom operations
	// generates two read timeslots and one write timeslot
	// dir input determines value of write if reads are both 0

	// wait for DS2482 to be ready
	ds2482BusyWait(&status); // TODO CHECK 4 ERR! (nonzero return)
	// send 1WSB command
	ds2482SendCmdArg(DS2482_CMD_1WT, dir?0x00:0x80);
	// wait for command to finish
	ds2482BusyWait(&status); // TODO CHECK 4 ERR! (nonzero return)
	// return the value of the read slots
	return (status & (DS2482_STATUS_SBR|DS2482_STATUS_TSB))>>5;
}

uint8_t ds2482BusLevel(void)
{
	uint8_t status;
	// get status
	ds2482BusyWait(&status); // TODO CHECK 4 ERR! (nonzero return)
	// return bus level value
	if(status & DS2482_STATUS_LL)
		return 1;
	else
		return 0;
}

void ds2482BusWriteByte(uint8_t data)
{
	// wait for DS2482 to be ready
	uint8_t status;
	ds2482BusyWait(&status); // TODO CHECK 4 ERR! (nonzero return)
	// send 1WWB command
	ds2482SendCmdArg(DS2482_CMD_1WWB, data);
}

/**
 * Returns 0 on success, err code on error.
 */
uint8_t ds2482BusReadByte(uint8_t *out)
{
    // wait for DS2482 to be ready
    uint8_t status;
    uint8_t  ret = ds2482BusyWait(&status);
    if( ret ) return ret;
    
    // send 1WRB command
    ret = ds2482SendCmd(DS2482_CMD_1WRB);
    if( ret ) return ret;
    // wait for read to finish
    ret = ds2482BusyWait(&status); // TODO CHECK 4 ERR! (nonzero return)
    if( ret ) return ret;
    // set read pointer to data register
    ret = ds2482SendCmdArg(DS2482_CMD_SRP, DS2482_READPTR_RDR);
    if( ret ) return ret;
    // read data
    ret = i2cMasterReceiveNI(DS2482I2cAddr, 1, out);
    if( ret ) return ret;

    return 0;
}
