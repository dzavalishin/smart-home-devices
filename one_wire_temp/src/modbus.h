#include <inttypes.h>

#include "defs.h"



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



void modbus_init( void );
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

