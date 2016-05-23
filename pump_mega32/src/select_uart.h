#ifdef __AVR_ATmega128__

#if 1

// to exclude UART from digital data comparison and auto-send
#define UART_EXCL_EXCLPOS 3 // port d
#define UART_EXCL_MASK (_BV(PD2)|_BV(PD3))


#define UCSRB UCSR1B
#define UCSRA UCSR1A
#define UCSRC UCSR1C

#define UDR UDR1

#define UBRRL UBRR1L
#define UBRRH UBRR1H

#define UART_ISR_UDRE USART1_UDRE_vect
#define UART_ISR_TX USART1_TX_vect
#define UART_ISR_RX USART1_RX_vect

#else


#define UCSRB UCSR0B
#define UCSRA UCSR0A
#define UCSRC UCSR0C

#define UDR UDR0

#define UBRRL UBRR0L
#define UBRRH UBRR0H

#define UART_ISR_UDRE USART0_UDRE_vect
#define UART_ISR_TX USART0_TX_vect
#define UART_ISR_RX USART0_RX_vect

#endif

#else // Atmega8

// Dont define, device has only one port

#define UART_ISR_UDRE USART_UDRE_vect
#define UART_ISR_TX USART_TXC_vect
#define UART_ISR_RX USART_RXC_vect

// to exclude UART from digital data comparison and auto-send
//#define UART_EXCL_PORT PORTD
#define UART_EXCL_EXCLPOS 3 // port d
#define UART_EXCL_MASK (_BV(PD0)|_BV(PD1))


#endif
