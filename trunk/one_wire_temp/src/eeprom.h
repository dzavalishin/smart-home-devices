void load_eeprom_settings();


#define EEPROM_TIMEOUT_SEC      60

#define EEPROM_WR_MAP 0x01              // request to write temperature sensor mappings
#define EEPROM_WR_485 0x02              // request to write communication parameters and address

void eeprom_request_write( uint8_t what );
void eeprom_timer_5sec(void);


#define save_eeprom_settings() 			eeprom_request_write( EEPROM_WR_485 );
#define temp_sens_save_logical_numbers()        eeprom_request_write( EEPROM_WR_MAP );






/**
 *
 * EEPROM address defines
 *
**/

#define EEPROM_MAP_BASE 0
#define N_TEMP_SENS_EEPROM_RECORDS 64

#define EEPROM_MAP_REC_SZ 8


#define EEPROM_CONF_BASE ((64+32)*8)

#define EEPROM_CONF_VALID		EEPROM_CONF_BASE+0
#define EEPROM_CONF_MODBUS_ADDR		EEPROM_CONF_BASE+1
#define EEPROM_CONF_UART_SPEED_LO      	EEPROM_CONF_BASE+2
#define EEPROM_CONF_UART_SPEED_HI      	EEPROM_CONF_BASE+3



#if ( EEPROM_MAP_BASE + (N_TEMP_SENS_EEPROM_RECORDS*EEPROM_MAP_REC_SZ) ) > EEPROM_CONF_BASE
#       error EEPROM address conflict
#endif

/**
 *
 * EEPROM 1wire sensors id storage
 *
 * 64 records max, 8 bytes each
 *
 * Sensor id record:
 *
 * - 6 bytes of sensor ROM id (bytes 1-6, skipping first byte of family number and last byte of CRC)
 * - 1 byte of logical sensor ID
 * - 1 spare byte
 *
**/


#define SENS_ID_BYTE 6

