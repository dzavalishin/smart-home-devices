/**
 *
 * Pump controller
 *
 * Copyright (C) 2016 Dmitry Zavalishin, dz@dz.ru
 *
 * Actual pump control code.
 * Runs in a main loop..
 *
**/

#include "defs.h"
#include "adc.h"

#include <avr/io.h>

#include "pump.h"
#include "errlog.h"


long    uptime = 0;

int     pump_work_seconds;      // Total seconds pump works

char    system_failed = 0;

char    active_pump = 0;        // 0 = main
char    pump_state = 0;         // on?

char 	have_below = 0;
char 	have_above = 0;

// TODO all sensors disabled - use Di? external relay?


struct sensor   sens[N_SENSORS] =
{
    { .adc_channel = 0, .conf.active = 1 },
    { .adc_channel = 1, .conf.active = 1 },
    { .adc_channel = 2, .conf.active = 0 },
    { .adc_channel = 3, .conf.active = 0 },
}
;



static void read_and_convert( struct sensor *in );
static void set_pump_state( char state );
static void pump_reset(void);
static void sanity_checks( void );
static void process_din( void );


void
pump_control(void)
{
    char nActive = 0;

    char i;

    for( i = 0; i < N_SENSORS; i++ )
    {
        read_and_convert( sens+i );
    }

    for( i = 0; i < N_SENSORS; i++ )
    {
        struct sensor *in = sens+i;

        if( !in->conf.active )
            continue;

        nActive++;

        if( in->is_above ) have_above = 1;
        if( in->is_below ) have_below = 1;
    }

    if( !nActive )
        process_din();
    else
    {
        if( have_above ) set_pump_state( 0 );
        else if( have_below ) set_pump_state( 1 );
    }

    sanity_checks();
}


// No analog sensors active, use din - traditional binary pressure sensor
static void process_din( void )
{
    char in = ! (PINB & _BV(PB1)); // active low

    // Sanity checks use it
    have_below = in;
    have_above = !have_below;

    set_pump_state( in );
}



static void read_and_convert( struct sensor *in )
{
    if( (in->adc_channel < 0 ) || (in->adc_channel > SERVANT_NADC) )
    {
        in->out_value = -1;
        return;
    }

    if( !in->conf.active )
        return;

    in->in_value = adc_value[ (int) in->adc_channel ];

    double v = in->in_value - in->conf.convert_in_L;

    v /= (in->conf.convert_in_H - in->conf.convert_in_L);
    v *= (in->conf.convert_out_H - in->conf.convert_out_L);

    v += in->conf.convert_out_L;

    in->out_value = (int) v;

    if( in->out_value > in->max )
        in->max = in->out_value;


    in->is_below = in->is_above = 0;

    if( in->out_value >= in->conf.H_level )
        in->is_above = 1;

    if( in->out_value <= in->conf.L_level )
        in->is_below = 1;

}


static void pump_onoff( char pump, char state )
{
    if( pump )
    {
        // spare one
        if(state) 	PORTB |= _BV(PB3);
        else 		PORTB &= ~_BV(PB3);
    }
    else
    {
        // main one
        if(state) 	PORTB |= _BV(PB2);
        else 		PORTB &= ~_BV(PB2);
    }
}

static void set_pump_state( char state )
{
    /*
    static char prev_state = 0;

    if( prev_state != state )
    {
        prev_state = state;
        if( state )

    }
    */

    pump_state = state;

    if( !state )
        pump_work_seconds = 0;

    pump_onoff( !active_pump, 0 ); // turn off unused one
    pump_onoff( active_pump, state && (!system_failed) );
}


// -----------------------------------------------------------------------
// Timers
// -----------------------------------------------------------------------

// 12Hr
#define BIG_RETRY_TIME (12L*3600L)

static long     big_retry_timer = 0;


void pump_every_second(void)
{
    uptime++;

    if( pump_state )
        pump_work_seconds++;

    big_retry_timer++;

    if( big_retry_timer > BIG_RETRY_TIME )
    {
        big_retry_timer = 0;

        // Attemp all over again
        pump_reset_all();
    }
}


// -----------------------------------------------------------------------
// Pump failure - use spare one or fail at all
// -----------------------------------------------------------------------

static void process_pump_failure(void)
{
    if( active_pump )
        system_failed = 1;
    else
        active_pump = 1;

    pump_reset();
}

// -----------------------------------------------------------------------
// Reset state
// -----------------------------------------------------------------------

static void pump_reset(void)
{
    pump_work_seconds = 0;

    set_pump_state( 0 ); // for any case
}

void pump_reset_all(void)
{
    system_failed = 0;
    active_pump = 0;

    pump_reset();

    log_event( LOG_ID_RETRY );
}


// -----------------------------------------------------------------------
// Sanity checks
// -----------------------------------------------------------------------



// No pressure working for some time
#define SANITY_PRESSURE_TIME_SEC        20

// Work too long in one run, 10 min
#define SANITY_LONG_RUN_TIME_SEC        (10*60)

// On system start give pump some more time to fill all the pipes
#define SANITY_STARTUP_TIME             (5*60)

static void sanity_checks( void )
{
    // we pump some time and some sensors still show pressure is below lower level

    unsigned check_seconds = pump_work_seconds;

    // Suppose secondary pump is twice as bad as main, give it twice more time
    if( active_pump )
        check_seconds /= 2;

    if( (check_seconds > SANITY_PRESSURE_TIME_SEC) && have_below && (uptime > SANITY_STARTUP_TIME) )
    {
        log_event( LOG_ID_NO_PRESSURE );
        process_pump_failure();
    }

    if( check_seconds > SANITY_LONG_RUN_TIME_SEC )
    {
        log_event( LOG_ID_LONG_RUN );
        process_pump_failure();
    }





}











