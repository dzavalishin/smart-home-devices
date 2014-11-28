/*********************************************************************************
 Title:    DS18X20-Functions via One-Wire-Bus
 Author:   Martin Thomas <eversmith@heizung-thomas.de>
 http://www.siwawi.arubi.uni-kl.de/avr-projects
 Software: avr-gcc 3.4.1 / avr-libc 1.0.4
 Hardware: any AVR - tested with ATmega16/ATmega32 and 3 DS18B20

 Partly based on code from Peter Dannegger and others

 changelog:
 20041124 - Extended measurements for DS18(S)20 contributed by Carsten Foss (CFO)
 200502xx - function DS18X20_read_meas_single
 20050310 - DS18x20 EEPROM functions (can be disabled to save flash-memory)
 (DS18X20_EEPROMSUPPORT in ds18x20.h)

 **********************************************************************************/

#include <avr/io.h>

#include "ds18x20.h"
#include "onewire.h"
#include "crc8.h"

#include "dallas.h"
#include "runtime_cfg.h"


//#define DS18X20_VERBOSE

#ifdef DS18X20_VERBOSE
#include <stdio.h>
#endif




void dallasCommand( uint8_t command, uint8_t *id )
{
	uint8_t i;

	dallasReset();

	if( id ) {
		dallasWriteByte( OW_MATCH_ROM );			// to a single device
		i = OW_ROMCODE_SIZE;
		do {
			dallasWriteByte( *id );
			id++;
		} while( --i );
	}
	else {
		dallasWriteByte( OW_SKIP_ROM );			// to all devices
	}

	dallasWriteByte( command );
}








/*
 convert raw value from DS18x20 to Celsius
 input is:
 - familycode fc (0x10/0x28 see header)
 - scratchpad-buffer
 output is:
 - cel full celsius
 - fractions of celsius in millicelsius*(10^-1)/625 (the 4 LS-Bits)
 - subzero =0 positiv / 1 negativ
 always returns  DS18X20_OK
 TODO invalid-values detection (but should be covered by CRC)
 */
uint8_t DS18X20_meas_to_cel( uint8_t fc, uint8_t *sp,
                             uint8_t* subzero, uint8_t* cel, uint8_t* cel_frac_bits)
{
    uint16_t meas;
    uint8_t  i;

    meas = sp[0];  // LSB
    meas |= ((uint16_t)sp[1])<<8; // MSB
    //meas = 0xff5e; meas = 0xfe6f;

    //  only work on 12bit-base
    if( fc == DS18S20_ID ) { // 9 -> 12 bit if 18S20
        /* Extended measurements for DS18S20 contributed by Carsten Foss */
        meas &= (uint16_t) 0xfffe;	// Discard LSB , needed for later extended precicion calc
        meas <<= 3;					// Convert to 12-bit , now degrees are in 1/16 degrees units
        meas += (16 - sp[6]) - 4;	// Add the compensation , and remember to subtract 0.25 degree (4/16)
    }

    // check for negative
    if ( meas & 0x8000 )  {
        *subzero=1;      // mark negative
        meas ^= 0xffff;  // convert to positive => (twos complement)++
        meas++;
    }
    else *subzero=0;

    // clear undefined bits for B != 12bit
    if ( fc == DS18B20_ID ) { // check resolution 18B20
        i = sp[DS18B20_CONF_REG];
        if ( (i & DS18B20_12_BIT) == DS18B20_12_BIT ) ;
        else if ( (i & DS18B20_11_BIT) == DS18B20_11_BIT )
            meas &= ~(DS18B20_11_BIT_UNDF);
        else if ( (i & DS18B20_10_BIT) == DS18B20_10_BIT )
            meas &= ~(DS18B20_10_BIT_UNDF);
        else { // if ( (i & DS18B20_9_BIT) == DS18B20_9_BIT ) {
            meas &= ~(DS18B20_9_BIT_UNDF);
        }
    }

    *cel  = (uint8_t)(meas >> 4);
    *cel_frac_bits = (uint8_t)(meas & 0x000F);

    return DS18X20_OK;
}







/*
 convert raw value from DS18x20 to binary word
 input is:
 - familycode fc (0x10/0x28 see header)
 - scratchpad-buffer
 return is:
 - 16 bit word with decimal point between 4 and 3 (start from 0) digits
 - ie low 4 bits are fractions
 */
uint16_t DS18X20_meas_to_word( uint8_t fc, uint8_t *sp)
{
    uint16_t meas;
    uint8_t  i;

    meas = sp[0];  // LSB
    meas |= ((uint16_t)sp[1])<<8; // MSB
    //meas = 0xff5e; meas = 0xfe6f;

    //  only work on 12bit-base
    if( fc == DS18S20_ID ) { // 9 -> 12 bit if 18S20
        /* Extended measurements for DS18S20 contributed by Carsten Foss */
        meas &= (uint16_t) 0xfffe;	// Discard LSB , needed for later extended precicion calc
        meas <<= 3;					// Convert to 12-bit , now degrees are in 1/16 degrees units
        meas += (16 - sp[6]) - 4;	// Add the compensation , and remember to subtract 0.25 degree (4/16)
    }

    uint8_t subzero = 0;

    // check for negative
    if ( meas & 0x8000 )  {
        subzero=1;      // mark negative
        meas ^= 0xffff;  // convert to positive => (twos complement)++
        meas++;
    }
    else subzero=0;

    // clear undefined bits for B != 12bit
    if ( fc == DS18B20_ID ) { // check resolution 18B20
        i = sp[DS18B20_CONF_REG];
        if ( (i & DS18B20_12_BIT) == DS18B20_12_BIT ) ;
        else if ( (i & DS18B20_11_BIT) == DS18B20_11_BIT )
            meas &= ~(DS18B20_11_BIT_UNDF);
        else if ( (i & DS18B20_10_BIT) == DS18B20_10_BIT )
            meas &= ~(DS18B20_10_BIT_UNDF);
        else { // if ( (i & DS18B20_9_BIT) == DS18B20_9_BIT ) {
            meas &= ~(DS18B20_9_BIT_UNDF);
        }
    }

    return subzero ? -meas : meas;
}


/* find DS18X20 Sensors on 1-Wire-Bus
 input/ouput: diff is the result of the last rom-search
 output: id is the rom-code of the sensor found */
void DS18X20_find_sensor(uint8_t *diff, uint8_t id[])
{
    for (;;) {
        *diff = ow_rom_search( *diff, &id[0] );
        if ( *diff==OW_PRESENCE_ERR || *diff==OW_DATA_ERR ||
             *diff == OW_LAST_DEVICE ) return;
        if (
            id[0] == DS18B20_ID ||
            id[0] == DS18S20_ID ||
            id[0] == DS1822_ID ||
            id[0] == DS2401_ID
           ) return;
    }
}

/* get power status of DS18x20
 input  : id = rom_code
 returns: DS18X20_POWER_EXTERN or DS18X20_POWER_PARASITE */
uint8_t	DS18X20_get_power_status(uint8_t id[])
{
    uint8_t pstat;
    ow_reset();
    ow_command(DS18X20_READ_POWER_SUPPLY, id);
    pstat=ow_bit_io(1); // pstat 0=is parasite/ !=0 ext. powered
    ow_reset();
    return (pstat) ? DS18X20_POWER_EXTERN:DS18X20_POWER_PARASITE;
}




/* start measurement (CONVERT_T) for all sensors if input id==NULL
 or for single sensor. then id is the rom-code */
uint8_t DS18X20_start_meas( uint8_t with_power_extern, uint8_t id[])
{
	if( onewire_2482_available )
	{


		dallasReset(); 
		if( ow_input_pin_state() ) { // only send if bus is "idle" = high
			dallasCommand( DS18X20_CONVERT_T, id );
			if (with_power_extern != DS18X20_POWER_EXTERN)
				ow_parasite_enable();
			return DS18X20_OK;
		}
		else {
#ifdef DS18X20_VERBOSE
			puts( "DS18X20_start_meas: Short Circuit!\r" );
#endif
			return DS18X20_START_FAIL;
		}

	}

	else // use direct port IO
	{


		ow_reset(); //**
		if( ow_input_pin_state() ) { // only send if bus is "idle" = high
			ow_command( DS18X20_CONVERT_T, id );
			if (with_power_extern != DS18X20_POWER_EXTERN)
				ow_parasite_enable();
			return DS18X20_OK;
		}
		else {
#ifdef DS18X20_VERBOSE
			puts( "DS18X20_start_meas: Short Circuit!\r" );
#endif
			return DS18X20_START_FAIL;
		}
	}
}


uint8_t DS18X20_read_meas_word(uint8_t id[], uint16_t *out)
{
    uint8_t i;
    uint8_t sp[DS18X20_SP_SIZE];

    ow_reset(); //**
    
	if( onewire_2482_available )
	{

    
    dallasCommand(DS18X20_READ, id);
    for ( i=0 ; i< DS18X20_SP_SIZE; i++ )        sp[i]=ow_byte_rd();
    //for ( i=0 ; i< DS18X20_SP_SIZE; i++ ) sp[i]=dallasReadByte();

    if ( crc8( &sp[0], DS18X20_SP_SIZE ) )
        return DS18X20_ERROR_CRC;
    //DS18X20_meas_to_cel(id[0], sp, subzero, cel, cel_frac_bits);
    *out = DS18X20_meas_to_word( id[0], sp );
    return DS18X20_OK;
	}
	else
	{
		ow_command(DS18X20_READ, id);
	    for ( i=0 ; i< DS18X20_SP_SIZE; i++ )        sp[i]=ow_byte_rd();
	    //for ( i=0 ; i< DS18X20_SP_SIZE; i++ ) sp[i]=dallasReadByte();

	    if ( crc8( &sp[0], DS18X20_SP_SIZE ) )
	        return DS18X20_ERROR_CRC;
	    //DS18X20_meas_to_cel(id[0], sp, subzero, cel, cel_frac_bits);
	    *out = DS18X20_meas_to_word( id[0], sp );
	    return DS18X20_OK;
	}
}



