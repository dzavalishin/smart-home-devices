/**
 *
 * DZ-MMNET-MODBUS: Modbus/TCP I/O module based on MMNet101.
 *
 * Software PWM.
 *
 * Multiple-output PWM using one HW timer.
 *
**/

#include "defs.h"
#include "runtime_cfg.h"
#include "servant.h"

#include "io_pwm.h"


#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#include <sys/event.h>


#define DEBUG 0

#if SERVANT_NPWM > 0


#if SERVANT_NPWM != 2
#  error This implementation supports just 2 hw PWM outputs
#endif

// TODO KILLME
void set_an(unsigned char port_num, unsigned char data)
{
}





static void pwm_init_dev( dev_major* d )
{
    (void) d;

}

static int8_t pwm_start_dev( dev_major* d )
{
    (void) d;

    return -1;
}

// TODO
static void pwd_stop_dev( dev_major* d )
{
    (void) d;

}


#endif // SERVANT_NPWM > 0



dev_major io_pwm =
{
    .name = "pwm",

#if SERVANT_NPWM > 0
    .init	= pwm_init_dev,
    .start	= pwm_start_dev,
    .stop	= pwd_stop_dev, // TODO
    .timer	= 0,
#endif

    .started	= 0,

    .to_string 	 = 0,
    .from_string = 0,

    .minor_count = SERVANT_NPWM,
    .subdev 	 = 0,
};






















#if 0
// можно использовать встроенные PWM, но их мало
// и они занимают таймеры
// для большего числа аналоговых выводов
// PWM можно реализовать на одном таймере

#define PWM_WAIT_CYCLE_START 0


#if ((SERVANT_NPWM) < 0) || ((SERVANT_NPWM) > 4 )
#error SERVANT_NPWM must be 0..4
#endif

#if ((SERVANT_PWM_BIT) < 0) || ((SERVANT_PWM_BIT) > 7 )
#error SERVANT_PWM_BIT must be 0..7
#endif

#if ((SERVANT_PWM_BIT) + (SERVANT_NPWM)) > 8
#error SERVANT_NPWM + SERVANT_PWM_BIT must be 0..7
#endif


#define OUTS_MASK ( ( 0x0Fu >> (4-(SERVANT_NPWM)) ) << SERVANT_PWM_BIT)

unsigned char 	pwm_mask_byte;          // Active channels
unsigned char 	pwm_count;
unsigned char 	pwm[SERVANT_NPWM];
unsigned char 	pwm_sort[SERVANT_NPWM], pwm_order[SERVANT_NPWM];
unsigned int 	pwm_time[SERVANT_NPWM+1];

#if PWM_WAIT_CYCLE_START
static HANDLE   endOfPwmCycle;
#endif // PWM_WAIT_CYCLE_START



#define BIT_FOR_OUT(___out) (_BV((___out)+SERVANT_PWM_BIT))
#define SET_TO_1(___out) (SERVANT_PWM_PORT |= BIT_FOR_OUT(___out))
#define SET_TO_0(___out) (SERVANT_PWM_PORT &= ~BIT_FOR_OUT(___out))




static inline uint16_t pwm_calc_time(uint8_t value)
{
    return ~( value * SERVANT_PWM_SPEED );
}

void set_an(unsigned char port_num, unsigned char data)
{
    unsigned char i, j, min; //, s;

    if(!RT_IO_ENABLED(IO_PWM)) return;

    // Check port number
    if( port_num >= SERVANT_NPWM ) return;

    pwm[ port_num ] = data;

    // сортировка значений PWM-ов, запоминаем порядок
    for( i = 0; i < SERVANT_NPWM; i++)
    {
        pwm_sort[i] = pwm[i];
        pwm_order[i] = i;
    }

    for( i = 0; i < SERVANT_NPWM-1; i++ )
    {
        min = i;
        for( j = i+1; j < SERVANT_NPWM; j++ )
        {
            if( pwm_sort[j] < pwm_sort[min] )
            {
                min = j;
            }
        }
        uint8_t s = pwm_sort[i];
        pwm_sort[i] = pwm_sort[min];
        pwm_sort[min] = s;

        if(min != i)
        {
            pwm_order[min] = pwm_order[i]; // запоминаем порядок
            pwm_order[i] = min;
        }
    }

#if PWM_WAIT_CYCLE_START
    // TODO wait for cycle end or stop/restart timer
    NutEventBroadcast( &endOfPwmCycle ); // reset event if any, wait for _next_ cycle
    NutEventWait( &endOfPwmCycle );
#endif // PWM_WAIT_CYCLE_START

    // период модуляции разделяем на 256*(PWM_SPEED=10) машинных циклов
    // на периоде модуляции последовательно запускаем таймер между вылючениями каждого выхода PWM
    pwm_time[0] = pwm_calc_time( pwm_sort[0] );
    for( i = 1; i < SERVANT_NPWM; i++ )
    {
        pwm_time[i] = pwm_calc_time( pwm_sort[i] - pwm_sort[i-1] );
    }

    // после выкдючения всех PWM подождать до окончания периода
    pwm_time[SERVANT_NPWM] = pwm_calc_time( 0xff - pwm_sort[SERVANT_NPWM-1] ); // от последнего импульса до конца цикла

    // Enable corresponding PWM channel, or set out for full min/max
    pwm_mask_byte |= BIT_FOR_OUT(port_num); //_BV(port_num+SERVANT_PWM_BIT);
    if( data == 255 )
    {
        cli();
        pwm_mask_byte &= ~BIT_FOR_OUT(port_num);
        SET_TO_1(port_num);
        sei();
    }
    if( data == 0 )
    {
        cli();
        pwm_mask_byte &= ~BIT_FOR_OUT(port_num);
        SET_TO_0(port_num);
        sei();
    }

#if DEBUG
    for( i = 0; i < SERVANT_NPWM+1; i++ )
    {
        printf("pwm_time[%d] = %u (%u)\n", i, pwm_time[i], ~pwm_time[i] );
    }

    for( i = 0; i < SERVANT_NPWM; i++ )
    {
        printf("pwm[%d] = %u\t", i, pwm[i] );
        printf("pwm_order[%d] = %u\n", i, pwm_order[i] );
    }
#endif
}

void
pwm_set_default_output_state(void)
{
    uint8_t i;
    for( i = 0; i < SERVANT_NPWM+1; i++ )
    {
        set_an( i, 0 );
    }
}



#endif

