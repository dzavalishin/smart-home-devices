/**
 *
 * DZ-MMNET-MODBUS: Modbus/TCP I/O module based on MMNet101.
 *
 * 1-Wire temperature sensors.
 *
**/

#define DEBUG 0

#include "defs.h"
#include "servant.h"
#include "runtime_cfg.h"

#include <inttypes.h>
#include <stdio.h>

#include "io_temp.h"
//#include <dev/owibus.h>

// for avrportg
//#include <dev/gpio.h>
//#include <cfg/arch.h>

#include "ds18x20.h"

#include "dev_map.h"




//#define debug_puts(c)


#if SERVANT_1WMAC
uint8_t 	serialNumber [OW_ROMCODE_SIZE];
#endif

#if SERVANT_NTEMP > 0
uint8_t 	nTempSensors = 0;

uint8_t 	gTempSensorIDs[SERVANT_NTEMP][OW_ROMCODE_SIZE];
#if !OW_ONE_BUS
uint8_t 	gTempSensorBus[SERVANT_NTEMP]; // Which bus this sensor lives on
#endif
//uint8_t 	gTempSensorLogicalNumber[SERVANT_NTEMP]; // Report sensor with this logical number

uint16_t 	currTemperature[SERVANT_NTEMP];

uint16_t 	ow_error_cnt; // 1wire error counter

#if !OW_ONE_BUS
uint8_t 	ow_bus_error_cnt[N_1W_BUS]; // 1wire error counter per bus
#endif

#endif


#if ENABLE_1WIRE

void count_1w_bus_error( uint8_t bus );
static void clear_temperature_data(void);

#if !OW_ONE_BUS
static void select_1w_bus( uint8_t bus );
#endif




static void init_temperature(void)
{
    nTempSensors = 0;

    clear_temperature_data();

    if( !(RT_IO_ENABLED(IO_1W1)|RT_IO_ENABLED(IO_1W8)) )
        return;
#if B1W_NON_FIXED_PORT

    // Bus 0 - usual PG4 bus, 1-7 - buses on PB

#if OW_ONE_BUS
    //uint8_t sc =
    search_sensors(0);
#else
    uint8_t bus;
    for( bus = 0; bus < N_1W_BUS; bus++ )
    {
        select_1w_bus( bus );
        //uint8_t sc =
        search_sensors(bus);

        if( !RT_IO_ENABLED(IO_1W8) )
            break;
    }
#endif


#else
    // TODO choose which buses to init

    uint8_t bus;
    for( bus = 0; bus < N_1W_BUS; bus++ )
    {
#if !OW_ONE_BUS
        ow_set_bus(&PIND,&PORTD,&DDRD,PD0+bus);
#endif
        //uint8_t sc =
        search_sensors(bus);
    }
#endif // B1W_NON_FIXED_PORT
}


static void rescan_temperature(void)
{
    //if( !(RT_IO_ENABLED(IO_1W1)|RT_IO_ENABLED(IO_1W8)) )        return;
    init_temperature();
}




// ---------------------------------------------------------------
// Find out what sensors do we have on a given bus
// ---------------------------------------------------------------


uint8_t search_sensors(uint8_t currBus)
{
//    uint8_t i;
    uint8_t id[OW_ROMCODE_SIZE];
    uint8_t diff;

    DPUTS( "?" );
    //	DPUTS( "Scan for DS18X20\n" );

    for( diff = OW_SEARCH_FIRST;
         (diff != OW_LAST_DEVICE) && (nTempSensors < SERVANT_NTEMP) ; )
    {
        DS18X20_find_sensor( &diff, &id[0] );

        if( diff == OW_PRESENCE_ERR )
        {
            //			DPUTS( "No Sensor\n" );
            DPUTS( "-" );
            break;
        }

        if( diff == OW_DATA_ERR )
        {
            REPORT_ERROR(ERR_FLAG_1WIRE_SCAN_FAULT);
            //			DPUTS( "Bus Error\n" );
            DPUTS( "B" );
            //ow_error_cnt++;
            //ow_bus_error_cnt[currBus];
            count_1w_bus_error( currBus );
            break;
        }

        //		DPUTS( "OK! got some!\n" );
        DPUTS( "+" );

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
            ow_map_add_found(id, nTempSensors); // make map of permanent index to runtime array pos
#if !OW_ONE_BUS
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



// ---------------------------------------------------------------
// Timer-driven temperature sensors scan loop
// ---------------------------------------------------------------

#if 1
// Called from main loop once a second
static void temp_meter_measure(void)
{
    if( !(RT_IO_ENABLED(IO_1W1)|RT_IO_ENABLED(IO_1W8)) )
        return;

    // Read data from the previous cycle
    read_temperature_data();
    // Request next measurement;
    request_temperature_measurement();
}
#else

static uint8_t timerCallNumber = 0;

void temp_meter_measure(void)
{
    if( !(RT_IO_ENABLED(IO_1W1)|RT_IO_ENABLED(IO_1W8)) )
        return;


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
#endif



// ---------------------------------------------------------------
// Broadcast request to start measurement
// ---------------------------------------------------------------


static void request_temperature_measurement(void)
{
#if !OW_ONE_BUS
    uint8_t bus;
    for( bus = 0; bus < N_1W_BUS; bus++ )
    {
        select_1w_bus( bus );
#endif
        // if( DS18X20_start_meas( DS18X20_POWER_PARASITE, NULL ) != DS18X20_OK)
        if( DS18X20_start_meas( DS18X20_POWER_EXTERN, NULL ) != DS18X20_OK)
        {
            // Error starting temp mesaure.
            REPORT_ERROR(ERR_FLAG_1WIRE_START_FAULT);
            //ow_error_cnt++;
            //ow_bus_error_cnt[bus];
#if !OW_ONE_BUS
            count_1w_bus_error( bus );
#else
            count_1w_bus_error( 0 );
#endif
            //led1_timed( 200 );
        }
#if !OW_ONE_BUS
    }
#endif
}


// ---------------------------------------------------------------
// Scan all sensors and read out last measurement
// ---------------------------------------------------------------


void read_temperature_data(void)
{
    uint8_t i;

    for( i = 0; i < nTempSensors; i++ )
    {
        uint16_t out;

        /* can't happen
        if( ow_is_empty_rom( &gTempSensorIDs[i][0] ) )
        {
            printf("emptry ROM");
            currTemperature[i] = ERROR_VALUE_16;
            continue;
        }
        */
#if !OW_ONE_BUS
        if( (!RT_IO_ENABLED(IO_1W8)) && (gTempSensorBus[i] != 0) )
            continue;


        select_1w_bus( gTempSensorBus[i] );
        //ow_set_bus(&PINB,&PORTB,&DDRB,PB0+gTempSensorBus[i]);
#endif

        if( DS18X20_read_meas_word(&gTempSensorIDs[i][0], &out) != DS18X20_OK )
        {
            REPORT_ERROR(ERR_FLAG_1WIRE_READ_FAULT);
            //ow_error_cnt++;
            //ow_bus_error_cnt[i]++;
            count_1w_bus_error( i );

            currTemperature[i] = ERROR_VALUE_16;

            continue;
        }

        currTemperature[i] = out;
    }
}


void count_1w_bus_error( uint8_t bus )
{
    ow_error_cnt++;
#if !OW_ONE_BUS
    char s[] = "err bus _\n";
    s[8] = '0' + bus;
    DPUTS( s );

    if( bus < N_1W_BUS )
        ow_bus_error_cnt[bus]++;
#endif
    //led1_timed( 110 );
    //led2_timed( 110 );
}


#if !OW_ONE_BUS

#if B1W_NON_FIXED_PORT
static void select_1w_bus( uint8_t bus )
{
#if !OW_ONE_BUS
    char s[] = "sel bus _\n";
    s[8] = '0' + bus;
    DPUTS( s );

    if( bus == 0 )
        ow_set_bus(&PING,&PORTG,&DDRG,OW_DEFAULT_PIN);
    else
        ow_set_bus(&PIND,&PORTD,&DDRD,PD0+bus+1); // bus starts from 1, pin from 2
#endif
}
#else
static void select_1w_bus( uint8_t bus ) // unused?
{
#if !OW_ONE_BUS
    ow_set_bus(&PIND,&PORTD,&DDRD,PD0+bus);
#endif
}
#endif

#endif // !OW_ONE_BUS


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


static void clear_temperature_data(void)
{
    uint8_t i;

    for ( i = 0; i < SERVANT_NTEMP; i++ )
        currTemperature[i] = ERROR_VALUE_16;
}

































// ----------------------------------------------------------------------
// Report status / set value
// ----------------------------------------------------------------------


static int8_t
temp_to_string( struct dev_minor *sub, char *out, uint8_t out_size )
{
    if( out_size < 20 ) return -1;

    if( sub->number >= nTempSensors ) return -1;

    temptoa( currTemperature[ sub->number ], out);

    return 0;
}


// ----------------------------------------------------------------------
// Once a second
// ----------------------------------------------------------------------


static void temp_timer( dev_major* d )
{
    static volatile uint8_t temperatureRescanCnt = 0;

    (void) d;

    temp_meter_measure();

    // TODO FIXME hangs
    if( temperatureRescanCnt-- <= 0 )
    {
//        rescan_temperature();
        temperatureRescanCnt = TEMPERATURE_RESCAN_SEC;
    }

}

// ----------------------------------------------------------------------
// Init/start
// ----------------------------------------------------------------------


static int8_t temp_init( dev_major* d )
{
    uint8_t i;

    //if( !RT_IO_ENABLED(IO_ADC) )        return;

    if( init_subdev( d, SERVANT_NTEMP, "temp" ) )
        return -1;

    // TODO general func
    for( i = 0; i < d->minor_count; i++ )
    {
        dev_minor *m = d->subdev + i;

        m->to_string = temp_to_string;
        //m->from_string = pwm_from_string;
    }

    init_temperature();

    return 0;
}



static int8_t temp_start( dev_major* d )
{

    return 0;
}


// ----------------------------------------------------------------------
// General IO definition
// ----------------------------------------------------------------------




dev_major io_temp =
{
    .name = "temp",

#if SERVANT_NTEMP > 0
    .init	= temp_init,
    .start	= temp_start,
//    .stop	= temp_stop,
    .timer 	= temp_timer,
#endif // ENABLE_SPI

//    .to_string = temp_to_string,
    .from_string = 0,

    .minor_count = SERVANT_NTEMP,
    .subdev = 0,
};

















#endif // ENABLE_1WIRE
