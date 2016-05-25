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

#define RS_ENABLE 	sbi(PORTD,6)
#define RS_DISABLE	cbi(PORTD,6)

#define E_ENABLE 	sbi(PORTD,7)
#define E_DISABLE 	cbi(PORTD,7)

#define RW_WRITE 	cbi(PORTD,5)

static void lcd_write_data( int data );
static void lcd_write_cmd(  int cmd  );


// ------------------------------------------------------------------
// Init
// ------------------------------------------------------------------


void lcd_init(void)
{
    // настройка порта
    PORTD=0x00;
    DDRD=0xFC;

    PORTC=0x00;
    DDRC=0xFF;

    RW_WRITE;

    RS_DISABLE;  // rs = 0
    delay_ms(20);

    lcd_write_data(0x30);
    //    delay_ms(50);

    lcd_write_data(0x30);
    //    delay_ms(50);

    lcd_write_data(0x38);
    //    delay_ms(20);


    //    lcd_write_data(0x08);   // полное выключение дисплея
    //    delay_ms(20);

    lcd_write_data(0x01);   // очистка дисплея
    //    delay_ms(20);

    lcd_write_data(0x06);   // сдвиг курсора вправо
    //    delay_ms(20);

    lcd_write_data(0x0D);   // включение дисплея
    //    delay_ms(20);

    lcd_write_data(0x0F);   // включение дисплея
    //    delay_ms(20);

    //    lcd_write_data(0x07);   // scroll

    RS_ENABLE;  //rs = 1
}


// ------------------------------------------------------------------
// Internal
// ------------------------------------------------------------------

void lcd_write_data(int data)
{
    PORTC = data;

    E_ENABLE;
    delay_ms(2);
    E_DISABLE;
    delay_ms(2);
}


void lcd_write_cmd(int cmd)
{
    RS_DISABLE;
    lcd_write_data(cmd);
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
    if (line == 1)
        pos += 0x40;

    pos |= 0b10000000;
    lcd_write_cmd(pos);
}

void lcd_clear()
{
    lcd_write_cmd(0x01);
}



