#include "ui_lcd.h"
#include "delay.h"
#include "util.h"

#include <compat/deprecated.h>
#include <avr/io.h>
#include <stdlib.h>

#include "runtime_cfg.h"


//#define LCD_4_BIT ( RT_IO_ENABLED( IO_LCD4 ) )
#define LCD_4_BIT 0

// ------------------------------------------------------------------
// 
// Qick and dirty LCD code.
// 
// ------------------------------------------------------------------

#define LCD_IO_DELAY 1

// TODO turn cursor off
// TODO read LCD ready bit, speedup io

#define RS_ENABLE 	do { sbi(PORTE,2); delay_us(LCD_IO_DELAY); } while(0)
#define RS_DISABLE	do { cbi(PORTE,2); delay_us(LCD_IO_DELAY); } while(0)

//#define RS_ENABLE 	do { cbi(PORTE,2); delay_us(LCD_IO_DELAY); } while(0)
//#define RS_DISABLE	do { sbi(PORTE,2); delay_us(LCD_IO_DELAY); } while(0)

#define E_ENABLE 	do { sbi(PORTE,0); delay_us(LCD_IO_DELAY); } while(0)
#define E_DISABLE 	do { cbi(PORTE,0); delay_us(LCD_IO_DELAY); } while(0)

#define RW_READ 	do { sbi(PORTE,1); delay_us(LCD_IO_DELAY); } while(0)
#define RW_WRITE 	do { cbi(PORTE,1); delay_us(LCD_IO_DELAY); } while(0)

#define SET_DDR_OUT     do { (DDRF  = 0xFF); delay_us(LCD_IO_DELAY); } while(0)
#define SET_DDR_IN      do { (DDRF  = 0x00); delay_us(LCD_IO_DELAY); } while(0)

static void lcd_out(uint8_t data);

static void lcd_write( uint8_t data );

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

    //printf("LCD Init ddre=%x...", DDRE );

    RS_DISABLE;
    delay_ms(20);

#if 0
    lcd_write_cmd(0x30);
       //delay_ms(50);

    lcd_write_cmd(0x30);
        //delay_ms(50);

    if( !LCD_4_BIT )
        lcd_write_cmd(0x38);
        //delay_ms(20);
#else
    if( LCD_4_BIT )
    {
        lcd_out(0x3);
        delay_ms(5);
        lcd_out(0x3);
        delay_ms(5);
        lcd_out(0x3);
        delay_ms(5);
        lcd_out(0x2); // 4 bit mode
        delay_ms(5);
        lcd_write_cmd(0x28);   // full set mode
    }
    {
        lcd_out(0x30);
        delay_ms(5);
        lcd_out(0x30);
        delay_ms(5);
        lcd_out(0x38);
        delay_ms(5);
    }
#endif

    //    lcd_write_cmd(0x08);   // полное выключение дисплея
    //    delay_ms(20);

    lcd_write_cmd(0x01);   // clear display
        delay_ms(20);

    lcd_write_cmd(0x06);   // set automatic cursor move to the right
        delay_ms(20);

    //lcd_write_cmd(0x0D);   // включение дисплея
    //    delay_ms(20);

    lcd_write_cmd(0x0F);   // turn display, cursor blink & cursor on
        delay_ms(20);

    //    lcd_write_cmd(0x07);   // scroll

    RS_ENABLE;
        delay_ms(20);

    //printf("LCD init done ddre=%x f=%x\n", DDRE, DDRF );

    // Now clear and write some test

    //lcd_clear();
    //lcd_gotoxy( 0, 0 );    lcd_puts("Init ...");
    //delay_ms(1500);
}


// ------------------------------------------------------------------
// Read
// ------------------------------------------------------------------

static uint8_t lcd_read( void )
{
    uint8_t data;

    PORTF = 0; // assure no pullup

    SET_DDR_IN;
    RW_READ;

    if( LCD_4_BIT )
    {
        E_ENABLE;
        delay_ms(1);
        data = (PINF << 4) & 0xF0u;
        E_DISABLE;
        delay_ms(1);

        E_ENABLE;
        delay_ms(1);
        data |= PINF & 0x0F;
        E_DISABLE;
    }
    else
    {
        E_ENABLE;
        delay_ms(1);
        data = PINF;
        E_DISABLE;
    }

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


// ------------------------------------------------------------------
// Write
// ------------------------------------------------------------------

static void lcd_out(uint8_t data)
{
        PORTF = data;
//delay_ms(2);
        E_ENABLE;
        delay_ms(2);
//printf("w portf=%x, pinf=%x, ddrf=%x\n", PORTF, PINF, DDRF );
        E_DISABLE;
}


static void lcd_write(uint8_t data)
{
    //E_DISABLE; delay_ms(1);

    RW_WRITE;
    PORTF = 0;
    SET_DDR_OUT;

    if( LCD_4_BIT )
    {
        lcd_out(data >> 4);     // NB! First we send upper 4 bits, but on lower 4 bits of port
        delay_ms(1);
        lcd_out(data);     	// Now lower 4 bits
    }
    else
    {
        lcd_out(data);
    }

}

static void lcd_write_data(uint8_t data)
{
    RS_ENABLE;
//delay_ms(2);
    lcd_write(data);
//delay_ms(50);
    //printf( "LCD data cmd In=%d\n", lcd_read_cmd() );
//    delay_ms(2);
    //printf( "LCD cmd In=%d\n", lcd_read_cmd() );


}

static void lcd_write_cmd(uint8_t cmd)
{
    RS_DISABLE;
//delay_ms(2);
    lcd_write(cmd); 
//delay_ms(50);
    //printf( "LCD cmd cmd In=%d\n", lcd_read_cmd() );
//    delay_ms(2);
    //printf( "LCD cmd In=%d\n", lcd_read_cmd() );



    //RS_ENABLE;
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

void lcd_putx( char c )
{
    lcd_puthex( &c, 1 );
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

void lcd_clear_line( uint8_t line )
{
    lcd_gotoxy( 0, line );

    uint8_t i;

    for( i = 0; i < 20; i++ )
        lcd_putc(' ');
}


