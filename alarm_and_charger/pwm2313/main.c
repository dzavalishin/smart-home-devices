#include "defs.h"

#include <avr/io.h>
#include <stdlib.h>

#include <avr/interrupt.h>
#include <util/delay.h>

#include "main.h"
#include "util.h"

uint8_t volatile activity = 0;


int
main(void)
{
	// LED DDR
	DDRD |= _BV(PD6);

	turn_led_on();

	init_eeprom();
	eeprom_load(); // NB! enables interrupts!

	init_pwm();
	init_usart();

//	init_spi();
//	init_ss();

	init_encoders();

	activity = 1;
//main_pwm[0] = 0xFF;
    sei();

	for(;;)
	{
	turn_led_off();

    _delay_ms(200);

	if( activity )
	{
		activity = 0;
		turn_led_on();
	}

    _delay_ms(200);
//main_pwm[1] = 0xFF;
	}

}


void timer10hz(void)
{

	read_encoders();
//	activity = 1;

}



