#if 0

#include "defs.h"
//#include "crumb_defs.h"
#include "delay.h"
#include "select_uart.h"
#include "uart.h"
#include "adc.h"
#include "pwm.h"
#include "dports.h"
//#include "packet_types.h"
#include "alarm.h"
#include "freq.h"
#include "temperature.h"
#include "util.h"
#include "serial_number.h"
#include "onewire.h"
#include "timer0.h"

#include <avr/io.h>
#include <avr/interrupt.h>
//#include <util/delay.h>
#include <string.h>
#include <stdlib.h>

unsigned volatile char send_pong_flag = 0;
unsigned volatile char send_ack_flag = 0; // crashes if static!
unsigned volatile char send_nak_flag = 0;

unsigned char ourNodeNumber = 0; // On start we are node zero. This is a broadcast address.


#define PACKET_SIZE 8
unsigned volatile char rx_packet[PACKET_SIZE]; // TODO move out packet size def
unsigned volatile char rx_count = 0; // how many bytes in rx_packet are occupied


static void uart_set_send(unsigned char v);


// TO DO - maybe, we should use parity bit?


int crc16_calc( const unsigned char *byt, int size );

//#define BAUD 9600
#define BAUD 38400
#define UBBR_VALUE ((F_CPU/16/BAUD)-1)

//UART init
// desired baud rate: 9600
// actual: baud rate:9615 (0,2%)
// char size: 8 bit
// parity: Even
void uart_init(void)
{
    UCSRB = 0x00; //disable while setting baud rate
    UCSRA = 0x00;

    UBRRH = UBBR_VALUE>>8;
    UBRRL = UBBR_VALUE;

    //UCSRB = 0x18;
#ifdef __AVR_ATmega128__
    // Atmega 128 has separate UCSRC and UBRRH registers
    UCSRC = 0x6; // no parity, one stop, 8 bit
#else
    UCSRC = _BV(URSEL) | 0x6; 
#endif

    //UCSRB = (1<<RXEN)|(1<<TXEN);
    UCSRB = (_BV(TXEN))|_BV(RXCIE)|_BV(RXEN);

#if HALF_DUPLEX
    //kills all :( uart_set_send(0); // For RS485
    HALF_DUPLEX_PORT &= ~_BV(HALF_DUPLEX_PIN);
#endif
    // TODO enable RX interrupts

}

void uart_sendbyte(uint8_t Data)
{

    // Wait if a byte is being transmitted
    while( ! (UCSRA & _BV(UDRE)) )
        ;

    UCSRA |= _BV(TXC); // reset transmit complete flag for RS485 half-duplex logic
    // Transmit data
    UDR = Data;
}

volatile unsigned char inSend = 0;
static void uart_set_send(unsigned char v)
{
#if HALF_DUPLEX

    if( v )
    {
        // Start sending
        wait_for_halfduplex_timeout(); // Wait for reception to stop - opens interrups!
        HALF_DUPLEX_PORT |= _BV(HALF_DUPLEX_PIN);
        rx_count = 0; // discard partial packets
    }
    else
    {
        // Wait if a byte is being transmitted
        while( ! (UCSRA & _BV(TXC)) )
            ;

        HALF_DUPLEX_PORT &= ~_BV(HALF_DUPLEX_PIN);
    }
    //HALF_DUPLEX_DDR |= _BV(HALF_DUPLEX_PIN);
    inSend = v;
#endif // HALF_DUPLEX
}


//ISR(USART1_UDRE_vect)
ISR(UART_ISR_UDRE)
{
    sei();
}

//ISR(USART1_TX_vect)
ISR(UART_ISR_TX)
{
    sei();
}

void process_rx_data(void);
void process_rx_packet( unsigned char node, unsigned char type, unsigned char port, unsigned int data);
void process_broadcast_rx_packet( unsigned char type, unsigned char port, unsigned int data);


//ISR(USART1_RX_vect)
ISR(UART_ISR_RX)
{
    unsigned char inbyte;

    // If we are sending, ignore incoming - RS485 is half duplex
    if( inSend || rx_count >= PACKET_SIZE)
    {
        inbyte = UDR; // Loose byte!
        sei();
        //process_rx_data();
        return;
    }

    inbyte = UDR;

#if HALF_DUPLEX
    mark_halfduplex_timeout();
#endif

    // start of packet has to be 0x11
    if((rx_count == 0) && (inbyte != 0x11))
        return;

    //turn_led_on();

    rx_packet[rx_count++] = inbyte; // receive byte
    //sei();

    if(rx_count >= PACKET_SIZE)
        process_rx_data();

}

void process_rx_data(void)
{
    //sei();
    //if(0 == rx_count )        return;

    unsigned int crc = crc16_calc(rx_packet, PACKET_SIZE-2);
    unsigned char crc_hi= (crc>>8) & 0xFFu;

    //if(rx_packet[0] == 0x11)        turn_led_on();

//turn_led_on();
    if(
       (rx_packet[PACKET_SIZE-2] != crc_hi) ||
       (rx_packet[PACKET_SIZE-1] != (crc & 0xFFu))
      )
    {
        cli();
        send_nak_flag++;
#if 1
        unsigned char i;
        for( i = 1; i < rx_count; i++ )
        {
            if( rx_packet[i] == 0x11 )
            {
                memmove( rx_packet, rx_packet+i, PACKET_SIZE-i );

                rx_count = PACKET_SIZE-i;
                sei();
                return;
            }
        }
#endif
        rx_count = 0; // discard packet
        sei();
        return;
    }

//turn_led_on();

    unsigned char node = rx_packet[1];

    // Not for us?
    if( node != 0 && node != ourNodeNumber )
    {
        rx_count = 0; // free packet mem
        return;
    }

    unsigned char type = rx_packet[2];
    unsigned char port = rx_packet[3];
    unsigned int data = rx_packet[4];
    data |= (rx_packet[5]) << 8;
    rx_count = 0; // free packet mem
    sei();
    process_rx_packet( node, type, port, data );
}


void process_rx_packet(
                       unsigned char node,
                       unsigned char type,
                       unsigned char port,
                       unsigned int data)
{
    if( (node == 0) && (type & 0x80) )
    {
        // Broadcast, process specially
        process_broadcast_rx_packet( type, port, data);
        return;
    }
    switch(type) {
    
    case TOHOST_NAK: break; // Echo?
    case TOHOST_ACK: break; // Echo?

    case FROMHOST_PING:
        send_pong_flag++;
        break;
    
    case FROMHOST_DIGITAL_REQUEST: request_dig_data_send(port); break;
    
    case FROMHOST_ANALOG_REQUEST: request_adc_data_send(port); break;

    case FROMHOST_DIGITAL_OUT_REQUEST:
    case FROMHOST_ANALOG_OUT_REQUEST:
        // TODO implement
        send_nak_flag++;
        break;

    case FROMHOST_FREQ_VALUE_REQUEST:  
#if SERVANT_NFREQ > 0
        request_freq_data_send(port); 
#endif
        break;

    case FROMHOST_TEMPERATURE_VALUE_REQUEST: 
#if N_TEMPERATURE_IN > 0
        request_temp_data_send(port);
#endif
        break;


    case FROMHOST_DIGITAL_OUT:
        set_dig(port, data);      send_ack_flag++;
        break;

    case FROMHOST_ANALOG_OUT:
#if SERVANT_NPWM > 0
        set_an(port, data);       send_ack_flag++;
#endif
        break;

    case FROMHOST_DIGITAL_OUT_ENABLE:
        set_ddr(port, data);      send_ack_flag++;
        break;

    case FROMHOST_ANALOG_OUT_ENABLE:
    //case 0x31:			// включение PWM-ов
#if SERVANT_NPWM > 0
        pwm_mask_byte=data;
#endif
        send_ack_flag++;
        break;
        // "warning" setup
    case 0x40:		 	// задание битовой маски для предупреждающего сигнала
        warn_mask[port]=data;     send_ack_flag++;
        break;
    case 0x41:			//задание нижнего значения аналогового порта
        warn_min[port]=data;      send_ack_flag++;
        break;
    case 0x48:		 	// задание сравниваемого значения
        warn_byte[port]=data;     send_ack_flag++;
        break;
    case 0x49:			// задание верхнего значения
        warn_max[port]=data;      send_ack_flag++;
        break;
        // "alarm" setup
        // то же, что и для "warning"
    case 0x50:		 	// задание битовой маски
        al_mask[port]=data;       send_ack_flag++;
        break;
    case 0x51:			//задание нижнего значения аналогового порта
        al_min[port]=data;        send_ack_flag++;
        break;
    case 0x58:		 	// задание сравниваемого значения
        al_byte[port]=data;       send_ack_flag++;
        break;
    case 0x59:			// задание верхнего значения
        al_max[port]=data;        send_ack_flag++;
        break;

    default:
        send_nak_flag++;
        break;
    }
}


unsigned char mayBeSetNodeNumber_flags = 0;
unsigned char mayBeSetNodeNumber_node = 0;
void mayBeSetNodeNumber(unsigned char node)
{
    if(mayBeSetNodeNumber_node == 0)
        mayBeSetNodeNumber_node = node;

    if(mayBeSetNodeNumber_node != node)
        return;

    if(mayBeSetNodeNumber_flags == 0x07)
        ourNodeNumber = mayBeSetNodeNumber_node;
}

char checkSerial(unsigned int data, int pos)
{
    if(serialNumber[pos] != (0xFF & data))
        return 0;

    if(serialNumber[pos+1] != (0xFF & (data>>8)))
        return 0;

    return 1;
}

void process_broadcast_rx_packet(
                       unsigned char type,
                       unsigned char port,
                       unsigned int data)
{
    switch(type) {
    case FROMHOST_BROADCAST_SETNODE_A:
        if( ourNodeNumber != 0 ) return;
        if(!checkSerial(data,0)) return;
        mayBeSetNodeNumber_flags |= 0x01;
        mayBeSetNodeNumber(port);
        break;

    case FROMHOST_BROADCAST_SETNODE_B:
        if( ourNodeNumber != 0 ) return;
        if(!checkSerial(data,2)) return;
        mayBeSetNodeNumber_flags |= 0x02;
        mayBeSetNodeNumber(port);
        break;

    case FROMHOST_BROADCAST_SETNODE_C:
        if( ourNodeNumber != 0 ) return;
        if(!checkSerial(data,4)) return;
        mayBeSetNodeNumber_flags |= 0x04;
        mayBeSetNodeNumber(port);
        break;
    }
 
}


void uart_request_pong() {    send_pong_flag++; }

char pongNo = 9; // so that it will be sent at the beginning
void send_protocol_replies(void)
{
    while(send_nak_flag > 0)
    {
        send_nak_flag--;
        send_pack( TOHOST_NAK, 0, 0);
    }

    while(send_ack_flag > 0)
    {
        send_ack_flag--;
        send_pack( TOHOST_ACK, 0, 0);
    }

    while(send_pong_flag > 0)
    {
        send_pong_flag--;
        send_pack( TOHOST_PONG, 0, 0);
        if(pongNo++ > 2)
        {
            send_long_packet(TOHOST_SERIAL, OW_ROMCODE_SIZE, serialNumber );
            send_long_packet(TOHOST_DEVTYPE, sizeof(DEVICE_NAME)-1, DEVICE_NAME );
            pongNo = 0;
        }
    }

}

void send_pack(unsigned char send_type, unsigned char send_port, unsigned int send_data) {
    unsigned char pack[PACKET_SIZE];
    // отослать пакет на ПК
    unsigned int crc, crc1;

    //uart_puts(" pkt -> ");

#if HALF_DUPLEX
    uart_set_send(1); // For RS485
#endif

    uart_sendbyte(0x12);
    pack[0]=0x12;
    uart_sendbyte(ourNodeNumber);
    pack[1]=ourNodeNumber;
    uart_sendbyte(send_type);
    pack[2]=send_type;
    uart_sendbyte(send_port);
    pack[3]=send_port;
    uart_sendbyte(send_data);
    pack[4]=send_data;
    uart_sendbyte(send_data >> 8);
    pack[5]=send_data >> 8;

    crc=crc16_calc(pack, PACKET_SIZE-2);
    crc1=crc>>8;

    // TODO need some timeout between packets or data loss occurs :(
    delay_ms(10);

    uart_sendbyte(crc1);	// старший байт crc
    uart_sendbyte(crc);	// младший байт crc

    // TODO need some timeout between packets or data loss occurs :(
    delay_ms(20); // 20 total is too small
#if HALF_DUPLEX
    uart_set_send(0); // For RS485
#endif

    //uart_puts(" <- pkt ");

    //turn_led_on();

}

/**
 *
 * Send extended packet up to 255 bytes long.
 *
 * Packet is a combination of standard short packet of a special
 * type and extended data with separate additional CRC bytes.
 *
 * Short packet's type is fixed, data byte is length of extension
 * (without CRC) and port number is used for extended packet type. 
 *
 * Java code also supposes that first byte of ext data is 
 * port, if applicable.
 *
**/

//void send_long_packet(unsigned char short_val, unsigned char type, unsigned char port, unsigned char long_len, unsigned char *long_data )
void send_long_packet(unsigned char ext_type, unsigned char long_len, unsigned char *long_data )
{
    send_pack( TOHOST_LONG_PACKET, ext_type, long_len/*+2*/ );
    uart_set_send(1); // For RS485

    unsigned int crc=crc16_calc(long_data, long_len);
    unsigned char crc1=crc>>8;

    unsigned char i = long_len;
    while(i-->0)
    {
        uart_sendbyte(*long_data);
        long_data++;
    }

    uart_sendbyte(crc1);// старший байт crc
    uart_sendbyte(crc);	// младший байт crc
    uart_set_send(0); // For RS485
}


int crc16_calc( const unsigned char *byt, int size )
{
  /* Calculate CRC-16 value; uses The CCITT-16 Polynomial,
     expressed as X^16 + X^12 + X^5 + 1 */

  int crc = (int) 0xffff;
  int index;
  char b;

  for( index=0; index<size; ++index )
    {
      crc ^= (((int) byt[index]) << 8);
      for( b=0; b<8; ++b )
	{
	  if( crc & (int) 0x8000 )
	    crc = (crc << 1) ^ (int) 0x1021;
	  else
	    crc = (crc << 1);
	}
    }
  return crc;
}

/*
 * Debug
 */

void uart_putc( unsigned char c ) {
    uart_sendbyte(c);
    if(c=='\r')
        uart_sendbyte('\n');
}

void uart_puts(const char *s )
{
#if HALF_DUPLEX
    uart_set_send(1); // For RS485
#endif
    while (*s) 
      uart_putc(*s++);

    //delay_ms(20);
#if HALF_DUPLEX
    uart_set_send(0); // For RS485
#endif

}/* uart_puts */


/*************************************************************************
Function: uart_puts_p()
Purpose:  transmit string from program memory to UART
Input:    program memory string to be transmitted
Returns:  none
************************************************************************** /
void uart_puts_p(const prog_char *progmem_s )
{
    register char c;
    
    while ( (c = pgm_read_byte_far(progmem_s++)) )
      uart_putc(c);

}/* uart_puts_p */


void uart_puti( const int val )
{
    char buffer[sizeof(int)*8+1];

    itoa(val, buffer, 10);
    uart_puts( buffer );

}/* uart_puti */

void uart_puthex_nibble(const unsigned char b)
{
    unsigned char  c = b & 0x0f;
    if (c>9) c += 'A'-10;
    else c += '0';
    uart_putc(c);
} /* uart_puthex_nibble */

void uart_puthex_byte(const unsigned char  b)
{
    uart_puthex_nibble(b>>4);
    uart_puthex_nibble(b);
} /* uart_puthex_byte */

#endif
