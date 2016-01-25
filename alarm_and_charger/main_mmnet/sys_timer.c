#if 0


/**
 *
 * DZ-MMNET-CHARGER: Acc charger module based on MMNet101.
 *
 * System timer - callouts, etc.
 *
**/

#include "defs.h"
#include "runtime_cfg.h"
#include "servant.h"


//TIMER1 initialize - prescale:1
// WGM: 0) Normal, TOP=0xFFFF
// desired value: 1000uSec
// actual value: 1000,000uSec (0,0%)
void system_timer_init(void)
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

void system_timer_start(void)
{
    TIMSK |= _BV(TOIE1);
    TCCR1B = 0x01; // start Timer1
}

// TIMER1 has overflowed
ISR(TIMER1_OVF_vect)
{
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

    // TODO don't turn on channels with level = 0
again:
    pwm_count++; // для всех PWM

    if( pwm_count >= SERVANT_NPWM+1 )
    {
        pwm_count = 0;	// начало цикла PWM
        uint8_t mask = pwm_mask_byte & OUTS_MASK; // не включать неактивные PWM-ы
        SERVANT_PWM_PORT |= mask;	// включить все выходы, 0-3
    }


    // выключить каждый PWM последовательно по возрастанию значения
    if( pwm_count > 0 )
    {
        uint8_t j = pwm_order[pwm_count-1];
        uint8_t setmask = BIT_FOR_OUT(j); //_BV(j+SERVANT_PWM_BIT);

        if( pwm_mask_byte & setmask )
            SERVANT_PWM_PORT &= ~setmask;
    }

#if PWM_WAIT_CYCLE_START
    if( pwm_count >= SERVANT_NPWM )
        NutEventPostFromIrq(&endOfPwmCycle);
#endif // PWM_WAIT_CYCLE_START

    //if( (pwm_time[pwm_count] >= 65535) && (pwm_count < SERVANT_NPWM+1)  )
    if( pwm_time[pwm_count] >= 65535 )
        goto again;

    TCNT1H = pwm_time[pwm_count] >> 8;
    TCNT1L = pwm_time[pwm_count];

    sei();
}


#endif
