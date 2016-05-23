#include "defs.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include "util.h"
#include "main.h"

void spi_set_register( uint8_t addr, uint8_t data );


// Setup pin interrupt to react on slave select pin

// INT1
#define SS_PORT		PORTD
#define SS_PIN		PD3

void
init_ss(void)
{
	// rISING EDGE OF int1 GENERATES INTERRUPT
	//MCUCR |= ISC11 | ISC10;

	// Any edge on INT1 generates interrupt
	MCUCR |= ISC10;

	GIMSK |= INT1; // Enable int 1 interrupt
}


// External interrupt INT1 handler
ISR(INT1_vect) 
{    
	// Read the bit itself

	uint8_t ss = SS_PORT & _BV(SS_PIN);

	if( !ss )
	{
		// Falling edge, master selected us and will xmit
		spi_count = 0;
		spi_rx_enable = 1;

		return;
	}

	// Rising edge, io done
	spi_rx_enable = 0;

	// Process data
	uint8_t i = 0;
	while( spi_count > 2 )
	{
		// Byte 1 is address (register number)
		uint8_t addr = spi_data[i++];
		// Byte 2 is data to store
		uint8_t data = spi_data[i++];

		spi_count -= 2;
		spi_set_register( addr, data );
	}

}


void
spi_set_register( uint8_t addr, uint8_t data )
{
	// Reg 0-4 - PWM outputs 

	switch( addr )
	{
		case 0:
		case 1:
		case 2:
		case 3:
			main_pwm[addr] = data;
			break;

		case 4:
			usart_pwm = data;
	}

}



