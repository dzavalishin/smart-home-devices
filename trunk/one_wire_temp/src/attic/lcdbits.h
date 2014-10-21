#define ASSERT_E sbi(LCD_CTRL_PORT, LCD_CTRL_E)
#define DEASSERT_E cbi(LCD_CTRL_PORT, LCD_CTRL_E)


#if !LCD_NO_INPUT
static void lcd_set_rw_read()
{
    sbi(LCD_CTRL_PORT, LCD_CTRL_RW);		// set R/W to "read"
}
#endif

static void lcd_set_rw_write()
{
#if !LCD_NO_INPUT
    cbi(LCD_CTRL_PORT, LCD_CTRL_RW);			// set R/W to "write"
#endif
}


static void lcd_set_rs_control()
{
    // set RS to "control"
    cbi(LCD_CTRL_PORT, LCD_CTRL_RS);
}

static void lcd_set_rs_data()
{
    // set RS to "data"
    sbi(LCD_CTRL_PORT, LCD_CTRL_RS);
}


static void lcd_set_io_read()
{
#ifdef LCD_DATA_4BIT
    outb(LCD_DATA_DDR, inb(LCD_DATA_DDR)&0x0F);		// set data I/O lines to input (4bit)
    outb(LCD_DATA_POUT, inb(LCD_DATA_POUT)|0xF0);	// set pull-ups to on (4bit)
#else
    outb(LCD_DATA_DDR, 0x00);			// set data I/O lines to input (8bit)
    outb(LCD_DATA_POUT, 0xFF);			// set pull-ups to on (8bit)
#endif
}


static void lcd_io_write(u08 data)
{
    lcd_set_rw_write();
#ifdef LCD_DATA_4BIT
    // 4 bit write
    sbi(LCD_CTRL_PORT, LCD_CTRL_E);	// set "E" line
    outb(LCD_DATA_DDR, inb(LCD_DATA_DDR)|0xF0);	// set data I/O lines to output (4bit)
    outb(LCD_DATA_POUT, (inb(LCD_DATA_POUT)&0x0F) | (data&0xF0) );	// output data, high 4 bits
    LCD_DELAY;								// wait
    LCD_DELAY;								// wait
    cbi(LCD_CTRL_PORT, LCD_CTRL_E);	// clear "E" line
    LCD_DELAY;								// wait
    LCD_DELAY;								// wait
    sbi(LCD_CTRL_PORT, LCD_CTRL_E);	// set "E" line
    outb(LCD_DATA_POUT, (inb(LCD_DATA_POUT)&0x0F) | (data<<4) );	// output data, low 4 bits
    LCD_DELAY;								// wait
    LCD_DELAY;								// wait
    cbi(LCD_CTRL_PORT, LCD_CTRL_E);	// clear "E" line
#else
    // 8 bit write
    sbi(LCD_CTRL_PORT, LCD_CTRL_E);	// set "E" line
    outb(LCD_DATA_DDR, 0xFF);				// set data I/O lines to output (8bit)
    outb(LCD_DATA_POUT, data);				// output data, 8bits
    LCD_DELAY;								// wait
    LCD_DELAY;								// wait
    cbi(LCD_CTRL_PORT, LCD_CTRL_E);	// clear "E" line
#endif

    delay_ms(5);

    //	leave data lines in input mode so they can be most easily used for other purposes
    lcd_set_io_read();

}




static u08 lcd_io_read()
{
    register u08 data;

    lcd_set_io_read();
    sbi(LCD_CTRL_PORT, LCD_CTRL_RW);		// set R/W to "read"

#ifdef LCD_DATA_4BIT
    // 4 bit read
    sbi(LCD_CTRL_PORT, LCD_CTRL_E);	// set "E" line
    LCD_DELAY;								// wait
    LCD_DELAY;								// wait
    data = inb(LCD_DATA_PIN)&0xF0;	// input data, high 4 bits
    cbi(LCD_CTRL_PORT, LCD_CTRL_E);	// clear "E" line
    LCD_DELAY;								// wait
    LCD_DELAY;								// wait
    sbi(LCD_CTRL_PORT, LCD_CTRL_E);	// set "E" line
    LCD_DELAY;								// wait
    LCD_DELAY;								// wait
    data |= inb(LCD_DATA_PIN)>>4;			// input data, low 4 bits
    cbi(LCD_CTRL_PORT, LCD_CTRL_E);	// clear "E" line
#else
    // 8 bit read
    sbi(LCD_CTRL_PORT, LCD_CTRL_E);	// set "E" line
    LCD_DELAY;								// wait
    LCD_DELAY;								// wait
    data = inb(LCD_DATA_PIN);			// input data, 8bits
    cbi(LCD_CTRL_PORT, LCD_CTRL_E);	// clear "E" line
#endif
    return data;
}



static void lcd_reverse_pulse_e()
{
    DEASSERT_E;

    LCD_DELAY;
    LCD_DELAY;

    ASSERT_E;

    LCD_DELAY;
    LCD_DELAY;
}











// custom LCD characters
unsigned char __attribute__ ((progmem)) LcdCustomChar[] =
{
    0x00, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00, // 0. 0/5 full progress block
    0x00, 0x1F, 0x10, 0x10, 0x10, 0x10, 0x1F, 0x00, // 1. 1/5 full progress block
    0x00, 0x1F, 0x18, 0x18, 0x18, 0x18, 0x1F, 0x00, // 2. 2/5 full progress block
    0x00, 0x1F, 0x1C, 0x1C, 0x1C, 0x1C, 0x1F, 0x00, // 3. 3/5 full progress block
    0x00, 0x1F, 0x1E, 0x1E, 0x1E, 0x1E, 0x1F, 0x00, // 4. 4/5 full progress block
    0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x00, // 5. 5/5 full progress block
    0x03, 0x07, 0x0F, 0x1F, 0x0F, 0x07, 0x03, 0x00, // 6. rewind arrow
    0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x00, // 7. stop block
    0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x1B, 0x00, // 8. pause bars
    0x18, 0x1C, 0x1E, 0x1F, 0x1E, 0x1C, 0x18, 0x00, // 9. fast-forward arrow
    0x00, 0x04, 0x04, 0x0E, 0x0E, 0x1F, 0x1F, 0x00, // 10. scroll up arrow
    0x00, 0x1F, 0x1F, 0x0E, 0x0E, 0x04, 0x04, 0x00, // 11. scroll down arrow
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 12. blank character
    0x00, 0x0E, 0x19, 0x15, 0x13, 0x0E, 0x00, 0x00,	// 13. animated play icon frame 0
    0x00, 0x0E, 0x15, 0x15, 0x15, 0x0E, 0x00, 0x00,	// 14. animated play icon frame 1
    0x00, 0x0E, 0x13, 0x15, 0x19, 0x0E, 0x00, 0x00,	// 15. animated play icon frame 2
    0x00, 0x0E, 0x11, 0x1F, 0x11, 0x0E, 0x00, 0x00,	// 16. animated play icon frame 3
};


