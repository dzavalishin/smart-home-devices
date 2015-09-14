/**
 *
 * DZ-MMNET-MODBUS: Modbus/TCP I/O module based on MMNet101.
 *
 * Main: startup code.
 *
**/

#include <dev/lanc111.h>

#include "defs.h"
#include "util.h"
#include "net_io.h"
#include "runtime_cfg.h"

#include <dev/nicrtl.h>
#include <dev/debug.h>
#include <dev/urom.h>
#include <dev/twif.h>

#include <sys/nutconfig.h>
#include <sys/version.h>
#include <sys/thread.h>
#include <sys/timer.h>

#include <sys/confnet.h>
#include <sys/socket.h>
#include <sys/event.h>

#include <arpa/inet.h>

#include <pro/dhcp.h>
#include <pro/sntp.h>

#ifdef NUTDEBUG
#include <sys/osdebug.h>
#include <net/netdebug.h>
#endif

#include <io.h>

#include <sys/syslog.h>

#include "cgi.h"

#include "web.h"

#include "modbus_srv.h"
#include <modbus.h>


#include "io_dig.h"
#include "io_adc.h"
#include "io_temp.h"
#include "io_bmp180.h"

#include "onewire.h"

// NB - contains var def and init
#include "makedate.h"


static int tryToFillMac(unsigned char *mac, unsigned char *oneWireId);

static void init_devices(void);

static void init_net(void);
static void init_cgi(void);
static void init_httpd(void);
static void init_sntp(void);

#if ENABLE_SYSLOG
static void init_syslog(void);
#endif

// in servant.c
THREAD(main_loop, arg); 
void each_second(HANDLE h, void *arg);






THREAD(long_init, __arg)
{
    while(1) // to satisfy no return
    {
        // SNTP kills us :(
#if ENABLE_SNTP
        init_sntp();
#endif

//        _timezone = -((long)ee_cfg.timezone) * 60L * 60L;
//        _daylight = 0; // No DST in Russia now
#if ENABLE_SYSLOG
        init_syslog();
#endif

#if SERVANT_TCP_COM0 || SERVANT_TCP_COM1
        init_tcp_com();
#endif

#if SERVANT_NTEMP > 0
        init_temperature();
#endif

#if ENABLE_SPI
        char a = 0xEF, b = 0x01;
        while( 1 )
        {
            a += 1;
            b += 2;

            spi_send( a, b );

            test_spi();

            NutSleep(1000); // remove
        }
#endif

        NutThreadExit();
    }
}







/*!
 * \brief Main application routine.
 *
 * Nut/OS automatically calls this entry after initialization.
 */
int main(void)
{

    u_long baud = 38400;

    NutThreadSetSleepMode(SLEEP_MODE_IDLE); // Let the CPU sleep in idle

    // DO VERY EARLY!
    init_runtime_cfg();         // Load defaults
    runtime_cfg_eeprom_read();  // Now attempt to load saved state

    led_ddr_init(); // Before using LED!
    LED_ON;

    set_half_duplex0(0);
    set_half_duplex1(1);

    // Initialize the uart device.
#if 1
    NutRegisterDevice(&devDebug1, 0, 0); // USB
    freopen("uart1", "w", stdout);
#else
    NutRegisterDevice(&devDebug0, 0, 0); // RS232
    freopen("uart0", "w", stdout);
#endif

    _ioctl(_fileno(stdout), UART_SETSPEED, &baud);
    NutSleep(50);
    printf("\n\nController %s, Nut/OS %s, build from %s\n", DEVICE_NAME, NutVersionString(), makeDate );


#if 0 || defined(NUTDEBUG)
    NutTraceTcp(stdout, 0);
    NutTraceOs(stdout, 0);
    NutTraceHeap(stdout, 0);
    NutTracePPP(stdout, 0);
#endif

    // We need it here because we use 1-wire 2401 serial as MAC address
    init_devices();

    init_net();


    _timezone = (((long)ee_cfg.timezone) * 60L * 60L);
    //_timezone = -(((long)ee_cfg.timezone) * 60L * 60L);
    //_timezone = ((long)1) * 60L * 60L;
    _daylight = 0; // No DST in Russia now
    NutThreadCreate("LongInit", long_init, 0, 2640);


    // Register our device for the file system.
    NutRegisterDevice(&devUrom, 0, 0);

    init_cgi();
    init_httpd();

    printf("httpd ready\n");



    modbus_init( 9600, 1 ); // we don't need both parameters, actually
    NutThreadCreate( "ModBusTCP", ModbusService, (void *) 0, 640);
    NutThreadSetPriority(254);
    NutTimerStart(1000, each_second, 0, 0 ); // Each second call each_second

    LED_OFF;

    NutThreadCreate("MainLoop", main_loop, 0, 640);

    for (;;)
    {
        NutSleep(1000);
    }

    return 33;
}


static void init_net(void)
{
    char tries = 5;


    // Register Ethernet controller
    if (NutRegisterDevice(&DEV_ETHER, 0xFF00, 5))
    {
        puts("Registering Ethernet failed");
        fail_led();
        return;
    }



    // TODO fixme
#if SERVANT_1WMAC
    tryToFillMac( ee_cfg.mac_addr, serialNumber ); // do not try to use 18b20 as MAC addr
#if 0
    if( !tryToFillMac( ee_cfg.mac_addr, serialNumber ) )
    {
        int tno;
        for( tno = 0; tno < SERVANT_NTEMP; tno++ )
            if( tryToFillMac( ee_cfg.mac_addr, gTempSensorIDs[tno] ) )
                break;
    }
#endif
#endif

    // LAN configuration using EEPROM values or DHCP/ARP method.
    // If it fails, use fixed values.


    printf("Configure LAN... ");

    // No. Prevents DHCP.
    //confnet.cdn_cip_addr = ee_cfg.ip_addr; // OS will use it as default if no DHCP - do we need NutNetIfConfig?

    while( tries-- > 0 )
    {
        if( 0 == NutDhcpIfConfig("eth0", ee_cfg.mac_addr, 60000) )
            goto dhcp_ok;

        puts("EEPROM/DHCP/ARP config failed");

        LED_OFF;
        NutSleep(320);
        flash_led_once();
    }

    // Use static ip address
    NutNetIfConfig("eth0", ee_cfg.mac_addr, ee_cfg.ip_addr, ee_cfg.ip_mask);



dhcp_ok:
    printf("Network ready, addr=%s\n", inet_ntoa(confnet.cdn_ip_addr));

}


static void init_cgi(void)
{

    // Register our CGI sample. This will be called  by http://host/cgi-bin/test.cgi?anyparams
    //NutRegisterCgi("test.cgi", ShowQuery );

    // Register some CGI samples, which display interesting system informations.
    NutRegisterCgi("threads.cgi", ShowThreads);
    NutRegisterCgi("timers.cgi", ShowTimers);
    NutRegisterCgi("sockets.cgi", ShowSockets);

    // Register our app CGI - i/o/net reports
    NutRegisterCgi("inputs.cgi", CgiInputs);
    NutRegisterCgi("outputs.cgi", CgiOutputs);

    NutRegisterCgi("network.cgi", CgiNetwork);

    // Web config and general status
    NutRegisterCgi("form.cgi", ShowForm);
    NutRegisterCgi("status.cgi", CgiStatus);

    // OpenHAB integration - read (/write&) value with http
    NutRegisterCgi("item.cgi", CgiNetIO );

    // Protect the cgi-bin directory with user and password.
    //NutRegisterAuth("cgi-bin", "root:root");

}

static void init_httpd(void)
{
    uint8_t i;

    for (i = 1; i <= 6; i++)
    {
        char *thname = "httpd0";

        thname[5] = '0' + i;
        NutThreadCreate(thname, Service, (void *) (uptr_t) i, 640);
    }

}

#if ENABLE_SNTP
static void init_sntp(void)
{
    uint32_t timeserver = confnet.cdn_gateway; // inet_addr(MYTIMED);
    time_t now;

    if(NutSNTPGetTime(&timeserver, &now) == 0)
    {
        stime(&now);
        puts("Got SNTP time");
        sntp_available = 1;
    }
}
#endif

#if ENABLE_SYSLOG
static void init_syslog(void)
{
    openlog( DEVICE_NAME, LOG_PERROR, LOG_USER );
    setlogserver( ee_cfg.ip_syslog, 0 );
    syslog( LOG_INFO, "%s started on Nut/OS %s, build from %s", DEVICE_NAME, NutVersionString(), makeDate );
}
#endif


static int tryToFillMac(unsigned char *mac, unsigned char *oneWireId)
{
#if SERVANT_1WMAC
    int i;
    char found = 0;
    for( i = 0; i < OW_ROMCODE_SIZE; i++ )
        if(oneWireId[i] != 0 )
        {
            found = 1;
            break;
        }

    if( !found )
        return -1;

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
    return 0;
#else
    return -1;
#endif
}







// Initialize all peripherals

void init_devices(void)
{
#if ENABLE_TWI
    {
        unsigned long busspeed = 10000;	// 10kHz
        //unsigned long busspeed = 25;	// 50 Hz

        printf("I2C init...");

        TwInit(0);
        TwIOCtl(TWI_SETSPEED, &busspeed);
        TwIOCtl(TWI_GETSPEED, &busspeed);
        printf(" done, speed is %ld\n", busspeed);
    }
#endif // ENABLE_TWI



    //stop errant interrupts until set up
    cli(); //disable all interrupts


    dio_init();
#if SERVANT_NADC > 0
    adc_init();
#endif


#if SERVANT_NPWM > 0
    timer1_init();
#endif


#if SERVANT_NFREQ > 0
    //freq_meter_init();
    icp_init();
#endif

    sei(); //re-enable interrupts

#if 0
    DEBUG_PUTS("sending temperature from init... ");
    temp_meter_sec_timer();
    NutSleep(1000);
    DEBUG_PUTS("sending temperature from init... ");
    temp_meter_sec_timer();
#endif

    set_half_duplex0(0);
    set_half_duplex1(0);

#if ENABLE_SPI
    spi_init();
    //printf("SPI = 0x%02X\n", spi_send( 0x80, 0 )); // read reg 0, whoami

    spi_send( 0x01, 0xFF );
    spi_send( 0x07, 0xAA );

    spi_send( 0x33, 0xFF );
    spi_send( 0x77, 0xAA );

    spi_send( 0xFF, 0x55 );
    spi_send( 0xEE, 0x0A );

#endif

    set_half_duplex0(1);
    set_half_duplex1(1);

#if SERVANT_BMP180
    bmp180_calibration();
#endif // SERVANT_BMP180

    //all peripherals are now initialized

#if SERVANT_NADC > 0
    adc_start();
#endif

#if SERVANT_NPWM > 0
    timer1_start();
#endif





}








