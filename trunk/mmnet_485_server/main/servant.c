/*!
 * Gardemarine servant code for MMNet101 UDP.
 */

/* These values are used if there is no valid configuration in EEPROM. */
//#define MYMAC   0x00, 0x06, 0x98, 0x00, 0x00, 0x00
#define MYMAC   0x02, 0x06, 0x98, 0x00, 0x00, 0x00
#define MYIP    "192.168.1.200"
#define MYMASK  "255.255.255.0"


#include <string.h>
#include <io.h>

#ifdef ETHERNUT2
#include <dev/lanc111.h>
#else
#include <dev/nicrtl.h>
#endif

#include <dev/debug.h>
#include <dev/urom.h>

#include <dev/twif.h>


#include <sys/nutconfig.h>
#include <sys/version.h>
#include <sys/thread.h>
#include <sys/timer.h>
//#include <sys/heap.h>
#include <sys/confnet.h>
#include <sys/socket.h>
#include <sys/event.h>

#include <arpa/inet.h>

#include <pro/dhcp.h>

#ifdef NUTDEBUG
#include <sys/osdebug.h>
#include <net/netdebug.h>
#endif

#include "defs.h"
#include "util.h"
//#include "net_io.h"

#include "runtime_cfg.h"

//#include "adc.h"
//#include "dports.h"
//#include "temperature.h"
//#include "icp.h"
//#include "freq.h"

#include "util.h"
//#include "dallas.h"
//#include "ds2482.h"

#include "spi.h"

//#include "child-1wire.h"

#include "oscgi.h"
#include "cgi.h"

#include "web.h"




u_char mac[] = { MYMAC };






void init_devices(void);














// Called from timer once in 5 seconds
static void  pong_sender(HANDLE h, void *arg)
{
//    net_request_pong();
}




volatile uint8_t temperatureMeterCnt;

static void each_second(HANDLE h, void *arg)
{
    //temp_meter_sec_timer();
    //NutEventPostAsync(&temperatureMeterEvent);
    temperatureMeterCnt++;
}




HANDLE sendOutEvent;

void triggerSendOut(void)
{
    //NutEventPostFromIrq(&sendOutEvent);
    NutEventPostAsync(&sendOutEvent);
}




// Returns nonzero on success
static int tryToFillMac(char *mac, char *oneWireId)
{
#if 0
    int i;
    char found = 0;
    for( i = 0; i < OW_ROMCODE_SIZE; i++ )
        if(oneWireId[i] != 0 )
        {
            found = 1;
            break;
        }

    if( !found )
        return 0;

    // Ow romcode is 8 bytes, mac is 6, we use 5 lowest ones

    for( i = 0; i < 5; i++ )
    {
        mac[i+1] = oneWireId[i+1];
    }
    mac[0] = 0x2;

    printf("Made MAC from 1wire id: ");
    for( i = 0; i < 6; i++ )
    {
        if(i != 0 ) putchar('-');
        printf("%02X", 0xFFu & mac[i]);
    }
    putchar('\n');
    return 1;
#else
    return 0;
#endif
}





THREAD(main_loop, arg)
{
    for (;;)
    {
        LED_ON;

        // Give others some chance to work - I mean both threads and
        // other net nodes
        NutSleep(10);
        //LED_OFF; // at least 10 msec of LED ON
        NutEventWait(&sendOutEvent, 100); // Once in 100 msec try sending anyway


        LED_OFF;

#if 0
        send_changed_adc_data();
        send_changed_dig_data();

#if SERVANT_NFREQ > 0
        send_changed_freq_data();
#endif
#endif

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

        /* */

        send_protocol_replies();


#if TEST_PWM
        OCR2 += 1; // temp - pwm duty change
#endif



    }

}








/*!
 * \brief Main application routine.
 *
 * Nut/OS automatically calls this entry after initialization.
 */
int main(void)
{
    //    u_long baud = 115200;
    u_long baud = 38400;
    //u_char i;

    NutThreadSetSleepMode(SLEEP_MODE_IDLE); // Let the CPU sleep in idle

    led_ddr_init(); // Before using LED!
    LED_ON;

    /*
     * Initialize the uart device.
     */
#if defined(__AVR_ATmega128__) || defined(__AVR_ATmega103__) || 1

#if 1
    NutRegisterDevice(&devDebug1, 0, 0); // USB
    freopen("uart1", "w", stdout);
#else
    NutRegisterDevice(&devDebug0, 0, 0); // RS232
    freopen("uart0", "w", stdout);
#endif

#else
    NutRegisterDevice(&devDebug2, 0, 0);
    freopen("sci2dbg", "w", stdout);
#endif
    _ioctl(_fileno(stdout), UART_SETSPEED, &baud);
    NutSleep(200);
    printf("\n\nGardemarine MMNet101 controller, Nut/OS %s\n", NutVersionString());


#if 0 || defined(NUTDEBUG)
    NutTraceTcp(stdout, 0);
    NutTraceOs(stdout, 0);
    NutTraceHeap(stdout, 0);
    NutTracePPP(stdout, 0);
#endif

    // We need it here because we use 1-wire 2401 serial as MAC address
    init_devices();

    /*
     while(1) { //sei();
     flash_led_once();
     //LED_ON;
     printf("aft fl l once");
     //NutSleep(200);
     printf("aft sleep 200");
     }*/


    /*
     * Register Ethernet controller.
     */
#if defined(__AVR_ATmega128__) || defined(__AVR_ATmega103__)
    //if (NutRegisterDevice(&DEV_ETHER, 0x8300, 5))
    if (NutRegisterDevice(&DEV_ETHER, 0xFF00, 5))
#else
        if (NutRegisterDevice(&devEth0, NIC_IO_BASE, 0))
#endif
        {
            puts("Registering device failed");
            fail_led();
        }


    {
        u_long ip_addr = inet_addr(MYIP);
        u_long ip_mask = inet_addr(MYMASK);

#if 0
        if( !tryToFillMac( mac, serialNumber ) )
        {
            int tno;
            for( tno = 0; tno < N_TEMPERATURE_IN; tno++ )
                if( tryToFillMac( mac, gTempSensorIDs[tno] ) )
                    break;
        }
#endif
        /*
         * LAN configuration using EEPROM values or DHCP/ARP method.
         * If it fails, use fixed values.
         */
#if 1
        //if (NutDhcpIfConfig("eth0", mac, 60000)) {
        while(NutDhcpIfConfig("eth0", mac, 60000)) {

            puts("EEPROM/DHCP/ARP config failed");
            //NutNetIfConfig("eth0", mac, ip_addr, ip_mask);
            LED_OFF;
            NutSleep(320);
            //LED_ON;
            flash_led_once();

            cli();
            __asm__ __volatile__("jmp 0");
        }
#else
        {
            NutNetIfConfig("eth0", mac, ip_addr, ip_mask);
        }
#endif
        printf("%s ready\n", inet_ntoa(confnet.cdn_ip_addr));

    }
#if 1
    /*
     * Register our device for the file system.
     */
    NutRegisterDevice(&devUrom, 0, 0);

    /*
     * Register our CGI sample. This will be called
     * by http://host/cgi-bin/test.cgi?anyparams
     */
    NutRegisterCgi("test.cgi", ShowQuery);

    /*
     * Register some CGI samples, which display interesting
     * system informations.
     */
    NutRegisterCgi("threads.cgi", ShowThreads);
    NutRegisterCgi("timers.cgi", ShowTimers);
    NutRegisterCgi("sockets.cgi", ShowSockets);

    NutRegisterCgi("inputs.cgi", CgiInputs);
    NutRegisterCgi("outputs.cgi", CgiOutputs);

    NutRegisterCgi("network.cgi", CgiNetwork);

    /*
     * Finally a CGI example to process a form.
     */
    NutRegisterCgi("form.cgi", ShowForm);

    /*
     * Protect the cgi-bin directory with
     * user and password.
     */
    NutRegisterAuth("cgi-bin", "root:root");

    /*
     * Start 6 server threads. 4 is not enough.
     */
    {
        uint8_t i;

        for (i = 1; i <= 6; i++) {
            char *thname = "httpd0";

            thname[5] = '0' + i;
            NutThreadCreate(thname, Service, (void *) (uptr_t) i, 640);
        }
    }
#endif


    NutThreadSetPriority(254);

    NutTimerStart(5000, pong_sender, 0, 0 ); // Each 5 seconds send pong packet

    NutTimerStart(1000, each_second, 0, 0 ); // Each second call each_second

    LED_OFF;

    NutThreadCreate("MainLoop", main_loop, 0, 640);

    for (;;)
    {
        NutSleep(1000);
    }

}


void pwm_init(void); // temp


//dallas_rom_id_T dallas_roms[DALLAS_MAX_DEVICES];

//call this routine to initialize all peripherals
void init_devices(void)
{
    {
        unsigned long busspeed = 10000;	// 10kHz
        //unsigned long busspeed = 25;	// 50 Hz
        printf("I2C init...");
        //i2cInit();
        TwInit(0);
        TwIOCtl(TWI_SETSPEED, &busspeed);

        TwIOCtl(TWI_GETSPEED, &busspeed);

        printf(" done, speed is %ld\n", busspeed);
    }

#if 0
    child_1wire_init();

    {
        printf("DS2482 i2c/1wire bridge init...");

        onewire_2482_available = !ds2482Init((DS2482_I2C_ADDR+0x6)>>1);


        printf(" %s\n", (!onewire_2482_available) ? "err" : "ok");

        ds2482SendCmdArg(DS2482_CMD_WCFG,
                         (0xF0 | DS2482_CFG_APU) & ~(DS2482_CFG_APU<<4)
                        );


        //ds2482SendCmdArg(DS2482_CMD_WCFG, 0xF0 ); // Most default mode

        printf("DS2482 cfg: 0x%02X\n", ds2482ReadConfig() );

        uint8_t status;
        ds2482BusyWait(&status);
        printf("DS2482 status: 0x%02X\n", status );

        printf("1wire: %s devices\n", dallasReset() == DALLAS_PRESENCE ? "have" : "no" );
    }
    //dallasFindDevices(dallas_roms, DALLAS_MAX_DEVICES);
    //dallasFindDevices(dallas_roms, 4);

    init_udp_net_io();
#endif


    //stop errant interrupts until set up
    cli(); //disable all interrupts

    //MCUCR = 0x00;
    //GICR  = 0x00;
#ifdef __AVR_ATmega128__
    //EIMSK = 0; // external interrupts off
#endif

    //init_alarm();

//    port_init();

//    adc_init();

    // ������� 0, 1
#if SERVANT_NPWM > 0
    //timer1_init();
#endif


#if SERVANT_NFREQ > 0
    //freq_meter_init();
    icp_init();
#endif

#if N_TEMPERATURE_IN > 0
    init_temperature();
#endif
    sei(); //re-enable interrupts

#if 0
    DEBUG_PUTS("sending temperature from init... ");
    temp_meter_sec_timer();
    NutSleep(1000);
    DEBUG_PUTS("sending temperature from init... ");
    temp_meter_sec_timer();
#endif


#if ENABLE_SPI
    spi_init();
    printf("SPI = 0x%02X\n", spi_send( 0x80, 0 )); // read reg 0, whoami
#endif


    //all peripherals are now initialized

#if SERVANT_NADC > 0
    adc_start();
#endif

#if SERVANT_NPWM > 0
    //timer1_start();
#endif


#if TEST_PWM
    pwm_init();
#endif



}



