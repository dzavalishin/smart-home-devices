#include "freq_defs.h"
#include "timer0.h"
#include "defs.h"
#include "uart.h"
#include "modbus.h"
#include "util.h"
#include "temperature.h"
#include "eeprom.h"
#include "ui_menu.h"


#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>





static void  timer0_50ms();
static void  timer0_5sec();


// TODO move here timer const

unsigned int t0_count, halfsec_count;

// For 8 MHz Atmega8 we have to divide by 400 000 to get 50mSec
// So prescale will be 1024 and we will count to 195, actually from 61 to 256
// and then will divide by two in software

#define PRESCALED_FREQ (F_CPU/1024)
#define TRAGET_FREQ 100
#define DIVIDER (PRESCALED_FREQ/TRAGET_FREQ)
#define TCNT0PRELOAD (256-(DIVIDER/2))

void timer0_init(void)
{

    TCCR0 = 0x00; 			// stop
    TCNT0 = TCNT0PRELOAD; 		// set count - TODO - calc correct value
    //OCR0  = 0xC3;  			// set compare
    TCCR0 = 0x05; 			// start timer, clk/1024

    t0_count = 0;
    halfsec_count = 0;

    TIMSK |= _BV(TOIE0);
}


char softDivider = 0;
#define SOFT_DIV 10

ISR(TIMER0_OVF_vect)
{
    cli();

    TCNT0 = TCNT0PRELOAD; 		// reload counter value


#if HAVE_MODBUS_RTU
    modbus_timer_callout_5msec();
#endif

    // 200 Hz / 5 msec here

    softDivider++;
    if(softDivider < SOFT_DIV)
    {
        sei(); // need?
        return;
    }

//    modbus_timer_callout_5msec();

    // 20 Hz / 50 msec here

    timer0_50ms();

    // each 0.5 sec

    if( halfsec_count++ >= 100 )
    {
        halfsec_count = 0;

#if SERVANT_NFREQ > 0
        freq_meter_05sec_timer(); 	// goes first as more time-critical
#endif
#if N_TEMPERATURE_IN > 0
        temp_meter_05sec_timer();
#endif
        menu_timer_05sec();
    }

    // TODO too long with interrupts off

    sei();

    // each 5 sec

    t0_count++;
    if( t0_count == 1000 )
    {
        t0_count = 0;
        timer0_5sec();
    }

}



#if HALF_DUPLEX
volatile char halfduplex_pause_count = 0;

void mark_halfduplex_timeout() { halfduplex_pause_count = 1; }


void wait_for_halfduplex_timeout()
{
    char ena = SREG & _BV(SREG_I); // was enabled?
    sei();
    while(halfduplex_pause_count > 0)
        ;
    if(!ena) cli();
}
#endif


static uint16_t led1_time, led2_time;

// each 50 msec
void timer0_50ms()
{

#if HALF_DUPLEX
    if(halfduplex_pause_count > 0)
        halfduplex_pause_count--;
#endif

    if( led1_time > 0 )
    {
        led1_time -= 50;
        turn_led1_off();
    }

    if( led2_time > 0 )
    {
        led2_time -= 50;
        turn_led2_off();
    }

}

void led1_timed( uint16_t msec )
{
    led1_time += msec;
    led1_time %= 500;
    turn_led1_on();
}

void led2_timed( uint16_t msec )
{
    led2_time += msec;
    led2_time %= 500;
    turn_led2_on();
}


void
timer0_5sec()
{
    eeprom_timer_5sec();
}









