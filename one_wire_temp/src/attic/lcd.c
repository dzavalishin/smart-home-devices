# if 0
/*! \file lcd.c \brief Character LCD driver for HD44780/SED1278 displays. */
//*****************************************************************************
//
// File Name	: 'lcd.c'
// Title		: Character LCD driver for HD44780/SED1278 displays
//					(usable in mem-mapped, or I/O mode)
// Author		: Pascal Stang
// Created		: 11/22/2000
// Revised		: 4/30/2002
// Version		: 1.1
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <compat/deprecated.h>
//#include "global.h"

#include "delay.h"

#include "lcd.h"

#include "lcdbits.h"



/*************************************************************/
/********************** LOCAL FUNCTIONS **********************/
/*************************************************************/

void lcdInitHW(void)
{
	// initialize I/O ports
	// if I/O interface is in use
#ifdef LCD_PORT_INTERFACE
	// initialize LCD control lines
	//cbi(LCD_CTRL_PORT, LCD_CTRL_RS);
	lcd_set_rs_control();
	lcd_set_rw_write();
	DEASSERT_E;

	//cbi(LCD_CTRL_PORT, LCD_CTRL_RW);
	//cbi(LCD_CTRL_PORT, LCD_CTRL_E);

	// initialize LCD control lines to output
#if !LCD_NO_INPUT
	sbi(LCD_CTRL_DDR, LCD_CTRL_RW);
#endif
	sbi(LCD_CTRL_DDR, LCD_CTRL_E);
	sbi(LCD_CTRL_DDR, LCD_CTRL_RS);
	// initialize LCD data port to input
	// initialize LCD data lines to pull-up

	#ifdef LCD_DATA_4BIT
		outb(LCD_DATA_DDR, inb(LCD_DATA_DDR)&0x0F);		// set data I/O lines to input (4bit)
		outb(LCD_DATA_POUT, inb(LCD_DATA_POUT)|0xF0);	// set pull-ups to on (4bit)
	#else
		outb(LCD_DATA_DDR, 0x00);						// set data I/O lines to input (8bit)
		outb(LCD_DATA_POUT, 0xFF);						// set pull-ups to on (8bit)
	#endif

//	lcd_set_io_read();
#else
	// enable external memory bus if not already enabled
	sbi(MCUCR, SRE);			// enable bus interface
#endif
}

#if LCD_NO_INPUT||0
void lcdBusyWait(void)
{
	delay_ms(20);
}
#else
void lcdBusyWait(void)
{
	// wait until LCD busy bit goes to zero
	// do a read from control register
#ifdef LCD_PORT_INTERFACE
	lcd_set_rs_control();
	lcd_set_io_read();
	lcd_set_rw_read();
	ASSERT_E;

	LCD_DELAY;								// wait
	while(inb(LCD_DATA_PIN) & 1<<LCD_BUSY)
	{

		lcd_reverse_pulse_e();
#ifdef LCD_DATA_4BIT						// do an extra clock for 4 bit reads
		lcd_reverse_pulse_e();
#endif
	}

	DEASSERT_E;


	//	leave data lines in input mode so they can be most easily used for other purposes
#else
	// memory bus read
	// sbi(MCUCR, SRW);			// enable RAM waitstate
	// wait until LCD busy bit goes to zero
	while( (*((volatile unsigned char *) (LCD_CTRL_ADDR))) & (1<<LCD_BUSY) )
		;
	// cbi(MCUCR, SRW);			// disable RAM waitstate
#endif
}
#endif

void lcdControlWrite(u08 data) 
{
	lcdBusyWait();							// wait until LCD not busy
// write the control byte to the display controller
#ifdef LCD_PORT_INTERFACE
	lcd_set_rs_control();
	lcd_io_write(data);

#else
	*((volatile unsigned char *) (LCD_CTRL_ADDR)) = data;
#endif
}

#if !LCD_NO_INPUT
// read the control byte from the display controller
u08 lcdControlRead(void)
{
	register u08 data;

	lcdBusyWait();				// wait until LCD not busy

#ifdef LCD_PORT_INTERFACE

	lcd_set_rs_control();
	data = lcd_io_read();

#else
	data = *((volatile unsigned char *) (LCD_CTRL_ADDR));
#endif
	return data;
}
#endif


// write a data byte to the display
void lcdDataWrite(u08 data) 
{
	lcdBusyWait();							// wait until LCD not busy
#ifdef LCD_PORT_INTERFACE
	lcd_set_rs_data();
	lcd_io_write(data);
#else
	*((volatile unsigned char *) (LCD_DATA_ADDR)) = data;
#endif
}

#if !LCD_NO_INPUT
u08 lcdDataRead(void)
{
// read a data byte from the display
	register u08 data;
	lcdBusyWait();				// wait until LCD not busy
#ifdef LCD_PORT_INTERFACE

	lcd_set_rs_data();
	data = lcd_io_read();

#else
	data = *((volatile unsigned char *) (LCD_DATA_ADDR));
#endif
	return data;
}
#endif



/*************************************************************/
/********************* PUBLIC FUNCTIONS **********************/
/*************************************************************/

void lcdInit()
{
	// sync pulse for scope
	lcd_set_rs_control();
	delay_ms(1);
	lcd_set_rs_data();
	delay_ms(1);
        lcd_set_rs_control();
	delay_ms(1);

	// initialize hardware
	lcdInitHW();

	delay_ms(40);
	lcdControlWrite(0x30);
	delay_ms(20);
	lcdControlWrite(0x30);
	delay_ms(20);
	lcdControlWrite(0x30);
	delay_ms(20);



	// LCD function set
	lcdControlWrite(LCD_FUNCTION_DEFAULT);

	// clear LCD
//	lcdControlWrite(1<<LCD_CLR);
//	delay_ms(60);	// wait 60ms

	// set entry mode
	lcdControlWrite(1<<LCD_ENTRY_MODE | 1<<LCD_ENTRY_INC);
	// set display to on
	lcdControlWrite(1<<LCD_ON_CTRL | 1<<LCD_ON_DISPLAY | 1<<LCD_ON_BLINK | LCD_ON_CURSOR );
	//lcdControlWrite(1<<LCD_ON_CTRL | 1<<LCD_ON_DISPLAY );
	// move cursor to home
	lcdControlWrite(1<<LCD_HOME);
	// set data address to 0
	lcdControlWrite(1<<LCD_DDRAM | 0x00);


	delay_ms(5);
	lcdDataWrite('A');
	lcdDataWrite('-');
	lcdDataWrite('B');

#if 0
	// load the first 8 custom characters
	lcdLoadCustomChar((u08*)LcdCustomChar,0,0);
	lcdLoadCustomChar((u08*)LcdCustomChar,1,1);
	lcdLoadCustomChar((u08*)LcdCustomChar,2,2);
	lcdLoadCustomChar((u08*)LcdCustomChar,3,3);
	lcdLoadCustomChar((u08*)LcdCustomChar,4,4);
	lcdLoadCustomChar((u08*)LcdCustomChar,5,5);
	lcdLoadCustomChar((u08*)LcdCustomChar,6,6);
	lcdLoadCustomChar((u08*)LcdCustomChar,7,7);

	// move cursor to home
	lcdControlWrite(1<<LCD_HOME);
	// set data address to 0 - if no read!
	lcdControlWrite(1<<LCD_DDRAM | 0x00);
#endif

}

void lcdHome(void)
{
	// move cursor to home
	lcdControlWrite(1<<LCD_HOME);
}

void lcdClear(void)
{
	// clear LCD
	lcdControlWrite(1<<LCD_CLR);
}

void lcdGotoXY(u08 x, u08 y)
{
	register u08 DDRAMAddr;

	// remap lines into proper order
	switch(y)
	{
	case 0: DDRAMAddr = LCD_LINE0_DDRAMADDR+x; break;
	case 1: DDRAMAddr = LCD_LINE1_DDRAMADDR+x; break;
	case 2: DDRAMAddr = LCD_LINE2_DDRAMADDR+x; break;
	case 3: DDRAMAddr = LCD_LINE3_DDRAMADDR+x; break;
	default: DDRAMAddr = LCD_LINE0_DDRAMADDR+x;
	}

	// set data address
	lcdControlWrite(1<<LCD_DDRAM | DDRAMAddr);
}

void lcdLoadCustomChar(u08* lcdCustomCharArray, u08 romCharNum, u08 lcdCharNum)
{
	register u08 i;

#if !LCD_NO_INPUT
	u08 saveDDRAMAddr;

	// backup the current cursor position
	saveDDRAMAddr = lcdControlRead() & 0x7F;
#endif

	// multiply the character index by 8
	lcdCharNum = (lcdCharNum<<3);	// each character occupies 8 bytes
	romCharNum = (romCharNum<<3);	// each character occupies 8 bytes

	// copy the 8 bytes into CG (character generator) RAM
	for(i=0; i<8; i++)
	{
		// set CG RAM address
		lcdControlWrite((1<<LCD_CGRAM) | (lcdCharNum+i));
		// write character data
		lcdDataWrite( pgm_read_byte(lcdCustomCharArray+romCharNum+i) );
	}

#if !LCD_NO_INPUT
	// restore the previous cursor position
	lcdControlWrite(1<<LCD_DDRAM | saveDDRAMAddr);
#endif
}

void lcdPrintData(char* data, u08 nBytes)
{
	register u08 i;

	// check to make sure we have a good pointer
	if (!data) return;

	// print data
	for(i=0; i<nBytes; i++)
	{
		lcdDataWrite(data[i]);
	}
}

void lcdProgressBar(u16 progress, u16 maxprogress, u08 length)
{
	u08 i;
	u32 pixelprogress;
	u08 c;

	// draw a progress bar displaying (progress / maxprogress)
	// starting from the current cursor position
	// with a total length of "length" characters
	// ***note, LCD chars 0-5 must be programmed as the bar characters
	// char 0 = empty ... char 5 = full

	// total pixel length of bargraph equals length*PROGRESSPIXELS_PER_CHAR;
	// pixel length of bar itself is
	pixelprogress = ((progress*(length*PROGRESSPIXELS_PER_CHAR))/maxprogress);
	
	// print exactly "length" characters
	for(i=0; i<length; i++)
	{
		// check if this is a full block, or partial or empty
		// (u16) cast is needed to avoid sign comparison warning
		if( ((i*(u16)PROGRESSPIXELS_PER_CHAR)+5) > pixelprogress )
		{
			// this is a partial or empty block
			if( ((i*(u16)PROGRESSPIXELS_PER_CHAR)) > pixelprogress )
			{
				// this is an empty block
				// use space character?
				c = 0;
			}
			else
			{
				// this is a partial block
				c = pixelprogress % PROGRESSPIXELS_PER_CHAR;
			}
		}
		else
		{
			// this is a full block
			c = 5;
		}
		
		// write character to display
		lcdDataWrite(c);
	}

}

void lcdPuts(char* data)
{
	while(*data)
	{
		lcdDataWrite(*data++);
	}
}

#endif
