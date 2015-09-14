/**
 *
 * DZ-MMNET-MODBUS: Modbus/TCP I/O module based on MMNet101.
 *
 * Main loop.
 *
**/

#include "defs.h"
#include "util.h"
#include "runtime_cfg.h"

#include <string.h>
#include <io.h>

#include <modbus.h>

#include <sys/nutconfig.h>
#include <sys/thread.h>
#include <sys/timer.h>


#include "io_dht.h"
#include "io_dig.h"
#include "io_adc.h"
#include "io_temp.h"
#include "io_bmp180.h"


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

#if SERVANT_BMP180
        bmp180_getdata(); // todo check success?
#endif // SERVANT_BMP180

#if SERVANT_NTEMP
        if( temperatureMeterCnt > 0 )
        {
            temp_meter_measure();
            temperatureMeterCnt = 0; // Can lead to less than sec delay to next call. Ignore?
        }
#endif


    }

}






