#include <string.h>
#include <stdio.h>

#include <sys/timer.h>

#include "onewire.h"
#include "ds18x20.h"
#include "net_io.h"
#include "icp.h" // disabling
#include "packet_types.h"
#include "temperature.h"
#include "util.h"

#include "child-1wire.h"


#define DS_DEBUG 1

#define DISABLE_ICP 0


#if DS_DEBUG
#define DS_DEBUG_PUTS(msg) DEBUG_PUTS(msg)
#else
#define DS_DEBUG_PUTS(msg) {}
#endif

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
	ow_set_bus(&PING,&PORTG,&DDRG,PG2);
#endif

	nTempSensors = search_sensors();
}


uint8_t search_sensors(void)
{
	uint8_t i;
	uint8_t id[OW_ROMCODE_SIZE];
	uint8_t diff, nSensors;
#if DS_DEBUG
	DS_DEBUG_PUTS( "\rScanning Bus for DS18X20\r\n" );
#endif
	nSensors = 0;

	for( diff = OW_SEARCH_FIRST;
		diff != OW_LAST_DEVICE && nSensors < N_TEMPERATURE_IN ; )
	{
		DS18X20_find_sensor( &diff, &id[0] );

		if( diff == OW_PRESENCE_ERR ) {
#if DS_DEBUG
			DS_DEBUG_PUTS( "No Sensor found\r\n" );
#endif
			break;
		}

		if( diff == OW_DATA_ERR ) {
#if DS_DEBUG
			DS_DEBUG_PUTS( "Bus Error\r\n" );
#endif
			break;
		}
#if DS_DEBUG
                printf( "got some 1wire: 0x%02X\n", id[0] );
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










#if N_TEMPERATURE_IN > 8
#error one byte for temp send mask
#endif


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

//#define DS_POWER_SRC DS18X20_POWER_PARASITE
#define DS_POWER_SRC DS18X20_POWER_EXTERN

static uint8_t timerCallNumber = 0;
void
temp_meter_sec_timer(void)
{
#if 0
    read_temperature_data();
#else
    timerCallNumber++;
    switch(timerCallNumber)
    {
    case 1:
#if DISABLE_ICP
        disable_icp( 1 );
#endif
        if ( DS18X20_start_meas( DS_POWER_SRC, NULL ) != DS18X20_OK)
        {
            // Error starting temp mesaure. TODO: Report!
        }
#if DISABLE_ICP
        disable_icp( 0 );
#endif
        break;

    case 2:
        // 1 sec passed, read measurements.
        // TODO we can restart measurements after reading them
        timerCallNumber = 0;
#if DISABLE_ICP
        disable_icp( 1 );
#endif
        read_temperature_data();

#if 1
        if ( DS18X20_start_meas( DS_POWER_SRC, NULL ) != DS18X20_OK)
        {
            // Error starting temp mesaure. TODO: Report!
        }
        timerCallNumber = 1; // to return here.
#endif

#if DISABLE_ICP
        disable_icp( 0 );
#endif
        break;
    }
#endif
}

void read_temperature_data(void)
{
    uint8_t i;
    for ( i = 0; i < nTempSensors; i++ ) {
        uint16_t out;
#if 0
        out = 0;
        //out = ch1w_read_temperature( i );
#else
        if ( DS18X20_read_meas_word(&gTempSensorIDs[i][0], &out) != DS18X20_OK ) {
            // TODO send some error here
            continue;
        }
#endif
        // process out
        if( oldTemperature[i] != out )
        {
            temp_send_mask |= _BV(i);
            oldTemperature[i] = out;
        }

    }
}

void
send_changed_temperature_data(void)
{
    uint8_t i;

    for ( i = 0; i < nTempSensors; i++ ) {
        if( temp_send_mask & _BV(i) )
        {
            temp_send_mask &= ~_BV(i);
            send_pack(TOHOST_TEMPERATURE_VALUE, i, (oldTemperature[i]) );
#if DS_DEBUG
            printf("send chan %d temp 0x%04X\n", i, oldTemperature[i] );
#endif
        }

    }
}



/*
 * DS temp sensor debug
 */


#if DS_DEBUG
/*
void uart_put_temp(const uint8_t subzero, uint8_t cel,
	uint8_t cel_frac_bits)
{
	uint8_t buffer[sizeof(int)*8+1];
	uint16_t decicelsius;
	uint8_t i, j;

	putchar((subzero)?'-':'+');
	printf("%d.",(int)cel);
	itoa((cel_frac_bits*DS18X20_FRACCONV),buffer,10);
	j=4-strlen(buffer);
	for (i=0;i<j;i++) DS_DEBUG_PUTS("0");
	puts(buffer);
	DS_DEBUG_PUTS("-C [");
	// "rounding"
	putchar((subzero)?'-':'+');
	decicelsius = DS18X20_temp_to_decicel(subzero, cel, cel_frac_bits);
	printf("%d.", (int)(decicelsius/10) );
	putchar( (decicelsius%10) + '0');
	DS_DEBUG_PUTS("-C]");
}
*/
/*
#ifdef DS18X20_EEPROMSUPPORT
static void eeprom_test(void)
{
    uint8_t sp[DS18X20_SP_SIZE], th, tl;

	DS_DEBUG_PUTS( "\rDS18x20 EEPROM support test for fist sensor\r" );
	// DS18X20_recall_E2(&gTempSensorIDs[0][0]); // already done at power-on
	DS18X20_read_scratchpad( &gTempSensorIDs[0][0], sp);
	th = sp[DS18X20_TH_REG];
	tl = sp[DS18X20_TL_REG];
	DS_DEBUG_PUTS( "TH/TL from EEPROM sensor 1 : " );
	uart_puti(th);
	DS_DEBUG_PUTS( " / " );
	uart_puti(tl);
	DS_DEBUG_PUTS( "\r" );
	tl++; th++;
	DS18X20_write_scratchpad( &gTempSensorIDs[0][0],
		th, tl, DS18B20_12_BIT);
	DS_DEBUG_PUTS( "TH+1 and TL+1 written to scratchpad\r" );
	DS18X20_copy_scratchpad(  DS18X20_POWER_PARASITE,
		&gTempSensorIDs[0][0] );
	DS_DEBUG_PUTS( "scratchpad copied to DS18x20 EEPROM\r" );
	DS18X20_recall_E2(&gTempSensorIDs[0][0]);
	DS_DEBUG_PUTS( "DS18x20 EEPROM copied back to scratchpad\r" );
	DS18X20_read_scratchpad( &gTempSensorIDs[0][0], sp);
	if ( (th == sp[DS18X20_TH_REG]) && (tl == sp[DS18X20_TL_REG]) )
		DS_DEBUG_PUTS( "TH and TL verified\r" );
	else
		DS_DEBUG_PUTS( "verify failed\r" );
	th = sp[DS18X20_TH_REG];
	tl = sp[DS18X20_TL_REG];
	DS_DEBUG_PUTS( "TH/TL from EEPROM sensor 1 now : " );
	uart_puti(th);
	DS_DEBUG_PUTS( " / " );
	uart_puti(tl);
	DS_DEBUG_PUTS( "\r" );
}
#endif
*/



/*
int ds_test( void )
{
	uint8_t nSensors, i;
	uint8_t subzero, cel, cel_frac_bits;


#ifndef OW_ONE_BUS
	ow_set_bus(&PING,&PORTG,&DDRG,PG2);
#endif

	DS_DEBUG_PUTS( "\rDS18X20 1-Wire-Reader Demo by Martin Thomas\r" );
	DS_DEBUG_PUTS( "-------------------------------------------" );

	// dbg DS_DEBUG_PUTS( "\r" );
	// dbg uart_puti((int) OW_CONF_DELAYOFFSET);
	// dbg DS_DEBUG_PUTS( "\r" );

	nSensors = search_sensors();
	printf( "%d DS18X20 Sensor(s) available:\r", (int) nSensors );

#ifdef DS18X20_VERBOSE
	for (i=0; i<nSensors; i++) {
		DS_DEBUG_PUTS("# in Bus :");
		uart_puti((int) i+1);
		DS_DEBUG_PUTS(" : ");
		DS18X20_show_id_uart( &gTempSensorIDs[i][0], OW_ROMCODE_SIZE );
		DS_DEBUG_PUTS( "\r" );
	}
#endif

	for (i=0; i<nSensors; i++) {
		printf("Sensor# %d is a ", (int) i+1);
		if ( gTempSensorIDs[i][0] == DS18S20_ID) DS_DEBUG_PUTS("DS18S20/DS1820");
		else DS_DEBUG_PUTS("DS18B20");
		DS_DEBUG_PUTS(" which is ");
		if ( DS18X20_get_power_status( &gTempSensorIDs[i][0] ) ==
			DS18X20_POWER_PARASITE )
			DS_DEBUG_PUTS( "parasite" );
		else DS_DEBUG_PUTS( "externally" );
		DS_DEBUG_PUTS( " powered\r" );
	}

#ifdef DS18X20_EEPROMSUPPORT
	if (nSensors>0) {
		eeprom_test();
	}
#endif


	for(;;) {				// main loop

            if ( nSensors == 1 )
            {
                DS_DEBUG_PUTS( "\rThere is only one sensor -> Demo of \"read_meas_single\":\r" );
                i = gTempSensorIDs[0][0]; // family-code for conversion-routine
                DS18X20_start_meas( DS18X20_POWER_PARASITE, NULL );
                NutSleep(DS18B20_TCONV_12BIT);
                DS18X20_read_meas_single(i, &subzero, &cel, &cel_frac_bits);
                uart_put_temp(subzero, cel, cel_frac_bits);
                DS_DEBUG_PUTS("\r");
            }

            //DS_DEBUG_PUTS( "\rConvert_T and Read Sensor by Sensor (reverse order)\r" );
            DS_DEBUG_PUTS( "\rConvert_T and Read Sensor by Sensor (reverse order)\r" );
            for ( i=nSensors; i>0; i-- ) {
			if ( DS18X20_start_meas( DS18X20_POWER_PARASITE,
				&gTempSensorIDs[i-1][0] ) == DS18X20_OK ) {
				NutSleep(DS18B20_TCONV_12BIT);
				printf("Sensor# %d =", (int) i);
				if ( DS18X20_read_meas( &gTempSensorIDs[i-1][0], &subzero,
						&cel, &cel_frac_bits) == DS18X20_OK ) {
					uart_put_temp(subzero, cel, cel_frac_bits);
				}
				else DS_DEBUG_PUTS("CRC Error (lost connection?)");
				DS_DEBUG_PUTS("\r");
			}
			else DS_DEBUG_PUTS("Start meas. failed (short circuit?)");
		}

		DS_DEBUG_PUTS( "\rConvert_T for all Sensors and Read Sensor by Sensor\r" );
		if ( DS18X20_start_meas( DS18X20_POWER_PARASITE, NULL )
			== DS18X20_OK) {
			NutSleep(DS18B20_TCONV_12BIT);
			for ( i=0; i<nSensors; i++ ) {
				printf("Sensor# %d =",(int) i+1);
				if ( DS18X20_read_meas( &gTempSensorIDs[i][0], &subzero,
						&cel, &cel_frac_bits) == DS18X20_OK ) {
					uart_put_temp(subzero, cel, cel_frac_bits);
				}
				else DS_DEBUG_PUTS("CRC Error (lost connection?)");
				DS_DEBUG_PUTS("\r");
			}
		}
		else DS_DEBUG_PUTS("Start meas. failed (short circuit?)");

#ifdef DS18X20_VERBOSE
		// all devices:
		DS_DEBUG_PUTS( "\rVerbose output\r" );
		DS18X20_start_meas( DS18X20_POWER_PARASITE, NULL );
		NutSleep(DS18B20_TCONV_12BIT);
		DS18X20_read_meas_all_verbose();
#endif

		//SLEEP3SEC();
	}
}
*/
#endif // DS TEST


#endif // N_TEMPERATURE_IN > 0

