#include "freq_defs.h"
#include "pwm.h"
#include "defs.h"


#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>



#if SERVANT_NPWM > 0

unsigned char pwm_count;
unsigned char pwm[SERVANT_NPWM];
unsigned char pwm_sort[SERVANT_NPWM], pwm_order[SERVANT_NPWM];
unsigned char pwm_mask_byte;
unsigned int pwm_time[SERVANT_NPWM];


//TIMER1 initialize - prescale:1 !!! для максимальной скорости PWM-ов
// WGM: 0) Normal, TOP=0xFFFF
// desired value: 1000uSec
// actual value: 1000,000uSec (0,0%)
void timer1_init(void)
{
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
    // TCCR1B = 0x01; //start Timer позже

    pwm_count=0;
    pwm_mask_byte=0b00000000;
}

void timer1_start(void)
{
    TIMSK |= _BV(TOIE1);

    TCCR1B = 0x01; //start Timer1
}

//#pragma interrupt_handler timer1_ovf_isr:15
//void timer1_ovf_isr(void)
ISR(TIMER1_OVF_vect)
{
    // реализация нескольких PWM-ов на 1 таймере
    unsigned char j, mask;
    cli();   // запертить прерывания
    // TIMER1 has overflowed

    if (pwm_mask_byte>0) {

        pwm_count++; // для всех PWM

        if (pwm_count==SERVANT_NPWM+1) {
            pwm_count=0;	// начало цикла PWM
            mask=pwm_mask_byte&0x0f; // не включать неактивные PWM-ы
            PORTC|=mask;	// включить все выходы, PC0-3; изменить, если изменятся выводы аналоговых выходов
        }

        j=pwm_order[pwm_count-1];
        // выключить каждый PWM последовательно по возрастанию значения
        if (pwm_count!=0)
        {
            if ((pwm_mask_byte & _BV(j))!=0)
            {
                PORTC &= ~_BV(j);
            }
        }

        TCNT1H=pwm_time[pwm_count]>>8;
        TCNT1L=pwm_time[pwm_count];
    }
    else {
        // PWM-ы не включать, ждать 100 мкс.
        TCNT1H = 0xF0; //setup
        TCNT1L = 0x60;
    }
    sei();   // разрешить прерывания
}


void set_an(unsigned char port_num, unsigned char data) {
    // можно использовать встроенные PWM, но их мало
    // и они занимают таймеры
    // для большего числа аналоговых выводов
    // PWM можно реализовать на одном таймере
    unsigned char i, j, min, s;

    pwm[port_num]=data;

    // сортировка значений PWM-ов, запоминаем порядок
    for (i=0; i<SERVANT_NPWM; i++) {
        pwm_sort[i]=pwm[i];
        pwm_order[i]=i;
    }

    for (i=0; i<SERVANT_NPWM; i++) {
        min=i;
        for (j=i+1; j<SERVANT_NPWM; j++) {
            if (pwm_sort[j]<pwm_sort[min]) {
                min=j;
            }
        }
        s=pwm_sort[i];
        pwm_sort[i]=pwm_sort[min];
        pwm_sort[min]=s;

        if (min!=i) {
            pwm_order[min]=pwm_order[i]; // запоминаем порядок
            pwm_order[i]=min;
        }
    }

    // на периоде модуляции последовательно запускаем таймер
    // между вылючениями каждого выхода PWM
    pwm_time[0]=~(pwm_sort[0]*10);
    for (i=1; i<SERVANT_NPWM; i++) {
        pwm_time[i]=~((pwm_sort[i]-pwm_sort[i-1])*SERVANT_PWM_SPEED);
        // период модуляции разделяем на 256*(PWM_SPEED=10) машинных циклов
    }
    // после выкдючения всех PWM подождать до окончания периода
    pwm_time[SERVANT_NPWM]=~((0xff-pwm_sort[SERVANT_NPWM-1])*SERVANT_PWM_SPEED); // от последнего импульса до конца цикла
}


#endif // SERVANT_NPWM > 0

