/**
 *
 * DZ-MMNET-CHARGER: Modbus/TCP I/O module based on MMNet101.
 *
 * LCD: SPI data exchange queue.
 *
 * We communicate with LCD through SPI serial IO. So we need to convert usual
 * port bit-bang IO to serial port IO adding queue to support easy thread switch.
 *
 * Queue contains words with control (register select in lower bit) byte (upper 8 bits)
 * and data byte (low 8 bits).
 *
**/

#include "ui_lcd_queue.h"

#include <avr/interrupt.h>

#include <sys/timer.h>


#define LCD_Q_SIZE 16

static uint16_t lcd_queue[LCD_Q_SIZE];

static uint8_t lcd_q_get_pos = 0;
static uint8_t lcd_q_put_pos = 0;


void lcd_q_put( uint16_t data )
{
    cli();

    // Wait for output to drain
    while( lcd_q_full() )
    {
        sei();
        NutSleep(1);
        cli();
    }

    lcd_queue[ lcd_q_put_pos++ ] = data;

    sei();
}



uint16_t lcd_q_get( void )
{
    uint16_t rc = 0;

    cli();

    if( lcd_q_get_pos != lcd_q_put_pos )
    {
        rc = lcd_queue[lcd_q_get_pos++];
        if( lcd_q_get_pos == lcd_q_put_pos )
        {
            lcd_q_get_pos = 0;
            lcd_q_put_pos = 0;
        }
    }

    sei();

    return rc;
}

uint8_t lcd_q_empty( void )
{
    cli();
    uint8_t rc = lcd_q_get_pos == lcd_q_put_pos;
    sei();
    return rc;
}

uint8_t lcd_q_full( void )
{
    return lcd_q_put_pos >= LCD_Q_SIZE;
}

