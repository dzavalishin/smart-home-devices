/**
 *
 * DZ-MMNET-MODBUS: Modbus/TCP I/O module based on MMNet101.
 *
 * Main loop.
 *
**/

#include "defs.h"
#include "util.h"
#include "servant.h"
#include "runtime_cfg.h"

#include <string.h>
#include <io.h>

#include <modbus.h>

#include <sys/nutconfig.h>
#include <sys/thread.h>
#include <sys/timer.h>


//#include "io_dht.h"
#include "io_dig.h"
#include "io_adc.h"
#include "io_pwm.h"
#include "io_temp.h"
//#include "io_bmp180.h"

#define TEMPERATURE_RESCAN_SEC 240

static uint8_t 		second_counter = 0; // general per second counter, used by fail led
static uint8_t 		network_activity = 60; // Will wait 1 min before initial network io not showing failure

static volatile uint8_t temperatureMeterCnt = 0;
static volatile uint8_t temperatureRescanCnt = 0;

static volatile uint8_t dht11meterCnt = -2; // Give sensor 2 seconds to become working
uint8_t 		dht11_errorCnt = 0;

void each_second(HANDLE h, void *arg)
{
    second_counter++;
    temperatureMeterCnt++;
    dht11meterCnt++;

    // Will become 0 right now, point to trigger connection lost action
    if( network_activity == 1 )
    {
        dio_set_default_output_state();
        //pwm_set_default_output_state();
    }

    if( network_activity > 0 )
        network_activity--;

    if( 0 == network_activity )
    {
        if( second_counter & 1 )
            FAIL_LED_ON;
        else
            FAIL_LED_OFF;
    }
    else
        FAIL_LED_OFF;

#if ENABLE_SPI && 0
        static char a = 0xEF, b = 0x01;
        {
            a += 1;
            b += 2;

            //spi_send( a, b );

            test_spi();

            //NutSleep(1000); // remove
        }
#endif

}







THREAD(main_loop, arg)
{
    static uint16_t modbus_event_cnt_prev = 0;
    char had_io = 0;
    char count = 0;

    for (;;)
    {
        had_io = 0; // TODO http requests must do too

        if( had_io || network_activity || ( (count & 3) == 0 ) )
            LED_ON;

        NutSleep(10);


        // Give other threads some chance to work
        NutSleep(90);
        LED_OFF; // at least 10 msec of LED off

        //NutEventWait(&sendOutEvent, 100); // Once in 100 msec try sending anyway

        // No IO?
        if( modbus_event_cnt_prev != modbus_event_cnt )
        {
            modbus_event_cnt_prev = modbus_event_cnt;
            had_io = 1;
            notice_activity();
        }

        //if(!had_io )            LED_OFF;



#if SERVANT_DHT11
        if(dht11meterCnt > 0)
        {
            int8_t rc = dht_getdata( &dht_temperature, &dht_humidity );

            if( rc )
            {
                dht11_errorCnt++;
                if( dht11_errorCnt > 10 )
                {
                    dht11_errorCnt = 10; // no roll-over 255
                    dht_temperature = ERROR_VALUE_16;
                    dht_humidity = ERROR_VALUE_16;
                }
                dht11meterCnt -= 2; // Error? Redo in 2 sec
            }
            else
            {
                dht11_errorCnt = 0;
                dht11meterCnt -= 6; // Ok? Once in 6 secs
            }
        }
#endif // SERVANT_DHT11

#if SERVANT_BMP180
        if( bmp180_getdata() )
        {
            bmp180_temperature   = ERROR_VALUE_32;
            bmp180_pressure      = ERROR_VALUE_32;
            bmp180_pressure_mmHg = ERROR_VALUE_16;
        }
#endif // SERVANT_BMP180

#if SERVANT_NTEMP
        if( temperatureRescanCnt <= 0 )
        {
            rescan_temperature();
            temperatureRescanCnt = TEMPERATURE_RESCAN_SEC;
        }

        if( temperatureMeterCnt > 0 )
        {
#if ENABLE_SPI
            {
                //test_spi();
                //spi_send( 0x55, 0xAA );
                static char a = 0xEF, b = 0x01;

                a += 2;
                b += 1;

            //spi_send( a, b );
                spi_send( a, b );
            }
#endif
//            printf(".");
            temp_meter_measure();
            temperatureMeterCnt = 0; // Can lead to less than sec delay to next call. Ignore?
        }
#endif


    }

}



void notice_activity() // Called by network code to mark that something uses us as IO. If not - we will light fail LED.
{
    network_activity = 30; // Seconds between network calls
}


