#include "defs.h"
#include "util.h"

#include <inttypes.h>
#include <avr/io.h>

#include <sys/confnet.h>

#include <time.h>
#include <stdio.h>

#include "ui_menu.h"
#include "ui_lcd.h"

//#include "modbus.h"
//#include "mqtt.h"

#include "io_temp.h"
#include "io_dig.h"

#include "runtime_cfg.h"

// ---------------------------------------------------------------
//
// Menu system common state
//
// ---------------------------------------------------------------

// TODO optimize
//#define MENU_LEN 9
static uint8_t getMenuLen(void);
#define MENU_LEN getMenuLen()

static void menu_read_input(void);

void menu_event_saved(void);
void menu_display_saved(void);

void menu_event_main(void);
void menu_display_main(void);
void menu_monitor_main(void);
void put_menu_item( uint8_t ip );

void select_menu( struct menu_t *m );


extern struct menu_t menu[];

struct menu_t menu_main = { "Main", "", menu_display_main, menu_event_main, menu_monitor_main };
struct menu_t *curr_menu = &menu_main;

struct menu_t menu_saved = { "Saved", "", menu_display_saved, menu_event_saved, menu_monitor_main };

uint8_t keys;                   // bit per key, pressed or not
uint8_t changed = REPAINT; 	// chnaged keys or VR, bitmask
uint8_t encoder = 0;            // Encoder value
uint8_t encoder_old = 0;            // Encoder value



// ---------------------------------------------------------------
//
// Menu elements - special
//
// ---------------------------------------------------------------

static uint8_t ui_marker_unsaved;

void ui_report_unsaved( uint8_t yes )
{
    ui_marker_unsaved = yes;
    changed |= REPAINT;
}


void menu_display_help(void)
{
    lcd_gotoxy( 14, 0 );    lcd_puts("|  +1 ");
    lcd_gotoxy( 14, 1 );    lcd_puts("|  -1 ");
    lcd_gotoxy( 14, 2 );    lcd_puts("| Save");
    lcd_gotoxy( 14, 3 );    lcd_puts("| Exit");
}

void menu_event_saved(void)
{
    if( changed & KEY_OK )
        select_menu( &menu_main );
}

void menu_display_saved(void)
{
    lcd_gotoxy( 0, 0 );
    lcd_puts("Setting accepted");
//    lcd_gotoxy( 0, 1 );
//    lcd_puts(" Press any key  ");
}

// ---------------------------------------------------------------

uint8_t menu_shift, menu_curr, menu_curr_old;

void menu_event_main(void)
{
    if( changed & KEY_OK )
        select_menu( menu + menu_curr );
}


void menu_display_main(void)
{
    //    uint8_t i;

    menu_curr = encoder % MENU_LEN; // max menu items
    //if( menu_curr < 0 ) menu_curr = 0;
    if( menu_curr >= MENU_LEN ) menu_curr = MENU_LEN-1;

    menu_shift = menu_curr-1;
    if( menu_curr <= 0 ) menu_shift = 0; // curr is uint, low bound does not work

    //if( menu_shift < 0 ) menu_shift = 0;
    if( menu_shift >= MENU_LEN-2 ) menu_shift = MENU_LEN-3;

    if( ((changed & ~KEY_ENC) == 0) && (menu_curr == menu_curr_old) ) return;
    menu_curr_old = menu_curr;

    lcd_gotoxy( 14, 0 );
    lcd_puts("      "); // Clear tail

    lcd_gotoxy( 0, 0 );
    put_menu_item(menu_shift);
    put_menu_item(menu_shift+1);
    put_menu_item(menu_shift+2);

    lcd_gotoxy( 12, 1 );
    lcd_puts("        "); // Clear tail
    lcd_gotoxy( 0, 1 );
    lcd_puts(">");
    lcd_puts(menu[menu_curr].long_name);
}

void put_menu_item( uint8_t ip )
{
    if( ip == menu_curr ) lcd_putc('<');
    lcd_puts(menu[ip].name);
    if( ip == menu_curr ) lcd_putc('>');
    lcd_putc(' ');
}

void menu_monitor_main( void )
{
    char buf[20];

    time_t secs = time(NULL);
    struct _tm *stm = localtime(&secs);

    snprintf( buf, sizeof(buf),  "%02d/%02d/%04d, %02d:%02d:%02d",
            stm->tm_mday, stm->tm_mon + 1, stm->tm_year + 1900,
            stm->tm_hour, stm->tm_min, stm->tm_sec
            );

    lcd_gotoxy( 0, 3 );
    lcd_puts( buf );
}

// ---------------------------------------------------------------
//
// Menu elements - general
//
// ---------------------------------------------------------------



static void menu_put_temp(uint8_t id);

void menu_monitor_temperature(void)
{
    lcd_gotoxy( 0, 0 ); menu_put_temp( encoder   );
    lcd_gotoxy( 0, 1 ); menu_put_temp( encoder+1 );
    lcd_gotoxy( 0, 2 ); menu_put_temp( encoder+2 );
    lcd_gotoxy( 0, 3 ); menu_put_temp( encoder+3 );
/*
    lcd_gotoxy( 10, 0 ); menu_put_temp( encoder+4 );
    lcd_gotoxy( 10, 1 ); menu_put_temp( encoder+5 );
    lcd_gotoxy( 10, 2 ); menu_put_temp( encoder+6 );
    lcd_gotoxy( 10, 3 ); menu_put_temp( encoder+7 );
*/
}

static void menu_put_temp(uint8_t id)
{
    id %= SERVANT_NTEMP;

    lcd_puts("T");
    if( id < 10 ) lcd_putc(' ');
    lcd_puti(id);
    lcd_puts(": ");
    lcd_put_temp(currTemperature[id]);
    //lcd_puts(" B.");
    //lcd_puti(gTempSensorBus[id]);
    lcd_puts("  ");
}



// ---------------------------------------------------------------


void menu_display_net_status(void)
{
    lcd_gotoxy( 0, 0 );
    lcd_puts("IP A:");
    lcd_put_ip_addr( confnet.cdn_ip_addr );

    lcd_gotoxy( 0, 1 );
    lcd_puts("IP M:");
    lcd_put_ip_addr( confnet.cdn_ip_mask );

    lcd_gotoxy( 0, 2 );
    lcd_puts("MAC address: ");

    lcd_gotoxy( 0, 3 );

    char buf[20];

    static prog_char tfmt[] = "%02X:%02X:%02X:%02X:%02X:%02X";
    sprintf_P( buf, tfmt,
                  ee_cfg.mac_addr[0], ee_cfg.mac_addr[1], ee_cfg.mac_addr[2],
                  ee_cfg.mac_addr[3], ee_cfg.mac_addr[4], ee_cfg.mac_addr[5]
                 );
    lcd_puts( buf );
}






// ---------------------------------------------------------------


void menu_display_io(void)
{
    lcd_gotoxy( 0, 0 );
    lcd_puts("Channels: ");
    lcd_gotoxy( 0, 1 );
    lcd_put_bits( dio_state );

    if( changed & KEY_OK )        select_menu( &menu_main );
}


// ---------------------------------------------------------------


void menu_display_scan(void)
{
    lcd_gotoxy( 0, 0 );
    lcd_puts("Ok: Rescan 1Wire");
    lcd_gotoxy( 0, 1 );
    init_temperature(); // Prints
}




// ---------------------------------------------------------------



void menu_display_bus(void)
{
    uint8_t bus = 0; //encoder % N_1W_BUS;    if(bus >= N_1W_BUS) bus = N_1W_BUS-1;

    uint8_t i, c = 0;
    for( i = 0; i < nTempSensors; i++ )
        if( gTempSensorBus[i] == bus )
            c++;

    lcd_gotoxy( 0, 0 );

    lcd_puts("1W sensors: ");
    lcd_puti(c);
    lcd_puts(" ");
}

void menu_monitor_bus(void)
{
    uint8_t bus = encoder % N_1W_BUS;
    if(bus >= N_1W_BUS) bus = N_1W_BUS-1;

    lcd_gotoxy( 0, 1 );

    lcd_puts("1W errors:  ");
    lcd_puti( ow_error_cnt ); //ow_bus_error_cnt[bus]);
    lcd_puts("  ");

    if( changed & KEY_OK )        select_menu( &menu_main );
}

// ---------------------------------------------------------------
//
// Error log
//
// ---------------------------------------------------------------



void menu_display_errlog(void)
{
    //uint8_t pos = encoder;
    lcd_gotoxy( 0, 0 );

    //lcd_put_event( pos );
    lcd_gotoxy( 0, 1 );
    //lcd_put_event( pos+1 );

}



// ---------------------------------------------------------------
//
// IO Errors status
//
// ---------------------------------------------------------------


void menu_display_status(void)
{
    static uint8_t cnt_main_monitor;

    //lcd_clear_line( 1 );

    //lcd_clear_line( 2 );
    lcd_gotoxy( 17, 2 ); lcd_puts("   ");
    lcd_gotoxy( 0, 2 );
    lcd_puts("MQTT/UDP:");
    //lcd_puti( mqtt_io_count % 100 ); // Take 2 digits
#warning connect MQTT/UDP
/*
    lcd_puts(" ModBus:");
    lcd_puti( modbus_event_cnt % 100 ); // Take 2 digits
    lcd_putc( (cnt_main_monitor++ & 1) ? ':' : '.' );
    lcd_puti( (modbus_crc_cnt+modbus_exceptions_cnt) % 100 ); // Take 2 digits
    //lcd_puts("  ");
*/
    //lcd_clear_line( 3 );
    lcd_gotoxy( 16, 3 ); lcd_puts("    ");
    lcd_gotoxy( 0, 3 );
    lcd_puts("EEprom:");
    lcd_putc( ui_marker_unsaved ? '*' : '.' );
    lcd_puts(" 1WErr:");
    lcd_puti( ow_error_cnt % 100 ); // Take 2 digits
    //lcd_puts("  ");

    lcd_gotoxy( 0, 0 );
    lcd_puts("IO Status  ");
}


// ---------------------------------------------------------------
//
// Run menu system
//
// ---------------------------------------------------------------



struct menu_t menu[] = {
    { "Help", 	"Keys functions",	menu_display_help, 		0, 0 },

    { "Net", 	"Network status",	menu_display_net_status, 	0, menu_display_net_status },
    { "Log", 	"System log    ",      	menu_display_errlog,            0, menu_display_errlog },

    { "Switch",	"Switch lights",	menu_display_io, 		0, menu_display_io },

    { "Temp", 	"Show temperatures",	menu_monitor_temperature, 	0, menu_monitor_temperature },
    { "Scan", 	"Rescan sensors",	menu_display_scan, 		0, 0 },
    

    { "1wBus",  "1Wire bus status",	menu_display_bus,               0, menu_monitor_bus },

    { "Status",	"IO Err Status",	menu_display_status, 		0, menu_display_status },
};

#define _MENU_SIZE ( (sizeof(menu)) / (sizeof(struct menu_t))  )
//static uint8_t menu_nItems = _MENU_SIZE;
static uint8_t getMenuLen(void) { return _MENU_SIZE; }

static uint8_t display_monitor = 1;

// Called from main in loop
void menu_run(void)
{
    // read keys & vr!
    menu_read_input();
    //menu_read_encoder();

    if( changed )
    {
        //printf("menu event %x", changed);
        if( curr_menu->event != 0 )
            curr_menu->event();

        // We had 'No' pressed and curr_menu didn't consume?
        if( changed & KEY_NO )
            select_menu( &menu_main );

        curr_menu->display();
        changed = 0;
    }

    if(display_monitor && curr_menu->monitor)
        curr_menu->monitor();

    display_monitor = 0;
}

void menu_timer_05sec(void) { display_monitor++; }


void select_menu( struct menu_t *m )
{
    curr_menu = m;
    changed &= ~KEY_NO;
    changed &= ~KEY_OK;
    changed |= REPAINT;
    lcd_clear();
    display_monitor++;
}





// ---------------------------------------------------------------
//
// Run menu system
//
// ---------------------------------------------------------------

void menu_init(void)
{
    uint8_t key_mask = _BV(BUTTON_F1_BIT) | _BV(BUTTON_F2_BIT) | _BV(BUTTON_F3_BIT) | _BV(BUTTON_F4_BIT);
    uint8_t enc_mask = _BV(ENCODER_PRESS_BIT) | _BV(ENCODER_A_BIT) | _BV(ENCODER_B_BIT);
    // Pullup
    BUTTONS_PORT |= key_mask;
    ENCODER_PORT |= enc_mask;

    // Input
    BUTTONS_DDR &= ~key_mask;
    ENCODER_DDR &= ~enc_mask;

    changed |= KEY_NO; // to repaint main menu on first time

}

static uint8_t new_keys_0 = 0;
static uint8_t new_keys_1 = 0;
static uint8_t keys_old = 0;
static void menu_read_input(void)
{

    uint8_t new_keys = 0;

    if(!BUTTON_F1) 	new_keys |= KEY_F1;
    if(!BUTTON_F2) 	new_keys |= KEY_F2;
    if(!BUTTON_F3) 	new_keys |= KEY_F3;
    if(!BUTTON_F4) 	new_keys |= KEY_F4;

    if(!ENCODER_PRESS)	new_keys |= KEY_OK;

    // kill jitter
    if( (new_keys == new_keys_0) && (new_keys == new_keys_1) )
    {
        keys &= REPAINT; // clear all key bits
        keys |= new_keys;
    }

    new_keys_1 = new_keys_0;
    new_keys_0 = new_keys;

    changed = keys & ( keys ^ keys_old );
    changed &= ~REPAINT; // don't touch repaint bit

    if( encoder_old != encoder )
        changed |= KEY_ENC;

    encoder_old = encoder;

    keys_old = keys;

    if( (curr_menu == &menu_main) && (changed & KEY_MASK_F1_F4) )
    {
        //printf( "menu_read_input 0x%x\n", changed );
        // In main menu F1-F4 are not used in menu system, but translated to dio subsystem as light switches

        dio_front_buttons_changed |= changed & KEY_MASK_F1_F4;
        changed &= ~KEY_MASK_F1_F4; // kill 'em, we transferred 'em
    }


}





