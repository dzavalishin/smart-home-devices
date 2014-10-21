#define LCD_NO_INPUT 0
#define LCD_PORT_INTERFACE



#define LCD_CTRL_PORT PORTD
#define LCD_CTRL_DDR  DDRD

#define LCD_CTRL_E  PD7
#define LCD_CTRL_RS PD6
#define LCD_CTRL_RW PD5





#define LCD_DATA_DDR  DDRC
#define LCD_DATA_POUT PORTC
#define LCD_DATA_PIN  PINC



#define u08 uint8_t
#define u16 uint16_t
#define u32 uint32_t



#define LCD_LINE0_DDRAMADDR		0x00
#define LCD_LINE1_DDRAMADDR		0x40
#define LCD_LINE2_DDRAMADDR		0x14
#define LCD_LINE3_DDRAMADDR		0x54


#include "delay.h"
// 2us?
#define LCD_DELAY	delay_us(100)
