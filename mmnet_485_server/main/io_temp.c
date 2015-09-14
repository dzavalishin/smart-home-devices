/**
 *
 * DZ-MMNET-MODBUS: Modbus/TCP I/O module based on MMNet101.
 *
 * 1-Wire temperature sensors.
 *
**/

#include "defs.h"
#include "runtime_cfg.h"

#include <inttypes.h>
#include <stdio.h>

#include "io_temp.h"
//#include <dev/owibus.h>

// for avrportg
//#include <dev/gpio.h>
//#include <cfg/arch.h>

#include "ds18x20.h"



#define debug_puts(c)


#if SERVANT_1WMAC
uint8_t serialNumber [OW_ROMCODE_SIZE];
#endif

#if SERVANT_NTEMP > 0
uint8_t nTempSensors = 0;

uint8_t gTempSensorIDs[SERVANT_NTEMP][OW_ROMCODE_SIZE];
#ifndef OW_ONE_BUS
uint8_t gTempSensorBus[SERVANT_NTEMP]; // Which bus this sensor lives on
#endif
uint8_t gTempSensorLogicalNumber[SERVANT_NTEMP]; // Report sensor with this logical number

uint16_t currTemperature[SERVANT_NTEMP];

uint16_t ow_error_cnt; // 1wire error counter

#ifndef OW_ONE_BUS
uint8_t ow_bus_error_cnt[N_1W_BUS]; // 1wire error counter per bus
#endif

#endif


#if ENABLE_1WIRE

void count_1w_bus_error( uint8_t bus );


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
         (diff != OW_LAST_DEVICE) && (nTempSensors < SERVANT_NTEMP) ; )
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
            //ow_error_cnt++;
            //ow_bus_error_cnt[currBus];
            count_1w_bus_error( currBus );
            break;
        }

        //		debug_puts( "OK! got some!\n" );
        debug_puts( "+" );

        onewire_available = 1;

#if SERVANT_1WMAC
        if(id[0] == DS2401_ID)
        {
            ow_copy_rom( serialNumber, id );
        }
        else
#endif
        {
#if SERVANT_NTEMP > 0
            ow_copy_rom( gTempSensorIDs[nTempSensors], id );
#ifndef OW_ONE_BUS
            gTempSensorBus[nTempSensors] = currBus;
#endif
#endif
            nTempSensors++;
        }

    }

    return nTempSensors;
}












#if SERVANT_NTEMP > 0

#define TEMP_FAST_RESTART 1


static void read_temperature_data(void);
static void request_temperature_measurement(void);

static uint8_t timerCallNumber = 0;


// ---------------------------------------------------------------
// Timer-driven temperature sensors scan loop
// ---------------------------------------------------------------


void temp_meter_measure(void)
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

        read_temperature_data();
#if TEMP_FAST_RESTART
        timerCallNumber = 1;
        goto req;
#else
        timerCallNumber = 0;
        break;
#endif
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
        // if( DS18X20_start_meas( DS18X20_POWER_PARASITE, NULL ) != DS18X20_OK)
        if( DS18X20_start_meas( DS18X20_POWER_EXTERN, NULL ) != DS18X20_OK)
        {
            // Error starting temp mesaure.
            REPORT_ERROR(ERR_FLAG_1WIRE_START_FAULT);
            //ow_error_cnt++;
            //ow_bus_error_cnt[bus];
            count_1w_bus_error( bus );
            //led1_timed( 200 );
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
            //ow_error_cnt++;
            //ow_bus_error_cnt[i]++;
            count_1w_bus_error( i );
            continue;
        }
        //led2_timed( 15 );
        currTemperature[i] = out;
    }
}


void count_1w_bus_error( uint8_t bus )
{
    ow_error_cnt++;
#ifndef OW_ONE_BUS
    if( bus < N_1W_BUS )
        ow_bus_error_cnt[bus]++;
#endif
    //led1_timed( 110 );
    //led2_timed( 110 );
}


#endif // SERVANT_NTEMP > 0



const char *temptoa( uint16_t t, char *out )
{
    // t has 4 fraction bits

    char *frac = "0";

    switch( (t >> 2) & 3 )
    {
    case 0: frac = "00"; break;
    case 1: frac = "25"; break;
    case 2: frac = "50"; break;
    case 3: frac = "75"; break;
    }

    sprintf( out, "%d.%s", t >> 4, frac );

    return out;
}
























#endif // ENABLE_1WIRE
