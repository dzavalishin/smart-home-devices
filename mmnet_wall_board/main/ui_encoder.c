/**
 *
 * DZ-MMNET-WALL: Wall control panel based on MMNet01.
 *
 * Encoder using interrupts.
 *
**/

#include "defs.h"
#include "util.h"
#include "runtime_cfg.h"

#include "ui_menu.h"


#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <dev/irqreg.h>


// 0xD0; // Bits we use to connect encoder
#define ENCODER_BITS 0b11010000


static void EncoderInterrupt(void *arg);

void encoder_init( void )
{
    // in menu code
    //ENCODER_DDR &= ~ENCODER_BITS; // Set as input
    //ENCODER_PORT |= ENCODER_BITS; // Pull up

    // Enable interrupts on encoder A/B pins - any change of state

    /* Register interrupt handler and enable interrupts. */

    if( NutRegisterIrqHandler(&sig_INTERRUPT6, EncoderInterrupt, 0) )
        return; // TODO report err

    if( NutRegisterIrqHandler(&sig_INTERRUPT7, EncoderInterrupt, 0) )
        return; // TODO report err

    EICRB |= _BV(ISC70) | _BV(ISC60);
    EICRB &= ~( _BV(ISC71) | _BV(ISC61) );

    EIMSK |= 0b11000000; // enable 6 & 7

}



static void EncoderInterrupt(void *arg)
{
    (void) arg;

    static uint8_t a_prev, b_prev;
    uint8_t a, b;


    a = ENCODER_A;
    b = ENCODER_B;

    if( (b_prev == b) && (a_prev != a) )
    {
        // a changed
        if( a    && (!b) ) encoder--;
        if( (!a) && (!b) ) encoder++;
        if( a    && b    ) encoder++;
        if( (!a) && b    ) encoder--;
    }

    if( (a_prev == a) && (b_prev != b) )
    {
        // b changed
        if( a    && (!b) ) encoder++;
        if( (!a) && (!b) ) encoder--;
        if( a    && b    ) encoder--;
        if( (!a) && b    ) encoder++;
    }

}


