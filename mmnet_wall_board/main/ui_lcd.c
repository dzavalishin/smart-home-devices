#include "ui_lcd.h"
#include "delay.h"
#include "util.h"

#include <compat/deprecated.h>
#include <avr/io.h>
#include <stdlib.h>


// ------------------------------------------------------------------
// 
// Qick and dirty LCD code.
// 
// ------------------------------------------------------------------

// TODO turn cursor off
// TODO read LCD ready bit, speedup io

#define RS_ENABLE 	sbi(PORTE,2)
#define RS_DISABLE	cbi(PORTE,2)

#define E_ENABLE 	sbi(PORTE,0)
#define E_DISABLE 	cbi(PORTE,0)

#define RW_READ 	sbi(PORTE,1)
#define RW_WRITE 	cbi(PORTE,1)

#define SET_DDR_OUT     (DDRF  = 0xFF)
#define SET_DDR_IN      (DDRF  = 0x00)


static void lcd_write_data( uint8_t data );
static void lcd_write_cmd(  uint8_t cmd  );

static uint8_t lcd_read_data( void );
static uint8_t lcd_read_cmd( void );

// ------------------------------------------------------------------
// Init
// ------------------------------------------------------------------


void lcd_init(void)
{
    // IO setup
    PORTE &= ~0x07;
    DDRE  |=  0x07; // bits 0-2

    RW_WRITE;

    PORTF = 0x00;
    DDRF  = 0xFF;

    printf("LCD Init ddre=%x...", DDRE );

    RS_DISABLE;
    delay_ms(20);

    lcd_write_data(0x30);
    //    delay_ms(50);

    lcd_write_data(0x30);
    //    delay_ms(50);

    lcd_write_data(0x38);
    //    delay_ms(20);


    //    lcd_write_data(0x08);   // полное выключение дисплея
    //    delay_ms(20);

    lcd_write_data(0x01);   // clear display
    //    delay_ms(20);

    lcd_write_data(0x06);   // set automatic cursor move to the right
    //    delay_ms(20);

    lcd_write_data(0x0D);   // включение дисплея
    //    delay_ms(20);

    lcd_write_data(0x0F);   // turn display, cursor blink & cursor on
    //    delay_ms(20);

    //    lcd_write_data(0x07);   // scroll

    RS_ENABLE;

    puts(" done");

    // Now clear and write some test

    //lcd_clear();
    lcd_puts("Init ...");
    delay_ms(150);
}


// ------------------------------------------------------------------
// Internal
// ------------------------------------------------------------------

static uint8_t lcd_read( void )
{
    uint8_t data;

    SET_DDR_IN;
    RW_READ;

    E_ENABLE;
    delay_ms(1);
    data = PINF;
    E_DISABLE;

    RW_WRITE;
    SET_DDR_OUT;

    return data;
}

static uint8_t lcd_read_data( void )
{
    RS_ENABLE;
    return lcd_read();
}

static uint8_t lcd_read_cmd( void )
{
    RS_DISABLE;
    return lcd_read();
}



static void lcd_write(uint8_t data)
{
    RW_WRITE;
    SET_DDR_OUT;

    PORTF = data;
delay_ms(2);
    E_ENABLE;
    delay_ms(2); 
    E_DISABLE;
    printf( "LCD cmd In=%d\n", lcd_read_cmd() );
    delay_ms(2);
    //printf( "LCD cmd In=%d\n", lcd_read_cmd() );

delay_ms(50);
}

static void lcd_write_data(uint8_t data)
{
    RS_ENABLE;
    lcd_write(data);
}

static void lcd_write_cmd(uint8_t cmd)
{
    RS_DISABLE;
    lcd_write(cmd); 
    RS_ENABLE;
}

// ------------------------------------------------------------------
// Output
// ------------------------------------------------------------------


void lcd_putc(char c) { lcd_write_data(c); }


void lcd_puts( const char* str )
{
    while(*str)
    {
        //	if( *str < ' ' )			lcd_set_cursor(1,0);	else
        lcd_write_data(*str++);
    }
}

void lcd_puti(int i)
{
    char buf[20];
    lcd_puts(itoa(i,buf,10));
}



void lcd_puthex_byte(const unsigned char  b)
{
    lcd_putc(hexdigit(b>>4));
    lcd_putc(hexdigit(b));
}

void lcd_puthex(const char *bp, int cnt )
{
    while( cnt-- > 0 )
    {
        lcd_puthex_byte( *bp++ );
    }
}


// ------------------------------------------------------------------
// Control
// ------------------------------------------------------------------


void lcd_gotoxy( int pos, int line )
{
    pos %= 20;

    switch(line)
    {
    default:
    case 0:	break;

    case 1:	pos += 0x40; break;
    case 2:	pos += 0x14; break;
    case 3:	pos += 0x54; break;
    }

    pos |= 0b10000000;
    lcd_write_cmd(pos);
}

void lcd_clear( void )
{
    lcd_write_cmd(0x01);
}



