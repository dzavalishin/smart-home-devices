/**
 *
 * DZ-MMNET-WALL: Wall control panel based on MMNet01.
 *
 * Misc code - LED, delays.
 *
**/

#include "util.h"
#include "defs.h"

#include "ui_lcd.h"


#include <sys/timer.h>

// --------------------------------------------------------------------------
// LED
// --------------------------------------------------------------------------


void led_ddr_init(void)
{
    LED_DDR |= _BV(LED);
#ifdef FAIL_LED_EXCLPOS
    FAIL_LED_DDR |= _BV(FAIL_LED);
#endif
    LED_OFF;
}


void fail_led(void)
{
#ifdef FAIL_LED_EXCLPOS
    /* set LED pin as output */
    FAIL_LED_DDR |= _BV(FAIL_LED);

    FAIL_LED_PORT &= ~_BV(FAIL_LED);

    //delay_thsec();
    //FAIL_LED_PORT |= _BV(FAIL_LED);
    //delay_esec();
#endif
}


void flash_led_once(void)
{
    /* set LED pin as output */
    //LED_DDR |= _BV(LED);

    LED_ON;
    delay_thsec();
    LED_OFF;
    delay_esec();
}

// --------------------------------------------------------------------------
// LCD
// --------------------------------------------------------------------------


void lcd_status_line(const char *msg)
{
    lcd_gotoxy( 0, 3 );
    lcd_puts("Init ");
    lcd_puts( msg );
    lcd_puts("          ");
    lcd_gotoxy( 0, 0 );
}


// --------------------------------------------------------------------------
// Delays
// --------------------------------------------------------------------------

void delay_halfsec(void)
{
    NutSleep(500);
}

void delay_sec(void) { delay_halfsec(); delay_halfsec(); }

// 1/4 sec
void delay_qsec(void)
{
    NutSleep(250);
}

// 1/8 sec
void delay_esec(void)
{
    NutSleep(125);
}

// ~1/16 sec
void delay_thsec(void)
{
    NutSleep(62);
}
