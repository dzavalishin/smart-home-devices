#include <inttypes.h>


/*
 *	UI Controls
 *
 *      4 buttons + Encoder press
 *
**/


#define KEY_OK  0x01    // Encoder press
#define KEY_ENC 0x02    // Encoder value changed

#define KEY_NO  0x04     // == F1

#define KEY_F1 0x04
#define KEY_F2 0x08
#define KEY_F3 0x10
#define KEY_F4 0x20


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

