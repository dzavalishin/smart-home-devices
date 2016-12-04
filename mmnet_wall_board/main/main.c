/**
 *
 * DZ-MMNET-WALL: Wall control panel based on MMNet01.
 *
 * Main: startup code.
 *
**/

#ifdef MMNET101
#  include <dev/lanc111.h>
#else
#  include <dev/nicrtl.h>
#endif

#include "defs.h"
#include "util.h"
#include "runtime_cfg.h"
#include "servant.h"

#include <dev/nicrtl.h>
#include <dev/debug.h>
#include <dev/urom.h>
#include <dev/twif.h>
#include <dev/null.h>

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

#include "map.h"


#include "io_dig.h"
#include "io_pwm.h"
#include "io_temp.h"

#include "onewire.h"

#include "ui_lcd.h"


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



dev_major *devices[] =
{
    &io_dig,
    &io_pwm,
};


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

#if SERVANT_TUN0 || SERVANT_TUN1
        init_tunnels();
#endif

#if SERVANT_LUA
        lua_init();
#endif

#if ENABLE_SPI && 0
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

    //u_long baud = DEFAULT_BAUD;

    NutThreadSetSleepMode(SLEEP_MODE_IDLE); // Let the CPU sleep in idle

    // DO VERY EARLY!
    init_runtime_cfg();         // Load defaults
    runtime_cfg_eeprom_read();  // Now attempt to load saved state

    led_ddr_init(); // Before using LED!
    LED_ON;

    //set_half_duplex0(0);
    //set_half_duplex1(1);

    // Initialize the uart device.
    if( RT_IO_ENABLED(IO_LOG) )
    {
#if 1 // must be 1
#if !SERVANT_TUN1
        NutRegisterDevice(&devDebug1, 0, 0); // USB
        freopen("uart1", "w", stdout);
#endif
#else
#if !SERVANT_TUN0
        NutRegisterDevice(&devDebug0, 0, 0); // RS232
        freopen("uart0", "w", stdout);
#endif
#endif

        _ioctl(_fileno(stdout), UART_SETSPEED, &ee_cfg.dbg_baud);
        NutSleep(50);
        printf("\n\nController %s, Nut/OS %s, build from %s\n", DEVICE_NAME, NutVersionString(), makeDate );

#if 0 || defined(NUTDEBUG)
        NutTraceTcp(stdout, 0xFF);
        NutTraceOs(stdout, 0xFF);
        NutTraceHeap(stdout, 0xFF);
        NutTracePPP(stdout, 0xFF);
#endif
    }
    else
    {
        NutRegisterDevice(&devNull, 0, 0);
        freopen("null", "w", stdout);
    }

    // We need it here because we use 1-wire 2401 serial as MAC address
    init_devices();
    led_ddr_init(); // Before using LED!
    LED_ON;

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

    NutThreadExit();
    //for(;;) ; // make compiler happy
    for (;;)
    {
        NutSleep(1000);
    }

    return 33;
}


static void init_net(void)
{
//    char tries = 250; // make sure we are really have DHCP address

#if 0
    // Register Ethernet controller
    if (NutRegisterDevice(&DEV_ETHER, 0xFF00, 5))
    {
        puts("Registering Ethernet failed");
        fail_led();
        return;
    }
#endif


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
    // RTL needs this?
    memcpy(confnet.cdn_mac, ee_cfg.mac_addr, 6);

    //confnet.cdn_ip_addr = inet_addr( DEFAULT_IP );
    //confnet.cdn_ip_mask = inet_addr( DEFAULT_MASK );
    //confnet.cdn_cip_addr = confnet.cdn_ip_addr;

    // Register Ethernet controller -- RTL needs mac before?
    if (NutRegisterDevice(&DEV_ETHER, 0xFF00, 5))
    {
        puts("Registering Ethernet failed");
        fail_led();
        return;
    }

    // LAN configuration using EEPROM values or DHCP/ARP method.
    // If it fails, use fixed values.


    printf("Configure LAN %s... ", confnet.cd_name);

    // No. Prevents DHCP.
    //confnet.cdn_cip_addr = ee_cfg.ip_addr; // OS will use it as default if no DHCP - do we need NutNetIfConfig?
#if 1
    confnet.cdn_ip_addr = confnet.cdn_cip_addr = 0;
    // We do not work if no DHCP
    //while( tries-- > 0 )
    while( 1 )
    {
        if( 0 == NutDhcpIfConfig( confnet.cd_name, ee_cfg.mac_addr, 60000) )
        //if( 0 == NutDhcpIfConfig("eth0", ee_cfg.mac_addr, 60000) )
            goto dhcp_ok;

        puts("EEPROM/DHCP/ARP config failed");

        LED_OFF;
        NutSleep(3020);
        LED_ON;
        //flash_led_once();
    }
#endif
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
    //setlogserver( ee_cfg.ip_syslog, 0 );

    //_write(_fileno(stdout), "!!\n", 3);

    char buf[512];
    //sprintf( buf, "%s started on Nut/OS %s, build from %s", DEVICE_NAME, NutVersionString(), makeDate );
    //syslog( LOG_INFO, 0 );
    syslog( LOG_INFO, "!! mmnet !!", 0 );
    //syslog( LOG_INFO, "%s started on Nut/OS %s, build from %s", DEVICE_NAME, NutVersionString(), makeDate );
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

#if 0
    printf("Made MAC from 1wire id: ");
    for( i = 0; i < 6; i++ )
    {
        if(i != 0 ) putchar('-');
        printf("%02X", 0xFFu & mac[i]);
    }
    putchar('\n');
#endif

    return 0;
#else
    return -1;
#endif
}








#if 1

#define NMAJOR ( sizeof(devices) / sizeof(dev_major) )

static void init_regular_devices(void)
{
    dev_major *	dev;
    uint8_t	i;

    // Init
    for( i = 0; i < NMAJOR; i++ )
    {
        dev = devices[i];

        dev->init( dev );
    }
}

static void start_regular_devices(void)
{
    dev_major *	dev;
    uint8_t	i;

    // Start
    for( i = 0; i < NMAJOR; i++ )
    {
        dev = devices[i];
        dev->started = ! dev->start( dev );
    }
}


void timer_regular_devices(void)
{
    dev_major *	dev;
    uint8_t	i;

    for( i = 0; i < NMAJOR; i++ )
    {
        dev = devices[i];

        if( dev->started )
            dev->timer( dev );
    }

}

// TODO call on manual reboot
void stop_regular_devices(void)
{
    dev_major *	dev;
    uint8_t	i;

    for( i = 0; i < NMAJOR; i++ )
    {
        dev = devices[i];

        if( dev->started )
            dev->stop( dev );

        dev->started = 0;
    }

}


#endif













// Initialize all peripherals

void init_devices(void)
{



    //stop errant interrupts until set up
    cli(); //disable all interrupts


    dio_init();


    //lcd_init();
    //encoder_init(); menu_init();


#if SERVANT_NPWM > 0
    //printf(" pwm init...");
    //timer1_init();
#endif



    sei(); //re-enable interrupts


#if SERVANT_NTEMP > 0
    printf(" 1w init...");
    init_temperature();
#endif



#if SERVANT_NPWM > 0
    //printf(" timer1 start...");
    //timer1_start();
#endif

    init_regular_devices();
    start_regular_devices();

    printf(" DONE\n");


}









