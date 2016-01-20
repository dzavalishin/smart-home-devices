#include "defs.h"

#include <util/delay.h>
#include <avr/io.h>

#include "util.h"
//#include "uart.h"
#include "main.h"


#if 0

// --------------------------------------------------------------------------
// Formatting
// --------------------------------------------------------------------------

char hexdigit( unsigned char d )
{
    if( d < 0xa )
        return '0' + d;
    return 'A'+d-0xa;
}

// --------------------------------------------------------------------------
// Delays
// --------------------------------------------------------------------------

void delay_sec() { delay_halfsec(); delay_halfsec(); }
void delay_halfsec()
{
    uint8_t i; 
    for( i = 50; i > 0; --i )
        _delay_ms(10);
}

// 1/4 sec
void delay_qsec()
{
    uint8_t i; 
    for( i = 25; i > 0; --i )
        _delay_ms(10);
}

// 1/8 sec
void delay_esec()
{
    uint8_t i; 
    for( i = 25; i > 0; --i )
        _delay_ms(5);
}

// ~1/16 sec
void delay_thsec()
{
    uint8_t i; 
    for( i = 6; i > 0; --i )
        _delay_ms(11);
}
#endif

// --------------------------------------------------------------------------
// LED
// --------------------------------------------------------------------------
#if 0

/* flash onboard LED */
void flash_led(uint8_t count)
{
    uint8_t i; 
    for (i = count; i > 0; --i)
        flash_led_once();
}


void flash_led_once()
{

    //LED_PORT &= ~_BV(LED);
    turn_led_on();
    delay_thsec();
    //LED_PORT |= _BV(LED);
    turn_led_off();
    delay_esec();
}

void flash_led_long()
{
    //LED_PORT |= _BV(LED);
    turn_led_off();
    delay_sec();
    //LED_PORT &= ~_BV(LED);
    turn_led_on();
    delay_sec();
    //LED_PORT |= _BV(LED);
    turn_led_off();
    delay_sec();
}
#endif

#if 0
void turn_led_on()
{
    LED_PORT |= _BV(LED);
    //LED_PORT &= ~_BV(LED);
}


void turn_led_off()
{
    //LED_PORT |= _BV(LED);
    LED_PORT &= ~_BV(LED);
}
#endif


#if 0

/*
 * Debug
 */

void uart_putc( unsigned char c ) {
    uart_sendbyte(c);
    if(c=='\r')
        uart_sendbyte('\n');
}

void uart_puts(const char *s )
{
#if HALF_DUPLEX
    uart_set_send(1); // For RS485
#endif
    while (*s) 
      uart_putc(*s++);

    //_delay_ms(20);
#if HALF_DUPLEX
    uart_set_send(0); // For RS485
#endif

}/* uart_puts */


/*************************************************************************
Function: uart_puts_p()
Purpose:  transmit string from program memory to UART
Input:    program memory string to be transmitted
Returns:  none
**************************************************************************/
void uart_puts_p(const prog_char *progmem_s )
{
    register char c;
    
#if HALF_DUPLEX
    uart_set_send(1); // For RS485
#endif
    //while ( (c = pgm_read_byte_far(progmem_s++)) )
    while ( (c = pgm_read_byte(progmem_s++)) )
      uart_putc(c);
#if HALF_DUPLEX
    uart_set_send(0); // For RS485
#endif

}/* uart_puts_p */


void uart_puti( const int val )
{
    char buffer[sizeof(int)*8+1];

    itoa(val, buffer, 10);
    uart_puts( buffer );

}/* uart_puti */

void uart_puthex_nibble(const unsigned char b)
{
    unsigned char  c = b & 0x0f;
    if (c>9) c += 'A'-10;
    else c += '0';
    uart_putc(c);
} /* uart_puthex_nibble */

void uart_puthex_byte(const unsigned char  b)
{
    uart_puthex_nibble(b>>4);
    uart_puthex_nibble(b);
} /* uart_puthex_byte */

#endif


int crc16_calc( const unsigned char *byt, int size )
{
  /* Calculate CRC-16 value; uses The CCITT-16 Polynomial,
     expressed as X^16 + X^12 + X^5 + 1 */

  int crc = (int) 0xffff;
  int index;
  char b;

  for( index=0; index<size; ++index )
    {
      crc ^= (((int) byt[index]) << 8);
      for( b=0; b<8; ++b )
	{
	  if( crc & (int) 0x8000 )
	    crc = (crc << 1) ^ (int) 0x1021;
	  else
	    crc = (crc << 1);
	}
    }
  return crc;
}

/*
void delay_ms(unsigned char ms)
{
    _delay_ms(ms);
}
*/
