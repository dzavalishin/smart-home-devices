//*****************************************************************************
// File Name	: dallas.c
// Title		: Dallas 1-Wire Library
// Revision		: 6
// Notes		:
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
//*****************************************************************************

//----- Include Files ---------------------------------------------------------
#include <avr/io.h>				// include I/O definitions (port names, pin names, etc)
#include <avr/interrupt.h>		// include interrupt support
#include <string.h>				// include string support
#include <stdio.h>
#include "delay.h"
#include "ds2482.h"				// include dallas support
#include "dallas.h"				// include dallas support
#include "util.h"

//----- Global Variables -------------------------------------------------------
static uint8_t last_discrep = 0;	// last discrepancy for FindDevices
static uint8_t done_flag = 0;		// done flag for FindDevices

uint8_t dallas_crc;					// current crc global variable
uint8_t dallas_crc_table[] =		// dallas crc lookup table
{
    0, 94,188,226, 97, 63,221,131,194,156,126, 32,163,253, 31, 65,
    157,195, 33,127,252,162, 64, 30, 95, 1,227,189, 62, 96,130,220,
    35,125,159,193, 66, 28,254,160,225,191, 93, 3,128,222, 60, 98,
    190,224, 2, 92,223,129, 99, 61,124, 34,192,158, 29, 67,161,255,
    70, 24,250,164, 39,121,155,197,132,218, 56,102,229,187, 89, 7,
    219,133,103, 57,186,228, 6, 88, 25, 71,165,251,120, 38,196,154,
    101, 59,217,135, 4, 90,184,230,167,249, 27, 69,198,152,122, 36,
    248,166, 68, 26,153,199, 37,123, 58,100,134,216, 91, 5,231,185,
    140,210, 48,110,237,179, 81, 15, 78, 16,242,172, 47,113,147,205,
    17, 79,173,243,112, 46,204,146,211,141,111, 49,178,236, 14, 80,
    175,241, 19, 77,206,144,114, 44,109, 51,209,143, 12, 82,176,238,
    50,108,142,208, 83, 13,239,177,240,174, 76, 18,145,207, 45,115,
    202,148,118, 40,171,245, 23, 73, 8, 86,180,234,105, 55,213,139,
    87, 9,235,181, 54,104,138,212,149,203, 41,119,244,170, 72, 22,
    233,183, 85, 11,136,214, 52,106, 43,117,151,201, 74, 20,246,168,
    116, 42,200,150, 21, 75,169,247,182,232, 10, 84,215,137,107, 53
};

//----- Functions --------------------------------------------------------------

/*--------------------------------------------------------------------------
 * dallasFindNextDevice: find the next device on the bus
 * input................ rom_id - pointer to store the rom id found
 * returns.............. true or false if a device was found
 *-------------------------------------------------------------------------*/
uint8_t dallasFindNextDevice(dallas_rom_id_T* rom_id);

void dallasInit(void)
{
    //ds2482Init();
}

uint8_t dallasReset(void)
{
    // reset the bus
    if(ds2482BusReset())
        return DALLAS_PRESENCE;
    else
        return DALLAS_NO_PRESENCE;
}

uint8_t dallasReadBit(void)
{
    return ds2482BusTransferBit(1);
}

void dallasWriteBit(uint8_t bit)
{
    ds2482BusTransferBit(bit);
}

// Returns 0 on ok or err code on error
uint8_t dallasReadByte(uint8_t *out)
{
    //printf("dsrb ");
    return ds2482BusReadByte(out);
}

void dallasWriteByte(uint8_t byte)
{
    ds2482BusWriteByte(byte);
}

uint8_t dallasReadRAM(dallas_rom_id_T* rom_id, uint16_t addr, uint8_t len, uint8_t *data)
{
    uint8_t i;
    uint8_t error;

    union int16_var_U
    {
        uint16_t i16;
        uint8_t i08[2];
    } int16_var;

    // first make sure we actually have something to do
    if (data == NULL)
        return DALLAS_NULL_POINTER;
    if (len == 0)
        return DALLAS_ZERO_LEN;

    // reset the bus and request the device
    error = dallasMatchROM(rom_id);
    if (error != DALLAS_NO_ERROR)
        return error;

    // enter read mode
    dallasWriteByte(DALLAS_READ_MEMORY);

    // write address one byte at a time
    int16_var.i16 = addr;
    dallasWriteByte(int16_var.i08[0]);
    dallasWriteByte(int16_var.i08[1]);

    // read data from device 1 byte at a time
    for(i=0;i<len;i++)
    {
        if( dallasReadByte(data+i) )
        	return DALLAS_DEVICE_ERROR;
    }

    return DALLAS_NO_ERROR;
}

uint8_t dallasWriteRAM(dallas_rom_id_T* rom_id, uint16_t addr, uint8_t len, uint8_t* data)
{
    uint8_t i;
    uint8_t error;

    union int16_var_U
    {
        uint16_t i16;
        uint8_t i08[2];
    } int16_var;

    // first make sure we actually have something to do
    if (data == NULL)
        return DALLAS_NULL_POINTER;
    if (len == 0)
        return DALLAS_ZERO_LEN;

    // reset the bus and request the device
    error = dallasMatchROM(rom_id);
    if (error != DALLAS_NO_ERROR)
        return error;

    // enter write mode
    dallasWriteByte(DALLAS_WRITE_MEMORY);

    // write address one byte at a time
    int16_var.i16 = addr;
    dallasWriteByte(int16_var.i08[0]);
    dallasWriteByte(int16_var.i08[1]);

    // write data one byte at a time
    for(i=0;i<len;i++)
    {
    	uint8_t ch;
    	
        dallasWriteByte(data[i]);

        // future: Check CRC16, for now just read it so we can go on
        dallasReadByte(&ch);
        dallasReadByte(&ch);

        // verify the data
        dallasReadByte(&ch);
        if (ch != data[i])
            return DALLAS_VERIFY_ERROR;
    }

    return DALLAS_NO_ERROR;
}

void dallasWaitUntilDone(void)
{
    //timerPause(6);

    // wait until we recieve a one
    //	cli();
    //	while(!dallasReadBit());
    //	sei();
    while(!ds2482BusLevel());
}

uint8_t dallasReadROM(dallas_rom_id_T* rom_id)
{
    uint8_t i;

    // reset the 1-wire bus and look for presence
    i = dallasReset();
    if (i != DALLAS_PRESENCE)
        return i;

    // send READ ROM command
    dallasWriteByte(DALLAS_READ_ROM);

    // get the device's ID 1 byte at a time
    for(i=0;i<8;i++)
    {
        if( dallasReadByte( rom_id->byte + i ) )
        	return DALLAS_DEVICE_ERROR;
    }

    return DALLAS_NO_ERROR;
}

uint8_t dallasMatchROM(dallas_rom_id_T* rom_id)
{
    uint8_t i;

    // reset the 1-wire and look for presence
    i = dallasReset();
    if (i != DALLAS_PRESENCE)
        return i;

    // send MATCH ROM command
    dallasWriteByte(DALLAS_MATCH_ROM);

    // write id one byte at a time
    for(i=0;i<8;i++)
        dallasWriteByte(rom_id->byte[i]);

    return DALLAS_NO_ERROR;
}

void dallasPrintROM(dallas_rom_id_T* rom_id)
{
    int8_t i;

    // print out the rom in the format: xx xx xx xx xx xx xx xx
    for(i=7;i>=0;i--)
    {
        printf("%02X ", rom_id->byte[i]);
    }
}

uint8_t dallasAddressCheck(dallas_rom_id_T* rom_id, uint8_t family)
{
    //	uint8_t i;

    //	dallas_crc = 0;

    //	for(i=1;i<7;i++)
    //	{
    //		dallasCRC(rom_id->byte[i]);
    //		rprintfuint8_t(rom_id->byte[i]);
    //		rprintfChar(' ');
    //	}
    //	rprintfCRLF();

    //	rprintfuint8_t(dallas_crc);
    //	rprintfCRLF();

    //run CRC on address

    //make sure we have the correct family
    if (rom_id->byte[DALLAS_FAMILY_IDX] == family)
        return DALLAS_NO_ERROR;

    return DALLAS_ADDRESS_ERROR;
}

uint8_t dallasCRC(uint8_t i)
{
    // update the crc global variable and return it
    dallas_crc = dallas_crc_table[dallas_crc^i];
    return dallas_crc;
}


uint8_t dallasFindNextDevice(dallas_rom_id_T* rom_id)
{
    uint8_t bit;
    uint8_t i = 0;
    uint8_t bit_index = 1;
    uint8_t byte_index = 0;
    uint8_t bit_mask = 1;
    uint8_t discrep_marker = 0;

    printf("1w: search next\n");

    // reset the CRC
    dallas_crc = 0;

    if (done_flag || dallasReset() != DALLAS_PRESENCE)
    {
        // no more devices parts detected
        printf("1w: no more\n");
        return 0;
    }

    // send search ROM command
    dallasWriteByte(DALLAS_SEARCH_ROM);

    // loop until through all 8 ROM bytes
    while(byte_index<8)
    {
        // read line 2 times to determine status of devices
        //    00 - devices connected to bus with conflicting bits
        //    01 - all devices have a 0 in this position
        //    10 - all devices ahve a 1 in this position
        //    11 - there are no devices connected to bus
        i = 0;
        cli();
        if (dallasReadBit())
            i = 2;				// store the msb if 1
        delay_us(120);
        //delay(120);
        if (dallasReadBit())
            i |= 1;				// store the lsb if 1
        sei();

        printf(" i=%d", i);

        if (i==3)
        {
            // there are no devices on the 1-wire
            break;
        }
        else
        {
            if (i>0)
            {
                // all devices coupled have 0 or 1
                // shift 1 to determine if the msb is 0 or 1
                bit = i>>1;
            }
            else
            {
                // if this discrepancy is before the last discrepancy on a
                // previous FindNextDevice then pick the same as last time
                if (bit_index<last_discrep)
                    bit = ((rom_id->byte[byte_index] & bit_mask) > 0);
                else
                    bit = (bit_index==last_discrep);

                // if 0 was picked then record position with bit mask
                if (!bit)
                    discrep_marker = bit_index;
            }

            // isolate bit in rom_id->byte[byte_index] with bit mask
            if (bit)
                rom_id->byte[byte_index] |= bit_mask;
            else
                rom_id->byte[byte_index] &= ~bit_mask;

            // ROM search write
            //cli();
            dallasWriteBit(bit);
            //sei();

            // ncrement bit index counter and shift the bit mask
            bit_index++;
            bit_mask <<= 1;

            if (!bit_mask)
            {
                // if the mask is 0 then go to new ROM
                // accumulate the CRC and incriment the byte index and bit mask
                dallasCRC(rom_id->byte[byte_index]);
                byte_index++;
                bit_mask++;
            }
        }
    }

    if ((bit_index < 65) || dallas_crc)
    {
        // search was unsuccessful - reset the last discrepancy to 0 and return false
        last_discrep = 0;
        return 0;
    }

    // search was successful, so set last_discrep and done_flag
    last_discrep = discrep_marker;
    done_flag = (last_discrep==0);

    return 1;
}






// oneTriplet
//
// Uses the 1-Wire Triplet command.
//
// dir � Returns the direction that was chosen (1) or (0).
// firstBit - Returns the first bit of the search (1) or (0).
// secondBit � Returns the complement of the first bit (1) or (0).
//
// returns SUCCES or FAILURE
//
void owTriplet(uint8_t *dir, uint8_t *firstBit, uint8_t *secondBit)
{
    uint8_t buff[3];
    uint8_t test;
    buff[0] = 0x78;
    if(*dir>0)
        *dir = (uint8_t)0xFF;
    buff[1] = *dir;


    uint8_t status;
    ds2482BusyWait(&status); // TODO CHECK 4 ERR! (nonzero return)
    // send 1WSB command
    //ds2482SendCmdArg(DS2482_CMD_1WT, dir?0x00:0x80);
    ds2482SendCmdArg(DS2482_CMD_1WT, dir?0x00:0xFF);
    // wait for command to finish
    ds2482BusyWait(&status); // TODO CHECK 4 ERR! (nonzero return)


    test = status & 0x20;
    if(test == 0x20)
        *firstBit = 1;
    else
        *firstBit = 0;
    test = status & 0x40;
    if(test == 0x40)
        *secondBit = 1;
    else
        *secondBit = 0;
    test = status & 0x80;
    if(test == 0x80)
        *dir = 1;
    else
        *dir = 0;
}

// Global value for the current serial number
uint8_t SearchSerialNum[8];
uint8_t LastDiscrepancy;

// oneWireSearch
//
// Does a 1-Wire search using the 1-Wire Triplet command.
//
// resetSearch � Reset the search(1) or not(0).
// lastDevice - If the last device has been found(1) or not(0).
// deviceAddress � The returned serial number.
//
// returns SUCCES or FAILURE
//

#define FAILURE 0
#define SUCCESS 1

uint8_t OWSearch(uint8_t resetSearch, uint8_t *lastDevice, uint8_t *deviceAddress)
{
    uint8_t retVal = FAILURE;
    uint8_t bit_number = 1;
    uint8_t last_zero = 0;
    uint8_t serial_byte_number = 0;
    uint8_t serial_byte_mask = 1;
    uint8_t firstBit, secondBit, dir;
    uint8_t i = 0;
    if(resetSearch)
    {
        lastDevice = 0;
        LastDiscrepancy = 0;
    }
    // if the last call was not the last one
    if (!(*lastDevice))
    {
        // reset the 1-wire
        // if there are no parts on 1-wire, return FALSE
        if( dallasReset() != DALLAS_PRESENCE )
        {
            // reset the search
            lastDevice = 0;
            LastDiscrepancy = 0;
            return FAILURE;
        }
        // Issue the Search ROM command
        //OWWireByte(0xF0);

        // send search ROM command
        dallasWriteByte(DALLAS_SEARCH_ROM);


        // loop to do the search
        do
        {
            if (bit_number < LastDiscrepancy)
            {
                if(SearchSerialNum[serial_byte_number] & serial_byte_mask)
                    dir = 1;
                else
                    dir = 0;
            }
            else
            {
                // if equal to last pick 1, if not then pick 0
                if(bit_number==LastDiscrepancy)
                    dir = 1;
                else
                    dir = 0;
            }
            owTriplet(&dir, &firstBit, &secondBit);
            /*if(!owTriplet(&dir, &firstBit, &secondBit))
            {
                return FAILURE;
            } */
            // if 0 was picked then record its position in LastZero
            if (firstBit==0 && secondBit==0 && dir == 0)
            {
                last_zero = bit_number;
            }
            // check for no devices on 1-wire
            if (firstBit==1 && secondBit==1)
                break;
            // set or clear the bit in the SerialNum byte serial_byte_number
            // with mask serial_byte_mask
            if (dir == 1)
                SearchSerialNum[serial_byte_number] |= serial_byte_mask;
            else
                SearchSerialNum[serial_byte_number] &= ~serial_byte_mask;
            // increment the byte counter bit_number
            // and shift the mask serial_byte_mask
            bit_number++;
            serial_byte_mask <<= 1;
            // if the mask is 0 then go to new SerialNum[portnum] byte serial_byte_number
            // and reset mask
            if (serial_byte_mask == 0)
            {
                serial_byte_number++;
                serial_byte_mask = 1;
            }
        }
        while(serial_byte_number < 8); // loop until through all SerialNum[portnum]
        retVal = FAILURE;
        // if the search was successful then
        if (bit_number == 65)//|| crcl))
        {
            // search successful so set LastDiscrepancy,LastDevice
            LastDiscrepancy = last_zero;
            if(LastDiscrepancy==0)
                *lastDevice = SUCCESS;
            else
                *lastDevice = FAILURE;
            for(i=0; i<8; i++)
            {
                deviceAddress[i] = SearchSerialNum[i];
            }
            return SUCCESS;
        }
    }
    // if no device found then reset counters so next 'next' will be
    // like a first
    if (!retVal || !SearchSerialNum[0])
    {
        LastDiscrepancy = 0;
        *lastDevice = FAILURE;
        retVal = FAILURE;
    }
    return retVal;
}



#if 0
uint8_t dallasFindDevices(dallas_rom_id_T rom_id[], uint8_t max)
{
    uint8_t num_found = 0;
    dallas_rom_id_T id;

    // reset the rom search last discrepancy global
    last_discrep = 0;
    done_flag = 0;

    // check to make sure presence is detected before we start
    if (dallasReset() == DALLAS_PRESENCE)
    {
        // --> stang
        //while (dallasFindNextDevice(&rom_id[num_found]) && (num_found<DALLAS_MAX_DEVICES))
        //	num_found++;

        // continues until no additional devices are found
        while (dallasFindNextDevice(&id) && (num_found<max))
        {
            memcpy(&rom_id[num_found++], &id, 8);
            printf("1wire: 0x%02X 0x%02X device found\n", id.byte[0], id.byte[7]);
        }
    }

    return num_found;
}
#else
uint8_t dallasFindDevices(dallas_rom_id_T rom_id[], uint8_t max)
{
    uint8_t num_found = 0;
    dallas_rom_id_T id;

    // reset the rom search last discrepancy global
    last_discrep = 0;
    done_flag = 0;

    char lastDevice;


    // check to make sure presence is detected before we start
    if (dallasReset() != DALLAS_PRESENCE)
        return 0;

    char first = 1;

    // continues until no additional devices are found
    while (  num_found<max )
    {
        if(FAILURE != OWSearch(first, &lastDevice, (void *)&id))
            break;

        first = 0;
        memcpy(&rom_id[num_found++], &id, 8);
        memcpy( &lastDevice, &id, 8);
        printf("1wire: 0x%02X 0x%02X device found\n", id.byte[0], id.byte[7]);

        //if(lastDevice) break;
    }

    return num_found;
}

#endif















void dallasPrintError(uint8_t error)
{
    // if there was not an error, return
    if (error == DALLAS_NO_ERROR)
        return;

    // print header message
    printf("ERROR %c:", error);

    // print custom error message
    switch (error)
    {
    case DALLAS_NO_PRESENCE:
        DEBUG_PUTS("no presence detected");
        break;
    case DALLAS_INVALID_CHANNEL:
    	DEBUG_PUTS("Invalid Chan");
        break;
    case DALLAS_VERIFY_ERROR:
    	DEBUG_PUTS("Verify");
        break;
    case DALLAS_ADDRESS_ERROR:
    	DEBUG_PUTS("Bad Addr");
        break;
    case DALLAS_CRC_ERROR:
    	DEBUG_PUTS("Data CRC");
        break;
    case DALLAS_DEVICE_ERROR:
    	DEBUG_PUTS("No response");
        break;
    case DALLAS_FORMAT_ERROR:
    	DEBUG_PUTS("Bad return format");
        break;
    case DALLAS_NULL_POINTER:
    	DEBUG_PUTS("Null Pointer");
        break;
    case DALLAS_ZERO_LEN:
    	DEBUG_PUTS("RAM rd/wr 0 bytes");
        break;
    case DALLAS_BUS_ERROR:
    	DEBUG_PUTS("Bus error, check pullup");
        break;
    case DALLAS_RESOLUTION_ERROR:
    	DEBUG_PUTS("resolution out of range");
        break;
    default:
    	DEBUG_PUTS("Unknown");
    }
    putchar('\n');
}
