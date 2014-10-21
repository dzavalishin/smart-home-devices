#include "ui_menu.h"
#include "uart.h"
#include "util.h"
#include "adc.h"
#include "pwm.h"
#include "timer0.h"
#include "temperature.h"
#include "eeprom.h"
#include "ui_lcd.h"
#include "modbus.h"

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <stdlib.h>

#include "delay.h"


void init_devices(void);




int main()
{

    cli();
    //unsigned char csr = MCUCSR;
    MCUCSR = 0;

#if USE_WDOG
    wdt_disable();
#endif

    init_devices();
    flash_led2_once();

    lcd_clear();
    lcd_gotoxy( 0, 0 );
    lcd_puts("Load EEPROM:");
    lcd_gotoxy( 0, 1 );

    load_eeprom_settings();
//    flash_led2_once();

#if USE_WDOG
    wdt_enable(WDTO_2S);
#endif

    for (;;)
    {
        modbus_process_rx();
#if 1
        menu_run();
#else
        lcd_gotoxy( 0, 0 );

        int i;

        i = adc_value[6] >> 5;
        lcd_puts("vr1=");
        lcd_puti(i);
        i = adc_value[7] >> 5;
        lcd_puts(" vr2=");
        lcd_puti(i);
        lcd_puts("  ");
#endif

#if USE_WDOG
        wdt_reset();
#endif
    }


    while(1)
        ; // die here

    return 1;
}






void init_devices(void)
{
    // --------------------------------------------------------------
    // DDR and core CPU init
    // --------------------------------------------------------------

    // stop errant interrupts until set up
    cli();

    MCUCR = 0;
    TIMSK = 0;
    //GICR  = 0x00;

#if HALF_DUPLEX
    HALF_DUPLEX_DDR |= _BV(HALF_DUPLEX_PIN);
#endif

    LED1_DDR |= _BV(LED1);
    LED2_DDR |= _BV(LED2);


    // --------------------------------------------------------------
    // initialize subsystems
    // --------------------------------------------------------------

    lcd_init();
    uart_init();

    adc_init();

#if SERVANT_NPWM > 0
    timer1_init();      	// timers 0, 1
#endif

    timer0_init();
    sei(); 			// re-enable interrupts

    modbus_init();

    lcd_clear();
    lcd_gotoxy( 0, 0 );
    lcd_puts("Scan 1Wire:");
    lcd_gotoxy( 0, 1 );
    init_temperature();


    menu_init();

    // --------------------------------------------------------------
    // all peripherals are now initialized, start 'em
    // --------------------------------------------------------------

    adc_start();

#if SERVANT_NPWM > 0
    timer1_start();
#endif

}










