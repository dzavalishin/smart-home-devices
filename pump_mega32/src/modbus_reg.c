/**
 *
 *	Modbus
 *
 *	Registers map.
 *
 **/

/*
 TODO check rx/tx reg count limits!

 TODO modbus_read_register return errno!

 */

#include <inttypes.h>

#include "defs.h"
#include "eeprom.h"
#include "uart.h"
#include "adc.h"

#include "modbus.h"
#include "modbus_reg.h"

#include "temperature.h"



static void send_descriptor( uint16_t *val, uint8_t displ, uint8_t pos, uint8_t persId, void *romBase );



static char *device_id0 = DEVICE_ID0;
static char *device_id1 = DEVICE_ID1;
static char *device_id2 = DEVICE_ID2;



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



#define INRANGE( _n, _start, _cnt ) ( ((_n) >= (_start)) && ( (_n) < ((_start)+(_cnt)) ) )

#define BEGIN_RANGE( _id, _n, _start, _cnt ) \
    if( INRANGE( _n, _start, _cnt ) ) \
    { uint16_t _id = (_n) - (_start);

#define END_R_RANGE() return 1; }
#define END_W_RANGE() return 0; }



// return modbus err no or 0
int modbus_write_register( uint16_t nReg, uint16_t value )
{

    switch(nReg)
    {
    case MB_REG_SETUP_BUS_ADDR: modbus_our_address = value; save_eeprom_settings(); 		return 0;
    case MB_REG_SETUP_BUS_SPEED_LO: modbus_set_baud( value ); save_eeprom_settings();  		return 0;

    case MB_REG_FLAGS_ERROR: error_flags &= value; pump_reset_all();   				return 0;

    BEGIN_RANGE( id, nReg, MB_REG_SETUP_CONV, MB_COUNT_SETUP_CONV )
    	//= value
    END_W_RANGE()

    BEGIN_RANGE( id, nReg, MB_REG_SETUP_TRIG, MB_COUNT_SETUP_TRIG )
    	//= value
    END_W_RANGE()

    }

    return MODBUS_EXCEPTION_ILLEGAL_DATA_ADDFRESS;
}




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
        *val = *((uint16_t*)serialNumber+nReg-MB_REG_ROMID);
        return 1;
    }
#endif

#ifndef OW_ONE_BUS
    if( INRANGE( nReg, MB_REG_COUNTERS_1WBUS, MB_COUNT_COUNTERS_1WBUS ) )
    {
        uint8_t id = nReg-MB_REG_COUNTERS_1WBUS;
        *val = ow_bus_error_cnt[id];
        return 1;
    }
#endif


    // EEPROM/persistent id order
    BEGIN_RANGE( id, nReg, MB_REG_ROM, MB_COUNT_ROM )

        uint16_t nRec = id / (MB_REG_ROM_RECSIZE/2);
        uint8_t displ = id % (MB_REG_ROM_RECSIZE/2);


        uint8_t record[EEPROM_MAP_REC_SZ];
        temp_sens_read_record( nRec, record );

        uint8_t persId = record[SENS_ID_BYTE];
        int8_t pos = find_pos_by_persistent_id( persId );

        send_descriptor( val, displ, pos, persId, record );

    END_R_RANGE()



    // ram/sequential id order
    BEGIN_RANGE( id, nReg, MB_REG_MAP, MB_COUNT_MAP )

        uint16_t nRec = id / (MB_REG_ROM_RECSIZE/2);
        uint8_t displ = id % (MB_REG_ROM_RECSIZE/2);

        int8_t pos = nRec;

        if( pos >= N_TEMPERATURE_IN )
            return 0;

        uint8_t persId = gTempSensorLogicalNumber[pos];

        send_descriptor( val, displ, pos, persId, (gTempSensorIDs[pos]) + 1 );

    END_R_RANGE()

    BEGIN_RANGE( id, nReg, MB_REG_SETUP_CONV, MB_COUNT_SETUP_CONV )
        *val = 0; //currTemperature[id];
    END_R_RANGE()

    BEGIN_RANGE( id, nReg, MB_REG_SETUP_TRIG, MB_COUNT_SETUP_TRIG )
        *val = 0; //currTemperature[id];
    END_R_RANGE()


    switch(nReg)
    {
    case MB_REG_SETUP_BUS_ADDR: *val = modbus_our_address; break;
    case MB_REG_SETUP_BUS_SPEED_LO: *val = uart_speed; break;
    case MB_REG_SETUP_BUS_SPEED_HI: *val = 0; break;
    case MB_REG_SETUP_BUS_SPEED_ID: *val = 0; break;
    case MB_REG_SETUP_BUS_MODE: *val = 0; break;

    case MB_REG_FLAGS_ERROR: *val = error_flags; break;


//    case MB_REG_ID+0:
//    case MB_REG_ID+1: *val = 'DZ'; break;
//    case MB_REG_ID+2: *val = '1W'; break;
//    case MB_REG_ID+3: *val = 'T8'; break;

    case MB_REG_ID+0:
    case MB_REG_ID+1: *val = *((uint16_t *)device_id0) /* 'DZ' */; break;
    case MB_REG_ID+2: *val = *((uint16_t *)device_id1); break;
    case MB_REG_ID+3: *val = *((uint16_t *)device_id2); break;



    case MB_REG_HWCONF_DI: *val = 8; break; // one 8 bit reg
    case MB_REG_HWCONF_DO: *val = 8; break;
    case MB_REG_HWCONF_AI: *val = SERVANT_NADC; break;
    case MB_REG_HWCONF_AO: *val = SERVANT_NPWM; break;
    case MB_REG_HWCONF_CN: *val = SERVANT_NCNT; break;
    case MB_REG_HWCONF_TS: *val = N_TEMPERATURE_IN; break;
    case MB_REG_HWCONF_TA: *val = nTempSensors; break;



    case MB_REG_COUNTER_IO: *val = modbus_event_cnt; break;
    case MB_REG_COUNTER_CRC: *val = modbus_crc_cnt; break;
    case MB_REG_COUNTER_EXC: *val = modbus_exceptions_cnt; break;
    case MB_REG_COUNTER_1WE: *val =  ow_error_cnt; break;

    case MB_REG_IO+0: *val = PINB; break;       // All dig ins
    case MB_REG_IO+1: *val = PORTB; break;      // All dig outs read


    default:
        return 0;
    }

    return 1;
}






/** -
 *
 *  64 x record:
 *   6 bytes 1wire ROM id
 *   2 bytes persistent id
 *   2 bytes current internal id (search order) or -1 if non-active, NO WR
 *   2 bytes bus number or -1, NO WR
 *   4 bytes pad
 *
**/

static void send_descriptor( uint16_t *val, uint8_t displ, uint8_t pos, uint8_t persId, void *romBase )
{
    switch( displ )
    {
    case 3: *val = persId; break;
    case 4: *val = pos; break;
    case 5:
        {
            if( pos < N_TEMPERATURE_IN )
                *val = gTempSensorBus[pos];
            else
                *val = -1;
        }
        break;

    case 6: *val = currTemperature[pos]; break;

    default:
        if( (displ < 3) && (pos < N_TEMPERATURE_IN) )
            *val = *((uint16_t *)(romBase+displ*2));
        else
            *val = 0;
        break;
    }
}















