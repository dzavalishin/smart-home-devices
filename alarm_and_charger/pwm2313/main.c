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
	DDRD |= _BV(PD6);

	turn_led_on();

	init_eeprom();
	eeprom_load();


	init_spi();
	init_pwm();
	init_usart();

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
	}

}

