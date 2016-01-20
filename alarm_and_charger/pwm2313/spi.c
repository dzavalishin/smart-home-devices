#include "defs.h"

#include <avr/io.h>
#include <avr/interrupt.h>
//#include <util/delay.h>

#include "main.h"


#define CTRL_PORT   DDRB
#define DATA_PORT   PORTB
#define SS_PIN      PB4
#define CLK_PIN     PB7
#define DI_PIN      PB5
#define DO_PIN      PB6
static char res = 1;
/*
    Initialize USI as slave
*/
void init_spi(void)
{
    //DO pin is configured for output
    CTRL_PORT |= _BV(DO_PIN);
    //All other pins as input
    CTRL_PORT &= ~_BV(DI_PIN) | ~_BV(CLK_PIN) | ~_BV(SS_PIN);
    //I didn't think that I needed to set pull ups.
    DATA_PORT |= _BV(DI_PIN) | _BV(CLK_PIN);

	// TODO positive edge clock!
     
    //Enable USI overflow interrupt, set three wire mode and set
    //  clock to External, (positive?) negative edge.
    USICR = _BV(USIOIE) | _BV(USIWM0) |  _BV(USICS1);
    //Clear overflow flag
    USISR = _BV(USIOIF);    
    sei();
}
/*
    USI overflow intterupt - triggered when transfer complete
*/
ISR(USI_OVERFLOW_vect)
{
    res = USIDR;
    USISR = _BV(USIOIF);
     
    USIDR = ~res;   

	activity++;
}

