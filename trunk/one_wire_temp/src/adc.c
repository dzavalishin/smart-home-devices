#include "adc.h"
#include "defs.h"
#include "util.h"
#include "uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#if SERVANT_NADC > 8
#  error No more than eight channels, sorry
#endif

// Keeps current ADC channel value
unsigned volatile int adc_value[SERVANT_NADC];

// input we are converting now
unsigned volatile char current_ad_input = SERVANT_NADC-1;

//unsigned char get_an(unsigned char port_num) { return adc_value[port_num]; }

//static void postProcessAdc(unsigned char channel);

// no left align ADLAR=0
// internal reference, AREF with external capacitor
#define ADMUX_REFS (_BV(REFS1)|_BV(REFS0))
//#define ADMUX_REFS 0b11100000

//ADC initialize
void adc_init(void)
{
    ADCSRA = 0x00; 		//disable adc
    ADMUX = ADMUX_REFS; 	//0b11100000;

    ACSR  = 0x80; 		// disable analog comparator
    ADCSRA = 0x7|_BV(ADEN); 	// enabled, PreScaler = 111
}


void adc_start()
{
    cli();

    current_ad_input++;
    if(current_ad_input >= SERVANT_NADC)
        current_ad_input = 0;


    ADMUX &= ADMUX_REFS;
    ADMUX |= current_ad_input; 		// we use single-ended inputs only

    ADCSRA |= _BV(ADIF); 		// reset IF by WRITING ONE!! to it
    ADCSRA |= _BV(ADSC)|_BV(ADIE); 	// enable interrupts and start conversion

    sei();
}


void adc_stop()
{
    cli();
    ADCSRA &= ~_BV(ADIE);
					// make sure we will get no interrupt after sei()
    ADCSRA |= _BV(ADIF);		// reset IF by WRITING ONE!! to it
    sei();
}


ISR(ADC_vect)
{
    ADCSRA &= ~_BV(ADIE); // no more ints

    adc_value[current_ad_input] = 0xFF & ADCL;
    adc_value[current_ad_input] |= 0x300 & (((int)ADCH) << 8);

    sei();
//    postProcessAdc(current_ad_input);
    adc_start();
}







