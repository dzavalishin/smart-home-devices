#include <avr/io.h>
#include <avr/interrupt.h>

#include <stdlib.h>

#include "main.h"

volatile unsigned int elapsed_cycles = 0;
volatile unsigned char red, green, blue;

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

    // set outputs to PWM
    TCCR0A |= (1 << COM0A1);
    TCCR1A |= (1 << COM1A1);
    TCCR1A |= (1 << COM1B1);

    // initial PWM duty cycle
    OCR0A = 0;
    OCR1A = 0;
    OCR1B = 0;

    // overflow interrupt setup
    //TIMSK |= (1 << TOIE0);
    sei();
}


ISR(TIMER0_OVF_vect)
{
    elapsed_cycles++;

    if (elapsed_cycles == 10000)
    {
        red = rand() / (RAND_MAX / 0xff + 1);
        green = rand() / (RAND_MAX / 0xff + 1);
        blue = rand() / (RAND_MAX / 0xff + 1);

        elapsed_cycles = 0;
    }

    OCR0A = red;
    OCR1A = green;
    OCR1B = blue;
}


