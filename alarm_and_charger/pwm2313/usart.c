#include "defs.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include "main.h"

// sync gives higher speed?!
#define SYNC_MODE 1


volatile uint8_t usart_pwm = 128; 

static void set_usart_data(void)
{
	uint8_t reduced = usart_pwm >> 5; // We can do only 3 bit pwm with usart

	uint8_t bits = 0xFF;
	bits >>= 8 - reduced;

	UDR = bits;		
}

void
init_usart(void)
{
#if SYNC_MODE
	//DDR_XCK - set sync master mode
	DDRD |= _BV(PD2);
#endif

#if 1
	// About 25 KHz, if set higher it loads CPU too much
	UBRRL = 15; 
	UBRRH = 0;
#else
	// 0.5 mbps (*2?)
	unsigned int ubrr_value = 4;

	// Set baud rate
	UBRRL = ubrr_value & 255; 
	UBRRH = ubrr_value >> 8;
#endif
	// Frame Format: 8 data bits
	UCSRC = _BV(UCSZ1) | _BV(UCSZ0);
	//if(stopbits == 2) UCSRC |= _BV(USBS);

#if SYNC_MODE
	UCSRC |= _BV(UMSEL);
#endif

	//if(x2) 
	//UCSRA = _BV(U2X); // 2x - async only

	UCSRA = 0;

	// USART Data Register Empty Interrupt Enable
	UCSRB = _BV(UDRIE);

	// Enable The receiver and transmitter
	//UCSRB |= _BV(RXEN) | _BV(TXEN);
	UCSRB |= _BV(TXEN);

	set_usart_data();
}


ISR(USART_UDRE_vect) 
{
	set_usart_data();
}

