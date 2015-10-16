/**
 *
 * DZ-MMNET-MODBUS: Modbus/TCP I/O module based on MMNet101.
 *
 * Software PWM.
 *
 * Multiple-output PWM using one HW timer.
 *
**/

// можно использовать встроенные PWM, но их мало
// и они занимают таймеры
// для большего числа аналоговых выводов
// PWM можно реализовать на одном таймере


#include "defs.h"
#include "runtime_cfg.h"
#include "servant.h"

#include "io_pwm.h"


#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <sys/event.h>


#if SERVANT_NPWM > 0

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

//static HANDLE   endOfPwmCycle;


//TIMER1 initialize - prescale:1 !!! для максимальной скорости PWM-ов
// WGM: 0) Normal, TOP=0xFFFF
// desired value: 1000uSec
// actual value: 1000,000uSec (0,0%)
void timer1_init(void)
{
    // Init anyway, just do not start, if turned off
    //if(!RT_IO_ENABLED(IO_PWM)) return;

    TCCR1B = 0x00; //stop
    TCNT1H = 0xF0; //setup
    TCNT1L = 0x60;
    OCR1AH = 0x01;
    OCR1AL = 0x90;
    OCR1BH = 0x01;
    OCR1BL = 0x90;
    ICR1H  = 0x01;
    ICR1L  = 0x90;
    TCCR1A = 0x00;
    // TCCR1B = 0x01; //start Timer later

    pwm_count = 0;
    pwm_mask_byte = 0b00000000;
}

void timer1_start(void)
{
    if(!RT_IO_ENABLED(IO_PWM)) return;

    TIMSK |= _BV(TOIE1);
    TCCR1B = 0x01; // start Timer1
}




// TIMER1 has overflowed
ISR(TIMER1_OVF_vect)
{
    uint8_t mask;

    cli();

    // No active outputs
    if( pwm_mask_byte == 0 )
    {
        // Do not turn on PWMs, wait 100 uSec
        TCNT1H = 0xF0;
        TCNT1L = 0x60;
        sei();
        return;
    }

    pwm_count++; // для всех PWM

    if( pwm_count >= SERVANT_NPWM+1 )
    {
        pwm_count = 0;	// начало цикла PWM
        mask = pwm_mask_byte & OUTS_MASK; // не включать неактивные PWM-ы
        SERVANT_PWM_PORT |= mask;	// включить все выходы, 0-3
    }


    // выключить каждый PWM последовательно по возрастанию значения
    if( pwm_count > 0 )
    {
        uint8_t j = pwm_order[pwm_count-1];
        if( (pwm_mask_byte & _BV(j+SERVANT_PWM_BIT)) != 0 )
        {
            SERVANT_PWM_PORT &= ~_BV(j+SERVANT_PWM_BIT);
        }
    }

    TCNT1H = pwm_time[pwm_count]>>8;
    TCNT1L = pwm_time[pwm_count];

//    if( pwm_count >= SRVANT_NPWM+1 )
//        NutEventPostFromIrq(&endOfPwmCycle);

    sei();
}


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

    for( i = 0; i < SERVANT_NPWM; i++ )
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

    // TODO wait for cycle end or stop/restart timer
    //NutEventBroadcast( &endOfPwmCycle ); // reset event if any
    //NutEventWait( &endOfPwmCycle );

    // на периоде модуляции последовательно запускаем таймер между вылючениями каждого выхода PWM
    pwm_time[0] = pwm_calc_time( pwm_sort[0] );
    for( i = 1; i < SERVANT_NPWM; i++ )
    {
        pwm_time[i] = pwm_calc_time( pwm_sort[i] - pwm_sort[i-1] );
        // период модуляции разделяем на 256*(PWM_SPEED=10) машинных циклов
    }

    // после выкдючения всех PWM подождать до окончания периода
    pwm_time[SERVANT_NPWM] = pwm_calc_time( 0xff - pwm_sort[SERVANT_NPWM-1] ); // от последнего импульса до конца цикла

    // Enable corresponding PWM channel
    pwm_mask_byte |= _BV(port_num+SERVANT_PWM_BIT);

}


#endif // SERVANT_NPWM > 0

