/**
 *
 * DZ-MMNET-WALL: Wall control panel based on MMNet01.
 *
 * Digital ports I/O.
 *
**/

#include "defs.h"
#include "util.h"
#include "runtime_cfg.h"

#include "io_dig.h"


#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>


#if SERVANT_NDIG > 0

static unsigned char get_dig_in(unsigned char port_num);

static unsigned char get_dig_out(unsigned char port_num);
static void set_dig_out(unsigned char port_num, unsigned char data);

static unsigned char get_ddr(unsigned char port_num);
static void set_ddr(unsigned char port_num, unsigned char data);


/*

// Contains reverse bitmask for each port.
// bit == 1 means do not trigger data send on bit value changed
static unsigned char exclude_port_bits[SERVANT_NDIG];

// exclPos is 0 for port A and so on
void add_exclusion_mask( unsigned char exclPos, unsigned char bitmask )
{
    if( exclPos >= SERVANT_NDIG ) return;
    exclude_port_bits[exclPos] |= bitmask;
}
*/


// TODO support dynamic io conf here
void dio_init(void)
{
/*
    // Totally unavailable on MMNET101 ports

    add_exclusion_mask( 0, 0xFF ); // Port A
    add_exclusion_mask( 2, 0xFF ); // Port C


    add_exclusion_pin( ETHERNET_INTERRUPT_EXCLPOS, ETHERNET_INTERRUPT_PIN );

    add_exclusion_pin( DATAFLASH_EXCLPOS, DATAFLASH_SCK_PIN );
    add_exclusion_pin( DATAFLASH_EXCLPOS, DATAFLASH_MOSI_PIN );
    add_exclusion_pin( DATAFLASH_EXCLPOS, DATAFLASH_MISO_PIN );
    add_exclusion_pin( DATAFLASH_EXCLPOS, DATAFLASH_CS_PIN );

    add_exclusion_pin( OW_EXCLPOS, OW_DEFAULT_PIN ); // TODO exclude multibus pins too!

#if ENABLE_TWI
    add_exclusion_pin( TWI_EXCLPOS, TWI_SDA );
    add_exclusion_pin( TWI_EXCLPOS, TWI_SCL );
#endif

#if SERVANT_DHT11
    if(RT_IO_ENABLED(IO_DHT))
        add_exclusion_pin( DHT_EXCLPOS, DHT_PIN );
#endif

    // todo fixme
    //add_exclusion_mask( UART1_EXCL_EXCLPOS, UART1_EXCL_MASK );
    //add_exclusion_mask( UART2_EXCL_EXCLPOS, UART2_EXCL_MASK );
#if 0 // gone to tunnel code
#if SERVANT_TUN0
    add_exclusion_pin( UART0_EXCLPOS, UART0_TX_PIN );
    add_exclusion_pin( UART0_EXCLPOS, UART0_RX_PIN );
#endif
#if SERVANT_TUN1
    add_exclusion_pin( UART1_EXCLPOS, UART0_TX_PIN );
    add_exclusion_pin( UART1_EXCLPOS, UART0_RX_PIN );
#endif
#endif

#if ENABLE_HALF_DUPLEX_0
    if(RT_IO_ENABLED(IO_TUN0))
        add_exclusion_pin( HALF_DUPLEX0_EXCLPOS, HALF_DUPLEX0_PIN );
#endif
#if ENABLE_HALF_DUPLEX_1
    if(RT_IO_ENABLED(IO_TUN1))
        add_exclusion_pin( HALF_DUPLEX1_EXCLPOS, HALF_DUPLEX1_PIN );
#endif

    add_exclusion_pin( LED_EXCLPOS, LED );

#ifdef FAIL_LED_EXCLPOS
    add_exclusion_pin( FAIL_LED_EXCLPOS, FAIL_LED );
#endif
*/

    unsigned char i;
    for( i = 0; i < SERVANT_NDIG; i++ )
    {
        set_ddr( i, 0 );
        //set_dig( i, 0xFF );
    }

    // TODO fixme read DDR mask from EEPROM

    // Set port values BEFORE enabling DDE
    dio_set_default_output_state();

    DDRB = 0; // ee_cfg.ddr_b;
    DDRD = 0; // ee_cfg.ddr_d;
    DDRE = 0; // ee_cfg.ddr_e;
    DDRF = 0; // ee_cfg.ddr_f;
    DDRG = 0; // ee_cfg.ddr_g;



    // Now set dde for pins we use in a dedicated way



    led_ddr_init(); // again - first time was in main
}

void
dio_set_default_output_state( void ) // Used on start and if communications are lost
{
    PORTB = 0; // ee_cfg.start_b;
    PORTD = 0; // ee_cfg.start_d;
    PORTE = 0; // ee_cfg.start_e;
    PORTF = 0; // ee_cfg.start_f;
    PORTG = 0; // ee_cfg.start_g;
}









// Direct
unsigned char   dio_read_port( unsigned char port ) { return get_dig_in(port); }

void
dio_write_port( unsigned char port, unsigned char new_bits )
{
    /*
    if( port >= SERVANT_NDIG ) return;
    cli();

    unsigned char prev = get_dig_out(port);

    new_bits &= ~exclude_port_bits[port]; // reset bits that we can NOT change

    prev &= exclude_port_bits[port]; // reset bits that we can change
    prev |= new_bits;

    set_dig_out( port, prev );

    sei();
    */
}


unsigned char   dio_get_port_ouput_mask( unsigned char port ) { return get_ddr(port); }

    /*
void
dio_set_port_ouput_mask( unsigned char port, unsigned char new_mask )
{
    if( port >= SERVANT_NDIG ) return;
    cli();

    unsigned char prev = get_ddr(port);

    new_mask &= ~exclude_port_bits[port]; // reset bits that we can NOT change

    prev &= exclude_port_bits[port]; // reset bits that we can change
    prev |= new_mask;

    set_ddr( port, prev );

    sei();
}
    */




unsigned char
dio_read_port_bit( unsigned char port, unsigned char nBit )
{
    return 0x1 & (dio_read_port(port) >> nBit );
}

/*
void
dio_write_port_bit( unsigned char port, unsigned char nBit, unsigned char value )
{
    cli();
    unsigned char prev = get_dig_out( port );

    if( value )         prev |= _BV(nBit);
    else         	prev &= ~(_BV(nBit));

    dio_write_port( port, prev );
    sei();
}
*/

unsigned char
dio_get_port_ouput_mask_bit( unsigned char port, unsigned char nBit )
{
    return 0x1 & (get_ddr(port) >> nBit);
}

/*
void
dio_set_port_ouput_mask_bit( unsigned char port, unsigned char nBit, unsigned char value )
{
    cli();
    unsigned char prev = get_ddr( port );

    if( value )         prev |= _BV(nBit);
    else         	prev &= ~(_BV(nBit));

    dio_set_port_ouput_mask( port, prev );
    sei();
}
*/



















static unsigned char get_dig_in(unsigned char port_num)
{
    if( port_num >= SERVANT_NDIG ) return 0;
    switch (port_num) {
    case 0:		return PINA;
    case 1:		return PINB;
    case 2:		return PINC;
    case 3:		return PIND;
    case 4:		return PINE;
    case 5:		return PINF;
    case 6:		return PING;
    }
    return 0;
}





static void set_dig_out(unsigned char port_num, unsigned char data)
{
    if( port_num >= SERVANT_NDIG ) return;
    switch (port_num) {
    case 0:		PORTA=data;		break;
    case 1:		PORTB=data;		break;
    case 2:		PORTC=data;		break;
    case 3:		PORTD=data;		break;
    case 4:		PORTE=data;		break;
    case 5:		PORTF=data;		break;
    case 6:		PORTG=data;		break;
    }
}


static unsigned char get_dig_out(unsigned char port_num)
{
    if( port_num >= SERVANT_NDIG ) return 0;
    switch (port_num) {
    case 0:		return PORTA;
    case 1:		return PORTB;
    case 2:		return PORTC;
    case 3:		return PORTD;
    case 4:		return PORTE;
    case 5:		return PORTF;
    case 6:		return PORTG;
    }
    return 0;
}


static void set_ddr(unsigned char port_num, unsigned char data)
{
    if( port_num >= SERVANT_NDIG ) return;
    switch (port_num) {
    case 0:		DDRA=data;		break;
    case 1:		DDRB=data;		break;
    case 2:		DDRC=data;		break;
    case 3:		DDRD=data;		break;
    case 4:		DDRE=data;		break;
    case 5:		DDRF=data;		break;
    case 6:		DDRG=data;		break;
    }
}

static unsigned char get_ddr(unsigned char port_num)
{
    if( port_num >= SERVANT_NDIG ) return 0;
    switch (port_num) {
    case 0:		return DDRA;
    case 1:		return DDRB;
    case 2:		return DDRC;
    case 3:		return DDRD;
    case 4:		return DDRE;
    case 5:		return DDRF;
    case 6:		return DDRG;
    }
    return 0;
}





#endif // SERVANT_NDIG > 0













