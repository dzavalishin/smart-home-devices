/**
 *
 * DZ-MMNET-WALL: Wall control panel based on MMNet01.
 *
 * HW PWM to control LCD backlight.
 *
**/


#define DEBUG 0

#include "defs.h"
#include "runtime_cfg.h"
#include "servant.h"

#include "io_pwm.h"


#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include <sys/event.h>



#if SERVANT_NPWM > 0




/*
// TIMER1 has overflowed
ISR(TIMER1_OVF_vect)
{
    cli();

    sei();
}
*/


void set_lcd_brightness( unsigned char level )
{
    OCR2 = level;
    printf("set_an %d\n", level );
}



#endif // SERVANT_NPWM > 0

// PWM2 on PB7



static void pwm_init_dev( dev_major* d )
{
    (void) d;
    printf("pwm_init_dev\n");
    //timer1_init();

    //TCCR2 = (1 << COM21) | (1 << PWM2) | (1 << CS20);
    TCCR2 = (1 << COM21) | (1 << WGM20) | (1 << CS20);

}
static void pwm_start_dev( dev_major* d )
{
    (void) d;
    printf("pwm_start_dev\n");
    //timer1_start();
}

//static void pwd_stop_dev( dev_major* d ) { (void) d;  } // TODO


dev_major io_pwm =
{
    .name = "pwm",

    .init = pwm_init_dev,
    .start = pwm_start_dev,
    .stop = 0, // TODO
    .timer = 0,

    .to_string = 0,
    .from_string = 0,

    .minor_count = SERVANT_NPWM,
    .subdev = 0,
};



