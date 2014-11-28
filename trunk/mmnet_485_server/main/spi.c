
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h>

#include <stdio.h> // printf

#include "spi.h"
#include "util.h"
#include "defs.h"


#if ENABLE_SPI

#define PB_SS_PIN (_BV(PB4))

// turn SSEL on
static inline void ss_on(void) {    PORTB &= (~PB_SS_PIN); }

// turn SSEL off
static inline void ss_off(void){    PORTB |= PB_SS_PIN;  }


// Assume interrupts disabled during init
void spi_init(void)
{
        volatile char IOReg;
printf("SPI init...");
        ss_off();

        // set PB0(/SS), PB1(SCK), PB2(MOSI) as output
        DDRB |= PB_SS_PIN|(_BV(PB0))|(_BV(PB1))|(_BV(PB2));

        // enable SPI Interrupt and SPI in Master Mode with SCK = CK/16
        //SPCR  = /*(1<<SPIE)|*/ (1<<SPE)|(1<<MSTR)|(1<<SPR0);
        // enable SPI Interrupt and SPI in Master Mode with SCK = CK/128
        SPCR  = /*(1<<SPIE)|*/ (1<<SPE)|(1<<MSTR)|_BV(SPR0)|_BV(SPR1);

        SPCR  |= (1<<CPOL);
        SPCR  |= (1<<CPHA);

        IOReg   = SPSR;                 	// clear SPIF bit in SPSR
        IOReg   = SPDR;

printf(" done\n");
}

// transmit complete
ISR(SPI_STC_vect)
{
    sei();
}


static char spi_transfer(volatile char data)
{
  SPDR = data;                    // Start the transmission
  while (!(SPSR & (1<<SPIF)))     // Wait the end of the transmission
   {};
  return SPDR;                    // return the received byte
}


unsigned char spi_send( unsigned char cmd1, unsigned char cmd2 )
{
    spi_init();

    unsigned char out1, out2;
printf("SPI send SS on...");
    ss_on();

    //delay_us(10);

printf(" Xfer1...");
    out1 = spi_transfer(cmd1);
printf(" = 0x%02X, Xfer2...", out1);
    out2 = spi_transfer(cmd2);

    //delay_us(10);

printf(" = 0x%02X, SS off...", out2);
    ss_off();
printf(" done\n");
    return out2;
}

#endif // ENABLE_SPI

