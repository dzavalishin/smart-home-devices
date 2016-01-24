/**
 *
 * DZ-MMNET-MODBUS: Modbus/TCP I/O module based on MMNet101.
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


// ADC initialize
static void adc_init( dev_major* d )
{
    (void) d;

    if( !RT_IO_ENABLED(IO_ADC) )
        return;

    if( init_subdev( &io_adc, SERVANT_NADC, "adc" ) )
        return;


    ADCSRA = 0x00; //disable adc
    ADMUX = ADMUX_REFS; //0b11100000;

    ACSR  = 0x80; // disable analog comparator
    ADCSRA = 0x7|_BV(ADEN); // enabled, PreScaler = 111
}


static int8_t adc_start( void )
{
    if( !RT_IO_ENABLED(IO_ADC) ) return -1; // TODO KILLME

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


