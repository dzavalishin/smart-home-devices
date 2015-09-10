#include <inttypes.h>

//#include "defs.h"



#define HAVE_MODBUS_ASCII 1
#define HAVE_MODBUS_RTU 1
#define HAVE_MODBUS_TCP 1
#define HAVE_DCON 0



// Modbus spec says so
#define MODBUS_MAX_ADDRESS 247

// why 25?
#define MODBUS_MAX_RX_PKT 25

// to read 32 temperature measurements at once
#define MODBUS_MAX_TX_REG 32
#if HAVE_MODBUS_ASCII
#	define MODBUS_MAX_TX_PKT ((MODBUS_MAX_TX_REG*2+16)*2)
#else
#	define MODBUS_MAX_TX_PKT (MODBUS_MAX_TX_REG*2+16)
#endif

#define GET_WORD(_pos,_rx) ((int) ((_rx)[_pos]<<8) + (_rx)[(_pos)+1])
#define PUT_WORD(_pos,_val,_tx) ( ((_tx)[_pos] = ((_val)>>8)), ((_tx)[(_pos)+1] = (_val)) )



// Must be called before using modbus lib. 
void modbus_init( uint16_t baud, unsigned char our_address );

void modbus_set_baud( uint16_t baud );




extern unsigned char modbus_our_address;
extern unsigned char modbus_rx_buf[MODBUS_MAX_RX_PKT];

extern unsigned char modbus_tx_buf[MODBUS_MAX_TX_PKT];
extern unsigned char tx_len;

void modbus_rx_byte(unsigned char inbyte);

int modbus_read_regs( unsigned char *tx_buf, uint16_t startReg, uint16_t nReg );
uint8_t modbus_read_register( uint16_t nReg, uint16_t *val );

int modbus_write_regs( unsigned char *rx_buf, uint16_t startReg, uint16_t nReg );
int modbus_write_register( uint16_t nReg, uint16_t value );


// public

#if HAVE_MODBUS_RTU
void modbus_timer_callout_5msec(void);
int modbus_CRC16(unsigned char *buf, char bufsize);
#endif

#if HAVE_MODBUS_ASCII
uint8_t is_modbus_ascii_frame( unsigned char *cp, uint16_t len);
uint8_t check_modbus_ascii_lrc( unsigned char * cp, uint16_t len );


uint8_t modbus_ascii_decode( unsigned char *cp, uint16_t len );
uint8_t modbus_ascii_encode( unsigned char *cp, uint16_t len );
#endif


#if HAVE_MODBUS_TCP
void modbus_process_tcp_pkt(unsigned char *pkt, unsigned len ); // call with incoming pkt
//void modbus_send_tcp( unsigned char *modbus_tx_buf, int tx_len, unsigned int ); // must be implemented in user's code
void modbus_send_tcp( unsigned char *modbus_tx_buf, int tx_len ); // must be implemented in user's code
void modbus_parse_tcp_hdr( unsigned char *pkt, unsigned int *pkt_len );

#endif

void modbus_start_tx(void);	// implemented outside, must start UART transmission


void modbus_process_rx(void);

extern uint16_t modbus_event_cnt;
extern uint16_t modbus_crc_cnt;
extern uint16_t modbus_exceptions_cnt;



#define MODBUS_EXCEPTION_ILLEGAL_FUNCTION		1
#define MODBUS_EXCEPTION_ILLEGAL_DATA_ADDFRESS		2
#define MODBUS_EXCEPTION_ILLEGAL_DATA_VALUE		3
#define MODBUS_EXCEPTION_DEVICE_FAILURE			4
#define MODBUS_EXCEPTION_ACK				5
#define MODBUS_EXCEPTION_BUSY				6
#define MODBUS_EXCEPTION_NACK				7
#define MODBUS_EXCEPTION_MEMORY_ERROR			8



// ------------------------------------------------------------------
// Repored in modbus op 7: read exception status, byte 2
// ------------------------------------------------------------------

#define MODBUS_REPORT_ERROR(_flag) (error_flags |= (_flag))


extern uint8_t         modbus_error_flags; // bit per error type



#define PROTOCOL_MODBUS_RTU     (1 << 0)
#define PROTOCOL_MODBUS_ASCII   (1 << 1)
#define PROTOCOL_DCON           (1 << 2)
#define PROTOCOL_MODBUS_TCP     (1 << 3)


