#include <stdlib.h>

#include "uart.h"
#include "util.h"

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

    //delay_ms(20);
#if HALF_DUPLEX
    uart_set_send(0); // For RS485
#endif

}/* uart_puts */


/*************************************************************************
 Function: uart_puts_p()
 Purpose:  transmit string from program memory to UART
 Input:    program memory string to be transmitted
 Returns:  none
 ************************************************************************** /
 void uart_puts_p(const prog_char *progmem_s )
 {
 register char c;

 while ( (c = pgm_read_byte_far(progmem_s++)) )
 uart_putc(c);

 }
 */


void uart_puti( const int val )
{
    char buffer[sizeof(int)*8+1];

    itoa(val, buffer, 10);
    uart_puts( buffer );

}/* uart_puti */

#if 0
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


void uart_puthex_byte(const unsigned char  b)
{
    uart_putc(hexdigit(b>>4));
    uart_putc(hexdigit(b));
}
