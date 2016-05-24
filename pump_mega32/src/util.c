//#include "crumb_defs.h"
#include "defs.h"
#include "util.h"

//#include <util/delay.h>
#include "delay.h"
#include <avr/io.h>

#define _delay_ms delay_ms

// --------------------------------------------------------------------------
// Formatting
// --------------------------------------------------------------------------

char hexdigit( unsigned char d )
{
    d &= 0xF;

    if( d < 0xa )
        return '0' + d;

    return 'A'+d-0xa;
}

void hex_encode( unsigned char *dest, unsigned char src )
{
    dest[0] = hexdigit( (src) >> 4 );
    dest[1] = hexdigit( src );
}


void lcd_put_temp( uint16_t temp )
{
    if( (temp >> 4) < 10 ) lcd_putc(' ');
    lcd_puti( temp >> 4 );
    lcd_putc( '.' );
    switch( (temp >> 2) & 3 )
    {
    case 0:        lcd_puts("0 "); break;
    case 1:        lcd_puts("25"); break;
    case 2:        lcd_puts("5 "); break;
    case 3:        lcd_puts("75"); break;
    }
}

// fixed *100
void lcd_putf( int f )
{
    double d = (unsigned)f;
    d /= 100;

    buf[6];
    snprintf( buf, sizeof(buf), "%5.2f", d );
    lcd_puts(buf);
}



uint8_t nibble_from_hex( uint8_t h )
{
    if( h >= '0' && h <= '9' ) return h - '0';
    if( h >= 'a' && h <= 'f' ) return h - 'a' + 10;
    if( h >= 'A' && h <= 'F' ) return h - 'A' + 10;

    return 0;
}

uint8_t fromhex( unsigned char *src )
{
    return ( nibble_from_hex( src[0] ) << 4 ) | nibble_from_hex( src[1] );
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


// --------------------------------------------------------------------------
// LED
// --------------------------------------------------------------------------


/* flash onboard LED */
void flash_led(uint8_t count)
{
    uint8_t i;
    for (i = count; i > 0; --i)
        flash_led_once();
}


void flash_led_once()
{

    LED1_PORT &= ~_BV(LED1);
    delay_thsec();
    LED1_PORT |= _BV(LED1);
    delay_esec();
}

void flash_led_long()
{
    LED1_PORT |= _BV(LED1);
    delay_sec();
    LED1_PORT &= ~_BV(LED1);
    delay_sec();
    LED1_PORT |= _BV(LED1);
    delay_sec();
}

void turn_led_on()
{
    LED1_PORT &= ~_BV(LED1);
}


void turn_led_off()
{
    LED1_PORT |= _BV(LED1);
}

/* flash onboard LED */
void flash_led2(uint8_t count)
{
    uint8_t i;
    for (i = count; i > 0; --i)
        flash_led2_once();
}


void flash_led2_once()
{

    LED2_PORT &= ~_BV(LED2);
    delay_thsec();
    LED2_PORT |= _BV(LED2);
    delay_esec();
}

void turn_led2_on()
{
    LED2_PORT &= ~_BV(LED2);
}


void turn_led2_off()
{
    LED2_PORT |= _BV(LED2);
}
