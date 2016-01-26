/**
 *
 * DZ-MMNET-CHARGER: Acc charger module based on MMNet101.
 *
 * Analog inputs (ADC).
 *
**/

#include "defs.h"
#include "util.h"
#include "runtime_cfg.h"

#include "io_adc.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#if SERVANT_NADC > 8
#  error No more than eight channels, sorry
#endif

// Keeps current ADC channel value
unsigned volatile int adc_value[SERVANT_NADC];

// input we are converting now
unsigned volatile char current_ad_input = SERVANT_NADC-1;


// no left align ADLAR=0
// internal reference 2.56v, AREF with external capacitor
#define ADMUX_REFS (_BV(REFS1)|_BV(REFS0))

float get_float_adc( uint8_t nAdc )
{
    return adc_value[ nAdc ] / 1600.0f;
}

static int8_t
adc_to_string( struct dev_minor *sub, char *out, uint8_t out_size )
{
    if( out_size < 20 ) return -1;

    //double fav = adc_value[ sub->number ] / 400.0;
    dtostrf( get_float_adc( sub->number ), 5, 3, out );

    //return dev_uint16_to_string( sub, out, out_size, adc_value[ sub->number ] );

    //snprintf( out, out_size, "%d", adc_value[ sub->number ] );
    return 0;
}


// ADC initialize
static int8_t adc_init( dev_major* d )
{
    if( init_subdev( d, SERVANT_NADC, "adc" ) )
        return -1;

    dev_init_subdev_getset( d, 0, adc_to_string );

    ADCSRA = 0x00; //disable adc
    ADMUX = ADMUX_REFS; //0b11100000;

    ACSR  = 0x80; // disable analog comparator
    ADCSRA = 0x7|_BV(ADEN); // enabled, PreScaler = 111

    return 0;
}


static int8_t adc_start( void )
{
    cli();
    current_ad_input++;

    if(current_ad_input >= SERVANT_NADC)
        current_ad_input = 0;

    ADMUX &= ADMUX_REFS;
    ADMUX |= current_ad_input; // we use single-ended inputs only

    ADCSRA |= _BV(ADIF); // reset IF by WRITING ONE!! to it

    ADCSRA |= _BV(ADSC)|_BV(ADIE); // enable interrupts and start conversion
    sei();

    return 0;
}


static void adc_stop( dev_major* d )
{
    (void) d;

    cli();
    ADCSRA &= ~_BV(ADIE);
    // make sure we will get no interrupt after sei()
    ADCSRA |= _BV(ADIF); // reset IF by WRITING ONE!! to it
    sei();
}

ISR(ADC_vect)
{
    ADCSRA &= ~_BV(ADIE); // no more ints

    adc_value[current_ad_input] = (0xFFu & ADCL) | (0x300u & (((unsigned int)ADCH) << 8));
    io_adc.subdev[current_ad_input].io_count++;

    sei();
    //postProcessAdc(current_ad_input);
    adc_start();
}

// ----------------------------------------------------------------------
// General IO definition
// ----------------------------------------------------------------------

static int8_t adc_start_dev( dev_major* d ) { (void) d; adc_start(); return 0; }


dev_major io_adc =
{
    .name = "adc",

    .init = adc_init,
    .start = adc_start_dev,
    .stop = adc_stop,
    .timer = 0,

    .to_string = 0,
    .from_string = 0,

    .minor_count = SERVANT_NADC,
    .subdev = 0,
};


