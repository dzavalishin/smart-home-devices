#include "defs.h"
#include "delay.h"

#include "select_uart.h"
#include "uart.h"

#include "util.h"
#include "timer0.h"
//#include "crc16.h"

#include "modbus.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>




uint8_t  uart_auto_protocol = PROTOCOL_MODBUS_RTU;




#define UBBR_VALUE ((F_CPU/16/ DEFAULT_UART_BAUD )-1)

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

    UCSRB = (_BV(TXEN))|_BV(RXCIE)|_BV(RXEN);

#if HALF_DUPLEX
    //kills all :( uart_set_send(0); // For RS485
    HALF_DUPLEX_PORT &= ~_BV(HALF_DUPLEX_PIN);
#endif

}

uint16_t        uart_speed = DEFAULT_UART_BAUD;

void uart_set_baud( uint16_t baud )
{
    uart_speed = baud;

    uint16_t ubbr = ((F_CPU / 16) / baud ) - 1;

    UBRRH = ubbr >> 8;
    UBRRL = ubbr;
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
void uart_set_send(unsigned char v)
{
#if HALF_DUPLEX

    if( v )
    {
        // Start sending
//        wait_for_halfduplex_timeout(); // Wait for reception to stop - opens interrups!
        HALF_DUPLEX_PORT |= _BV(HALF_DUPLEX_PIN);
        //rx_count = 0; // discard partial packets
    }
    else
    {
        // Wait if a byte is being transmitted
//        while( ! (UCSRA & _BV(TXC)) )
//            ;

        HALF_DUPLEX_PORT &= ~_BV(HALF_DUPLEX_PIN);
    }
    //HALF_DUPLEX_DDR |= _BV(HALF_DUPLEX_PIN);
    inSend = v;
#endif // HALF_DUPLEX
}





static uint8_t send_buf_pos = 0;

static void send_next_byte()
{
    while( (UCSRA & _BV(UDRE)) )
    {
        UCSRA |= _BV(TXC); // reset transmit complete flag for RS485 half-duplex logic

        if( send_buf_pos >= tx_len )
        {
            // Stop send
            UCSRB &= ~_BV(UDRIE);
            return;
        }

        // Transmit data
        UDR = modbus_tx_buf[send_buf_pos];

        send_buf_pos++;
    }
}


ISR(UART_ISR_UDRE)
{
    send_next_byte();
    sei();
}


ISR(UART_ISR_TX)
{
    UCSRB &= ~_BV(TXCIE);
    sei();
    uart_set_send(0);
}



void modbus_start_tx()
{
    uart_set_send(1);

    delay_ms(1); // For half-duplex to actually switch

    send_buf_pos = 0;
    UCSRB |= _BV(UDRIE)|_BV(TXCIE);

    send_next_byte();
}


ISR(UART_ISR_RX)
{
    unsigned char inbyte;

    inbyte = UDR;

    // If we are sending, ignore incoming - RS485 is half duplex
    if( inSend /*|| rx_count >= PACKET_SIZE */)
    {
        // Loose byte!
        sei();
        return;
    }

#if HALF_DUPLEX
    mark_halfduplex_timeout();
#endif

    modbus_rx_byte(inbyte);
}






