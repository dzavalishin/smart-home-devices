#include <inttypes.h>
#include <avr/io.h>

#include "ui_menu.h"
#include "util.h"
#include "adc.h"
#include "modbus.h"
#include "temperature.h"
#include "eeprom.h"
#include "uart.h"
#include "defs.h"


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
uint8_t vr1,  vr2; 		// value 0-31, vr curr position
uint8_t vr1_prev,  vr2_prev; 	// value 0-31, vr prev position



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

    menu_curr = (vr1 * MENU_LEN) / 32; // max menu items
    //if( menu_curr < 0 ) menu_curr = 0;
    if( menu_curr >= MENU_LEN ) menu_curr = MENU_LEN-1;

    menu_shift = menu_curr-1;
    if( menu_curr <= 0 ) menu_shift = 0; // curr is uint, low bound does not work

    //if( menu_shift < 0 ) menu_shift = 0;
    if( menu_shift >= MENU_LEN-2 ) menu_shift = MENU_LEN-3;

    if( ((changed & ~KEY_VR1) == 0) && (menu_curr == menu_curr_old) ) return;
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
        save_eeprom_settings();

//        curr_menu = &menu_saved;
        select_menu( &menu_saved );
    }
}

void menu_display_rs485addr(void)
{
    modbus_our_address = (vr1 / 2) + vr2 * 16;
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
    uint8_t sel = vr2/4;
    if( sel < 6 )        rs485_speed = (1 << sel) * 1200l;
//    else        	 rs485_speed = 115200;

    if( changed & KEY_OK )
    {
        modbus_set_baud( rs485_speed );
        save_eeprom_settings();

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

    lcd_puts("     ");
}

// ---------------------------------------------------------------

static void menu_put_temp(uint8_t id, char c);

void menu_monitor_temperature(void)
{
    lcd_gotoxy( 0, 0 ); menu_put_temp(vr1,'<');
    lcd_gotoxy( 0, 1 ); menu_put_temp(vr2,'>');
}

static void menu_put_temp(uint8_t id, char c)
{
    if(id >= N_TEMPERATURE_IN) id = N_TEMPERATURE_IN-1;

    lcd_putc( c );
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
        remove_mapping_by_id(sensor_persistent_id);
        remove_mapping_by_rom(sensor_rom_code);

        add_mapping( sensor_rom_code, sensor_persistent_id );

        temp_sens_save_logical_numbers();

        //curr_menu = &menu_saved;
        select_menu( &menu_saved );
    }
}

void menu_display_map(void)
{
    uint8_t index = vr1;

    if(index >= N_TEMPERATURE_IN) index = N_TEMPERATURE_IN-1;

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

#if SERVANT_NCNT || SERVANT_NDI

void put_menu_cnt(uint8_t cno );

void menu_display_di(void)
{
    lcd_gotoxy( 0, 0 );
#if SERVANT_NCNT > 0
    put_menu_cnt(0);
    put_menu_cnt(1);
#else
    lcd_puts("  No counters   ");
#endif

    lcd_gotoxy( 0, 1 );
#if SERVANT_NDI > 0
    /*
     lcd_puts("  DI:  ");
     uint8_t i;
     for( i = 0; i < 8; i++ )
     {
     lcd_putc( ((di >>i) & 1) ? '1' : '0' );
     lcd_putc(' ');
     */
#else
    lcd_puts(" No digital ins ");
#endif
}


#if SERVANT_NCNT > 0
void put_menu_cnt(uint8_t cno )
{
    lcd_puts("   Cnt");
    lcd_puti(cno);
    lcd_puts(":  ");
    lcd_puti(counter[cno]);
}
#endif

#endif

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
    uint8_t shift = vr1;

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
    //lcd_puthex((char *)modbus_rx_buf,MODBUS_MAX_RX_PKT);
    lcd_puthex((char *) shift_buf( modbus_tx_buf, MODBUS_MAX_TX_PKT),8);
}

// ---------------------------------------------------------------



void menu_display_bus(void)
{
    uint8_t bus = vr2/4;
    if(bus >= N_1W_BUS) bus = N_1W_BUS-1;

    uint8_t i, c = 0;
    for( i = 0; i < N_TEMPERATURE_IN; i++ )
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
    uint8_t bus = vr2/4;
    if(bus >= N_1W_BUS) bus = N_1W_BUS-1;

    lcd_gotoxy( 9, 1 );

    lcd_puts("Err:");
    lcd_puti( ow_error_cnt ); //ow_bus_error_cnt[bus]);
    lcd_puts("  ");
}


// ---------------------------------------------------------------
//
// Run menu system
//
// ---------------------------------------------------------------



struct menu_t menu[] = {
    { "Help", 	menu_display_help, 		0, 0 },
    { "Addr", 	menu_display_rs485addr, 	menu_event_rs485addr, 0 },
    { "Spd", 	menu_display_rs485speed, 	menu_event_rs485speed, 0 },
    { "Temp", 	menu_monitor_temperature, 	0, menu_monitor_temperature },
    { "Map", 	menu_display_map, 		menu_event_map, 0 },
    { "Scan", 	menu_display_scan, 		0, 0 },
    { "AIn", 	menu_display_ai, 		0, menu_display_ai },
#if SERVANT_NCNT || SERVANT_NDI
    { "DIn", 	menu_display_di, 		0, 0 },
#endif
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



#define BUTTONS_PORT PORTA
#define BUTTONS_PIN PINA
#define BUTTONS_DDR DDRA

#define BUTTON1_BIT PA4
#define BUTTON2_BIT PA5

#define BUTTON1 (PINA & (1<<BUTTON1_BIT))
#define BUTTON2 (PINA & (1<<BUTTON2_BIT))

// ---------------------------------------------------------------
//
// Run menu system
//
// ---------------------------------------------------------------

void menu_init(void)
{
    // Pullup
    BUTTONS_PORT |= BUTTON1_BIT;
    BUTTONS_PORT |= BUTTON2_BIT;

    // Input
    BUTTONS_DDR &= ~BUTTON1_BIT;
    BUTTONS_DDR &= ~BUTTON2_BIT;
}

static uint8_t keys_old = 0;
static void menu_read_input(void)
{
    keys &= ~KEY_OK;
    keys &= ~KEY_NO;

    if(!BUTTON1) keys |= KEY_OK;
    if(!BUTTON2) keys |= KEY_NO;

    if( (keys & KEY_OK) && !(keys_old & KEY_OK) ) changed |= KEY_OK;
    if( (keys & KEY_NO) && !(keys_old & KEY_NO) ) changed |= KEY_NO;

    keys_old = keys;

    // ADC is 10 bit, we need 32 steps (5 bit)

    vr1 = (adc_value[6] + 15) >> 5;
    vr2 = (adc_value[7] + 15) >> 5;

    if( vr1_prev != vr1 ) { vr1_prev = vr1; 	changed |= KEY_VR1; }
    if( vr2_prev != vr2 ) { vr2_prev = vr2; 	changed |= KEY_VR2; }

}





