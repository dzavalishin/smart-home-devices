/**
 *
 * DZ-MMNET-CHARGER: Modbus/TCP I/O module based on MMNet101.
 *
 * TWI (I2C) support.
 *
**/

#include "defs.h"

//#include <sys/thread.h>
//#include <sys/timer.h>

#include <avr/io.h>
//#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h>

#include <stdio.h> // printf

#include "io_twi.h"
#include "util.h"
#include "dev_map.h"
#include "prop.h"

#include <dev/twif.h>

//#include "delay.h"

#if ENABLE_TWI


static int8_t      twi_rtc_to_string( struct dev_minor *sub, char *out, uint8_t out_size );
static int8_t      twi_eeprom_to_string( struct dev_minor *sub, char *out, uint8_t out_size );


// ----------------------------------------------------------------------
// Parameters
// ----------------------------------------------------------------------

static uint16_t         twi_speed = 10000; // 10kHz

static void inline twi_setup( void )
{
    unsigned long busspeed = twi_speed;
    TwIOCtl(TWI_SETSPEED, &busspeed);
}

errno_t
twi_activate_prop_change(struct dev_properties *ps, void *context, uint16_t offset, void *vp )
{
    twi_setup();
    return 0;
}

// ----------------------------------------------------------------------
// Init/start
// ----------------------------------------------------------------------


// Assume interrupts disabled during init
static int8_t twi_init( dev_major* d )
{
    if( init_subdev( d, 2, 0 ) )
        return -1;

    d->subdev[0].name = "twi.rtc";
    d->subdev[0].to_string = twi_rtc_to_string;

    d->subdev[1].name = "twi.eeprom";
    d->subdev[1].to_string = twi_eeprom_to_string;


    unsigned long busspeed = twi_speed;	

    printf("I2C init...");

    TwInit(0);

    twi_setup( );

    TwIOCtl(TWI_GETSPEED, &busspeed);
    printf(" done, speed is %ld\n", busspeed);


    return 0;
}

static int8_t twi_start( dev_major* d )
{
    return 0;
}

#endif // ENABLE_TWI

// ----------------------------------------------------------------------
// ToString
// ----------------------------------------------------------------------

static int8_t      twi_rtc_to_string( struct dev_minor *sub, char *out, uint8_t out_size )
{
{
    return dev_uint16_to_string( sub, out, out_size, 0 );
}
}

static int8_t      twi_eeprom_to_string( struct dev_minor *sub, char *out, uint8_t out_size )
{
{
    return dev_uint16_to_string( sub, out, out_size, 0 );
}
}


// ----------------------------------------------------------------------
// General IO definition
// ----------------------------------------------------------------------

static dev_property twi_prop[] =
{
    {	.type = pt_int16, .name = "speed", .valp = &twi_speed, .activate = twi_activate_prop_change }
};

static dev_properties twi_props =
{
    twi_prop,
    PROP_COUNT(twi_prop)
};


dev_major io_twi =
{
    .name = "twi",

#if ENABLE_TWI
    .init	= twi_init,
    .start	= twi_start,
//    .stop	= twi_stop,
//    .timer 	= twi_test_send,
#endif // ENABLE_TWI

    .to_string = 0, //twi_to_string,
    .from_string = 0,

    .minor_count = 1,
    .subdev = 0,

    .prop = &twi_props,
};
