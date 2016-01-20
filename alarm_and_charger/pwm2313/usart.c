#include "defs.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include "main.h"


uint8_t usart_pwm = 128; 

void
init_usart(void)
{
#if 1
	// Just maximum possible baud rate - supposed to be 0.5 mbps @8 MHz
	UBRRL = 0; 
	UBRRH = 0;
#else
	// 0.5 mbps (*2?)
	unsigned int ubrr_value = 4;

	// Set baud rate
	UBRRL = ubrr_value & 255; 
	UBRRH = ubrr_value >> 8;
#endif
	// Frame Format: __synchronous__, 8 data bits
	UCSRC = _BV(UCSZ1) | _BV(UCSZ0) | _BV(UMSEL);
	//if(stopbits == 2) UCSRC |= _BV(USBS);

	//if(x2) 
	//UCSRA = _BV(U2X); // 2x - async only

	UCSRA = 0;

	// USART Data Register Empty Interrupt Enable
	UCSRB = _BV(UDRIE);

	// Enable The receiver and transmitter
	//UCSRB |= _BV(RXEN) | _BV(TXEN);
	UCSRB |= _BV(TXEN);
}


ISR(USART_UDRE_vect) 
{
	uint8_t reduced = usart_pwm >> 5; // We can do only 3 bit pwm with usart

	uint8_t bits = 0xFF;
	bits >>= 8 - reduced;
	UDR = bits;		

}
