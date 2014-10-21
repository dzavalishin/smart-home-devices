#include <string.h>

#include "onewire.h"
#include "ds18x20.h"
#include "uart.h"
#include "delay.h"
#define _delay_ms delay_ms
//#include "packet_types.h"
#include "temperature.h"

#define DS_DEBUG 0


// This code works not only for temperature sensors, but for
// all 1-wire stuff also. For now it's only hardware serial
// number chip - DS2401.

uint8_t serialNumber [OW_ROMCODE_SIZE]; // = {0x55,0xAA};

uint8_t nTempSensors = 0;

uint8_t search_sensors(void);

#if N_TEMPERATURE_IN > 0
uint8_t gTempSensorIDs[N_TEMPERATURE_IN][OW_ROMCODE_SIZE];
uint16_t oldTemperature[N_TEMPERATURE_IN];
#endif


void
init_temperature(void)
{
#ifndef OW_ONE_BUS
	ow_set_bus(&PINB,&PORTB,&DDRB,PB0);
#endif

	nTempSensors = search_sensors();
}


uint8_t search_sensors(void)
{
	uint8_t i;
	uint8_t id[OW_ROMCODE_SIZE];
	uint8_t diff, nSensors;
#if DS_DEBUG
	uart_puts_P( "\rScanning Bus for DS18X20\r\n" );
#endif
	nSensors = 0;

	for( diff = OW_SEARCH_FIRST;
		diff != OW_LAST_DEVICE && nSensors < N_TEMPERATURE_IN ; )
	{
		DS18X20_find_sensor( &diff, &id[0] );

		if( diff == OW_PRESENCE_ERR ) {
#if DS_DEBUG
			uart_puts_P( "No Sensor found\r\n" );
#endif
			break;
		}

		if( diff == OW_DATA_ERR ) {
#if DS_DEBUG
			uart_puts_P( "Bus Error\r\n" );
#endif
			break;
		}
#if DS_DEBUG
			uart_puts_P( "got some\r\n" );
#endif

                if(id[0] == DS2401_ID)
                {
                    for (i=0;i<OW_ROMCODE_SIZE;i++)
                        serialNumber[i]=id[i];
                }
                else
                {
#if N_TEMPERATURE_IN > 0
                    for (i=0;i<OW_ROMCODE_SIZE;i++)
                        gTempSensorIDs[nSensors][i]=id[i];
#endif
		nSensors++;
                }

	}

	return nSensors;
}












#if N_TEMPERATURE_IN > 0

//#define SLEEP3SEC() {_delay_ms(500);_delay_ms(500);_delay_ms(500);_delay_ms(500);_delay_ms(500);_delay_ms(500);}

//#define DS_DEBUG 0


uint8_t gTempSensorIDs[N_TEMPERATURE_IN][OW_ROMCODE_SIZE];
uint16_t oldTemperature[N_TEMPERATURE_IN];


unsigned volatile char temp_send_mask = 0; // bit 0 == 1 -> need to send ch 0

void request_temp_data_send(unsigned char channel)
{
    temp_send_mask |= _BV(channel);
}

void read_temperature_data(void);

uint8_t timerCallNumber = 0;
void
temp_meter_05sec_timer(void)
{
    timerCallNumber++;
    switch(timerCallNumber)
    {
    case 1:
        if ( DS18X20_start_meas( DS18X20_POWER_PARASITE, NULL ) != DS18X20_OK)
        {
            // Error starting temp mesaure. 
        }
        break;
    case 2:
        // 0.5 sec passed, do nothing - we need 0.75 sec
        break;
    case 3:
        // 1 sec passed, read measurements.
        timerCallNumber = 0;
        read_temperature_data();
        break;
    }
}

void read_temperature_data(void)
{
    uint8_t i;
    for ( i = 0; i < nTempSensors; i++ ) {
        uint16_t out;
        if ( DS18X20_read_meas_word(&gTempSensorIDs[i][0], &out) != DS18X20_OK ) {
            continue;
        }
        // process out
        if( oldTemperature[i] != out )
        {
            temp_send_mask |= _BV(i);
            oldTemperature[i] = out;
        }

    }
}

void
send_changed_temperature_data()
{
    uint8_t i;

    for ( i = 0; i < nTempSensors; i++ ) {
        if( temp_send_mask & _BV(i) )
        {
            temp_send_mask &= ~_BV(i);
#warning send temperature
//            send_pack(TOHOST_TEMPERATURE_VALUE, i, (oldTemperature[i]) );
        }

    }
}



/*
 * DS temp sensor debug
 */


#if DS_DEBUG
void uart_put_temp(const uint8_t subzero, uint8_t cel,
	uint8_t cel_frac_bits)
{
	uint8_t buffer[sizeof(int)*8+1];
	uint16_t decicelsius;
	uint8_t i, j;

	uart_putc((subzero)?'-':'+');
	uart_puti((int)cel);
	uart_puts_P(".");
	itoa((cel_frac_bits*DS18X20_FRACCONV),buffer,10);
	j=4-strlen(buffer);
	for (i=0;i<j;i++) uart_puts_P("0");
	uart_puts(buffer);
	uart_puts_P("-C [");
	// "rounding"
	uart_putc((subzero)?'-':'+');
	decicelsius = DS18X20_temp_to_decicel(subzero, cel, cel_frac_bits);
	uart_puti( (int)(decicelsius/10) );
	uart_puts_P(".");
	uart_putc( (decicelsius%10) + '0');
	uart_puts_P("-C]");
}

#ifdef DS18X20_EEPROMSUPPORT
static void eeprom_test(void)
{
    uint8_t sp[DS18X20_SP_SIZE], th, tl;

	uart_puts_P( "\rDS18x20 EEPROM support test for fist sensor\r" );
	// DS18X20_recall_E2(&gTempSensorIDs[0][0]); // already done at power-on
	DS18X20_read_scratchpad( &gTempSensorIDs[0][0], sp);
	th = sp[DS18X20_TH_REG];
	tl = sp[DS18X20_TL_REG];
	uart_puts_P( "TH/TL from EEPROM sensor 1 : " );
	uart_puti(th);
	uart_puts_P( " / " );
	uart_puti(tl);
	uart_puts_P( "\r" );
	tl++; th++;
	DS18X20_write_scratchpad( &gTempSensorIDs[0][0],
		th, tl, DS18B20_12_BIT);
	uart_puts_P( "TH+1 and TL+1 written to scratchpad\r" );
	DS18X20_copy_scratchpad(  DS18X20_POWER_PARASITE,
		&gTempSensorIDs[0][0] );
	uart_puts_P( "scratchpad copied to DS18x20 EEPROM\r" );
	DS18X20_recall_E2(&gTempSensorIDs[0][0]);
	uart_puts_P( "DS18x20 EEPROM copied back to scratchpad\r" );
	DS18X20_read_scratchpad( &gTempSensorIDs[0][0], sp);
	if ( (th == sp[DS18X20_TH_REG]) && (tl == sp[DS18X20_TL_REG]) )
		uart_puts_P( "TH and TL verified\r" );
	else
		uart_puts_P( "verify failed\r" );
	th = sp[DS18X20_TH_REG];
	tl = sp[DS18X20_TL_REG];
	uart_puts_P( "TH/TL from EEPROM sensor 1 now : " );
	uart_puti(th);
	uart_puts_P( " / " );
	uart_puti(tl);
	uart_puts_P( "\r" );
}
#endif





int ds_test( void )
{
	uint8_t nSensors, i;
	uint8_t subzero, cel, cel_frac_bits;


#ifndef OW_ONE_BUS
	ow_set_bus(&PINB,&PORTB,&DDRB,PB0);
#endif

	uart_puts_P( "\rDS18X20 1-Wire-Reader Demo by Martin Thomas\r" );
	uart_puts_P( "-------------------------------------------" );

	// dbg uart_puts_P( "\r" );
	// dbg uart_puti((int) OW_CONF_DELAYOFFSET);
	// dbg uart_puts_P( "\r" );

	nSensors = search_sensors();
	uart_puti((int) nSensors);
	uart_puts_P( " DS18X20 Sensor(s) available:\r" );

#ifdef DS18X20_VERBOSE
	for (i=0; i<nSensors; i++) {
		uart_puts_P("# in Bus :");
		uart_puti((int) i+1);
		uart_puts_P(" : ");
		DS18X20_show_id_uart( &gTempSensorIDs[i][0], OW_ROMCODE_SIZE );
		uart_puts_P( "\r" );
	}
#endif

	for (i=0; i<nSensors; i++) {
		uart_puts_P("Sensor# ");
		uart_puti((int) i+1);
		uart_puts_P(" is a ");
		if ( gTempSensorIDs[i][0] == DS18S20_ID)
			uart_puts_P("DS18S20/DS1820");
		else uart_puts_P("DS18B20");
		uart_puts_P(" which is ");
		if ( DS18X20_get_power_status( &gTempSensorIDs[i][0] ) ==
			DS18X20_POWER_PARASITE )
			uart_puts_P( "parasite" );
		else uart_puts_P( "externally" );
		uart_puts_P( " powered\r" );
	}

#ifdef DS18X20_EEPROMSUPPORT
	if (nSensors>0) {
		eeprom_test();
	}
#endif


	for(;;) {				// main loop

            if ( nSensors == 1 )
            {
                uart_puts_P( "\rThere is only one sensor -> Demo of \"read_meas_single\":\r" );
                i = gTempSensorIDs[0][0]; // family-code for conversion-routine
                DS18X20_start_meas( DS18X20_POWER_PARASITE, NULL );
                _delay_ms(DS18B20_TCONV_12BIT);
                DS18X20_read_meas_single(i, &subzero, &cel, &cel_frac_bits);
                uart_put_temp(subzero, cel, cel_frac_bits);
                uart_puts_P("\r");
            }

            //uart_puts_P( "\rConvert_T and Read Sensor by Sensor (reverse order)\r" );
            uart_puts( "\rConvert_T and Read Sensor by Sensor (reverse order)\r" );
		for ( i=nSensors; i>0; i-- ) {
			if ( DS18X20_start_meas( DS18X20_POWER_PARASITE,
				&gTempSensorIDs[i-1][0] ) == DS18X20_OK ) {
				_delay_ms(DS18B20_TCONV_12BIT);
				uart_puts_P("Sensor# ");
				uart_puti((int) i);
				uart_puts_P(" = ");
				if ( DS18X20_read_meas( &gTempSensorIDs[i-1][0], &subzero,
						&cel, &cel_frac_bits) == DS18X20_OK ) {
					uart_put_temp(subzero, cel, cel_frac_bits);
				}
				else uart_puts_P("CRC Error (lost connection?)");
				uart_puts_P("\r");
			}
			else uart_puts_P("Start meas. failed (short circuit?)");
		}

		uart_puts_P( "\rConvert_T for all Sensors and Read Sensor by Sensor\r" );
		if ( DS18X20_start_meas( DS18X20_POWER_PARASITE, NULL )
			== DS18X20_OK) {
			_delay_ms(DS18B20_TCONV_12BIT);
			for ( i=0; i<nSensors; i++ ) {
				uart_puts_P("Sensor# ");
				uart_puti((int) i+1);
				uart_puts_P(" = ");
				if ( DS18X20_read_meas( &gTempSensorIDs[i][0], &subzero,
						&cel, &cel_frac_bits) == DS18X20_OK ) {
					uart_put_temp(subzero, cel, cel_frac_bits);
				}
				else uart_puts_P("CRC Error (lost connection?)");
				uart_puts_P("\r");
			}
		}
		else uart_puts_P("Start meas. failed (short circuit?)");

#ifdef DS18X20_VERBOSE
		// all devices:
		uart_puts_P( "\rVerbose output\r" );
		DS18X20_start_meas( DS18X20_POWER_PARASITE, NULL );
		_delay_ms(DS18B20_TCONV_12BIT);
		DS18X20_read_meas_all_verbose();
#endif

		//SLEEP3SEC();
	}
}

#endif // DS TEST


#endif // N_TEMPERATURE_IN > 0

