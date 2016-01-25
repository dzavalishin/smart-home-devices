/**
 *
 * DZ-MMNET-CHARGER: Acc charger module based on MMNet101.
 *
 * Hardware PWM.
 *
**/

#include "defs.h"
#include "runtime_cfg.h"
#include "servant.h"

#include "io_pwm.h"


#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include <sys/event.h>

//#warning use timer 1 instead of 0, timer 0 is OS


#define DEBUG 0

#if SERVANT_NPWM > 0


#if SERVANT_NPWM != 2
#  error This implementation supports just 2 hw PWM outputs
#endif

static uint8_t          pwm_val[SERVANT_NPWM];






// TODO KILLME
void set_an(unsigned char port_num, unsigned char data)
{
}


// ----------------------------------------------------------------------
// IO
// ----------------------------------------------------------------------

#if 0
static inline void pwm_init_timer_0(void)
{
    // fast PWM, non-inverting, max clock
    //CR0 = WGM00 | WGM01 | COM01 | CS00;

    // TODO move OS clock to timer 3

    // fast PWM, non-inverting, don't touch clock - network stack uses it, apparently
    TCCR0 |= WGM00 | WGM01 | COM01;
    TCCR0 &= ~COM00;

    DDRB |= _BV(PB4);
}

static inline void pwm_set_pwm_0( uint8_t val )
{
    OCR0 = val;
}
#endif

static inline void pwm_init_timer_1(void)
{
    TCCR1A = COM1B1 | WGM10;
    TCCR1B = WGM12 | CS10;
    DDRB |= _BV(PB6);
}

static inline void pwm_set_pwm_1( uint8_t val )
{
    //OCR1BH = val;
    //OCR1BL = 0;
    OCR1BH = 0;
    OCR1BL = val;
}


static inline void pwm_init_timer_2(void)
{
    // fast PWM, non-inverting, max clock
    TCCR2 = WGM20 | WGM21 | COM21 | CS20;

    DDRB |= _BV(PB7);
}


static inline void pwm_set_pwm_2( uint8_t val )
{
    OCR2 = val;
}




// ----------------------------------------------------------------------
// Report status / set value
// ----------------------------------------------------------------------

static int8_t      pwm_to_string( struct dev_minor *sub, char *out, uint8_t out_size )   	// 0 - success
{
    //snprintf( out, out_size, "%d", pwm_val[sub->number] );
    return dev_uint16_to_string( sub, out, out_size, pwm_val[sub->number] );
}


static int8_t      pwm_from_string( struct dev_minor *sub, const char *in)         			// 0 - success
{
    int data = atoi(in);

    uint16_t channel = sub->number;

    if( channel >= SERVANT_NPWM )
        return -1;

     pwm_val[channel] = data;

    return 0;
}



// ----------------------------------------------------------------------
// Init/start
// ----------------------------------------------------------------------

static int8_t pwm_init_dev( dev_major* d )
{
    //uint8_t i;
//return -1;
    if( init_subdev( d, SERVANT_NPWM, "pwm" ) )
        return -1;

    dev_init_subdev_getset( d, pwm_from_string, pwm_to_string );
/*
    for( i = 0; i < d->minor_count; i++ )
    {
        dev_minor *m = d->subdev+i;


        m->to_string   = pwm_to_string;
        m->from_string = pwm_from_string;
    }
*/
    //pwm_init_timer_0();
    pwm_init_timer_1();
    pwm_init_timer_2();

    return 0;
}

static int8_t pwm_start_dev( dev_major* d )
{
    (void) d;

    //return -1;
    return 0;
}

static void pwd_stop_dev( dev_major* d )
{
    (void) d;

    // Turn off outputs
    TCCR0 &= ~COM00;
    TCCR0 &= ~COM01;

    TCCR2 &= ~COM21;
    TCCR2 &= ~COM20;

    DDRB &= ~_BV(PB4);
    DDRB &= ~_BV(PB7);

}

// ----------------------------------------------------------------------
// Test
// ----------------------------------------------------------------------

static void pwm_test_data( dev_major* d )
{
    pwm_val[0]	= rand() / (RAND_MAX / 0xff + 1);
    pwm_val[1]	= rand() / (RAND_MAX / 0xff + 1);

    pwm_set_pwm_1( pwm_val[0] );
    pwm_set_pwm_2( pwm_val[1] );

    // TODO FIXME dio resets DDRs for us, fix it there and remove here
    // TODO really? try to remove it here
    DDRB |= _BV(PB4);
    DDRB |= _BV(PB7);

}


#endif // SERVANT_NPWM > 0



dev_major io_pwm =
{
    .name = "pwm",

#if SERVANT_NPWM > 0
    .init	= pwm_init_dev,
    .start	= pwm_start_dev,
    .stop	= pwd_stop_dev, // TODO
    .timer	= pwm_test_data,
#endif

    .started	= 0,

    .to_string 	 = 0,
    .from_string = 0,

    .minor_count = SERVANT_NPWM,
    .subdev 	 = 0,
};






















