/**
 *
 * DZ-MMNET-MODBUS: Modbus/TCP I/O module based on MMNet101.
 *
 * Main loop.
 *
**/

#include "defs.h"
#include "util.h"


#include <string.h>
#include <io.h>

#include <modbus.h>

#include <sys/nutconfig.h>
#include <sys/thread.h>
#include <sys/timer.h>

#include "runtime_cfg.h"
#include "adc.h"
#include "dports.h"
//#include "temperature.h"
//#include "icp.h"
//#include "freq.h"
#include "util.h"
//#include "dallas.h"
//#include "ds2482.h"
//#include "spi.h"
//#include "child-1wire.h"
#include "dht.h"


//HANDLE sendOutEvent;

//void triggerSendOut(void)
//{
//    NutEventPostAsync(&sendOutEvent);
//}

static volatile uint8_t temperatureMeterCnt = 0;
static volatile uint8_t dht11meterCnt = 0;
static uint8_t dht11_errorCnt = 0;

void each_second(HANDLE h, void *arg)
{
    //temp_meter_sec_timer();
    //NutEventPostAsync(&temperatureMeterEvent);
    temperatureMeterCnt++;
    dht11meterCnt++;
}







THREAD(main_loop, arg)
{
    static uint16_t modbus_event_cnt_prev = 0;
    char had_io = 0;

    for (;;)
    {
        had_io = 0; // TODO http requests must do too
        LED_OFF; // at least 10 msec of LED off
        NutSleep(10);
        LED_ON;

        // Give other threads some chance to work
        NutSleep(90);

        //NutEventWait(&sendOutEvent, 100); // Once in 100 msec try sending anyway

        // No IO?
        if( modbus_event_cnt_prev != modbus_event_cnt )
        {
            modbus_event_cnt_prev = modbus_event_cnt;
            had_io = 1;
        }

        if(!had_io )
            LED_OFF;



#if SERVANT_DHT11
        if(dht11meterCnt > 0)
        {
            int8_t rc = dht_getdata( &dht_temperature, &dht_humidity );

            if( rc )
            {
                dht11_errorCnt++;
                if( dht11_errorCnt > 10 )
                {
                    dht11_errorCnt = 1; // no roll-over 255
                    dht_temperature = -1;
                    dht_humidity = -1;
                }
                dht11meterCnt -= 1; // Error? Redo in 1 sec
            }
            else
            {
                dht11_errorCnt = 0;
                dht11meterCnt -= 2; // Ok? Once in 2 secs
            }
        }
#endif // SERVANT_DHT11


//        send_changed_adc_data();
//        send_changed_dig_data();
//        send_changed_freq_data();

#if 0
        //temp_meter_sec_timer();
        //NutEventPostAsync(&temperatureMeterEvent);
        if(temperatureMeterCnt)
        {
            temperatureMeterCnt--;
            temp_meter_sec_timer();

            // test
            ch1w_read_temperature( 0 );

        }
#endif

#if N_TEMPERATURE_IN > 0
        send_changed_temperature_data();
#endif





    }

}






