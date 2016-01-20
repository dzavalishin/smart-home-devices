#include "defs.h"

#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdlib.h>

#include "main.h"

volatile unsigned int elapsed_cycles = 0;
//volatile unsigned char red, green, blue;

volatile uint8_t main_pwm[4] = { 0, 0, 0, 0 };


void init_pwm(void)
{
    // init timers as fast PWM
    TCCR0A = (1 << WGM00) | (1 << WGM01);
    TCCR1A = (1 << WGM10) | (1 << WGM12);

    // set prescaler to 1
    TCCR0B |= (1 << CS00);
    TCCR1B |= (1 << CS00);

    // set ports to output
    DDRB |= (1 << PB2);
    DDRB |= (1 << PB3);
    DDRB |= (1 << PB4);

    DDRD |= (1 << PD5);

    // set outputs to PWM
    TCCR0A |= (1 << COM0A1);
    TCCR0A |= (1 << COM0B1);

    TCCR1A |= (1 << COM1A1);
    TCCR1A |= (1 << COM1B1);

    // initial PWM duty cycle
    OCR0A = 0;
    OCR1A = 0;
    OCR0B = 0;
    OCR1B = 0;

    // overflow interrupt setup
    TIMSK |= (1 << TOIE0);
}


ISR(TIMER0_OVF_vect)
{
    elapsed_cycles++;

    if (elapsed_cycles == 20000)
    {
        main_pwm[0]	= rand() / (RAND_MAX / 0xff + 1);
        main_pwm[1]	= rand() / (RAND_MAX / 0xff + 1);
        main_pwm[2]	= rand() / (RAND_MAX / 0xff + 1);
        main_pwm[3]	= rand() / (RAND_MAX / 0xff + 1);

		usart_pwm	= rand() / (RAND_MAX / 0xff + 1);

        elapsed_cycles = 0;

		activity++;
    }

    OCR0A = main_pwm[2];
	OCR0B = main_pwm[3];

    OCR1A = main_pwm[1];
    OCR1B = main_pwm[0];
}


