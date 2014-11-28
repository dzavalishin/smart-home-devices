#include "util.h"
#include "dports.h"
#include "packet_types.h"
#include "defs.h"
#include "net_io.h"

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
//#include <avr/sleep.h>
//#include <avr/wdt.h>

// TODO move LED ddr setup here
// TODO - ? and take it into the account when setting DDR from host?

// Contains reverse bitmask for each port.
// bit == 1 means do not trigger data send on bit value changed
static int exclude_from_compare[SERVANT_NDIG];

// exclPos is 0 for port A and so on
static void add_exclusion_mask( unsigned char exclPos, unsigned char bitmask )
{
    /*
    char i = 255;
    switch(portIoAddr)
    {
#ifdef __AVR_ATmega128__
    case PINA:
    case DDRA:
    case PORTA: i = 0; break;
#endif

    case PINB:
    case DDRB:
    case PORTB: i = 1; break;

    case PINC:
    case DDRC:
    case PORTC: i = 2; break;

    case PIND:
    case DDRD:
    case PORTD: i = 3; break;

#ifdef __AVR_ATmega128__
    case PINE:
    case DDRE:
    case PORTE: i = 4; break;

    case PINF:
    case DDRF:
    case PORTF: i = 5; break;

    case PING:
    case DDRG:
    case PORTG: i = 6; break;
#endif
    } */

    if( /*exclPos == 255 || exclPos < 0 || */ exclPos >= SERVANT_NDIG ) return;
    exclude_from_compare[exclPos] |= bitmask;
}

#define add_exclusion_pin(p,b) add_exclusion_mask(p,_BV(b))

void port_init(void)
{
#if ALARM_OUTPUTS_USED
    add_exclusion_pin( SERVANT_WARN_EXCLPOS, SERVANT_WARN_PIN );
    add_exclusion_pin( SERVANT_ALARM_EXCLPOS, SERVANT_ALARM_PIN );
#endif

    add_exclusion_pin( OW_EXCLPOS, OW_PIN );

    // todo fix
    //add_exclusion_mask( UART_EXCL_EXCLPOS, UART_EXCL_MASK);

    add_exclusion_pin( LED_EXCLPOS, LED );
#ifdef FAIL_LED_EXCLPOS
    add_exclusion_pin( FAIL_LED_EXCLPOS, FAIL_LED );
#endif

//#if HALF_DUPLEX
//    add_exclusion_pin( HALF_DUPLEX_EXCLPOS, HALF_DUPLEX_PIN );
///#endif

    unsigned char i;
    for( i = 0; i < SERVANT_NDIG; i++ )
    {
        set_ddr( i, 0 );
        //set_dig( i, 0xFF );
    }


//#if HALF_DUPLEX
//    HALF_DUPLEX_DDR |= _BV(HALF_DUPLEX_PIN);
//#endif

    led_ddr_init(); // again - first time was in main
}




// Keeps current digital input values
unsigned char dig_value[SERVANT_NDIG], dig_prev[SERVANT_NDIG];
unsigned volatile char dig_send_mask = 0; // bit 0 == 1 -> need to send ch 0

// must be called on regular basis (from interrupt)
void check_digital_inputs(void)
{
#ifdef __AVR_ATmega128__
    dig_value[0] = PINA;
#endif
    dig_value[1] = PINB;
    dig_value[2] = PINC;
    dig_value[3] = PIND;
#ifdef __AVR_ATmega128__
    dig_value[4] = PINE;
    dig_value[5] = PINF;
    dig_value[6] = PING;
#endif
    unsigned char i;
    for(i = 0; i < SERVANT_NDIG; i++)
    {
        if(
           (dig_value[i] & (~exclude_from_compare[i]))
           !=
           (dig_prev[i] & (~exclude_from_compare[i]))
           )
        {
            dig_prev[i] = dig_value[i];
            dig_send_mask |= _BV(i);

            // TODO check for alarm/warning here or in send
        }
    }
}

// TODO call from packet reception code
void request_dig_data_send(unsigned char channel)
{
    dig_send_mask |= _BV(channel);
}


void send_changed_dig_data(void)
{
    //if(adc_send_mask == 0)
    //return; // be quick

    unsigned char i;
    for( i = 0; i < SERVANT_NDIG; i++ )
    {
        //cl i();
        if( dig_send_mask & _BV(i) )
        {
            dig_send_mask &= ~_BV(i);
            //sei();
            send_pack(TOHOST_DIGITAL_VALUE, i, dig_value[i] );
        }
    }
    //sei();

}



void set_dig(unsigned char port_num, unsigned char data) {
    switch (port_num) {
#ifdef __AVR_ATmega128__
    case 0:		PORTA=data;		break;
#endif
    case 1:		PORTB=data;		break;
    case 2:		PORTC=data;		break;
    case 3:		PORTD=data;		break;
#ifdef __AVR_ATmega128__
    case 4:		PORTE=data;		break;
    case 5:		PORTF=data;		break;
    case 6:		PORTG=data;		break;
#endif
    }
}


unsigned char get_dig_out(unsigned char port_num) {
    switch (port_num) {
#ifdef __AVR_ATmega128__
    case 0:		return PORTA;
#endif
    case 1:		return PORTB;
    case 2:		return PORTC;
    case 3:		return PORTD;
#ifdef __AVR_ATmega128__
    case 4:		return PORTE;
    case 5:		return PORTF;
    case 6:		return PORTG;
#endif
    }
    return 0;
}


void set_ddr(unsigned char port_num, unsigned char data) {
    switch (port_num) {
#ifdef __AVR_ATmega128__
    case 0:		DDRA=data;		break;
#endif
    case 1:		DDRB=data;		break;
    case 2:		DDRC=data;		break;
    case 3:		DDRD=data;		break;
#ifdef __AVR_ATmega128__
    case 4:		DDRE=data;		break;
    case 5:		DDRF=data;		break;
    case 6:		DDRG=data;		break;
#endif
    }
}
