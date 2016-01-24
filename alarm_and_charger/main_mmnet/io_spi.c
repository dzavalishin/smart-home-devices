
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h>

#include <stdio.h> // printf

#include "io_spi.h"
#include "util.h"
#include "defs.h"
#include "dev_map.h"

#include "delay.h"


#if ENABLE_SPI

#define DEBUG 1

//#define PB_SS_PIN (_BV(PB0))

char spi_debug = DEBUG;

static uint16_t spi_xfer_count = 0;

// ----------------------------------------------------------------------
// Slave on/off
// ----------------------------------------------------------------------


static void set_slave( uint8_t slave );

// turn SSEL on
static inline void ss_on( uint8_t slave )  { set_slave( slave );  PB_SS_PORT &= (~PB_SS_PIN); }

// turn SSEL off
static inline void ss_off( void )          {  PB_SS_PORT |= PB_SS_PIN;    }


// ----------------------------------------------------------------------
// Init/start
// ----------------------------------------------------------------------


// Assume interrupts disabled during init
static int8_t spi_init( dev_major* d )
{
    volatile char IOReg;
    if(spi_debug) printf("SPI init...");

    if( init_subdev( d, 1, "spi" ) )
        return -1;

    ss_off();
    // set PB0(/SS), PB1(SCK), PB2(MOSI) as output
    DDRB |= (_BV(PB0)) | (_BV(PB1)) | (_BV(PB2));

    PB_SS_DDR |= PB_SS_PIN;


    // enable SPI Interrupt and SPI in Master Mode with SCK = CK/16
    //SPCR  = /*(1<<SPIE)|*/ (1<<SPE)|(1<<MSTR)|(1<<SPR0);
    // enable SPI Interrupt and SPI in Master Mode with SCK = CK/128
    SPCR  = /*(1<<SPIE)|*/ (1<<SPE)|(1<<MSTR)|_BV(SPR0)|_BV(SPR1);

    // This is for 595 shift register
    SPCR  |= (1<<CPOL)|(1<<CPHA);

    //SPCR  |= (1<<CPOL);
    //SPCR  |= (1<<CPHA);

    IOReg   = SPSR;                 	// clear SPIF bit in SPSR
    IOReg   = SPDR;

    if(spi_debug) printf(" done\n");

    return 0;
}

// transmit complete
ISR(SPI_STC_vect)
{
    sei();
}

static int8_t spi_start( dev_major* d )
{
    return 0;
}


// ----------------------------------------------------------------------
// Report status
// ----------------------------------------------------------------------


int8_t
spi_to_string( struct dev_minor *sub, char *out, uint8_t out_size )     // 0 - success
{
    if( out_size < 25 ) return -1;

    sprintf( out, "xfer count = %u", spi_xfer_count );

    return 0;
}



// ----------------------------------------------------------------------
// IO
// ----------------------------------------------------------------------


static char spi_transfer(volatile char data)
{
    SPDR = data;                    // Start the transmission
    while (!(SPSR & (1<<SPIF)))     // Wait the end of the transmission
        ;
    return SPDR;                    // return the received byte
}


unsigned char spi_send( unsigned char ss, unsigned char cmd1, unsigned char cmd2 )
{
    //spi_init();

    spi_xfer_count++;

    unsigned char out1, out2;
    if(spi_debug) printf("SPI send SS on...");
    ss_on(ss);
    delay_us(10);
    //NutSleep(1);

    if(spi_debug) printf(" Xfer1...");
    out1 = spi_transfer(cmd1);
    if(spi_debug) printf(" = 0x%02X, Xfer2...", out1);
    out2 = spi_transfer(cmd2);

    delay_us(10);
    //NutSleep(1);

    if(spi_debug) printf(" = 0x%02X, SS off...", out2);

    //ss_on();
    //delay_us(100);

    ss_off();

    //delay_us(100);
    //ss_on();

    if(spi_debug) printf(" done\n");
    return out2;
}


#endif // ENABLE_SPI

// ----------------------------------------------------------------------
// Slave select logic
// ----------------------------------------------------------------------

static void
set_slave( uint8_t slave )
{
    PORTD &= 0x1F; // clear top 3 bits
    PORTD |= (slave & 0x7) << 5;
}

// ----------------------------------------------------------------------
// Test
// ----------------------------------------------------------------------

static void spi_test_send( dev_major* d )
{
    static char a = 0xEF, b = 0x01;

    (void) d;

    a += 2;
    b += 1;

    spi_send( 0, a, b );
}


// ----------------------------------------------------------------------
// General IO definition
// ----------------------------------------------------------------------

//static int8_t spi_start_dev( dev_major* d ) { (void) d; spi_start(); return 0; }


dev_major io_spi =
{
    .name = "spi",

#if ENABLE_SPI
    .init	= spi_init,
    .start	= spi_start,
//    .stop	= spi_stop,
    .timer 	= spi_test_send,
#endif // ENABLE_SPI

    .to_string = spi_to_string,
    .from_string = 0,

    .minor_count = 1,
    .subdev = 0,
};


