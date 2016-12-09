#include <inttypes.h>


/*
 *	UI Controls
 *
 *      4 buttons + Encoder press
 *
**/


// NB! Must be low 4 bits, used as bit field in transferring to dio

#define KEY_NO  0x01     // == F1

#define KEY_F1 0x01
#define KEY_F2 0x02
#define KEY_F3 0x04
#define KEY_F4 0x08

#define KEY_MASK_F1_F4 0x0F

#define KEY_OK  0x10    // Encoder press
#define KEY_ENC 0x20    // Encoder value changed


#define REPAINT 0x80



struct menu_t
{
	char *name;
//	char *long_name;
	void (*display)( void );
        void (*event)( void );
        void (*monitor)( void );        // Called every cycle
};


void menu_init(void);
void menu_run(void);
void menu_timer_05sec(void);


// Show marker that we have unsaved config
void ui_report_unsaved( uint8_t yes );


void encoder_init( void );




extern uint8_t encoder;            // Encoder value



#define BUTTONS_PORT PORTD
#define BUTTONS_PIN PIND
#define BUTTONS_DDR DDRD

#define BUTTON_F1_BIT PD0
#define BUTTON_F2_BIT PD1
#define BUTTON_F3_BIT PD2
#define BUTTON_F4_BIT PD3

#define BUTTON_F1 (BUTTONS_PIN & (1<<BUTTON_F1_BIT))
#define BUTTON_F2 (BUTTONS_PIN & (1<<BUTTON_F2_BIT))
#define BUTTON_F3 (BUTTONS_PIN & (1<<BUTTON_F3_BIT))
#define BUTTON_F4 (BUTTONS_PIN & (1<<BUTTON_F4_BIT))


#define ENCODER_PORT PORTE
#define ENCODER_PIN PINE
#define ENCODER_DDR DDRE

#define ENCODER_PRESS_BIT PE4

#define ENCODER_A_BIT PE6
#define ENCODER_B_BIT PE7


#define ENCODER_A (ENCODER_PIN & (1<<ENCODER_A_BIT))
#define ENCODER_B (ENCODER_PIN & (1<<ENCODER_B_BIT))
#define ENCODER_PRESS (ENCODER_PIN & (1<<ENCODER_PRESS_BIT))





