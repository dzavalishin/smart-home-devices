#include <string.h>

#include "onewire.h"
#include "ds18x20.h"
#include "uart.h"
#include "delay.h"
#define _delay_ms delay_ms
#include "util.h"
#include "temperature.h"

#include "modbus_reg.h"

#define DS_DEBUG 1

#ifdef OW_ONE_BUS
#	define N_1W_BUS 1
#else
#	define N_1W_BUS 8
#endif


#if DS_DEBUG
//#define uart_puts_P(c) debug_puts(c)
#else
#define debug_puts(c)
#endif

#if OW_SERIAL_ID
// This code works not only for temperature sensors, but for
// all 1-wire stuff also. For now it's only hardware serial
// number chip - DS2401.

uint8_t serialNumber [OW_ROMCODE_SIZE]; // = {0x55,0xAA};

#endif


uint8_t nTempSensors = 0;

static uint8_t search_sensors(uint8_t currBus);

#if N_TEMPERATURE_IN > 0
uint8_t gTempSensorIDs[N_TEMPERATURE_IN][OW_ROMCODE_SIZE];
uint8_t gTempSensorBus[N_TEMPERATURE_IN]; // Which bus this sensor lives on
uint8_t gTempSensorLogicalNumber[N_TEMPERATURE_IN]; // Report sensor with this logical number

uint16_t currTemperature[N_TEMPERATURE_IN];

uint16_t ow_error_cnt; // 1wire error counter
#endif


void init_temperature(void)
{
    nTempSensors = 0;

    uint8_t bus;
    for( bus = 0; bus < N_1W_BUS; bus++ )
    {
#ifndef OW_ONE_BUS
        ow_set_bus(&PINB,&PORTB,&DDRB,PB0+bus);
#endif
        //uint8_t sc =
        search_sensors(bus);
    }
}


// ---------------------------------------------------------------
// Find out what sensors do we have on a given bus
// ---------------------------------------------------------------


uint8_t search_sensors(uint8_t currBus)
{
//    uint8_t i;
    uint8_t id[OW_ROMCODE_SIZE];
    uint8_t diff;

    debug_puts( "?" );
    //	debug_puts( "Scan for DS18X20\n" );

    for( diff = OW_SEARCH_FIRST;
         (diff != OW_LAST_DEVICE) && (nTempSensors < N_TEMPERATURE_IN) ; )
    {
        DS18X20_find_sensor( &diff, &id[0] );

        if( diff == OW_PRESENCE_ERR )
        {
            //			debug_puts( "No Sensor\n" );
            debug_puts( "-" );
            break;
        }

        if( diff == OW_DATA_ERR )
        {
            REPORT_ERROR(ERR_FLAG_1WIRE_SCAN_FAULT);
            //			debug_puts( "Bus Error\n" );
            debug_puts( "B" );
            ow_error_cnt++;
            break;
        }

        //		debug_puts( "OK! got some!\n" );
        debug_puts( "+" );


#if OW_SERIAL_ID
        if(id[0] == DS2401_ID)
        {
            ow_copy_rom( serialNumber, id );
//            for( i=0; i < OW_ROMCODE_SIZE; i++ )
//                serialNumber[i]=id[i];
        }
        else
#endif
        {
#if N_TEMPERATURE_IN > 0
            ow_copy_rom( gTempSensorIDs[nTempSensors], id );
//            for( i=0; i < OW_ROMCODE_SIZE; i++)
//                gTempSensorIDs[nTempSensors][i]=id[i];

            gTempSensorBus[nTempSensors] = currBus;
#endif
            nTempSensors++;
        }

    }

    return nTempSensors;
}












#if N_TEMPERATURE_IN > 0

#define TEMP_FAST_RESTART 1


static void read_temperature_data(void);
static void request_temperature_measurement(void);

static uint8_t timerCallNumber = 0;


// ---------------------------------------------------------------
// Timer-driven temperature sensors scan loop
// ---------------------------------------------------------------


void temp_meter_05sec_timer(void)
{
    //flash_led_once();

    switch(timerCallNumber++)
    {
    case 0:             // Start measurements
    req:
        request_temperature_measurement();
        break;

    case 1:        	// 0.5 sec passed, do nothing - we need 0.75 sec
        break;

    case 2:        	// 1 sec passed, read measurements.

        timerCallNumber = 0;
        read_temperature_data();
#if TEMP_FAST_RESTART
        timerCallNumber = 1;
        goto req;
#endif
        break;
    }
}



// ---------------------------------------------------------------
// Broadcast request to start measurement
// ---------------------------------------------------------------


static void request_temperature_measurement(void)
{
    uint8_t bus;

    for( bus = 0; bus < N_1W_BUS; bus++ )
    {
#ifndef OW_ONE_BUS
        ow_set_bus(&PINB,&PORTB,&DDRB,PB0+bus);
#endif
        if ( DS18X20_start_meas( DS18X20_POWER_EXTERN, NULL ) != DS18X20_OK)
            //	        if ( DS18X20_start_meas( DS18X20_POWER_PARASITE, NULL ) != DS18X20_OK)
        {
            // Error starting temp mesaure.
            REPORT_ERROR(ERR_FLAG_1WIRE_READ_FAULT);
            ow_error_cnt++;
        }
    }
}


// ---------------------------------------------------------------
// Scan all sensors and read out last measurement
// ---------------------------------------------------------------


void read_temperature_data(void)
{
    uint8_t i;

    for ( i = 0; i < nTempSensors; i++ )
    {
        uint16_t out;

#ifndef OW_ONE_BUS
        ow_set_bus(&PINB,&PORTB,&DDRB,PB0+gTempSensorBus[i]);
#endif

        if ( DS18X20_read_meas_word(&gTempSensorIDs[i][0], &out) != DS18X20_OK )
        {
            REPORT_ERROR(ERR_FLAG_1WIRE_READ_FAULT);
            ow_error_cnt++;
            continue;
        }
        currTemperature[i] = out;
    }
}





#endif // N_TEMPERATURE_IN > 0

