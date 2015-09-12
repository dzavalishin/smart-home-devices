/**
 *
 * DZ-MMNET-MODBUS: Modbus/TCP I/O module based on MMNet101.
 *
 * Global configuration definitions.
 *
**/

// ------------------------------------------------------------------------
// Configuration (on/off/number) of ins/outs
// ------------------------------------------------------------------------

#define	SERVANT_NPWM	0	// количество аналоговых выходов -- PWM
#define	SERVANT_NADC	8	// количество аналоговых входов

#define	SERVANT_NDIG	7	// количество цифровых портов (входы)  - в байтах
#define	SERVANT_NDIGOUT	0	// количество цифровых портов (выходы) - в байтах

#define	SERVANT_DHT11   1       // Have or not DHT11/22

// FREQ/PWM duty inputs
// pos 0 is freq for first channel, 1 is duty cycle for first channel
// pos 2 is freq for second channel, 3 is duty cycle for it

#define SERVANT_NFREQ 	0

#define N_TEMPERATURE_IN 0

// RS232 on dev board
#define SERVANT_TCP_COM0        0
// USB serial on dev board
#define SERVANT_TCP_COM1        0




// ------------------------------------------------------------------------
// Configuration (on/off/number) of components
// ------------------------------------------------------------------------

// SNTP/Syslog kills us :(
#define ENABLE_SNTP 1
#define ENABLE_SYSLOG 0


// Unused
#define ENABLE_SPI 0
// 1-wire child processor
#define ENABLE_CH1W 0
#define USE_TWI 0




// ------------------------------------------------------------------------
// Network
// ------------------------------------------------------------------------



/* These values are used if there is no valid configuration in EEPROM. */

#define DEFAULT_MAC   0x02, 0x06, 0x98, 0x00, 0x00, 0x00
#define DEFAULT_IP    "192.168.88.127"
#define DEFAULT_MASK  "255.255.0.0"

// For sntp time setup
#define DEFAULT_TZ    -3

#define DEFAULT_SYSLOGD  "192.168.1.130"
//#define MYSYSLOGD  "syslog."


// ------------------------------------------------------------------------
// Hardware connections
// ------------------------------------------------------------------------

#define LED_EXCLPOS  6 // Port G
#define LED_DDR  DDRG
#define LED_PORT PORTG
#define LED_PIN  PING
#define LED      PING3

#if 0

#define FAIL_LED_EXCLPOS  1
#define FAIL_LED_DDR  DDRB
#define FAIL_LED_PORT PORTB
#define FAIL_LED_PIN  PINB
#define FAIL_LED      PINB6

#endif


// OneWire
#define OW_EXCLPOS  6 // Port G
#define OW_PIN  PG4 // TSC1
#define OW_IN   PING
#define OW_OUT  PORTG
#define OW_DDR  DDRG
#define OW_CONF_DELAYOFFSET 0


// DHT11/22 port
#define DHT_EXCLPOS  1 // Port B
#define DHT_DDR DDRB
#define DHT_PORT PORTB
#define DHT_PIN PINB
#define DHT_INPUTPIN PB4


#define ETHERNET_INTERRUPT_EXCLPOS      4 // Port E
#define ETHERNET_INTERRUPT_PIN 		PE5

#define DATAFLASH_EXCLPOS      		1 // Port B
#define DATAFLASH_SCK_PIN 		PB1
#define DATAFLASH_MOSI_PIN 		PB2
#define DATAFLASH_MISO_PIN 		PB3
#define DATAFLASH_CS_PIN 		PB5


// ------------------------------------------------------------------------
// Hardware definitions
// ------------------------------------------------------------------------



// We have 16 MHz on MMNet101
#define F_CPU     16000000


// количество машинных циклов на один интервал PWM
// период модуляции разделяется на 256 интервалов
// частота PWM: 4.0000Mhz/256/PWM_SPEED = 1.5 Кгц.
#define	SERVANT_PWM_SPEED	10	
#define	SERVANT_PWM_PORT        PORTD
// PWM bits start at this bit, so now it's PD4..7
#define SERVANT_PWM_BIT         4

#define DEVICE_NAME "DZ-MMNET-MODBUS"
//#define DEVICE_REV "$Revision: 0 $"
//#define DEVICE_DATE "$Date: $"

extern char *makeDate; // filled in by makefile

#define HALF_DUPLEX 0



#define EEPROM_CFG_BASE 0


















// ------------------------------------------------------------------------
// Alarms - unused, delete
// ------------------------------------------------------------------------



#define ALARM_OUTPUTS_USED 0

#if ALARM_OUTPUTS_USED
// TODO - WRONG!
#define	SERVANT_ALARM_PIN	0	//PB0
#define	SERVANT_WARN_PIN	1	//PB1

#define SERVANT_ALARM_PORT      PORTB
#define SERVANT_WARN_PORT       PORTB

#define SERVANT_ALARM_EXCLPOS   1
#define SERVANT_WARN_EXCLPOS    1

#endif







// ------------------------------------------------------------------------
// CPU freq macros
// ------------------------------------------------------------------------



#if   defined F3686400
#define F_CPU     3686400
#elif defined F7372800
#define F_CPU     7372800
#elif defined F8000000
#define F_CPU     8000000
#elif defined F14745600
#define F_CPU     14745600
#elif defined F16000000
#define F_CPU     16000000
#elif defined F20000000
#define F_CPU     20000000
#endif

#define F_OSC F_CPU
