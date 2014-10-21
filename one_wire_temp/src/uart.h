#include "defs.h"

#include <inttypes.h>

void uart_init(void);
void uart_set_baud( uint16_t baud );

extern uint16_t        uart_speed;

// NB! Temp!
void uart_sendbyte(uint8_t Data);


// Debug

//#ifndef P
//#define P(s) ({static const char c[] __attribute__ ((progmem)) = s;c;})
//#define P(s) (s)
//#endif

#include <avr/pgmspace.h>

extern void uart_puthex_byte(const unsigned char b);
extern void uart_puthex_nibble(const unsigned char b);
extern void uart_puti( int i );
//#define uart_puts_P(__s)       uart_puts_p(P(__s))
//#define uart_puts_P(__s)       uart_puts(__s)
extern void uart_puts_p(const prog_char *s );
extern void uart_puts(const char *s );
extern void uart_putc(unsigned char data);


void uart_set_send(unsigned char v);

void modbus_start_tx(void);	


#define PROTOCOL_MODBUS_RTU     (1 << 0)
#define PROTOCOL_MODBUS_ASCII   (1 << 1)
#define PROTOCOL_DCON           (1 << 2)

extern uint8_t  uart_auto_protocol;

#if HAVE_DCON
uint8_t is_dcon_frame( uint8_t *cp, uint8_t len );
void dcon_process_pkt( uint8_t *cp, uint8_t len );
#endif

