#include "defs.h"

#include <avr/io.h>
#include <stdlib.h>

#include <avr/interrupt.h>
#include <util/delay.h>

#include "main.h"
#include "util.h"

uint8_t volatile activity = 0;
uint8_t eeprom_timeout_counter = 0;

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

	init_spi();
	init_ss();

	init_encoders();

	activity = 1;

    sei();

	for(;;)
	{
	turn_led_off();

    _delay_ms(200);

	if( activity )
	{
		activity = 0;
		turn_led_on();

		// If some activity (pwm data changed) - plan eeprom save.
		// Not immediate - let's wait for 20 sec to gather all changes.
		eeprom_timeout_counter = 20*5; // 20 sec
	}

	if( eeprom_timeout_counter-- == 1 )
		eeprom_save();

    _delay_ms(200);
	}

}


void timer10hz(void)
{

	read_encoders();

}



