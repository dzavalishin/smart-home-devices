/**
 *
 *	Modbus
 *
 *	Registers map.
 *
 **/

/*
 TODO check rx/tx reg count limits!
 */

#include <inttypes.h>

#include "defs.h"
#include "eeprom.h"
#include "uart.h"
#include "adc.h"

#include "modbus.h"
#include "modbus_reg.h"

#include "temperature.h"


//static uint8_t modbus_read_register( uint16_t nReg, uint16_t *val );

// return nReplyBytes
int modbus_read_regs( unsigned char *tx_buf, uint16_t startReg, uint16_t nReg )
{
    int i;

    if(nReg > MODBUS_MAX_TX_REG) return 0; // TODO move to pkt code


    for( i = startReg; i < (startReg+nReg); i++ )
    {
        uint16_t reg;

        if( !modbus_read_register( i, &reg ) )
            return 0;

        uint16_t pos = i - startReg;

        PUT_WORD( pos*2, reg, tx_buf );
    }

    return nReg*2;
}


// return modbus err no or 0
int modbus_write_regs( unsigned char *rx_buf, uint16_t startReg, uint16_t nReg )
{
    int i;

    //if(nReg > MODBUS_MAX_TX_REG) return MODBUS_EXCEPTION_ILLEGAL_DATA_ADDFRESS; // TODO move to pkt code

    for( i = startReg; i < (startReg+nReg); i++ )
    {
        uint16_t pos = i - startReg;
        uint16_t reg = GET_WORD(pos*2,rx_buf);

        int err = modbus_write_register( i, reg );
        if( err )
            return err;

    }

    return 0;
}

// return modbus err no or 0
int modbus_write_register( uint16_t nReg, uint16_t value )
{

    switch(nReg)
    {
    case MB_REG_SETUP_BUS_ADDR: modbus_our_address = value; save_eeprom_settings(); 		return 0;
    case MB_REG_SETUP_BUS_SPEED_LO: modbus_set_baud( value ); save_eeprom_settings();  		return 0;

    case MB_REG_FLAGS_ERROR: error_flags &= value; 						return 0;

    }

    return MODBUS_EXCEPTION_ILLEGAL_DATA_ADDFRESS;
}


#define INRANGE( _n, _start, _cnt ) ( ((_n) >= (_start)) && ( (_n) < ((_start)+(_cnt)) ) )

uint8_t modbus_read_register( uint16_t nReg, uint16_t *val )
{
    if( INRANGE( nReg, MB_REG_TEMP, MB_COUNT_TEMP ) )
    {
        int id = nReg-MB_REG_TEMP;
        int pos = find_pos_by_persistent_id( id );

        if( (pos < 0) || (pos >= nTempSensors) )
            *val = 0xFFFF;
        else
            *val = currTemperature[pos];
        return 1;
    }

    if( INRANGE( nReg, MB_REG_TEMP_DIRECT, MB_COUNT_TEMP_DIRECT ) )
    {
        int id = nReg-MB_REG_TEMP_DIRECT;

        if( (id < 0) || (id >= nTempSensors) )
            *val = 0xFFFF;
        else
            *val = currTemperature[id];
        return 1;
    }

#if SERVANT_NADC > 0
    if( INRANGE( nReg, MB_REG_AI, MB_COUNT_AI ) )
    {
        int id = nReg-MB_REG_AI;
        if( id >= SERVANT_NADC ) return 0;
        *val = adc_value[id];
        return 1;
    }
#endif
#if OW_SERIAL_ID
    if( INRANGE( nReg, MB_REG_ROMID, MB_COUNT_ROMID ) )
    {
        *val = *((uint16_t*)serialNumber+nReg-MB_REG_ROM);
        return 1;
    }
#endif

    switch(nReg)
    {
    case MB_REG_SETUP_BUS_ADDR: *val = modbus_our_address; break;
    case MB_REG_SETUP_BUS_SPEED_LO: *val = uart_speed; break;
    case MB_REG_SETUP_BUS_SPEED_HI: *val = 0; break;
    case MB_REG_SETUP_BUS_SPEED_ID: *val = 0; break;
    case MB_REG_SETUP_BUS_MODE: *val = 0; break;

    case MB_REG_FLAGS_ERROR: *val = error_flags; break;


    case MB_REG_ID+0:
    case MB_REG_ID+1: *val = 'DZ'; break;
    case MB_REG_ID+2: *val = '1W'; break;
    case MB_REG_ID+3: *val = 'T8'; break;



    case MB_REG_HWCONF_DI: *val = SERVANT_NDI; break;
    case MB_REG_HWCONF_DO: *val = 0; break;
    case MB_REG_HWCONF_AI: *val = SERVANT_NADC; break;
    case MB_REG_HWCONF_AO: *val = SERVANT_NPWM; break;
    case MB_REG_HWCONF_CN: *val = SERVANT_NCNT; break;
    case MB_REG_HWCONF_TS: *val = N_TEMPERATURE_IN; break;
    case MB_REG_HWCONF_TA: *val = nTempSensors; break;



    case MB_REG_COUNTER_IO: *val = modbus_event_cnt; break;
    case MB_REG_COUNTER_CRC: *val = modbus_crc_cnt; break;
    case MB_REG_COUNTER_EXC: *val = modbus_exceptions_cnt; break;
    case MB_REG_COUNTER_1WE: *val =  ow_error_cnt; break;



    default:
        return 0;
    }

    return 1;
}

