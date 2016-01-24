/**
 *
 * DZ-MMNET-MODBUS: Modbus/TCP I/O module based on MMNet101.
 *
 * Software PWM.
 *
 * Multiple-output PWM using one HW timer.
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


#define DEBUG 0

#if SERVANT_NPWM > 0


#if SERVANT_NPWM != 2
#  error This implementation supports just 2 hw PWM outputs
#endif

static uint8_t          pwm_val[SERVANT_NPWM];




static inline void pwm_init_timer_0(void)
{
    // fast PWM, non-inverting, max clock
    TCCR0 = WGM00 | WGM01 | COM01 | CS00;
}

static inline void pwm_set_pwm_0( uint8_t val )
{
    OCR0 = val;
}

static inline void pwm_init_timer_2(void)
{
    // fast PWM, non-inverting, max clock
    TCCR2 = WGM20 | WGM21 | COM21 | CS20;
}


static inline void pwm_set_pwm_2( uint8_t val )
{
    OCR2 = val;
}























// TODO KILLME
void set_an(unsigned char port_num, unsigned char data)
{
}


static int8_t      pwm_to_string( struct dev_minor *sub, char *out, uint8_t out_size )   	// 0 - success
{
    //snprintf( out, out_size, "%d", pwm_val[sub->number] );
    return dev_uint16_to_string( sub, out, out_size, pwm_val[sub->number] );
}

/*
static int8_t      pwm_from_string( struct dev_minor *sub, char *)         			// 0 - success
{
    return -1;
}
*/

static int8_t pwm_init_dev( dev_major* d )
{
    uint8_t i;
return;
    if( init_subdev( d, SERVANT_NADC, "pwm" ) )
        return -1;

    for( i = 0; i < d->minor_count; i++ )
    {
        dev_minor *m = d->subdev+i;


        m->to_string = pwm_to_string;
        //m->from_string = pwm_from_string;
    }

    pwm_init_timer_0();
    pwm_init_timer_2();

    return 0;
}

static int8_t pwm_start_dev( dev_major* d )
{
    (void) d;

    return -1;
}

// TODO
static void pwd_stop_dev( dev_major* d )
{
    (void) d;

}


#endif // SERVANT_NPWM > 0



dev_major io_pwm =
{
    .name = "pwm",

#if SERVANT_NPWM > 0
    .init	= pwm_init_dev,
    .start	= pwm_start_dev,
    .stop	= pwd_stop_dev, // TODO
    .timer	= 0,
#endif

    .started	= 0,

    .to_string 	 = 0,
    .from_string = 0,

    .minor_count = SERVANT_NPWM,
    .subdev 	 = 0,
};






















