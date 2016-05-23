#include <inttypes.h>


/*
 *	UI menu
 *
 * LEFT & RIGHT Variable Resistors
 * Exit & Menu/Ok buttons
 *
 * Help & overview
 * RS485 address -> show IO for current address
 * RS485 speed -> show IO on bus (all addr)
 * sensors map ->
 * LEFT VR = ROM id - RIGHT VR = persistent id, OK = save
 * sensors readout
 *  * LEFT VR = choose sensor (internal ID order), TODO right vr = correction +-, OK = save
 * or left vr - sensor 1, right vr - sensor 2, display 2 of them
 *  counters values
 * DI values
 *
**/


#define KEY_OK 0x01
#define KEY_NO 0x02
#define KEY_VR1 0x10
#define KEY_VR2 0x20

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

