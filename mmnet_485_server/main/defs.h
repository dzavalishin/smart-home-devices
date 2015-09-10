// ------------------------------------------------------------------------
// Configuration (on/off/number of ins/outs)
// ------------------------------------------------------------------------

#define TEST_PWM 0
#define USE_TWI 0


#define	SERVANT_NPWM	0	// количество аналоговых выходов -- PWM
#define	SERVANT_NADC	8	// количество аналоговых входов

#define	SERVANT_NDIG	7	// количество цифровых портов (входы)  - в байтах
#define	SERVANT_NDIGOUT	0	// количество цифровых портов (выходы) - в байтах

#define	SERVANT_DHT11   1       // Have or not DHT11/22

// FREQ/PWM duty inputs
// pos 0 is freq for first channel, 1 is duty cycle for first channel
// pos 2 is freq for second channel, 3 is duty cycle for it

#define SERVANT_NFREQ 0

#define N_TEMPERATURE_IN 0

#define ENABLE_SPI 0

// 1-wire child processor
#define ENABLE_CH1W 0


// ------------------------------------------------------------------------
// Network
// ------------------------------------------------------------------------



/* These values are used if there is no valid configuration in EEPROM. */

#define MYMAC   0x02, 0x06, 0x98, 0x00, 0x00, 0x00
#define MYIP    "192.168.88.127"
#define MYMASK  "255.255.0.0"

// For sntp time setup
#define MYTZ    -3



// ------------------------------------------------------------------------
// Hardware connections
// ------------------------------------------------------------------------

#define LED_EXCLPOS  6
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
#define DHT_DDR DDRD
#define DHT_PORT PORTD
#define DHT_PIN PIND
#define DHT_INPUTPIN PD5

// TODO! Excl HDT11 pin from dig data ports


// ------------------------------------------------------------------------
// Hardware definitions
// ------------------------------------------------------------------------



// We have 16 MHz on MMNet101
#define F_CPU     16000000


// количество машинных циклов на один интервал PWM
// период модуляции разделяется на 256 интервалов
// частота PWM: 4.0000Mhz/256/PWM_SPEED = 1.5 Кгц.
#define	SERVANT_PWM_SPEED	10	

#define DEVICE_NAME "DZ-MMNET-MODBUS"


#define HALF_DUPLEX 0

























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
