#include <avr/io.h>

#include "ds18x20.h"
#include "onewire.h"
#include "crc8.h"


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
            id[0] == DS2401_ID
           ) return;
    }
}





/* start measurement (CONVERT_T) for all sensors if input id==NULL
 or for single sensor. then id is the rom-code */
uint8_t DS18X20_start_meas( uint8_t with_power_extern, uint8_t id[])
{
    ow_reset(); //**
    if( ow_input_pin_state() ) { // only send if bus is "idle" = high
        ow_command( DS18X20_CONVERT_T, id );
        if (with_power_extern != DS18X20_POWER_EXTERN)
            ow_parasite_enable();
        //led1_timed( 155 );
        return DS18X20_OK;
    }
    else {
        //led2_timed( 55 );
#ifdef DS18X20_VERBOSE
        uart_puts_P( "DS18X20_start_meas: Short Circuit !\r" );
#endif
        return DS18X20_START_FAIL;
    }
}



uint8_t DS18X20_read_meas_word(uint8_t id[], uint16_t *out)
{
    uint8_t i;
    uint8_t sp[DS18X20_SP_SIZE];

    ow_reset(); //**
    ow_command(DS18X20_READ, id);

    for ( i=0 ; i< DS18X20_SP_SIZE; i++ ) sp[i]=ow_byte_rd();

    if ( crc8( &sp[0], DS18X20_SP_SIZE ) )
        return DS18X20_ERROR_CRC;

    *out = DS18X20_meas_to_word( id[0], sp );
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
	        			// Extended measurements for DS18S20 contributed by Carsten Foss
        meas &= (uint16_t) 0xfffe;	// Discard LSB , needed for later extended precicion calc
        meas <<= 3;			// Convert to 12-bit , now degrees are in 1/16 degrees units
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
    if ( fc == DS18B20_ID )
    { // check resolution 18B20
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
