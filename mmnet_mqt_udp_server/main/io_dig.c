/**
 *
 * DZ-MMNET-WALL: Wall control panel based on MMNet01.
 *
 * Digital ports I/O.
 *
**/

#include "defs.h"
#include "util.h"
#include "runtime_cfg.h"

#include "io_dig.h"
#include "mqtt.h"


#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <sys/thread.h>
#include <sys/timer.h>




static void dio_process( void );
static void dio_input( void );


static unsigned char get_dig_in(unsigned char port_num);

//static unsigned char get_dig_out(unsigned char port_num);
//static void set_dig_out(unsigned char port_num, unsigned char data);

static unsigned char get_ddr(unsigned char port_num);
//static void set_ddr(unsigned char port_num, unsigned char data);



THREAD(dio_proc, __arg)
{
    while(1)
    {
        NutSleep(2);
        dio_process();
    }
}



// TODO support dynamic io conf here
void dio_init(void)
{

    // Set port values BEFORE enabling DDE
    PORTB = 0; // ee_cfg.start_b;
    PORTD = 0; // ee_cfg.start_d;
    PORTE = 0; // ee_cfg.start_e;
    PORTF = 0; // ee_cfg.start_f;
    PORTG = 0; // ee_cfg.start_g;


    DDRB = 0; // ee_cfg.ddr_b;
    DDRD = 0; // ee_cfg.ddr_d;
    DDRE = 0; // ee_cfg.ddr_e;
    DDRF = 0; // ee_cfg.ddr_f;
    DDRG = 0; // ee_cfg.ddr_g;


    // Now set dde for pins we use in a dedicated way

    led_ddr_init(); // again - first time was in main

    DI_DDR  &= ~0b11001111; // set as input
    DI_PORT |=  0b11001111; // pull up

    DO_DDR  |=  0b01011111; // set as out
    //DO_PORT |=  0b00001111; // LEDs are low level activated, turn off
    DO_PORT &=  0b11110000; // LEDs are low level activated, turn on
}




uint8_t         dio_front_buttons_changed = 0;          // Front panel button in non-menu mode pressed
uint32_t        dio_remote_state_changed = 0;           // We've got remote (MQTT broker) state changeg
uint32_t        dio_remote_state = 0;                   // Remote state as we got from MQTT
uint32_t        dio_state = 0;                   	// Our state as we know






static uint8_t  dio_start = 1;

static void dio_process( void )
{
    uint8_t fb_ch = dio_front_buttons_changed;
    uint8_t re_ch = dio_remote_state_changed;

    dio_input();

    // Process state changes from MQTT or front buttons

    // Nothing changed? no work.
    if( (!fb_ch) && (!re_ch) && (!dio_start) )
        return;

    dio_start = 0;

    if( re_ch )
    {
        dio_state &= ~re_ch; // clear those which changed remotely
        dio_state |= re_ch & dio_remote_state; // set those which changed to 1 remotely
    }

    dio_state ^= fb_ch;

    DO_PORT &= ~DO_LED_MASK;
    DO_PORT |= DO_LED_MASK & (~dio_state);

    if( fb_ch )
    {
        //printf("fb_ch 0x%x", fb_ch);
        // Send to remote what we changed localy
        uint8_t mask, ch = 0;
        for( mask = 1; mask < 0x10; mask <<= 1, ch++ )
        {
            if( fb_ch & mask )
                mqtt_send_channel( (dio_state & mask) ? 1 : 0, ch );
        }
    }

    dio_front_buttons_changed &= ~fb_ch;
    dio_remote_state_changed &= ~re_ch;
}


static void dio_in_channel( uint8_t ch, uint8_t *prev_state, uint8_t new_state, uint8_t is_button )
{
    new_state = new_state ? 1 : 0; // not -1

    // If switch, we react on any change, for button we react to 1->0 only (press, not release)
    if( is_button && new_state )
        return;

    if( new_state == *prev_state )
        return;

    *prev_state = new_state;

    dio_front_buttons_changed |= _BV(ch);
}


uint8_t di0_prev = -1; // undefined
uint8_t di1_prev = -1;

static void dio_input( void )
{
    uint8_t di0 = DI_PIN & _BV(DI_1_BIT);
    uint8_t di1 = DI_PIN & _BV(DI_2_BIT);

    dio_in_channel( ee_cfg.di_channel[0], &di0_prev, di0, ee_cfg.di_mode[0] & DI_IS_BUTTON );
    dio_in_channel( ee_cfg.di_channel[1], &di1_prev, di1, ee_cfg.di_mode[1] & DI_IS_BUTTON );
}


















// Direct
unsigned char   dio_read_port( unsigned char port ) { return get_dig_in(port); }



unsigned char   dio_get_port_ouput_mask( unsigned char port ) { return get_ddr(port); }





unsigned char
dio_read_port_bit( unsigned char port, unsigned char nBit )
{
    return 0x1 & (dio_read_port(port) >> nBit );
}


unsigned char
dio_get_port_ouput_mask_bit( unsigned char port, unsigned char nBit )
{
    return 0x1 & (get_ddr(port) >> nBit);
}




static unsigned char get_dig_in(unsigned char port_num)
{
    if( port_num > 6 ) return 0;
    switch (port_num) {
    case 0:		return PINA;
    case 1:		return PINB;
    case 2:		return PINC;
    case 3:		return PIND;
    case 4:		return PINE;
    case 5:		return PINF;
    case 6:		return PING;
    }
    return 0;
}


/*
static void set_dig_out(unsigned char port_num, unsigned char data)
{
    if( port_num >= SERVANT_NDIG ) return;
    switch (port_num) {
    case 0:		PORTA=data;		break;
    case 1:		PORTB=data;		break;
    case 2:		PORTC=data;		break;
    case 3:		PORTD=data;		break;
    case 4:		PORTE=data;		break;
    case 5:		PORTF=data;		break;
    case 6:		PORTG=data;		break;
    }
}
* /

static unsigned char get_dig_out(unsigned char port_num)
{
    if( port_num >= SERVANT_NDIG ) return 0;
    switch (port_num) {
    case 0:		return PORTA;
    case 1:		return PORTB;
    case 2:		return PORTC;
    case 3:		return PORTD;
    case 4:		return PORTE;
    case 5:		return PORTF;
    case 6:		return PORTG;
    }
    return 0;
}
*/
/*
static void set_ddr(unsigned char port_num, unsigned char data)
{
    if( port_num >= SERVANT_NDIG ) return;
    switch (port_num) {
    case 0:		DDRA=data;		break;
    case 1:		DDRB=data;		break;
    case 2:		DDRC=data;		break;
    case 3:		DDRD=data;		break;
    case 4:		DDRE=data;		break;
    case 5:		DDRF=data;		break;
    case 6:		DDRG=data;		break;
    }
}
*/
static unsigned char get_ddr(unsigned char port_num)
{
    if( port_num > 6 ) return 0;
    switch (port_num) {
    case 0:		return DDRA;
    case 1:		return DDRB;
    case 2:		return DDRC;
    case 3:		return DDRD;
    case 4:		return DDRE;
    case 5:		return DDRF;
    case 6:		return DDRG;
    }
    return 0;
}




static void dio_init_dev( dev_major* d )
{
    (void) d;
    printf("dio_init_dev\n");



}


static uint8_t dio_start_dev( dev_major* d )
{
    (void) d;
    printf("dio_start_dev\n");

    NutThreadCreate("IO", dio_proc, 0, 1024 );

    return 0;
}

//static void dio_stop_dev( dev_major* d ) { (void) d;  } // TODO


dev_major io_dig =
{
    .name = "dio",

    .init = dio_init_dev,
    .start = dio_start_dev,
    .stop = 0, // TODO
    .timer = 0, //dio_timer,

    .to_string = 0,
    .from_string = 0,

    .minor_count = 0,
    .subdev = 0,
};




















