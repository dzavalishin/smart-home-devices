/**
 *
 * DZ-MMNET-MODBUS: Modbus/TCP I/O module based on MMNet101.
 *
 * Analog inputs (ADC).
 *
**/

#include "adc.h"
#include "defs.h"
#include "util.h"
//#include "packet_types.h"

#include "net_io.h"

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


// ADC initialize
void adc_init(void)
{
    ADCSRA = 0x00; //disable adc
    ADMUX = ADMUX_REFS; //0b11100000;

    ACSR  = 0x80; // disable analog comparator
    ADCSRA = 0x7|_BV(ADEN); // enabled, PreScaler = 111
}


void adc_start()
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
}


void adc_stop(void)
{
    cli();
    ADCSRA &= ~_BV(ADIE);
    // make sure we will get no interrupt after sei()
    ADCSRA |= _BV(ADIF); // reset IF by WRITING ONE!! to it
    sei();
}

ISR(ADC_vect)
{
    ADCSRA &= ~_BV(ADIE); // no more ints

//    adc_value[current_ad_input] = 0xFF & ADCL;
//    adc_value[current_ad_input] |= 0x300 & (((int)ADCH) << 8);

    adc_value[current_ad_input] = (0xFFu & ADCL) | (0x300u & (((unsigned int)ADCH) << 8));


    sei();
    //postProcessAdc(current_ad_input);
    adc_start();
}

// ----------------------------------------------------------------------
// Postprocessing - called after each adc cycle
// ----------------------------------------------------------------------

#if 0

unsigned volatile char adc_send_mask = 0; // bit 0 == 1 -> need to send ch 0
unsigned volatile int adc_prev[SERVANT_NADC];

void postProcessAdc(unsigned char channel)
{
#if 1
    signed int diff = adc_value[channel] - adc_prev[channel];
    if( diff < 0 ) diff = -diff;

    if( diff > 17 ) {
        // если значение АЦП изменилось больше чем на 2
        adc_prev[channel] = adc_value[channel];
        adc_send_mask |= _BV(channel);

        //triggerSendOut(); // Ask sending thread to do its job
        // TODO check alarm here or in send changed
    }

#else
    if( adc_value[channel] != adc_prev[channel] ) { // без лишних проверок
        adc_prev[channel] = adc_value[channel];
        adc_send_mask |= _BV(channel);
        //flash_led_once();
        //ua rt_sendbyte(hexdigit(channel&0xFu));
        // TODO check alarm here or in send changed

    }
#endif
}

#if 0
// called from packet reception code
void request_adc_data_send(unsigned char channel)
{
    adc_send_mask |= _BV(channel);
}


void send_changed_adc_data()
{
#if 0
    unsigned char i;
    for( i = 0; i < SERVANT_NADC; i++ )
    {
        if( adc_send_mask & _BV(i) )
        {
            adc_send_mask &= ~_BV(i);
            send_pack(TOHOST_ANALOG_VALUE, i, adc_value[i] );
        }
    }
#endif
}


#endif

#endif
