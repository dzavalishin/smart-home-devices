/**
 *
 * DZ-MMNET-CHARGER: Modbus/TCP I/O module based on MMNet101.
 *
 * Charger state machine.
 *
**/

// TODO detect insanity, shutdown
// TODO disconnect self on battery very low

#include "defs.h"
#include "servant.h"


#include <inttypes.h>
#include <stdio.h>

#include "io_adc.h"
#include "io_pwm.h"
#include "io_spi.h"

#include <sys/timer.h>


#define V_DIODE_DROP 0.7
#define R_CURRENT_SENSE 0.1

#define ADC_DIVIDER 20.0

typedef enum ch_state
{
    ch_init,		// starting state - find out and switch to needed state
    ch_charge,    	// 
    ch_discharge,    	//
    ch_idle,    	// Acc is charged, no current flow
    ch_low_charge,    	// Acc charge is very low, load is disconnected

    ch_failed,          // Something wen completely wrong, all shut down - TODO

} ch_state;

typedef struct charger
{
    ch_state            state;

    float               low_charge_voltage;  // minimal charge acc voltage
    float               max_charge_voltage;  // maximal charge acc voltage

    float               min_charge_current;
    float               max_charge_current;

    void                (*ch_input)( struct charger *c );
    void                (*ch_output)( struct charger *c );

    // Voltage circuit:
    // mains -> diode -> PWM MOSFET -> load <-> Isense resistor <-> acc
    // need diode to be able to check for existance of mains voltage

    float               mains_voltage;  // incoming voltage from AC/DC unit
    float               load_voltage;   // voltage on output terminals
    float               acc_voltage;    // voltage on accumuator

    float               charge_current;  // maximal charge acc voltage

    uint8_t             charge_pwm_percentage; // 0-255 % of charger PWM

    uint8_t             load_enabled; // load connected (relay)

    uint8_t             red_led, green_led;
    uint8_t             button;

} charger;

typedef uint8_t bool;
//#define bool


static void     ch_display( charger *c );
static void     ch_dump( charger *c );



// -----------------------------------------------------------------------
// State switches
// -----------------------------------------------------------------------

//static inline void ch_st_to_init( charger *c ) { c->state = ch_init; }

// -----------------------------------------------------------------------
// Read and calc voltages and currents
// -----------------------------------------------------------------------

static inline void
ch_get_data( charger *c )
{
    c->ch_input( c );

    c->charge_current = (c->load_voltage - c->acc_voltage) / R_CURRENT_SENSE;
}

static void ch_input_12v( struct charger *c )
{
    c->mains_voltage = get_float_adc( 0 ) / ADC_DIVIDER;
    c->load_voltage  = get_float_adc( 1 ) / ADC_DIVIDER;
    c->acc_voltage   = get_float_adc( 2 ) / ADC_DIVIDER;

    c->button = !! (spi_di & 0x10); // TODO real bit?

}

static void ch_input_24v( struct charger *c )
{
    c->mains_voltage = get_float_adc( 4 ) / ADC_DIVIDER;
    c->load_voltage  = get_float_adc( 5 ) / ADC_DIVIDER;
    c->acc_voltage   = get_float_adc( 6 ) / ADC_DIVIDER;

    c->button = !! (spi_di & 0x20); // TODO real bit?
}

// -----------------------------------------------------------------------
// Control outputs
// -----------------------------------------------------------------------


static inline void
ch_control_outs( charger *c )
{
    c->ch_output( c );
}


static void ch_output_12v( struct charger *c )
{
    set_an( 0, c->charge_pwm_percentage );

    // TODO relay control - c->load_enabled
    // TODO LEDs
}

static void ch_output_24v( struct charger *c )
{
    set_an( 1, c->charge_pwm_percentage );

    // TODO relay control - c->load_enabled
    // TODO LEDs

}



// -----------------------------------------------------------------------
// Queries
// -----------------------------------------------------------------------

static inline bool
ch_have_mains_power( charger *c )
{
    return c->mains_voltage > c->low_charge_voltage + V_DIODE_DROP;
}

static inline bool
ch_acc_low( charger *c )
{
    return c->acc_voltage < c->low_charge_voltage;
}

static inline bool
ch_acc_hi( charger *c )
{
    return c->acc_voltage > c->max_charge_voltage;
}

static inline bool
ch_acc_50_percent( charger *c )
{
    return c->acc_voltage < (c->max_charge_voltage + c->low_charge_voltage) / 2;
}


// -----------------------------------------------------------------------
// Charge current control
// -----------------------------------------------------------------------

void
ch_set_charge_current( charger *c )
{
    // Not charging at all
    if( (c->state == ch_discharge) || (c->state == ch_low_charge) || (c->state == ch_init) || (c->state == ch_failed) )
    {
        c->charge_pwm_percentage = 0;
        return;
    }

    // overvoltage safety first
    if( c->load_voltage > (c->max_charge_voltage + V_DIODE_DROP) )
    {
        c->charge_pwm_percentage--;
        return;
    }
/*
    // Acc charge is ok
    if( c->acc_voltage > max_charge_voltage )
    {
        c->charge_pwm_percentage--;
        return;
    }
*/
    if( c->state == ch_idle )
    {
        // In idle state we keep acc voltage on max+0.1 to max-0.1 volt level
    as_idle:
        if( c->acc_voltage > c->max_charge_voltage+0.1 )
            c->charge_pwm_percentage--;

        if( c->acc_voltage < (c->max_charge_voltage-0.1) )
            c->charge_pwm_percentage++;

        return;
    }

    // charge

    if( c->acc_voltage < (c->max_charge_voltage-0.1) )
    {
        // Current control

        if( c->charge_current > c->max_charge_current )
            c->charge_pwm_percentage--;

        if( c->charge_current > c->min_charge_current )
            c->charge_pwm_percentage--;
    }
    else
    {
        // voltage control
        goto as_idle;
    }
}

// -----------------------------------------------------------------------
// Act on current state
// -----------------------------------------------------------------------


static void
ch_state_action( charger *c )
{
    switch( c->state )
    {
    case ch_init:
        // starting state - find out and switch to needed state
        if( ! ch_have_mains_power( c ) )
        {
            // Have no power, we're discharging then
            if( ch_acc_low( c ) )
                c->state = ch_low_charge;
            else
                c->state = ch_discharge;
            break;
        }

        if( ch_acc_hi( c ) )
        {
            c->state = ch_idle;
            break;
        }

        // We're powered and not fully charged - go charge
        c->state = ch_charge;
        break;

    case ch_charge:
        ch_set_charge_current( c );
        c->load_enabled = 1;

        if( ch_acc_hi( c ) )
            c->state = ch_idle;

        if( ! ch_have_mains_power( c ) )
            c->state = ch_discharge;

        break;

    case ch_discharge:
        if( ch_acc_low( c ) )
            c->state = ch_low_charge;

        if( ch_have_mains_power( c ) )
            c->state = ch_charge;

        break;

    case ch_idle:
        // Acc is charged, no current flow
        ch_set_charge_current( c );
        c->load_enabled = 1;

        if( ! ch_have_mains_power( c ) )
            c->state = ch_discharge;

        break;

    case ch_low_charge:
        // Acc charge is very low, load is disconnected
        c->load_enabled = 0;

        if( ch_have_mains_power( c ) )
            c->state = ch_charge;

        break;

    case ch_failed:
        // Insane, all off
        c->load_enabled = 0;

        break;
    }
}


// -----------------------------------------------------------------------
// Main loop
// -----------------------------------------------------------------------

charger c12v =
{
    .state = ch_init,

    .low_charge_voltage = 10.8,
    .max_charge_voltage = 13.8,

    .min_charge_current = 0,
    .max_charge_current = 400.0,

    .ch_input = ch_input_12v,
    .ch_output = ch_output_12v,

};


charger c24v =
{
    .state = ch_init,

    .low_charge_voltage = 21.6,
    .max_charge_voltage = 13.8*2,

    .min_charge_current = 0,
    .max_charge_current = 200.0,

    .ch_input = ch_input_24v,
    .ch_output = ch_output_24v,

};

void
charger_main_loop( void )
{

    for(;;)
    {
        ch_get_data( &c12v );
        ch_state_action( &c12v );
        ch_set_charge_current( &c12v );
        ch_control_outs( &c12v );

        ch_display( &c12v );
        ch_dump( &c12v );


        ch_get_data( &c24v );
        ch_state_action( &c24v );
        ch_set_charge_current( &c24v );
        ch_control_outs( &c24v );

        ch_display( &c24v );
        ch_dump( &c24v );

        NutSleep( 500 );
    }

}


// -----------------------------------------------------------------------
// Display
// -----------------------------------------------------------------------

static void
ch_display( charger *c )
{

    // We have 2 buttons and 4 lights

    if( c->button )
        c->state = ch_init;

    // Green light - on = load is on, flash = on battery

    if( !c->load_enabled )
        c->green_led = 0;
    else
    {
        if( c->state == ch_discharge )
            c->green_led = !c->green_led;
        if( c->state == ch_charge )
        {
            c->green_led++;
            if( c->green_led > 3 )
                c->green_led = 0; // flash slowly
        }
        else
            c->green_led = 1;
    }

    // Red light - on = battery is empty, flash = battery is low

    if( !c->load_enabled )
        c->red_led = 1;
    else
    {
        if( ch_acc_50_percent( c ) )
            c->red_led = !c->red_led;
    }
}


// -----------------------------------------------------------------------
// Dump
// -----------------------------------------------------------------------

static void
ch_dump( charger *c )
{
    printf(
           "state %d, in %.2fV, load %.2fV, acc %.2fV, I %.2fA, pwm %.1f%%, load ena %d",
           c->state,
           (double)c->mains_voltage, (double)c->load_voltage, (double)c->acc_voltage,
           (double)c->charge_current, (double)(c->charge_pwm_percentage / 2.55), c->load_enabled );

}










