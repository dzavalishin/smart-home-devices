/**
 *
 * DZ-MMNET-CHARGER: Acc charger module based on MMNet101.
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


static uint8_t 		second_counter = 0; // general per second counter, used by fail led
static uint8_t 		network_activity = 60; // Will wait 1 min before initial network io not showing failure

//static volatile uint8_t temperatureMeterCnt = 0;
//static volatile uint8_t temperatureRescanCnt = 0;

//static volatile uint8_t dht11meterCnt = -2; // Give sensor 2 seconds to become working
//uint8_t 		dht11_errorCnt = 0;

void each_second(HANDLE h, void *arg)
{
    second_counter++;
    //temperatureMeterCnt++;
    //dht11meterCnt++;

    // Will become 0 right now, point to trigger connection lost action
    if( network_activity == 1 )
    {
        //dio_set_default_output_state();
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




/*
#if SERVANT_NTEMP
        if( temperatureRescanCnt <= 0 )
        {
            rescan_temperature();
            temperatureRescanCnt = TEMPERATURE_RESCAN_SEC;
        }

        if( temperatureMeterCnt > 0 )
        {
//            printf(".");
            temp_meter_measure();
            temperatureMeterCnt = 0; // Can lead to less than sec delay to next call. Ignore?
        }
#endif
*/

    }

}



void notice_activity() // Called by network code to mark that something uses us as IO. If not - we will light fail LED.
{
    network_activity = 30; // Seconds between network calls
}


