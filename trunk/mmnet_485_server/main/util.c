#include "util.h"
#include "defs.h"

#include <sys/timer.h>

// --------------------------------------------------------------------------
// LED
// --------------------------------------------------------------------------


void led_ddr_init(void)
{
    LED_DDR |= _BV(LED);
//    FAIL_LED_DDR |= _BV(FAIL_LED);
    LED_OFF;
}


void fail_led(void)
{
    /* set LED pin as output */
    //FAIL_LED_DDR |= _BV(FAIL_LED);

//    FAIL_LED_PORT &= ~_BV(FAIL_LED);

    //delay_thsec();
    //FAIL_LED_PORT |= _BV(FAIL_LED);
    //delay_esec();
}


void flash_led_once(void)
{
    /* set LED pin as output */
    //LED_DDR |= _BV(LED);
    //puts("fl led once");
    LED_ON;
    //puts("fl led once aftn on");
    delay_thsec();
    LED_OFF;
    delay_esec();

    //puts("fl led once aftn delays");
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
