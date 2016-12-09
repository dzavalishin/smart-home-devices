#include <inttypes.h>
#include <avr/io.h>

#include "ui_menu.h"
#include "ui_lcd.h"

#include "util.h"
#include "modbus.h"
//#include "eeprom.h"
#include "defs.h"
//#include "errlog.h"

#include "io_temp.h"
#include "io_dig.h"


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

struct menu_t menu_main = { "Main", menu_display_main, menu_event_main, menu_monitor_main };
struct menu_t *curr_menu = &menu_main;

struct menu_t menu_saved = { "Saved", menu_display_saved, menu_event_saved, menu_monitor_main };

uint8_t keys;                   // bit per key, pressed or not
uint8_t changed = REPAINT; 	// chnaged keys or VR, bitmask
//uint8_t vr1,  vr2; 		// value 0-31, vr curr position
//uint8_t vr1_prev,  vr2_prev; 	// value 0-31, vr prev position
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
    lcd_gotoxy( 0, 0 );
    lcd_puts("Change VR Select");
    lcd_gotoxy( 0, 1 );
    lcd_puts("Exit Button   Ok");
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

    //d_gotoxy( 0, 0 );
    lcd_gotoxy( 0, 0 );
    put_menu_item(menu_shift);
    put_menu_item(menu_shift+1);
    put_menu_item(menu_shift+2);

    lcd_puts("        "); // Clear tail
}

void put_menu_item( uint8_t ip )
{
//    lcd_putc(' ');
    if( ip == menu_curr ) lcd_putc('<');
    lcd_puts(menu[ip].name);
    if( ip == menu_curr ) lcd_putc('>');
    lcd_putc(' ');
}

void menu_monitor_main( void )
{
    static uint8_t cnt_main_monitor;

    lcd_gotoxy( 0, 1 );
    lcd_puts("EE:");
    lcd_putc( ui_marker_unsaved ? '*' : '.' );
    lcd_puts(" IO:");
    lcd_puti( modbus_event_cnt % 100 ); // Take 2 digits
    lcd_putc( (cnt_main_monitor++ & 1) ? ':' : '.' );
    lcd_puti( (modbus_crc_cnt+modbus_exceptions_cnt) % 100 ); // Take 2 digits
    lcd_putc( ' ' );
    lcd_puti( ow_error_cnt % 100 ); // Take 2 digits
    lcd_puts("               ");
}

// ---------------------------------------------------------------
//
// Menu elements - general
//
// ---------------------------------------------------------------


void menu_event_rs485addr(void)
{
    if( changed & KEY_OK )
    {
#warning make save_eeprom_settings
        //save_eeprom_settings();

        select_menu( &menu_saved );
    }
}

void menu_display_rs485addr(void)
{
    modbus_our_address = encoder;
    if( modbus_our_address > MODBUS_MAX_ADDRESS )
        modbus_our_address = MODBUS_MAX_ADDRESS;

    if(changed & REPAINT)
    {
        lcd_gotoxy( 0, 0 );
        lcd_puts("RS485 address:  ");
    }

    lcd_gotoxy( 0, 1 );
    lcd_puts("< MSB ");
    lcd_puti(modbus_our_address);
    lcd_puts(" LSB >   ");
}

// ---------------------------------------------------------------

static uint16_t rs485_speed = 0xFF;

void menu_event_rs485speed(void)
{
    uint8_t sel = encoder % 7;
    if( sel < 6 )        rs485_speed = (1 << sel) * 1200l;
//    else        	 rs485_speed = 115200;

    if( changed & KEY_OK )
    {
        modbus_set_baud( rs485_speed );
#warning save_eeprom_settings
        //save_eeprom_settings();

        //curr_menu = &menu_saved;
        select_menu( &menu_saved );
    }
}

void menu_display_rs485speed(void)
{
    lcd_gotoxy( 0, 0 );
    lcd_puts("RS485 speed:    ");

    lcd_gotoxy( 0, 1 );
    lcd_puts("<- Change ");
/*
    if(rs485_speed == 0xFF)
        rs485_speed = uart_speed;

    // puti is 16 bit, need some help
    switch(rs485_speed)
    {
    case 38400:        	lcd_puts("38400"); break;
    case 57600:        	lcd_puts("57600"); break;
//    case 115200:        lcd_puts("115200"); break;

    default:
        lcd_puti(rs485_speed); break;
    }
*/
    lcd_puts("     ");
}

// ---------------------------------------------------------------

static void menu_put_temp(uint8_t id);

void menu_monitor_temperature(void)
{
    lcd_gotoxy( 0, 0 ); menu_put_temp( encoder);
    lcd_gotoxy( 0, 1 ); menu_put_temp( encoder+1);
}

static void menu_put_temp(uint8_t id)
{
    if(id >= SERVANT_NTEMP) id = SERVANT_NTEMP-1;

    lcd_puts(" T");
    if( id < 10 ) lcd_putc(' ');
    lcd_puti(id);
    lcd_puts(": ");
    lcd_put_temp(currTemperature[id]);
    lcd_puts(" B.");
    lcd_puti(gTempSensorBus[id]);
    lcd_puts("    ");
}


// ---------------------------------------------------------------

unsigned char * sensor_rom_code;
int8_t sensor_persistent_id = -1;

void menu_event_map(void)
{
    if( changed & KEY_OK )
    {
#warning mapping
        //remove_mapping_by_id(sensor_persistent_id);
        //remove_mapping_by_rom(sensor_rom_code);

        //add_mapping( sensor_rom_code, sensor_persistent_id );

        //temp_sens_save_logical_numbers();

        //curr_menu = &menu_saved;
        select_menu( &menu_saved );
    }
}

void menu_display_map(void)
{
/*
    uint8_t index = vr1;

    if(index >= SERVANT_NTEMP) index = SERVANT_NTEMP-1;

    sensor_rom_code = gTempSensorIDs[index];
    sensor_persistent_id = vr2;

//    int pos = find_pos_by_persistent_id(sensor_persistent_id);
//    uint16_t temp = currTemperature[pos];
//    if( pos < 0 ) temp = -99;

    uint16_t temp = currTemperature[index];

    lcd_gotoxy( 0, 0 );
    lcd_puts("> ROM:");
    lcd_puthex((char *)sensor_rom_code,OW_ROMCODE_SIZE);
    lcd_puts(" ");

    lcd_gotoxy( 0, 1 );
    lcd_puts("< ID: ");
    lcd_puti(sensor_persistent_id);
    lcd_puts(" t=");
    lcd_put_temp(temp);
    lcd_puts(" ");
*/
}

// ---------------------------------------------------------------

void put_menu_ai(uint8_t aino );

void menu_display_ai(void)
{
#if SERVANT_NADC > 0
    lcd_gotoxy( 0, 0 );
    put_menu_ai(0);
    put_menu_ai(1);

    lcd_gotoxy( 0, 1 );
    put_menu_ai(2);
    put_menu_ai(3);
#else
    //lcd_clear();
    lcd_puts("No analog inputs");
#endif
}


#if SERVANT_NADC > 0
void put_menu_ai(uint8_t aino )
{
    //lcd_puts("AI");
    lcd_puti( aino );
    lcd_puts(": ");
    lcd_puti( adc_value[aino] );
    lcd_puts(" ");
}
#endif



// ---------------------------------------------------------------

void put_menu_pressure(uint8_t aino );

void menu_display_pressure(void)
{
    lcd_gotoxy( 0, 0 );
    put_menu_pressure(0);
    put_menu_pressure(1);

    lcd_gotoxy( 0, 1 );
    put_menu_pressure(2);
    put_menu_pressure(3);
}


void put_menu_pressure(uint8_t aino )
{
    lcd_puts("P");
    lcd_puti( aino );
    lcd_puts(": ");
    //lcd_puti( sens[aino].out_value );
    lcd_puts(" ");
}

// ---------------------------------------------------------------


void menu_display_status(void)
{

    lcd_gotoxy( 0, 0 );
    /*
    lcd_puts( active_pump ? "Alt" : "Main");
    lcd_puts(" pump ");

    lcd_puts( pump_state ? "on " : "off");

    lcd_gotoxy( 0, 1 );
    lcd_puts( system_failed ? "Fail " : "Ok   ");
    */
}






// ---------------------------------------------------------------


void menu_display_di(void)
{
    lcd_gotoxy( 0, 0 );
    lcd_puts("  DIn:  ");
    lcd_putx( PINB );

    lcd_gotoxy( 0, 1 );
    lcd_puts("  Dout: ");
    lcd_putx( PORTB );

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

static void *shift_buf( void *bp, uint8_t blen )
{
#if 0
    uint8_t shift = encoder;

    blen -= 8; // Show 8 bytes

    if( shift >= blen ) shift = blen-1;

    return bp+shift;
#else
    return bp;
#endif
}

void menu_display_485_i(void)
{
    lcd_gotoxy( 0, 0 );
    lcd_puts("< RS485 RX data:" );
}

void menu_monitor_485_i(void)
{
    lcd_gotoxy( 0, 1 );
    //lcd_puthex((char *)modbus_rx_buf,MODBUS_MAX_RX_PKT);
    lcd_puthex((char *) shift_buf( modbus_rx_buf, MODBUS_MAX_RX_PKT),8);
}

void menu_display_485_o(void)
{
    lcd_gotoxy( 0, 0 );
    lcd_puts("< RS485 TX data:" );
}

void menu_monitor_485_o(void)
{
    lcd_gotoxy( 0, 1 );
    lcd_puthex((char *) shift_buf( modbus_tx_buf, MODBUS_MAX_TX_PKT),8);
}

// ---------------------------------------------------------------



void menu_display_bus(void)
{
    uint8_t bus = encoder % N_1W_BUS;
    if(bus >= N_1W_BUS) bus = N_1W_BUS-1;

    uint8_t i, c = 0;
    for( i = 0; i < SERVANT_NTEMP; i++ )
        if( gTempSensorBus[i] == bus )
            c++;

    lcd_gotoxy( 0, 0 );

    lcd_puts("<1w Bus ");
    lcd_puti(bus);

    lcd_gotoxy( 0, 1 );
    lcd_puts("Sens:");
    lcd_puti(c);
    lcd_puts(" ");
}

void menu_monitor_bus(void)
{
    uint8_t bus = encoder % N_1W_BUS;
    if(bus >= N_1W_BUS) bus = N_1W_BUS-1;

    lcd_gotoxy( 9, 1 );

    lcd_puts("Err:");
    lcd_puti( ow_error_cnt ); //ow_bus_error_cnt[bus]);
    lcd_puts("  ");
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
// Sensor channels parameters config: conversion, trigger levels
//
// ---------------------------------------------------------------





void menu_display_conv(void)
{
    //uint8_t itemNo = encoder;

    lcd_puts("   ");
}

void menu_event_conv(void)
{
    //uint8_t sel = vr2/4;

    if( changed & KEY_OK )
    {
        // Save changes
        //modbus_set_baud( rs485_speed );
        //save_eeprom_settings();

        select_menu( &menu_saved );
    }
}


void menu_display_trig(void)
{
    //uint8_t itemNo = encoder;
}

void menu_event_trig(void)
{
    //uint8_t sel = vr2/4;

    if( changed & KEY_OK )
    {
        // Save changes

        select_menu( &menu_saved );
    }
}



// ---------------------------------------------------------------
//
// Run menu system
//
// ---------------------------------------------------------------



struct menu_t menu[] = {
    { "Help", 	menu_display_help, 		0, 0 },

    { "Status", menu_display_status, 		0, menu_display_status },
    { "Status", menu_display_errlog,            0, menu_display_errlog },

    { "Conv", 	menu_display_conv, 		menu_event_conv, menu_display_conv },
    { "Trig", 	menu_display_trig, 		menu_event_trig, menu_display_trig },

    { "Sens", 	menu_display_pressure, 		0, menu_display_pressure },
    { "AIn", 	menu_display_ai, 		0, menu_display_ai },
    { "DIn", 	menu_display_di, 		0, menu_display_di },

    { "Addr", 	menu_display_rs485addr, 	menu_event_rs485addr, 0 },
    { "Spd", 	menu_display_rs485speed, 	menu_event_rs485speed, 0 },
    { "Temp", 	menu_monitor_temperature, 	0, menu_monitor_temperature },
    { "Map", 	menu_display_map, 		menu_event_map, 0 },
    { "Scan", 	menu_display_scan, 		0, 0 },
    



    { "485I", 	menu_display_485_i, 		0, menu_monitor_485_i },
    { "485O", 	menu_display_485_o, 		0, menu_monitor_485_o },
    { "1wBus",  menu_display_bus,               0, menu_monitor_bus },
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

    if( changed )
    {
        if( curr_menu->event != 0 )
            curr_menu->event();

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
    // Pullup
    BUTTONS_PORT |= BUTTON_F1_BIT | BUTTON_F2_BIT | BUTTON_F3_BIT | BUTTON_F4_BIT;
    ENCODER_PORT |= ENCODER_PRESS_BIT | ENCODER_A_BIT | ENCODER_B_BIT;

    // Input
    BUTTONS_DDR &= ~(BUTTON_F1_BIT | BUTTON_F2_BIT | BUTTON_F3_BIT | BUTTON_F4_BIT);
    ENCODER_DDR &= ~(ENCODER_PRESS_BIT | ENCODER_A_BIT | ENCODER_B_BIT);

#warning TODO enc interrupts
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

    //if( (keys & KEY_OK) && !(keys_old & KEY_OK) ) changed |= KEY_OK;
    //if( (keys & KEY_NO) && !(keys_old & KEY_NO) ) changed |= KEY_NO;

    changed = keys & ( keys ^ keys_old );
    changed &= ~REPAINT; // don't touch repaint bit

    if( encoder_old != encoder )
        changed |= KEY_ENC;

    keys_old = keys;

    if( (curr_menu == &menu_main) && (changed & KEY_MASK_F1_F4) )
    {
        printf( "menu_read_input 0x%x\n", changed );
        // In main menu F1-F4 are not used in menu system, but translated to dio subsystem as light switches

        dio_front_buttons_changed |= changed & KEY_MASK_F1_F4;
        changed &= ~KEY_MASK_F1_F4; // kill 'em, we transferred 'em
    }


}





