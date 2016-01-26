/**
 *
 * DZ-MMNET-CHARGER: Modbus/TCP I/O module based on MMNet101.
 *
 * Charger state machine.
 *
**/

#include <inttypes.h>

#define V_DIODE_DROP 0.7
#define R_CURRENT_SENSE 0.1

typedef enum ch_state
{
    ch_init,		// starting state - find out and switch to needed state
    ch_charge,    	// 
    ch_discharge,    	//
    ch_idle,    	// Acc is charged, no current flow
    ch_low_charge,    	// Acc charge is very low, load is disconnected

} ch_state;

typedef struct charger
{
    ch_state            state;

    float               low_charge_voltage;  // minimal charge acc voltage
    float               max_charge_voltage;  // maximal charge acc voltage

    float               min_charge_current;
    float               max_charge_current;

    void                (*ch_input)( struct charger *c );

    // Voltage circuit:
    // mains -> diode -> PWM MOSFET -> load <-> Isense resistor <-> acc
    // need diode to be able to check for existance of mains voltage

    float               mains_voltage;  // incoming voltage from AC/DC unit
    float               load_voltage;   // voltage on output terminals
    float               acc_voltage;    // voltage on accumuator

    float               charge_current;  // maximal charge acc voltage

    int                 charge_pwm_percentage; // 0-255 % of charger PWM

} charger;

typedef uint8_t bool;
//#define bool

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
    c->mains_voltage = get_float_adc( 0 );
    c->load_voltage = get_float_adc( 1 );
    c->acc_voltage = get_float_adc( 2 );
}

static void ch_input_24v( struct charger *c )
{
    c->mains_voltage = get_float_adc( 4 );
    c->load_voltage = get_float_adc( 5 );
    c->acc_voltage = get_float_adc( 6 );
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

// -----------------------------------------------------------------------
// Charge current control
// -----------------------------------------------------------------------

void
ch_set_charge_current( charger *c )
{
    // Not charging at all
    if( (c->state == ch_discharge) || (c->state == ch_low_charge) || (c->state == ch_init) )
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
        ch_load_enable( 1 );

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
        ch_load_enable( 1 );

        if( ! ch_have_mains_power( c ) )
            c->state = ch_discharge;

        break;

    case ch_low_charge:
        // Acc charge is very low, load is disconnected
        ch_load_enable( 0 );

        if( ch_have_mains_power( c ) )
            c->state = ch_charge;

        break;


    }
}















